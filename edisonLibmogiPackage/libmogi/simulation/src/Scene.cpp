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

#include <sstream>

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

Scene::Scene(const Scene &param) {
	initialize();
}

Scene &Scene::operator=(const Scene &param) {
	if (this != &param)  // only run if it is not the same object
			{
	}

	return *this;
}

void Scene::initialize() {
	totalTriangles = 0;
	rootNode.name = "Root Node";

// locationM.makeI(4);
// scaleM.makeI(4);
// orientationM.makeI(4);
#ifdef BUILD_FOR_IOS
	std::string vertexSource = "shaders/ios";
	std::string fragmentSource = "shaders/ios";
#else
#ifdef RESOURCES_DIRECTORY
	std::string vertexSource = RESOURCES_DIRECTORY;
	std::string fragmentSource = RESOURCES_DIRECTORY;
	vertexSource.append("/shaders");
	fragmentSource.append("/shaders");
#else
	std::string vertexSource = "Shaders";
	std::string fragmentSource = "Shaders";
#endif
#endif
	std::string sceneVertexSource = vertexSource;
	sceneVertexSource.append("/simple.vsh");
	std::string sceneFragmentSource = fragmentSource;
	sceneFragmentSource.append("/simple.fsh");

	//std::cout << "Initializing scene shader " << sceneVertexSource.c_str() << " ...";
	shadowMapShader.initialize(sceneVertexSource.c_str(),
			sceneFragmentSource.c_str());
	//std::cout << "Done." << std::endl;

	std::string geometryVertexSource = vertexSource;
	geometryVertexSource.append("/geometry.vsh");
	std::string geometryFragmentSource = fragmentSource;
	geometryFragmentSource.append("/geometry.fsh");
	//std::cout << "Initializing geometry shader...";
	//shadowMapShader.initialize(geometryVertexSource.c_str(), geometryFragmentSource.c_str());	// TODO:  Why am I re-initializing this?  It's been too long
	gShader.initialize(geometryVertexSource.c_str(), geometryFragmentSource.c_str()); // TODO: from the above, this should be a part of the gShader, currently unused
	//std::cout << "Done." << std::endl;
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

	for (int i = 0; i < textures.size(); i++) {
		delete textures[i];
	}
	textures.clear();

	for (int i = 0; i < meshes.size(); i++) {
		delete meshes[i];
	}
	meshes.clear();

	for (int i = 0; i < lights.size(); i++) {
		delete lights[i];
	}
	lights.clear();

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

void populateNode(Node **theNode, aiNode *node, Node *nodeParent,
		int meshIDOffset, std::vector<NodeMatrixAndMeshID*>* meshesToDraw) {
	(*theNode)->name = node->mName.C_Str();
	// std::cout << "\t\t";
	// for (int i = 0; i < prettyPrintTracker; i++) {
	//	std::cout << "|--";
	//}
	// std::cout << "Node name: " << name << ", Mesh Ids: ";
	// theNode->parent = nodeParent;
	//(*theNode)->moveToUnderParent(nodeParent);
	*theNode = nodeParent->adoptChild(theNode);

	// nodeParent->adoptChild(&theNode);

	for (int i = 0; i < node->mNumMeshes; i++) {
		NodeMatrixAndMeshID *nodeMatrixAndID = new NodeMatrixAndMeshID;
		nodeMatrixAndID->ID = node->mMeshes[i] + meshIDOffset;
//		nodeMatrixAndID->modelMatrix = (*theNode)->getModelMatrix(); // &outputMatrix;	// NodeMatrixAndMeshID refactoring
		nodeMatrixAndID->parentNode = *theNode;	// NodeMatrixAndMeshID refactoring
		// std::cout << nodeMatrixAndID->ID << " ";
		// meshIDs.push_back(nodeMatrixAndID);
//		(*theNode)->pushMatrixAndMeshID(nodeMatrixAndID);
		meshesToDraw->push_back(nodeMatrixAndID);
	}

	// std::cout << std::endl;

	Node *child;
	for (int i = 0; i < node->mNumChildren; i++) {
		// prettyPrintTracker++;
		child = new Node;
		// child->set(node->mChildren[i], this, meshIDOffset);
		populateNode(&child, node->mChildren[i], *theNode, meshIDOffset,
				meshesToDraw);
		//(*theNode)->pushChild(child);
		//(*theNode)->adoptChild(&child);
		// prettyPrintTracker--;
	}

	Matrix transformationMatrix(4, 4);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			transformationMatrix(i, j) = node->mTransformation[i][j]; //*(node->mTransformation[i*4 + j]);
		}
	}
	// resetModelMatrix();
	// transformationMatrix.name(theNode->name.c_str());
	// transformationMatrix.print_stats();
	(*theNode)->setModelMatrix(transformationMatrix);
}

