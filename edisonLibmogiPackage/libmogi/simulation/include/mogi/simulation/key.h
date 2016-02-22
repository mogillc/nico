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

#ifndef MOGI_KEY_H
#define MOGI_KEY_H

//#include <assimp/scene.h>
#include <vector>

#include "mogi/math/mmath.h"

namespace Mogi {
namespace Simulation {

/**
 Abstract class to handle a keyframe for animations.
 */

class MBkey {
private:
public:
	double time;

	//	MBkey();
	//	~MBkey();
	//	MBkey( const MBkey& param );
	//	MBkey & operator = (const MBkey&);
};

/**
 Concrete class to handle a rotation keyframe for animations.
 */
class MBkeyRotation: public MBkey {
public:
	Math::Quaternion value;
//	void set(aiQuatKey* key);
};

/**
 Concrete class to handle a location keyframe for animations.
 */
class KeyLocation: public MBkey {
public:
	Math::Vector value;
	KeyLocation();
//	void set(aiVectorKey* key);
};

/**
 Concrete class to handle a scale keyframe for animations.
 */
class KeyScale: public MBkey {
public:
	Math::Vector value;
	KeyScale();
//	void set(aiVectorKey* key);
};
}
}

#endif
