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

#ifndef MOGI_SCENE_H
#define MOGI_SCENE_H

#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <string>
#include <stdint.h>


#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#include <string.h>
#endif

#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include "mogi/math/mmath.h"

#include "animation.h"
#include "bone.h"
#include "light.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"

#define MM_PER_METER (1000.0)

namespace Mogi {

	/**
	 * @namespace Mogi::Simulation
	 * \brief Simulation handling and building using OpenGL.
	 */
	namespace Simulation {

		/*!
		 \struct Renderable
		 \brief Represents an association between a mesh, material, and node to be drawn.
		 */
		struct Renderable {
			/*! Mesh Identifier.
			 */
			MBmesh* mesh;

			MBmaterial* material;

			/*! Transformation matrix specific to the mesh, with respect to the node.
			 */
			Math::Node* node;
		};

		/** @file */
		class Scene {
		private:
			MBshader* shadowMapShader;

			std::vector<Animation*> animations;
			std::vector<MBmaterial*> materials; // This could be loaded, but may not be important.
			// All mesh storage
			std::vector<MBmesh*> meshes;
			std::vector<Texture*> textures;
			std::vector<Camera *> cameras;
			std::vector<MBlight *> lights;

			// Represents the meshes that will be drawn
			std::vector<Renderable*> renderables;

			int sortByMeshAndDraw( Camera* cam, MBshader* shader, std::vector<Renderable*>& meshes );

		public:
			// Attributes:
			Math::Node rootNode;
			// Default constructors/destructor:
			Scene();
			~Scene();

			// Methods:
			void update();

			int draw(Camera *cam, MBshader *shader);

			int buildShadowMaps();

			/*! \breif Adds renderable objects to the scene to be rendered.
				
			 This will create the set of associations to be drawn in the scene.
			 Only unique combinations of all three elements will be added as a renderable.
			 \param node A node that should be under rootNode tree structure.
			 \param mesh The geometric definition. If already contained in this scene's meshes, that will be used.  If not contained, the mesh will be added to the list of meshes owned by this object (to be deleted).
			 \param material The properties of the rendering.  Like the mesh, this will be added to the list of memory to be deleted if it does not already exist under this object.
			 \return The newly added renderable, or the renderable previously added with the same properties.
			 */
			Renderable* addRenderable(Math::Node* node, MBmesh* mesh, MBmaterial* material);

			std::vector<Renderable*> getRenderablesFromNode(Math::Node* node);
			std::vector<Renderable*> getRenderablesFromMesh(MBmesh* mesh);
			std::vector<Renderable*> getRenderablesFromMaterial(MBmaterial* material);

			/*!
			 \return The id of the material added.
			 */
			MBmaterial* addMaterial(MBmaterial* material);
			MBmesh* addMesh(MBmesh* mesh);
			Texture* addTexture(Texture* mesh);

			std::vector<Renderable*>& getRenderables();
			std::vector<MBmesh*>& getMeshes();
			std::vector<MBmaterial*>& getMaterials();

			std::vector<Camera*>& getCameras();
			std::vector<MBlight*>& getLights();
			std::vector<Animation*>& getAnimations();
			std::vector<Texture*>& getTextures();

			static std::string getResourceDirectory();
		};

		void drawString(const char *str, int x, int y, float color[4], void *font);
		void drawString3D(const char *str, float pos[3], float color[4], void *font);
		
	}  // End Simulation namespace
	
}  // End Mogi namespace

#endif
