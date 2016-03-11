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

#ifndef MOGI_HEXAPOD_H
#define MOGI_HEXAPOD_H

#include <map>
#include <string>

#include "mogi/app/appcomm.h"
#include "mogi/dynamixel/dynamixel.h"
#include "mogi/math/systems.h"
#include "mogi/statechart/statechart.h"

#include "mogi/robot/leg.h"
#include "mogi/robot/walkingdynamics.h"

namespace Mogi {

	/**
	 * @namespace Mogi::Robot
	 * \brief Handles robots.
	 */
	namespace Robot {

		/*! \class DmitriHeadMechanism
		 \brief This is a very specific example class that handles the very specific head mechanism on the Dmitri design.
		 */
		class DmitriHeadMechanism {
		private:
			double pitch, roll, yaw;  // These are arbitrary values in terms of the
			// hexapod, but can control accessory servos
			double pitchzero, rollzero, yawzero;
			double gear_ratio;  // different versions have different gear ratios
			double motor_center;

			/*! \brief This function computes the pitch motor angle from pitch angle of the head.
			 Because of the strange mechanism of the head, the pitch angle requires goofy
			 kinematics computation to produce the required motor angle to produce the
			 desired pitch.
			 @param thetah The head pitch angle
			 @return Returns the motor angles
			 */
			double goofytrig(double thetah); // function to operate funky linkage geometry (degrees)
		public:
			DmitriHeadMechanism();

			/*! \brief Sets the head angles.
			 This sets the pitch, roll, and yaw (x, y, and z, respectively) of the head.
			 @param vector A 3-vector of the x, y and z rotation of the body
			 */
			void setAngles(const Math::Vector& vector);
			/*! \brief Sets the head angles.
			 This sets the pitch, roll, and yaw (x, y, and z, respectively) of the head.
			 @param x X-rotation (pitch)
			 @param y Y-rotation (roll)
			 @param z Z-rotation (yaw)
			 */
			void setAngles(double x, double y, double z);

			/*! \brief Gets the motor angles from the last computation.
			 \return The angles of all the motor angles.
			 */
			Math::Vector getAngles();

			/*! \brief Sets the calibration for the head mechanism.
			 \param gearRatio The gear ratio at the motor.
			 \param servoCenter The motor angle offset.
			 */
			void setCalibration(double gearRatio, double servoCenter);

			/*! \brief Processes head angles and produces motor angles.
			 This should be called to compute the motor angles based on the set desired
			 head angles.
			 \return The motor angle result.
			 */
			Math::Vector computeAngles();
		};

		/*!
		 @class Hexapod
		 \brief An example of a hexapod implementation.

		 The hexapod class handles hexapod operation.  This includes the
		 reading of calibration files, walking dynamics, balance gestures,
		 and kinematics construction.  Though this class may operate most
		 common configurations, it should be thought of as an example
		 implementation of Mogi::Math, Mogi::Dynamixel, and Mogi::StateChart
		 utilities into a robotics application.

		 An Hexapod object implements a Mogi::StateChart::Diagram.  This is based on the
		 following design:
		 \image html HexapodState.png
		 @since 2013-04-22
		 */
		class Hexapod: public Bot, private Mogi::Thread  // Defines all positions
		{
		private:

			Mogi::Math::LowPassFilter<Mogi::Math::Vector> balanceGesturesLocationFilter;
			Mogi::Math::LowPassFilter<Mogi::Math::Quaternion> balanceGesturesOrientationFilter;

			WalkingDynamics* mWalkingDynamics;

			/*! \brief Provides an internal indexing to arbitrarily chosen motor IDs, defined int he JSON files.
			 */
			std::map<unsigned char, unsigned char> dynamixelID;

			/*! \brief The final output body location.

			 The final vector is computed as  \f$\vec{v}_f = \vec{v}_b + \sum\limits_{n} \Delta\vec{v}_n\f$
			 */
			Math::Vector bodyLocationTotal;
			/*! \brief The final output body location offset.

			 This is the sum of all body location offsets \f$\sum\limits_{n} \Delta\vec{v}_n\f$
			 */
			Math::Vector bodyLocationOffsetTotal;

			/*! \brief The body location offset, as set by the user.
			 */
			Math::Vector bodyLocationOffsetUser;

			/*! \brief The offset of the body, used as a standing height offset.

			 This is initially 0, then increases to standingHeight during power up, then decreases to 0 on powerDown.
			 \sa bodyLocation.
			 */
			Math::Vector bodyLocationOffsetStanding;

			/*! \brief The additional offset of the body as provided by balance gesture computations.
			 \sa bodyLocation.
			 */
			Math::Vector bodyLocationOffsetBalanceGestures;

