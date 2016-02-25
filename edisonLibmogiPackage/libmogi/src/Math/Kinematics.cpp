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

#include "mogi/math/kinematics.h"

#include <math.h>

#ifdef UNIX
#include <iostream>
#endif

#include <stdlib.h>

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
namespace Math {

InverseKinematics::InverseKinematics() {
	motorAngles = Vector::create();
	motorScratch = Vector::create();

	endEffectorLocation = Vector::create();
	endEffectorLocation->setLength(3);
	endEffectorQuaternion = Quaternion::create();

	relativeEELocation = Vector::create();
	relativeEELocation->setLength(3);
	relativeEEQuaternion = Quaternion::create();

	child = NULL;

	configuration = UNSUPPORTED;
	numLinks = 0;
}

InverseKinematics::~InverseKinematics() {
#ifdef UNIX
	if (child != NULL) {
		delete child;
	}
	delete motorAngles;
	delete motorScratch;
	delete endEffectorLocation;
	delete endEffectorQuaternion;
	delete relativeEELocation;
	delete relativeEEQuaternion;
	if (numLinks > 0) {
		for (unsigned int i = 0; i < numLinks; i++) {
			delete links[i]->link;
			delete links[i]->orientation;
			delete links[i]->location;
			delete links[i];
		}
		delete[] links;
	}
#else
	if (child != NULL) {
		child->~InverseKinematics();
		free(child);
	}
	motorAngles->~Vector();
	motorScratch->~Vector();
	endEffectorLocation->~Vector();
	endEffectorQuaternion->~Quaternion();
	relativeEELocation->~Vector();
	relativeEEQuaternion->~Quaternion();
	if (numLinks > 0) {
		for (unsigned int i = 0; i < numLinks; i++) {
			links[i]->link->~Vector();
			links[i]->orientation->~Quaternion();
			links[i]->location->~Vector();
			free(links[i]);
		}
		free(links);
	}
#endif
}

InverseKinematics* InverseKinematics::create(
		ConfigurationType configurationType) {
	// InverseKinematics *result = NULL;
#ifdef UNIX
	if (configurationType == ZYYXZY) {
		InverseKinematicsOF* result = new InverseKinematicsOF;
		result->setType(configurationType);
		return result;
	} else if (configurationType < NUM_CONFIGURATIONS) {
		InverseKinematicsCF* result = new InverseKinematicsCF;
		result->setType(configurationType);
		return result;
	}
#else
	if (configurationType == ZYYXZY) {
		InverseKinematicsOF* result =
		(InverseKinematicsOF*)malloc(sizeof(InverseKinematicsOF));
		new (result) InverseKinematicsOF;
		result->setType(configurationType);
		return result;

	} else if (configurationType < NUM_CONFIGURATIONS) {
		InverseKinematicsCF* result =
		(InverseKinematicsCF*)malloc(sizeof(InverseKinematicsCF));
		new (result) InverseKinematicsCF;
		result->setType(configurationType);
		return result;
	}
#endif
	return NULL;
}

KinematicsStatus InverseKinematics::handleParameters(const Vector& params) {
	switch (configuration) {
	case ZYY:
	case YYX:
		if (params.size() != 3) {
			return KINEMATICS_INVALID_PARAMETER;
		}
		*endEffectorLocation = params;
		break;

	case ZYYXZY:
		if (params.size() != 7) {
			return KINEMATICS_INVALID_PARAMETER;
		}
		(*endEffectorLocation)(0) = params.valueAsConst(0, 0);
		(*endEffectorLocation)(1) = params.valueAsConst(1, 0);
		(*endEffectorLocation)(2) = params.valueAsConst(2, 0);
		(*endEffectorQuaternion)(0) = params.valueAsConst(3, 0);
		(*endEffectorQuaternion)(1) = params.valueAsConst(4, 0);
		(*endEffectorQuaternion)(2) = params.valueAsConst(5, 0);
		(*endEffectorQuaternion)(3) = params.valueAsConst(6, 0);
		break;

	default:
		return KINEMATICS_BAD_CONFIGURATION;
		break;
	}

	return KINEMATICS_NO_ERROR;
}

KinematicsStatus InverseKinematicsOF::compute(const Vector& location) {
	if (handleParameters(location)) {
		return KINEMATICS_INVALID_PARAMETER;
	}
	findRelativeLocation();
	switch (configuration) {
	case ZYYXZY:
		return iterativeZYYXZYInverseKinematics();
		break;
	default:
		break;
	}
	return KINEMATICS_BAD_CONFIGURATION;
}

KinematicsStatus InverseKinematicsCF::compute(const Vector& location) {
	if (handleParameters(location)) {
		return KINEMATICS_INVALID_PARAMETER;
	}
	findRelativeLocation();
	switch (configuration) {
	case ZYY:

		return compute3DOFGeneral();
		break;
	case YYX:
		return computeYYXSolution();
		break;

	default:
		break;
	}
	return KINEMATICS_BAD_CONFIGURATION;
}

const Vector& InverseKinematics::getMotorAngles() {
	return *motorAngles;
}

void InverseKinematics::setType(ConfigurationType configurationType) {
	if (configurationType < NUM_CONFIGURATIONS) {
		configuration = configurationType;

		switch (configuration) {
		case ZYY:
		case YYX:
			numLinks = 4;
			break;
		case ZYYY:
			numLinks = 5;
			break;
		case ZYYYX:
			numLinks = 6;
			break;
		case ZYYXZY:
			numLinks = 7; // Only need to append 3 links to the end of the child:
			child = InverseKinematics::create(ZYY);
			break;

		case UNSUPPORTED:
		case NUM_CONFIGURATIONS:
			break;
		}

		motorAngles->setLength(numLinks > 0 ? numLinks - 1 : 0);
		motorScratch->setLength(motorAngles->size());

		#ifdef UNIX
		links = new Link* [numLinks];
#else
		links = (Link**) malloc(numLinks * sizeof(Link*));
#endif
		for (unsigned int i = 0; i < numLinks; i++) {
			#ifdef UNIX
			links[i] = new Link;
#else
			links[i] = (Link*) malloc(sizeof(Link));
#endif
			links[i]->link = Vector::create();
			links[i]->link->setLength(3);
			links[i]->orientation = Quaternion::create();
			links[i]->location = Vector::create();
			links[i]->location->setLength(3);
		}
	} else {
		configuration = UNSUPPORTED;
	}
}
//
//	void InverseKinematics::setConfiguration( Matrix &linkConfig )//, Matrix
//&axisConfig )
//	{
//		//bool test = true;
//
//		// This setter must ensure that this inverse kinematics
//implementation can solve it
//		/*
//		 if ((linkConfig.numColumns() == 4)) {// &&
//(axisConfiguration->numColumns() == 3)) {
//		 for (int i = 1; i < 4; i++) {
//		 test &= linkConfig.value(1,i) == 0;
//		 }
//		 if ((linkConfig.value(0,1) == 0) &&
//		 (linkConfig.value(2,2) == 0) && test)
//		 {
//		 configuration = ZYY;
//		 }
//
//		 } else if ((linkConfig.numColumns() == 7)) {// &&
//(axisConfiguration->numColumns() == 6)) {
//		 for (int i = 1; i < 7; i++) {
//		 test &= linkConfig.value(1,i) == 0;
//		 }
//		 for (int i = 4; i < 7; i++) {
//		 test &= linkConfig.value(2,i) == 0;
//		 }
//
//		 if ((linkConfig.value(0,1) == 0) &&
//		 (linkConfig.value(2,2) == 0) && test)
//		 {
//		 configuration = ZYYXZY;
//		 }
//		 }
//		 */
//		if (configuration != UNSUPPORTED) {
//			for (int j = 0; j < linkConfig.numColumns(); j++) {
//				Vector temp(3);
//				for (int i = 0; i < 3; i++) {
//					temp(i) = linkConfig.value(i,j);
//				}
//				setLink( j, temp);
//			}
//		}
//	}

KinematicsStatus InverseKinematics::setLink(unsigned int linkIndex,
		const Vector& link) {
	if (linkIndex >= numLinks) {
		return KINEMATICS_OUT_OF_RANGE;
	}

	*links[linkIndex]->link = link;

	if (configuration == ZYYXZY) {
		if (linkIndex < 3) {
			return child->setLink(linkIndex, link);
		} else if (linkIndex == 3) {
			return child->setLink(3, *links[3]->link + *links[4]->link);
		} else if (linkIndex == 4) {
			return child->setLink(3, *links[3]->link + *links[4]->link);
		}
	}

	return KINEMATICS_NO_ERROR;
}

void InverseKinematics::setBase(const Matrix& location,
		const Quaternion& orientation) {
	if ((location.numRows() == 3) && (location.numColumns() == 1)) {
		*links[0]->location = location;
	}

	(*links[0]->orientation)(0) = orientation.valueAsConst(0, 0);
	(*links[0]->orientation)(1) = -orientation.valueAsConst(1, 0);
	(*links[0]->orientation)(2) = -orientation.valueAsConst(2, 0);
	(*links[0]->orientation)(3) = -orientation.valueAsConst(3, 0);
}

void InverseKinematics::findRelativeLocation() {
	// First translate relative to the base location:
	*relativeEELocation = *endEffectorLocation - *links[0]->location;
	// Then rotate about the base's orientation
	*relativeEELocation = links[0]->orientation->makeRotationMatrix()
			* *relativeEELocation;
	// Also apply the new angle to the endEffectorQuat:
	*relativeEEQuaternion = (*links[0]->orientation) * (*endEffectorQuaternion);
}

//	KinematicsStatus InverseKinematicsCF::compute3DOF( ) {
//		Vector newLocation;
//
//		newLocation = *relativeEELocation - (*links[0]->link);
//		newLocation.fastValue(2,0) -= links[1]->link->value(2,0);
//
//		// Find the distance to the end effector location projection on
//the XY plane:
//		float xyDist = sqrtf(square(newLocation(0,0)) +
//square(newLocation.fastValue(1,0)));
//		// Find the overal magnitude to the end effector:
//		float jointDist = newLocation.magnitude();
//		float tibiaDist = sqrtf( square(links[3]->link->value(2,0)) +
//square(links[3]->link->value(0,0)) );
//		float femurDist = links[2]->link->value(0,0);
//
//		motorScratch->fastValue(2, 0) = MOGI_PI +
//atan2f(links[3]->link->value(2,0), links[3]->link->value(0,0)) -
//lawCos(femurDist, tibiaDist, jointDist);
//		//if (motorScratch->fastValue(2, 0) != motorScratch->fastValue(2,
//0)) {
//		//		return KINEMATICS_OUT_OF_RANGE;
//		//}
//
//		//float lawOfCosinePart = ( square(femurDist) - square(tibiaDist)
//+ square(jointDist)) / (2*jointDist*femurDist);
//		//if ((lawOfCosinePart < -1) || (lawOfCosinePart > 1)) {
//		//	return KINEMATICS_OUT_OF_RANGE;
//		//}
//		float femurDiff  = lawCos(femurDist, jointDist, tibiaDist);
//		motorScratch->fastValue(1, 0) =
//-(atan2f(newLocation.fastValue(2,0), xyDist) + femurDiff);
//		motorScratch->fastValue(0, 0) = atan2f(
//newLocation.fastValue(1,0), newLocation.fastValue(0,0) );
//
//		if ((motorScratch->fastValue(0, 0) != motorScratch->fastValue(0,
//0)) ||
//			(motorScratch->fastValue(1, 0) != motorScratch->fastValue(1,
//0)) ||
//			(motorScratch->fastValue(2, 0) != motorScratch->fastValue(2,
//0))) {
//			return KINEMATICS_BAD_RESULT;	// This should never be
//reached if both acos values are pre-checked...
//		}
//
//		*motorAngles = *motorScratch;
//
//		return KINEMATICS_NO_ERROR;
//	}

// Uses an iterative method to solve for all motor angles given location and
// final oreintation in quaternions
KinematicsStatus InverseKinematicsOF::iterativeZYYXZYInverseKinematics() {
	// First find the location of joint 6:
	Matrix P6orientation = relativeEEQuaternion->makeRotationMatrix();
	*links[6]->location = *relativeEELocation
			- (P6orientation * (*links[6]->link));

	// Vector following the radius at t = 0:
	Matrix u(3, 1);
	u = P6orientation * (*links[6]->link);
	u.normalize();
	u *= -1;  // should be in opposite direction

	// Vector that defines the plane the circle is on:
	Matrix n(3, 1);
	n(1, 0) = 1;  // This particular method only fors for a 6th axis pointing in
	// the y-direction
	n = P6orientation * n;
	n.normalize();

	// Have to implement a search algorithm to determine t
	// Check for a NaN:
	if (t != t) {
		t = 0;
	}
	// Some parameters to define the circle:
	Matrix v1 = u * links[5]->link->valueAsConst(0, 0);
	Matrix v2 = n * links[5]->link->valueAsConst(0, 0);
	v2 = u.cross(v2);

	float error = 1000;
	float olderror = error;
	unsigned int iterations = 0;
	float direction = 1;

	Matrix P34norm(3, 1);
	Matrix P3NewUpNorm(3, 1);
	Matrix R1, R2, R3;
	Matrix uc;

	Vector childAngles;
	float ucDotP34;

	bool childSuccessful = false;
	while ((error > .005118 * 0.5) && (iterations < 1000)) {
		iterations++;

		// based on t, determine links[5]->location:
		*links[5]->location = *links[6]->location + cos(t) * v1 + sin(t) * v2;

		// Solve for first 3DOF kinematics:
		if (child->compute(*links[5]->location) == KINEMATICS_NO_ERROR) {
			childSuccessful = true;
			childAngles = child->getMotorAngles();

			// Perform some forward kinematics for Joints 3 and 4:
			R1.makeZRotation(childAngles(0));
			R2.makeYRotation(childAngles(1));
			R3.makeYRotation(childAngles(2));

			// Normalized vector from P3 to P4:
			P34norm = R1 * (R2 * (R3 * Vector::xAxis)); // WOAH!  This was originally
			// yAxis locally defined, but
			// as an X axis!  Decided to
			// keep it as an x axis

			// Find the tangent vector on the circle:
			uc = n.cross(*links[5]->location - *links[6]->location);
			uc.normalize();

			// Final dot product determines our error (missing cos(t), but the
			// normalized result is fine)
			ucDotP34 = uc.dot(P34norm);
		}

		if (!childSuccessful) {
			ucDotP34 = NAN;
		}
		if (ucDotP34 >= 0) {
			error = ucDotP34;
			if (error > olderror) {
				direction *= -.95;
			}
			t += ucDotP34 / 2 * direction;
		} else if (ucDotP34 < 0) {
			error = -ucDotP34;
			if (error > olderror) {
				direction *= -.95;
			}
			t += ucDotP34 / 2 * direction;
		} else {
			// We have a NaN somewhere, attempt to recover:
			savedT += MOGI_PI / 500;
			error = 1313;
			t = savedT;
		}
	}
	if (!childSuccessful) {
		return KINEMATICS_BAD_RESULT;
	}
	if (error > maxError) {
		return KINEMATICS_OVER_MAX_ERROR;
	}

	// We now have an acceptable solution for the last motor:
	t += MOGI_PI;
	if (t < 0) {
		t = 2 * MOGI_PI - fmodf(-t, 2 * MOGI_PI);
	} else {
		t = fmodf(t, 2 * MOGI_PI);
	}
	t -= MOGI_PI;
	motorScratch->value(5, 0) = t;

	// Using that solution, we can solve for the 5th joint angle using the cross
	// product definition method:
	Matrix P45norm = *links[6]->location - *links[5]->location;
	P45norm.normalize();
	Matrix P34normCrossP45norm = P34norm.cross(P45norm);
	float P34normCrossP45normDotUc = P34normCrossP45norm.dot(uc);
	motorScratch->value(4, 0) = asinf(P34normCrossP45normDotUc);
	if (((P34norm.dot(P45norm)) < 0)) {
		motorScratch->value(4, 0) = MOGI_PI - motorScratch->value(4, 0);
	}
	motorScratch->value(4, 0) = fmodf(motorScratch->value(4, 0) + MOGI_PI,
			2 * MOGI_PI) - MOGI_PI;

	// this is for getting a later orientation (does not need to be iterated)
	P3NewUpNorm.value(2, 0) = 1;
	P3NewUpNorm = R1 * (R2 * (R3 * P3NewUpNorm));
	// Cross product definition:
	Matrix P3UpCrossUc = P3NewUpNorm.cross(uc);
	float P3UpCrossUcDotP34norm = P3UpCrossUc.dot(P34norm);
	motorScratch->value(3, 0) = asinf(P3UpCrossUcDotP34norm);
	if (((P3NewUpNorm.dot(uc)) < 0)) {
		motorScratch->value(3, 0) = MOGI_PI - motorScratch->value(3, 0);
	}
	motorScratch->value(3, 0) = fmodf(motorScratch->value(3, 0) + MOGI_PI,
			2 * MOGI_PI) - MOGI_PI;

	// Load the solved angles:
	if ((motorScratch->value(3, 0) != motorScratch->value(3, 0))
			|| (motorScratch->value(4, 0) != motorScratch->value(4, 0))
			|| (motorScratch->value(5, 0) != motorScratch->value(5, 0))) {
		return KINEMATICS_BAD_RESULT;
	}

	motorAngles->value(0, 0) = childAngles.value(0, 0);
	motorAngles->value(1, 0) = childAngles.value(1, 0);
	motorAngles->value(2, 0) = childAngles.value(2, 0);
	motorAngles->value(3, 0) = motorScratch->value(3, 0);
	motorAngles->value(4, 0) = motorScratch->value(4, 0);
	motorAngles->value(5, 0) = motorScratch->value(5, 0);
	return KINEMATICS_NO_ERROR;
}

#define base (links[0]->link)
#define coxa (links[1]->link)
#define femur (links[2]->link)
#define tibia (links[3]->link)

KinematicsStatus InverseKinematicsCF::compute3DOFGeneral() {
	// This solution is a bit more general ,but take a bit more math to compute:
	Vector newLocation;

	// Vector *base = links[0]->link;
	// Vector *coxa = links[1]->link;
	// Vector *femur = links[2]->link;
	// Vector *tibia = links[3]->link;

	newLocation = *relativeEELocation - (*base);
	newLocation.value(2, 0) -= coxa->valueAsConst(2, 0); // Offset the z distance

	/////////////
	// First we can compute the coxa angle:
	/////////////
	// method to figure out coxa without femur angle dependence:
	float newTibiaY =
	coxa->value(1, 0) + femur->value(1, 0) +
	tibia->value(1, 0);  // treats femur and coxa as coplanar in zx
	float xyMag = sqrtf(
			square(newLocation.value(0, 0)) + square(newLocation.value(1, 0))); // magnitude of the location in x,y
	float delta = asin(newTibiaY / xyMag);     // Represents coxa angle offset
	float coxaTheta = atan2f(newLocation.value(1, 0), newLocation.value(0, 0))
			- delta;
	motorScratch->value(0, 0) = coxaTheta;

	/////////////
	// Build triangle of projected distances onto x-z plane:
	/////////////
	float femurzxMag = sqrtf(
			square(femur->value(0, 0)) + square(femur->value(2, 0))); // Femur must only be in x direction
	float tibiazxMag = sqrtf(
			square(tibia->value(0, 0)) + square(tibia->value(2, 0))); // tibia magnitude in x-z

	float projXmCoxaX = cos(delta) * xyMag - coxa->value(0, 0);
	float d = sqrtf(square(projXmCoxaX) + square(newLocation.value(2, 0)));

	/////////////
	// Solve for the femur:
	/////////////
	float femurOffset = atan2f(newLocation.value(2, 0), projXmCoxaX); // rolls the femur so the EE ends up
	// with the right Z location
	delta = asin(femur->value(2, 0) / femurzxMag); // Now represents femur angle offset
	motorScratch->value(1, 0) = delta
			- (femurOffset + lawCos(femurzxMag, d, tibiazxMag));

	/////////////
	// With known delta for the femur, solve for the tibia:
	/////////////
	delta = asin(tibia->value(2, 0) / tibiazxMag) - delta; // Now represents tibia angle offset
	motorScratch->value(2, 0) = delta
			+ (MOGI_PI - lawCos(tibiazxMag, femurzxMag, d)); // acos( v/tibiazxMag ) + delta;

//				if (barfExplode == 2) {
//					barfExplode = 3;
//					std::cout << "----------------------------" << std::endl;
//					newLocation.name("newLocation");
//					newLocation.print_stats();
//					relativeEELocation->name("relativeEELocation");
//					relativeEELocation->print_stats();
//					base->name("base");
//					base->print_stats();
//					std::cout << " - coxtheta:    " << coxaTheta << std::endl;
//					std::cout << " - femurzxMag:  " << femurzxMag << std::endl;
//					std::cout << " - tibiazxMag:  " << tibiazxMag << std::endl;
//					std::cout << " - femurOffset: " << femurOffset << std::endl;
//					std::cout << " - delta:       " << delta << std::endl;
//					std::cout << " - d:           " << d << std::endl;
//					std::cout << " - motorScratch->fastValue(2, 0) : "
//					<< motorScratch->fastValue(2, 0) << std::endl;
//					std::cout << std::endl;
//				} else if (barfExplode == 3) {
//					barfExplode = 1;
//				}

	if ((motorScratch->valueAsConst(0, 0) != motorScratch->valueAsConst(0, 0))
			|| (motorScratch->valueAsConst(1, 0)
					!= motorScratch->valueAsConst(1, 0))
			|| (motorScratch->valueAsConst(2, 0)
					!= motorScratch->valueAsConst(2, 0))) {
//					if (barfExplode == 1) {
//      barfExplode = 2;
//
//      std::cout << "----------------------------" << std::endl;
//      newLocation.name("newLocation");
//      newLocation.print_stats();
//      relativeEELocation->name("relativeEELocation");
//      relativeEELocation->print_stats();
//      base->name("base");
//      base->print_stats();
//
//      std::cout << " - coxtheta:    " << coxaTheta << std::endl;
//      std::cout << " - femurzxMag:  " << femurzxMag << std::endl;
//      std::cout << " - tibiazxMag:  " << tibiazxMag << std::endl;
//      std::cout << " - femurOffset: " << femurOffset << std::endl;
//      std::cout << " - delta:       " << delta << std::endl;
//      std::cout << " - d:           " << d << std::endl;
//      std::cout << " - motorScratch->fastValue(2, 0) : "
//						<< motorScratch->fastValue(2, 0) << std::endl;
//
//      std::cout << std::endl;
//					}

		return KINEMATICS_BAD_RESULT;
	}

	*motorAngles = *motorScratch;

	return KINEMATICS_NO_ERROR;
}

KinematicsStatus InverseKinematicsCF::computeYYXSolution() {
	// Note: the language of tibia, femur and coxa is used here, but it really looks like the tibia has 2 joints and there is no coxa joint.
	// Visually, the coxa omotor perates something that looks like a femur in ZYY kinematics, and the femur motor operates the tibia in ZYY kinematics.
	// The tibia motor here just looks like a twist in the tibia.

	// First get the relative location of the desired end effector location with respect to
	// the first motor shaft (the coxa).
	Vector newLocation;
	newLocation = *relativeEELocation - (*base);

	// Need to account for offsets in Y of the femur link:
	double tibiaRelativeYLocation = newLocation.value(1, 0)
			- (coxa->value(1, 0) + femur->value(1, 0));
	// First compute the angle of the last X axis motor since it is the only motor that may modify the Y position:
	double tibiaYZmagnitude = sqrt(
			square(tibia->value(1, 0)) + square(tibia->value(2, 0)));

	// Easy error check, reachable if tibiaYZmagnitude >= tibiaRelativeYLocation
	// Note that a larger tibiaYZmagnitude means more reachability.
	// Similarly, if the link is 0 in both Y and Z, then the relative location MUST already be in line
	// with the tibia motor, i.e. it is a 2ODF effective linkage system.
	if (tibiaYZmagnitude < tibiaRelativeYLocation) {
		return KINEMATICS_OUT_OF_RANGE;
	}
	// Determine the tibia angle.
	// Note, there are two solutions here, since we assume the motor to be high, we take the negative angle solution:
	double angleTibiaMotorToEE = -acos(
			tibiaRelativeYLocation / tibiaYZmagnitude);

	// Account for the intrinsic angle offset of the tibia:
	double tibiaAngle = angleTibiaMotorToEE
			- atan2(tibia->value(2, 0), tibia->value(1, 0));
	motorScratch->value(2, 0) = tibiaAngle;

	// Since the tibia is rotated, the projection onto the Z-X plane will have a modified Z location:
	double newZprojection = tibiaYZmagnitude * sin(angleTibiaMotorToEE);

	// Now there are 2DOF left.  First, set the "femur" separation to match the XY magnitude of the leg to the coxa:
	double footToFemurMotorX = femur->value(0, 0) + tibia->value(0, 0);
	double footToFemurMotorZ = femur->value(2, 0) + newZprojection;
	double footToFemurMotorZXmagnitude = sqrt(
			square(footToFemurMotorZ) + square(footToFemurMotorX));

	// Need the magnitude of the coxa link in ZX:
	double magnitudeZXcoxa = sqrt(
			square(coxa->value(0, 0)) + square(coxa->value(2, 0)));

	// preparing for the law of cosines, need the magnitude from the coxa to the the desired foot as well:
	double footToCoxaMotorZXmagnitude = sqrt(
			square(newLocation.value(0, 0)) + square(newLocation.value(2, 0)));

	// use law of cosines to determine the effective angle for the femur:
	double effectiveAngleFemur = (MOGI_PI
			- lawCos(magnitudeZXcoxa, footToFemurMotorZXmagnitude,
					footToCoxaMotorZXmagnitude));
	if (effectiveAngleFemur != effectiveAngleFemur) {
		return KINEMATICS_OUT_OF_RANGE;
	}

	double angleCoxaInitial = atan2(coxa->value(2, 0),
	coxa->value(0, 0));
	double femurAngle = effectiveAngleFemur - angleCoxaInitial
			+ atan2(footToFemurMotorZ, footToFemurMotorX);
	motorScratch->value(1, 0) = femurAngle;

	// Now determine the rigid assembly after the coxa based on the new femur angle:
	double coxaAngle = -lawCos(magnitudeZXcoxa, footToCoxaMotorZXmagnitude,
			footToFemurMotorZXmagnitude) + angleCoxaInitial
			- atan2(newLocation.value(2, 0), newLocation.value(0, 0));
	motorScratch->value(0, 0) = coxaAngle;

	// Check for any NaN's
	if ((motorScratch->valueAsConst(0, 0) != motorScratch->valueAsConst(0, 0))
			|| (motorScratch->valueAsConst(1, 0)
					!= motorScratch->valueAsConst(1, 0))
			|| (motorScratch->valueAsConst(2, 0)
					!= motorScratch->valueAsConst(2, 0))) {
		return KINEMATICS_BAD_RESULT;
	}

	*motorAngles = *motorScratch;

	return KINEMATICS_NO_ERROR;
}

const Vector makeEndEffectorVector(const Vector& location,
		const Quaternion& orientation) {
	Vector ret(7);
	for (unsigned int i = 0; i < 3; i++) {
		ret(i) = location.valueAsConst(i, 0);
	}
	for (unsigned int i = 0; i < 4; i++) {
		ret(i + 3) = orientation.valueAsConst(i, 0);
	}
	return ret;
}
}
}

#ifdef _cplusplus
}
#endif
