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

#include <math.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <typeinfo>

#include "hexapod.h"

#include "mogi/Logger.h"

#ifdef IDENT_C
static const char* const Hexapod_C_Id = "$Id$";
#endif

#define STAND_TO_WALK_RATIO (.9)

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
using namespace Dynamixel;
using namespace Math;
using namespace App;

namespace Robot {

Hexapod::Hexapod(Node* rootNode) :
		dynamixelHandler(NULL) {
	if (rootNode == NULL) {
		rootNode = new Node;
	}
	buildStateChart();

	buildNodeStructure(rootNode);
}

Hexapod::~Hexapod() {
	Mogi::Logger& logger = Mogi::Logger::getInstance();

	while (stateChart.getCurrentState() != stateChart.getFinalState()) {
		powerDown();
	}

	logger << "~Hexapod() " << std::endl;
	
	while (Thread::running());

	delete dynamixelHandler;

	for (std::vector<HexapodLeg*>::iterator it = legs.begin(); it != legs.end();
			it++) {
		HexapodLeg* leg = *it;
		delete leg;
	}

	delete mWalkingDynamics;
	pthread_mutex_destroy(&lock);
}

//void Hexapod::setDesiredHeading(const double& heading) {
//	if (!isWalkable()) {
//		return;
//	}
//
//	this->heading = heading;
//	bodyOrientationDesired.makeFromAngleAndAxis(heading, Vector::zAxis);
//}
//
//void Hexapod::setDesiredPosition(const Vector& position) {
//	if (!isWalkable()) {
//		return;
//	}
//
//	bodyLocationDesired(0) = position.valueAsConst(0, 0);
//	bodyLocationDesired(1) = position.valueAsConst(1, 0);
//}


//int Hexapod::setConfigurationFromJSONString(std::string jsonConfiguration) {
//	JsonValueInterface root;
//	Json::Reader reader;
//
//	if (!reader.parse(jsonConfiguration, root)) {
//		std::cerr << "Unable to parse JSON configuration for Hexapod."
//				<< std::endl;
//		return -1;
//	}

int Hexapod::setConfigurationFromJSONValue(JsonValueInterface& root) {
	JsonValueInterface hexapod;
	hexapod = root["hexapod"];
	setStringValueIfSafe(&name, hexapod["name"]);

	if (hexapod["WalkingDynamics"].isString()) {
		if (hexapod["WalkingDynamics"].asString().compare("DynamicGait") == 0) {
			delete mWalkingDynamics;
			mWalkingDynamics = new DynamicGait;
		} else if (hexapod["WalkingDynamics"].asString().compare("RigidGait") == 0) {
			// nothing to do
		} else {
			std::cerr << "Error: Unable to parse WalkingDynamics:" << hexapod["WalkingDynamics"].asString() << std::endl;
			return -1;
		}
	}

	double minimumFootLiftHeight = 5;
	double maximumFootLiftHeight = 20;

	setDoubleValueIfSafe(&minimumFootLiftHeight, hexapod["minimumFootLiftHeight"]);
	setDoubleValueIfSafe(&maximumFootLiftHeight, hexapod["maximumFootLiftHeight"]);

	mWalkingDynamics->setHeightBounds(minimumFootLiftHeight, maximumFootLiftHeight);

	if (!setDoubleValueIfSafe(&standingHeight, hexapod["standingHeight"])) {
		std::cerr << "WARNING! Could not find the hexapod's standing height" << std::endl;
	}

	JsonValueInterface linkArray;
	linkArray = hexapod["links"];
	if (!linkArray.isArray() || linkArray.size() != 1) {
		std::cerr << "ERROR! key \"links\" is not an array!" << std::endl;
		return -1;
	}

	JsonValueInterface body = linkArray[0];
	setStringValueIfSafe(&getBodyNode()->name, body["name"]);

	JsonValueInterface bodyLinks = body["links"];
	if (!bodyLinks.isArray() || bodyLinks.size() != 6) {
		std::cerr
				<< "Unable to parse body links from JSON configuration for Hexapod."
				<< std::endl;
		return -1;
	}
	for (unsigned int i = 0; i < bodyLinks.size(); i++) {
		JsonValueInterface leg = bodyLinks[i];
		//setStringValueIfSafe(&legs[i]->name, leg["name"]);

		HexapodLeg* newLeg = HexapodLeg::createFromJSON(leg, getBodyNode());
		if (newLeg != NULL) {
			legs.push_back(newLeg);
		} else {
			std::cerr << "Error: unable to create leg from json from array index " << i << std::endl;
			continue;
		}

		Vector location(3);
		Quaternion orientation;
		JsonValueInterface locationArray = leg["location"];
		if (locationArray.isArray() && locationArray.size() == 3) {
			for (unsigned int j = 0; j < 3; j++) {
				location(j) = locationArray[j].asDouble();
			}
		}
		JsonValueInterface orientationArray = leg["orientation"];
		if (orientationArray.isArray() && orientationArray.size() == 4) {
			for (unsigned int j = 0; j < 4; j++) {
				orientation(j) = orientationArray[j].asDouble();
			}
		}
		this->coxaLocationRelativeToBody[i] = location; // TODO: fix this to make more general for any leg base
		legs[i]->setBase(location, orientation);

		JsonValueInterface legLinks = leg["links"];
		if (!legLinks.isArray() || legLinks.size() != 1) {
			return -1;
		}

		JsonValueInterface base = legLinks[0];
//		if (base["name"].isString()) {	// TODO
//		}

//		Vector link(3);
//		JsonValueInterface baseLink = base["link"];
//		if (baseLink.isArray() && baseLink.size() == 3) {
//			for (unsigned int j = 0; j < 3; j++) {
//				link(j) = baseLink[j].asDouble();
//			}
//			legs[i]->setBaseDimensions(&link);
//		}

		JsonValueInterface baseLinks = base["links"];
		if (!baseLinks.isArray() || baseLinks.size() != 1) {
			std::cerr
					<< "Unable to parse JSON configuration for Hexapod for base links "
					<< i << std::endl;
			continue;
		}

		JsonValueInterface coxa = baseLinks[0];

		if (!coxa.isObject()) {
			std::cerr
					<< "Unable to parse JSON configuration for Hexapod for coxa "
					<< i << std::endl;
			continue;
		}
//
//		JsonValueInterface coxaLink = coxa["link"];
//		if (coxaLink.isArray() && coxaLink.size() == 3) {
//			for (unsigned int j = 0; j < 3; j++) {
//				link(j) = coxaLink[j].asDouble();
//			}
//			legs[i]->setCoxaDimensions(&link);
//		}
//
//		setDoubleValueIfSafe(&(legs[i]->angles(0)), coxa["startAngle"]);
		//					if (coxa["startAngle"].isDouble()) {
		//      legAngles[i](0) = coxa["startAngle"].asDouble();
		//					}

		if (dynamixelHandler != NULL) {
			//						dynamixelID.
			//						setIntValueIfSafe(&(dynamixelID.at(i * 3 + 1)), coxa["motorID"]);
			if (coxa["motorID"].isInt()) {
				dynamixelID[i * 3 + 1] = coxa["motorID"].asInt();
				// dyn[i*3+1]->set_byte(REG_ID, coxa["motorID"].asInt());	// TODO:
				// change the indexing of the motors to a std::map maybe
			}
			if (coxa["motorReverse"].isBool()
					&& coxa["motorCenter"].isDouble()) {
				dyn[dynamixelID[i * 3 + 1]]->setCalibration(
						coxa["motorCenter"].asDouble(),
						coxa["motorReverse"].asBool());
			}
		}

		JsonValueInterface coxaLinks = coxa["links"];

		if (!coxaLinks.isArray() || coxaLinks.size() != 1) {
			std::cerr
					<< "Unable to parse JSON configuration for Hexapod for coxa links "
					<< i << std::endl;
			continue;
		}

		JsonValueInterface femur = coxaLinks[0];

		if (!femur.isObject()) {
			std::cerr
					<< "Unable to parse JSON configuration for Hexapod for femur "
					<< i << std::endl;
			continue;
		}

//		JsonValueInterface femurLink = femur["link"];
//		if (femurLink.isArray() && femurLink.size() == 3) {
//			for (unsigned int j = 0; j < 3; j++) {
//				link(j) = femurLink[j].asDouble();
//			}
//			legs[i]->setFemurDimensions(&link);
//		}
//
//		setDoubleValueIfSafe(&(legAngles[i](1)), femur["startAngle"]);
		//					if (femur["startAngle"].isDouble()) {
		//      legAngles[i](1) = femur["startAngle"].asDouble();
		//					}
		if (dynamixelHandler != NULL) {
			if (femur["motorID"].isInt()) {
				//					dyn[i*3+2]->set_byte(REG_ID,
				//femur["motorID"].asInt());
				dynamixelID[i * 3 + 2] = femur["motorID"].asInt();
			}
			if (femur["motorReverse"].isBool()
					&& femur["motorCenter"].isDouble()) {
				dyn[dynamixelID[i * 3 + 2]]->setCalibration(
						femur["motorCenter"].asDouble(),
						femur["motorReverse"].asBool());
			}
		}

		JsonValueInterface femurLinks = femur["links"];

		if (!femurLinks.isArray() || femurLinks.size() != 1) {
			std::cerr << "Unable to parse JSON configuration for Hexapod for femur links " << i << std::endl;
			continue;
		}

		JsonValueInterface tibia = femurLinks[0];

		if (!tibia.isObject()) {
			std::cerr << "Unable to parse JSON configuration for Hexapod for tibia " << i << std::endl;
			continue;
		}

//		JsonValueInterface tibiaLink = tibia["link"];
//		if (tibiaLink.isArray() && tibiaLink.size() == 3) {
//			for (unsigned int j = 0; j < 3; j++) {
//				link(j) = tibiaLink[j].asDouble();
//			}
//			legs[i]->setTibiaDimensions(&link);
//		}
//
//		setDoubleValueIfSafe(&(legAngles[i](2)), tibia["startAngle"]);
		//					if (tibia["startAngle"].isDouble()) {
		//      legAngles[i](2) = tibia["startAngle"].asDouble();
		//					}
		if (dynamixelHandler != NULL) {
			if (tibia["motorID"].isInt()) {
				//					dyn[i*3+3]->set_byte(REG_ID,
				//tibia["motorID"].asInt());
				dynamixelID[i * 3 + 3] = tibia["motorID"].asInt();
			}
			if (tibia["motorReverse"].isBool()
					&& tibia["motorCenter"].isDouble()) {
				dyn[dynamixelID[i * 3 + 3]]->setCalibration(
						tibia["motorCenter"].asDouble(),
						tibia["motorReverse"].asBool());
			}
		}
	}

	return 0;
}


void Hexapod::entryAction() {  // TODO: Finish this to fit the setup/loop/finish

	// Timer stuff:
	Time mainTimer, timeTest;

	Vector balanceGesturesPositionSolution(3);
	Quaternion balanceGesturesOrientationSolution;

	// Main kinematics loop:
	timeTest.initialize();
	mainTimer.initialize();  // initialize timer for computing velocities

	double desiredLoopTime = 1.0 / 60.0;
	double sleepTime = 0;
	double loopTime = 0;
	double sleepTimeOffset = 0;

	while (!terminate)  // Loop forever, until termination flag is set
	{
		stateChart.update();

		mainTimer.update();

		// Now we begin modifying the hexapod structure, therefore this function is
		// called:
		beginModifying();
		{
			//      bodyMotion(mainTimer.dTime());
			double standingError = standingHeight - bodyLocationOffsetStanding(2);
			if (standingError != 0) {
				double heightToMove = standingSpeed * mainTimer.dTime();
				if (fabs(standingError) <= heightToMove) {
					// The motion exceeds the amount to move based on velocity.
					bodyLocationOffsetStanding(2) = standingHeight;
				} else {
					double standingVelocityDirection = standingError/fabs(standingError);
					bodyLocationOffsetStanding(2) += standingSpeed * standingVelocityDirection * mainTimer.dTime();
				}
			}


			// I used to set the gait right here;

			// Dynamic foot placement, gives foot locations for walking:
			if (WD_enabled) {
//				for (unsigned int i = 0; i < 6; i++) {
//					mWalkingDynamics->setPivot(i, footPivotForWalking[i]);
//				}
//				mWalkingDynamics->setBodyLocation(bodyLocationDesired);
//				mWalkingDynamics->setBodyHeading(heading);
				//        walkingDynamicsfunc(mainTimer.dTime());
				//mWalkingDynamics->perform(mainTimer.dTime());
//				foot = mWalkingDynamics->getFootLocations();
//				for (unsigned int i = 0; i < mWalkingDynamics->getFootLocations().size(); i++) {
//					foot[i] = mWalkingDynamics->getFootLocations()[i];
//				}
				foot = mWalkingDynamics->getFootLocations();
			}

			// Set the body vector to the desired vector:
			bodyLocation = mWalkingDynamics->getBodyLocation();// bodyLocationDesired;
			bodyOrientation.makeFromAngleAndAxis(mWalkingDynamics->getBodyHeading(), Vector::zAxis);

			// If balance gestures are enabled:
			if (BG_enabled) {
				balanceGestures(foot, coxaLocationRelativeToBody,
						&balanceGesturesPositionSolution,
						&balanceGesturesOrientationSolution);
			} else {
				balanceGesturesPositionSolution = bodyLocation;
				balanceGesturesOrientationSolution = bodyOrientation;
			}
			// LPF the balancce gesture solution:
			bodyLocationOffsetBalanceGestures =
					balanceGesturesLocationFilter.filter(
							balanceGesturesPositionSolution - bodyLocation,
							mainTimer.dTime());
			bodyOrientationOffsetBalanceGesture =
					balanceGesturesOrientationFilter.filter(
							balanceGesturesOrientationSolution
									* bodyOrientation.inverse(),
							mainTimer.dTime());
			//  If enabled, perform inverse kinematics:
			if (IK_enabled) {
				inverseKinematics();
			}
		}
		endModifying(); // moving this above the updateMotors(), could be what's
		// causing glitching in danceNico fist-pumping.

		// If the servo module is enabled, then update the motor positions:
		if (keepUpdatingMotors) {
			updateMotors();
		}

		// This ensures a desireable thread update rate:
		timeTest.update();
		loopTime = timeTest.dTime();
		sleepTime = desiredLoopTime - loopTime + sleepTimeOffset;
		if (sleepTime > 0) {
			usleep(sleepTime * 1000000.0);
		}
		timeTest.reset();
		sleepTimeOffset += 0.001
				* (desiredLoopTime - 1.0 / mainTimer.frequency());

	}  // end main walking loop
}

void Hexapod::setDynamixelHandler(Handler* handler) {
	dynamixelHandler = handler;
	if (dynamixelHandler == NULL) {
		return;
	}

	for (unsigned int i = 0; i < 21; i++)  // initialize motor handling
			{
		dynamixelHandler->forceAddDynamixel(i + 1);
		// dyn[i]->uploadAll();    // Have all current motor information
	}

	// dyn = new std::vector<Motor*>;
	dyn = dynamixelHandler->getDynamixels();

	for (std::map<unsigned char, Motor*>::iterator it = dyn.begin();
			it != dyn.end(); it++) {
		dynamixelHandler->pushInstruction(
				new Instruction(Instruction::READ, DataRange(0, 2),
						it->second));
	}
	while (dynamixelHandler->busy())
		;
	for (std::map<unsigned char, Motor*>::iterator it = dyn.begin();
			it != dyn.end(); it++) {
		dynamixelHandler->pushInstruction(
				new Instruction(Instruction::READ,
						DataRange(2, it->second->getNumberOfRegisters() - 2),
						it->second));
	}
	while (dynamixelHandler->busy())
		;
}

void Hexapod::setUpMotors() {
	if (dynamixelHandler == NULL) {
		return;
	}

	for (unsigned int i = 2; i < 18; i += 3) {
		dyn[dynamixelID[i - 1]]->setGoalPosition(0);
	}

	for (unsigned int ii = 1; ii < 22; ii++) {
		dyn[dynamixelID[ii]]->setWord(REG_Torque_Lim, 0x3FF);
		dynamixelHandler->pushInstruction(
				new Instruction(Instruction::WRITE,
						DataRange(REG_Torque_Lim, 2), dyn[dynamixelID[ii]]));
	}

	// Detect head type and set correct gear ratio:
	//			std::cout << "Determining head type:\n";
	if ((dyn[dynamixelID[19]]->getWord(REG_Model_Number)
			== dyn[dynamixelID[20]]->getWord(REG_Model_Number))
			&& (dyn[dynamixelID[20]]->getWord(REG_Model_Number)
					== dyn[dynamixelID[21]]->getWord(REG_Model_Number))) {
		//				std::cout << "  - Head motors match!  Setting gear
		//ratio:\n";
		//				std::cout << "  - Discovered motor model: " << dyn[18]->model <<
		//":\n";
		if (dyn[dynamixelID[19]]->getWord(REG_Model_Number) == 12) {
			//					std::cout << "      - Motor is
			//AX-12, gear ratio set to 15:8\n";
			headMechanism.setCalibration(15.0 / 8.0, 150);
			head_enable = true;
		} else if (dyn[dynamixelID[19]]->getWord(REG_Model_Number)
				== MODEL_DX117) {
			//					std::cout << "      - Motor is
			//DX-117, gear ratio set to 15:15\n";
			headMechanism.setCalibration(15.0 / 15.0, 180);
			head_enable = true;
		} else {
			//					std::cout << "      -
			//UNRECOGNIZED MOTOR!  Disabling head unit...\n";
			headMechanism.setCalibration(1, 165);
			head_enable = false;
		}
	} else {
		//				std::cout << "  - MOTORS DO NOT MATCH!  Disabling head
		//unit...\n";
		//				std::cout << "  - Motor types are " << dyn[18]->model << ", " <<
		//dyn[19]->model << ", and " << dyn[20]->model << std::endl;
		headMechanism.setCalibration(1, 165);
		head_enable = false;
	}

	for (unsigned int ii = 0; ii < legs.size(); ii++) {
		Vector angles = legs[ii]->getKinematics();
		dyn[dynamixelID[ii * 3 + 1]]->setGoalPosition(angles(0));
		dyn[dynamixelID[ii * 3 + 2]]->setGoalPosition(angles(1));
		dyn[dynamixelID[ii * 3 + 3]]->setGoalPosition(angles(2));
	}
}

void Hexapod::applyRatio(void* param) {
	Hexapod* This = (Hexapod*) param;
	for (unsigned int i = 0; i < 6; i++) {
		This->mWalkingDynamics->setPivot(i, This->footPivotForWalking[i]*STAND_TO_WALK_RATIO);
	}
}

void Hexapod::revertRatio(void* param) {
	Hexapod* This = (Hexapod*) param;
	for (unsigned int i = 0; i < 6; i++) {
		This->mWalkingDynamics->setPivot(i, This->footPivotForWalking[i]);
	}
}

void Hexapod::beginModifying() {
	pthread_mutex_lock(&lock);
}

void Hexapod::endModifying() {
	pthread_mutex_unlock(&lock);
}

bool Hexapod::bodyHeightCheck(void* param) {
	Hexapod* This = (Hexapod*) param;
	if (fabs(This->bodyLocationOffsetStanding(2) - This->standingHeight) == 0) {
//		This->standingSpeed = 0;
		return true;
	}
	return false;
}

bool Hexapod::feetAtRestCheck(void* param) {
	Hexapod* This = (Hexapod*) param;
	for (unsigned int i = 0; i < 6; i++) {

		Vector difference = This->foot[i] - This->mWalkingDynamics->getFootPivots()[i];
		if (difference.magnitude() > 1.0) {
			return false;
		}
	}
	return true;
}

void Hexapod::zeroBody(void* param) {
	Hexapod* This = (Hexapod*) param;
	This->zeroBody();
}

void Hexapod::zeroBody() {
	//setDesiredHeading(0);
	setBodyLocation(Vector(3));
	setBodyOrientation(Quaternion());
}

void Hexapod::enableWalking() {
	enableWalkingEvent.trigger();
}

void Hexapod::disableWalking() {
	disableWalkingEvent.trigger();
}

void Hexapod::enableWalkingDynamics(void* param) {
	Hexapod* This = (Hexapod*) param;
	This->WD_enabled = true;
}

