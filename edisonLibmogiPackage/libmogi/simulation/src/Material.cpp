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

#include "material.h"
#include <iostream>
#include "mogi/math/mmath.h"
using namespace std;

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

MBmaterial::MBmaterial() {
	colorAmbient.setLength(3);
	colorDiffuse.setLength(3);
	colorEmissive.setLength(3);
	colorSpecular.setLength(3);
	colorTransparent.setLength(3);
}

	MBmaterial::~MBmaterial() {
		for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); it++) {
			delete *it;
		}
		textures.clear();
	}

	void MBmaterial::setName(const std::string& name) {
		this->name = name;
	}

	void MBmaterial::setDirectory(const std::string& path) {
		this->directory = path;
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

	void MBmaterial::setShininess(float level, float strength) {
		shininess = level;
		specularStrength = strength;
	}
	void MBmaterial::setMetallicLevel(float level) {
		metallicLevel = level;
	}

	void MBmaterial::forceDisable(bool colorMap, bool normalMap, bool heightMap, bool specularityMap) {
		colorMapEnable = !colorMap;
		normalMapEnable = !normalMap;
		heightMapEnable = !heightMap;
		specularityMapEnable = !specularityMap;
	}

void MBmaterial::sendToShader(MBshader *shader) {
	for (int i = 0; i < textures.size(); i++) {
		textures[i]->sendTextureToShader(shader);
	}

	shader->sendInteger("colorMapEnable", colorMapEnable );
	shader->sendInteger("heightMapEnable", heightMapEnable );
	shader->sendInteger("normalMapEnable", normalMapEnable );
	shader->sendInteger("specularityMapEnable", specularityMapEnable );

	Mogi::Math::Vector specular(2);
	specular(0) = shininess;
	specular(1) = specularStrength;
	shader->sendMatrix("uSpecular", specular);
//	std::cout << "Sending uSpecularExponent = " << shininess << std::endl;
//	shader->sendFloat("uSpecularExponent", shininess );/// 1000);
//	shader->sendFloat("uSpecularLevel", specularStrength);
	shader->sendFloat("uMetallicLevel", metallicLevel);
}

	void MBmaterial::addTexture(Texture* texture) {
		// TODO: error checking, and multiple texture support of course
		textures.push_back(texture);
	}

	std::vector<Texture*>& MBmaterial::getTextures() {
		return textures;
	}

#ifdef _cplusplus
}
#endif
