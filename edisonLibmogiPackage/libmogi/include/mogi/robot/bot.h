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

#ifndef MOGI_ROBOT_H
#define MOGI_ROBOT_H

#include <string>
#include "mogi/math/node.h"
#include "mogi/app/json.h"

#include "mogi/robot/joint.h"

namespace Mogi {

	/**
	 * @namespace Mogi::Robot
	 * \brief Handles robots.
	 */
	namespace Robot {

		/*! \class Bot
		 \brief An abstract class that supports a kinematics Math::Node based robot construction.
		 */
		class Bot {
		protected:
			/*! The root node of the robot.
			 */
			Math::Node* nodeLocation;

			/*! The joint definitions of the robot.
			 */
			std::vector<Joint*> joints;

			/*! \brief Sets up the node based on a parent node.
			 \param node The parent node.  If NULL, a node is allocated.
			 */
			void setupNode(Math::Node* node);

			/*! \brief Performs the node construction.
			 \param root The root node.
			 */
			void buildNodeStructure(Math::Node* root);

		public:
			/*! The robot label.
			 */
			std::string name;

			Bot();
			virtual ~Bot();

			/*! \brief Updates all child nodes.
			 \param dTime The time step, if needed.
			 */
			virtual void update(double dTime);

			/*! \brief Sets the angles of all joints, for forward kinematics.
			 \param angles The angles of the joints.
			 \return 0 if success, -1 otherwise (i.e. a dimension mismatch).
			 */
			virtual int setKinematics(const Math::Vector& angles);

			/*! \brief Gets the current kinemaatic magnitudes
			 \return The currently applied joint kinematics (angle and translation magnitudes).
			 */
			Math::Vector getKinematics();

			/*! \brief Sets parameters of the hexapod if defined in the JSON configuration file.
			 \param jsonConfigurationFilePath The file containing the JSON configuration.
			 \return 0 if success, otherwise a failure occurred.
			 */
			int setConfigurationFromJSONFilePath(std::string jsonConfigurationFilePath);

			/*! \brief Sets parameters of the hexapod if defined in the JSON configuration.
			 \param jsonConfiguration The JSON string containing the configuration.
			 \return 0 if success, otherwise a failure occurred.
			 */
			int setConfigurationFromJSONString(std::string jsonConfiguration);

#ifdef LIBJSONCPP_FOUND
			/*! \brief Sets parameters of the robot if defined in the JSON configuration.
			 \param leg The JSON string containing the configuration.
			 \return 0 if success, otherwise a failure occurred.
			 */
			virtual int setConfigurationFromJSONValue(Json::Value leg);
#endif
		};
		
	}
	
}

#endif
