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

#include "channel.h"
#include <iostream>

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;
using namespace Math;

Channel::Channel() {
	parentNode = NULL;
	scale.makeI(4);
	rotation.makeI(4);
	location.makeI(4);
}

Channel::~Channel() {
	clearKeys();
}

void Channel::clearKeys() {
	for (int i = 0; i < locationKeys.size(); i++) {
		delete locationKeys[i];
	}
	locationKeys.clear();
	for (int i = 0; i < scalingKeys.size(); i++) {
		delete scalingKeys[i];
	}
	scalingKeys.clear();
	for (int i = 0; i < rotationKeys.size(); i++) {
		delete rotationKeys[i];
	}
	rotationKeys.clear();
}

void Channel::set(aiNodeAnim *channel) {
	name = channel->mNodeName.C_Str();
	// std::cout << "Channel name: " << name << std::endl;

	clearKeys();

	KeyLocation *locationKey;
	for (int i = 0; i < channel->mNumPositionKeys; i++) {
		locationKey = new KeyLocation;

		locationKeys.push_back(locationKey);
	}

	MBkeyRotation *rotationKey;
	for (int i = 0; i < channel->mNumRotationKeys; i++) {
		rotationKey = new MBkeyRotation;

		rotationKeys.push_back(rotationKey);
	}

	KeyScale *scalingKey;
	for (int i = 0; i < channel->mNumScalingKeys; i++) {
		scalingKey = new KeyScale;

		scalingKeys.push_back(scalingKey);
	}
}

void Channel::findNode(Node *rootNode) {
	if ((parentNode = rootNode->findChildByName(name)) == NULL) {
		std::cout << "Error!  Could not find the node that matches channel: "
				<< name << std::endl;
	}
}

void Channel::update(double time) {
	/*
	 for (int i = 0; i < scalingKeys.size(); i++) {
	 if (time >= scalingKeys[i]->time) {
	 for (int j = 0; j < 3; j++) {
	 scale(i,i) = scalingKeys[i]->value(i);
	 }
	 break;
	 }
	 }

	 for (int i = 0; i < rotationKeys.size(); i++) {
	 if (time >= rotationKeys[i]->time) {
	 //rotation = rotationKeys[i]->value.makeRotationMatrix4();
	 break;
	 }
	 }

	 for (int i = 0; i < locationKeys.size(); i++) {
	 if (time >= locationKeys[i]->time) {
	 for (int j = 0; j < 3; j++) {
	 location(i,i) = locationKeys[i]->value(i);
	 }
	 break;
	 }
	 }
	 */

	model = location * rotation * scale;
	if (parentNode) {
		// parentNode->setModelMatrix(model);
	}
}

#ifdef _cplusplus
}
#endif
