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

#ifndef MOGI_ANIMATION_H
#define MOGI_ANIMATION_H

//#include <assimp/scene.h>
#include <vector>

#include "channel.h"
#include "mogi/math/mmath.h"
#include "mogi/math/systems.h"

namespace Mogi {
namespace Simulation {

/**
 *  An animation handler class.  This imports an Assimp animation and handles
 * the playback and channel connections.
 */

class Animation {
private:
	double ticksPerSecond;
	double duration;
	std::string name;
	std::vector<Channel*> channels;

	Math::Time timer;
	double loopTime;

	void init();

public:
	Animation();
	~Animation();
//	Animation(const Animation& param);
//	Animation& operator=(const Animation&);

	//! Defines the animation from Assimp
	/*!
	 \param animation The animation from Assimp.
	 */
	//void set(aiAnimation* animation);

	//! Sets the animation's channel's parent node.
	/*!
	 \param rootNode The top level node to search for a match.
	 */
	void matchChannelsToNodes(Math::Node* rootNode);

	//! Begins playing the animation.
	/*!
	 \sa stop()
	 */
	void play();

	//! Pauses and resets the animation.
	/*!
	 \sa play()
	 */
	void stop();

	//! Updates all internal values.  This should be called on each iteration
	//! loop, before rendering.
	/*!
	 */
	void update();
};
}
}

#endif
