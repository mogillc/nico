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

#ifndef MOGI_JOINT_H
#define MOGI_JOINT_H

#include "mogi/math/node.h"

namespace Mogi {

	/**
	 * @namespace Mogi::Robot
	 * \brief Handles robots.
	 */
	namespace Robot {

		/*! \class Joint
		 \brief An abstract class that operates a kinematics Math::Node.
		 */
		class Joint {
		protected:
			/*! The node associated with this joint.  This class does not handle the memory.
			 */
			Math::Node* node;

			/*! The axis of operation (i.e. rotation or translation).  Initialized as the z axis.
			 */
			Math::Vector axis;

			/*! The amount applied to the axis, typically angle or translation.
			 */
			double value;

		public:
			/*!	\brief Supported derived classes for the constructor.
			 */
			typedef enum {
				PRISMATIC,  /*!< Designates a translational joint. */
				REVOLUTE,   /*!< Designates a rotational joint. */
			} Type;

			Joint();
			virtual ~Joint();

			/*! \brief Creates a concrete object based on the parameters.
			 \param node The node to operate.
			 \param axis The axis of operation.
			 \param type The type of joint to create.
			 \return The newly created object.
			 */
			static Joint* create(Math::Node* node, const Math::Vector& axis, Type type);

			/*! \brief Sets the joint's associated node to operate.

			 This becomes an association, not a containment.
			 \param node The node to operate.
			 */
			void setNode( Math::Node* node );

			/*! \brief Sets the axis of operation for this joint.
			 \param axis The axis of operation.
			 */
			void setAxis( const Math::Vector& axis );

			/*! \brief Sets the joint's kinematics component. // TODO: rename this maybe

			 This must be overridden by the derived class to define how this is performed.
			 \param value The value to operate the joint.
			 */
			virtual void setValue( double value ) = 0;

			/*! \brief Gets the currently applied joint magnitude
			 \return The currently applied magnitude of the joint.
			 */
			double getValue();
		};

		/*! \class Revolute
		 \brief Represents a joint with a rotational joint.

		 This uses the base Joint class axis as an axis of rotation.
		 */
		class Revolute : public Joint {
		public:
			/*! \brief Sets the joint angle
			 \param value The joint angle, in radians.
			 */
			void setValue( double value );
		};

		/*! \class Prismatic
		 \brief Represents a joint with a translational joint.

		 This uses the base Joint class axis as an axis of translation.
		 */
		class Prismatic : public Joint {
		public:
			/*! \brief Sets the joint distance in mm.
			 \param value The joint translation in mm.
			 */
			void setValue( double value );
		};
	}
	
}

#endif
