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

#include <iostream>
#include <math.h>
#include <stdio.h>	// printf
#include <stdlib.h>	// EXIT_SUCCESS, EXIT_FAILURE

#include "node.h"

using namespace Mogi::Math;

bool testConstructor();
bool testAddChildren();
bool testChildren();
//bool testParentMove();
bool testNodeMath();
bool testNodePrint();
//bool testMeshID();

bool matricesEqual(Matrix* A, Matrix* B) {
	bool testPassed = true;

	if (A->numRows() != B->numRows()) {
		printf(" - %s rows != %s rows, %d != %d\n", A->getName(), B->getName(),
				A->numRows(), B->numRows());
		testPassed = false;
	}

	if (A->numColumns() != B->numColumns()) {
		printf(" - %s cols != %s cols, %d != %d\n", A->getName(), B->getName(),
				A->numColumns(), B->numColumns());
		testPassed = false;
	}

	if (testPassed)
		for (int i = 0; i < A->numRows(); i++) {
			for (int j = 0; j < A->numColumns(); j++) {
				if (fabs((*A)(i, j) - (*B)(i, j)) > 0.000001) {
					testPassed = false;
					printf(" - %s(%d,%d) != %s(%d,%d), %f != %f\n",
							A->getName(), i, j, B->getName(), i, j, (*A)(i, j),
							(*B)(i, j));
				}
			}
		}

	return testPassed;
}

