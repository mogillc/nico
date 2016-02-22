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

#include "key.h"

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

/*
 MBkey::MBkey() {
 init();
 }

 MBkey::~MBkey() {

 //std::cout << "Deleting Bone: " << name << std::endl;
 }

 MBkey::MBkey( const MBkey& param ) {
 init();

 }
 MBkey & MBkey::operator = (const MBkey& param) {
 if (this != &param) // only run if it is not the same object
 {

 }
 return *this;
 }
 */

KeyLocation::KeyLocation() {
	value.setLength(3);
}

KeyScale::KeyScale() {
	value.setLength(3);
}

//void KeyLocation::set(aiVectorKey* key) {
//	time = key->mTime;
//	value(0) = key->mValue.x;
//	value(1) = key->mValue.y;
//	value(2) = key->mValue.z;
//}
//
//void KeyScale::set(aiVectorKey* key) {
//	time = key->mTime;
//	value(0) = key->mValue.x;
//	value(1) = key->mValue.y;
//	value(2) = key->mValue.z;
//}
//
//void MBkeyRotation::set(aiQuatKey* key) {
//	time = key->mTime;
//	value(0) = key->mValue.w;
//	value(1) = key->mValue.x;
//	value(2) = key->mValue.y;
//	value(3) = key->mValue.z;
//}

#ifdef _cplusplus
}
#endif
