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
		rootNode.name = "Root Node";

		ShadowMapShaderParameters parameters;
		shadowMapShader = ShaderFactory::getInstance(&parameters);
	}

	Scene::~Scene() {
		for (int i = 0; i < animations.size(); i++) {
			delete animations[i];
		}

		for (int i = 0; i < materials.size(); i++) {
			delete materials[i];
		}

		for (int i = 0; i < textures.size(); i++) {
			delete textures[i];
		}

		for (std::vector<Renderable*>::iterator it = renderables.begin(); it != renderables.end(); it++) {
			delete *it;
		}

		for (int i = 0; i < lights.size(); i++) {
			delete lights[i];
		}

		for (int i = 0; i < cameras.size(); i++) {
			delete cameras[i];
		}

		for (int i = 0; i < meshes.size(); i++) {
			delete meshes[i];
		}
	}

	void Scene::update() {
		// Matrix modelMatrix = locationM * orientationM * scaleM;

		for (int i = 0; i < animations.size(); i++) {
			animations[i]->update();
		}

		rootNode.update();
	}

	int Scene::buildShadowMaps() {
		int triangles = 0;
		for (std::vector<MBlight*>::iterator it = lights.begin(); it != lights.end(); it++) {
			MBlight* light = *it;

			light->updateLightCamera();
			{
				if (light->prepareShadowMap()) {
					triangles += draw(light->getCamera(), shadowMapShader);
					light->finishShadowMap();
				}
			}
		}
		return triangles;
	}

	int Scene::sortByMeshAndDraw( Camera* cam, MBshader* shader, std::vector<Renderable*>& renderables ) {
		int triangles = 0;

		// Order the renderables by mesh:
		std::map<MBmesh*, std::vector<Renderable*> > orderedByMesh;
		for (std::vector<Renderable*>::iterator it = renderables.begin(); it != renderables.end(); it++) {
			orderedByMesh[(*it)->mesh].push_back( *it );
		}

		shader->useProgram();

		shader->sendInteger("nLights", (int)lights.size());

		for (int i = 0; i < lights.size(); i++) {
			lights[i]->setShadowUniforms( shader, i );
		}

		Vector viewPosition = cam->getLocation();
		shader->sendMatrix("viewPosition", viewPosition);

		Matrix ViewProjection = cam->getProjectionMatrix() * cam->getViewMatrix();

		// For each mesh:
		for (std::map<MBmesh*, std::vector<Renderable*> >::iterator it = orderedByMesh.begin(); it != orderedByMesh.end(); it++) {
			MBmesh* mesh = it->first;
			mesh->bindForDrawing(shader);

			// For each node/material to be rendered for each mesh:
			for (std::vector<Renderable*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
				Renderable* renderable = *it2;

				for (int j = 0; j < lights.size(); j++) {
					lights[j]->sendToShader(shader, *(renderable->node->getModelMatrix()), j);
				}

				Matrix modelViewProjectionMatrix = ViewProjection * *renderable->node->getModelMatrix();
				Matrix modelMatrix = *renderable->node->getModelMatrix();
				Matrix normalMatrix = modelMatrix.subMatrix(3, 3).inverse().transpose();

				shader->sendMatrix("modelViewProjectionMatrix", modelViewProjectionMatrix);
				shader->sendMatrix("normalMatrix", normalMatrix);
				shader->sendMatrix("modelMatrix", modelMatrix);

				renderable->material->sendToShader(shader);

				triangles += mesh->draw(shader);
			}
			mesh->unbindFromDrawing();
		}

		shader->stopProgram();

		return triangles;
	}

	int Scene::draw(Camera *cam, MBshader *shader) {

		if( dynamic_cast<const DynamicShader*>(shader) ) {
			std::map<MBshader*, std::vector<Renderable*> > orderedShaders;

			// "Static" shader properties that dictate dynamic shader construction:
			// Materials -> all texture maps, specular amounts
			// Lights -> quantity.    Dynamic: color, position, MVP matrix
			// Mesh/Camera -> All dynamic: MVP, MV, V, and N matrices

			shader->sendInteger("nLights", (int)lights.size());	// needed for all variants of the shader, will be compiled.

			//		std::cerr << "Beginning ordering" << std::endl;
			for (std::vector<Renderable*>::iterator it = renderables.begin(); it != renderables.end(); it++) {
				(*it)->material->sendToShader(shader);	// Note: there is a lot of possible variability within this call
				orderedShaders[((DynamicShader*)shader)->getActualShader()].push_back(*it);	// Add a renderable to to this shader
			}

			int triangles = 0;
			for (std::map<MBshader*, std::vector<Renderable*> >::iterator it = orderedShaders.begin(); it != orderedShaders.end(); it++) {
				MBshader* theShader = it->first;
				std::vector<Renderable*>& theRenderablesForThisShader = it->second;

				triangles += sortByMeshAndDraw(cam, theShader, theRenderablesForThisShader);
			}
			return triangles;
		}

		// Here the shader is static, so only use the program once for the entire draw.
		return sortByMeshAndDraw(cam, shader, renderables);
	}

