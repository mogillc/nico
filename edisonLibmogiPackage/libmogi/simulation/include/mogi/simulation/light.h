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

#ifndef MOGI_LIGHT_H
#define MOGI_LIGHT_H

#include "camera.h"
#include "framebuffer.h"
#include "mesh.h"
#include "mogi/math/mmath.h"
#include "mogi/math/node.h"
#include "shader.h"
#include "texture.h"

#include <vector>

#include <assimp/scene.h>

namespace Mogi {
namespace Simulation {

class MBlight {
protected:
	enum {
		TYPE_SPOT, TYPE_POINT, TYPE_DIRECTIONAL, NUM_TYPES
	};

	int type;

	std::string name;
	Math::Node* parentNode;

	FrameBuffer* frameBuffer;
	// GLuint FBO;
	// GLuint shadowMap;
	// GLuint renderTexture;
	int width, height;
	bool shadowEnable;
	Math::Vector location;
	Math::Quaternion orientation;
	Math::Vector color;
	// Shader *shadowShader;
	bool shadowsAllocated;

	Math::Matrix shadowMapMatrix;
	double FOV;

	Camera lightAsCamera;  // for computing shadowmap matrices

	float intensity;

public:
	static MBlight* create(aiLight* light);
	// Attributes:

	// Methods:
	MBlight();
	virtual ~MBlight();

	virtual void set(aiLight* light) = 0;

	void setLocation(double x, double y, double z);
	void setLocation(Math::Vector loc);
	Math::Vector& getLocation() {
		return location;
	}
	;
	float* getLocationPointer() {
		return location.dataAsFloat();
	}
	;

	// void setOrientation(double x, double y, double z);
	void setOrientation(Math::Quaternion ori);
	Math::Quaternion& getOrientation() {
		return orientation;
	}
	;
	float* getOrientationPointer() {
		return orientation.dataAsFloat();
	}
	;

	void setColor(double r, double g, double b);
	void setColor(Math::Vector col);
	Math::Vector& getColor() {
		return color;
	}
	;
	float* getColorPointer() {
		return color.dataAsFloat();
	}
	;

	void setFOV(double value) {
		FOV = value;
		lightAsCamera.setFOV(FOV);
	}
	;
	double getFOV() {
		return FOV;
	}
	;

	void setEnabled(bool value) {
		shadowEnable = value;
	}
	;
	bool isEnabled() {
		return shadowEnable;
	}
	;

	bool prepareShadowMap();
	void finishShadowMap();

	void setShadowUniforms(MBshader* shader, int location);

	const Math::Matrix& buildShadowMapMatrix(const Math::Matrix& modelMatrix);

	void findNode(Math::Node* rootNode);

	void updateLightCamera();
	Camera* getCamera() {
		return &lightAsCamera;
	}
	;

	virtual void sendToShader(MBshader* shader, const Math::Matrix& modelMatrix,
			int index) = 0;
};

class MBspotLight: public MBlight {
private:
	float innerCone;
	float outerCone;

public:
	void set(aiLight* light);
	void sendToShader(MBshader* shader, const Math::Matrix& modelMatrix,
			int index);
};

class MBpointLight: public MBlight {
private:
	//	Mesh *lightMesh;
	float attenuationQuadratic;
	float attenuationLinear;
	float attenuationConstant;

	float sphereSize;

	void updateSphere();

public:
	void set(aiLight* light);
	void sendToShader(MBshader* shader, const Math::Matrix& modelMatrix,
			int index);
};

class MBdirectionalLight: public MBlight {
public:
	void set(aiLight* light);
	void sendToShader(MBshader* shader, const Math::Matrix& modelMatrix,
			int index);
};
}
}

#endif
