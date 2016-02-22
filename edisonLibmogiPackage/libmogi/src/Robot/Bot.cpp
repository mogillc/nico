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

#include <bot.h>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Robot;
	using namespace Math;

	Bot::Bot() :
	nodeLocation(NULL) {
	}

	Bot::~Bot() {
		for (std::vector<Joint*>::iterator it = joints.begin(); it != joints.end(); it++) {
			delete *it;
		}
	}

	void Bot::update(double dTime) {
		if (nodeLocation)
			nodeLocation->update();
	}

	void Bot::setupNode(Node* node) {
		nodeLocation = node;
		if (nodeLocation == NULL) {
			std::cout << "Warning! root node is NULL" << std::endl;
			nodeLocation = new Node;
		}
	}

	int Bot::setKinematics(const Math::Vector& angles) {
		if (angles.size() != joints.size()) {
			return -1;
		}
		int i = 0;
		for (std::vector<Joint*>::iterator it = joints.begin(); it != joints.end(); it++, i++) {
			(*it)->setValue(angles.valueAsConst(i, 0));
		}
		return 0;
	}

	Math::Vector Bot::getKinematics() {
		Vector result(joints.size());
		int i = 0;
		for (std::vector<Joint*>::iterator it = joints.begin(); it != joints.end(); it++, i++) {
			result.value(i, 0) = (*it)->getValue();
		}
		return result;
	}

	void Bot::buildNodeStructure(Node* root) {
		setupNode(root);
	}

	int Bot::setConfigurationFromJSONValue(App::JsonValueInterface& leg) {
		return -1;
	}

	int Bot::setConfigurationFromJSONFilePath( std::string jsonConfigurationFilePath) {
		std::ifstream jsonConfigurationFile(jsonConfigurationFilePath.c_str());
		std::stringstream jsonConfiguration;
		jsonConfiguration << jsonConfigurationFile.rdbuf();

		return setConfigurationFromJSONString(jsonConfiguration.str());
	}

	int Bot::setConfigurationFromJSONString(std::string jsonConfiguration) {
		App::JsonValueInterface root;
		App::JsonValueInterface::parse(jsonConfiguration, root);
		return setConfigurationFromJSONValue(root);
	}
	
#ifdef _cplusplus
}
#endif
