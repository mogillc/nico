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

#ifndef MOGI_LEG_H
#define MOGI_LEG_H

#include <map>
#include <string>

#include "mogi/robot/bot.h"
#include "mogi/math/kinematics.h"

namespace Mogi {

	/**
	 * @namespace Mogi::Robot
	 * \brief Handles robots.
	 */
	namespace Robot {

		/*! \class HexapodLeg
		 \brief An abstract class defining a hexapod leg.

		 The methods in this may be overiden to define certain aspects of the leg.  This may be used
		 to represent arbitrarily constructed legs.  It is up to the conrecte derived class to define
		 the particular configuration.

		 */
		class HexapodLeg: public Bot {
		public:

			HexapodLeg();
			virtual ~HexapodLeg() = 0;

			/*! \brief Will compute the inverse kinematics, with respect to the node location.
			 \param location The desired end effector (foot) location.
			 \return The motor angles, in radians as a vector:\f$[\theta_c,\theta_f,\theta_t]^T\f$.
			 */
			virtual const Math::Vector& computeAngles(Math::Vector& location);

			/*! \brief Sets the base location and orientation.
			 \param location The base location \f$\vec{v}_b\f$.
			 \param orientation The base orientation \f$q_b\f$.
			 */
			virtual void setBase(const Math::Vector& location, const Math::Quaternion& orientation);

			/*! \brief Gets the node of the base link.
			 \return The base link node.
			 */
			Math::Node* getBaseNode();

			/*! \brief Creates a hexapod leg based on the configuration.
			 \param jsonConfiguration The JSON configuration for this leg.
			 \param root The root node to build the structure into (i.e. the body node).
			 \return A newly allocated leg based on the configuration.  If configuration fails, NULL.
			 */
			static HexapodLeg* createFromJSON( App::JsonValueInterface& jsonConfiguration, Math::Node* root );

			/*! \brief Sets parameters of the leg if defined in the JSON configuration.
			 \param leg The JSON string containing the configuration.
			 \return 0 if success, otherwise a failure occurred.
			 */
			virtual int setConfigurationFromJSONValue( App::JsonValueInterface& leg ) = 0;

		protected:
			/*! The inverse kinematics solver
			 */
			Math::InverseKinematics* ikSolver;

			/*! \brief Sets the dimension of the node of a given child depth.
			 This should be called before any other method.
			 \param nodeNumber The node depth level to be set.
			 \param dimensions The link definition.
			 \return -1 if bad input, 0 if successfully set.
			 */
			virtual int setNodeDimensions(unsigned int nodeNumber, const Math::Vector* dimensions);

			/*! \brief Builds the kinematics node structure.
			 This should be called before any other method.
			 \param root The parent node.
			 */
			virtual void buildNodeStructure(Math::Node* root) = 0;
		};


		/*! \class HexapodLeg3DOF
		 \brief An abstract class defining a 3 Degree of Freedom hexapod leg.

		 This class is configured by setting the 3 link definitions for the base \f$\vec{l}_b\f$, coxa \f$\vec{l}_c\f$, femur \f$\vec{l}_f\f$, and tibia \f$\vec{l}_t\f$.  The base of the leg may also be set \f$\vec{v}_b\f$, as well as the base orientation quaternion \f$q_b\f$.  Finally, the desired end effector location (the foot) \f$\vec{v}_f\f$ may be set by calling HexapodLeg::computeAngles(). The corresponding angles for all the motors may then be solved: \f$[\theta_c,\theta_f,\theta_t]^T\f$.

		 */
		class HexapodLeg3DOF: public HexapodLeg {
		public:
			/*! \brief Sets the base link dimensions.
			 \param dimensions A vector of size 3, corresponding to \f$\vec{l}_b\f$.
			 \return 0 if success, -1 otherwise (dimension mismatch).
			 */
			int setBaseDimensions(const Math::Vector* dimensions);

			/*! \brief Sets the coxa link dimensions.
			 \param dimensions A vector of size 3, corresponding to \f$\vec{l}_c\f$.
			 \return 0 if success, -1 otherwise (dimension mismatch).
			 */
			int setCoxaDimensions(const Math::Vector* dimensions);

			/*! \brief Sets the femur link dimensions.
			 \param dimensions A vector of size 3, corresponding to \f$\vec{l}_f\f$.
			 \return 0 if success, -1 otherwise (dimension mismatch).
			 */
			int setFemurDimensions(const Math::Vector* dimensions);

			/*! \brief Sets the tibia link dimensions.
			 \param dimensions A vector of size 3, corresponding to \f$\vec{l}_t\f$.
			 \return 0 if success, -1 otherwise (dimension mismatch).
			 */
			int setTibiaDimensions(const Math::Vector* dimensions);

			/*! \brief Sets parameters of the leg if defined in the JSON configuration.
			 \param leg The JSON string containing the configuration.
			 \return 0 if success, otherwise a failure occurred.
			 */
			int setConfigurationFromJSONValue( App::JsonValueInterface& leg );

		};

		/*! \class HexapodLegZYY
		 \brief An example of a 3DOF hexapod leg in a ZYY axis oriented joint configuration.
		 
		 This class assumes that the coxa motor is pointed in alignment with the z axis, and the femur and tibia motors are pointed in alignment with the y axis.

		 \image html legKinematics.jpg
		 */
		class HexapodLegZYY: public HexapodLeg3DOF {
		private:
			//int setNodeDimensions(unsigned int nodeNumber, const Math::Vector* dimensions);

			/*! \brief Builds the kinematics node structure.
			 This should be called before any other method.
			 \param root The parent node.
			 */
			void buildNodeStructure(Math::Node* root);

		public:

			HexapodLegZYY();
			~HexapodLegZYY();
		};

		/*! \class HexapodLegYYX
		 \brief An example of a 3DOF hexapod leg in a ZYY axis oriented joint configuration.
		 
		 This class assumes that the coxa motor is pointed in alignment with the y axis, the femur motor is aligned witht e y axis, and the tibia motor is pointed in alignment with the x axis.

		 \image html legKinematics.jpg
		 */
		class HexapodLegYYX: public HexapodLeg3DOF {
		private:

			/*! \brief Builds the kinematics node structure.
			 This should be called before any other method.
			 \param root The parent node.
			 */
			void buildNodeStructure(Math::Node* root);

		public:

			HexapodLegYYX();
			~HexapodLegYYX();
		};
	}
}

#endif
