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

#include "framebuffer.h"
#include <iostream>

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

_FrameBuffer::_FrameBuffer() {
	//	setup();
}

void _FrameBuffer::initialize(int numberOfRenderTextures) {
	glGenFramebuffers(1, &frameBuffer);

	glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer);

	xResolution = 1280;
	yResolution = 720;

	// std::cout << "Creating depthTexture: " << std::endl;
	depthTexture.create(xResolution, yResolution, true);
#ifdef OPENGLES_FOUND
	// TODO: OpenGLES is this even needed?
#else
	depthTexture.setType(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F);
#endif

	//std::cout << "_FrameBuffer::initialize(): Creating " << numberOfRenderTextures << " renderTextures: " << std::endl;
	for (int i = 0; i < numberOfRenderTextures; i++) {
	//	std::cout << " -- " << i+1 << "...";
		Texture *newTexture = new Texture;
		newTexture->create(xResolution, yResolution, false);
#ifdef OPENGLES_FOUND
		// TODO: OpenGLES is this even needed?
#else
		newTexture->setType(GL_RGBA, GL_RGBA32F);
#endif
		renderTextures.push_back(newTexture);
	//	std::cout << "Done." << std::endl;
	}

	xResolution = 0;
	yResolution = 0;
	resize(1280, 720);
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
#ifdef OPENGLES_FOUND
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
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		if (depthTexture.resize(width, height)) {
			return -1;
		}

		for (int i = 0; i < renderTextures.size(); i++) {
			if (renderTextures[i]->resize(width, height)) {
				return -1;
			}
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
					GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
					renderTextures[i]->getTexture(), 0);
		}

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D, depthTexture.getTexture(), 0);

		//			result = resizeOther(width, height);

		GLenum DrawBuffers[renderTextures.size()];
		for (int i = 0; i < renderTextures.size(); i++) {
			DrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glDrawBuffers(renderTextures.size(), DrawBuffers);

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printCheckFramebufferStatus(Status);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		xResolution = width;
		yResolution = height;
	}
	return result;
}

void _FrameBuffer::attachFramebufferForReading() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
}

void _FrameBuffer::attachFramebuffer() {
#ifdef OPENGLES_FOUND
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);	// TODO: test this
#else
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
#endif
}

void _FrameBuffer::removeFramebuffer() {
#ifdef OPENGLES_FOUND
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// TODO: test this
#else
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif
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

FrameBuffer::FrameBuffer() {
	setup();
}

MBGBuffer::MBGBuffer() {
	setup();
}

//	MBGBuffer::MBGBuffer() {
//		std::cout << " MBGVUFFERBITSCHCES!!!" << std::endl;
//
//		//initialize(MBGBuffer::NUM_TEXTURES);
//	}

#ifdef _cplusplus
}
#endif
