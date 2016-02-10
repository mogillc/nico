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

#include <fstream>
#include <iostream>
#include <sstream>

#include <math.h>    // sin,cos
#include <stdio.h>   // printf
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE

#include <typeinfo>	 // typeid()

#include "hexapod.h"
#include "mogi.h"

using namespace Mogi;
using namespace Math;
using namespace Robot;
using namespace Dynamixel;

bool testConfiguration();
bool testConfigurationYYX();

int main(int argc, char* argv[]) {
	bool allTestsPass = true;

	std::cout << " - Beginning Configuration tests:" << std::endl;
	allTestsPass = testConfiguration() ? allTestsPass : false;
	std::cout << " - Beginning Configuration YYX tests:" << std::endl;
	allTestsPass = testConfigurationYYX() ? allTestsPass : false;

	if (allTestsPass) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool testConfiguration() {
	bool allTestsPass = true;

	std::cout << "Testing creation ...................... ";
	std::string jsonConfigurationPath = std::string(getResourceDirectory()) + "/configurations/legTest.json";
	//std::cout << "Loading configuration file: " << jsonConfigurationPath << std::endl;
	std::ifstream jsonConfigurationFile( jsonConfigurationPath.c_str() );
	std::string jsonConfiguration((std::istreambuf_iterator<char>(jsonConfigurationFile)),
								  std::istreambuf_iterator<char>());

	Json::Value legConfiguration;
	Json::Reader reader;
	if (!reader.parse(jsonConfiguration, legConfiguration)) {
		std::cerr << "Unable to parse JSON configuration from: " << jsonConfiguration << std::endl;
		return false;
	}
	Node root;
	HexapodLeg* leg = HexapodLeg::createFromJSON(legConfiguration, &root);

	if ( leg == NULL ||
		typeid(*leg) != typeid(HexapodLegZYY)) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	leg->update(0);
	std::cout << " - Checking values .................... ";
	Vector angles = leg->getKinematics();
	if ( angles.size() != 3 ||
		angles(0) != 0.1 ||
		angles(1) != -2.2116812281 ||
		angles(2) != 2.3561944902 ) {
		std::cout << "FAILED" << std::endl;
		angles.print();
		leg->getBaseNode()->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->print();
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

		Vector startAngles = leg->getKinematics();

	std::cout << " - Checking 0 angles .................. ";
	leg->setKinematics(Vector(3));	// Set the joint angles
	leg->update(0);					// perform kinematics node updates
	angles = leg->getKinematics();	// get the currently aplied angles
	if ( angles.size() != 3 ||
		angles(0) != 0.0 ||
		angles(1) != 0.0 ||
		angles(2) != 0.0 ||
		leg->getBaseNode()->child(0)->getModelMatrix()->valueAsConst(2, 3) != 15.0 ||
		leg->getBaseNode()->child(0)->child(0)->getModelMatrix()->valueAsConst(0, 3) != 29.0 ||
		leg->getBaseNode()->child(0)->child(0)->getModelMatrix()->valueAsConst(2, 3) != -9.0 ||
		leg->getBaseNode()->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(0, 3) != 89.0 ||
		leg->getBaseNode()->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(2, 3) != -9.0 ||
		leg->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(0, 3) != 184.0 ||
		leg->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(2, 3) != -40.0 ) {
		std::cout << "FAILED" << std::endl;
		angles.print();
		leg->getBaseNode()->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->print();
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}



	std::cout << " - Checking compute angles ............ ";
	Vector foot(3);
	foot(0) = 184;	// from above, resulting in angles of ~0
	foot(1) = 0;
	foot(2) = -40;
	Vector result = leg->computeAngles(foot);
	if ( fabs(result(0) - 0) > 0.000001 ||
		fabs(result(1) - 0) > 0.000001 ||
		fabs(result(2) - 0) > 0.000001 ) {
		std::cout << "FAILED" << std::endl;
		result.print();
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << " - Checking set/compute angles ........ ";
	leg->setKinematics(startAngles);
	leg->update(0);	// perform node updates
	foot(0) = leg->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(0, 3);	// from above, resulting in angles of ~0
	foot(1) = leg->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(1, 3);
	foot(2) = leg->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(2, 3);
	result = leg->computeAngles(foot);
	if ( fabs(result(0) - startAngles(0)) > 0.000001 ||
		fabs(result(1) - startAngles(1)) > 0.000001 ||
		fabs(result(2) - startAngles(2)) > 0.000001 ) {
		std::cout << "FAILED" << std::endl;
		foot.print();
		result.print();
		startAngles.print();
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	delete leg;

	return allTestsPass;
}

bool testConfigurationYYX() {
	bool allTestsPass = true;

	std::cout << "Testing creation ...................... ";
	std::string jsonConfigurationPath = std::string(getResourceDirectory()) + "/configurations/legTestYYX.json";
	//std::cout << "Loading configuration file: " << jsonConfigurationPath << std::endl;
	std::ifstream jsonConfigurationFile( jsonConfigurationPath.c_str() );
	std::string jsonConfiguration((std::istreambuf_iterator<char>(jsonConfigurationFile)),
								  std::istreambuf_iterator<char>());

	Json::Value legConfiguration;
	Json::Reader reader;
	if (!reader.parse(jsonConfiguration, legConfiguration)) {
		std::cerr << "Unable to parse JSON configuration from: " << jsonConfiguration << std::endl;
		return false;
	}
	Node root;
	HexapodLeg* leg = HexapodLeg::createFromJSON(legConfiguration, &root);

	if ( leg == NULL ||
		typeid(*leg) != typeid(HexapodLegYYX)) {
		std::cout << "FAILED" << std::endl;
		return false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	leg->update(0);
	std::cout << " - Checking values .................... ";
	Vector angles = leg->getKinematics();
	if ( angles.size() != 3 ||
		angles(0) != -2.0943951024 ||
		angles(1) != 0.5 ||
		angles(2) != 0.0 ) {
		std::cout << "FAILED" << std::endl;
		angles.print();
		leg->getBaseNode()->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->print();
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	Vector startAngles = leg->getKinematics();

	std::cout << " - Checking 0 angles .................. ";
	leg->setKinematics(Vector(3));	// Set the joint angles
	leg->update(0);					// perform kinematics node updates
	angles = leg->getKinematics();	// get the currently aplied angles
	if ( angles.size() != 3 ||
		angles(0) != 0.0 ||
		angles(1) != 0.0 ||
		angles(2) != 0.0 ||
		leg->getBaseNode()->child(0)->getModelMatrix()->valueAsConst(2, 3) != 0.0 ||
		fabs(leg->getBaseNode()->child(0)->child(0)->getModelMatrix()->valueAsConst(0, 3) - 110.0) > 0.000001 ||
		fabs(leg->getBaseNode()->child(0)->child(0)->getModelMatrix()->valueAsConst(2, 3) - 0.0) > 0.000001 ||
		fabs(leg->getBaseNode()->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(0, 3) - 147.7) > 0.000001 ||
		fabs(leg->getBaseNode()->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(2, 3) - -33.85) > 0.000001 ||
		fabs(leg->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(0, 3) - 56.15) > 0.000001 ||
		fabs(leg->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(2, 3) - -210.91) > 0.000001 ) {
		std::cout << "FAILED" << std::endl;
		angles.print();
		leg->getBaseNode()->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->child(0)->getModelMatrix()->print();
		((HexapodLegZYY*)leg)->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->print();
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << " - Checking compute angles ............ ";
	Vector foot(3);
	foot(0) = 56.15;	// from above, resulting in angles of ~0
	foot(1) = 0;
	foot(2) = -210.91;
	Vector result = leg->computeAngles(foot);
	if ( fabs(result(0) - 0) > 0.000001 ||
		fabs(result(1) - 0) > 0.000001 ||
		fabs(result(2) - 0) > 0.000001 ) {
		std::cout << "FAILED" << std::endl;
		result.print();
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << " - Checking set/compute angles ........ ";
	leg->setKinematics(startAngles);
	leg->update(0);	// perform node updates
	foot(0) = leg->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(0, 3);	// from above, resulting in angles of ~0
	foot(1) = leg->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(1, 3);
	foot(2) = leg->getBaseNode()->child(0)->child(0)->child(0)->child(0)->getModelMatrix()->valueAsConst(2, 3);
	result = leg->computeAngles(foot);
	if ( fabs(result(0) - startAngles(0)) > 0.000001 ||
		fabs(result(1) - startAngles(1)) > 0.000001 ||
		fabs(result(2) - startAngles(2)) > 0.000001 ) {
		std::cout << "FAILED" << std::endl;
		foot.print();
		result.print();
		startAngles.print();
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	delete leg;

	return allTestsPass;
}