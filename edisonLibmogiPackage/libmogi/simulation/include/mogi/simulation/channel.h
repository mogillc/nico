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

#ifndef MOGI_CHANNEL_H
#define MOGI_CHANNEL_H

//#include <assimp/scene.h>
#include <vector>

#include "key.h"
#include "mogi/math/mmath.h"
#include "mogi/math/node.h"

namespace Mogi {
namespace Simulation {

class Channel {
private:
public:
	std::string name;
	Math::Node *parentNode;

	Math::Matrix scale;
	Math::Matrix rotation;
	Math::Matrix location;
	Math::Matrix model;

	std::vector<KeyLocation *> locationKeys;
	std::vector<MBkeyRotation *> rotationKeys;
	std::vector<KeyScale *> scalingKeys;

	void clearKeys();

public:
	//void set(aiNodeAnim *channel);
	void findNode(Math::Node *rootNode);

	Channel();
	~Channel();

	// Updates the modelMatrix in the node given the time:
	void update(double time);
	/*
	 Channel();
	 ~Channel();
	 Channel( const Channel& param );
	 Channel & operator = (const Channel&);
	 */
};
}
}

#endif
