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

		struct Pixel {
			GLubyte r;
			GLubyte g;
			GLubyte b;
			GLubyte a;

			Pixel(): r(127), g(127), b(127), a(255) {};
		};

		class Image8 {
		private:
			Pixel* mData;
			int mWidth;
			int mHeight;

		public:
			Image8(int width, int height);
			~Image8();

			Pixel& operator() (const int& x, const int& y);

			const void* data() const;

			const int& width() const;
			const int& height() const;

			void DrawPixel(unsigned int x, unsigned int y, const Pixel& color);
			void DrawLine(float x0, float y0, float x1, float y1, const Pixel& color);
			void DrawThickLine(int x0, int y0, int x1, int y1, float wd, const Pixel& color);
			void DrawCircle(int x0, int y0, int radius, const Pixel& color);
			void DrawCircle(int x0, int y0, int radius, int thickness, const Pixel& color);
		};

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

			// If there is no data, set this to NULL and the size/type will be allocated with default data.
			void reconfigure(const GLvoid* data);

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

			void setFromImage(const Image8& image);

			GLenum getType();
			GLenum getFormat();
			GLint getInternalFormat();
		};
		
		GLuint loadTexture(const char *name);
	}
}

#endif
