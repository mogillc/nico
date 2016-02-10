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

#include <vector>

#include <assimp/scene.h>

namespace Mogi {
namespace Simulation {

class MBmaterial {
private:
	std::string name;
	std::string directory;
	std::vector<Texture *> textures;

	bool colorMapEnable;
	bool normalMapEnable;
	bool heightMapEnable;
	bool specularityMapEnable;

	Math::Vector colorDiffuse;
	Math::Vector colorSpecular;
	Math::Vector colorAmbient;
	Math::Vector colorEmissive;
	Math::Vector colorTransparent;

	GLfloat shininess;
	GLfloat specularStrength;
	GLfloat metallicLevel;

public:
	// Mostly for debugging:
	bool colorMapUserEnable;
	bool normalMapUserEnable;
	bool heightMapUserEnable;
	bool specularityMapUserEnable;

	MBmaterial();

	void set(aiMaterial *material, std::string directoryOfObject);

	void setColorDiffuse(float red, float green, float blue);

	void importTextures(aiMaterial *material);
	int loadTextures(aiMaterial *material, aiTextureType type,
			std::string uniformVariable);

	void sendToShader(MBshader *shader);
};
}
}

#endif
