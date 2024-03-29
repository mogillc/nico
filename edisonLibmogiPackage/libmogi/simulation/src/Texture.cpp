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
#include "texture.h"

#ifdef SDL2_FOUND
// Need SDL for image file loading:
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#if defined(BUILD_FOR_IOS) || defined(ANDROID)
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
		width = -1;
		height = -1;
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

	void Texture::sendTextureToShader(MBshader* shader) {
		if (shaderUniformName != "") {
//			char uniformName[64];
////			if (arrayIndex >= 0) {
////				sprintf(uniformName, "%s[%d]", shaderUniformName.c_str(), arrayIndex);
////			} else {
////				sprintf(uniformName, "%s", shaderUniformName.c_str());
////			}
//			sprintf(uniformName, "%s", shaderUniformName.c_str());
//
////			shader->sendTexture(uniformName, textureID);
//			shader->setTexture(uniformName, (GLint)textureID, arrayIndex >= 0 ? arrayIndex : 0);
			shader->setTexture(shaderUniformName, (GLint)textureID, arrayIndex >= 0 ? arrayIndex : 0);

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
			if(MogiGLInfo::getInstance()->isGLES()) {
				format = GL_DEPTH_COMPONENT;
				type = GL_UNSIGNED_INT;
				if (MogiGLInfo::getInstance()->getVersion() >= 300) {
					internalFormat = GL_DEPTH_COMPONENT24;	// in ES3, explicit sizes must be given to internal format
				} else {
					internalFormat = GL_DEPTH_COMPONENT;	//iOS, Raspberry pi!
				}

			} else { // Not GLES:
				format = GL_DEPTH_COMPONENT;
				internalFormat = GL_DEPTH_COMPONENT32F;
				type = GL_FLOAT;
			}
		} else {
			if(MogiGLInfo::getInstance()->isGLES()) {
				if (MogiGLInfo::getInstance()->getVersion() >= 300) {	// check for ES
					internalFormat = GL_RGBA8;	// in ES3, explicit sizes must be given to internal format
				} else {
					internalFormat = GL_RGBA; // raspberry pi, iOS
				}
				format = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
				//#else
			} else { // Not GLES:
				format = GL_RGBA;
				internalFormat = GL_RGBA8;
				type = GL_FLOAT;
				//#endif
			}
		}
		if ((textureID = createTexture(w, h)) == 0) {
			return 0;
		}
		return -1;
	}

	void Texture::reconfigure(const GLvoid* data) {
		checkGLError();

		glBindTexture(GL_TEXTURE_2D, textureID);
		checkGLError();

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
		checkGLError();
	}


	void Texture::setFromImage(const Image8& image) {
		bool needToCreateTexture = false;
		if (width == -1 || height == -1) {
			//			textureID = createTexture(image.width(), image.height());
			needToCreateTexture = true;
		} else {
			width = image.width();
			height = image.height();
		}
		if (MogiGLInfo::getInstance()->getVersion() >= 300) {
			internalFormat = GL_RGBA8;
		} else {
			internalFormat = GL_RGBA;
		}
		format = GL_RGBA;

		//
		if(MogiGLInfo::getInstance()->isGLES()) {
			type = GL_UNSIGNED_BYTE;
			//#else
		} else {
//#ifdef OPENGLES_FOUND
//#ifdef GL_ES_VERSION_2_0 // also defined for 3.0
//			#warning Untested Texture GL_UNSIGNED_BYTE
//			type = GL_UNSIGNED_BYTE;
//#else

#ifdef GL_UNSIGNED_INT_8_8_8_8_REV
			type = GL_UNSIGNED_INT_8_8_8_8_REV;
#else
			#warning Untested use of GL_UNSIGNED_BYTE for replacement of GL_UNSIGNED_INT_8_8_8_8_REV
			type = GL_UNSIGNED_BYTE;
#endif
		}
		if (needToCreateTexture) {
			textureID = createTexture(image.width(), image.height());
		}
		reconfigure(image.data());
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	}

	GLenum Texture::getType() {
		return type;
	}
	GLenum Texture::getFormat() {
		return format;
	}
	GLint Texture::getInternalFormat() {
		return internalFormat;
	}

	void Texture::setType(GLenum textureFormat, GLint textureInternalFormat) {
		if ((textureInternalFormat != internalFormat)
			|| (textureFormat != format)) {
			format = textureFormat;
			internalFormat = textureInternalFormat;

			reconfigure(NULL);
		}
	}

	int Texture::resize(int w, int h) {
		if ((w != width) || (h != height)) {
			width = w;
			height = h;

			reconfigure(NULL);
		}
		return 0;
	}

	int Texture::loadFromImage(std::string file) {
		if ((textureID = loadTexture(file.c_str())) != 0) {
			return 0;
		}
		std::cout << "Whoops, texture ID is: " << textureID << std::endl;
		return -1;
	}

	// cplusplusguy stuff:
	GLuint Texture::createTexture(int w, int h) {
		// unsigned int textureId = 0;
		checkGLError();

		glGenTextures(1, &textureID);
		checkGLError();

		//	glBindTexture(GL_TEXTURE_2D, textureID);
		//		checkGLError();
		// This will call glBindTexture and glTexImage2D
		resize(w, h);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		checkGLError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		if(MogiGLInfo::getInstance()->isGLES()) {
			if (MogiGLInfo::getInstance()->getVersion() >= 300) {	// NOT SURE IF WE SHOULD BLANKET ALL OF THIS
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // from working iOS
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			} else {
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // prevents repeating edge...?
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

		} else {
//#ifdef OPENGLES_FOUND
//#ifdef GL_ES_VERSION_2_0 // also defined for 3.0
#ifndef GL_CLAMP_TO_BORDER
			std::cerr << "Unhandled texture thingy!" << std::endl;
#else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // prevents repeating edge...?
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
#endif
		}

		checkGLError();

		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}


	GLuint Simulation::loadTexture(const char* name) {
		GLuint texture;
#ifdef SDL2_FOUND	// TODO: find another type of support for loading a texture with SDL?
		SDL_Surface* img = IMG_Load(name);

		if (img == NULL) {
			std::cout << "Error! Could not load texture: " << name << std::endl;
			return 0;
		}

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		if(MogiGLInfo::getInstance()->isGLES()) {
			GLint mode;
			if (img->format->BytesPerPixel == 3) { // RGB 24bit
				mode = GL_RGB;
			} else if (img->format->BytesPerPixel == 4) { // RGBA 32bit
				mode = GL_RGBA;
			} else {
				std::cerr << "Error: unkown uiamge format, img->format->BytesPerPixel == " << img->format->BytesPerPixel << std::endl;
				mode = GL_RGBA;
				SDL_FreeSurface(img);
				return -1;
			}
			glTexImage2D(GL_TEXTURE_2D, 0, mode, img->w, img->h, 0, mode, GL_UNSIGNED_BYTE, img->pixels);

		} else {
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

			SDL_Surface* img2 = SDL_ConvertSurface(img, &form2, SDL_SWSURFACE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img2->w, img2->h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, img2->pixels);


			SDL_FreeSurface(img2);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		SDL_FreeSurface(img);

#else // SDL2_FOUND

		// Important!  By default we will wrap the textures.  On OpenGL ES 2, this ONLY works if the
		// Textures are a power of 2!  The _loadTexture function will then resize a texture to the next
		// Highest power of 2 on an ES 2 context.  For example, a 700x700 texture will be resized to 1024x1024
		texture = _loadTexture(name, MogiGLInfo::getInstance()->getVersion());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// wrap the textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, 0);
#endif // SDL2_FOUND	// TODO: find another type of support for loading a texture with SDL?

		std::cout << " - Loaded Texture ID:" << texture << "\tFile: " << name << std::endl;
		return texture;
	}
	
#ifdef _cplusplus
}
#endif