int Scene::set(const aiScene *scene, const char *fileName, bool createNode) {
	std::string fileLocation(objectLocation);
	fileLocation.append("/");
	fileLocation.append(fileName);

	// First process the nodes:
	// std::cout << "\tScanning Nodes, current Mesh size: " << meshes.size() << ":
	// " << std::endl;

	// rootNode.set(scene->mRootNode, NULL, meshes.size());
	Node *currentNode = NULL;
	if (createNode) {
		currentNode = rootNode.addNode("dummy");
		// currentNode->set( scene->mRootNode, &rootNode, meshes.size());
		populateNode(&currentNode, scene->mRootNode, &rootNode, meshes.size(), &meshesToDraw);
	}
	// rootNode.findChildByName("dummy")->set( scene->mRootNode, &rootNode,
	// meshes.size());

	// Now process the animations:
	// std::cout << "\tNumber of Animations : " << scene->mNumAnimations <<
	// std::endl;
	Animation *animation;
	for (int i = 0; i < scene->mNumAnimations; i++) {
		animation = new Animation;
		animation->set(scene->mAnimations[i]);
		// Now that we are here, process the channels (since nodes were solved
		// ABOVE)
		animation->matchChannelsToNodes(&rootNode);
		animations.push_back(animation);
	}

	// Set the materials:
	// std::cout << "\tNumber of Textures   : " << scene->mNumTextures <<
	// std::endl;
	// std::cout << "\tNumber of Materials  : " << scene->mNumMaterials <<
	// std::endl;
	MBmaterial *material;
	int materialIDOffset = materials.size();
	for (int i = 0; i < scene->mNumMaterials; i++) {
		material = new MBmaterial;
		material->set(scene->mMaterials[i], objectLocation);
		materials.push_back(material);
	}

	// Set the textures:
	Texture *texture;
	for (int i = 0; i < scene->mNumTextures; i++) {
		texture = new Texture;
		texture->set(scene->mTextures[i]);
		textures.push_back(texture);
	}

	// Set the meshes:
	int triangles = 0;
	/// std::cout << "\tNumber of Meshes     : " << scene->mNumMeshes <<
	/// std::endl;
	MBmesh *mesh;
	for (int i = 0; i < scene->mNumMeshes; i++) {
		mesh = new MBmesh();
		triangles += mesh->set(scene->mMeshes[i], scene->mMaterials,
				objectLocation, materialIDOffset);
		mesh->matchBonesToNodes(&rootNode);
		mesh->fileName = fileLocation;
		meshes.push_back(mesh);

		nodeToMeshMap[currentNode].push_back(mesh);
	}

	// Set the lights:
	// std::cout << "\tNumber of Lights     : " << scene->mNumLights << std::endl;
	MBlight *light;
	for (int i = 0; i < scene->mNumLights; i++) {
		light = MBlight::create(scene->mLights[i]);
		light->findNode(&rootNode);
		lights.push_back(light);
	}

	// Set the cameras:
	// std::cout << "\tNumber of Cameras    : " << scene->mNumCameras <<
	// std::endl;
	Camera *camera;
	for (int i = 0; i < scene->mNumCameras; i++) {
		camera = new Camera;
		camera->set(scene->mCameras[i]);
		// cameras.push_back(camera);
	}
	return triangles;
}

