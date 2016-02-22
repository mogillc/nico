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

#include "mesh.h"

using namespace std;

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

MBmesh::MBmesh() {
	glGenBuffers(1, &vertexBufferObject);
	glGenBuffers(1, &indexBufferObject);
}

MBmesh::~MBmesh() {
	glDeleteBuffers(1, &vertexBufferObject);
	glDeleteBuffers(1, &indexBufferObject);

	for (int i = 0; i < bones.size(); i++) {
		delete bones[i];
	}
	bones.clear();
}

//MBmesh& MBmesh::operator=(const MBmesh& param) {
//	name = param.name;
//	indices = param.indices;
//	data = param.data;
//
//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
//	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(VertexData), &data[0],
//			GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
//			&indices[0], GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	return *this;
//}

int MBmesh::drawWithMaterials(MBshader* shader,
		std::vector<MBmaterial*>& materials) {
	materials[materialIndex]->sendToShader(shader);

	return draw(shader);
}

//	void MBmesh::sendMatricesToShader( MBshader *shader, Camera *camera) {
//		MBmatrix myModelViewMatrix = camera->getViewMatrix() * ;
//		MBmatrix myModelViewProjectionMatrix =
//camera->getProjectionMatrix() * myModelViewMatrix;
//		MBmatrix myNormalMatrix =
//GLKMatrix4GetMatrix3(myModelViewMatrix);
//	}

int MBmesh::draw(MBshader* shader) {
	// attribute vec3 vertex
	GLint vertex = glGetAttribLocation(shader->program(), "position");  // 0
	GLint normal = glGetAttribLocation(shader->program(), "normal");  // 1
	GLint tangent = glGetAttribLocation(shader->program(), "tangent");  // 2
	GLint color = glGetAttribLocation(shader->program(), "color");  // 3
	GLint UV = glGetAttribLocation(shader->program(), "UV");  // 4

//	GLint currentObject;
//	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentObject);
//	if (currentObject != vertexBufferObject) {
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); // TODO: would it be better to not bind every time when rendering the same mesh multiple times in a row?
//	}
//	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentObject);
//	if (currentObject != indexBufferObject) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
//	}

	glEnableVertexAttribArray(vertex);
	glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0);

	glEnableVertexAttribArray(normal);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
			(void*) (3 * sizeof(float)));

	glEnableVertexAttribArray(tangent);
	glVertexAttribPointer(tangent, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
			(void*) (6 * sizeof(float)));

	glEnableVertexAttribArray(color);
	glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
			(void*) (9 * sizeof(float)));

	glEnableVertexAttribArray(UV);
	glVertexAttribPointer(UV, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData),
			(void*) (12 * sizeof(float)));

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(vertex);
	glDisableVertexAttribArray(normal);
	glDisableVertexAttribArray(tangent);
	glDisableVertexAttribArray(color);
	glDisableVertexAttribArray(UV);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return getNumberOfVertices()/3;
}

	void MBmesh::setObjectLocation(const std::string& path) {
		this->objectLocation = path;
	}

	void MBmesh::setBones( const std::vector<Bone*>& bones) {
		this->bones = bones;
	}

	void MBmesh::setMaterialIndex( const int& index ) {
		this->materialIndex = index;
	}

	void MBmesh::setIndices( const std::vector<unsigned int>& indices ) {
		this->indices = indices;
	}

	void MBmesh::setVertexData( const std::vector<VertexData>& vertexData ) {
		data = vertexData;
	}

	int MBmesh::getNumberOfVertices() {
		return data.size();
	}

	void MBmesh::setColor(float red, float green, float blue) {
		for (std::vector<VertexData>::iterator it = data.begin(); it != data.end(); it++) {
			(*it).color.x = red;
			(*it).color.y = green;
			(*it).color.z = blue;
		}
	}

	void MBmesh::loadVerticesToVertexBufferObject() {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(VertexData), &data[0],
					 GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
					 &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

void MBmesh::matchBonesToNodes(Math::Node* rootNode) {
	for (int i = 0; i < bones.size(); i++) {
		bones[i]->findNode(rootNode);
	}
}

	void MBmesh::generateCodeFromMesh() {
		std::cout << "Generated post process:" << std::endl;

		std::cout << "void generatedPostProcessMeshCode(MBmesh* meshToSet) {" << std::endl;
		std::cout << "VertexData data;" << std::endl;
		std::cout << "std::vector<VertexData> vertexData;" << std::endl;
		for (std::vector<VertexData>::iterator it = getVertexData().begin(); it != getVertexData().end(); it++) {
			std::cout << "data.normal.x = " << (*it).normal.x << ";" << std::endl;
			std::cout << "data.normal.y = " << (*it).normal.y << ";" << std::endl;
			std::cout << "data.normal.z = " << (*it).normal.z << ";" << std::endl;
			std::cout << "data.position.x = " << (*it).position.x << ";" << std::endl;
			std::cout << "data.position.y = " << (*it).position.y << ";" << std::endl;
			std::cout << "data.position.z = " << (*it).position.z << ";" << std::endl;
			std::cout << "data.color.x = " << (*it).color.x << ";" << std::endl;
			std::cout << "data.color.y = " << (*it).color.y << ";" << std::endl;
			std::cout << "data.color.z = " << (*it).color.z << ";" << std::endl;
			std::cout << "data.tangent.x = " << (*it).tangent.x << ";" << std::endl;
			std::cout << "data.tangent.y = " << (*it).tangent.y << ";" << std::endl;
			std::cout << "data.tangent.z = " << (*it).tangent.z << ";" << std::endl;
			std::cout << "data.U = " << (*it).U << ";" << std::endl;
			std::cout << "data.V = " << (*it).V << ";" << std::endl;
			std::cout << "vertexData.push_back(data);" << std::endl;
		}

		std::cout << "unsigned int index;" << std::endl;
		std::cout << "std::vector<unsigned int> indices;" << std::endl;
		for (std::vector<unsigned int>::iterator it = getIndices().begin(); it != getIndices().end(); it++) {
			std::cout << "index = " << (*it) << ";" << std::endl;
			std::cout << "indices.push_back(index);" << std::endl;
		}
		std::cout << "meshToSet->setVertexData(vertexData);" << std::endl;
		std::cout << "meshToSet->setIndices(indices);" << std::endl;
		std::cout << "meshToSet->loadVerticesToVertexBufferObject();" << std::endl;
		std::cout << "}" << std::endl;

	}

#ifdef _cplusplus
}
#endif
