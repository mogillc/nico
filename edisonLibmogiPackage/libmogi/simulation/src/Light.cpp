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

#include "light.h"
#include <math.h>
using namespace std;

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;
	using namespace Math;

	MBlight::MBlight() {
		width = 1000;
		height = 1000;
		setLocation(0, 0, 0);
		// setOrientation(0, 0, 0);
		setColor(1.0, 1.0, 1.0);
		setEnabled(true);

		intensity = 1;

		shadowsAllocated = false;
		// createTextures();

		shadowMapMatrix.makeI(4);
		FOV = 50;

		frameBuffer = new FrameBuffer(width, height);
		frameBuffer->resize(width, height);// still needed?
	}

	MBlight::~MBlight() {
		delete frameBuffer;
	}

	void MBlight::setName(const std::string& name) {
		this->name = name;
	}

	bool MBlight::prepareShadowMap() {
		if (shadowEnable) {
			frameBuffer->attachFramebuffer();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glViewport(0, 0, width, height);

			glEnable (GL_DEPTH_TEST);
			glEnable (GL_CULL_FACE);
			glCullFace (GL_FRONT);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		return shadowEnable;
	}

	void MBlight::finishShadowMap() {
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
		frameBuffer->removeFramebuffer();
	}

	void MBlight::setShadowUniforms(MBshader *shader, int location) {
		char str[64];
		frameBuffer->setDepthUniform("shadowMap");
		frameBuffer->setDepthIndex(location);
		frameBuffer->sendTexturesToShader(shader);

		sprintf(str, "shadowEnable[%d]", location);
		shader->sendInteger(str, shadowEnable);
		sprintf(str, "mapWidth[%d]", location);
		shader->sendFloat(str, width);
		sprintf(str, "mapHeight[%d]", location);
		shader->sendFloat(str, height);
	}

	void MBlight::setLocation(double x, double y, double z) {
		location.setLength(3);
		location(0) = x;
		location(1) = y;
		location(2) = z;
		lightAsCamera.setLocation(location);
	}

	void MBlight::setLocation(Vector loc) {
		if (loc.size() == 3) {
			location = loc;
			lightAsCamera.setLocation(location);
		} else {
			std::cout << "Error! cannot set light location, size of vector should be 3, this is size " << loc.size() << std::endl;
		}
	}

	void MBlight::setColor(double r, double g, double b) {
		color.setLength(3);
		color(0) = r;
		color(1) = g;
		color(2) = b;
		// color(3) = 1.0;
	}

	void MBlight::setColor(Vector col) {
		if (col.size() == 3) {
			color = col;
		} else {
			std::cout << "Error! cannot set light location, size of vector should be 3, this is size " << col.size() << std::endl;
		}
	}


	void MBlight::setOrientation(Quaternion ori) {
		orientation = ori;
		lightAsCamera.setOrientation(orientation);
	}

	void MBlight::updateLightCamera() {
		lightAsCamera.setClipBounds(0.1, 20);
		lightAsCamera.setFOV(FOV);
		lightAsCamera.setResolution(width, height);
		lightAsCamera.setOrientation(orientation);
		lightAsCamera.setLocation(location);

		lightAsCamera.update();

		lightAsCamera.setModelMatrix(*parentNode->getModelMatrix());
	}

	const Matrix &MBlight::buildShadowMapMatrix(const Matrix &modelMatrix) {

		// lightAsCamera.update();
		updateLightCamera();

		Matrix biasMatrix(4, 4);
		biasMatrix(0, 0) = 0.5;
		biasMatrix(1, 1) = 0.5;
		biasMatrix(2, 2) = 0.5;
		biasMatrix(3, 3) = 1.0;
		biasMatrix(0, 3) = 0.5;
		biasMatrix(1, 3) = 0.5;
		biasMatrix(2, 3) = 0.5;

		// projection.name("camera.projectionMatrix");
		// view.name("camera.viewMatrix");
		biasMatrix.name("biasMatrix");

		shadowMapMatrix.name("shadowMapMatrix");
		shadowMapMatrix = biasMatrix * lightAsCamera.getProjectionMatrix();

		// lightAsCamera.setModelMatrix(parentNode->getModelMatrix());
		shadowMapMatrix = shadowMapMatrix * lightAsCamera.getViewMatrix();
		shadowMapMatrix = shadowMapMatrix * modelMatrix;

		biasMatrix = *parentNode->getModelMatrix();
		location(0) = biasMatrix(0, 3);
		location(1) = biasMatrix(1, 3);
		location(2) = biasMatrix(2, 3);

		return shadowMapMatrix;
	}

	void MBlight::findNode(Node *rootNode) {
		if ((parentNode = rootNode->findChildByName(name)) == NULL) {
			std::cout << "Error!  Could not find the node that matches light: "
			<< name << std::endl;
		} else {
			// Set the parent node offset matrix:
			// parentNode->setOffsetMatrix(offsetMatrix);
		}
	}

	void MBpointLight::sendToShader(MBshader *shader, const Matrix& modelMatrix,
									int index) {
		Matrix shadowMatrix = buildShadowMapMatrix(modelMatrix);
		char variableName[64];

		sprintf(variableName, "lightModelViewProjectionMatrix[%d]", index);
		shader->sendMatrix(variableName, shadowMatrix);

		sprintf(variableName, "lightPos[%d]", index);
		shader->sendMatrix(variableName, location);

		sprintf(variableName, "lightColor[%d]", index);
		shader->sendMatrix(variableName, color);

//		sprintf(variableName, "kc[%d]", index);
//		shader->sendFloat(variableName, attenuationConstant);
//
//		sprintf(variableName, "kl[%d]", index);
//		shader->sendFloat(variableName, attenuationLinear);
//
//		sprintf(variableName, "kq[%d]", index);
//		shader->sendFloat(variableName, attenuationQuadratic);

		Vector kAtt(3);
		kAtt(0) = attenuationConstant;
		kAtt(1) = attenuationLinear;
		kAtt(2) = attenuationQuadratic;
		
		sprintf(variableName, "kAtt[%d]", index);
		shader->sendMatrix(variableName, kAtt);
	}

	void MBspotLight::sendToShader(MBshader *shader, const Matrix& modelMatrix,
								   int index) {
		Matrix shadowMatrix = buildShadowMapMatrix(modelMatrix);
		char variableName[64];

		sprintf(variableName, "lightModelViewProjectionMatrix[%d]", index);
		shader->sendMatrix(variableName, shadowMatrix);

		sprintf(variableName, "lightPos[%d]", index);
		shader->sendMatrix(variableName, location);

		sprintf(variableName, "lightColor[%d]", index);
		shader->sendMatrix(variableName, color);

//		sprintf(variableName, "kc[%d]", index);
//		shader->sendFloat(variableName, attenuationConstant);
//
//		sprintf(variableName, "kl[%d]", index);
//		shader->sendFloat(variableName, attenuationLinear);
//
//		sprintf(variableName, "kq[%d]", index);
//		shader->sendFloat(variableName, attenuationQuadratic);

		Vector kAtt(3);
		kAtt(0) = attenuationConstant;
		kAtt(1) = attenuationLinear;
		kAtt(2) = attenuationQuadratic;

		sprintf(variableName, "kAtt[%d]", index);
		shader->sendMatrix(variableName, kAtt);
	}

	void MBdirectionalLight::sendToShader(MBshader *shader,
										  const Matrix &modelMatrix, int index) {
		Matrix shadowMatrix = buildShadowMapMatrix(modelMatrix);
		char variableName[64];

		sprintf(variableName, "lightModelViewProjectionMatrix[%d]", index);
		shader->sendMatrix(variableName, shadowMatrix);

		sprintf(variableName, "lightPos[%d]", index);
		shader->sendMatrix(variableName, location);

		sprintf(variableName, "lightColor[%d]", index);
		shader->sendMatrix(variableName, color);
	}

	void MBspotLight::setAttenuationFactors(float constant, float linear, float quadratic) {
		this->attenuationConstant = constant;
		this->attenuationLinear = linear;
		this->attenuationQuadratic = quadratic;
	}

	void MBspotLight::setCone(float innerCone, float outerCone) {
		this->innerCone = innerCone;
		this->outerCone = outerCone;
	}

	void MBpointLight::setAttenuationFactors(float constant, float linear, float quadratic) {
		this->attenuationConstant = constant;
		this->attenuationLinear = linear;
		this->attenuationQuadratic = quadratic;
	}
	void MBpointLight::updateSphere() {
		float maxChannel = fmax(fmax(color(0), color(1)), color(2));
		sphereSize = (-attenuationLinear
					  + sqrtf(
							  attenuationLinear * attenuationLinear
							  - 4 * attenuationQuadratic
									* (attenuationConstant
									   - 256 * maxChannel * intensity)))
		/ (2 * attenuationQuadratic);
	}
	
#ifdef _cplusplus
}
#endif
