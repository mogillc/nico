/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *            This program is distributed under the LGPL, version 2           *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License              *
 *   version 2.1 as published by the Free Software Foundation;                *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   https://github.com/mogillc/nico/tree/master/edisonLibmogiPackage/libmogi *
 *                                                                            *
 *****************************************************************************/

#ifndef MOGI_KINEMATICS_H
#define MOGI_KINEMATICS_H

#include "mogi/math/mmath.h"

namespace Mogi {

/**
 * @namespace Mogi::Math
 * \brief Math tools, mainly focused on matrices, vectors, and quaternions.
 */
namespace Math {

/*! Radius of the RX/MX-28 hinges. */
#define RX_28_HINGE_OFFSET (25.5)
/*! Thickness of a side bracket with respect to the mounting hole. */
#define RX_28_SIDE_OFFSET (5.7)
/*! Distance from the rotation axis to the bottom mounting holes. */
#define RX_28_LONG_HOLE_OFFSET (35.8)
/*! Distance from the rotation axis to the side mounting holes. */
#define RX_28_SIDE_HOLE_OFFSET (15)
/*! Hinge width, distance from the horn surface to idler surface. */
#define RX_28_HORN_TO_IDLER (35.5 + 2.75 * 2)

/*! Distance for a specific 3D printed bracket to mount two EX/MX-106 motors. */
#define EX_106_DOUBLE_MATE (110)
/*! Hinge width, distance from the horn surface to idler surface. */
#define EX_106_HORN_TO_IDLER (46.0 + 3.0 * 2)
/*! Radius of the 3D printed hinge design. */
#define EX_106_HINGE_RADIUS_BUNTING (30.0)

/*! Hinge width, distance from the horn surface to idler surface. */
#define RX_64_HORN_TO_IDLER (41.0 + 3.0 * 2)
/*! Radius of the off the shelf brackets for the RX/MX-64. */
#define RX_64_HINGE_RADIUS (30.0)
/*! Overall width of the hinge. */
#define RX_64_HINGE_WIDTH (RX_64_HORN_TO_IDLER + 2 * 2.0)

/*! For a specific 3D printed bracket. */
#define RX_64_ANGLE_BRACKET_BUNTING (23.5)
/*! For a specific 3D printed bracket. */
#define RX_28_COLUMN_BUNTING (45.0)
/*! For a specific 3D printed bracket. */
#define END_EFFECTOR_CC (35.0)

/**
 Return values of the inverse kinematic functions.
 */
typedef enum {
	KINEMATICS_NO_ERROR = 0,
	KINEMATICS_INVALID_PARAMETER = -1,
	KINEMATICS_OUT_OF_RANGE = -2,
	KINEMATICS_BAD_CONFIGURATION = -3,
	KINEMATICS_OVER_MAX_ERROR = -4,
	KINEMATICS_BAD_RESULT = -5
} KinematicsStatus;

/**  Computes forward kinematics to get end effector location and orientation
 for the 6DOF arm.

 For the arm defined by axis ZYYXZY, the end effector location and orientation
 is calculated.
 */
// const Vector forwardKinematics( Matrix& linkConfig, Vector& angles );
/**  (Deprecated) Converts the motor angles to match the arm.

 This function very specifically converts the solution angles to an AX-12 arm.
 */
// const Vector convertToMotorAngles(Vector& desiredAngles);
/**  \brief Combines location and orientation into a vector.

 Many of the functions require a size 7 vector combining location and
 orientation quaternion, so this function append the quaternion to the location
 vector and returns.
 */
const Vector makeEndEffectorVector(const Vector& location,
		const Quaternion& orientation);

/** \struct Link
 \brief Definition of a rigid segment between two joints.
 */
typedef struct {
	/**
	 The 3D vector that defines the segment base and end attachments.
	 */
	Vector* link;

	/**
	 The orientation of the segment.
	 */
	Quaternion* orientation;

	/**
	 The location of the base of the segment.
	 */
	Vector* location;
} Link;

/** \class InverseKinematics
 \brief Computes inverse kinematics for specific set of configurations.
 */
class InverseKinematics {
public:
	/** The supported configuration types.
	 These are the only types that are currently supported, each representing the
	 rotation axis of the robotic arm.
	 */
	typedef enum {
		UNSUPPORTED, /**< Unsupported configuration */
		ZYY, /**< Configuration of an arm with rotations at the z, y, then y axis */
		YYX, /**< Configuration of an arm with rotations at the y, y, then x axis */
		ZYYY, /**< Configuration of an arm with rotations at the z, y, y, then y
		 axis */
		ZYYYX, /**< Configuration of an arm with rotations at the z, y, y, y, then x
		 axis */
		ZYYXZY, /**< Configuration of an arm with rotations at the z, y, y, x, z,
		 then y axis */
		NUM_CONFIGURATIONS /**< Number of supported configurations */
	} ConfigurationType;

protected:
	/** The computed motor angles.
	 */
	Vector* motorAngles;
	/** scratch space for computing motor angles.
	 */
	Vector* motorScratch;

	/** The configuration type of this instance.
	 */
	ConfigurationType configuration;

