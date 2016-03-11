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

#ifndef MOGI_MATERIAL_H
#define MOGI_MATERIAL_H

#include "mogi/math/mmath.h"
#include "shader.h"
#include "texture.h"
#include "dynamicShader.h"

#include <vector>

namespace Mogi {
namespace Simulation {

class MBmaterial {
private:
	std::string name;
	std::string directory;
//	std::vector<Texture *> textures;

//	bool colorMapEnable;

	ShadowShaderParameters::ColorSource colorSource;

	Texture* colorMap;
	Texture* normalMap;
	Texture* specularMap;
	Texture* heightMap;

//	bool normalMapEnable;
//	bool heightMapEnable;
//	bool specularityMapEnable;

	Math::Vector colorDiffuse;
	Math::Vector colorSpecular;
	Math::Vector colorAmbient;
	Math::Vector colorEmissive;
	Math::Vector colorTransparent;

	GLfloat specularExponent;
	GLfloat specularStrength;
	GLfloat metallicLevel;

public:

	enum TextureType {
		COLOR,
		NORMAL,
		SPECULAR,
		HEIGHT
	};

	MBmaterial();
	~MBmaterial();

	void setName(const std::string& name);
	const std::string& getName() { return name; }
	void setDirectory(const std::string& path);
	const std::string& getDirectory() { return directory; }
	void setColorSource(ShadowShaderParameters::ColorSource source);
	void setColorDiffuse(float red, float green, float blue);
	void setColorAmbient(float red, float green, float blue);
	void setColorEmissive(float red, float green, float blue);
	void setColorSpecular(float red, float green, float blue);
	void setColorTransparent(float red, float green, float blue);

	const Math::Vector& getColorDiffuse();

	void setShininess(float exponent, float strength);
	void setMetallicLevel(float level);

//	void forceDisable(bool colorMap, bool normalMap, bool heightMap, bool specularityMap);

	void sendToShader(MBshader *shader);

//	void addTexture(Texture* texture);
//	std::vector<Texture*>& getTextures();
	void setTexture(Texture* texture, TextureType type);
};
}
}

#endif
