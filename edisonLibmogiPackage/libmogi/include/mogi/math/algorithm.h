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

#ifndef MOGI_ALGORITHM_H
#define MOGI_ALGORITHM_H

#include "mogi/math/mmath.h"

#include <vector>

namespace Mogi {

/*!
 * @namespace Mogi::Math
 * \brief Math tools, mainly focused on matrices, vectors, and quaternions.
 */
namespace Math {

/*! \class GrahamScan
 \brief Finds a convex hull of a set of vectors.
 
 Implementation of a graham scan algorithm on a Vector set \f$ \vec{v}_i \in \mathbb{R}^2 \f$
 */
class GrahamScan {
private:
	std::vector<Vector*>* input;
	std::vector<Vector*> result;

public:
	/*! \brief Checks how the ordered set of points are oriented with respect to each other.
	 \param p0 The first point \f$ \vec{P}_0 \in \mathbb{R}^2 \f$.
	 \param p1 The middle point \f$ \vec{P}_1 \in \mathbb{R}^2 \f$.
	 \param p2 The last point \f$ \vec{P}_2 \in \mathbb{R}^2 \f$.
	 \return \f$ r =  \left\{
	 \begin{array}{ll}
	 < 0 & \angle P_0P_1P_2 > \pi, clockwise \\
			 0 & \angle P_0P_1P_2 = \pi, colinear \\
			 > 0 & \angle P_0P_1P_2 < \pi, counterclockwise
	 \end{array}
	 \right. \f$
	 */
	static double ccw(const Vector* p0, const Vector* p1, const Vector* p2);

	/*! \brief Sets the vectors to be sorted.
	 \param input The input vectors to find the convex hull;
	 */
	void setInput(std::vector<Vector*>& input);

	/*! \brief Performs the algorithm.
	 \return The reference to the ordered set of points.
	 */
	std::vector<Vector*>& getConvexHull();

	/*! \brief After the scan is performed, this checks to see if a point is inside the hull.
	 \param p The point \f$ \vec{p} \in \mathbb{R}^2 \f$ to check if it is in the region.
	 \return True if inside the hull, false if outside.
	 */
	bool inHull(Vector& p);
};

}

}

#endif