	/** The number of links in this instance including the base.  Note, a 3DOF arm
	 contains 3 links, 6DOF contains 7, etc.
	 */
	unsigned int numLinks;

	/**
	 The desired orientation of the end effector.
	 */
	Quaternion* endEffectorQuaternion;
	/**
	 The desired location of the end effector.
	 */
	Vector* endEffectorLocation;

	/**
	 The relative orientation of the end effector with respect to the base.
	 */
	Quaternion* relativeEEQuaternion;
	/**
	 The relative location of the end effector with respect to the base.
	 */
	Vector* relativeEELocation;

	/**
	 The set of links that define the arm.
	 */
	Link** links;

	/**
	 Some solutions involve solving for a subset of inverse kinematics, handled
	 here.
	 */
	InverseKinematics* child;

	/**
	 Computes the relative end effector location and orientation based on the base
	 and end effector settings.
	 */
	void findRelativeLocation();

	/**
	 Fills out the end effector location and orientation based on the input
	 vector.
	 This function is weird and should be removed.
	 @param params The vector containing the end effector data.
	 @return Any errors if params is invalid.
	 */
	KinematicsStatus handleParameters(const Vector& params);

public:

	/**  Set the base location and orientation.

	 This sets the location and orientation of the base.  This does not need to be
	 called, but will default to location at the origin and no rotation.  This is
	 useful when needing to call for inverse kinematics in global coordinates
	 where the segment location is known.
	 @param location The 3D location of the base.
	 @param orientation The orientation of the base.
	 */
	void setBase(const Matrix& location, const Quaternion& orientation);

	/**  Computes Inverse Kinematics.
	 Calling this computes the inverse kinematics solution based on the configured
	 setup.  Configuration must be performed first.
	 @param location the desired end effector location.  This may be coupled with
	 the end effector orientation.
	 @return The status on the attempt to compute the angles.
	 */
	virtual KinematicsStatus compute(const Vector& location) = 0;

	/**  Returns the solution of inverse kinematics.
	 The solution to the inverse kinematics is returned by calling this function.
	 @return The previously solved motor angles.
	 */
	const Vector& getMotorAngles();

	/**  Sets the link configuration for the arm.

	 The link length configuration is set using a matrix of 3D column vectors,
	 where each ascending column is the next link.  The links typically should
	 extend in the x direction, as a general solution has not yet been found.  If
	 an unsupported configuration is implemented, the error is set.
	 */
	// void setConfiguration( Matrix &linkConfig );//, Matrix &axisConfig );
	/**  Sets the overall configuration.
	 Inverse kinematics is tricky to solve for the general case.  This sets the
	 type based on ConfigurationType.
	 @param configurationType The specific type of configuration to try to solve
	 for.
	 */
	void setType(ConfigurationType configurationType);

	/**  Creates a InverseKinematicsOF or InverseKinematicsCF object based on the
	 configuration type of the arm.
	 @param configurationType The specific type of configuration to try to solve
	 for.
	 */
	static InverseKinematics* create(ConfigurationType configurationType);

	/**  Creates a InverseKinematicsOF or InverseKinematicsCF object based on the
	 configuration type of the arm.
	 @param linkIndex The link to be defined.  For a 3DOF arm, the valid values
	 are 0, 1, 2, and 3.
	 @param link The 3D vector that defines the rigid link.
	 @return The status on the attempt to set the link.
	 */
	KinematicsStatus setLink(unsigned int linkIndex, const Vector& link);

	InverseKinematics();
	virtual ~InverseKinematics();
};

/** \class InverseKinematicsCF
 \brief The set of inverse kinematics solutions for closed formed solutions.

 This solves various forms of inverse kinematics where closed from solutions are
 easily determined.
 */
class InverseKinematicsCF: public InverseKinematics {
private:
	KinematicsStatus ZYYInverseKinematics(Vector& location);

	// KinematicsStatus compute3DOF( );
	KinematicsStatus compute3DOFGeneral();
	KinematicsStatus computeYYXSolution();

public:
	KinematicsStatus compute(const Vector& location);
};

/** \class InverseKinematicsOF
 \brief The set of inverse kinematics solutions for open formed solutions.

 This solves various forms of inverse kinematics where no closed formed solution
 exists, and therefore iterative methods are used to minimize error.
 */
class InverseKinematicsOF: public InverseKinematics {
private:
	float t;  // the angle of the 6th axis, to be iterated
	float savedT;
	float maxError;

	/**  Solves for the ZYYXZY arm.
	 This function uses a circle based on the radius of link 5, located at the 6th
	 motor (Y).  A 3DOF solution is given based on a point on the circle (ZYYXZ),
	 since the configuration of motors 4 and 5 do not effect the location of link
	 5.  The angle of the 6th motor is guessed, then the 3DOF solution determined
	 if motor 5 can be placed tangent to the circle.  The angle is adjusted until
	 the error is below a threshold.
	 */
	KinematicsStatus iterativeZYYXZYInverseKinematics();

public:
	KinematicsStatus compute(const Vector& location);

	InverseKinematicsOF() :
			InverseKinematics(), t(0), savedT(MOGI_PI), maxError(.005118 * 0.5) {
	}
};

}

}

#endif
