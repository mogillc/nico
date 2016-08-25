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
#include "material.h"
#include <iostream>
#include "mogi/math/mmath.h"

#include "dynamicShader.h"
using namespace std;

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

MBmaterial::MBmaterial() {
	colorSource = ShadowShaderParameters::COLOR_SOURCE_MATERIAL;
	colorAmbient.setLength(3);
	colorDiffuse.setLength(3);
	colorEmissive.setLength(3);
	colorSpecular.setLength(3);
	colorTransparent.setLength(3);

	colorMap = NULL;
	normalMap = NULL;
	specularMap = NULL;
	heightMap = NULL;

//	colorMapEnable = false;
//	normalMapEnable = false;
//	specularityMapEnable = false;
//	heightMapEnable = false;

	specularExponent = 32.0;
	specularStrength = 1.0;
	metallicLevel = 0.5;
}

	MBmaterial::~MBmaterial() {
//		for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); it++) {
//			delete *it;
//		}
//		textures.clear();
	}

	void MBmaterial::setName(const std::string& name) {
		this->name = name;
	}

	void MBmaterial::setDirectory(const std::string& path) {
		this->directory = path;
	}

	void MBmaterial::setColorSource(ShadowShaderParameters::ColorSource source) {
		colorSource = source;
	}

	void MBmaterial::setColorDiffuse(float red, float green, float blue) {
		colorDiffuse(0) = red;
		colorDiffuse(1) = green;
		colorDiffuse(2) = blue;
	}
	void MBmaterial::setColorAmbient(float red, float green, float blue) {
		colorAmbient(0) = red;
		colorAmbient(1) = green;
		colorAmbient(2) = blue;
	}
	void MBmaterial::setColorEmissive(float red, float green, float blue) {
		colorEmissive(0) = red;
		colorEmissive(1) = green;
		colorEmissive(2) = blue;
	}
	void MBmaterial::setColorSpecular(float red, float green, float blue) {
		colorSpecular(0) = red;
		colorSpecular(1) = green;
		colorSpecular(2) = blue;
	}
	void MBmaterial::setColorTransparent(float red, float green, float blue) {
		colorTransparent(0) = red;
		colorTransparent(1) = green;
		colorTransparent(2) = blue;
	}

	void MBmaterial::setShininess(float exponent, float strength) {
		specularExponent = exponent;
		specularStrength = strength;
	}
	void MBmaterial::setMetallicLevel(float level) {
		metallicLevel = level;
	}

	const Math::Vector& MBmaterial::getColorDiffuse() {
		return colorDiffuse;
	}

//	void MBmaterial::forceDisable(bool colorMap, bool normalMap, bool heightMap, bool specularityMap) {
//		colorMapEnable = !colorMap;
//		if (colorMapEnable) {
//			colorSource = ShadowShaderParameters::COLOR_SOURCE_MAP;
//		}
//		normalMapEnable = !normalMap;
//		heightMapEnable = !heightMap;
//		specularityMapEnable = !specularityMap;
//	}

void MBmaterial::sendToShader(MBshader *shader) {
//	for (int i = 0; i < textures.size(); i++) {
//		textures[i]->sendTextureToShader(shader);
//	}
	checkGLError();

	if (normalMap) {
		normalMap->sendTextureToShader(shader);
//		shader->sendInteger("normalMapEnable", true );
		shader->setInt("normalMapEnable", true);
	} else {
//		shader->sendInteger("normalMapEnable", false );
		shader->setInt("normalMapEnable", false);
	}
	if (specularMap) {
		specularMap->sendTextureToShader(shader);
//		shader->sendInteger("specularityMapEnable", true );
		shader->setInt("specularityMapEnable", true);
	} else {
//		shader->sendInteger("specularityMapEnable", false );
		shader->setInt("specularityMapEnable", false);
	}
	if (heightMap) {
		heightMap->sendTextureToShader(shader);
//		shader->sendInteger("heightMapEnable", true );
		shader->setInt("heightMapEnable", true);
	} else {
//		shader->sendInteger("heightMapEnable", false );
		shader->setInt("heightMapEnable", false);
	}

	if (colorMap) {
		colorMap->sendTextureToShader(shader);
		colorSource = ShadowShaderParameters::COLOR_SOURCE_MAP;
	}
	checkGLError();

//	shader->sendInteger("colorSource", colorSource);
	shader->setInt("colorSource", colorSource);

	checkGLError();

	if(colorSource == ShadowShaderParameters::COLOR_SOURCE_MATERIAL) {
//		shader->sendInteger("colorSource",  ShadowShaderParameters::COLOR_SOURCE_MATERIAL);
//		shader->sendMatrix("mColorDiffuse", colorDiffuse);
		shader->setMatrix("mColorDiffuse", colorDiffuse);
	} else if (colorSource == ShadowShaderParameters::COLOR_SOURCE_VERTEX_DATA) {
//		shader->sendInteger("colorSource",  ShadowShaderParameters::COLOR_SOURCE_VERTEX_DATA);
	}
//	shader->sendInteger("heightMapEnable", heightMapEnable );
//	shader->sendInteger("normalMapEnable", normalMapEnable );
//	shader->sendInteger("specularityMapEnable", specularityMapEnable );

	checkGLError();

	Mogi::Math::Vector specular(2);
	specular(0) = specularExponent;
	specular(1) = specularStrength;
//	shader->sendMatrix("uSpecular", specular);
	//	shader->sendFloats("uSpecular", specular.dataAsFloat(), specular.size());//("uSpecular", specular);
//	shader->sendFloat("specularProperties[0]", specular(0));
//	shader->sendFloat("specularProperties[1]", specular(1));
	shader->setFloat("specularProperties", specular(0), 0);
	shader->setFloat("specularProperties", specular(1), 1);

	checkGLError();
//	std::cout << "Sending uSpecularExponent = " << specularExponent << std::endl;
//	shader->sendFloat("uSpecularExponent", specularExponent );/// 1000);
//	shader->sendFloat("uSpecularLevel", specularStrength);
//	shader->sendFloat("uMetallicLevel", metallicLevel);
	shader->setFloat("uMetallicLevel", metallicLevel);

	checkGLError();
}

	void MBmaterial::setTexture(Mogi::Simulation::Texture *texture, Mogi::Simulation::MBmaterial::TextureType type) {
		switch (type) {
			case COLOR:
				colorMap = texture;
				colorMap->setUniformName("colorMap");
				colorMap->setUniformIndex(0);
				break;

			case NORMAL:
				normalMap = texture;
				normalMap->setUniformName("normalMap");
				normalMap->setUniformIndex(0);
				break;

			case HEIGHT:
				heightMap = texture;
				heightMap->setUniformName("heightMap");
				heightMap->setUniformIndex(0);
				break;

			case SPECULAR:
				specularMap = texture;
				specularMap->setUniformName("specularityMap");
				specularMap->setUniformIndex(0);
				break;
		}
	}

//	void MBmaterial::addTexture(Texture* texture) {
//		// TODO: error checking, and multiple texture support of course
//		textures.push_back(texture);
//	}
//
//	std::vector<Texture*>& MBmaterial::getTextures() {
//		return textures;
//	}

#ifdef _cplusplus
}
#endif