			/*! \brief The main body location, used for computing relative coordinates.

			 This is the base location, ignoring all offsets.  This is used for relative coordinate computation and pivot point updating.
			 \sa bodyOrientation.
			 */
			Math::Vector bodyLocation;

			/*! \brief The body orientation, used for computing relative coordinates.

			 This is the base orientation, ignoring all offsets.  This is used for relative coordinate computation and pivot point updating.
			 \sa bodyLocation.
			 */
			Math::Quaternion bodyOrientation;

			/*! \brief The body orientation offset, externally set.
			 */
			Math::Quaternion bodyOrientationOffset;

			/*! \brief The desired body orientation, for walking dynamics.
			 */
			//Math::Quaternion bodyOrientationDesired;

			/*! \brief The body orientation offset, computed from balance gestures.
			 */
			Math::Quaternion bodyOrientationOffsetBalanceGesture;

			/*! \brief The final output of the body orientation and all offsets.
			 *
			 * The final orientation is computed as  \f$q_f = (\prod\limits_{n} \Delta q_n) * q_b\f$
			 */
			Math::Quaternion bodyOrientationTotal;

			/*! \brief The final body orientation offset from all offsets.
			 */
			Math::Quaternion bodyOrientationOffsetTotal;

			/*! \brief The body orientation offset defined by the user.
			 */
			Math::Quaternion bodyOrientationOffsetUser;

			/*! \brief The location of the coxa with respect to the body, used for balance gestures.
			 */
			Math::Vector coxaLocationRelativeToBody[6];

			/*! \brief The resting location of each foot, for walking dynamics trajectories.
			 */
			Math::Vector footPivotForWalking[6];

			/*! \brief The mutual exclusion implementation for parameter adjusting.
			 */
			pthread_mutex_t lock;

			/*! \brief If true, commands will be continued to be sent to the dynamixelHandler.
			 */
			bool keepUpdatingMotors;  // This is a quick hack for IDF corpse mode

			bool IK_enabled;
			bool BG_enabled;
			bool WD_enabled;

			StateChart::State* Initializing;
			StateChart::State* Walking;
			StateChart::CompositeState* powered;
			StateChart::State* idleState;

			static void construct(void*);

			StateChart::Diagram stateChart;
			StateChart::Event powerUpEvent;
			StateChart::Event powerDownEvent;
			static bool bodyHeightCheck(void*);

			static void initializeKinematics(void*);

			static void startMotors(void*);
			void startMotors();

			static void stopMotors(void*);
			void stopMotors();

			static void applyRatio(void*);
			static void revertRatio(void*);

			static void zeroBody(void*);
			void zeroBody();

			// StateChart::Diagram poweredStateChart;
			StateChart::Event disableWalkingEvent;
			StateChart::Event enableWalkingEvent;
			static bool feetAtRestCheck(void*);

			static void enableWalkingDynamics(void*);
			static void adjustStance(void*);
			static void disableWalkingDynamics(void*);

			static void setRestingBody(void*);

			//	void InternalThreadEntry();
			void entryAction();
			//	void doAction();

			void buildNodeStructure(Math::Node* root);

			void buildStateChart();

			/*! \brief Initializes the motors in the hexapod.

			 This Loads the values from the calibration file, opens and configures the
			 FTDI device, and powers up the motors.  This also loads the class parameters
			 with default values.
			 */
			void setUpMotors();

			/*! \brief With the opened FTDI port, the internal motor angles are built into the dynamixel protocol and sent to all the motors.
			 Updates the hexapod motors.
			 */
			void updateMotors();

			/*! \brief This takes the current set angles and applies forward kinematics to determine the foot location vectors.
			 Results are stored in foot.
			 Computes forward kinematics of all legs.
			 */
			void forwardKinematics();

			/*! \brief This takes the current set foot vectors and computes the angles required of the motors to achieve the desired position.

			 Final angles are stored on thetac, thetaf, and thetat for the coxa, femur, and tibia, respectively.
			 Computes inverse kinematics of all legs from foot vectors.
			 @return Returns -1 on computational or range error
			 */
			int inverseKinematics();

			/*! \brief Computes the body location.
			 This sums all the class body offsets into the final body vector.
			 */
			void finalizeBodyVector();

			//protected:

			/*! \brief Enables updating the head mechanism motors.
			 */
			bool head_enable;

			/*! \brief Gets the leg base Node.
			 \param legIndex The specific leg to get the node from.
			 \return The leg base Node.
			 */
			Math::Node* getLegBaseNode(unsigned int legIndex);

