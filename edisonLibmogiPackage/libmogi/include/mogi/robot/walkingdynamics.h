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

#ifndef MOGI_WALKINGDYNAMICS_H
#define MOGI_WALKINGDYNAMICS_H

#include "mogi/math/mmath.h"
#include "mogi/math/algorithm.h"
#include "mogi/statechart/statechart.h"
#include "mogi/math/systems.h"

namespace Mogi {

	/**
	 * @namespace Mogi::Robot
	 * \brief Handles robots.
	 */
	namespace Robot {

		/*! \class WalkingDynamics
		 \brief An abstract class to operate walking dynamics.

		 Each foot begins a stepping sequence as defined by a set of gait parameters.
		 The stepping location of each foot is determined by a current pivot point,
		 represting the resting location of the foot relative to the hexapod.
		 The further the foot is from the pivot point, the further the foot will step
		 beyond the pivot location, in a dampened oscillator format.
		 \image html walkingDynamics.jpg
		 */
		class WalkingDynamics {
		private:
			/*! \brief The equilibrium location of the foot relative to the foot.

			 This represents the resting position of each foot, i.e. representative of 0 strech.
			 */
			std::vector<Math::Vector> footPivotRelativeToBody;

			std::vector<Math::Vector> footPivotPriorLocation;

			/*! \brief Computes the gobal position of the pivots from the body location, heading, and relative pivot locations.

			 The resulting positions are represented in the footPivotForWalking vector.
			 */
			void computeGlobalPivotLocations(const double& dTime);

		protected:

			/*! \brief The velocity of the pivot computed fromt he previous perform() call.
			 */
			std::vector<Math::Vector> footPivotVelocity;

			/*! \brief Used as an output for foot locations.
			 */
			std::vector<Math::Vector> footLocations;

			/*! \brief The body's center location.
			 */
			Math::Vector bodyLocation;

			/*! \brief The body's heading.
			 */
			double bodyHeading;

			/*! \brief Minimum lift height when performing the step.
			 */
			double minimumFootLiftHeight;

			/*! \brief Maximum foot height when stepping.
			 */
			double maximumFootLiftHeight;

			/*! \brief The radius around the pivot that is an acceptable region.
			 */
			double pivotRadius;

			/*! \brief The equilibrium location of the foot.
			 
			 This represents the resting position of each foot, i.e. representative of 0 strech.
			 */
			std::vector<Math::Vector> footPivotForWalking;

			WalkingDynamics();

		public:

			virtual ~WalkingDynamics();
			
			/*! \brief Updates the internal state and foot locations.
			 \param dTime The time step between calls.
			 */
			virtual void perform(const double& dTime) = 0;
			
			/*! \brief Updates the pivot location used for determining foot trajectories.

			 This should be performed before the first perform() call to initialize everything.
			 \param footIndex The index of the corresponding leg.
			 \param pivot The new pivot location.
			 */
			void setPivot(unsigned int footIndex, const Math::Vector& pivot);

			/*! \brief Sets the body center location and heading.
			 \param location The body location.
			 \param heading The body heading in radians.
			 \param timeDifference The amount of time since the previous call of this method.
			 */
			void setBody(const Math::Vector& location, const double& heading, const double& timeDifference);

			/*! \brief Sets the lift height limits to ensure a minimum and maximum lift height while stepping.
			 \param minHeight The minimimum foot lift height.
			 \param maxHeight The maximum foot lift height.
			 */
			void setHeightBounds(double minHeight, double maxHeight);

			/*! \brief Gets the current location of the feet.
			 \return The current locations of the feet from the last perform() computation.
			 */
			const std::vector<Math::Vector>& getFootLocations();

			/*! \brief Gets the current location of the foot pivots.
			 \return The current locations of the foot pivots from the last setPivot() call.
			 */
			const std::vector<Math::Vector>& getFootPivots();

			/*! \brief Gets the current location of the center of gravity.
				\return The current locations of the foot pivots from the last setPivot() call.
			 */
			const Math::Vector& getBodyLocation();

			/*! \brief Gets the current heading of the body.
				\return The heading in radians.
			 */
			const double& getBodyHeading();

			/*! \brief Gets the acceptable radius around each pivot.
				\return The pivot region radius.
			 */
			const double& getPivotRadius();

		};

