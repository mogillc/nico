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

MBmesh& MBmesh::operator=(const MBmesh& param) {
	name = param.name;
	indices = param.indices;
	data = param.data;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(VertexData), &data[0],
			GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
			&indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return *this;
}

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

	return totalTriangles;
}

int MBmesh::set(aiMesh* mesh, aiMaterial** materials, std::string fileLocation,
		int materialIDOffset) {
	// Name:
	if (mesh->mName.length <= 1) {
		name = fileLocation;
	} else {
		name = mesh->mName.C_Str();
	}
	std::cout << "\t\tAdding mesh: " << name << std::endl;

	// Bitangents: (not needed...?)
	objectLocation = fileLocation;
	// Bones:
	bones = getBones(mesh);

	// Faces (indices):
	// indices.clear();
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)  // 0..2
				{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Material Index:
	materialIndex = mesh->mMaterialIndex + materialIDOffset;

	// Vertex Colors:
	// Normals:
	// Tangents:
	// Texture Coordinates:
	// Vertices:
	aiColor4D col;
	aiMaterial* mat = materials[mesh->mMaterialIndex];
	aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &col);
	Vector3d defaultColor;
	defaultColor.x = col.r;
	defaultColor.y = col.g;
	defaultColor.z = col.b;
	// data.clear();
	// importTextures(mat);
	for (int i = 0; i < mesh->mNumVertices; i++) {
		VertexData tmp;
		Vector3d tmpVec;

		// position
		tmpVec.x = mesh->mVertices[i].x;
		tmpVec.y = mesh->mVertices[i].y;
		tmpVec.z = mesh->mVertices[i].z;
		tmp.position = tmpVec;

		// normals
		tmpVec.x = mesh->mNormals[i].x;
		tmpVec.y = mesh->mNormals[i].y;
		tmpVec.z = mesh->mNormals[i].z;
		tmp.normal = tmpVec;

		// tangent
		if (mesh->mTangents) {
			tmpVec.x = mesh->mTangents[i].x;
			tmpVec.y = mesh->mTangents[i].y;
			tmpVec.z = mesh->mTangents[i].z;
		} else {
			tmpVec.x = 1.0;
			tmpVec.y = tmpVec.z = 0;
		}
		tmp.tangent = tmpVec;

		// colors
		if (mesh->mColors[0]) {
			//!= material color
			tmpVec.x = mesh->mColors[0][i].r;
			tmpVec.y = mesh->mColors[0][i].g;
			tmpVec.z = mesh->mColors[0][i].b;
		} else {
			tmpVec = defaultColor;
		}
		tmp.color = tmpVec;

		// color
		if (mesh->mTextureCoords[0]) {
			tmpVec.x = mesh->mTextureCoords[0][i].x;
			tmpVec.y = mesh->mTextureCoords[0][i].y;
		} else {
			tmpVec.x = tmpVec.y = tmpVec.z = 0.0;
		}
		tmp.U = tmpVec.x;
		tmp.V = tmpVec.y;
		data.push_back(tmp);
	}

	loadVerticesToVertexBufferObject();

	totalTriangles = mesh->mNumVertices / 3;
	return totalTriangles;
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

#ifdef _cplusplus
}
#endif