			/*! \brief Gets the coxa Node.
			 \param legIndex The specific leg to get the node from.
			 \return The coxa  Node.
			 */
			Math::Node* getCoxaNode(unsigned int legIndex);

			/*! \brief Gets the femur Node.
			 \param legIndex The specific leg to get the node from.
			 \return The femur Node.
			 */
			Math::Node* getFemurNode(unsigned int legIndex);

			/*! \brief Gets the tibia Node.
			 \param legIndex The specific leg to get the node from.
			 \return The tibia Node.
			 */
			Math::Node* getTibiaNode(unsigned int legIndex);

			/*! \brief Gets the foot Node.
			 \param legIndex The specific leg to get the node from.
			 \return The foot Node.
			 */
			Math::Node* getFootNode(unsigned int legIndex);

			int setKinematics(const Math::Vector& angles );

			/*! \brief True if the internal thread should terminate.
			 */
			bool terminate;

		public:

			/*! \brief Sets parameters of the hexapod if defined in the JSON configuration.

			 Note: This or an inherited version MUST be performed, otherwise the node
			 structure will be incomplete since this is responsible for creating leg nodes.
			 \see Bot::setConfigurationFromJSONString(), setConfigurationFromJSONFilePath()
			 \param jsonConfiguration The JSON string containing the configuration.
			 \return 0 if success, otherwise a failure occurred.
			 */
			int setConfigurationFromJSONValue(App::JsonValueInterface& jsonConfiguration);

			/*! \brief Gets the body Node.
			 \return The body's Node.
			 */
			Math::Node* getBodyNode();

			/*! \brief Represents the dmitri head mechanism, if installed.
			 */
			DmitriHeadMechanism headMechanism;

			/*! \brief Used for sending motor commands.
			 */
			Dynamixel::Handler* dynamixelHandler;

			/* Attributes */

			/*! \brief The set of 6 legs in the hexapod.
			 */
			std::vector<HexapodLeg*> legs;

			/*! \brief The speed in \f$\frac{mm}{s}\f$ that the hexapod raises the body during power up.
			 */
			double standingSpeed;

			/*! \brief The standing height in \f$mm\f$ that the hexapod raises the body to once powered.
			 */
			double standingHeight;  // Sets the desired body height.

			/*! \brief The current foot location of each foot.
			 */
			std::vector<Math::Vector> foot;

			/*! \brief The set of all dynamixel motors, as assigned from the dynamixelHandler.
			 */
			std::map<unsigned char, Dynamixel::Motor*> dyn;

			// Methods:
			/*! \brief Constructs the hexapod under a specific kinematics Node.
			 \param rootNode The root node to build the hexapod under.
			 */
			Hexapod(Math::Node* rootNode);
			~Hexapod();

			/*! \brief Starts a predefined kinematics thread to continually update internal parameters and stands up the hexapod.
			 This function blocks, and continues once the hexapod is completely standing.
			 This is similar to standUp(), but this continues a kinematics thread.
			 Powers and stands up the hexapod, executes thread for walking.
			 */
			void powerUp();

			/*! \brief Enables walking if in a manually controlled foot location mode.
			 */
			void enableWalking();

			/*! \brief Disables walking so that feet may be manually controlled.
			 */
			void disableWalking();

			/*! \brief Returns the current state's label.
			 \return The current state label.
			 */
			std::string currentState();

			/*! \brief Provides the state diagram used to handle the hexapod's power state.
			 \return The state chart used for operation.
			 */
			StateChart::Diagram* getStateChart() {
				return &stateChart;
			}

			/*! \brief Checks to see if the hexapod is currently walking.

			 Once in the walking state, the setDesiredHeading() and setDesiredPosition()
			 methods will work.
			 @return True is in walking state, false otherwise.
			 */
			bool isWalkable();

			/*! \brief Checks to see if the hexapod is standing and idle.

			 In the Idle state, a user may set parameters that the walking dynamics
			 methods usually sets, like foot positions.  The methods
			 setFootRelativeToBody() and setFootRelativeToRest() will only change the feet
			 when the hexapod is in the Idle state.
			 @return True is in idle state, false otherwise.
			 */
			bool isIdle();

			/*! \brief Checks for the powered state of the hexapod.

			 This may be used to make sure that motors are not externally driven until this class no longer updates them, or to make sure tht the hexapod has fully lowered the body when powering down.
			 \return True if the state is
			 */
			bool isPoweredOff();

			/*! \brief Starts a shutdown of the hexapod kinematics thread by lowering the body  until the legs do not support the body.
			 Then the motor torques are turned off.
			 Lowers the hexapod then disables motor power.
			 */
			void powerDown();

