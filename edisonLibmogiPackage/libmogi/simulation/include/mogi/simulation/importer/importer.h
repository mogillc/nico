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

#ifndef MOGI_SIMULATION_IMPORTER_H
#define MOGI_SIMULATION_IMPORTER_H

#include "mogi/simulation/scene.h"

#ifdef ASSIMP_FOUND
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#endif

namespace Mogi {
	namespace Simulation {

		class Importer {

		public:

			/*! \brief Loads a file containing the scene.

			 This calls a corresponding import method to load all meshes, textures, cameras, lights, and materials.
			 Currently this is only supported with a build of Assimp 3.0 or later.
			 \param mScene The scene to add the mesh to.
			 \param filename The name of the file, with extension.
			 \param createNode Will allocate a new node for mScene.  (why is this needed?)
			 \return The most recently added node.
			 */
			static Mogi::Math::Node* loadObject(Scene* mScene, const char *filename, const char *directory, bool createNode = true);

			/*! \brief Adds a mesh to a scene.
			 
			 Currently this is only supported with a build of Assimp 3.0 or later.
			 Also, this currently just calls loadObject() after ensuring the mesh has not yet been added.
			 \param mScene The scene to add the mesh to.
			 \param filename The name of the file, with extension.
			 \param directory The path to the file.  This is needed when assets like textures are in the same directory.
			 \return The index of the last added mesh in the mesh vector, or the index of this mesh when previously loaded.
			 */
			static MBmesh* addMesh( Scene* mScene, std::string filename, std::string directory);

#ifdef ASSIMP_FOUND
			static int setScene(const aiScene *aScene, Scene* mScene, const char *fileName, const char *objectLocation, bool createNode = true);
//			static void populateNode(Mogi::Math::Node **theNode, aiNode *node, Mogi::Math::Node *nodeParent, int meshIDOffset, std::vector<Renderable*>* meshesToDraw);
			static void populateNode(Mogi::Math::Node **mNode, aiNode* aNode, aiMesh** aMeshes, Mogi::Math::Node *nodeParent, std::vector<Renderable*>* mRenderables, std::map<unsigned int, MBmesh*>& meshes, std::map<unsigned int, MBmaterial*>& materials);

			static void setTexture(aiTexture *aTexture, Texture* mTexture);

//			static int setMesh(aiMesh* aMesh, aiMaterial** materials, std::string fileLocation, int materialIDOffset, MBmesh* mMesh);
			static int setMesh(aiMesh* aMesh, std::string fileLocation, MBmesh* mMesh);

			static void setMaterial(aiMaterial *material, std::string directoryOfObject, MBmaterial* mMaterial, Scene* textureStorage);
			static void importTextures(aiMaterial *material, MBmaterial* mMaterial, Scene* textureStorage);
			static int loadTextures(aiMaterial *material, aiTextureType type, MBmaterial* mMaterial, Scene* textureStorage);

			static MBlight* createAndSet(aiLight* aLight);
			static void setPointLight(aiLight* aLight, MBpointLight* mLight);
			static void setSpotLight(aiLight* aLight, MBspotLight* mLight);
			static void setDirectionalLight(aiLight* aLight, MBdirectionalLight* mLight);

			static void setKeyLocation(aiVectorKey* aVectorKey, KeyLocation* mKeyLocation);
			static void setKeyScale(aiVectorKey* aVectorKey, KeyScale* mKeyScale);
			static void setKeyRotation(aiQuatKey* aQuatKey, MBkeyRotation* mMBkeyRotation);
			static void set(aiCamera* aCamera, Camera* mCamera);
			static std::vector<Bone *> getBones(aiMesh *mesh);
			static void setBone(aiBone* aBone, Bone* mBone);
			static void setAnimation(aiAnimation* aAnimation, Animation* mAnimation);
			static void setChannel(aiNodeAnim *aNodeAnim, Channel* mChannel);
#endif // ASSIMP_FOUND
		};

	}
}

#endif // MOGI_SIMULATION_IMPORTER_H
