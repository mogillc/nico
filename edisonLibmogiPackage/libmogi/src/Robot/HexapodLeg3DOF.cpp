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

#include "hexapod.h"

#include <sstream>

#ifdef _cplusplus
extern "C" {
#endif

	namespace Mogi {
		using namespace Math;
		using namespace App;

		namespace Robot {

			int HexapodLeg3DOF::setBaseDimensions(const Vector* dimensions) {
				return setNodeDimensions(0, dimensions);
			}

			int HexapodLeg3DOF::setCoxaDimensions(const Vector* dimensions) {
				return setNodeDimensions(1, dimensions);
			}

			int HexapodLeg3DOF::setFemurDimensions(const Vector* dimensions) {
				return setNodeDimensions(2, dimensions);
			}

			int HexapodLeg3DOF::setTibiaDimensions(const Vector* dimensions) {
				return setNodeDimensions(3, dimensions);
			}

			int HexapodLeg3DOF::setConfigurationFromJSONValue( JsonValueInterface& leg ) {

				setStringValueIfSafe( &name, leg["name"]);


				//setBase(location, orientation);

				JsonValueInterface legLinks = leg["links"];
				if (!legLinks.isArray() || legLinks.size() != 1) {
					return -1;
				}

				JsonValueInterface base = legLinks[0];
				if (base["name"].isString()) {	// TODO
				}

				Vector link(3);
				JsonValueInterface baseLink = base["link"];
				if (baseLink.isArray() && baseLink.size() == 3) {
					for (unsigned int j = 0; j < 3; j++) {
						link(j) = baseLink[j].asDouble();
					}
					setBaseDimensions(&link);
				}

				JsonValueInterface baseLinks = base["links"];
				if (!baseLinks.isArray() || baseLinks.size() != 1) {
					std::cerr << "Unable to parse JSON configuration for HexapodLegZYY for base links of " << name << std::endl;
					return -1;
				}

				JsonValueInterface coxa = baseLinks[0];

				if (!coxa.isObject()) {
					std::cerr << "Unable to parse JSON configuration for HexapodLegZYY for coxa of" << name << std::endl;
					return -1;
				}

				JsonValueInterface coxaLink = coxa["link"];
				if (coxaLink.isArray() && coxaLink.size() == 3) {
					for (unsigned int j = 0; j < 3; j++) {
						link(j) = coxaLink[j].asDouble();
					}
					setCoxaDimensions(&link);
				}

//				setDoubleValueIfSafe(&(angles(0)), coxa["startAngle"]);
				double startAngle;
				if(setDoubleValueIfSafe(&startAngle, coxa["startAngle"])) {
					joints[0]->setValue(startAngle);
				}

				//		if (dynamixelHandler != NULL) {
				//			//						dynamixelID.
				//			//						setIntValueIfSafe(&(dynamixelID.at(i * 3 + 1)), coxa["motorID"]);
				//			if (coxa["motorID"].isInt()) {
				//				dynamixelID[i * 3 + 1] = coxa["motorID"].asInt();
				//				// dyn[i*3+1]->set_byte(REG_ID, coxa["motorID"].asInt());	// TODO:
				//				// change the indexing of the motors to a std::map maybe
				//			}
				//			if (coxa["motorReverse"].isBool()
				//				&& coxa["motorCenter"].isDouble()) {
				//				dyn[dynamixelID[i * 3 + 1]]->setCalibration(
				//															coxa["motorCenter"].asDouble(),
				//															coxa["motorReverse"].asBool());
				//			}
				//		}

				JsonValueInterface coxaLinks = coxa["links"];

				if (!coxaLinks.isArray() || coxaLinks.size() != 1) {
					std::cerr << "Unable to parse JSON configuration for HexapodLegZYY for coxa links of" << name << std::endl;
					return -1;
				}

				JsonValueInterface femur = coxaLinks[0];

				if (!femur.isObject()) {
					std::cerr << "Unable to parse JSON configuration for HexapodLegZYY for femur of" << name << std::endl;
					return -1;
				}

				JsonValueInterface femurLink = femur["link"];
				if (femurLink.isArray() && femurLink.size() == 3) {
					for (unsigned int j = 0; j < 3; j++) {
						link(j) = femurLink[j].asDouble();
					}
					setFemurDimensions(&link);
				}

//				setDoubleValueIfSafe(&(angles(1)), femur["startAngle"]);	//TODO: more moving:
				if(setDoubleValueIfSafe(&startAngle, femur["startAngle"])) {
					joints[1]->setValue(startAngle);
				}
				//		//					if (femur["startAngle"].isDouble()) {
				//		//      legAngles[i](1) = femur["startAngle"].asDouble();
				//		//					}
				//		if (dynamixelHandler != NULL) {
				//			if (femur["motorID"].isInt()) {
				//				//					dyn[i*3+2]->set_byte(REG_ID,
				//				//femur["motorID"].asInt());
				//				dynamixelID[i * 3 + 2] = femur["motorID"].asInt();
				//			}
				//			if (femur["motorReverse"].isBool()
				//				&& femur["motorCenter"].isDouble()) {
				//				dyn[dynamixelID[i * 3 + 2]]->setCalibration(
				//															femur["motorCenter"].asDouble(),
				//															femur["motorReverse"].asBool());
				//			}
				//		}

				JsonValueInterface femurLinks = femur["links"];

				if (!femurLinks.isArray() || femurLinks.size() != 1) {
					std::cerr << "Unable to parse JSON configuration for HexapodLegZYY for femur links of" << name << std::endl;
					return -1;
				}

				JsonValueInterface tibia = femurLinks[0];

				if (!tibia.isObject()) {
					std::cerr << "Unable to parse JSON configuration for HexapodLegZYY for tibia of" << name << std::endl;
					return -1;
				}

				JsonValueInterface tibiaLink = tibia["link"];
				if (tibiaLink.isArray() && tibiaLink.size() == 3) {
					for (unsigned int j = 0; j < 3; j++) {
						link(j) = tibiaLink[j].asDouble();
					}
					setTibiaDimensions(&link);
				}
				
//				setDoubleValueIfSafe(&(angles(2)), tibia["startAngle"]);	// TODO: more crap to move
				if(setDoubleValueIfSafe(&startAngle, tibia["startAngle"])) {
					joints[2]->setValue(startAngle);
				}
				//		//					if (tibia["startAngle"].isDouble()) {
				//		//      legAngles[i](2) = tibia["startAngle"].asDouble();
				//		//					}
				//		if (dynamixelHandler != NULL) {
				//			if (tibia["motorID"].isInt()) {
				//				//					dyn[i*3+3]->set_byte(REG_ID,
				//				//tibia["motorID"].asInt());
				//				dynamixelID[i * 3 + 3] = tibia["motorID"].asInt();
				//			}
				//			if (tibia["motorReverse"].isBool()
				//				&& tibia["motorCenter"].isDouble()) {
				//				dyn[dynamixelID[i * 3 + 3]]->setCalibration(
				//															tibia["motorCenter"].asDouble(),
				//															tibia["motorReverse"].asBool());
				//			}
				//		}
				
				return 0;
			}

		}
	}
#ifdef _cplusplus
}
#endif
