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

#include "texture.h"

#ifdef SDL2_FOUND
// Need SDL for image file loading:
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#ifdef BUILD_FOR_IOS
#include "resourceInterface.h"
#endif

#include <iostream>

using namespace std;

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

Texture::Texture() {
	shaderUniformName = "";
	arrayIndex = -1;
	width = 0;
	height = 0;
}

Texture::~Texture() {
	deleteTexture();
}

void Texture::deleteTexture() {
	glDeleteTextures(1, &textureID);
}

void Texture::setUniformName(std::string name) {
	shaderUniformName = name;
}
void Texture::setUniformIndex(int index) {
	arrayIndex = index;
}

void Texture::set(aiTexture* texture) {
	std::cout << "LOADING A TEXTURE WOEHOOHO!!!!1!! Unsupported :("
			<< std::endl;
}

void Texture::sendTextureToShader(MBshader* shader) {
	if (shaderUniformName != "") {
		char uniformName[64];
		if (arrayIndex >= 0) {
			sprintf(uniformName, "%s[%d]", shaderUniformName.c_str(),
					arrayIndex);
		} else {
			sprintf(uniformName, "%s", shaderUniformName.c_str());
		}

		shader->sendTexture(uniformName, textureID);

	} else {
		// std::cout << "whoops, \"" << shaderUniformName <<"\" is undefined"  <<
		// std::endl;
	}
}

GLuint Texture::getTexture() const {
	return textureID;
}

int Texture::create(int w, int h, bool isDepth) {

	// See notes on adopting OpenGLES3 here:
	// https://developer.apple.com/library/ios/documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/AdoptingOpenGLES3/AdoptingOpenGLES3.html
	if (isDepth) {
#ifdef OPENGLES_FOUND
		format = GL_DEPTH_COMPONENT;
		type = GL_UNSIGNED_INT;
#ifdef GL_ES_VERSION_3_0
		internalFormat = GL_DEPTH_COMPONENT24;	// in ES3, explicit sizes must be given to internal format
#else
		internalFormat = GL_DEPTH_COMPONENT;
#endif
#else
		format = GL_DEPTH_COMPONENT;
		internalFormat = GL_DEPTH_COMPONENT32F;
		type = GL_FLOAT;
#endif
	} else {
#ifdef OPENGLES_FOUND
#ifdef GL_ES_VERSION_3_0
		internalFormat = GL_RGBA8;	// in ES3, explicit sizes must be given to internal format
#else
		internalFormat = GL_RGBA;
#endif
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
#else
		format = GL_RGBA;
		internalFormat = GL_RGBA8;
		type = GL_FLOAT;
#endif
	}
	if ((textureID = createTexture(w, h)) == 0) {
		return 0;
	}
	return -1;
}

void Texture::setType(GLenum textureFormat, GLint textureInternalFormat) {
	if ((textureInternalFormat != internalFormat)
			|| (textureFormat != format)) {
		format = textureFormat;
		internalFormat = textureInternalFormat;

		reconfigure();
	}
}

int Texture::resize(int w, int h) {
	if ((w != width) || (h != height)) {
		width = w;
		height = h;

		reconfigure();
	}
	return 0;
}

void Texture::reconfigure() {
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
}

int Texture::loadFromImage(std::string file) {
	if ((textureID = loadTexture(file.c_str())) != 0) {
		return 0;
	}
	std::cout << "Whoops, texture ID is: " << textureID << std::endl;
	return -1;
}

	std::string glGetErrorToString(GLenum Status) {
		switch (Status) {
			case GL_NO_ERROR:
				return "glGetError() returned GL_NO_ERROR";
				break;
			case GL_INVALID_ENUM:
				return "glGetError() returned GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				return "glGetError() returned GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				return "glGetError() returned GL_INVALID_OPERATION";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				return "glGetError() returned GL_INVALID_FRAMEBUFFER_OPERATION";
				break;
			case GL_OUT_OF_MEMORY:
				return "glGetError() returned GL_OUT_OF_MEMORY";
				break;
		}
		return "???";
	}

