/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *   Proprietary and confidential.                                            *
 *                                                                            *
 *   Unauthorized copying of this file via any medium is strictly prohibited  *
 *   without the explicit permission of Mogi, LLC.                            *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   http://www.binpress.com/license/view/l/0088eb4b29b2fcff36e42134b0949f93  *
 *                                                                            *
 *****************************************************************************/

#include "scene.h"
#include "mogi.h"
#include "dynamicShader.h"


#include <sstream>
#include <algorithm>
#include <typeinfo> // Needed for typeid

using namespace std;

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Math;
using namespace Simulation;

Scene::Scene() {
	initialize();
}

Scene::~Scene() {
	clearVectors();
}

void Scene::initialize() {
	totalTriangles = 0;
	rootNode.name = "Root Node";

	ShadowMapShaderParameters parameters;
	shadowMapShader = ShaderFactory::getInstance(&parameters);

	GeometryShaderParameters parametersG;
	gShader = ShaderFactory::getInstance(&parametersG);
}

char *readFileBytes(const char *name, long *len) {
	char *ret;
	ifstream fl(name);
	fl.seekg(0, ios::end);
	// long
	*len = fl.tellg();
	if (*len > 0) {
		ret = new char[*len];
		fl.seekg(0, ios::beg);
		fl.read(ret, *len);
		fl.close();
	} else {
		ret = NULL;
		*len = 0;
	}

	return ret;
}

void Scene::clearVectors() {
	for (int i = 0; i < animations.size(); i++) {
		delete animations[i];
	}
	animations.clear();

	for (int i = 0; i < materials.size(); i++) {
		delete materials[i];
	}
	materials.clear();

	for (int i = 0; i < textures.size(); i++) {
		delete textures[i];
	}
	textures.clear();

	for (std::vector<NodeMatrixAndMeshID*>::iterator it = meshesToDraw.begin(); it != meshesToDraw.end(); it++) {
		delete *it;
	}

	for (int i = 0; i < lights.size(); i++) {
		delete lights[i];
	}
	lights.clear();

	for (int i = 0; i < cameras.size(); i++) {
		delete cameras[i];
	}
	cameras.clear();

	for (int i = 0; i < meshes.size(); i++) {
		delete meshes[i];
	}
	meshes.clear();


//	for (int i = 0; i < meshesToDraw.size(); i++) {
//		delete meshesToDraw[i];
//	}
//	meshesToDraw.clear();
}

bool cmp(const NodeMatrixAndMeshID* a, const NodeMatrixAndMeshID* b) {
	return a->ID < b->ID;
}

void Scene::attachMeshToNode(Math::Node* node, int meshId) {
	for (std::vector<NodeMatrixAndMeshID*>::iterator it = meshesToDraw.begin();
			it != meshesToDraw.end(); it++) {
		if (node == (*it)->parentNode && meshId == (*it)->ID) {
			return;
		}
	}
	NodeMatrixAndMeshID* newAssociation = new NodeMatrixAndMeshID;
	newAssociation->parentNode = node;
	newAssociation->ID = meshId;
	meshesToDraw.push_back(newAssociation);
	std::sort(meshesToDraw.begin(), meshesToDraw.end(), cmp);
}

void Scene::update() {
	// Matrix modelMatrix = locationM * orientationM * scaleM;

	for (int i = 0; i < animations.size(); i++) {
		animations[i]->update();
	}

	rootNode.update();
}

void Scene::setLocation(const Vector &loc) {
	if ((loc.numRows() == 3) && (loc.numColumns() == 1)) {
		setLocation(loc.valueAsConst(0, 0), loc.valueAsConst(1, 0),
				loc.valueAsConst(2, 0));
	}
}

void Scene::setLocation(double x, double y, double z) {
	rootNode.setLocation(x, y, z);
}

void Scene::setScale(const Vector &loc) {
	if ((loc.numRows() == 3) && (loc.numColumns() == 1)) {
		setScale(loc.valueAsConst(0, 0), loc.valueAsConst(1, 0),
				loc.valueAsConst(2, 0));
	}
}

void Scene::setScale(double x, double y, double z) {
	rootNode.setScale(x, y, z);
}

void Scene::setScale(double s) {
	setScale(s, s, s);
}

void Scene::setOrientation(Quaternion &quat) {
	rootNode.setOrientation(quat);
}

void Scene::setOrientation(double angle, Vector &axis) {
	Quaternion temp;
	temp.makeFromAngleAndAxis(angle, axis);
	setOrientation(temp);
}

Node *Scene::findNodeByName(std::string nodeName) {
	if (nodeName == rootNode.name) {
		return &rootNode;
	}

	return rootNode.findChildByName(nodeName);
}

