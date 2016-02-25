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

#include "importer.h"
#include <iostream>
#include <sstream>
#include <mogi.h>

#ifdef ASSIMP_FOUND

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Math;
	using namespace Simulation;



	Math::Node* Importer::loadObject(Scene* mScene, const char *filename, const char *location, bool createNode) {
		if (filename == NULL || location == NULL) {
			return NULL;
		}
		std::string objectLocation;
#if defined(BUILD_FOR_IOS) || defined(ANDROID)
		std::stringstream objectLocationss("");
		objectLocationss << Mogi::getResourceDirectory() << "/" << location;
		objectLocation = objectLocationss.str();
#else
		objectLocation = std::string(location);
#endif

#ifdef ASSIMP_FOUND
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

		set(scene, mScene, filename, objectLocation.c_str(), createNode);

		// std::cout << "File: " << fileLocation << " loaded!" << std::endl;

		// set() appends the node to the end, so return the last node:
		return mScene->rootNode.child(mScene->rootNode.numberOfChildren() - 1);
#else // ASSIMP_FOUND
		std::cerr << "Error: Mogi::Simulation::Importer::loadObject() unsupported in this build." << std::endl;
		return NULL;
#endif // ASSIMP_FOUND
	}

	int Importer::addMesh( Scene* mScene, std::string filename, std::string directory) {
		std::string fullPath = directory;
		fullPath.append("/");
		fullPath.append(filename);
		for (int i = 0; i < mScene->getMeshes().size(); i++) {
			if (mScene->getMeshes()[i]->fileName.compare(fullPath) == 0) {
				return i;  // do not need to load a new mesh, it's already loaded
			}
		}

		if(loadObject(mScene, filename.c_str(), directory.c_str(), false) == NULL) {  // load a new mesh
			std::cerr << "Error: Mogi::Simulation::Importer::addMesh() failed." << std::endl;
		}
		return (int)mScene->getMeshes().size() - 1;  // return the mesh index
	}