// cplusplusguy stuff:
GLuint Texture::createTexture(int w, int h) {
	// unsigned int textureId = 0;
	GLenum Status;

	Status = glGetError();
	if (Status != GL_NO_ERROR) {
		std::cout << "Error: Texture::createTexture(): Broke before texture created: " << glGetErrorToString(Status) << std::endl;
	}

	glGenTextures(1, &textureID);
	Status = glGetError();
	if (Status != GL_NO_ERROR) {
		std::cout << "Error: Texture::createTexture(): Broke at glGenTextures: " << glGetErrorToString(Status) << std::endl;
	}

	resize(w, h);
	glBindTexture(GL_TEXTURE_2D, textureID);
	Status = glGetError();
	if (Status != GL_NO_ERROR) {
		std::cout << "Error: Texture::createTexture(): Broke at glBindTexture() : " << glGetErrorToString(Status) << std::endl;
	}

	Status = glGetError();
	if (Status != GL_NO_ERROR) {
		std::cout << "Error: Texture::createTexture(): Broke at glTexImage2D: " << glGetErrorToString(Status) << std::endl;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
#ifdef OPENGLES_FOUND
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // from working iOS
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // prevents repeating edge...?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
#endif

	if (Status != GL_NO_ERROR) {
		// std::cout << "Error happened while creating the texture: " <<
		// gluErrorString(i) << "(gluErrorString(i) is deprecated so it was
		// removed)" << std::endl;
		// return 0;

		std::cout << "Error: Texture::createTexture(): Error happened while creating the texture: " << glGetErrorToString(Status) << std::endl
				<< "\t";
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

GLuint Simulation::loadTexture(const char* name) {
	GLuint texture;
#ifdef SDL2_FOUND	// TODO: find another type of support for loading a texture with SDL?
	SDL_Surface* img = IMG_Load(name);

	// SDL_PixelFormat form={ NULL, 32, 4, 0, 0, 0, 0, 8, 8, 8, 8, 0xff000000,
	// 0x00ff0000, 0x0000ff00, 0x000000ff, 0, 255 };
	SDL_PixelFormat form2; //={ SDL_PIXELFORMAT_RGB888, NULL, 32, 4, 0xff000000,
						   //0x00ff0000, 0x0000ff00, 0x000000ff};//, 0, 0, 0, 0,
						   //8, 8, 8, 8, 0, 255 };

	form2.format = SDL_PIXELFORMAT_RGBA8888;
	form2.palette = NULL;
	form2.BitsPerPixel = 32;
	form2.BytesPerPixel = 4;
	/*
	 form2.Rloss = 0;
	 form2.Gloss = 0;
	 form2.Bloss = 0;
	 form2.Aloss = 0;

	 form2.Rshift = 8;
	 form2.Gshift = 8;
	 form2.Bshift = 8;
	 form2.Ashift = 8;
	 */
	form2.Rmask = 0xff000000;
	form2.Gmask = 0x00ff0000;
	form2.Bmask = 0x0000ff00;
	form2.Amask = 0x000000ff;

	// SDL_SetColorKey(SDL_Surface *surface, <#int flag#>, <#Uint32 key#>)

	if (img == NULL) {
		std::cout << "Error! Could not load texture: " << name << std::endl;
		return 0;
	}
	SDL_Surface* img2 = SDL_ConvertSurface(img, &form2, SDL_SWSURFACE);


	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img2->w, img2->h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, img2->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	SDL_FreeSurface(img);
	SDL_FreeSurface(img2);

#else // SDL2_FOUND	// TODO: find another type of support for loading a texture with SDL?
//	std::cerr << "Simulation::loadTexture() Error: attempting to load an image " << name << " but SDL is not supported in this build" << std::endl;
//	return -1;
	texture = _loadTexture(name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// DEfaults testures to be tiled.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif // SDL2_FOUND	// TODO: find another type of support for loading a texture with SDL?
	return texture;
}

#ifdef _cplusplus
}
#endif