int main(int argc, char *argv[]) {
	bool allTestsPass = true;

	std::cout << " - Beginning Contructor tests:" << std::endl;
	allTestsPass = testConstructor() ? allTestsPass : false;
	std::cout << " - Beginning Children tests:" << std::endl;
	allTestsPass = testAddChildren() ? allTestsPass : false;
	allTestsPass = testChildren() ? allTestsPass : false;
	std::cout << " - Beginning Node Math tests:" << std::endl;
	allTestsPass = testNodeMath() ? allTestsPass : false;
//	std::cout << " - Beginning Mesh ID tests:" << std::endl;
//	allTestsPass = testMeshID() ? allTestsPass : false;
	std::cout << " - Beginning Node Print tests:" << std::endl;
	allTestsPass = testNodePrint() ? allTestsPass : false;
	//allTestsPass = testParentMove() ? allTestsPass: false;

	if (allTestsPass) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool testConstructor() {
	bool allTestsPass = true;
	Node* node = new Node;

	if (node->name != "" || node->getModelMatrix()->numColumns() != 4
			|| node->getModelMatrix()->numRows() != 4) {
		allTestsPass = false;
	}

	delete node;

	node = new Node;
	Node* node2 = new Node;
	node2 = node->adoptChild(&node2);
	int previousNumberOfChildren = node->numberOfChildren();

	std::cout << "Testing child destruction .............. ";
	delete node2;
	if (node->numberOfChildren() != 0 || previousNumberOfChildren != 1) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	delete node;

	return allTestsPass;
}

bool testAddChildren() {
	bool allTestsPass = true;

	Node* node = new Node;
	std::cout << "Testing addNode ........................ ";
	node->addNode("addNode test name");
	if (node->numberOfChildren() != 1
			|| node->child(0)->name != "addNode test name"
			|| node->child(0)->numberOfChildren() != 0) {
		std::cout << "FAILED" << std::endl;

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	delete node;

//	node = new Node;
//	std::cout << "Testing pushChild ...................... ";
//	Node* child = new Node;
//	node->pushChild(child);
//	if (node->numberOfChildren() != 1 ||
//		node->child(0) != child) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//	delete node;

	return allTestsPass;
}

bool testChildren() {
	bool allTestsPass = true;
	Node* node = new Node;

	Node* childNode = new Node;
	Node* childNodeRemember = childNode;

	std::cout << "Testing child adoption ................. ";
	childNode->name = "working";
	Node* remember2 = node->adoptChild(&childNode);
	if (childNode != NULL || remember2 != childNodeRemember
			|| node->numberOfChildren() != 1
			|| node->child(0)->name != "working"
			|| node != childNodeRemember->getParent()) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	delete node;

	node = new Node;
	std::cout << "Testing NULL child adoption ............ ";
	node->adoptChild(NULL);
	if (node->numberOfChildren() != 0) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	delete node;

	node = new Node;
	childNode = new Node;
	childNodeRemember = childNode;	// remember
	std::cout << "Testing child popping .................. ";
	node->adoptChild(&childNode);	// give up ownership
	Node* childNodeCompare = node->popChild(childNodeRemember); // get back ownership
	if (childNodeCompare != childNodeRemember
			|| node->numberOfChildren() != 0) {
		std::cout << "FAILED" << std::endl;
		std::cout << "\tchildNodeCompare = " << childNodeCompare
				<< ", childNodeRemember = " << childNodeRemember << std::endl;
		std::cout << "\tnumberOfChildren = " << node->numberOfChildren()
				<< std::endl;
		if (node->numberOfChildren() > 0) {
			std::cout << "\t\tchild(0) = " << node->child(0) << std::endl;
		}
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	delete node;

	node = new Node;
	std::cout << "Testing empty child popping ............ ";
	if (node->popChild(NULL) != NULL || node->numberOfChildren() != 0) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	delete node;

	node = new Node;
	childNode = new Node;
	Node* node2 = new Node;	// remember
	childNodeRemember = childNode;
	std::cout << "Testing child readoption ............... ";
	node->adoptChild(&childNode);	// give up ownership
	node2->adoptChild(&childNodeRemember); // get to new owner
	if (node->numberOfChildren() != 0 || node2->numberOfChildren() != 1
			|| node2->child(0)->getParent() != node2) {
		std::cout << "FAILED" << std::endl;
		std::cout << "\tnode->numberOfChildren() = " << node->numberOfChildren()
				<< ", node2->numberOfChildren() = " << node2->numberOfChildren()
				<< std::endl;

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	delete node;

	node = new Node;
	childNode = new Node;
	node2 = new Node;
	childNodeRemember = node2;
	std::cout << "Testing child findByName ............... ";
	node2->name = "name to search for";
	for (int i = 0; i < 100; i++) {
		node->addNode("dhfiuaf");
		childNode->addNode("ajhsbdfjasdfa");
		node2->addNode("123412341234123");
	}
	childNode->adoptChild(&node2);
	node->adoptChild(&childNode);	// give up ownership
	childNode = node->findChildByName("name to search for");
	if (childNodeRemember != childNode) {
		std::cout << "FAILED" << std::endl;

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	delete node;

	node = new Node;
	childNode = new Node;
	std::cout << "Testing repeat adoption ................ ";
	childNode = node->adoptChild(&childNode);
	childNode = node->adoptChild(&childNode);
	childNode = node->adoptChild(&childNode);
	if (node->numberOfChildren() != 1 || childNode->getParent() != node) {
		std::cout << "FAILED" << std::endl;

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	delete node;

	node = new Node;
	Node* nodePlay = node;
	std::cout << "Testing self adoption .................. ";
	nodePlay = nodePlay->adoptChild(&nodePlay);
	if (nodePlay == NULL || nodePlay->numberOfChildren() != 0||
	nodePlay->getParent() != NULL) {
		std::cout << "FAILED" << std::endl;

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	delete node;

	return allTestsPass;
}

bool testNodeMath() {
	bool allTestsPass = true;
	Node* node = new Node;

	Node* childNode = new Node;
	childNode = node->adoptChild(&childNode);

	std::cout << "Testing translation .................... ";
	childNode->setLocation(1, 1, 1);
	node->setLocation(-11, -1, 9);
	node->update();
	Matrix resultMatrix = *childNode->getModelMatrix();
	if (resultMatrix.numRows() != 4 || resultMatrix.numColumns() != 4
			|| resultMatrix(0, 3) != -10 || resultMatrix(1, 3) != 0
			|| resultMatrix(2, 3) != 10) {
		std::cout << "FAILED" << std::endl;

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing vector translation ............. ";
	Vector locationVector(3);
	locationVector(0) = 6;
	locationVector(1) = 7;
	locationVector(2) = 8;
	childNode->setLocation(locationVector);
	locationVector(0) = 67;
	locationVector(1) = -7;
	locationVector(2) = 3.2;
	node->setLocation(locationVector);
	node->update();
	resultMatrix = *childNode->getModelMatrix();
	if (resultMatrix.numRows() != 4 || resultMatrix.numColumns() != 4
			|| resultMatrix(0, 3) != 73 || resultMatrix(1, 3) != 0
			|| resultMatrix(2, 3) != 11.2) {
		std::cout << "FAILED" << std::endl;

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing rotation ....................... ";
	childNode->setLocation(1, 1, 1);
	node->setLocation(0, 0, 0);
	Vector weirdAxis(3);
	weirdAxis(0) = -1;
	weirdAxis(1) = 1;
	weirdAxis(2) = 0;
	node->setOrientation(MOGI_PI, weirdAxis);
	node->update();
	resultMatrix = *childNode->getModelMatrix();
	if (fabs(resultMatrix(0, 3) - -1) > .00000000001
			|| fabs(resultMatrix(1, 3) - -1) > .00000000001
			|| fabs(resultMatrix(2, 3) - -1) > .00000000001) {
		std::cout << "FAILED" << std::endl;
		resultMatrix.print();

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing scaling ........................ ";
	childNode->setLocation(1, 1, 1);
	node->setOrientation(*Quaternion::create());
	node->setScale(MOGI_PI, -exp(1), log(234234));
	node->update();
	resultMatrix = *childNode->getModelMatrix();
	if (resultMatrix(0, 3) != MOGI_PI || resultMatrix(1, 3) != -exp(1)
			|| resultMatrix(2, 3) != log(234234)) {
		std::cout << "FAILED" << std::endl;
		resultMatrix.print();

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing uniform scaling ................ ";
	childNode->setLocation(1, 1, 1);
	node->setOrientation(*Quaternion::create());
	node->setScale(log(234.234));
	node->update();
	resultMatrix = *childNode->getModelMatrix();
	if (resultMatrix(0, 3) != log(234.234) || resultMatrix(1, 3) != log(234.234)
			|| resultMatrix(2, 3) != log(234.234)) {
		std::cout << "FAILED" << std::endl;
		resultMatrix.print();

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing vector scaling ................. ";
	childNode->setLocation(1, 1, 1);
	Vector scaleVector(3);
	scaleVector(0) = -exp(1);
	scaleVector(1) = 234.52345;
	scaleVector(2) = 1 / MOGI_PI;
	node->setOrientation(*Quaternion::create());
	node->setScale(scaleVector);
	node->update();
	resultMatrix = *childNode->getModelMatrix();
	if (resultMatrix(0, 3) != -exp(1) ||
	resultMatrix(1,3) != 234.52345 ||
	resultMatrix(2,3) != 1/MOGI_PI) {
		std::cout << "FAILED" << std::endl;
		resultMatrix.print();

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing setting Model Matrix ........... ";
	Matrix modelMatrix(4, 4);
	modelMatrix(0, 0) = 2;
	modelMatrix(1, 1) = 3;
	modelMatrix(2, 2) = 16;
	modelMatrix(0, 3) = -40;
	modelMatrix(1, 3) = 2.3;
	modelMatrix(2, 3) = MOGI_PI / 9.8;
	modelMatrix(3, 3) = 1;
	node->setModelMatrix(modelMatrix);
	node->update();
	resultMatrix = *node->getModelMatrix();
	if (!matricesEqual(&modelMatrix, &resultMatrix)) {
		std::cout << "FAILED" << std::endl;
		resultMatrix.print();

		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	delete node;

	return allTestsPass;
}

//bool testParentMove() {
//	bool allTestsPass = true;
//
//	Node *testParent = new Node;
//	Node *testChild = new Node;
//	//testParent->addNode("");
//
//	std::cout << "Testing moving a child under a parent ... ";
//	testChild->moveToUnderParent(testParent);
//	if (testParent->child(0) != testChild) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	delete testParent;
//	//delete testChild;
//
//	return allTestsPass;
//}

//bool testMeshID() {
//	bool allTestsPass = true;
//
//	Node root;
//
//	std::cout << "Testing adding mesh ID ................. ";
//	root.addMeshID(6);
//	std::vector<NodeMatrixAndMeshID*> meshes = root.getMeshIDs();
//	if (meshes.size() == 1 && meshes[0]->ID == 6) {
//		std::cout << "Passed" << std::endl;
//	} else {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	}
//
//	std::cout << "Testing pushing back mesh ID ........... ";
//	NodeMatrixAndMeshID* newMeshId = new NodeMatrixAndMeshID;
//	newMeshId->ID = 9;
//	root.pushMatrixAndMeshID(newMeshId);
//	meshes = root.getMeshIDs();
//	if (meshes.size() == 2 && meshes[1]->ID == 9) {
//		std::cout << "Passed" << std::endl;
//	} else {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	}
//
//	std::cout << "Testing child mesh ID .................. ";
//	Node *child = root.addNode("child");
//	newMeshId = new NodeMatrixAndMeshID;
//	newMeshId->ID = 1000;
//	child->pushMatrixAndMeshID(newMeshId);
//	root.adoptChild(&child);
//	meshes = root.getMeshIDs();
//	if (meshes.size() == 3 && meshes[2]->ID == 1000) {
//		std::cout << "Passed" << std::endl;
//	} else {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	}
//
//	return allTestsPass;
//}

bool testNodePrint() {
	bool allTestsPass = true;

	Node root;
	root.name = "root";
	root.addNode("child1");
	root.addNode("child2")->addNode("grandchild1");

	std::cout << "Testing node printing: ................. ";

	std::string expected(
			"\t: root\n\t|--: child1\n\t|--: child2\n\t|--|--: grandchild1\n");

	if (expected.compare(root.structureToString()) != 0) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}