#ifdef ASSIMP_FOUND
	int Importer::set(const aiScene *scene, Scene* mScene, const char *fileName, const char *objectLocation, bool createNode) {
		std::string fileLocation(objectLocation);
		fileLocation.append("/");
		fileLocation.append(fileName);

		// First process the nodes:
		// std::cout << "\tScanning Nodes, current Mesh size: " << meshes.size() << ":
		// " << std::endl;

		// rootNode.set(scene->mRootNode, NULL, meshes.size());
		Node *currentNode = NULL;
		if (createNode) {
			currentNode = mScene->rootNode.addNode("dummy");
			// currentNode->set( scene->mRootNode, &rootNode, meshes.size());
			populateNode(&currentNode, scene->mRootNode, &mScene->rootNode, (int)mScene->getMeshes().size(), &mScene->getMeshestoDraw());
		}
		// rootNode.findChildByName("dummy")->set( scene->mRootNode, &rootNode,
		// meshes.size());

		// Now process the animations:
		// std::cout << "\tNumber of Animations : " << scene->mNumAnimations <<
		// std::endl;
		Animation *animation;
		for (int i = 0; i < scene->mNumAnimations; i++) {
			animation = new Animation;
			//animation->set(scene->mAnimations[i]);
			Simulation::Importer::set(scene->mAnimations[i], animation);

			// Now that we are here, process the channels (since nodes were solved
			// ABOVE)
			animation->matchChannelsToNodes(&mScene->rootNode);
			mScene->getAnimations().push_back(animation);
		}

		// Set the materials:
		// std::cout << "\tNumber of Textures   : " << scene->mNumTextures <<
		// std::endl;
		// std::cout << "\tNumber of Materials  : " << scene->mNumMaterials <<
		// std::endl;
		MBmaterial *material;
		int materialIDOffset = (int)mScene->getMaterials().size();
		for (int i = 0; i < scene->mNumMaterials; i++) {
			material = new MBmaterial;
			//		material->set(scene->mMaterials[i], objectLocation);
			Importer::set(scene->mMaterials[i], objectLocation, material);
			mScene->getMaterials().push_back(material);
		}

		// Set the textures:
		Texture *texture;
		for (int i = 0; i < scene->mNumTextures; i++) {
			texture = new Texture;
			//		texture->set(scene->mTextures[i]);
			Importer::set(scene->mTextures[i], texture);
			mScene->getTextures().push_back(texture);
		}

		// Set the meshes:
		int triangles = 0;
		/// std::cout << "\tNumber of Meshes     : " << scene->mNumMeshes <<
		/// std::endl;
		MBmesh *mesh;
		for (int i = 0; i < scene->mNumMeshes; i++) {
			mesh = new MBmesh();
			//		triangles += mesh->set(scene->mMeshes[i], scene->mMaterials,
			//				objectLocation, materialIDOffset);
			triangles += Importer::set(scene->mMeshes[i], scene->mMaterials, objectLocation, materialIDOffset, mesh)/3;
			mesh->matchBonesToNodes(&mScene->rootNode);
			mesh->fileName = fileLocation;
			mScene->getMeshes().push_back(mesh);

			mScene->nodeToMeshMap[currentNode].push_back(mesh);
		}

		// Set the lights:
		// std::cout << "\tNumber of Lights     : " << scene->mNumLights << std::endl;
		MBlight *light;
		for (int i = 0; i < scene->mNumLights; i++) {
			//		light = MBlight::create(scene->mLights[i]);
			light = Importer::createAndSet(scene->mLights[i]);
			if (light == NULL) {
				std::cerr << "Error: Unable to create light: " << scene->mLights[i]->mName.C_Str() << std::endl;
				continue;
			}
			light->findNode(&mScene->rootNode);
			mScene->lights.push_back(light);
		}

		// Set the cameras:
		// std::cout << "\tNumber of Cameras    : " << scene->mNumCameras <<
		// std::endl;
		Camera *camera;
		for (int i = 0; i < scene->mNumCameras; i++) {
			camera = new Camera;
			//		camera->set(scene->mCameras[i]);
			Importer::set(scene->mCameras[i], camera);
			// cameras.push_back(camera);
		}
		return triangles;
	}

	void Importer::populateNode(Node **theNode, aiNode *node, Node *nodeParent, int meshIDOffset, std::vector<NodeMatrixAndMeshID*>* meshesToDraw) {
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




	void Importer::set(aiTexture *aTexture, Texture* mTexture) {
		std::cout << "Warning: aiTexture -> Mogi Texture Unsupported" << std::endl;
	}

	int Importer::set(aiMesh* aMesh, aiMaterial** materials, std::string fileLocation, int materialIDOffset, MBmesh* mMesh) {
		// Name:
		if (aMesh->mName.length <= 1) {
			mMesh->name = fileLocation;
		} else {
			mMesh->name = aMesh->mName.C_Str();
		}
		std::cout << "\t\tAdding mesh: " << mMesh->name << std::endl;

		// Bitangents: (not needed...?)
		mMesh->setObjectLocation(fileLocation);
		// Bones:
		mMesh->setBones( Importer::getBones(aMesh) );

		// Faces (indices):
		// indices.clear();
		std::vector<unsigned int> indices;
		for (int i = 0; i < aMesh->mNumFaces; i++) {
			aiFace face = aMesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; j++)  // 0..2
			{
				indices.push_back(face.mIndices[j]);
			}
		}
		mMesh->setIndices(indices);

		// Material Index:
		mMesh->setMaterialIndex( aMesh->mMaterialIndex + materialIDOffset );

		// Vertex Colors:
		// Normals:
		// Tangents:
		// Texture Coordinates:
		// Vertices:
		aiColor4D col;
		aiMaterial* mat = materials[aMesh->mMaterialIndex];
		aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &col);
		Vector3d defaultColor;
		defaultColor.x = col.r;
		defaultColor.y = col.g;
		defaultColor.z = col.b;
		// data.clear();
		// importTextures(mat);
		std::vector<VertexData> vertexData;
		for (int i = 0; i < aMesh->mNumVertices; i++) {
			VertexData tmp;
			Vector3d tmpVec;

			// position
			tmpVec.x = aMesh->mVertices[i].x;
			tmpVec.y = aMesh->mVertices[i].y;
			tmpVec.z = aMesh->mVertices[i].z;
			tmp.position = tmpVec;

			// normals
			tmpVec.x = aMesh->mNormals[i].x;
			tmpVec.y = aMesh->mNormals[i].y;
			tmpVec.z = aMesh->mNormals[i].z;
			tmp.normal = tmpVec;

			// tangent
			if (aMesh->mTangents) {
				tmpVec.x = aMesh->mTangents[i].x;
				tmpVec.y = aMesh->mTangents[i].y;
				tmpVec.z = aMesh->mTangents[i].z;
			} else {
				tmpVec.x = 1.0;
				tmpVec.y = tmpVec.z = 0;
			}
			tmp.tangent = tmpVec;

			// colors
			if (aMesh->mColors[0]) {
				//!= material color
				tmpVec.x = aMesh->mColors[0][i].r;
				tmpVec.y = aMesh->mColors[0][i].g;
				tmpVec.z = aMesh->mColors[0][i].b;
			} else {
				tmpVec = defaultColor;
			}
			tmp.color = tmpVec;

			// color
			if (aMesh->mTextureCoords[0]) {
				tmpVec.x = aMesh->mTextureCoords[0][i].x;
				tmpVec.y = aMesh->mTextureCoords[0][i].y;
			} else {
				tmpVec.x = tmpVec.y = tmpVec.z = 0.0;
			}
			tmp.U = tmpVec.x;
			tmp.V = tmpVec.y;
			vertexData.push_back(tmp);
		}
		mMesh->setVertexData(vertexData);

		mMesh->loadVerticesToVertexBufferObject();

		return mMesh->getNumberOfVertices();
	}

	void Importer::set(aiMaterial *material, std::string directoryOfObject, MBmaterial* mMaterial) {
		aiString tempName;
		aiGetMaterialString(material, AI_MATKEY_NAME, &tempName);
		mMaterial->setName( tempName.C_Str() );
		// std::cout << "\t\tAdding material: " << name <<std::endl;

		mMaterial->setDirectory(directoryOfObject);

		aiColor3D color(0.f, 0.f, 0.f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		mMaterial->setColorDiffuse(color.r, color.g, color.b);

		material->Get(AI_MATKEY_COLOR_AMBIENT, color);
		mMaterial->setColorAmbient(color.r, color.g, color.b);

		material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		mMaterial->setColorEmissive(color.r, color.g, color.b);

		material->Get(AI_MATKEY_COLOR_SPECULAR, color);
		mMaterial->setColorSpecular(color.r, color.g, color.b);

		material->Get(AI_MATKEY_COLOR_TRANSPARENT, color);
		mMaterial->setColorTransparent(color.r, color.g, color.b);

		float shininess;
		if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess)) {
			shininess = 32;
		} else {
			// TODO: int eh future, assimp may not implement this wierd factor of 4 issue, so double check this in assimp 3.1
			shininess /= 4.0;
		}
