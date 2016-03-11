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

#ifndef MOGI_SHADER_H
#define MOGI_SHADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>

#include "mogi/simulation/mogiGL.h"
#include "mogi/math/mmath.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace Mogi {
	namespace Simulation {

		extern GLint uniforms[];

		struct MBuniform {
			std::string name;
			GLint location;
		};

		struct TextureUniform {
//			std::string name;
			GLint location;
			GLint value;
		};

		bool loadShaders(GLuint *g_program, const std::string& vertShaderPath, const std::string& fragShaderPath);
		// bool loadShaders( GLuint *g_program );
		bool compileShader(GLuint *shader, GLenum type, const std::string& source);
		bool linkProgram(GLuint prog);
		bool validateProgram(GLuint prog);
		GLchar *read_text_file(const char *name);

		class MBshader {
		private:
//			bool hasAttributes;
			//	std::vector<MBuniform *> uniforms;
			std::map<std::string, GLint*> uniforms;
			std::map<std::string, GLint*> attributes;
//			std::vector<TextureUniform *> textureUniforms;
			std::map<std::string, TextureUniform *> textureUniforms;
			int textureTracker;

			GLint getUniformLocation(std::string name);
			TextureUniform *getTextureUniform(std::string name);

		protected:
			std::string label;

		public:
			MBshader();
			virtual ~MBshader() = 0;

			virtual GLuint program() = 0;

			void useProgram();
			void stopProgram();
			void enableAttributes();
			void disableAttributes();

			GLint getAttributeLocation(std::string name);
			virtual int sendInteger(std::string name, int value);
			virtual int sendFloat(std::string name, float value);
			virtual int sendMatrix(std::string name, Math::Matrix &matrix);
			virtual int sendTexture(std::string name, GLuint texture);
			
			const std::string& getName();
			void setName(const std::string& newName);
		};

		class StaticShader : public MBshader {
		private:
			GLuint g_program;

		public:
			StaticShader();
			~StaticShader() {};

			GLuint program();

			int initializeFromPath(const char *vertShaderPath, const char *fragShaderPath);
			int initializeFromSource(const std::string& vertexSource, const std::string& fragemntSource);

		};

	}
}

#endif