//	bool cmp(const Renderable* a, const Renderable* b) {
//		if(a->mesh > b->mesh) return false;
//		if(a->mesh < b->mesh) return true;
//		if(a->node > b->node) return false;
//		if(a->node < b->node) return true;
//		if(a->material > b->material) return false;
//		if(a->material < b->material) return true;
//		return false;
//	}

	Renderable* Scene::addRenderable(Math::Node* node, MBmesh* mesh, MBmaterial* material) {
		for (std::vector<Renderable*>::iterator it = renderables.begin(); it != renderables.end(); it++) {	// No need to make a copy
			if ((*it)->mesh == mesh &&
//				(*it)->material == material &&	// Actually, renderings would be ambiguous if they only differed by a material...
				(*it)->node == node) {
				return *it;
			}
		}
		Renderable* newRenderable = new Renderable;
		newRenderable->node = node;	// Should this exist in the tree of the root node?
		newRenderable->material = addMaterial(material);
		newRenderable->mesh = addMesh(mesh);
		renderables.push_back(newRenderable);
		return newRenderable;
	}

	std::vector<Renderable*> Scene::getRenderablesFromNode(Math::Node* node) {
		std::vector<Renderable*> result;
		for (std::vector<Renderable*>::iterator it = renderables.begin(); it != renderables.end(); it++) {
			if ((*it)->node == node) {
				result.push_back(*it);
			}
		}
		return result;
	}
	std::vector<Renderable*> Scene::getRenderablesFromMesh(MBmesh* mesh) {
		std::vector<Renderable*> result;
		for (std::vector<Renderable*>::iterator it = renderables.begin(); it != renderables.end(); it++) {
			if ((*it)->mesh	== mesh) {
				result.push_back(*it);
			}
		}
		return result;
	}
	std::vector<Renderable*> Scene::getRenderablesFromMaterial(MBmaterial* material) {
		std::vector<Renderable*> result;
		for (std::vector<Renderable*>::iterator it = renderables.begin(); it != renderables.end(); it++) {
			if ((*it)->material == material) {
				result.push_back(*it);
			}
		}
		return result;
	}

	MBmaterial* Scene::addMaterial(MBmaterial* material) {
		for (std::vector<MBmaterial*>::iterator it = materials.begin(); it != materials.end(); it++) {
			if (*it == material) {
				return material;
			}
		}
		materials.push_back(material);
		return material;
	}

	MBmesh* Scene::addMesh(MBmesh* mesh) {
		for (std::vector<MBmesh*>::iterator it = meshes.begin(); it != meshes.end(); it++) {
			if (*it == mesh) {
				return mesh;
			}
		}
		meshes.push_back(mesh);
		return mesh;
	}

	Texture* Scene::addTexture(Texture* texture) {
		for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); it++) {
			if (*it == texture) {
				return texture;
			}
		}
		textures.push_back(texture);
		return texture;
	}

	std::vector<MBmaterial*>& Scene::getMaterials() {
		return materials;
	}
	std::vector<Renderable*>& Scene::getRenderables() {
		return renderables;
	}
	std::vector<MBmesh*>& Scene::getMeshes() {
		return meshes;
	}
	std::vector<Camera*>& Scene::getCameras() {
		return cameras;
	}
	std::vector<MBlight*>& Scene::getLights() {
		return lights;
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
		return Mogi::getResourceDirectory();
#endif
	}
	
#ifdef _cplusplus
}
#endif