		/*! \class RigidGait
		 \brief An example implementation of walking dynamics using rigid timing for the gait.

		 Each foot begins a stepping sequence as defined by a set of gait parameters.
		 The stepping location of each foot is determined by a current pivot point,
		 represting the resting location of the foot relative to the hexapod.
		 The further the foot is from the pivot point, the further the foot will step
		 beyond the pivot location, in a dampened oscillator format.
		 \image html walkingDynamics.jpg
		 */
		class RigidGait : public WalkingDynamics {
		private:
			double footTimeForTrajectoryFunctionsForWalking;
			double steppingTime;
			double phase[6];
			double switchpoint;
			std::vector<Math::Vector> footDestinationForWalking;
			std::vector<Math::Vector> footLastLocationOnGroundForWalking;

		public:

			RigidGait();

			/*! \brief Updates the internal state and foot locations.
			 \param dTime The time step between calls.
			 */
			void perform(const double& dTime);

			/*! \brief This modifies the gait parameters for various gait types.

			 The parameters are then used in walkingDynamics.
			 Modifies the gait parameters based on desired gait.
			 \image html gaitSequences.jpg
			 \param gait The desired gait index to be set.  Possible values include 1, 3, 4, or 6 for gaits of type ripple, tripod, quad, or wave, respectively
			 */
			void setGait(int gait);

		};

		/*! \class DynamicGait
		 \brief An example implementation of walking dynamics using statecharts in a bio-inspired fashion.

		 */
		class DynamicGait : public WalkingDynamics {
		private:
			struct CallbackParameter {
				DynamicGait* This;
				int footIndex;
				CallbackParameter(DynamicGait* This, int footIndex)
				:This(This), footIndex(footIndex) {};
			};

			/*! \brief The time difference between body location/heading updates.
			 */
			double timeDifferenceForPerform;
			double userControlFactor;

			std::vector<double> footTimers; // For keeping track of trajectory timing.
			double steppingTime;
			std::vector<CallbackParameter*> callbackParameters;

			// Each foot is either in the state of supporting the weight or is moving/externally controlled
			// Therefore, each foot has it's own simple statechart.
			std::map<Math::Vector*, StateChart::Diagram*> footStateCharts;
			std::map<Math::Vector*, StateChart::Event*> supportingToMovingEvent;

			// For checking if the center of gravity in the convex hull of the supporting foot locations.
			Math::GrahamScan zmpInHullChecker;

			// Returns true if foot is safe to be lifted.
			static bool bodyInHullIgnoringFoot( void* callbackParameter );

			// returns true if the foot is on the ground.
			static bool footOnGround( void* callbackParameter );

			// Action to determine the foot destination.
			static void determineFootDestination( void* callbackParameter );

			// Performs the trajectory when in the moving state.
			static void performTrajectory( void* callbackParameter );

		public:

			/*! \brief Used for determining when to be stepped.
			 */
			std::vector<double> steppingPriorities;

			DynamicGait();
			~DynamicGait();

			/*! \brief Updates the internal state and foot locations.
			 \param dTime The time step between calls.
			 */
			void perform(const double& dTime);

			/*! \brief Sets a drive factor for the stepping.
			 
			 Each stepping decision is based on a set of priorities from a set of rules.  Setting this
			 value high, greater than 1 will cause the legs to be more likely to take a step.  Setting
			 this value small 0 < controlFactor < 1 will cause stepping to be delayed a long time. For
			 small values, the gait will approach a ripple gait.  For large values, the gait will
			 approach a tripod gait.  The default value is 1.
			 \param controlFactor The priority multiplication factor, must be greater than 0.
			 */
			void setControlFactor(double controlFactor);
		};


		/*! \brief This computes the 2D values associated with a foot trajectory.

		 This is similar to following a circle using x = cos(-in) and y = sin(-in), however it is
		 modified so that the y value is set to 0 if negative.  Also, while y is forced
		 to 0, the x value becomes a linear function of in.  Finally, once
		 in==switchpoint, the mode switchs from linear mode to circular mode.  This
		 function is periodic about 2*pi, so switchpoint must be between 0 and 2*pi.
		 Computes the values associated with a half-circle style function.
		 \image html footTrajectory.jpg
		 @param outx Six output values in the x direction.  These values could correspond to forward/backward distance
		 @param outy Six output values in the y direction.  These values could correspond to height
		 @param upflag Six values denoting when the y value is greater than 0
		 @param input Six inputs to the function
		 @param switchpoint Number of channels to be allocated
		 */
		void footTrajectory(double* outx, double* outy, bool* upflag, double input, double switchpoint);
		
		
	}
}

#endif
