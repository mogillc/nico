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

	void MBmaterial::setName(const std::string& name) {
		this->name = name;
	}

	void MBmaterial::setDirectory(const std::string& path) {
		this->directory = path;
	}

//void MBmaterial::set(aiMaterial *material, std::string directoryOfObject) {
//	aiString tempName;
//	aiGetMaterialString(material, AI_MATKEY_NAME, &tempName);
//	name = tempName.C_Str();
//	// std::cout << "\t\tAdding material: " << name <<std::endl;
//
//	directory = directoryOfObject;
//
//	aiColor3D color(0.f, 0.f, 0.f);
//	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
//	colorDiffuse(0) = color.r;
//	colorDiffuse(1) = color.g;
//	colorDiffuse(2) = color.b;
//
//	material->Get(AI_MATKEY_COLOR_AMBIENT, color);
//	colorAmbient(0) = color.r;
//	colorAmbient(1) = color.g;
//	colorAmbient(2) = color.b;
//
//	material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
//	colorEmissive(0) = color.r;
//	colorEmissive(1) = color.g;
//	colorEmissive(2) = color.b;
//
//	material->Get(AI_MATKEY_COLOR_SPECULAR, color);
//	colorSpecular(0) = color.r;
//	colorSpecular(1) = color.g;
//	colorSpecular(2) = color.b;
//
//	material->Get(AI_MATKEY_COLOR_TRANSPARENT, color);
//	colorTransparent(0) = color.r;
//	colorTransparent(1) = color.g;
//	colorTransparent(2) = color.b;
//
//	if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess)) {
//		shininess = 32;
//	}
//	if (AI_SUCCESS
//			!= material->Get(AI_MATKEY_SHININESS_STRENGTH, specularStrength)) {
//		specularStrength = 1;
//	}
//
//	metallicLevel = 1.0;
//
//	importTextures(material);
//}
//
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
		colorMapUserEnable = colorMapEnable;
		normalMapUserEnable = normalMapEnable;
		heightMapUserEnable = heightMapEnable;
		specularityMapUserEnable = specularityMapEnable;
	}
//
//void MBmaterial::importTextures(aiMaterial *material) {
//	colorMapUserEnable = colorMapEnable = (0 < loadTextures(material, aiTextureType_DIFFUSE, "colorMap"));
//	normalMapUserEnable = normalMapEnable = ((0 < loadTextures(material, aiTextureType_NORMALS, "normalMap")) || (0 < loadTextures(material, aiTextureType_HEIGHT, "normalMap")));
//	heightMapUserEnable = heightMapEnable = (0 < loadTextures(material, aiTextureType_DISPLACEMENT, "heightMap"));
//	specularityMapUserEnable = specularityMapEnable = (0 < loadTextures(material, aiTextureType_SPECULAR, "specularityMap"));
//
//	/*
//	 std::cout << " - - Diffuse textures   : " << material->GetTextureCount(
//	 aiTextureType_DIFFUSE ) << std::endl;
//	 std::cout << " - - Normal textures    : " << material->GetTextureCount(
//	 aiTextureType_HEIGHT ) << std::endl;
//	 std::cout << " - - Displace textures  : " << material->GetTextureCount(
//	 aiTextureType_DISPLACEMENT ) << std::endl;
//	 std::cout << " - - Specular textures  : " << material->GetTextureCount(
//	 aiTextureType_SPECULAR) << std::endl;
//	 std::cout << " - - Shininess textures : " << material->GetTextureCount(
//	 aiTextureType_SHININESS ) << std::endl;
//	 std::cout << " - - Reflection textures: " << material->GetTextureCount(
//	 aiTextureType_REFLECTION ) << std::endl;
//	 std::cout << " - - Opacity textures   : " << material->GetTextureCount(
//	 aiTextureType_OPACITY ) << std::endl;
//	 std::cout << " - - Light Map textures : " << material->GetTextureCount(
//	 aiTextureType_LIGHTMAP ) << std::endl;
//	 std::cout << " - - Emissive textures  : " << material->GetTextureCount(
//	 aiTextureType_EMISSIVE ) << std::endl;
//	 std::cout << " - - Ambient textures   : " << material->GetTextureCount(
//	 aiTextureType_AMBIENT ) << std::endl;
//	 std::cout << " - - Normals textures   : " << material->GetTextureCount(
//	 aiTextureType_NORMALS ) << std::endl;
//	 std::cout << " - - NONE textures      : " << material->GetTextureCount(
//	 aiTextureType_NONE ) << std::endl;
//	 std::cout << " - - Unknown textures   : " << material->GetTextureCount(
//	 aiTextureType_UNKNOWN) << std::endl;
//	 */
//}
//
////int MBmaterial::loadTextures(aiMaterial *material, aiTextureType type,
////		std::string uniformVariable) {
////	Texture *texture;
////	int numberOfLoadedTextures = 0;
////	for (int i = 0; i < material->GetTextureCount(type); i++) {
////		numberOfLoadedTextures++;
////		texture = new Texture;
////		aiString str;
////		material->GetTexture(type, i, &str);
////
////		std::string textureLocation = directory + "/";
////		textureLocation.append(str.C_Str());
////
////		if (texture->loadFromImage(textureLocation) < 0) {
////			numberOfLoadedTextures--;
////			std::cout << "Error! Could not load texture: " << textureLocation
////					<< " for " << name << std::endl;
////		} else {
////			texture->setUniformName(uniformVariable);
////			texture->setUniformIndex(i);
////			textures.push_back(texture);
////		}
////	}
////
////	return numberOfLoadedTextures;
////}

void MBmaterial::sendToShader(MBshader *shader) {
	for (int i = 0; i < textures.size(); i++) {
		textures[i]->sendTextureToShader(shader);
	}

	shader->sendInteger("colorMapEnable", colorMapEnable && colorMapUserEnable);
	shader->sendInteger("heightMapEnable",
			heightMapEnable && heightMapUserEnable);
	shader->sendInteger("normalMapEnable",
			normalMapEnable && normalMapUserEnable);
	shader->sendInteger("specularityMapEnable",
			specularityMapEnable && specularityMapUserEnable);

	shader->sendFloat("uSpecularExponent", shininess / 1000);
	shader->sendFloat("uSpecularLevel", specularStrength);
	shader->sendFloat("uMetallicLevel", metallicLevel);
}

	void MBmaterial::addTexture(Texture* texture) {
		// TODO: error checking, and multiple texture support of course
		textures.push_back(texture);
	}

#ifdef _cplusplus
}
#endif