int Scene::buildShadowMaps() {
	int triangles = 0;
	for (int i = 0; i < lights.size(); i++) {
		// lights[i]->setEnabled(useShadows);
		lights[i]->updateLightCamera();
		{
			shadowMapShader->useProgram();
			{
				// Texture::resetTextureLocation();
				if (lights[i]->prepareShadowMap()) {
					triangles += draw(lights[i]->getCamera(), shadowMapShader);
					lights[i]->finishShadowMap();
				}
			}
			shadowMapShader->stopProgram();
		}
	}
	return triangles;
}

int Scene::draw(Camera *cam, MBshader *shader) {
	int triangles = 0;

	if( dynamic_cast<const DynamicShader*>(shader) ){//typeid(*shader) == typeid(DynamicShader)) {
		std::map<MBshader*, std::vector<NodeMatrixAndMeshID*> > orderedShaders;

		// "Static" shader properties that dictate dynamic shader construction:
		// Materials -> all texture maps, specular amounts
		// Lights -> quantity.    Dynamic: color, position, MVP matrix
		// Mesh/Camera -> All dynamic: MVP, MV, V, and N matrices

		// We only want to draw given meshes with lights.  Meshes may have materials.

		shader->sendInteger("nLights", (int)lights.size());	// needed for all variants of the shader, will be compiled.

//		std::cerr << "Beginning ordering" << std::endl;
		for (std::vector<NodeMatrixAndMeshID*>::iterator it = meshesToDraw.begin(); it != meshesToDraw.end(); it++) {
			int materialIndex = meshes[(*it)->ID]->getMaterialIndex();
//			std::cout << "materialIndex = " << materialIndex << std::endl;
			if (materialIndex > 0) {
				materials[ materialIndex ]->sendToShader(shader);	// should set all texture maps, etc.
			} else {
				// no color maps, etc.?
			}
			orderedShaders[((DynamicShader*)shader)->getActualShader()].push_back(*it);
		}

//		std::cout << "Resulting order:" << std::endl;
//		for (std::map<MBshader*, std::vector<NodeMatrixAndMeshID*> >::iterator it = orderedShaders.begin(); it != orderedShaders.end(); it++) {
//			std::cout << "Shader: " << it->first->getName() << ":";
//			for (std::vector<NodeMatrixAndMeshID*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
//				std::cout << " " << (*it2)->ID;
//			}
//			std::cout << std::endl;
//		}

		for (std::map<MBshader*, std::vector<NodeMatrixAndMeshID*> >::iterator it = orderedShaders.begin(); it != orderedShaders.end(); it++) {
			MBshader* theShader = it->first;
			std::vector<NodeMatrixAndMeshID*>& theMeshesToDraw = it->second;
			theShader->useProgram();

			for (int i = 0; i < lights.size(); i++) {
				lights[i]->setShadowUniforms( theShader, i );
			}

			theShader->sendMatrix("viewMatrix", cam->getViewMatrix());	// nmeeds to be send to each shader
			for (int i = 0; i < theMeshesToDraw.size(); i++) {
				for (int j = 0; j < lights.size(); j++) {
					lights[j]->sendToShader(theShader, *(theMeshesToDraw[i]->parentNode->getModelMatrix()), j);
				}

				Matrix myModelViewMatrix = cam->getViewMatrix() * *theMeshesToDraw[i]->parentNode->getModelMatrix();//modelMatrix;
				Matrix myModelViewProjectionMatrix = cam->getProjectionMatrix() * myModelViewMatrix;
				Matrix myNormalMatrix = myModelViewMatrix.subMatrix(3, 3);

				//myNormalMatrix = GLKMatrix3InvertAndTranspose(myNormalMatrix, NULL);
				myNormalMatrix = myNormalMatrix.inverse().transpose();

				theShader->sendMatrix("modelViewProjectionMatrix", myModelViewProjectionMatrix);
				theShader->sendMatrix("normalMatrix", myNormalMatrix);
				theShader->sendMatrix("modelViewMatrix", myModelViewMatrix);

				// meshes[meshIds[i]->ID]->sendMatricesToShader(shader, cam,
				// *meshIds[i]->modelMatrix);
				triangles += meshes[theMeshesToDraw[i]->ID]->drawWithMaterials(theShader, materials);
			}

			theShader->stopProgram();
		}
	} else {

		// Here the shader is static, so only use the program once for the entire draw.

		shader->useProgram();

		for (int i = 0; i < lights.size(); i++) {
			lights[i]->setShadowUniforms( shader, i );
		}

		shader->sendInteger("nLights", (int)lights.size());
		shader->sendMatrix("viewMatrix", cam->getViewMatrix());

		for (int i = 0; i < meshesToDraw.size(); i++) {
			for (int j = 0; j < lights.size(); j++) {
				lights[j]->sendToShader(shader, *(meshesToDraw[i]->parentNode->getModelMatrix()), j);
			}

			Matrix myModelViewMatrix = cam->getViewMatrix() * *meshesToDraw[i]->parentNode->getModelMatrix();//modelMatrix;
			Matrix myModelViewProjectionMatrix = cam->getProjectionMatrix() * myModelViewMatrix;
			Matrix myNormalMatrix = myModelViewMatrix.subMatrix(3, 3);

			//myNormalMatrix = GLKMatrix3InvertAndTranspose(myNormalMatrix, NULL);
			myNormalMatrix = myNormalMatrix.inverse().transpose();

			shader->sendMatrix("modelViewProjectionMatrix", myModelViewProjectionMatrix);
			shader->sendMatrix("normalMatrix", myNormalMatrix);
			shader->sendMatrix("modelViewMatrix", myModelViewMatrix);

			// meshes[meshIds[i]->ID]->sendMatricesToShader(shader, cam,
			// *meshIds[i]->modelMatrix);
			triangles += meshes[meshesToDraw[i]->ID]->drawWithMaterials(shader, materials);
		}
		
		shader->stopProgram();
	}
	return triangles;
}

