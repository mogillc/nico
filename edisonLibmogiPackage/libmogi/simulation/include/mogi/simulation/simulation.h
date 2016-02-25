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

#ifndef SIMULATION_H
#define SIMULATION_H

#include "scene.h"
#include "mogi/robot/hexapod.h"

namespace Mogi {
namespace Simulation {

#define ANGLE_LIMIT_ERROR -1
#define OUT_OF_RANGE_ERROR -2

enum gaits {
	RIPPLE, TRIPOD, QUAD, WAVE, MAX_NUMBER_OF_GAITS
};

class Simulation {
private:
	// GLfloat *thetac, *thetaf, *thetat;
	Hexapod hexapod;
	//    Scene *coxa;
	//    Scene *femur;
	//    Scene *tibia;

	Vector cameraLocation;
	Vector cameraOrientation;
	// NSMutableArray *myArray;

	GLfloat *coxad;
	GLfloat *coxa_x;
	GLfloat *coxa_y;
	GLfloat *femurd;
	GLfloat *tibiad;

	GLuint *tibiaError;
	GLuint *femurError;
	GLuint *coxaError;

	GLfloat *coxaCenter;
	GLfloat *coxaRange;
	GLfloat *femurCenter;
	GLfloat *femurRange;
	GLfloat *tibiaCenter;
	GLfloat *tibiaRange;

	GLfloat thickness;
	GLfloat *legPhase;

	int totalTriangles;

public:
	Scene *body;

	GLKMatrix4 baseModelViewMatrix;GLKMatrix4 projectionMatrix;
	MBmesh *vObj;
	GLfloat *thetac;
	GLfloat *thetaf;
	GLfloat *thetat;
	GLfloat *thetab;

	GLKVector3 headTheta;GLKVector3 bodyLocation;GLKVector3 bodyLocationDesired;GLKVector3 bodyVelocities;GLKVector3 bodyAngularVelocities;GLKVector3 bodyAngleDesired;GLKVector3 angles;GLKVector3 bodyLocationOffset;GLKVector3 anglesOffset;GLKVector3 *feetLocations;

	std::string gaitLabel;

	Simulation();
	~Simulation();
	void buildHexapod();
	int generateVertexObjects();
	int getNumberOfTriangles();
	void update(Hexapod *hex);

	const Vector &getCameraOrientation() {
		return cameraOrientation;
	}
	;
	const Vector &getCameraLocation() {
		return cameraLocation;
	}
	;
};
}
}

#endif
