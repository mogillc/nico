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

#ifndef MOGI_FRAMEBUFFER_H
#define MOGI_FRAMEBUFFER_H

#include "texture.h"

namespace Mogi {
namespace Simulation {

class _FrameBuffer {
private:
protected:
	GLuint frameBuffer;
	GLint priorFrameBuffer;	// Mostly for ES 2 support

	std::vector<Texture*> renderTextures;
	Texture depthTexture;

	int xResolution;
	int yResolution;

	virtual void setup(int xRes, int yRes) = 0;
	void initialize(int numberOfRenderTextures, int xRes, int yRes);

public:
	_FrameBuffer( int xRes, int yRes );
	virtual ~_FrameBuffer();

	//! Attaches the frame buffer for rendering.  If it is not desired to render
	//! to the frame buffer rather than the display viewport, call this function
	//! before drawing.  Call removeFramebuffer() when finished rendering to the
	//! frame buffer.
	/*!
	 \sa removeFramebuffer()
	 */
	void attachFramebuffer();

	void attachFramebufferForReading();

	//! Removes the frame buffer from rendering.  This should be called once
	//! drawing is complete when attachFramebuffer() was used.
	/*!
	 \sa attachFramebuffer()
	 */
	void removeFramebuffer();

	//! Sets the uniform name for when sending the texture to the shader.
	/*!
	 \param name is the uniform variable name of the render texture defined in the
	 shader.
	 \sa setDepthIndex(), sendTexturesToShader()
	 */
	void setRenderUniform(int texture, std::string name);

	//! Sets the uniform name for when sending the texture to the shader.
	/*!
	 \param name is the uniform variable name of the depth texture defined in the
	 shader.
	 \sa setRenderIndex(), sendTexturesToShader()
	 */
	void setDepthUniform(std::string name);

	//! Sends the uniform index for the shader.  If the uniform name is
	//! setRenderUniform("texture"), then the resulting name after calling
	//! setRenderIndex(2) will be "texture[2]".  If the index value is negative
	//! (default is -1), then an array format is not implemented.
	/*!
	 \param index is the index of the uniform to be sent to the shader.
	 \sa setRenderUniform()
	 */
	void setRenderIndex(int texture, int index);

	//! Sends the uniform index for the shader.  If the uniform name is
	//! setDepthUniform("texture"), then the resulting name after calling
	//! setDepthIndex(2) will be "texture[2]".  If the index value is negative
	//! (default is -1), then an array format is not implemented.
	/*!
	 \param index is the index of the uniform to be sent to the shader.
	 \sa setDepthUniform()
	 */
	void setDepthIndex(int index);

	//! Sends the render and depth textures to a shader.
	/*!
	 \param shader is the shader that is sent the textures.
	 \sa setRenderUniform(), setDepthUniform()
	 */
	void sendTexturesToShader(MBshader* shader);

	//! Returns a reference to the frame buffer's render texture.
	/*!
	 \return The render texture object.
	 \sa getDepthTexture()
	 */
	Texture& getRenderTexture(int index);

	//! Returns a reference to the frame buffer's depth texture.
	/*!
	 \return The depth texture object.
	 \sa getRenderTexture()
	 */
	Texture& getDepthTexture() {
		return depthTexture;
	}
	;

	//! Resizes the frame buffer's textures to the given resolution.  Textures are
	//! created with a default size of 1280x720, so use this to resize.
	/*!
	 \param width is the x resolution.
	 \param height is the y resolution.
	 \return 0 if success, otherwise fail.
	 */
	int resize(int width, int height);
};

/**
 *  A frame buffer handler class.  This handles a frame buffer for rendering to
 * textures in OpenGL.  This is typically used for post processing where it is
 * not desired to render to the display viewport directly, but rather to
 * textures for later use.
 */
class FrameBuffer: public _FrameBuffer {
private:
	//	GLuint frameBuffer;

protected:
	void setup(int xRes, int yRes);

public:
	enum FBUFFER_TEXTURE_TYPE {
		TEXTURE_TYPE_DIFFUSE, NUM_TEXTURES
	};

	FrameBuffer(int xRes, int yRes);
	//~FrameBuffer();
};

/**
 *  A shadow map frame buffer handler class.  This handles a frame buffer for
 * use with light sources to generate shadowmaps.
 */
class MBshadowMapFrameBuffer: private _FrameBuffer {
};

/**
 *  A G buffer handler class.  This handles a frame buffer for use with deffered
 * rendering.
 */
class MBGBuffer: public _FrameBuffer {
private:
public:
	enum FBUFFER_TEXTURE_TYPE {
		TEXTURE_TYPE_DIFFUSE,
		TEXTURE_TYPE_NORMAL,
		TEXTURE_TYPE_SPECULARITY,
		NUM_G_TEXTURES
	};

protected:
	void setup(int xRes, int yRes);

public:
	MBGBuffer(int xRes, int yRes);
	//	: FrameBuffer(NUM_TEXTURES) {}
};
}
}

#endif
