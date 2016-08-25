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

#ifndef MOGI_POST_PROCESS_H
#define MOGI_POST_PROCESS_H

#include "camera.h"
#include "framebuffer.h"
#include "mesh.h"
#include "scene.h"
#include "dynamicShader.h"

namespace Mogi {
namespace Simulation {

class MBpostprocess {
private:
	MBshader* basicShader;
	FinalProcessShaderParameters mFinalProcessShaderParameters;

protected:
	Camera camera;  // this wil be set as an orthographic projection

	Math::Matrix biasMatrix;

	Math::Matrix modelViewProjectionMatrix;
	Math::Matrix biasedModelViewProjectionMatrix;

	MBmesh renderPlane;

	FrameBuffer* frameBuffer;

	bool isFinalRender;

	void initBuffers();
	void finishBuffers();

	void drawPlane(MBshader* shader);

public:
	MBpostprocess(int xRes, int yRes);
	~MBpostprocess();

	void setAsFinalRender(bool value);

	int process(Texture& renderTexture, Camera& renderCamera);

	Texture& getDepthTexture();
	Texture& getRenderTexture(int index);
	FrameBuffer& getFrameBuffer() {
		return *frameBuffer;
	}
	;

	void resize(int xRes, int yRes);
};

class MBbokeh: public MBpostprocess {
private:
	// Bokeh parameters:
	GLfloat centerDepth;
	GLfloat focalDepth;
	GLfloat fLength;
	GLfloat fstop;
	bool debugFocus;
	bool autofocus;
	bool vignetteEnable;

	BokehShader* bokehShader;

public:
	MBbokeh(int xRes, int yRes);

	int process(Texture& renderTexture, Texture& depthTexture,
			Camera& renderCamera);

	void setAutoFocus(bool value) {
		autofocus = value;
	}
	;
	void setVignetteEnabled(bool value) {
		vignetteEnable = value;
	}
	;
	void setFstop(GLfloat value) {
		fstop = value;
	}
	;
	void setFdepth(GLfloat value) {
		focalDepth = value;
	}
	;
	void setFlength(GLfloat value) {
		fLength = value;
	}
	;
	void setDebugMode(bool value) {
		debugFocus = value;
	}
	;

	GLfloat getFstop() {
		return fstop;
	}
	;
	GLfloat getFdepth() {
		return focalDepth;
	}
	;
	GLfloat getFlength() {
		return fLength;
	}
	;
	GLfloat getCenterDepth() {
		return centerDepth;
	}
	;
};

class MBdeferredLighting: public MBpostprocess {
private:
	MBshader* lightingShader;
	DeferredLightingShaderParameters mDeferredLightingShaderParameters;

public:
	MBdeferredLighting(int xRes, int yRes);

	int process(MBGBuffer* geometryBuffer, Camera& renderCamera,
			std::vector<MBlight*>& lights);
};
}
}

#endif
