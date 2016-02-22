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

#ifndef MOGI_TEXTURE_H
#define MOGI_TEXTURE_H

#include "mogi/math/mmath.h"
#include "shader.h"

#include <vector>

namespace Mogi {
namespace Simulation {

/**
 *
 * @note Class
 */
class Texture {
private:
	std::string shaderUniformName;
	int arrayIndex;
	GLuint textureID;

	// If it is not loaded from the file, used for GL stuff:
	int width;
	int height;

	GLenum type;
	GLenum format;
	GLint internalFormat;

	void deleteTexture();
	GLuint createTexture(int w, int h);
	void reconfigure();

public:
	Texture();
	~Texture();

	// Methods:
	void sendTextureToShader(MBshader *shader);
	int loadFromImage(std::string file);
	int create(int w, int h, bool isDepth);
	void setUniformName(std::string name);
	void setUniformIndex(int index);
	GLuint getTexture() const;
	int resize(int w, int h);
	void setType(GLenum textureFormat, GLint textureInternalFormat);
};

GLuint loadTexture(const char *name);
}
}

#endif
