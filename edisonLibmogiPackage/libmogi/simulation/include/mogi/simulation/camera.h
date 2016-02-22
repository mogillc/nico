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

#ifndef MOGI_CAMERA_H
#define MOGI_CAMERA_H

#include <stdio.h>
#include "mogi/math/mmath.h"

#include <vector>
#include <string>
//#include <assimp/scene.h>

namespace Mogi {
namespace Simulation {

class Camera {
private:
	std::string name;

	bool isProjection;
	Math::Matrix projection;

	float imageWidth;
	float imageHeight;

	Math::Matrix view;

	Math::Vector location;
	Math::Quaternion orientation;

	int xResolution;
	int yResolution;
	double FOV;

	float farClip;
	float nearClip;
	float aspect;

public:
	Camera();

	void setResolution(int x, int y);
	int getXresolution() {
		return xResolution;
	}
	;
	int getYresolution() {
		return yResolution;
	}
	;
	float getNearClip() {
		return nearClip;
	}
	;
	float getFarClip() {
		return farClip;
	}
	;

	float getAspect() {
		return aspect;
	}
	;

	//void set(aiCamera* camera);
	void update();

	void setName( std::string name );

	void makeOrthographic(float width, float height, float clipNear,
			float clipFar);
	void makeProjection(int xRes, int yRes, float fov, float clipNear,
			float clipFar);
	void setFOV(double value) {
		FOV = value;
	}
	;
	double getFOV() {
		return FOV;
	}
	;

	void setLocation(double x, double y, double z);
	void setLocation(const Math::Vector& loc);
	const Math::Vector& getLocation() {
		return location;
	}
	;
	float* getLocationPointer() {
		return location.dataAsFloat();
	}
	;

	// void setOrientation(double x, double y, double z);
	void setOrientation(const Math::Quaternion& ori);
	const Math::Quaternion& getOrientation() {
		return orientation;
	}
	;
	float* getOrientationPointer() {
		return orientation.dataAsFloat();
	}
	;

	void setModelMatrix(const Math::Matrix& model);

	const Math::Matrix& getProjectionMatrix();
	float* getProjectionData();
	Math::Matrix& getViewMatrix();
	float* getViewData();
};
}
}

#endif
