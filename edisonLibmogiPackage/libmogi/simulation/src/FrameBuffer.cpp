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

#include "mogiGL.h"
#include "framebuffer.h"
#include <iostream>



#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

_FrameBuffer::_FrameBuffer( int xRes, int yRes ) {
	//	setup();
}

void _FrameBuffer::initialize(int numberOfRenderTextures, int xRes, int yRes) {

	xResolution = xRes;
	yResolution = yRes;

	// std::cout << "Creating depthTexture: " << std::endl;
	depthTexture.create(xResolution, yResolution, true);
	if(MogiGLInfo::getInstance()->isGLES()) {
//#ifdef OPENGLES_FOUND
	// TODO: OpenGLES is this even needed?
//#else
	} else {
	depthTexture.setType(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F);
//#endif
	}

	//std::cout << "_FrameBuffer::initialize(): Creating " << numberOfRenderTextures << " renderTextures: " << std::endl;
	for (int i = 0; i < numberOfRenderTextures; i++) {
	//	std::cout << " -- " << i+1 << "...";
		Texture *newTexture = new Texture;
		newTexture->create(xResolution, yResolution, false);
		if(MogiGLInfo::getInstance()->isGLES()) {
//#ifdef OPENGLES_FOUND
//		// TODO: OpenGLES is this even needed?
//#else
		} else {
		newTexture->setType(GL_RGBA, GL_RGBA32F);
//#endif
		}
		renderTextures.push_back(newTexture);
	//	std::cout << "Done." << std::endl;
	}

	glGenFramebuffers(1, &frameBuffer);

	xResolution = 0;
	yResolution = 0;
	resize(xRes, yRes);
}

_FrameBuffer::~_FrameBuffer() {
	glDeleteFramebuffers(1, &frameBuffer);
}

	void printCheckFramebufferStatus(GLenum Status) {
		std::cerr << "Framebuffer is not OK for post processing, status = "
		<< Status << " == ";
		switch (Status) {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
				break;
//#ifdef OPENGLES_FOUND
#ifdef GL_ES_VERSION_2_0 // also defined for 3.0
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
				break;
#endif
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED";
				break;
		}
		std::cerr << std::endl;
	}

int _FrameBuffer::resize(int width, int height) {
	int result = 0;
	if ((width != xResolution) || (height != yResolution)) {
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &priorFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		if (depthTexture.resize(width, height)) {
			return -1;
		}

		for (int i = 0; i < renderTextures.size(); i++) {
			if (renderTextures[i]->resize(width, height)) {
				return -1;
			}
#ifdef GL_ES_VERSION_3_0
			if (MogiGLInfo::getInstance()->getVersion() >= 300) {
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, renderTextures[i]->getTexture(), 0);
			} else {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, renderTextures[i]->getTexture(), 0);
			}
#else
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, renderTextures[i]->getTexture(), 0);

#endif
		}

#ifdef GL_ES_VERSION_3_0
		if (MogiGLInfo::getInstance()->getVersion() >= 300) {

			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.getTexture(), 0);
		} else {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.getTexture(), 0);
		}
#else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.getTexture(), 0);

#endif
		//			result = resizeOther(width, height);
//#ifdef GL_ES_VERSION_3_0
		if (MogiGLInfo::getInstance()->getVersion() >= 300) {
			GLenum DrawBuffers[renderTextures.size()];
			for (int i = 0; i < renderTextures.size(); i++) {
				DrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
			}

			glDrawBuffers((int)renderTextures.size(), DrawBuffers);

		} else {
			
//			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderTextures[0]->getTexture());
		}
//	#else
		//glDrawBuffers((int)renderTextures.size(), DrawBuffers);

//#endif

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printCheckFramebufferStatus(Status);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, priorFrameBuffer);

		Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Error: glBindFramebuffer(GL_FRAMEBUFFER, " << priorFrameBuffer << "): ";
			printCheckFramebufferStatus(Status);
		}


		xResolution = width;
		yResolution = height;
	}
	return result;
}

void _FrameBuffer::attachFramebufferForReading() {
//#ifdef OPENGLES_FOUND
#ifdef GL_ES_VERSION_2_0 // also defined for 3.0
	std::cerr << "_FrameBuffer::attachFramebufferForReading() is unsupported" << std::endl;
#else
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
#endif
}

void _FrameBuffer::attachFramebuffer() {
//#ifdef OPENGLES_FOUND
#ifdef GL_ES_VERSION_2_0 // also defined for 3.0
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &priorFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);	// TODO: test this
	if (MogiGLInfo::getInstance()->getVersion() < 130) {	// GLes2?
//		std::cout << "_FrameBuffer::attachFramebuffer() for  GLES 2.0!" << std::endl;
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,renderTextures[0]->getTexture(),0);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, depthTexture.getTexture(), 0);

//		glEnable( GL_DEPTH_TEST);
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_FRONT);
	}
#else
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
#endif
}

void _FrameBuffer::removeFramebuffer() {

	checkGLError();

//#ifdef OPENGLES_FOUND
#ifdef GL_ES_VERSION_2_0 // also defined for 3.0
	glBindFramebuffer(GL_FRAMEBUFFER, priorFrameBuffer);	// TODO: test this
#else
//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, priorFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, priorFrameBuffer);
#endif

	checkGLError();

}

void _FrameBuffer::setRenderUniform(int texture, std::string name) {
	renderTextures[texture]->setUniformName(name);
}

void _FrameBuffer::setRenderIndex(int texture, int index) {
	renderTextures[texture]->setUniformIndex(index);
}

void _FrameBuffer::setDepthUniform(std::string name) {
	depthTexture.setUniformName(name);
}

void _FrameBuffer::setDepthIndex(int index) {
	depthTexture.setUniformIndex(index);
}

void _FrameBuffer::sendTexturesToShader(MBshader *shader) {
	for (int i = 0; i < renderTextures.size(); i++) {
		renderTextures[i]->sendTextureToShader(shader);
	}
	depthTexture.sendTextureToShader(shader);
}

Texture &_FrameBuffer::getRenderTexture(int index) {
	if (index >= renderTextures.size()) {
		std::cout << "Error!  render texture index is TOO LARGE!" << std::endl;
	}
	return *renderTextures[index];
}

FrameBuffer::FrameBuffer(int xRes, int yRes)
 :_FrameBuffer(xRes, yRes) {
	setup(xRes, yRes);
}

	void FrameBuffer::setup(int xRes, int yRes) {
		initialize(NUM_TEXTURES, xRes, yRes);
	}

	MBGBuffer::MBGBuffer(int xRes, int yRes)
 :_FrameBuffer(xRes, yRes) {
	setup(xRes, yRes);
}

	void MBGBuffer::setup(int xRes, int yRes) {
		initialize(NUM_G_TEXTURES, xRes, yRes);
	}

//	MBGBuffer::MBGBuffer() {
//		std::cout << " MBGVUFFERBITSCHCES!!!" << std::endl;
//
//		//initialize(MBGBuffer::NUM_TEXTURES);
//	}

#ifdef _cplusplus
}
#endif