			/*! \brief This function is for multithreading purposes, and should be called to prevent stepping on other functions.
			 Must be called to begin modifying hexapod structure.
			 */
			void beginModifying();

			/*! \brief This function is for multithreading purposes, and should be called after the modifications have taken place.
			 Must be called upon hexapod class modification.
			 */
			void endModifying();

			/*! \brief This sets the orientation of the body.
			 Sets the body angles.
			 @param value The body quaternion.
			 */
			void setBodyOrientation(const Math::Quaternion& value);

			/*! \brief This sets the x, y, and z position of the body.
			 Sets the body location.
			 @param vector A 3-vector of the x, y and z location of the body
			 */
			void setBodyLocation(Math::Vector vector);

			/*! \brief This sets the x, y, and z position of the body.
			 Sets the body location.
			 @param x X-location
			 @param y Y-location
			 @param z Z-location
			 */
			void setBodyLocation(double x, double y, double z);

			/*! \brief This gets the current final body location.  This returned vector is the sum
			 of the body location and all the offsets applied.
			 Gets the current body vector.
			 @return Returns a 6-vector containing the location of x, y, and z, as well as
			 the rotation about x, y, and z.
			 */
			Math::Vector getBodyVector() {
				return bodyLocationTotal;
			}

			/*! \brief This gets the offsets that were added to the body location.
			 Gets the current body vector offsets.
			 @return Returns a 6-vector containing the location of x, y, and z, as well as
			 the rotation about x, y, and z.
			 */
			Math::Vector getBodyOffset() {
				return bodyLocationOffsetTotal;
			}

			/*! \brief Gets the final body orientation.
			 @return The final body orientation.
			 */
			const Math::Quaternion& getBodyOrientation() {
				return bodyOrientationTotal;
			}

			/*! \brief Gets the final body orientation offset.
			 @return The final body orientation offset.
			 */
			const Math::Quaternion& getBodyOrientationOffset() {
				return bodyOrientationOffsetTotal;
			}

			/*! \brief Sets the dynamixel handler to be used for motor communication.
			 This populates the motor list based on the configuration files.
			 It is up to the user to open the interface before calling this method.
			 \param handler The dynamixel handler to use for motor updates and configuration.
			 */
			void setDynamixelHandler(Dynamixel::Handler* handler);

			/*! \brief Sets the foot position relative to the body.
			 @param legIndex The leg index to be set: 0-5
			 @param location The relative location of the foot.
			 */
			void setFootRelativeToBody(unsigned int legIndex,
									   const Math::Vector& location);

			/*! \brief Sets the foot position relative to the foot rest position (foot
			 pivot point).
			 @param legIndex The leg index to be set: 0-5
			 @param location The relative location of the foot.
			 */
			void setFootRelativeToRest(unsigned int legIndex,
									   const Math::Vector& location);

			/*! \brief Returns the foot resting position, useful for setting global positions.
			 @param legIndex The leg index to be set: 0-5
			 \return The resting foot location.
			 */
			Math::Vector getCurrentPivotForFoot(unsigned int legIndex);


			/*! \brief Enables/Disabled the balance gestures before computing inverse kinematics.
			 @param enabled True for enabled, false if disabled.
			 */
			void setBalanceGesturesEnabled( bool enabled );

			/*! \brief Gets the walking dynamics used for walking behavior.
			 
			 The walking dynamics is also responsle for computing a safe body location and heading based on setting
			 the setters.  By default, operation of hexapod walking by setting the location and heading should be
			 performed on the WalkingDynamics object returned by this method.  The currently applied heading and
			 location may be retreived by calling WalkingDynamics::getBodyLocation() and 
			 Walkingdynamics::getBodyHeading() from this returned obejct.
			 @return The walking dynamics for this hexapod
			 */
			WalkingDynamics* getWalkingDynamics() {
				return mWalkingDynamics;
			}
		};
		
		/*! \brief Computes balance gestures solution to the foot locations.
		 
		 This computes the 6-vector of body location and orientation based on the set of
		 foot vectors and symmetrically located body-relative coxa locations.
		 \image html balanceGestures.jpg
		 \param footLocations A set of 6 vectors of all 6 foot locations
		 \param coxaLocations A set of 6 vectors of all 6 coxa locations
		 \param resultLocation The resulting location.
		 \param resultOrientation The resulting orientation.
		 */
		void balanceGestures(const std::vector<Math::Vector>& footLocations, Math::Vector* coxaLocations,
							 Math::Vector* resultLocation, Math::Quaternion* resultOrientation);
		
		
	}
}

#endif