MBmesh* Scene::getMesh(std::string meshName) {
	for (int i = 0; i < meshes.size(); i++) {
		if (meshes[i]->name.compare( meshName) == 0) {
			return meshes[i];
		}
	}
	return NULL;
}

	std::vector<MBmaterial*>& Scene::getMaterials() {
		return materials;
	}
	std::vector<NodeMatrixAndMeshID*>& Scene::getMeshestoDraw() {
		return meshesToDraw;
	}
	std::vector<MBmesh*>& Scene::getMeshes() {
		return meshes;
	}
	std::vector<Animation*>& Scene::getAnimations() {
		return animations;
	}
	std::vector<Texture*>& Scene::getTextures() {
		return textures;
	}

///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
// TTF_Font *font2 = NULL;
void drawString(const char *str, int x, int y, float color[4], void *font) {
	/*
	 glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	 glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	 glDisable(GL_TEXTURE_2D);

	 glColor4fv((GLfloat*) color);          // set text color
	 glRasterPos2i(x, y);        // place text position
	 //glTranslatef(x, y+8, 0);
	 // loop all characters in the string
	 while(*str)
	 {
	 glutBitmapCharacter(font, *str);

	 //glutStrokeCharacter(font, *str);
	 ++str;
	 }

	 glEnable(GL_TEXTURE_2D);
	 glEnable(GL_LIGHTING);
	 glPopAttrib();
	 */
	/*

	 if (font2 == NULL) {
	 font2 = TTF_OpenFont( "lazy.ttf", 1 );
	 }

	 if (font2 == NULL) {
	 std::cout << "SDL font failed!" <<std::endl;
	 } else {
	 *//*
	 GLuint stringTex = TextToTexture( font2, 255, 255, 255, "Hello!", 14 );
	 // this is where the magic happens

	 glEnable(GL_TEXTURE_2D);
	 glBindTexture(GL_TEXTURE_2D, stringTex );
	 glColor3f(1.0f, 1.0f, 1.0f);
	 glBegin(GL_QUADS);
	 glTexCoord2f( 0.0f, 1.0f ); glVertex2f( -1.0f, -1.0f );
	 glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  1.0f, -1.0f );
	 glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  1.0f,  1.0f );
	 glTexCoord2f( 0.0f, 0.0f ); glVertex2f( -1.0f,  1.0f );
	 glEnd();
	 glFinish();
	 glBindTexture(GL_TEXTURE_2D, 0 );
	 glDeleteTextures(1, &stringTex);
	 }
	 glDisable(GL_TEXTURE_2D);

	 SDL_Color col;
	 col.r = 200;
	 col.b = 0;
	 col.g = .5;
	 SDL_Rect rect;
	 rect.x = x+610;
	 rect.y = y-350;
	 rect.w = 100;
	 rect.h = 50;

	 SDL_GL_RenderText("Hello!", font2, col, &rect);
	 }
	 */
}

///////////////////////////////////////////////////////////////////////////////
// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
void drawString3D(const char *str, float pos[3], float color[4], void *font) {
	/*
	 glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	 glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	 glDisable(GL_TEXTURE_2D);

	 glColor4fv(color);          // set text color
	 glRasterPos3fv(pos);        // place text position

	 // loop all characters in the string
	 while(*str)
	 {
	 glutBitmapCharacter(font, *str);
	 ++str;
	 }

	 glEnable(GL_TEXTURE_2D);
	 glEnable(GL_LIGHTING);
	 glPopAttrib();
	 */
}

std::string Scene::getResourceDirectory() {
#ifdef RESOURCES_DIRECTORY
	return RESOURCES_DIRECTORY;
#else
	return "";
#endif
}

#ifdef _cplusplus
}
#endif
