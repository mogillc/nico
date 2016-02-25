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
		 \struct NodeMatrixAndMeshID
		 \brief Used for mesh indexing for simulation (Will be deprecated soon).
		 */
		struct NodeMatrixAndMeshID {
			/*! Mesh Identifier.
			 */
			unsigned int ID;

			/*! Transformation matrix specific to the mesh, with respect to the node.
			 */
			//	const Matrix *modelMatrix;
			Math::Node* parentNode;
		};

		/** @file */
		class Scene {
		private:
			MBshader* shadowMapShader;
			MBshader* gShader;

			std::vector<Animation *> animations;
			std::vector<MBmaterial *> materials; // This could be loaded, but may not be important.
			std::vector<MBmesh *> meshes;
			std::vector<Texture *> textures;

			std::vector<NodeMatrixAndMeshID*> meshesToDraw;

			std::string objectLocation;

			int totalTriangles;
			
			void initialize();

		public:
			// Attributes:

			std::map<Math::Node*,std::vector<Simulation::MBmesh*> > nodeToMeshMap;

			Math::Node rootNode;
			std::vector<Camera *> cameras;
			std::vector<MBlight *> lights;

			// Default constructors/destructor:
			Scene();
			~Scene();

			void clearVectors();

			// Methods:
			void update();

			int draw(Camera *cam, MBshader *shader);

			void setLocation(const Math::Vector &loc);
			void setLocation(double x, double y, double z);

			void setScale(const Math::Vector &loc);
			void setScale(double x, double y, double z);
			void setScale(double s);

			void setOrientation(Math::Quaternion &quat);
			void setOrientation(double angle, Math::Vector &axis);

			Math::Node *findNodeByName(std::string nodeName);

			int buildShadowMaps();

			MBmesh *getMesh(std::string meshName);

			std::vector<MBmesh*>& getMeshes();
			std::vector<NodeMatrixAndMeshID*>& getMeshestoDraw();
			std::vector<Animation*>& getAnimations();
			std::vector<Texture*>& getTextures();
			std::vector<MBmaterial*>& getMaterials();

			void attachMeshToNode(Math::Node* node, int meshId);

			static std::string getResourceDirectory();
		};

		void drawString(const char *str, int x, int y, float color[4], void *font);
		void drawString3D(const char *str, float pos[3], float color[4], void *font);
		
	}  // End Simulation namespace
	
}  // End Mogi namespace

#endif
