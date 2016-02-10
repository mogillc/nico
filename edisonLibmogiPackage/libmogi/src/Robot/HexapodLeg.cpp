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
#include <cxxabi.h>
#include <sstream>

#ifdef _cplusplus
extern "C" {
#endif

	namespace Mogi {
		using namespace Math;

		namespace Robot {

			HexapodLeg::HexapodLeg() {
				std::cout << "HexapodLeg::HexapodLeg() type:" << abi::__cxa_demangle(typeid(*this).name(), 0, 0, NULL) << std::endl;
			}

			HexapodLeg::~HexapodLeg() {
			}

			const Vector& HexapodLeg::computeAngles(Vector& location) {
				// Extract the base location and orientation from the node model matrix:
				Matrix legBaseTransormationMatrix = *nodeLocation->getModelMatrix();
				Matrix legBaseTranslationMatrix;
				Matrix legBaseOrientationMatrix;

				if(transformationToScaleRotationTranslation(&legBaseTransormationMatrix,
															NULL,
															&legBaseOrientationMatrix,
															&legBaseTranslationMatrix) == -1) {
					std::cerr << "HexapodLeg::computeAngles(): Bad base configuration for node " << nodeLocation->name << std::endl;
//					std::cerr << " - parent: " << nodeLocation->getParent()->name << std::endl;
//					legBaseTransormationMatrix.print();
//					nodeLocation->getParent()->getModelMatrix()->print();
					return ikSolver->getMotorAngles();
				}

				Vector legLocation(3);
				legLocation(0) = legBaseTranslationMatrix(0, 3);
				legLocation(1) = legBaseTranslationMatrix(1, 3);
				legLocation(2) = legBaseTranslationMatrix(2, 3);

				Quaternion legOrientation;
				legOrientation = legBaseOrientationMatrix.rotationToQuaternion();
				ikSolver->setBase(legLocation * MM_PER_METER, legOrientation);

				if (ikSolver->compute(location) != KINEMATICS_NO_ERROR) {
					// std::cout << "Leg " << nodeLocation->name << " can't reach!" <<
					// std::endl;
				}
				Vector result = ikSolver->getMotorAngles();
				
				setKinematics(result);
				
				return ikSolver->getMotorAngles();
			}

			void HexapodLeg::setBase(const Vector& location, const Quaternion& orientation) {
				nodeLocation->setLocation(location);
				nodeLocation->setOrientation(orientation);
				ikSolver->setBase(location, orientation);
			}

			int HexapodLeg::setNodeDimensions(unsigned int nodeNumber, const Vector* dimensions) {
				if (dimensions->size() != joints.size() || nodeNumber > joints.size()) {
					return -1;
				}

				Node* currentNode = nodeLocation->child(0);
				for (unsigned int i = 0; i < nodeNumber; i++) {
					currentNode = currentNode->child(0);
				}

				currentNode->setLocation(*dimensions);

				ikSolver->setLink(nodeNumber, *dimensions);

				return 0;
			}

			Node* HexapodLeg::getBaseNode() {
				return nodeLocation;
			}
#ifdef LIBJSONCPP_FOUND
			HexapodLeg* HexapodLeg::createFromJSON( Json::Value jsonConfiguration, Math::Node* root ) {
				if (!jsonConfiguration["type"].isString()) {
					return NULL;
				}
				std::string typeName = jsonConfiguration["type"].asString();
				HexapodLeg* result = NULL;

				if (typeName.compare("HexapodLegZYY") == 0) {
					result = new HexapodLegZYY;
					result->buildNodeStructure(root);
					if(result->setConfigurationFromJSONValue(jsonConfiguration)) {
						std::cerr << "Error: Unable to create ZYY leg from configuration." << std::endl;
						delete result;
						result = NULL;
					}
				} else if (typeName.compare("HexapodLegYYX") == 0) {
					result = new HexapodLegYYX;
					result->buildNodeStructure(root);
					if(result->setConfigurationFromJSONValue(jsonConfiguration)) {
						std::cerr << "Error: Unable to create YYX leg from configuration." << std::endl;
						delete result;
						result = NULL;
					}
				}

				std::cout << "HexapodLeg::createFromJSON() loaded:" << abi::__cxa_demangle(typeid(*result).name(), 0, 0, NULL) << std::endl;
				
				return result;
			}
#endif
		}
	}

#ifdef _cplusplus
}
#endif
