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

#ifndef MOGI_BONE_H
#define MOGI_BONE_H

#include <assimp/scene.h>
#include <vector>

#include "mogi/math/mmath.h"
#include "mogi/math/node.h"

namespace Mogi {
namespace Simulation {

struct vertexWeight {
	unsigned int vertexID;
	float weight;
};

class Bone {
private:
	Math::Node *parentNode;
	std::string name;
	// int numberOfWeights;
	Math::Matrix offsetMatrix;
	// vertexWeight *weights;
	std::vector<vertexWeight *> weights;
	void init();

public:
	Bone();
	~Bone();
	Bone(const Bone &param);
	Bone &operator=(const Bone &);

	void set(aiBone *bone);
	void set(const Bone *bone);
	const std::string &getName();
	void findNode(Math::Node *rootNode);
};

std::vector<Bone *> getBones(aiMesh *mesh);
}
}

#endif
