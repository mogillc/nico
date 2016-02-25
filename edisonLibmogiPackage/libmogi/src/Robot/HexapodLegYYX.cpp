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


			HexapodLegYYX::HexapodLegYYX() {
//				std::cout << "HexapodLeg::HexapodLeg() type:" << abi::__cxa_demangle(typeid(*this).name(), 0, 0, NULL) << std::endl;
				ikSolver = InverseKinematics::create(InverseKinematics::YYX);
			}
			;

			HexapodLegYYX::~HexapodLegYYX() {
				delete ikSolver;
			}

			void HexapodLegYYX::buildNodeStructure(Node* root) {
				Bot::buildNodeStructure(root);
				static unsigned int ID = 0;
				Vector tempLink(3);

				std::stringstream tempName;
				tempName << "HexapodYYXLegBase_" << ID;
				nodeLocation = root->addNode(tempName.str());
				tempName.str("");
				tempName << "coxa_" << ID << std::ends;

				Node* coxa = nodeLocation->addNode(tempName.str());
				joints.push_back(Joint::create(coxa, Vector::yAxis, Joint::REVOLUTE));
				tempName.str("");
				tempName << "femur_" << ID << std::ends;

				Node* femur = coxa->addNode(tempName.str());
				joints.push_back(Joint::create(femur, Vector::yAxis, Joint::REVOLUTE));
				tempName.str("");
				tempName << "tibia_" << ID << std::ends;

				Node* tibia = femur->addNode(tempName.str());
				joints.push_back(Joint::create(tibia, Vector::xAxis, Joint::REVOLUTE));
				tempName.str("");
				tempName << "foot_" << ID << std::ends;
				// Node *foot = tibia->addNode(tempName.str());
				tibia->addNode(tempName.str());  // Node for the foot
				
				ID++;
			}
		}
	}
#ifdef _cplusplus
}
#endif