	void Hexapod::adjustStance(void* param) {
		Hexapod* This = (Hexapod*) param;

		This->enableWalkingDynamics(param);

		if (typeid(*This->mWalkingDynamics) == typeid(DynamicGait)) {
			((DynamicGait*)This->mWalkingDynamics)->setControlFactor(5);
		}

		This->mWalkingDynamics->setBody(This->mWalkingDynamics->getBodyLocation(),
										This->mWalkingDynamics->getBodyHeading(),
										1.0/60.0);

		if (typeid(*This->mWalkingDynamics) == typeid(DynamicGait)) {
			((DynamicGait*)This->mWalkingDynamics)->setControlFactor(1);
		}
	}

void Hexapod::disableWalkingDynamics(void* param) {
	Hexapod* This = (Hexapod*) param;
	This->WD_enabled = false;
}

std::string Hexapod::currentState() {
	return stateChart.getCurrentState()->name();
}

void Hexapod::setRestingBody(void* param) {
	Hexapod* This = (Hexapod*) param;
	This->standingSpeed = 40;
	This->standingHeight = 0;
}

void Hexapod::buildStateChart() {
	////////////////////////////////
	// Main state diagram
	////////////////////////////////
	// Initializing state:
	Initializing = stateChart.addState(
			new StateChart::State("initializing"));
	Initializing->setCallbackEntry(construct, this);
	Initializing->setCallbackExit(initializeKinematics, this);

	// PoweringUp state:
	StateChart::State* poweringUp = stateChart.addState(
			new StateChart::State("poweringUp"));
	poweringUp->setCallbackEntry(startMotors, this);
	poweringUp->setCallbackExit(applyRatio, this);

	// Powered state (see configuration below):
	powered = (StateChart::CompositeState*) stateChart.addState(
			new StateChart::CompositeState("powered"));
	StateChart::Diagram* poweredStateChart = powered->getDiagram();

	// adjustingStance state:
	StateChart::State* adjustingStance = stateChart.addState(
			new StateChart::State("adjustingStance"));
	adjustingStance->setCallbackEntry(zeroBody, this);
	adjustingStance->setCallbackDo(adjustStance, this);
	adjustingStance->setCallbackExit(setRestingBody, this);

	// PoweringDown state:
	StateChart::State* poweringDown = stateChart.addState(
			new StateChart::State("poweringDown"));
	poweringDown->setCallbackEntry(zeroBody, this);
	poweringDown->setCallbackExit(stopMotors, this);

	// Initial -> Initializing
	StateChart::Transition* transition =
			stateChart.getInitialState()->addTransition(Initializing);

	// Initializing -> PoweringUp
	transition = Initializing->addTransition(poweringUp);
	transition->setEvent(&powerUpEvent);

	// Initializing -> Final
	transition = Initializing->addTransition(stateChart.getFinalState());
	transition->setEvent(&powerDownEvent);

	// PoweringUp -> adjustingStance
	transition = poweringUp->addTransition(adjustingStance);
	transition->setEvent(&powerDownEvent);
	transition->setCallbackAction(revertRatio, this);

	// PoweringUp -> Powered
	transition = poweringUp->addTransition(powered);
	StateChart::Guard* bodyHeightGuard = transition->addGuard(
			new StateChart::Guard);
	bodyHeightGuard->setGuardCallback(bodyHeightCheck, this);

	// Powered -> adjustingStance
	transition = powered->addTransition(adjustingStance);
	transition->setEvent(&powerDownEvent);
	transition->setCallbackAction(revertRatio, this);

	// adjustingStance -> PoweringDown
	transition = adjustingStance->addTransition(poweringDown);
	StateChart::Guard* feetAtRestGuard = transition->addGuard(
			new StateChart::Guard);
	feetAtRestGuard->setGuardCallback(feetAtRestCheck, this);
	transition->addGuard(feetAtRestGuard);

	// PoweringDown -> final
	transition = poweringDown->addTransition(stateChart.getFinalState());
	transition->addGuard(bodyHeightGuard);

	////////////////////////////////
	// Powered substate diagram
	////////////////////////////////
	// Walking state
	Walking = poweredStateChart->addState(new StateChart::State("Walking"));

	// Stopping state
	StateChart::State* Stopping = poweredStateChart->addState(
			new StateChart::State("Stopping"));
	Stopping->setCallbackEntry(zeroBody, this);
	Stopping->setCallbackExit(disableWalkingDynamics, this);

	// Idle state
	idleState = poweredStateChart->addState(new StateChart::State("Idle"));

	// Starting state
	StateChart::State* Starting = poweredStateChart->addState(
			new StateChart::State("Starting"));
	Starting->setCallbackEntry(enableWalkingDynamics, this);
	Starting->setCallbackDo(zeroBody, this);

	// initial -> Starting
	transition = poweredStateChart->getInitialState()->addTransition(Starting);

	// Walking -> Stopping
	transition = Walking->addTransition(Stopping);
	transition->setEvent(&disableWalkingEvent);

	// Stopping -> Idle
	transition = Stopping->addTransition(idleState);
	transition->addGuard(feetAtRestGuard);

	// Idle -> Starting
	transition = idleState->addTransition(Starting);
	transition->setEvent(&enableWalkingEvent);

	// Starting -> Walking
	transition = Starting->addTransition(Walking);
	transition->addGuard(feetAtRestGuard);

	stateChart.update();
}

void Hexapod::construct(void* param) {
	Hexapod* This = (Hexapod*) param;
	This->terminate = false;
	This->IK_enabled = true;
	This->BG_enabled = true;
	This->WD_enabled = false;

	This->balanceGesturesLocationFilter.setCurrentOutput(Vector(3));
	This->balanceGesturesLocationFilter.setTimeConstant(0.1);
	This->balanceGesturesOrientationFilter.setTimeConstant(0.1);

	This->mWalkingDynamics = new RigidGait;

	This->keepUpdatingMotors = true;

	This->bodyLocationTotal.setLength(3);
	This->bodyLocationTotal.name("bodyLocationFinal");

	This->bodyLocationOffsetTotal.setLength(3);
	This->bodyLocationOffsetTotal.name("bodyLocationFinalOffset");

	This->bodyLocationOffsetUser.setLength(3);
	This->bodyLocationOffsetUser.name("bodyLocationOffsetInternal");

	for (unsigned int i = 0; i < 6; i++) {
//		HexapodLeg* newLeg = new HexapodLegZYY();
//		This->legs.push_back(newLeg);

		//This->legAngles[i].setLength(3);

		This->coxaLocationRelativeToBody[i].setLength(3);

		This->foot.push_back(Vector(3));
		This->foot[i].name("foot");

		This->footPivotForWalking[i].setLength(3);
	}

//	This->bodyLocationDesired.setLength(3);
//	This->bodyLocationDesired.name("bodyLocationDesired");

	This->bodyLocationOffsetStanding.setLength(3);
	This->bodyLocationOffsetStanding.name("bodyLocationOffset");

	This->bodyLocationOffsetBalanceGestures.setLength(3);
	This->bodyLocationOffsetBalanceGestures.name(
			"bodyLocationBalanceGesturesOffset");

	This->bodyLocation.setLength(3);
	This->bodyLocation.name("bodyLocation");

	//This->heading = 0;

	pthread_mutex_init(&This->lock, NULL);

	for (unsigned int i = 1; i <= 21; i++) {
		This->dynamixelID[i] = i;
	}
}

void Hexapod::buildNodeStructure(Node* root) {
	//		if (nodeLocation == NULL) {
	////			std::cout << "Warning! root node is NULL" << std::endl;
	//			//root = new Node;
	//		}
	Bot::buildNodeStructure(root);
	//		std::cout << "Root node is :" << nodeLocation << std::endl;
	static unsigned int ID = 0;
	std::stringstream nodeName;
	//		Quaternion tempOrientation;

	nodeName << "hexapod_" << ID << std::ends;

	name = nodeName.str();
	nodeLocation = root->addNode(nodeName.str());
	nodeLocation->setScale(1.0 / MM_PER_METER);

	nodeName.str("");
	nodeName << "body_" << ID << std::ends;
	//Node* body =
	nodeLocation->addNode(nodeName.str());
	
//
//	//		int index = 0;
//	for (std::vector<HexapodLeg*>::iterator it = legs.begin(); it != legs.end();
//			it++) {
//		HexapodLeg* leg = *it;
//
//		//leg->buildNodeStructure(body);
//	}

	ID++;
}

void Hexapod::initializeKinematics(void* param) {
	Hexapod* This = (Hexapod*) param;
	if (This->legs.size() != 6) {
		std::cerr << "Error!  there are not 6 legs for this hexapod when performing forwardKinematics!" << std::endl;
	}
	This->forwardKinematics();  // Perform forward kinematics to get angles

	for (unsigned int i = 0; i < 6; i++) {
		This->foot[i](2) = 0;
		This->footPivotForWalking[i] = This->foot[i];
		This->mWalkingDynamics->setPivot(i, This->footPivotForWalking[i]);
	}

	This->mWalkingDynamics->setBody(Vector(3), 0, 1);
}

int Hexapod::setKinematics(const Vector& angles) {	// Note: Angles are ignored here...

	unsigned int i = 0;
	for (std::vector<HexapodLeg*>::iterator it = legs.begin(); it != legs.end();
			it++) {
		HexapodLeg* leg = *it;
		//Vector angles = legs[i++]->getKinematics();
		leg->setKinematics(legs[i++]->getKinematics());	// TODO this is weird:
	}

	return 0;
}

void Hexapod::updateMotors() {
	if (dynamixelHandler == NULL) {
		return;
	}
	Vector head_angles;

	for (unsigned int i = 0; i < 6; i++) // Set the goal position registers of each Dyanmixel
			{
				Vector angles = legs[i]->getKinematics();
		dyn[dynamixelID[i * 3 + 1]]->setGoalPosition(angles(0)); // * MOGI_PI/180.0);
		dyn[dynamixelID[i * 3 + 2]]->setGoalPosition(angles(1)); // * MOGI_PI/180.0);
		dyn[dynamixelID[i * 3 + 3]]->setGoalPosition(angles(2)); // * MOGI_PI/180.0);
	}

	if (head_enable == true) {
		// Compute angles for the head motors:
		head_angles = headMechanism.computeAngles();

		dyn[dynamixelID[19]]->setGoalPosition(head_angles(0));
		dyn[dynamixelID[20]]->setGoalPosition(head_angles(1));
		dyn[dynamixelID[21]]->setGoalPosition(head_angles(2));
	} else {
		dyn[dynamixelID[19]]->setGoalPosition(0);
		dyn[dynamixelID[20]]->setGoalPosition(0);
		dyn[dynamixelID[21]]->setGoalPosition(0);
	}

	for (std::map<unsigned char, Motor*>::iterator it = dyn.begin();
			it != dyn.end(); it++) {
		it->second->setMovingSpeed(0);
	}

	// Send the motor command
	if (dynamixelHandler !=
	NULL)  // push back the write all instruction on the handler
	{
		dynamixelHandler->pushInstruction(
				new Instruction(Instruction::SYNC_WRITE,
						DataRange(REG_Goal_Pos, 4)));
	}
}

void Hexapod::finalizeBodyVector() {
	bodyLocationOffsetTotal = bodyLocationOffsetStanding
			+ bodyLocationOffsetBalanceGestures + bodyLocationOffsetUser;
	bodyOrientationOffsetTotal = bodyOrientationOffset
			* bodyOrientationOffsetBalanceGesture * bodyOrientationOffsetUser;

	bodyLocationTotal = bodyLocation + bodyLocationOffsetTotal;
	bodyOrientationTotal = bodyOrientationOffsetTotal * bodyOrientation;

	Node* body = getBodyNode();  // nodeLocation;//->child(0);

	body->setLocation(bodyLocationTotal);  /// MM_PER_METER);
	body->setOrientation(bodyOrientationTotal);
	body->update();  // update child nodes
}

void Hexapod::setBodyOrientation(const Quaternion& value) {
	bodyOrientationOffsetUser = value;
}

void Hexapod::setBodyLocation(Vector location) {
	if (location.size() == 3) {
		bodyLocationOffsetUser = location;
	} else {
		std::cout << "setBodyLocation must only take a size 3 vector, not "
				<< location.size() << std::endl;
	}
}

void Hexapod::setBodyLocation(double x, double y, double z) {
	bodyLocationOffsetUser(0) = x;
	bodyLocationOffsetUser(1) = y;
	bodyLocationOffsetUser(2) = z;
}

Vector Hexapod::getCurrentPivotForFoot(unsigned int legindex) {
	//double pivotMagnitude = footPivotForWalking[legindex].magnitude(); // sqrt(pow(footPivotForWalking[i](0),2.0) +
	// pow(footPivotForWalking[i](1),2.0));	// foot
	// pivot magnitude relative to the body
	//double pivotAngle = atan2(footPivotForWalking[legindex](1), footPivotForWalking[legindex](0)) + heading; // angle difference of body and foot pivot

	return mWalkingDynamics->getFootPivots()[legindex];
//	double cosP = cos(heading);
//	double sinP = sin(heading);
//	Vector* footPivot = &footPivotForWalking[legindex];
//
//	Vector current_p(3);
//	current_p(0) = bodyLocation(0) + footPivot->value(0, 0) * cosP - sinP * footPivot->value(1, 0);
//	current_p(1) = bodyLocation(1) + footPivot->value(0, 0) * sinP + cosP * footPivot->value(1, 0);
//
//	return current_p;
}

void Hexapod::startMotors(void* param) {
	Hexapod* This = (Hexapod*) param;
	This->startMotors();
}

void Hexapod::startMotors() {

	setUpMotors();  // Hmmm, maybe this should be called externally

	if (dynamixelHandler != NULL) {

		//		if (walking()) {
		//			return;
		//		}
		//		terminate = false;
		// bodyVelocity(2) = 40;
		//		setWalking( false );
		//		std::cout << "Powering up motors..." << std::endl;

//		for (unsigned int i = 0; i < 6; i++) { // DO NOT RUN THIS FOR THE HEAD MOTORS!
		unsigned int i = 0;
		for (std::vector<HexapodLeg*>::iterator it = legs.begin(); it != legs.end(); it++, i++) {
			Vector angles = (*it)->getKinematics();
			for (unsigned int j = 0; j < 3; j++) {	// for all 3 angles
				dyn[dynamixelID[i * 3 + j + 1]]->setWord(REG_Moving_Speed,
						20 * 1023. / 114.);
				dynamixelHandler->pushInstruction(
						new Instruction(Instruction::WRITE,
								DataRange(REG_Moving_Speed, 2),
								dyn[dynamixelID[i * 3 + j + 1]]));

				dyn[dynamixelID[i * 3 + j + 1]]->setGoalPosition(angles(j));
				dynamixelHandler->pushInstruction(
						new Instruction(Instruction::WRITE,
								DataRange(REG_Goal_Pos, 2),
								dyn[dynamixelID[i * 3 + j + 1]]));

				dyn[dynamixelID[i * 3 + j + 1]]->setByte(REG_Torque_EN, 1); // re-enable torque if disabled, also  turn on the
				// LED for eff's sake
				dyn[dynamixelID[i * 3 + j + 1]]->setByte(REG_LED, 1);
				dynamixelHandler->pushInstruction(
						new Instruction(Instruction::WRITE,
								DataRange(REG_Torque_EN, 2),
								dyn[dynamixelID[i * 3 + j + 1]]));

				usleep(25000);  // Delay so power does not spike
			}
		}

		sleep(2);
	}

	//		std::cout << "Done Configuring Motors!" << std::endl;
	standingSpeed = 40;
	terminate = false;
	start();
	//		while( !walking() ); // Wait until the body is fully elevated
}

void Hexapod::powerUp() {
	powerUpEvent.trigger();
}

void Hexapod::powerDown() {
	powerDownEvent.trigger();
}

void Hexapod::stopMotors(void* param) {
	Hexapod* This = (Hexapod*) param;
	This->stopMotors();
}

void Hexapod::stopMotors() {
	if (dynamixelHandler != NULL) // To finish things up, we disable the applied
	// to torque of the motors
	{
		for (unsigned int i = 1; i < 22; i++) {
			// dyn[i]->writeValue( Torque_EN, 0x00); // re-enable torque if disabled,
			// also turn on the LED for eff's sake
			dyn[dynamixelID[i]]->setByte(REG_Torque_EN, 0x00);
			//				dyn[i]->writeValue( LED_Reg , 0x00);
			//// turn off LED
			dyn[dynamixelID[i]]->setByte(REG_LED, 0x00);
			dynamixelHandler->pushInstruction(
					new Instruction(Instruction::WRITE,
							DataRange(REG_Torque_EN, 2), dyn[dynamixelID[i]]));
			usleep(10000);
		}
	}

	// TODO: this should be moved into a separate method...?
	terminate = true;  // should cause kinematics thread to teminate
	// Thread::WaitForInternalThreadToExit();
	Thread::stop();
}

int Hexapod::inverseKinematics() {
	finalizeBodyVector();
	unsigned int i = 0;
	for (std::vector<HexapodLeg*>::iterator it = legs.begin(); it != legs.end();
			it++, i++) {
//		legAngles[i] = (*it)->computeAngles(foot[i]);
		(*it)->computeAngles(foot[i]);	// legAngles now store in the leg itself
	}
	return KINEMATICS_NO_ERROR;
}

void Hexapod::forwardKinematics() {
	setKinematics(NULL);
	update(0);
	if (nodeLocation) {
		unsigned int i = 0;
		for (std::vector<HexapodLeg*>::iterator it = legs.begin(); it != legs.end(); it++, i++) {
			Node* footN = getFootNode(i);

			Matrix temp;
			transformationToScaleRotationTranslation(footN->getModelMatrix(),
			NULL,
			NULL, &temp);

			foot[i](0) = temp(0, 3) * MM_PER_METER;
			foot[i](1) = temp(1, 3) * MM_PER_METER;
			foot[i](2) = temp(2, 3) * MM_PER_METER;
		}
	}
}

Node* Hexapod::getBodyNode() {
	if (nodeLocation) {
		return nodeLocation->child(0);
	}
	return NULL;
}

Node* Hexapod::getLegBaseNode(unsigned int legIndex) {
	Node* body = getBodyNode();
	if (body != NULL && legIndex < body->numberOfChildren()) {
		return body->child(legIndex);
	}
	return NULL;
}

Node* Hexapod::getCoxaNode(unsigned int legIndex) {
	Node* legBase = getLegBaseNode(legIndex);
	if (legBase != NULL) {
		return legBase->child(0);
	}
	return NULL;
}

Node* Hexapod::getFemurNode(unsigned int legIndex) {
	Node* coxa = getCoxaNode(legIndex);
	if (coxa != NULL) {
		return coxa->child(0);
	}
	return NULL;
}

Node* Hexapod::getTibiaNode(unsigned int legIndex) {
	Node* femur = getFemurNode(legIndex);
	if (femur != NULL) {
		return femur->child(0);
	}
	return NULL;
}

Node* Hexapod::getFootNode(unsigned int legIndex) {
	Node* tibia = getTibiaNode(legIndex);
	if (tibia != NULL) {
		return tibia->child(0);
	}
	return NULL;
}

void Hexapod::setFootRelativeToBody(unsigned int legIndex,
		const Vector& location) {
	if (!isIdle()) {
		return;  // maybe return a bool if it was not set?
	}
	Quaternion hexapodOrientation;
	hexapodOrientation.makeFromAngleAndAxis(mWalkingDynamics->getBodyHeading(), Vector::zAxis);
	foot[legIndex] = relativeToGlobal(hexapodOrientation, bodyLocation,
			location);
}

void Hexapod::setFootRelativeToRest(unsigned int legIndex,
		const Vector& location) {
	if (!isIdle()) {
		return;  // maybe return a bool if it was not set?
	}
	Quaternion hexapodOrientation;
	hexapodOrientation.makeFromAngleAndAxis(mWalkingDynamics->getBodyHeading(), Vector::zAxis);
//	foot[legIndex] = relativeToGlobal(hexapodOrientation, bodyLocation,
//			footPivotForWalking[legIndex] + location);
	foot[legIndex] = relativeToGlobal(hexapodOrientation, mWalkingDynamics->getFootPivots()[legIndex],
									  location);

}

bool Hexapod::isWalkable() {
	return stateChart.getCurrentState() == powered &&
	powered->getDiagram()->getCurrentState() == Walking;
}

bool Hexapod::isIdle() {
	return stateChart.getCurrentState() == powered &&
	powered->getDiagram()->getCurrentState() == idleState;

}

