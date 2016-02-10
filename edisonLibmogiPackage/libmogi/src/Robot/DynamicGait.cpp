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
#include <sstream>
#include <algorithm>

#ifdef IDENT_C
static const char* const RigidGait_C_Id = "$Id$";
#endif

#ifdef _cplusplus
extern "C" {
#endif

	namespace Mogi {
		//using namespace Dynamixel;
		using namespace Math;
		using namespace StateChart;

		namespace Robot {

			DynamicGait::DynamicGait() {
				timeDifferenceForPerform = 0;
				steppingTime = 0.6;
				pivotRadius = 30;
				userControlFactor = 1.0;

				for (int i = 0; i < footLocations.size(); i++) {
					// basic construction:
					footTimers.push_back((double)i * steppingTime * 6);
					steppingPriorities.push_back(double());

					// Callback parameters for this leg:
					CallbackParameter* callbackParameter = new CallbackParameter(this, i);
					callbackParameters.push_back(callbackParameter);

					// StateChart Diagram:
					std::stringstream name("");
					name << i;
					footStateCharts[&footLocations[i]] = new StateChart::Diagram( name.str() );

					// States:
					State* supporting = footStateCharts[&footLocations[i]]->addState(new StateChart::State("supporting"));
					State* moving = footStateCharts[&footLocations[i]]->addState(new StateChart::State("moving"));

					// State Actions:
					moving->setCallbackDo(performTrajectory, callbackParameter);

					// Transitions:
					footStateCharts[&footLocations[i]]->getInitialState()->addTransition(supporting);
					Transition *movingToSupporting = moving->addTransition(supporting);
					Transition *supportingToMoving = supporting->addTransition(moving);

					// Transition Actions:
					supportingToMoving->setCallbackAction(determineFootDestination, callbackParameter);

					// Guards:
					Guard* bodyInHullGuard = supportingToMoving->addGuard(new StateChart::Guard);
					bodyInHullGuard->setGuardCallback(bodyInHullIgnoringFoot, callbackParameter);	// We can use the same parameter.

					Guard* footOnGroundGuard = movingToSupporting->addGuard(new StateChart::Guard);
					footOnGroundGuard->setGuardCallback(footOnGround, callbackParameter);	// We can use the same parameter.

					// Transition events:
					supportingToMovingEvent[&footLocations[i]] = new StateChart::Event;
					supportingToMoving->setEvent(supportingToMovingEvent[&footLocations[i]]);

					footStateCharts[&footLocations[i]]->update();
				}
			}

			DynamicGait::~DynamicGait() {
				for (std::vector<CallbackParameter*>::iterator it = callbackParameters.begin(); it != callbackParameters.end(); it++) {
					delete (*it);
				}

				for (std::map<Math::Vector*, StateChart::Diagram*>::iterator it = footStateCharts.begin(); it != footStateCharts.end(); it++) {
					delete it->second;
				}

				for (std::map<Math::Vector*, StateChart::Event*>::iterator it = supportingToMovingEvent.begin(); it != supportingToMovingEvent.end(); it++) {
					delete it->second;
				}
			}

			bool DynamicGait::bodyInHullIgnoringFoot(  void* callbackParameter ) {
				//std::cout << "In DynamicGait::bodyInHullIgnoringFoot()" << std::endl;
				DynamicGait* This = ((CallbackParameter*)callbackParameter)->This;
				Math::Vector* footLocationToIgnore = &(This->footLocations[((CallbackParameter*)callbackParameter)->footIndex]);

				std::vector<Vector *> input;
				for (std::map<Vector*, StateChart::Diagram*>::iterator it = This->footStateCharts.begin(); it != This->footStateCharts.end(); it++) {
					if (it->first != footLocationToIgnore &&	// Obviously ignore this foot, as it may become unsupporting.
						it->second->getCurrentState()->name().compare("supporting") == 0) {	// Only consider supporting feet.
						input.push_back(it->first);
					}
				}
				//std::cout << " - foot location vector size: " << input.size() << std::endl;
				if (input.size() < 3) {
					return false;
				}
				This->zmpInHullChecker.setInput(input);
				This->zmpInHullChecker.getConvexHull();

				return This->zmpInHullChecker.inHull( This->bodyLocation );
			}

			bool DynamicGait::footOnGround( void* callbackParameter ) {
				DynamicGait* This = ((CallbackParameter*)callbackParameter)->This;
				int footIndex = ((CallbackParameter*)callbackParameter)->footIndex;

				//std::cout << "In DynamicGait::footOnGround() for " << This->footStateCharts[&(This->footLocations[footIndex])]->name() << std::endl;

				return This->footLocations[footIndex].value(2, 0) == 0;	// very simple foot on ground test.
			}

			void DynamicGait::determineFootDestination( void* callbackParameter ) {
				DynamicGait* This = ((CallbackParameter*)callbackParameter)->This;
				int footIndex = ((CallbackParameter*)callbackParameter)->footIndex;

				This->footTimers[footIndex] = 0;
			}

			void DynamicGait::performTrajectory( void* callbackParameter ) {
				DynamicGait* This = ((CallbackParameter*)callbackParameter)->This;
				int footIndex = ((CallbackParameter*)callbackParameter)->footIndex;

				// Where we should step is proportional to pivot velocity and the stepping time, in the direction of the pivot velocity.
				// If beyond the safe region, we limit it.
				Vector locationToStepRelativeToPivot = This->footPivotVelocity[footIndex] * This->steppingTime;// 1.0/60.0 * 30.0 * 1.5;
				double magnitudeStep = locationToStepRelativeToPivot.magnitude();
				if (magnitudeStep > This->pivotRadius) {
					locationToStepRelativeToPivot.normalize();
					locationToStepRelativeToPivot *= This->pivotRadius;
				}
				Vector footDestinationForWalking = This->footPivotForWalking[footIndex] + locationToStepRelativeToPivot;

				// Determine the time component based on the defined stepping time
				double time = This->footTimers[footIndex] / This->steppingTime;
				if(time > 1) {
					This->footLocations[footIndex](2) = 0;
					return;
				}

				// This determines the lifting height and the amount of horizontal movement.
				// If all else is held constant, the trajectory is a semicircle.
				double liftFactor = cos(MOGI_PI*(time - 0.5));

				// The faster we are controlled, the faster we should push the leg to the destination.
				// TODO: Maybe, it would be nice to know how many legs are being stepped here
				double pivotVelocityMagnitude = This->footPivotVelocity[footIndex].magnitude() * 6.0/This->steppingTime;
				if (pivotVelocityMagnitude < 50) {
					pivotVelocityMagnitude = 50;
				}
				Vector vectorToMove = (footDestinationForWalking - This->footLocations[footIndex]);
				vectorToMove(2) = 0;
				double magnitudeOfMove = vectorToMove.magnitude();
				if ( magnitudeOfMove > pivotVelocityMagnitude * This->timeDifferenceForPerform * liftFactor) {	// velocity limited motion
					vectorToMove.normalize();
					vectorToMove *= pivotVelocityMagnitude * This->timeDifferenceForPerform * liftFactor;
				}
				This->footLocations[footIndex] += vectorToMove;

				// we don't care about blending the height, just use the result from the
				// trajectory function:
				double footHeight = This->footPivotVelocity[footIndex].magnitude() * 2.0;
				if (footHeight > This->maximumFootLiftHeight) {
					footHeight = This->maximumFootLiftHeight;
				}
				if (footHeight < This->minimumFootLiftHeight) {
					footHeight = This->minimumFootLiftHeight;
				}
				This->footLocations[footIndex](2) = footHeight * liftFactor;
			}

			void DynamicGait::setControlFactor(double controlFactor) {
				userControlFactor = controlFactor > 0 ? controlFactor : 0;
			}

			void DynamicGait::perform(const double& dTime) {
				timeDifferenceForPerform = dTime;	// needed for performTrajectory

				// Update velocities and time component:
				for (unsigned int i = 0; i < footLocations.size(); i++) {
					footTimers[i] += timeDifferenceForPerform; // Increment the index, based on the loop time and
				}

				// This factor determines how hard the user is driving the hexapod.
				// For proper scaling, remove the effective radius factor and make it the average of the 6 legs.
				double globalControlFactor = 0;
				for (int i = 0; i < 6; i++) {
					globalControlFactor += footPivotVelocity[i].magnitude()/(pivotRadius * 6); //200.0;
				}
				globalControlFactor = globalControlFactor*2 + 1;

				// The faster we move, the faster we should step
				// For Nico, the control factor may drift
				steppingTime = 0.9 / log(userControlFactor+globalControlFactor);

				// Time to decide which feet should be triggered a step motion:
				for (int i = 0; i < 3; i++) {	// Being a hexapod, the maximum number of triggers we may have is 3
					std::map<double, Vector* > priorityToFootMap;	// used for sorting which should happen first

					// Here we will prioritize all legs by applying a set of stepping rules.
					// Some of this work is inspired by Randall Beer's hexapod neural network.
					// http://www.mitpressjournals.org/doi/abs/10.1162/neco.1992.4.3.356#.VrTPn8cqT-Q
					// Currently these are arbitrariy chosen weightings to create the priority.
					// Priorities are assigned in a way that only values >1 are considered.
					for (unsigned int i = 0; i < footLocations.size(); i++) {

						if (footStateCharts[&footLocations[i]]->getCurrentState()->name().compare("supporting") != 0) {
							continue;	// No need to rigeer the event, nor compute the trigger stuff
						}

						// Rule 1) Excitation based on leg strain, i.e. when far from the resting (pivot) location.
						double priority = ((const Vector)(footLocations[i] - footPivotForWalking[i])).magnitude();	// The higher the magnitude, the more likely it will step
						priority *= 1.0/(pivotRadius * 0.5);	// Normalization of the magnitude to half the radius
						priority = square( priority );			// square it to both reduce stepping when close but highly request it when far.
						priority = priority < 0.1 ? 0.1 : priority;

						// Rule 2) The bio-inspired aspect with CPGs from Beer's work, an inhibitory network.
						//         A leg stepping inhibits the neighboring legs and laterally located legs.
						//         TODO: varying inhibitory weights
						priority *= footStateCharts[&footLocations[(i + 1) % 6]]->getCurrentState()->name().compare("supporting") != 0 ? 0.1 : 1.0;
						priority *= footStateCharts[&footLocations[(i + 5) % 6]]->getCurrentState()->name().compare("supporting") != 0 ? 0.1 : 1.0;
						if (i == 0 || i == 3) {	// The ony case where the above doesn't handle latteral inhibition:
							priority *= footStateCharts[&footLocations[(i+3) % 6]]->getCurrentState()->name().compare("supporting") != 0 ? 0.2 : 1.0;
						}

						// Rule 3) We want feet to want to be on the ground when other feet are raised:
						for(std::map<Vector*,StateChart::Diagram*>::iterator it = footStateCharts.begin(); it != footStateCharts.end(); it++)
						{
							if (it->first != &footLocations[i] &&	// don't compare ourself
								it->second->getCurrentState()->name().compare("supporting") != 0 ) {
								priority *= 0.5;
							}
						}

						// Rule 4) If recently stepped, we should wait a bit before continuing.
						//         This help create self-oscillation, i.e. a CPG
						priority *= footTimers[i]/(2*6*steppingTime);	// The more recent the lift, the less likely it will be lifted.

						// Rule 5) The faster we drive it, the sooner feet should step.
						//         This value is always greater than 1.
						priority *= globalControlFactor;

						// Take into account the user's control amount:
						priority *= userControlFactor;

						steppingPriorities[i] = priority;	// Save for debugging/visualization.

						if (priority > 1.0) {	// Only consider taking a step if the priority is high enough.
							priorityToFootMap[priority] = &footLocations[i];
						}
					}

					// std::maps are automatically sorted byt he key, so we just take the highest priority leg and trigger a step.
					if (priorityToFootMap.size() > 0) {
						supportingToMovingEvent[priorityToFootMap.rbegin()->second]->trigger(); // rbegin signifise highest priority.
					} else {
						break;
					}
				}

				for(std::map<Vector*,StateChart::Diagram*>::iterator it = footStateCharts.begin(); it != footStateCharts.end(); it++)
				{
					it->second->update();	// If moving, will call the performTrajectory method
				}
				

			}
			
		}
	}
	
#ifdef _cplusplus
}
#endif
