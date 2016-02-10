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

#include "walkingdynamics.h"

#include <math.h>
#include <iostream>

#ifdef IDENT_C
static const char* const RigidGait_C_Id = "$Id$";
#endif

#ifdef _cplusplus
extern "C" {
#endif

	namespace Mogi {
		//using namespace Dynamixel;
		using namespace Math;

		namespace Robot {

			WalkingDynamics::WalkingDynamics()
			: bodyHeading(0) {
				for (int i = 0; i < 6; i++) {
					footPivotRelativeToBody.push_back(Vector(3));	// TODO: make this vector size 2
					footPivotPriorLocation.push_back(Vector(3));	// TODO: make this vector size 2
					footPivotVelocity.push_back(Vector(3));	// TODO: make this vector size 2
					footLocations.push_back(Vector(3));
					footPivotForWalking.push_back(Vector(3));	// TODO: make this vector size 2
				}

				bodyLocation.setLength(3);	// TODO: make this vector size 2
			}

			WalkingDynamics::~WalkingDynamics() {

			}

			void WalkingDynamics::computeGlobalPivotLocations(const double& dTime) {
				static bool first_time = true;

				// First let's take a look at
				for (int i = 0; i < footPivotRelativeToBody.size(); i++) {
					double cosP = cos(bodyHeading);
					double sinP = sin(bodyHeading);
					Vector* footPivot = &footPivotRelativeToBody[i];

					footPivotForWalking[i](0) = bodyLocation(0) + footPivot->value(0, 0) * cosP - sinP * footPivot->value(1, 0);
					footPivotForWalking[i](1) = bodyLocation(1) + footPivot->value(0, 0) * sinP + cosP * footPivot->value(1, 0);
					
					footPivotForWalking[i](2) = 0;
				}

				if (first_time)  // Initialize velocity measurement stuff
				{
					for (unsigned int i = 0; i < footLocations.size(); i++) {
						footPivotPriorLocation[i] = footPivotForWalking[i];

						// New initialization:
						footLocations[i] = footPivotForWalking[i];
					}
					first_time = false;
				}

				// Update velocities and time component:
				for (unsigned int i = 0; i < footLocations.size(); i++) {
					if (dTime > 0) {
						footPivotVelocity[i] = (footPivotForWalking[i] - footPivotPriorLocation[i]) / dTime;
					}
					footPivotPriorLocation[i] = footPivotForWalking[i];
				}
			}

			void WalkingDynamics::setPivot(unsigned int footIndex, const Math::Vector& pivot) {
				footPivotRelativeToBody[footIndex] = pivot;
			}

			void WalkingDynamics::setBody(const Math::Vector& location, const double& heading, const double& timeDifference) {	// TODO: place checkes in here based on the radius of the foot.
				bodyLocation = location;
				bodyHeading = heading;

				computeGlobalPivotLocations(timeDifference);

				perform(timeDifference);
			}

			void WalkingDynamics::setHeightBounds(double minHeight, double maxHeight) {
				minimumFootLiftHeight = minHeight > 0 ? minHeight : 0;
				maximumFootLiftHeight = maxHeight > minHeight ? maxHeight : minHeight;
			}

			const std::vector<Math::Vector>& WalkingDynamics::getFootLocations() {
				return footLocations;
			}

			const std::vector<Math::Vector>& WalkingDynamics::getFootPivots() {
				return footPivotForWalking;
			}

			const Math::Vector& WalkingDynamics::getBodyLocation() {
				return bodyLocation;
			}

			const double& WalkingDynamics::getBodyHeading() {
				return bodyHeading;
			}

			const double& WalkingDynamics::getPivotRadius() {
				return pivotRadius;
			}

		}
	}
	
#ifdef _cplusplus
}
#endif
