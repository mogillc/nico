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

void Bone::init() {
	name = "";
	offsetMatrix.makeI(4);
}

void Bone::set(const Bone* Bone) {
	name = Bone->name;
	offsetMatrix = Bone->offsetMatrix;

	weights = Bone->weights;
}

const std::string& Bone::getName() {
	return name;
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
