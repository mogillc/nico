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
												 | aiProcess_CalcTangentSpace | aiProcess_FlipUVs );
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

		setScene(scene, mScene, filename, objectLocation.c_str(), createNode);

		// std::cout << "File: " << fileLocation << " loaded!" << std::endl;

		// set() appends the node to the end, so return the last node:
		return mScene->rootNode.child(mScene->rootNode.numberOfChildren() - 1);
#else // ASSIMP_FOUND
		std::cerr << "Error: Mogi::Simulation::Importer::loadObject() unsupported in this build." << std::endl;
		return NULL;
#endif // ASSIMP_FOUND
	}

	MBmesh* Importer::addMesh( Scene* mScene, std::string filename, std::string directory) {
		std::string fullPath = directory;
		fullPath.append("/");
		fullPath.append(filename);
		for (int i = 0; i < mScene->getMeshes().size(); i++) {
			if (mScene->getMeshes()[i]->fileName.compare(fullPath) == 0) {
				std::cout << "This mesh is already loaded: " << fullPath << std::endl;
				return mScene->getMeshes()[i];  // do not need to load a new mesh, it's already loaded
			}
		}

		if(loadObject(mScene, filename.c_str(), directory.c_str(), false) == NULL) {  // load a new mesh
			std::cerr << "Error: Mogi::Simulation::Importer::addMesh() failed." << std::endl;
		}
		return *mScene->getMeshes().rbegin();  // return the mesh index
	}

