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
#include "mogi/simulation/uniform.h"
#include "mogi/math/mmath.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace Mogi {
	namespace Simulation {

		class MBshader;
		class StaticShader;

		bool loadShaders(GLuint *g_program, const std::string& vertShaderPath, const std::string& fragShaderPath);
		// bool loadShaders( GLuint *g_program );
		bool compileShader(GLuint *shader, GLenum type, const std::string& source);
		bool linkProgram(GLuint prog);
		bool validateProgram(GLuint prog);
		GLchar *read_text_file(const char *name);

		// Handles the set of uniforms and attributes for a shader:
		class ShaderParameters {
			friend class MBshader;

			std::map<StaticShader*,std::map<Uniform*, void*> > parameterMapping;

		private:
			MBshader* parent;

//			std::map<Uniform*, void*> uniformToParameterMap;

			void addAssociation(StaticShader* shader, const std::string& name, void* pointerToPersistentValue);

		protected:

			void addAssociationInt(StaticShader* shader, const std::string& name, GLint* pointerToPersistentValue);
			void addAssociationTexture(StaticShader* shader, const std::string& name, GLint* pointerToPersistentValue);
			void addAssociationFloat(StaticShader* shader, const std::string& name, GLfloat* pointerToPersistentValue);
			void addAssociationMatrix(StaticShader* shader, const std::string& name, Math::Matrix* pointerToPersistentValue);

		public:
			virtual ~ShaderParameters() {};
//			std::map<std::string, Uniform*> uniforms;
			void setParent(MBshader* shader);

			void syncParametersToUniforms();

			virtual void configureParameters(StaticShader* shader) {};

//
//			void addIntMapping(GLint* value, const std::string);

//			void sendAllUniformsToShader();
//			void initializeUniforms();
//			virtual void setUniform(const std::string& name, void* value, int index = 0);
//			void setInt(const std::string& name, const int& value, int index = 0);
//			void setTexture(const std::string& name, const TextureUniform& value, int index = 0);
//			void setFloat(const std::string& name, const float& value, int index = 0);
//			void setMatrix(const std::string& name, const Math::Matrix& value, int index = 0);
		};


		class MBshader {
			friend class DynamicShader;

		private:
//			bool hasAttributes;
			//	std::vector<MBuniform *> uniforms;
//			std::vector<TextureUniform *> textureUniforms;
//			int textureTracker;


		protected:
			std::string label;
//			std::map<std::string, GLint*> attributes;
//			std::map<std::string, GLint*> uniforms;
//			std::map<std::string, Uniform*> uniforms;
//			std::map<std::string, TextureUniform *> textureUniforms;

			MBshader* parent;
			ShaderParameters* params;

			bool hasDynamicParent;

			// This is invoked by getParameters if the parametrs are NULL and should return parameters of the correct type.
			virtual ShaderParameters* allocateParameters() = 0;

			virtual void setUniform(const std::string& name, void* value, int index = 0) = 0;


		public:
			MBshader();
			virtual ~MBshader() = 0;

			virtual GLuint program() = 0;

			virtual void updateUniforms() = 0;

			void useProgram();
			void stopProgram();
			void enableAttributes();
			void disableAttributes();

			virtual GLint getAttributeLocation(std::string name) = 0;
//			virtual GLint getUniformLocation(std::string name);
//			virtual TextureUniform *getTextureUniform(std::string name);

			void setExternalParameters( ShaderParameters* newParameters );

//			virtual int sendInteger(std::string name, int value);
//			virtual int sendFloat(std::string name, float value);
//			virtual int sendFloats(std::string name, float* values, int count);
//			virtual int sendMatrix(std::string name, Math::Matrix &matrix);
//			virtual int sendTexture(std::string name, GLuint textureID);

			const std::string& getName();
			void setName(const std::string& newName);

			// Allocate and set up the ShaderParameters, i.e. all uniforms/attributes in the program:
			ShaderParameters* getParameters();

			// If this shader is dynamic, then the parent dynamic shader will be returned.  Otherwise, this is returned.
			MBshader* getMainShader();

			// If this shader is dynamic, then the childmost static shader will be returned.  Otherwise, this is returned.
			virtual StaticShader* getActiveShader() = 0;

			// These only set uniforms by the uniform name and type:
			// It is better to set the shader parameters.
			void setInt(const std::string& name, const int& value, int index = 0);
			void setTexture(const std::string& name, const GLint& value, int index = 0);
			void setFloat(const std::string& name, const float& value, int index = 0);
			void setMatrix(const std::string& name, const Math::Matrix& value, int index = 0);
		};

		class StaticShader : public MBshader {
		private:
			GLuint g_program;

			// Each shader has a concrete set of uniforms:
			std::map<std::string, Uniform*> uniforms;
			std::map<std::string, GLint*> attributes;

			void initializeUniforms();
			void sendAllUniformsToShader();

			void setUniform(const std::string& name, void* value, int index = 0);
//			int textureTracker;

		public:
			StaticShader();
			~StaticShader() {};

			void updateUniforms();

			GLuint program();

			ShaderParameters* allocateParameters();
			GLint getAttributeLocation(std::string name);

			Uniform* getUniform(const std::string& name);

			StaticShader* getActiveShader();


			int initializeFromPath(const char *vertShaderPath, const char *fragShaderPath);
			int initializeFromSource(const std::string& vertexSource, const std::string& fragemntSource);

		};

	}
}

#endif
