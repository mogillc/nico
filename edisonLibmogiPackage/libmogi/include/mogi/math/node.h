/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *            This program is distributed under the LGPL, version 2           *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License              *
 *   version 2.1 as published by the Free Software Foundation;                *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   https://github.com/mogillc/nico/tree/master/edisonLibmogiPackage/libmogi *
 *                                                                            *
 *****************************************************************************/

#ifndef MOGI_NODE_H
#define MOGI_NODE_H

#include "mogi/math/mmath.h"

#include <vector>
#include <string>
//#ifdef UNIX
//#include <iostream>
//#else
//#include <stdlib.h>
//#endif

namespace Mogi {

/**
 * @namespace Mogi::Math
 * \brief Math tools, mainly focused on matrices, vectors, and quaternions.
 */
namespace Math {

class Node;

/*!
 \class Node
 \brief Used for constructing a kinematic system.
 */
class Node {
private:

	void* _mutex;

	/*! Keeps track of the tree level for printing.
	 */
	static int prettyPrintTracker;

	/*! The parent of this node.  Null if no parent exists, i.e. a root node.
	 */
	Node *parent;

	/*! The children nodes of this node.
	 */
	std::vector<Node*> children;

	/*! The final output transformation matrix considering the path to the root node.
	 */
	Matrix outputMatrix;

	/*! The transformation scale component S, where the transformation matrix is computed as T*R*S.
	 */
	Matrix scaleM;

	/*! The transformation translation component T, where the transformation matrix is computed as T*R*S.
	 */
	Matrix locationM;

	/*! The transformation rotation component R, where the transformation matrix is computed as T*R*S.
	 */
	Matrix orientationM;
	
	/*! \brief Called to update each children matrix outputMatrix, recursively performed.
	 */
	void updateChildren();
public:

	/*! \brief The label of this node.
	 */
	std::string name;

	Node();
	~Node();

	/**
	 * Updates the transformation matrix, and updates all children
	 */
	void update();
	//void update( Matrix& parentMatrix);

	/*! \brief Returns the outputMatrix, the transformation matrix  of this node with respect to the origin.
	 \return The output matrix.
	 */
	const Matrix* getModelMatrix();

	/*! \brief Assigns the scale S, translation T, and rotation R matrices of this node.
	 \param newModel The transformation matrix represented as T*R*M.
	 */
	void setModelMatrix(Matrix& newModel);

	/*! \brief Creates a formatted string of the node structure from this node and lower.
	 \return The formatted structure as a string.
	 */
	std::string structureToString();

	// Transformation manipulation:
	/*! \brief Sets the translation transformation matrix from a location vector.
	 \param location The location to set the transformation, a vector of 3 elements of x, y, and z.
	 */
	void setLocation(const Vector& location);

	/*! \brief Sets the translation transformation matrix from a vector location.
	 \param x The x location.
	 \param y The y location.
	 \param z The z location.
	 */
	void setLocation(double x, double y, double z);

	/*! \brief Sets the translation transformation matrix from a vector containing three scales.
	 \param scale The scale to set the transformation, a vector of 3 elements of x, y, and z.
	 */
	void setScale(const Vector& scale);

	/*! \brief Sets the translation transformation matrix from a vector location.
	 \param x The x location.
	 \param y The y location.
	 \param z The z location.
	 */
	void setScale(double x, double y, double z);

	/*! \brief Sets the scale of all three dimensions.
	 \param scale The scale to set all three scale values in x, y, and z.
	 */
	void setScale(double scale);

	/*! \brief Sets the rotation matrix component of the transformation matrix.
	 \param orientation The orientation of the node.
	 */
	void setOrientation(const Quaternion& orientation);

	/*! \brief Sets the orientation of the transformation matrix from a given angle and rotation axis.
	 \param angle The amount of rotation, in radians.
	 \param axis The rotation axis.
	 */
	void setOrientation(double angle, const Vector& axis);

	/*! \brief Creates a node as a child.
	 \param name The label associated with the new child node.
	 \return The node that was created.
	 */
	Node* addNode(std::string name);

	/*! \brief Finds a node in the tree structure based on the node label.
	 This returns the first node found, so it is up to user to ensure uniqueness.
	 \param childName The name of the node to search for.
	 \return The found node, otherwise a null pointer.
	 */
	Node* findChildByName(std::string childName);

	/*! \brief Returns a child by the index.  This may be deprecated soon as refactoring may be needed.
	 \param index The index of the child.
	 \return The associated child.
	 */
	Node* child(unsigned int index);

	/*! \brief Takes memory ownership over the node, and is added to the children.  This may be deprecated soon as refactoring may be needed.
	 \param child The pointer to the child pointer.
	 \return The child that was adopted.
	 */
	Node* adoptChild(Node** child);

	/*! \brief Removes a child from the list.  This may be deprecated soon as refactoring may be needed.
	 \param child The child to remove from the list.
	 \return The child removed from the list.  Null if there were no matches in the list.
	 */
	Node* popChild(Node* child);

	/*! \brief Returns the total number of children under this node.
	 \return The number of child nodes.
	 */
	int numberOfChildren();

	/*! \brief Returns the parent node.
	 \return The parent of this node, null if there is no parent.
	 */
	Node* getParent();

};

}

}

#endif