//		std::cout << "Specular Shine: " << shininess << std::endl;
		float specularStrength;
		if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS_STRENGTH, specularStrength)) {
			specularStrength = 1;
		}
//		std::cout << "Specular Strength: " << specularStrength<< std::endl;
		mMaterial->setShininess(shininess, specularStrength);

		mMaterial->setMetallicLevel(1.0);	// why is this here?
		
		importTextures(material, mMaterial);

	}

	void Importer::importTextures(aiMaterial *material, MBmaterial* mMaterial) {
		mMaterial->forceDisable(0 >= loadTextures(material, aiTextureType_DIFFUSE, "colorMap", mMaterial),
								(0 >= loadTextures(material, aiTextureType_NORMALS, "normalMap", mMaterial)) && (0 >= loadTextures(material, aiTextureType_HEIGHT, "normalMap", mMaterial)),
								0 >= loadTextures(material, aiTextureType_DISPLACEMENT, "heightMap", mMaterial),
								0 >= loadTextures(material, aiTextureType_SPECULAR, "specularityMap", mMaterial));
	}

	int Importer::loadTextures(aiMaterial *material, aiTextureType type, std::string uniformVariable, MBmaterial* mMaterial) {
		Texture *texture;
		int numberOfLoadedTextures = 0;
		for (int i = 0; i < material->GetTextureCount(type); i++) {
			numberOfLoadedTextures++;
			texture = new Texture;
			aiString str;
			material->GetTexture(type, i, &str);

			std::string textureLocation = mMaterial->getDirectory() + "/";
			textureLocation.append(str.C_Str());

			if (texture->loadFromImage(textureLocation) < 0) {
				numberOfLoadedTextures--;
				std::cout << "Error! Could not load texture: " << textureLocation << " for " << mMaterial->getName() << std::endl;
				delete texture;
			} else {
				texture->setUniformName(uniformVariable);
				texture->setUniformIndex(i);
				mMaterial->addTexture(texture);// textures.push_back(texture);
			}
		}

		return numberOfLoadedTextures;
	}


	void Importer::set(aiLight* aLight, MBpointLight* mLight) {
		std::cout << "\t\tAdding point light: " << aLight->mName.C_Str() << std::endl;

		mLight->setName(aLight->mName.C_Str());
		mLight->setFOV(aLight->mAngleOuterCone * 180.0 / MOGI_PI);

		mLight->setAttenuationFactors(aLight->mAttenuationConstant,
									  aLight->mAttenuationLinear,
									  aLight->mAttenuationQuadratic);
//		std::cout << "Set light constants: " << aLight->mAttenuationConstant << ", " << aLight->mAttenuationLinear << ", " <<aLight->mAttenuationQuadratic << std::endl;
	}

	void Importer::set(aiLight* aLight, MBspotLight* mLight) {
		std::cout << "\t\tAdding spot light: " << aLight->mName.C_Str() << std::endl;

		mLight->setName(aLight->mName.C_Str());
		mLight->setFOV(aLight->mAngleOuterCone * 180.0 / MOGI_PI);

		mLight->setCone(aLight->mAngleInnerCone, aLight->mAngleOuterCone);

		mLight->setAttenuationFactors(aLight->mAttenuationConstant,
									  aLight->mAttenuationLinear,
									  aLight->mAttenuationQuadratic);
//		std::cout << "Set light constants: " << aLight->mAttenuationConstant << ", " << aLight->mAttenuationLinear << ", " <<aLight->mAttenuationQuadratic << std::endl;

	}

	void Importer::set(aiLight* aLight, MBdirectionalLight* mLight) {

	}

	MBlight* Importer::createAndSet(aiLight* aLight) {
		MBlight* mLight = NULL;
		switch (aLight->mType) {
			case aiLightSource_DIRECTIONAL:
				mLight = new MBdirectionalLight;
				set(aLight, (MBdirectionalLight*)mLight);
				break;
			case aiLightSource_POINT:
				mLight = new MBpointLight;
				set(aLight, (MBpointLight*)mLight);
				break;
			case aiLightSource_SPOT:
				mLight = new MBspotLight;
				set(aLight, (MBspotLight*)mLight);
				break;
//			case aiLightSource_AMBIENT:
			case aiLightSource_UNDEFINED:
			case _aiLightSource_Force32Bit:
				break;
		}
		return mLight;
	}



	void Importer::set(aiVectorKey* aVectorKey, KeyLocation* mKeyLocation) {
		mKeyLocation->time = aVectorKey->mTime;
		mKeyLocation->value(0) = aVectorKey->mValue.x;
		mKeyLocation->value(1) = aVectorKey->mValue.y;
		mKeyLocation->value(2) = aVectorKey->mValue.z;
	}

	void Importer::set(aiVectorKey* aVectorKey, KeyScale* mKeyScale) {
		mKeyScale->time = aVectorKey->mTime;
		mKeyScale->value(0) = aVectorKey->mValue.x;
		mKeyScale->value(1) = aVectorKey->mValue.y;
		mKeyScale->value(2) = aVectorKey->mValue.z;
	}

	void Importer::set(aiQuatKey* aQuatKey, MBkeyRotation* mMBkeyRotation) {
		mMBkeyRotation->time = aQuatKey->mTime;
		mMBkeyRotation->value(0) = aQuatKey->mValue.w;
		mMBkeyRotation->value(1) = aQuatKey->mValue.x;
		mMBkeyRotation->value(2) = aQuatKey->mValue.y;
		mMBkeyRotation->value(3) = aQuatKey->mValue.z;
	}

	void Importer::set(aiCamera* aCamera, Camera* mCamera) {
		mCamera->setLocation(aCamera->mPosition[0], aCamera->mPosition[1], aCamera->mPosition[2]);

		mCamera->makeProjection(mCamera->getXresolution(), mCamera->getYresolution(), aCamera->mHorizontalFOV, aCamera->mClipPlaneNear, aCamera->mClipPlaneFar);

		mCamera->setName( aCamera->mName.C_Str() );
		// NOT FINISHED!
		// camera->mUp // the Up vector;
		// camera->mLookAt // the vector that the camera is looking at
	}

	std::vector<Bone*> Importer::getBones(aiMesh* mesh) {
		std::vector<Bone*> BoneSet;

		if (mesh->HasBones()) {
			Bone* temp;
			for (int i = 0; i < mesh->mNumBones; i++) {
				temp = new Bone;
				//temp->set(mesh->mBones[i]);
				set(mesh->mBones[i], temp);
				// std::cout << "Bone name: " << mesh->mBones[i]->mName.C_Str() <<
				// std::endl;
				BoneSet.push_back(temp);
			}
		}

		return BoneSet;
	}

	void Importer::set(aiBone* aBone, Bone* mBone) {
		//mBone->name = aBone->mName.C_Str();	// TODO:
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				//mBone->offsetMatrix(i, j) = *(aBone->mOffsetMatrix[i * 4 + j]);	// TODO:
			}
		}

		vertexWeight* tempVW;
		for (int i = 0; i < aBone->mNumWeights; i++) {
			tempVW = new vertexWeight;
			tempVW->weight = aBone->mWeights[i].mWeight;
			tempVW->vertexID = aBone->mWeights[i].mVertexId;
			//mBone->weights.push_back(tempVW);	// TODO:
			delete tempVW;	// TODO: remove this line when the above is filled in.

		}
		// std::cout << "Number of weights:" << Bone->mNumWeights << std::endl;
	}

	void Importer::set(aiAnimation* aAnimation, Animation* mAnimation) {
		//mAnimation->name = aAnimation->mName.C_Str();	// TODO:
		//mAnimation->duration = aAnimation->mDuration;	// TODO:
		//mAnimation->ticksPerSecond = aAnimation->mTicksPerSecond;	// TODO:

		Channel* tempChannel;
		//mAnimation->channels.clear();	// TODO:
		for (int i = 0; i < aAnimation->mNumChannels; i++) {
			tempChannel = new Channel;	// TODO:
			//			tempChannel->set(animation->mChannels[i]);
			set(aAnimation->mChannels[i], tempChannel);
			//mAnimation->channels.push_back(tempChannel);	// TODO:
			delete tempChannel;	// TODO: remove this line when the above is filled in.
		}
	}

	void Importer::set(aiNodeAnim *aNodeAnim, Channel* mChannel) {
		mChannel->name = aNodeAnim->mNodeName.C_Str();
		// std::cout << "Channel name: " << name << std::endl;

		mChannel->clearKeys();

		KeyLocation *locationKey;
		for (int i = 0; i < aNodeAnim->mNumPositionKeys; i++) {
			locationKey = new KeyLocation;

			mChannel->locationKeys.push_back(locationKey);
		}

		MBkeyRotation *rotationKey;
		for (int i = 0; i < aNodeAnim->mNumRotationKeys; i++) {
			rotationKey = new MBkeyRotation;

			mChannel->rotationKeys.push_back(rotationKey);
		}

		KeyScale *scalingKey;
		for (int i = 0; i < aNodeAnim->mNumScalingKeys; i++) {
			scalingKey = new KeyScale;

			mChannel->scalingKeys.push_back(scalingKey);
		}

	}
#endif // ASSIMP_FOUND

#ifdef _cplusplus
}
#endif

#endif // ASSIMP_FOUND
