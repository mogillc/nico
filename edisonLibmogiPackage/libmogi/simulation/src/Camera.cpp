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

#include "camera.h"
#include <math.h>
#include <iostream>

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;
using namespace Math;

Camera::Camera() {
	projection.makeI(4);
	view.makeI(4);
	farClip = 100;
	nearClip = .1;
	xResolution = 1280;
	yResolution = 720;
	aspect = fabsf((float) xResolution / (float) yResolution);
	FOV = 65;
	isProjection = true;
}

	void Camera::setName( std::string name ) {
		this->name = name;
	}

//void Camera::set(aiCamera* camera) {
//	aspect = camera->mAspect;
//	farClip = camera->mClipPlaneFar;
//	nearClip = camera->mClipPlaneNear;
//	FOV = camera->mHorizontalFOV;
//	name = camera->mName.C_Str();
//	orientation(0) = camera->mPosition[0];
//	orientation(1) = camera->mPosition[1];
//	orientation(2) = camera->mPosition[2];
//	// NOT FINISHED!
//	// camera->mUp // the Up vector;
//	// camera->mLookAt // the vector that the camera is looking at
//}

void Camera::makeProjection(int xRes, int yRes, float fov, float clipNear,
		float clipFar) {
	xResolution = xRes;
	yResolution = yRes;
	farClip = clipFar;
	nearClip = clipNear;
	FOV = fov;
	aspect = fabsf((float) xResolution / (float) yResolution);

	isProjection = true;
}

void Camera::makeOrthographic(float width, float height, float clipNear,
		float clipFar) {
	farClip = clipFar;
	nearClip = clipNear;
	imageHeight = height;
	imageWidth = width;

	isProjection = false;
}

void Camera::update() {
	if (isProjection) {
		aspect = fabsf((float) xResolution / (float) yResolution);
		projection = GLKMatrix4MakePerspective(FOV * MOGI_PI / 180.0, aspect,
				nearClip, farClip);
	} else {
		projection = GLKMatrix4MakeOrtho(-imageWidth / 2, imageWidth / 2,
				-imageHeight / 2, imageHeight / 2, nearClip, farClip);
	}

	// Main camera view:
	// view.makeI(4); // loads an identity matrix

	// view = GLKMatrix4Rotate( view, -orientation(0) * M_PI/180.0, 1.0f, 0.0f,
	// 0.0f);
	// view = GLKMatrix4Rotate( view, -orientation(1) * M_PI/180.0, 0.0f, 1.0f,
	// 0.0f);
	// view = GLKMatrix4Rotate( view, -orientation(2) * M_PI/180.0, 0.0f, 0.0f,
	// 1.0f);
	view = orientation.makeRotationMatrix4();
	view = view.transpose()
			* GLKMatrix4MakeTranslation(-location(0), -location(1),
					-location(2));
	// view *= GLKMatrix4MakeTranslation( -location(0), -location(1),
	// -location(2));
}

void Camera::setModelMatrix(const Matrix& model) {
	view = model;
	Matrix temp2;

	float scale = sqrt(
			model.valueAsConst(0, 0) * model.valueAsConst(0, 0)
					+ model.valueAsConst(0, 1) * model.valueAsConst(0, 1)
					+ model.valueAsConst(0, 2) * model.valueAsConst(0, 2));
	temp2.makeI(4);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			temp2(i, j) = model.valueAsConst(j, i) / scale;  // transpose
		}
	}
	view.makeI(4);
	view(0, 3) = -model.valueAsConst(0, 3);
	view(1, 3) = -model.valueAsConst(1, 3);
	view(2, 3) = -model.valueAsConst(2, 3);
	view = temp2 * view;
}

void Camera::setResolution(int x, int y) {
	xResolution = x;
	yResolution = y;
}

void Camera::setLocation(double x, double y, double z) {
	location.setLength(3);
	location(0) = x;
	location(1) = y;
	location(2) = z;
}

void Camera::setLocation(const Vector& loc) {
	if (loc.size() == 3) {
		location = loc;
	} else {
		std::cout
				<< "Error! cannot set light location, size of vector should be "
						"3, this is size " << loc.size() << std::endl;
	}
}

// void Camera::setOrientation(double x, double y, double z)
//{
//	orientation.set_size(3);
//	orientation(0) = x;
//	orientation(1) = y;
//	orientation(2) = z;
//}

void Camera::setOrientation(const Quaternion& ori) {
	orientation = ori;
}

const Matrix& Camera::getProjectionMatrix() {
	return projection;
}

Matrix& Camera::getViewMatrix() {
	return view;
}

float* Camera::getProjectionData() {
	return projection.dataAsFloat();
}

float* Camera::getViewData() {
	return view.dataAsFloat();
}

#ifdef _cplusplus
}
#endif