Node *Scene::loadObject(const char *filename, const char *location,
		bool createNode) {
	if (filename == NULL || location == NULL) {
		return NULL;
	}
#ifdef BUILD_FOR_IOS
	std::stringstream objectLocationss("");
	objectLocationss << Mogi::getResourceDirectory() << "/" << location;
	objectLocation = objectLocationss.str();
#else
	objectLocation = std::string(location);
#endif

	Assimp::Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 3);

	std::string fileLocation(objectLocation);
	fileLocation.append("/");
	fileLocation.append(filename);

	const aiScene *scene = importer.ReadFile(fileLocation,
			aiProcess_GenSmoothNormals | aiProcess_Triangulate
					| aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
	if (scene == NULL) {
		std::cout << "The file wasn't successfuly opened: " << fileLocation
				<< std::endl;
		return NULL;
	}

	if (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout
				<< "The file was opened but is incomplete or doesn't contain a "
						"root node: " << fileLocation << std::endl;
		return NULL;
	}

	// std::cout << "Loading: " << fileLocation << "" << std::endl;

	totalTriangles += set(scene, filename, createNode);

	// std::cout << "File: " << fileLocation << " loaded!" << std::endl;

	// set() appends the node to the end, so return the last node:
	return rootNode.child(rootNode.numberOfChildren() - 1);
}

void Scene::update() {
	// Matrix modelMatrix = locationM * orientationM * scaleM;

	for (int i = 0; i < materials.size(); i++) {
		materials[i]->colorMapUserEnable = colorMapUserEnable;
		materials[i]->normalMapUserEnable = normalMapUserEnable;
		materials[i]->heightMapUserEnable = heightMapUserEnable;
		materials[i]->specularityMapUserEnable = specularityMapUserEnable;
	}

	for (int i = 0; i < animations.size(); i++) {
		animations[i]->update();
	}

	//		rootNode.update( modelMatrix );
	// rootNode.setLocation(locationM(0,3), locationM(1,3), locationM(2,3));
	// rootNode.setScale(scaleM(0,0), scaleM(1,1), scaleM(2,2));
	// rootNode.setOrientation(orientationM.quaternion());
	rootNode.update();
}

void Scene::setLocation(const Vector &loc) {
	if ((loc.numRows() == 3) && (loc.numColumns() == 1)) {
		setLocation(loc.valueAsConst(0, 0), loc.valueAsConst(1, 0),
				loc.valueAsConst(2, 0));
	}
}

void Scene::setLocation(double x, double y, double z) {
	// locationM(0,3) = x;
	// locationM(1,3) = y;
	// locationM(2,3) = z;
	rootNode.setLocation(x, y, z);
}

void Scene::setScale(const Vector &loc) {
	if ((loc.numRows() == 3) && (loc.numColumns() == 1)) {
		setScale(loc.valueAsConst(0, 0), loc.valueAsConst(1, 0),
				loc.valueAsConst(2, 0));
	}
}

void Scene::setScale(double x, double y, double z) {
	// scaleM(0,0) = x;
	// scaleM(1,1) = y;
	// scaleM(2,2) = z;
	rootNode.setScale(x, y, z);
}

void Scene::setScale(double s) {
	setScale(s, s, s);
}

void Scene::setOrientation(Quaternion &quat) {
	// orientationM = quat.makeRotationMatrix4();
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
			shadowMapShader.useProgram();
			{
				// Texture::resetTextureLocation();
				if (lights[i]->prepareShadowMap()) {
					triangles += draw(lights[i]->getCamera(), &shadowMapShader);
					lights[i]->finishShadowMap();
				}
			}
			shadowMapShader.stopProgram();
		}
	}
	return triangles;
}

int Scene::draw(Camera *cam, MBshader *shader) {
	int triangles = 0;
//	std::vector<NodeMatrixAndMeshID *> meshIds = meshesToDraw;// rootNode.getMeshIDs();

	shader->sendInteger("nLights", lights.size());
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
		triangles += meshes[meshesToDraw[i]->ID]->drawWithMaterials(shader,
				materials);
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

	std::vector<MBmaterial*> Scene::getMaterials() {
		return materials;
	}

int Scene::addMesh(std::string filename, std::string directory) {
	std::string fullPath = directory;
	fullPath.append("/");
	fullPath.append(filename);
	for (int i = 0; i < meshes.size(); i++) {
		if (meshes[i]->fileName.compare(fullPath) == 0) {
			return i;  // do not need to load a new mesh, it's already loaded
		}
	}

	loadObject(filename.c_str(), directory.c_str(), false);  // load a new mesh
	return meshes.size() - 1;  // return the mesh index
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