#ifdef ASSIMP_FOUND
	int Importer::setScene(const aiScene *scene, Scene* mScene, const char *fileName, const char *objectLocation, bool createNode) {
		std::string fileLocation(objectLocation);
		fileLocation.append("/");
		fileLocation.append(fileName);
		std::cout << "Importing: " << fileLocation << std::endl;

		// Now process the animations:
		for (int i = 0; i < scene->mNumAnimations; i++) {
			Animation *animation = new Animation;
			Simulation::Importer::setAnimation(scene->mAnimations[i], animation);

			// Now that we are here, process the channels (since nodes were solved ABOVE)
			animation->matchChannelsToNodes(&mScene->rootNode);
			mScene->getAnimations().push_back(animation);
		}

		// Set the materials:
		std::map<unsigned int, MBmaterial*> materialIndexTranslation;
		for (int i = 0; i < scene->mNumMaterials; i++) {
			MBmaterial *material = new MBmaterial;
			Importer::setMaterial(scene->mMaterials[i], objectLocation, material, mScene);
			mScene->addMaterial(material);
			materialIndexTranslation[i] = material;
		}

		// Set the textures:
		for (int i = 0; i < scene->mNumTextures; i++) {
			Texture *texture = new Texture;
			Importer::setTexture(scene->mTextures[i], texture);
			mScene->getTextures().push_back(texture);
		}

		// Set the meshes:
		int triangles = 0;
		std::map<unsigned int, MBmesh*> meshIndexTranslation;
		for (int i = 0; i < scene->mNumMeshes; i++) {
			MBmesh *mesh = new MBmesh();
			triangles += Importer::setMesh(scene->mMeshes[i], objectLocation, mesh)/3;

			mesh->matchBonesToNodes(&mScene->rootNode);
			mesh->fileName = fileLocation;
			mScene->addMesh(mesh);
			meshIndexTranslation[i] = mesh;
		}

		// Populate the node(s):
		if (createNode) {
			Node *currentNode = mScene->rootNode.addNode("dummy");
			populateNode(&currentNode, scene->mRootNode, scene->mMeshes, &mScene->rootNode, &mScene->getRenderables(), meshIndexTranslation, materialIndexTranslation);
		}
		
		// Set the lights:
		for (int i = 0; i < scene->mNumLights; i++) {
			MBlight *light = Importer::createAndSet(scene->mLights[i]);
			if (light == NULL) {
				std::cerr << "Error: Unable to create light: " << scene->mLights[i]->mName.C_Str() << std::endl;
				continue;
			}
			light->findNode(&mScene->rootNode);
			mScene->getLights().push_back(light);
		}

		// Set the cameras:
		for (int i = 0; i < scene->mNumCameras; i++) {
			Camera *camera = new Camera;
			Importer::set(scene->mCameras[i], camera);
			mScene->getCameras().push_back(camera);
		}
		return triangles;
	}

	void Importer::populateNode(Mogi::Math::Node **mNode, aiNode* aNode, aiMesh** aMeshes, Mogi::Math::Node *nodeParent, std::vector<Renderable*>* mRenderables, std::map<unsigned int, MBmesh*>& meshes, std::map<unsigned int, MBmaterial*>& materials) {
		(*mNode)->name = aNode->mName.C_Str();
		*mNode = nodeParent->adoptChild(mNode);

		for (int i = 0; i < aNode->mNumMeshes; i++) {
			Renderable *renderable = new Renderable;
			renderable->mesh = meshes[aNode->mMeshes[i]];
			renderable->node = *mNode;
			renderable->material = materials[aMeshes[aNode->mMeshes[i]]->mMaterialIndex];
			mRenderables->push_back(renderable);
		}

		Node *child;
		for (int i = 0; i < aNode->mNumChildren; i++) {
			child = new Node;
			populateNode(&child, aNode->mChildren[i], aMeshes, *mNode, mRenderables, meshes, materials);
		}

		Matrix transformationMatrix(4, 4);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				transformationMatrix(i, j) = aNode->mTransformation[i][j]; //*(node->mTransformation[i*4 + j]);
			}
		}

		(*mNode)->setModelMatrix(transformationMatrix);
	}

	void Importer::setTexture(aiTexture *aTexture, Texture* mTexture) {
		std::cout << "Warning: aiTexture -> Mogi Texture Unsupported" << std::endl;
	}

	int Importer::setMesh(aiMesh* aMesh, std::string fileLocation, MBmesh* mMesh) {
		// Name:
		if (aMesh->mName.length <= 1) {
			mMesh->name = fileLocation;
		} else {
			mMesh->name = aMesh->mName.C_Str();
		}
		std::cout << "\t\tAdding mesh: " << mMesh->name << std::endl;

//		bool shouldGenerateTangents = !aMesh->HasTangentsAndBitangents();
		Vector v1(3), v2(3), normal(3), tangent(3);	/// used for tangent generation, if needed.

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

		// Material:
//		if (materials != NULL) {
//			mMesh->setMaterial( materials->at(aMesh->mMaterialIndex + materialIDOffset));
//		}

		// Vertex Colors:
		// Normals:
		// Tangents:
		// Texture Coordinates:
		// Vertices:
//		aiColor4D col;
		Vector col(3);
//		if (mMesh->getMaterial() != NULL) {
//			col = mMesh->getMaterial()->getColorDiffuse();
//		}
		Vector3d defaultColor;
		defaultColor.x = 1.0;
		defaultColor.y = 1.0;// col(1);
		defaultColor.z = 1.0;//col(2);
		// data.clear();
		// importTextures(mat);
		std::vector<VertexData> vertexData(aMesh->mNumVertices);

		int i = 0;
		for (std::vector<VertexData>::iterator it = vertexData.begin(); it != vertexData.end(); it++, i++) {

			// position
			it->position.x = aMesh->mVertices[i].x;
			it->position.y = aMesh->mVertices[i].y;
			it->position.z = aMesh->mVertices[i].z;

			// normals
			it->normal.x = aMesh->mNormals[i].x;
			it->normal.y = aMesh->mNormals[i].y;
			it->normal.z = aMesh->mNormals[i].z;

			// tangent
			if (aMesh->mTangents) {
				it->tangent.x = aMesh->mTangents[i].x;
				it->tangent.y = aMesh->mTangents[i].y;
				it->tangent.z = aMesh->mTangents[i].z;
			} else {
				// This is a VERY slow method:
				normal(0) = it->normal.x;
				normal(1) = it->normal.y;
				normal(2) = it->normal.z;
				v1 = normal.cross(Vector::xAxis);
				v2 = normal.cross(Vector::yAxis);
				if (v1.magnitude() > v2.magnitude()) {
					v1.normalize();
					tangent = v1;
				} else {
					v2.normalize();
					tangent = v2;
				}
				it->tangent.x = tangent(0);
				it->tangent.y = tangent(1);
				it->tangent.z = tangent(2);
			}

			// colors
			if (aMesh->mColors[0]) {
				//!= material color
				it->color.x = aMesh->mColors[0][i].r;
				it->color.y = aMesh->mColors[0][i].g;
				it->color.z = aMesh->mColors[0][i].b;
			} else {
				it->color = defaultColor;
			}

			// color
			if (aMesh->mTextureCoords[0]) {
				it->U = aMesh->mTextureCoords[0][i].x;
				it->V = aMesh->mTextureCoords[0][i].y;
			} else {
				it->U = it->V = 0.0;
			}
		}
		mMesh->setVertexData(vertexData);

		mMesh->loadVerticesToVertexBufferObject();

		return mMesh->getNumberOfVertices();
	}

	void Importer::setMaterial(aiMaterial *material, std::string directoryOfObject, MBmaterial* mMaterial, Scene* textureStorage) {
		aiString tempName;
		aiGetMaterialString(material, AI_MATKEY_NAME, &tempName);
		mMaterial->setName( tempName.C_Str() );
		std::cout << "\t\tAdding material: " << tempName.C_Str() <<std::endl;

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
		
		importTextures(material, mMaterial, textureStorage);

	}

	void Importer::importTextures(aiMaterial *material, MBmaterial* mMaterial, Scene* textureStorage) {
//		mMaterial->forceDisable(0 >= loadTextures(material, aiTextureType_DIFFUSE, "colorMap", mMaterial),
//								(0 >= loadTextures(material, aiTextureType_NORMALS, "normalMap", mMaterial)) && (0 >= loadTextures(material, aiTextureType_HEIGHT, "normalMap", mMaterial)),
//								0 >= loadTextures(material, aiTextureType_DISPLACEMENT, "heightMap", mMaterial),
//								0 >= loadTextures(material, aiTextureType_SPECULAR, "specularityMap", mMaterial));
		loadTextures(material, aiTextureType_DIFFUSE, mMaterial, textureStorage);
		if(0 >= loadTextures(material, aiTextureType_NORMALS, mMaterial, textureStorage)) {
			loadTextures(material, aiTextureType_HEIGHT, mMaterial, textureStorage);
		}
		loadTextures(material, aiTextureType_DISPLACEMENT, mMaterial, textureStorage);
		loadTextures(material, aiTextureType_SPECULAR, mMaterial, textureStorage);
	}

	int Importer::loadTextures(aiMaterial *material, aiTextureType type, MBmaterial* mMaterial, Scene* textureStorage) {
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
//				texture->setUniformName(uniformVariable);
//				texture->setUniformIndex(i);
				textureStorage->addTexture(texture);
				switch (type) {
					case aiTextureType_DIFFUSE:
						mMaterial->setTexture(texture, Mogi::Simulation::MBmaterial::COLOR);
						break;
					case aiTextureType_NORMALS:
						mMaterial->setTexture(texture, Mogi::Simulation::MBmaterial::NORMAL);
						break;
					case aiTextureType_HEIGHT:
						mMaterial->setTexture(texture, Mogi::Simulation::MBmaterial::NORMAL);
						break;
					case aiTextureType_DISPLACEMENT:
						mMaterial->setTexture(texture, Mogi::Simulation::MBmaterial::HEIGHT);
						break;
					case aiTextureType_SPECULAR:
						mMaterial->setTexture(texture, Mogi::Simulation::MBmaterial::SPECULAR);
						break;
					default:
						std::cerr << "Error: Importer::loadTextures() unrecognized texture type: " << type << std::endl;
//						delete texture;
						break;
				}
//				mMaterial->addTexture(texture);// textures.push_back(texture);
			}
		}

		return numberOfLoadedTextures;
	}


	void Importer::setPointLight(aiLight* aLight, MBpointLight* mLight) {
		std::cout << "\t\tAdding point light: " << aLight->mName.C_Str() << std::endl;

		mLight->setName(aLight->mName.C_Str());
		mLight->setFOV(aLight->mAngleOuterCone * 180.0 / MOGI_PI);

		mLight->setAttenuationFactors(aLight->mAttenuationConstant,
									  aLight->mAttenuationLinear,
									  aLight->mAttenuationQuadratic);
//		std::cout << "Set light constants: " << aLight->mAttenuationConstant << ", " << aLight->mAttenuationLinear << ", " <<aLight->mAttenuationQuadratic << std::endl;
	}

	void Importer::setSpotLight(aiLight* aLight, MBspotLight* mLight) {
		std::cout << "\t\tAdding spot light: " << aLight->mName.C_Str() << std::endl;

		mLight->setName(aLight->mName.C_Str());
		mLight->setFOV(aLight->mAngleOuterCone * 180.0 / MOGI_PI);

		mLight->setCone(aLight->mAngleInnerCone, aLight->mAngleOuterCone);

		mLight->setAttenuationFactors(aLight->mAttenuationConstant,
									  aLight->mAttenuationLinear,
									  aLight->mAttenuationQuadratic);
//		std::cout << "Set light constants: " << aLight->mAttenuationConstant << ", " << aLight->mAttenuationLinear << ", " <<aLight->mAttenuationQuadratic << std::endl;

	}

	void Importer::setDirectionalLight(aiLight* aLight, MBdirectionalLight* mLight) {

	}

	MBlight* Importer::createAndSet(aiLight* aLight) {
		MBlight* mLight = NULL;
		switch (aLight->mType) {
			case aiLightSource_DIRECTIONAL:
				mLight = new MBdirectionalLight;
				setDirectionalLight(aLight, (MBdirectionalLight*)mLight);
				break;
			case aiLightSource_POINT:
				mLight = new MBpointLight;
				setPointLight(aLight, (MBpointLight*)mLight);
				break;
			case aiLightSource_SPOT:
				mLight = new MBspotLight;
				setSpotLight(aLight, (MBspotLight*)mLight);
				break;
//			case aiLightSource_AMBIENT:
			case aiLightSource_UNDEFINED:
			case _aiLightSource_Force32Bit:
				break;
		}
		return mLight;
	}



	void Importer::setKeyLocation(aiVectorKey* aVectorKey, KeyLocation* mKeyLocation) {
		mKeyLocation->time = aVectorKey->mTime;
		mKeyLocation->value(0) = aVectorKey->mValue.x;
		mKeyLocation->value(1) = aVectorKey->mValue.y;
		mKeyLocation->value(2) = aVectorKey->mValue.z;
	}

	void Importer::setKeyScale(aiVectorKey* aVectorKey, KeyScale* mKeyScale) {
		mKeyScale->time = aVectorKey->mTime;
		mKeyScale->value(0) = aVectorKey->mValue.x;
		mKeyScale->value(1) = aVectorKey->mValue.y;
		mKeyScale->value(2) = aVectorKey->mValue.z;
	}

	void Importer::setKeyRotation(aiQuatKey* aQuatKey, MBkeyRotation* mMBkeyRotation) {
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
				setBone(mesh->mBones[i], temp);
				// std::cout << "Bone name: " << mesh->mBones[i]->mName.C_Str() <<
				// std::endl;
				BoneSet.push_back(temp);
			}
		}

		return BoneSet;
	}

	void Importer::setBone(aiBone* aBone, Bone* mBone) {
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

	void Importer::setAnimation(aiAnimation* aAnimation, Animation* mAnimation) {
		//mAnimation->name = aAnimation->mName.C_Str();	// TODO:
		//mAnimation->duration = aAnimation->mDuration;	// TODO:
		//mAnimation->ticksPerSecond = aAnimation->mTicksPerSecond;	// TODO:

		Channel* tempChannel;
		//mAnimation->channels.clear();	// TODO:
		for (int i = 0; i < aAnimation->mNumChannels; i++) {
			tempChannel = new Channel;	// TODO:
			//			tempChannel->set(animation->mChannels[i]);
			setChannel(aAnimation->mChannels[i], tempChannel);
			//mAnimation->channels.push_back(tempChannel);	// TODO:
			delete tempChannel;	// TODO: remove this line when the above is filled in.
		}
	}

	void Importer::setChannel(aiNodeAnim *aNodeAnim, Channel* mChannel) {
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
