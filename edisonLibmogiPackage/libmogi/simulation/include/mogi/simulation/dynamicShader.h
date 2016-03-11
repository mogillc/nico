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

#include <mogi/simulation/shader.h>




namespace Mogi {
	namespace Simulation {

		class ShaderParameters {
		public:
			virtual ~ShaderParameters() {};
			virtual const char* getVertexTemplate() const = 0;
			virtual const char* getFragmentTemplate() const = 0;
			virtual const void getMacros(std::map<std::string, std::string>& macros) const = 0;
			virtual const bool lessThan(const ShaderParameters* right) const = 0;
			virtual ShaderParameters* copy() const = 0;
			
		};

		class ShadowShaderParameters : public ShaderParameters {
		private:
			int pcfKernelSize;
		public:
			ShadowShaderParameters();

			enum ColorSource {
				COLOR_SOURCE_VERTEX_DATA,
				COLOR_SOURCE_MAP,
				COLOR_SOURCE_MATERIAL
			};

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

			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParameters* right) const;
			ShaderParameters* copy() const;

			std::string unrollLightLoop( int numberOfLights, bool shadowEnable, const std::string& specularCode ) const;
			std::string unrollShadowMapPCF( int lightIndex, bool shadowEnable ) const;
		};

		class BokehShaderParameters : public ShaderParameters {
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
			const bool lessThan(const ShaderParameters* right) const;
			ShaderParameters* copy() const;

			std::string unrollSamplingLoop() const;
		};

		class FinalProcessShaderParameters : public ShaderParameters {
		public:
			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParameters* right) const;
			ShaderParameters* copy() const;
		};

		class ShadowMapShaderParameters : public ShaderParameters {
		public:
			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParameters* right) const;
			ShaderParameters* copy() const;
		};

		class GeometryShaderParameters : public ShaderParameters {
		public:
			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParameters* right) const;
			ShaderParameters* copy() const;
		};

		class DeferredLightingShaderParameters : public ShaderParameters {
		public:
			const char* getVertexTemplate() const;
			const char* getFragmentTemplate() const;
			const void getMacros(std::map<std::string, std::string>& macros) const;
			const bool lessThan(const ShaderParameters* right) const;
			ShaderParameters* copy() const;
		};

		struct ShaderParametersCompare {
			bool operator()(const ShaderParameters* left, const ShaderParameters* right) const;
		};

		// Multiton factory
		class ShaderFactory  {
		protected:
			static std::map< ShaderParameters*, MBshader*, ShaderParametersCompare> instances;
			static int instanceCount;

			static void getGlobalMacros(std::map<std::string, std::string>& macros);
			static MBshader* create(const ShaderParameters* parameters);

		public:
			static MBshader* getInstance( ShaderParameters* parameters);
			static void destroyInstance( ShaderParameters* parameters);
		};



		// A class where based on the parameters, a singleton shader is returned.
		class DynamicShader : public MBshader {
		protected:
			ShaderParameters* parameters;

		public:
			virtual ~DynamicShader() = 0;
			DynamicShader();

			GLuint program();

			// This is an interface class, this returns the true shader:
			MBshader* getActualShader();

		};

		class ShadowShader : public DynamicShader {
		public:
			ShadowShaderParameters* getParameters();
			ShadowShader();
			~ShadowShader();

			// Overloaded from MBshader:
			int sendInteger(std::string name, int value);
			//			int sendFloat(std::string name, float value);
			//			int sendMatrix(std::string name, Math::Matrix &matrix);
			//			int sendTexture(std::string name, GLuint texture);
		};

		class BokehShader : public DynamicShader {
		public:
			BokehShader();
			~BokehShader();

			BokehShaderParameters* getParameters();

			// Overloaded from MBshader:
			int sendInteger(std::string name, int value);
			//			int sendFloat(std::string name, float value);
			//			int sendMatrix(std::string name, Math::Matrix &matrix);
			//			int sendTexture(std::string name, GLuint texture);
		};
		
	}
}

#endif
