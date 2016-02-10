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

#include "mogi/math/node.h"

#ifdef THREADS_FOUND
#include <pthread.h>
#else
#warning Threads not found.  This may still compile but will not be thread safe.
#endif

using namespace std;

#ifdef _cplusplus
extern "C"
{
#endif

namespace Mogi {
namespace Math {

Node::Node() {
	parent = NULL;
	//transformationMatrix.makeI(4);
	//modelMatrix.makeI(4);
	outputMatrix.makeI(4);
	scaleM.makeI(4);
	locationM.makeI(4);
	orientationM.makeI(4);

#ifdef THREADS_FOUND
	_mutex = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*) _mutex, NULL);
#endif
}

Node::~Node() {
	clearChildren();

	if (parent)
		parent->popChild(this);
#ifdef THREADS_FOUND
	pthread_mutex_destroy((pthread_mutex_t*) _mutex);
	delete (pthread_mutex_t*) _mutex;
#endif
}

void Node::clearChildren() {
	for (std::vector<Node*>::iterator it = children.begin();
			it != children.end();) {// no need to increment since we tell the parent to pop.
		if (*it != NULL) {
			delete ((Node*) (*it));
		}
	}
	children.clear();
}

int Node::prettyPrintTracker = 0;
std::string Node::structureToString() {
	std::string output("\t");
	//std::cout << "\t";
	for (int i = 0; i < prettyPrintTracker; i++) {
		output += "|--";
	}
	output += ": " + name + "\n";

	for (std::vector<Node*>::iterator it = children.begin();
			it != children.end(); it++) {
		prettyPrintTracker++;
		output += (*it)->structureToString();
		prettyPrintTracker--;
	}
	return output;
}

Node* Node::addNode(std::string name) {
	Node *child = new Node;
	child->name = name;
	child->parent = this;
	children.push_back(child);
	return child;
}
//
//	void Node::moveToUnderParent( Node *newParent ) {
//		if (newParent == this) {
//			return;
//		}
//		// Erase me from my parent's children:
//		if (parent) {
//			for ( std::vector<Node*>::iterator it = parent->children.begin(); it != parent->children.end(); it++) {
//				//Node *node = *it;
//				if (*it == this) {
//					//parent->children.erase( parent->children.begin() + i);
//					parent->children.erase(it);
//					break;
//				}
//			}
//
//
//			// Adopt me under my new parent:
//			parent = newParent;
//			parent->children.push_back(this);
//		} else {
//			parent = newParent;
//			//parent->children.push_back(this);
//		}
//
//	}

Node* Node::adoptChild(Node** child) {
	if (child == NULL) {
		return NULL;
	}
	if (*child == NULL || *child == this) {
		return *child;
	}
	// First remove the child from any existing parent
	if ((*child)->parent != NULL) {
		(*child)->parent->popChild(*child);
	}
	(*child)->parent = this;

	// Next, add this child to the list of children:
	children.push_back(*child);

	// Finally some cleanup, set the pointer to NULL:
	Node* remember = *child;
	*child = NULL;
	return remember;
}

Node* Node::popChild(Node* child) {
	for (std::vector<Node*>::iterator it = children.begin();
			it != children.end(); it++) {
		if (*it == child) {
			child->parent = NULL;	// no more parent for you!
			children.erase(it);
			return child;
		}
	}
	return NULL;
}

void Node::setModelMatrix(Matrix& newModel) {
	//modelMatrix = newModel;
	// This should set the scale, location, and orientation instead...
	//Matrix loc, sca, ori;
	//std::cout << "Setting model matrix for node: " << name << std::endl;
	transformationToScaleRotationTranslation(&newModel, &scaleM, &orientationM,
			&locationM);

}

Node* Node::findChildByName(std::string childName) {
	if (childName == name) {
		return this;
	}

	for (std::vector<Node*>::iterator it = children.begin();
			it != children.end(); it++) {
		Node *temp = (*it)->findChildByName(childName);
		if (temp) {
			return temp;
		}
	}

	return NULL;
}

const Matrix* Node::getModelMatrix() {
	return &outputMatrix;
}

void Node::updateChildren() {
	// Trickle to the result down the children:
	for (std::vector<Node*>::iterator it = children.begin();
			it != children.end(); it++) {
		(*it)->update();			//( outputMatrix );
	}
}

void Node::update() {
	// First compute my matrix:
#ifdef THREADS_FOUND
	pthread_mutex_lock((pthread_mutex_t*) _mutex);
#endif
	if (parent) {
		outputMatrix = *parent->getModelMatrix() * locationM * orientationM
				* scaleM;
	} else {
		outputMatrix = locationM * orientationM * scaleM;
	}

	updateChildren();
#ifdef THREADS_FOUND
	pthread_mutex_unlock((pthread_mutex_t*) _mutex);
#endif
}

void Node::setLocation(const Vector& loc) {
	if ((loc.numRows() == 3) && (loc.numColumns() == 1)) {
		setLocation(loc.valueAsConst(0, 0), loc.valueAsConst(1, 0),
				loc.valueAsConst(2, 0));
	}
}

void Node::setLocation(double x, double y, double z) {
	locationM.value(0, 3) = x;
	locationM.value(1, 3) = y;
	locationM.value(2, 3) = z;
}

void Node::setScale(const Vector& loc) {
	if ((loc.numRows() == 3) && (loc.numColumns() == 1)) {
		setScale(loc.valueAsConst(0, 0), loc.valueAsConst(1, 0),
				loc.valueAsConst(2, 0));
	}
}

void Node::setScale(double x, double y, double z) {
	scaleM.value(0, 0) = x;
	scaleM.value(1, 1) = y;
	scaleM.value(2, 2) = z;
}

void Node::setScale(double s) {
	setScale(s, s, s);
}

void Node::setOrientation(const Quaternion& quat) {
	orientationM = quat.makeRotationMatrix4();
}

void Node::setOrientation(double angle, const Vector& axis) {
	setOrientation(Quaternion::createFromAngleAndAxis(angle, axis));
}

Node* Node::child(unsigned int index) {
	return children[index];
}

int Node::numberOfChildren() {
	return children.size();
}

Node* Node::getParent() {
	return parent;
}

}
}

#ifdef _cplusplus
}
#endif
