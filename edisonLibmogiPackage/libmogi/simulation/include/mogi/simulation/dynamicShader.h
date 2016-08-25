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

#ifndef MOGI_DYNAMIC_SHADER_H
#define MOGI_DYNAMIC_SHADER_H

#include <map>

#include "mogi/simulation/shader.h"




namespace Mogi {
	namespace Simulation {

		class ShaderParametersDynamic : public ShaderParameters {
		public:
			virtual ~ShaderParametersDynamic() {};
			virtual const char* getVertexTemplate() const = 0;
			virtual const char* getFragmentTemplate() const = 0;
			virtual const void getMacros(std::map<std::string, std::string>& macros) const = 0;
			virtual const bool lessThan(const ShaderParametersDynamic* right) const = 0;
			virtual ShaderParametersDynamic* copy() const = 0;
//			virtual void setUniform(const std::string& name, void* value, int index = 0) = 0;

		};

		class ShadowShaderParameters : public ShaderParametersDynamic {
		private:
			int pcfKernelSize;

			void configureParameters(StaticShader* shader);

		public:
			ShadowShaderParameters();
			~ShadowShaderParameters();

			void allocateBasedOnLightCount();

			enum ColorSource {
				COLOR_SOURCE_VERTEX_DATA,
				COLOR_SOURCE_MAP,
				COLOR_SOURCE_MATERIAL
			};

			// Dynamic parameters, modifies shader source:
			ColorSource mColorSource;
//			bool colorMapEnable;
			bool specularMapEnable;
			bool normalMapEnable;
			bool disparityMapEnable;
//			bool colorMapEnableFromMaterial;
			bool specularMapEnableFromMaterial;
			bool normalMapEnableFromMaterial;
			bool disparityMapEnableFromMaterial;
			GLint numberOfLights;
			GLint numberOfShadowMaps;
			bool useShadows;

			// Uniforms that cahnge almost every render:
			Math::Matrix modelViewProjectionMatrix;
			Math::Matrix mColorDiffuse;
			GLfloat specularProperties[2];
			Math::Matrix normalMatrix;
			Math::Matrix modelMatrix;
			Math::Matrix viewPosition;
			Math::Matrix viewMatrix;
			Math::Matrix* lightPos;
			Math::Matrix* lightColor;
			Math::Matrix* kAtt;
			Math::Matrix* lightModelViewProjectionMatrix;
			GLint colorMap;
			GLint normalMap;
			GLint heightMap;
			GLint specularityMap;


			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParametersDynamic* right) const;
			ShaderParametersDynamic* copy() const;
//			void setUniform(const std::string& name, void* value, int index = 0);

			std::string unrollLightLoop( int numberOfLights, bool shadowEnable, const std::string& specularCode ) const;
			std::string unrollShadowMapPCF( int lightIndex, bool shadowEnable ) const;

			// Calling this will reallocated the correct number of lightpos, lightColor, kAtt, and lightModelVieProjectionMAtrix arrays:
			void setNumberOfLights( GLint lightCount );
		};

		class BokehShaderParameters : public ShaderParametersDynamic {
		public:
			BokehShaderParameters();

			bool debugEnable;
			bool autoFocusEnable;
			bool vignettingEnable;
			int samples;
			int rings;

			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParametersDynamic* right) const;
			ShaderParametersDynamic* copy() const;
//			void setUniform(const std::string& name, void* value, int index = 0);

			std::string unrollSamplingLoop() const;
		};

		class FinalProcessShaderParameters : public ShaderParametersDynamic {
		public:
			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParametersDynamic* right) const;
			ShaderParametersDynamic* copy() const;
//			void setUniform(const std::string& name, void* value, int index = 0);
		};

		class ShadowMapShaderParameters : public ShaderParametersDynamic {
		public:
			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParametersDynamic* right) const;
			ShaderParametersDynamic* copy() const;
//			void setUniform(const std::string& name, void* value, int index = 0);
		};

		class GeometryShaderParameters : public ShaderParametersDynamic {
		public:
			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParametersDynamic* right) const;
			ShaderParametersDynamic* copy() const;
//			void setUniform(const std::string& name, void* value, int index = 0);
		};

		class DeferredLightingShaderParameters : public ShaderParametersDynamic {
		public:
			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParametersDynamic* right) const;
			ShaderParametersDynamic* copy() const;
//			void setUniform(const std::string& name, void* value, int index = 0);
		};

		struct ShaderParametersCompare {
			bool operator()(const ShaderParametersDynamic* left, const ShaderParametersDynamic* right) const;
		};

		// Multiton factory
		class ShaderFactory  {
		protected:
			static std::map<ShaderParametersDynamic*, StaticShader*, ShaderParametersCompare> instances;
			static int instanceCount;

			static void getGlobalMacros(std::map<std::string, std::string>& macros);
			static StaticShader* create(const ShaderParametersDynamic* parameters);

		public:
			static StaticShader* getInstance( ShaderParametersDynamic* parameters);
			static void destroyInstance( ShaderParametersDynamic* parameters);
			static bool instanceExists( ShaderParametersDynamic* parameters);
		};



		// A class where based on the parameters, a singleton shader is returned.
		class DynamicShader : public MBshader {
		protected:
//			ShaderParametersDynamic* parameters;
			void reuseProgram();
			virtual void setUniform(const std::string& name, void* value, int index = 0);

		public:
			virtual ~DynamicShader() = 0;
			DynamicShader();

			void updateUniforms();

			ShaderParametersDynamic* getParametersDynamic();

			GLuint program();

			// This is an interface class, this returns the true shader:
			MBshader* getActualShader();

			StaticShader* getActiveShader();

			GLint getAttributeLocation(std::string name);
//			GLint getUniformLocation(std::string name);
//			TextureUniform* getTextureUniform(std::string name);

		};

		class ShadowShader : public DynamicShader {
		public:
			ShadowShaderParameters* getParametersMaterial();
			ShadowShader();
			~ShadowShader();

			void setUniform(const std::string& name, void* value, int index = 0);

			// Overloaded from MBshader:
//			int sendInteger(std::string name, int value);
			//			int sendFloat(std::string name, float value);
			//			int sendMatrix(std::string name, Math::Matrix &matrix);
			//			int sendTexture(std::string name, GLuint texture);

			ShaderParameters* allocateParameters();
		};

		class BokehShader : public DynamicShader {
		public:
			BokehShader();
			~BokehShader();

			BokehShaderParameters* getParametersBokeh();

			// Overloaded from MBshader:
			int sendInteger(std::string name, int value);
			//			int sendFloat(std::string name, float value);
			//			int sendMatrix(std::string name, Math::Matrix &matrix);
			//			int sendTexture(std::string name, GLuint texture);

			ShaderParameters* allocateParameters();
		};
		
	}
}

#endif