	bool Hexapod::isPoweredOff() {
		return stateChart.getCurrentState() == stateChart.getFinalState() ||
		stateChart.getCurrentState() == stateChart.getInitialState() ||
		stateChart.getCurrentState() == Initializing;
	}

	void Hexapod::setBalanceGesturesEnabled( bool enabled ) {
		BG_enabled = enabled;
	}

// This returns a 6-vector of location and XYZ angles based on coxa and
void balanceGestures(const std::vector<Math::Vector>& footLocations, Vector* coxaLocations,
		Vector* resultLocation, Quaternion* resultOrientation) {
	double quicktemp, quicktemp2;

	// Vector tempCoxa[6];
	Vector tempFoot[6];  // 1-3-2015
	Vector bgReturn(6);
	Vector bgLocation(3);
	Vector bgAngles(3);

	for (unsigned int i = 0; i < 6; i++) {
		// tempCoxa[i] = coxaLocations[i];	// Need to tstore the coxa locations
		tempFoot[i] = footLocations[i];  // 1-3-2015
		bgLocation += footLocations[i]; // Location is average location thanks to symmetry
	}
	bgLocation /= 6.0;

	for (unsigned int i = 0; i < 6; i++) {  // 1-3-2015
		tempFoot[i] -= bgLocation;   // 1-3-2015
	}

	// First compute Z rotation:
	quicktemp = 0;
	quicktemp2 = 0;
	//		for(int i=0; i<6; i++)		// Balanced z-axis rotation:
	//		{
	//			// Nothing rotated yet so no modified coxa locations
	//
	//			// For Z rotation, use XY
	//			quicktemp  += tempCoxa[i](0) * (bgLocation(1) -
	//footLocations[i](1)) - tempCoxa[i](1) * (bgLocation(0) -
	//footLocations[i](0));
	//			quicktemp2 += tempCoxa[i](1) * (bgLocation(1) -
	//footLocations[i](1)) + tempCoxa[i](0) * (bgLocation(0) -
	//footLocations[i](0));
	//		}
	//		bgAngles(2) = atan2( -quicktemp, -quicktemp2);

	for (unsigned int i = 0; i < 6; i++)  // Balanced z-axis rotation:
			{
		// Nothing rotated yet so no modified coxa locations

		// For Z rotation, use XY
		quicktemp += coxaLocations[i](0) * tempFoot[i](1)
				- coxaLocations[i](1) * tempFoot[i](0);
		quicktemp2 += coxaLocations[i](1) * tempFoot[i](1)
				+ coxaLocations[i](0) * tempFoot[i](0);
	}
	bgAngles(2) = atan2(quicktemp, quicktemp2);

	// Now for Y:
	quicktemp = 0;
	quicktemp2 = 0;
	//		for(int i=0; i<6; i++)		// Balanced y-axis rotation:
	//		{
	//			// This takes the solved Z-rotation to rotate the
	//points.
	//			double yetAnotherTemp = tempCoxa[i](0);
	//			tempCoxa[i](0) = tempCoxa[i](0)  * cosf( bgAngles(2) ) -
	//tempCoxa[i](1) * sinf( bgAngles(2) );
	//			tempCoxa[i](1) = yetAnotherTemp  * sinf( bgAngles(2) ) +
	//tempCoxa[i](1) * cosf( bgAngles(2) );
	//
	//			// For Y rotation, use ZX
	//			quicktemp  += tempCoxa[i](2) * (bgLocation(0) -
	//footLocations[i](0)) - tempCoxa[i](0) * (bgLocation(2) -
	//footLocations[i](2));
	//			quicktemp2 += tempCoxa[i](0) * (bgLocation(0) -
	//footLocations[i](0)) + tempCoxa[i](2) * (bgLocation(2) -
	//footLocations[i](2));
	//		}
	//		bgAngles(1) = atan2( -quicktemp, -quicktemp2);

	for (unsigned int i = 0; i < 6; i++)  // Balanced y-axis rotation:
			{
		// This takes the solved Z-rotation to rotate the points.
		//			double yetAnotherTemp = tempCoxa[i](0);
		//			tempCoxa[i](0) = tempCoxa[i](0)  * cosf( bgAngles(2) ) -
		//tempCoxa[i](1) * sinf( bgAngles(2) );
		//			tempCoxa[i](1) = yetAnotherTemp  * sinf( bgAngles(2) ) +
		//tempCoxa[i](1) * cosf( bgAngles(2) );
		double yetAnotherTemp = tempFoot[i](0);
		tempFoot[i](0) = tempFoot[i](0) * cosf(-bgAngles(2))
				- tempFoot[i](1) * sinf(-bgAngles(2));
		tempFoot[i](1) = yetAnotherTemp * sinf(-bgAngles(2))
				+ tempFoot[i](1) * cosf(-bgAngles(2));

		// For Y rotation, use ZX
		quicktemp += coxaLocations[i](2) * tempFoot[i](0)
				- coxaLocations[i](0) * tempFoot[i](2);
		quicktemp2 += coxaLocations[i](0) * tempFoot[i](0)
				+ coxaLocations[i](2) * tempFoot[i](2);
	}
	bgAngles(1) = atan2(quicktemp, quicktemp2);

	// Lastly for X angle:
	quicktemp = 0;
	quicktemp2 = 0;
	//		for(int i=0; i<6; i++)
	//		{
	//			// This takes the solved Y-rotation to rotate the
	//points.
	//			double yetAnotherTemp = tempCoxa[i](2);
	//			tempCoxa[i](2) = tempCoxa[i](2) * cosf( bgAngles(1) ) -
	//tempCoxa[i](0) * sinf( bgAngles(1) );
	//			tempCoxa[i](0) = yetAnotherTemp * sinf( bgAngles(1) ) +
	//tempCoxa[i](0) * cosf( bgAngles(1) );
	//
	//			// For X rotation, use YZ
	//			quicktemp  += tempCoxa[i](1) * (bgLocation(2) -
	//footLocations[i](2)) - tempCoxa[i](2) * (bgLocation(1) -
	//footLocations[i](1));
	//			quicktemp2 += tempCoxa[i](2) * (bgLocation(2) -
	//footLocations[i](2)) + tempCoxa[i](1) * (bgLocation(1) -
	//footLocations[i](1));
	//		}
	//		bgAngles(0) = 0;//atan2( -quicktemp, -quicktemp2);
	for (unsigned int i = 0; i < 6; i++) {
		// This takes the solved Y-rotation to rotate the points.
		double yetAnotherTemp = tempFoot[i](2);
		tempFoot[i](2) = tempFoot[i](2) * cosf(-bgAngles(1))
				- tempFoot[i](0) * sinf(bgAngles(1));
		tempFoot[i](0) = yetAnotherTemp * sinf(-bgAngles(1))
				+ tempFoot[i](0) * cosf(bgAngles(1));

		// For X rotation, use YZ
		quicktemp += coxaLocations[i](1) * tempFoot[i](2)
				- coxaLocations[i](2) * tempFoot[i](1);
		quicktemp2 += coxaLocations[i](2) * tempFoot[i](2)
				+ coxaLocations[i](1) * tempFoot[i](1);
	}
	bgAngles(0) = atan2(quicktemp, quicktemp2);

	// Store solution in return vector and return
	bgReturn(0) = bgLocation(0);
	bgReturn(1) = bgLocation(1);
	bgReturn(2) = bgLocation(2);
	bgReturn(3) = bgAngles(0);
	bgReturn(4) = bgAngles(1);
	bgReturn(5) = bgAngles(2);

	*resultLocation = bgLocation;

	Quaternion xRot, yRot, zRot;

	xRot.makeFromAngleAndAxis(bgAngles(0), Vector::xAxis);
	yRot.makeFromAngleAndAxis(bgAngles(1), Vector::yAxis);
	zRot.makeFromAngleAndAxis(bgAngles(2), Vector::zAxis);
	*resultOrientation = zRot * yRot * xRot;
	//*resultOrientation = ??? // TODO either fix BG (good) or convert Euler
	//result to quaternion (easy, poor)

	//		return bgReturn;
}

}
}

#ifdef _cplusplus
}
#endif
