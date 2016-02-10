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

#include "bone.h"

#include <iostream>

#ifdef _cplusplus
extern "C" {
#endif
using namespace Mogi;
using namespace Simulation;
using namespace Math;

Bone::Bone() {
	init();
}

Bone::~Bone() {
	for (int i = 0; i < weights.size(); i++) {
		delete weights[i];
	}
}

Bone::Bone(const Bone& param) {
	init();
	set(&param);
}
Bone& Bone::operator=(const Bone& param) {
	if (this != &param)  // only run if it is not the same object
			{
		set(&param);
	}
	return *this;
}

void Bone::init() {
	name = "";
	offsetMatrix.makeI(4);
}

void Bone::set(aiBone* Bone) {
	name = Bone->mName.C_Str();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			offsetMatrix(i, j) = *(Bone->mOffsetMatrix[i * 4 + j]);
		}
	}

	vertexWeight* tempVW;
	for (int i = 0; i < Bone->mNumWeights; i++) {
		tempVW = new vertexWeight;
		tempVW->weight = Bone->mWeights[i].mWeight;
		tempVW->vertexID = Bone->mWeights[i].mVertexId;
		weights.push_back(tempVW);
	}
	// std::cout << "Number of weights:" << Bone->mNumWeights << std::endl;
}

void Bone::set(const Bone* Bone) {
	name = Bone->name;
	offsetMatrix = Bone->offsetMatrix;

	weights = Bone->weights;
}

const std::string& Bone::getName() {
	return name;
}

std::vector<Bone*> Simulation::getBones(aiMesh* mesh) {
	std::vector<Bone*> BoneSet;

	if (mesh->HasBones()) {
		Bone* temp;
		for (int i = 0; i < mesh->mNumBones; i++) {
			temp = new Bone;
			temp->set(mesh->mBones[i]);
			// std::cout << "Bone name: " << mesh->mBones[i]->mName.C_Str() <<
			// std::endl;
			BoneSet.push_back(temp);
		}
	}

	return BoneSet;
}

void Bone::findNode(Node* rootNode) {
	if ((parentNode = rootNode->findChildByName(name)) == NULL) {
		std::cout << "Error!  Could not find the node that matches channel: "
				<< name << std::endl;
	} else {
		// Set the parent node offset matrix:
		// parentNode->setOffsetMatrix(offsetMatrix);
	}
}

#ifdef _cplusplus
}
#endif
