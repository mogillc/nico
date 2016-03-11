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
#include <math.h>
#include "dynamicShader.h"
#include <iostream>

using namespace std;

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

MBmesh::MBmesh()
	{// : material(NULL){
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


//	void MBmesh::sendMatricesToShader( MBshader *shader, Camera *camera) {
//		MBmatrix myModelViewMatrix = camera->getViewMatrix() * ;
//		MBmatrix myModelViewProjectionMatrix =
//camera->getProjectionMatrix() * myModelViewMatrix;
//		MBmatrix myNormalMatrix =
//GLKMatrix4GetMatrix3(myModelViewMatrix);
//	}

	void MBmesh::bindForDrawing(MBshader* shader) {
		// attribute vec3 vertex
		vertex = shader->getAttributeLocation("position");// glGetAttribLocation(shader->program(), "position");  // 0
		normal = shader->getAttributeLocation("normal");// glGetAttribLocation(shader->program(), "normal");  // 1
		tangent = shader->getAttributeLocation("tangent");// glGetAttribLocation(shader->program(), "tangent");  // 2
		//	GLint bitangent = glGetAttribLocation(shader->program(), "bitangent");  // 2
		color = shader->getAttributeLocation("color");// glGetAttribLocation(shader->program(), "color");  // 3
		UV = shader->getAttributeLocation("UV");// glGetAttribLocation(shader->program(), "UV");  // 4

//		std::cout << "vertex = " << vertex << std::endl;
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
		glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (3 * sizeof(float)));

		glEnableVertexAttribArray(tangent);
		glVertexAttribPointer(tangent, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (6 * sizeof(float)));

		//	glEnableVertexAttribArray(bitangent);
		//	glVertexAttribPointer(bitangent, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (9 * sizeof(float)));

		glEnableVertexAttribArray(color);
		glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (9 * sizeof(float)));

		glEnableVertexAttribArray(UV);
		glVertexAttribPointer(UV, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*) (12 * sizeof(float)));

	}

	int MBmesh::draw(MBshader* shader) {
		glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);

		return getNumberOfVertices()/3;
	}

	void MBmesh::unbindFromDrawing() {

		glDisableVertexAttribArray(vertex);
		glDisableVertexAttribArray(normal);
		glDisableVertexAttribArray(tangent);
		//	glDisableVertexAttribArray(bitangent);
		glDisableVertexAttribArray(color);
		glDisableVertexAttribArray(UV);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}



	void MBmesh::setObjectLocation(const std::string& path) {
		this->objectLocation = path;
	}

	void MBmesh::setBones( const std::vector<Bone*>& bones) {
		this->bones = bones;
	}

	void MBmesh::setIndices( const std::vector<unsigned int>& indices ) {
		this->indices = indices;
	}

	void MBmesh::setVertexData( const std::vector<VertexData>& vertexData ) {
		data = vertexData;
	}

	int MBmesh::getNumberOfVertices() {
		return (int)data.size();
	}

	void MBmesh::setColor(float red, float green, float blue) {
		for (std::vector<VertexData>::iterator it = data.begin(); it != data.end(); it++) {
			(*it).color.r = red;
			(*it).color.g = green;
			(*it).color.b = blue;
		}
	}

	void MBmesh::loadVerticesToVertexBufferObject() {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

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
			std::cout << "data.color.r = " << (*it).color.r << ";" << std::endl;
			std::cout << "data.color.g = " << (*it).color.g << ";" << std::endl;
			std::cout << "data.color.b = " << (*it).color.b << ";" << std::endl;
			std::cout << "data.tangent.x = " << (*it).tangent.x << ";" << std::endl;
			std::cout << "data.tangent.y = " << (*it).tangent.y << ";" << std::endl;
			std::cout << "data.tangent.z = " << (*it).tangent.z << ";" << std::endl;
//			std::cout << "data.bitangent.x = " << (*it).bitangent.x << ";" << std::endl;
//			std::cout << "data.bitangent.y = " << (*it).bitangent.y << ";" << std::endl;
//			std::cout << "data.bitangent.z = " << (*it).bitangent.z << ";" << std::endl;
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

	void MBmesh::makePlane(double width, double height) {
		std::vector<VertexData> vertexData(4);
		std::vector<unsigned int> indices(6);

		for (int i = 0; i < 2; i++) {	// y
			for (int j = 0; j < 2; j++) {	// x
				vertexData[j+i*2].U = j;
				vertexData[j+i*2].V = i;
				vertexData[j+i*2].position.x = ((double)j - 1.0/2.0)*width;
				vertexData[j+i*2].position.y = ((double)i - 1.0/2.0)*height;
				vertexData[j+i*2].normal.z = 1;
				vertexData[j+i*2].tangent.x = 1;
			}
		}

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 3;
		indices[4] = 2;
		indices[5] = 1;

		setVertexData(vertexData);
		setIndices(indices);

		loadVerticesToVertexBufferObject();
	}

	// Note, an attempt was made to reduce the number of rendered faces and vertices, however the top and bottom vertex
	// result in peculiar tangent twisting that seems to affect the lighting in the TBN matrix.
	void MBmesh::makeSphere(double radius, int numberLatitudeLines, int numberLongitudeLines) {
		if (numberLongitudeLines < 3 || numberLatitudeLines < 3) {
			std::cerr << "Error: MBmesh::makeSphere(): Please specify more than 3 longitude and latitude lines." << std::endl;
			return;
		}
		numberLatitudeLines--;
		VertexData data;
		std::vector<VertexData> vertexData;
		for (int i = 1; i < numberLatitudeLines; i++) {
			for (int j = 0; j < numberLongitudeLines; j++) {
				Math::Vector normal(3);
				data.normal.x = sin(MOGI_PI * (double)i/numberLatitudeLines)*cos(2.0*MOGI_PI * (double)j/numberLongitudeLines);
				data.normal.y = sin(MOGI_PI * (double)i/numberLatitudeLines)*sin(2.0*MOGI_PI * (double)j/numberLongitudeLines);
				data.normal.z = cos(MOGI_PI * (double)i/numberLatitudeLines);
				normal(0) = data.normal.x;
				normal(1) = data.normal.y;
				normal(2) = data.normal.z;
				data.position.x = radius * data.normal.x;
				data.position.y = radius * data.normal.y;
				data.position.z = radius * data.normal.z;
				Math::Vector tangent(3);
				tangent(0) = cos(MOGI_PI * (double)i/numberLatitudeLines)*cos(2.0*MOGI_PI * (double)j/numberLongitudeLines);	// TODO: test this
				tangent(1) = cos(MOGI_PI * (double)i/numberLatitudeLines)*sin(2.0*MOGI_PI * (double)j/numberLongitudeLines);
				tangent(2) = -sin(MOGI_PI * (double)i/numberLatitudeLines);
				tangent.normalize();
				data.tangent.x = tangent(0);	// TODO: test this
				data.tangent.y = tangent(1);
				data.tangent.z = tangent(2);
//				Math::Vector bitangent = normal.cross(tangent);
//				data.bitangent.x = bitangent(0);	// TODO: test this
//				data.bitangent.y = bitangent(1);
				//				data.bitangent.z = bitangent(2);
				data.U = (double)j/numberLongitudeLines;
				data.V = (double)i/numberLatitudeLines;
				vertexData.push_back(data);
			}
		}

		// Top Vertex:
		Math::Vector normal(3);
		data.normal.x = 0;
		data.normal.y = 0;
		data.normal.z = 1;
		normal(0) = data.normal.x;
		normal(1) = data.normal.y;
		normal(2) = data.normal.z;
		data.position.x = radius * data.normal.x;
		data.position.y = radius * data.normal.y;
		data.position.z = radius * data.normal.z;
		Math::Vector tangent(3);
		tangent(0) = 0;	// TODO: test this
		tangent(1) = 1;
		tangent(2) = 0;
		tangent.normalize();
		data.tangent.x = tangent(0);	// TODO: test this
		data.tangent.y = tangent(1);
		data.tangent.z = tangent(2);
		//				Math::Vector bitangent = normal.cross(tangent);
		//				data.bitangent.x = bitangent(0);	// TODO: test this
		//				data.bitangent.y = bitangent(1);
		//				data.bitangent.z = bitangent(2);
		data.U = 0.5;
		data.V = 0;
		vertexData.push_back(data);

		// Bottom Vertex:
//		Math::Vector normal(3);
		data.normal.x = 0;
		data.normal.y = 0;
		data.normal.z = -1;
		normal(0) = data.normal.x;
		normal(1) = data.normal.y;
		normal(2) = data.normal.z;
		data.position.x = radius * data.normal.x;
		data.position.y = radius * data.normal.y;
		data.position.z = radius * data.normal.z;
//		Math::Vector tangent(3);
		tangent(0) = 0;	// TODO: test this
		tangent(1) = -1;
		tangent(2) = 0;
		tangent.normalize();
		data.tangent.x = tangent(0);	// TODO: test this
		data.tangent.y = tangent(1);
		data.tangent.z = tangent(2);
		//				Math::Vector bitangent = normal.cross(tangent);
		//				data.bitangent.x = bitangent(0);	// TODO: test this
		//				data.bitangent.y = bitangent(1);
		//				data.bitangent.z = bitangent(2);
		data.U = 0.5;
		data.V = 0;
		vertexData.push_back(data);


		std::vector<unsigned int> indices;
		// Fill in the center main section:
		for (int i = 1; i < (numberLatitudeLines-1); i++) {
			for (int j = 1; j < numberLongitudeLines; j++) {
				indices.push_back(j + i*numberLongitudeLines); // this index
				indices.push_back(j + (i-1)*numberLongitudeLines); // the index above
				indices.push_back((j-1) + i*numberLongitudeLines); // the index left

				indices.push_back(j + (i-1)*numberLongitudeLines); // the index above
				indices.push_back((j-1) + (i-1)*numberLongitudeLines); // the index above left
				indices.push_back((j-1) + i*numberLongitudeLines); // the index left
			}

			// Stich up the ends:
			indices.push_back(i*numberLongitudeLines); // this index
			indices.push_back((i-1)*numberLongitudeLines); // the index above
			indices.push_back(numberLongitudeLines-1 + i*numberLongitudeLines); // the index left

			indices.push_back((i-1)*numberLongitudeLines); // the index above
			indices.push_back(numberLongitudeLines-1 + (i-1)*numberLongitudeLines); // the index above left
			indices.push_back(numberLongitudeLines-1 + i*numberLongitudeLines); // the index left

		}

		// Fill in the top and bottom caps:
		// Fill in the first triangle:
		indices.push_back(0 ); // this index (j = 0)
		indices.push_back( vertexData.size()-2); // the index above
		indices.push_back(numberLongitudeLines-1); // the index left (j = numberLongitudeLines)

			for (int j = 1; j < numberLongitudeLines; j++) {
				// Fill in the top ring:
				indices.push_back(j ); // this index
				indices.push_back( vertexData.size()-2); // the index above
				indices.push_back(j-1); // the index left

				// Fill in the bottom ring:
				indices.push_back(j + (numberLatitudeLines-2)*numberLongitudeLines); // this index
				indices.push_back(j-1 + (numberLatitudeLines-2)*numberLongitudeLines); // the index left
				indices.push_back( vertexData.size()-1); // the index above
			}
		// fill in the bottom final triangle:
		indices.push_back(0  + (numberLatitudeLines-2)*numberLongitudeLines); // this index
		indices.push_back(numberLongitudeLines-1 + (numberLatitudeLines-2)*numberLongitudeLines); // the index left
		indices.push_back( vertexData.size()-1); // the index below

		setVertexData(vertexData);
		setIndices(indices);

		loadVerticesToVertexBufferObject();
	}

	void MBmesh::makeSphereCorrectUV(double radius, int numberLatitudeLines, int numberLongitudeLines) {
			if (numberLongitudeLines < 3 || numberLatitudeLines < 3) {
				std::cerr << "Error: MBmesh::makeSphereCorrectUV(): Please specify more than 3 longitude and latitude lines." << std::endl;
				return;
			}
			numberLatitudeLines--;
			VertexData data;
			std::vector<VertexData> vertexData;
			for (int i = 0; i <= numberLatitudeLines; i++) {
				for (int j = 0; j <= numberLongitudeLines; j++) {
					Math::Vector normal(3);
					data.normal.x = sin(MOGI_PI * (double)i/numberLatitudeLines)*cos(2.0*MOGI_PI * (double)j/numberLongitudeLines);
					data.normal.y = sin(MOGI_PI * (double)i/numberLatitudeLines)*sin(2.0*MOGI_PI * (double)j/numberLongitudeLines);
					data.normal.z = cos(MOGI_PI * (double)i/numberLatitudeLines);
					normal(0) = data.normal.x;
					normal(1) = data.normal.y;
					normal(2) = data.normal.z;
					data.position.x = radius * data.normal.x;
					data.position.y = radius * data.normal.y;
					data.position.z = radius * data.normal.z;
					Math::Vector tangent(3);
					tangent(0) = cos(MOGI_PI * (double)i/numberLatitudeLines)*cos(2.0*MOGI_PI * (double)j/numberLongitudeLines);	// TODO: test this
					tangent(1) = cos(MOGI_PI * (double)i/numberLatitudeLines)*sin(2.0*MOGI_PI * (double)j/numberLongitudeLines);
					tangent(2) = -sin(MOGI_PI * (double)i/numberLatitudeLines);
					tangent.normalize();
					data.tangent.x = tangent(0);	// TODO: test this
					data.tangent.y = tangent(1);
					data.tangent.z = tangent(2);
					//				Math::Vector bitangent = normal.cross(tangent);
					//				data.bitangent.x = bitangent(0);	// TODO: test this
					//				data.bitangent.y = bitangent(1);
					//				data.bitangent.z = bitangent(2);
					data.U = (double)j/numberLongitudeLines;
					data.V = (double)i/numberLatitudeLines;
					vertexData.push_back(data);
				}
			}

		std::vector<unsigned int> indices;
		// Fill in the center main section:
		for (int i = 2; i < numberLatitudeLines; i++) {
			for (int j = 1; j <= numberLongitudeLines; j++) {
				indices.push_back(j + i*(numberLongitudeLines+1)); // this index
				indices.push_back(j + (i-1)*(numberLongitudeLines+1)); // the index above
				indices.push_back((j-1) + i*(numberLongitudeLines+1)); // the index left

				indices.push_back(j + (i-1)*(numberLongitudeLines+1)); // the index above
				indices.push_back((j-1) + (i-1)*(numberLongitudeLines+1)); // the index above left
				indices.push_back((j-1) + i*(numberLongitudeLines+1)); // the index left
			}
		}
		for (int j = 1; j <= numberLongitudeLines; j++) {
			// top cap:
			indices.push_back(j + 1*(numberLongitudeLines+1)); // this index
			indices.push_back(j + (1-1)*(numberLongitudeLines+1)); // the index above
			indices.push_back((j-1) + 1*(numberLongitudeLines+1)); // the index left

			// bottom cap:
			indices.push_back(j + (numberLatitudeLines-1)*(numberLongitudeLines+1)); // this index
			indices.push_back((j-1) + (numberLatitudeLines-1)*(numberLongitudeLines+1)); // the index left
			indices.push_back(j + ((numberLatitudeLines-1)+1)*(numberLongitudeLines+1)); // the index below
		}

		setVertexData(vertexData);
		setIndices(indices);

		loadVerticesToVertexBufferObject();
	}

	void MBmesh::makeCone(double radius, double height, int numberLongitudeLines) {
		if (numberLongitudeLines < 3) {
			std::cerr << "Error: MBmesh::makeCone): Please specify more than 3 corcualr resolution lines." << std::endl;
			return;
		}
		std::vector<VertexData> vertexData((numberLongitudeLines+1)*3);
		double coneAngle = atan2(radius, height);

		for (int i = 0; i <= numberLongitudeLines; i++) {
			// Base:
			vertexData[i].position.x = radius * cos((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i].position.y = radius * sin((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i].position.z = -height;
			vertexData[i].normal.x = 0;
			vertexData[i].normal.y = 0;
			vertexData[i].normal.z = -1;
			vertexData[i].tangent.x = 1;
			vertexData[i].tangent.y = 0;
			vertexData[i].tangent.z = 0;
			vertexData[i].U = cos((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI)*0.5 + 0.5;
			vertexData[i].V = sin((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI)*0.5 + 0.5;

			// Conic Base:
			vertexData[i+numberLongitudeLines+1].position.x = radius * cos((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i+numberLongitudeLines+1].position.y = radius * sin((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i+numberLongitudeLines+1].position.z = -height;
			vertexData[i+numberLongitudeLines+1].normal.x = cos(coneAngle) * cos((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i+numberLongitudeLines+1].normal.y = cos(coneAngle) * sin((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i+numberLongitudeLines+1].normal.z = sin(coneAngle);
			vertexData[i+numberLongitudeLines+1].tangent.x = -sin((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i+numberLongitudeLines+1].tangent.y = cos((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i+numberLongitudeLines+1].tangent.z = 0;
			vertexData[i+numberLongitudeLines+1].U = cos((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI)*0.5 + 0.5;
			vertexData[i+numberLongitudeLines+1].V = sin((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI)*0.5 + 0.5;

			// Conic Top:
			vertexData[i+(numberLongitudeLines+1)*2].position.x = 0;
			vertexData[i+(numberLongitudeLines+1)*2].position.y = 0;
			vertexData[i+(numberLongitudeLines+1)*2].position.z = 0;
			vertexData[i+(numberLongitudeLines+1)*2].normal.x = cos(coneAngle) * cos((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i+(numberLongitudeLines+1)*2].normal.y = cos(coneAngle) * sin((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i+(numberLongitudeLines+1)*2].normal.z = sin(coneAngle);
			vertexData[i+(numberLongitudeLines+1)*2].tangent.x = -sin((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i+(numberLongitudeLines+1)*2].tangent.y = cos((double)i/(double)numberLongitudeLines * 2.0*MOGI_PI);
			vertexData[i+(numberLongitudeLines+1)*2].tangent.z = 0;
			vertexData[i+(numberLongitudeLines+1)*2].U = 0.5;
			vertexData[i+(numberLongitudeLines+1)*2].V = 0.5;

		}
		setVertexData(vertexData);


		std::vector<unsigned int> indices;//(3*(numberLongitudeLines ));
		for (int i = 0; i < numberLongitudeLines; i++) {
			// cone:
			indices.push_back(i + numberLongitudeLines+1);
			indices.push_back(i+1 + numberLongitudeLines+1);
			indices.push_back(i + (numberLongitudeLines+1)*2);

			// base:
			indices.push_back(0);
			indices.push_back(i+1);
			indices.push_back(i );

		}
		setIndices(indices);

		loadVerticesToVertexBufferObject();
	}

	void MBmesh::makeBox(double width, double height, double depth) {
		std::vector<VertexData> vertexData(24);

		for (int y = 0; y < 2; y++) {
			for (int x = 0; x < 2; x++) {
				//Front:
				vertexData[x+2*y].position.x = -width/2 + (double)x*width;
				vertexData[x+2*y].position.y = -depth/2;
				vertexData[x+2*y].position.z = -height/2 + (double)y*height;
				vertexData[x+2*y].U = x;
				vertexData[x+2*y].V = y;
				vertexData[x+2*y].normal.x = 0;
				vertexData[x+2*y].normal.y = -1;
				vertexData[x+2*y].normal.z = 0;
				vertexData[x+2*y].tangent.x = 1;
				vertexData[x+2*y].tangent.y = 0;
				vertexData[x+2*y].tangent.z = 0;

				//Back:
				vertexData[x+2*y + 4].position.x = width/2 - (double)x*width;
				vertexData[x+2*y + 4].position.y = depth/2;
				vertexData[x+2*y + 4].position.z = -height/2 + (double)y*height;
				vertexData[x+2*y + 4].U = x;
				vertexData[x+2*y + 4].V = y;
				vertexData[x+2*y + 4].normal.x = 0;
				vertexData[x+2*y + 4].normal.y = 1;
				vertexData[x+2*y + 4].normal.z = 0;
				vertexData[x+2*y + 4].tangent.x = -1;
				vertexData[x+2*y + 4].tangent.y = 0;
				vertexData[x+2*y + 4].tangent.z = 0;

				//Left:
				vertexData[x+2*y + 8].position.x = -width/2;
				vertexData[x+2*y + 8].position.y = depth/2 - (double)x*depth;
				vertexData[x+2*y + 8].position.z = -height/2 + (double)y*height;
				vertexData[x+2*y + 8].U = x;
				vertexData[x+2*y + 8].V = y;
				vertexData[x+2*y + 8].normal.x = -1;
				vertexData[x+2*y + 8].normal.y = 0;
				vertexData[x+2*y + 8].normal.z = 0;
				vertexData[x+2*y + 8].tangent.x = 0;
				vertexData[x+2*y + 8].tangent.y = -1;
				vertexData[x+2*y + 8].tangent.z = 0;

				//Right:
				vertexData[x+2*y + 12].position.x = width/2;
				vertexData[x+2*y + 12].position.y = -depth/2 + (double)x*depth;
				vertexData[x+2*y + 12].position.z = -height/2 + (double)y*height;
				vertexData[x+2*y + 12].U = x;
				vertexData[x+2*y + 12].V = y;
				vertexData[x+2*y + 12].normal.x = 1;
				vertexData[x+2*y + 12].normal.y = 0;
				vertexData[x+2*y + 12].normal.z = 0;
				vertexData[x+2*y + 12].tangent.x = 0;
				vertexData[x+2*y + 12].tangent.y = 1;
				vertexData[x+2*y + 12].tangent.z = 0;

				//Top:
				vertexData[x+2*y + 16].position.x = -width/2 + (double)x*width;
				vertexData[x+2*y + 16].position.y = -depth/2 + (double)y*depth;
				vertexData[x+2*y + 16].position.z = height/2;
				vertexData[x+2*y + 16].U = x;
				vertexData[x+2*y + 16].V = y;
				vertexData[x+2*y + 16].normal.x = 0;
				vertexData[x+2*y + 16].normal.y = 0;
				vertexData[x+2*y + 16].normal.z = 1;
				vertexData[x+2*y + 16].tangent.x = 1;
				vertexData[x+2*y + 16].tangent.y = 0;
				vertexData[x+2*y + 16].tangent.z = 0;

				//Bottom:
				vertexData[x+2*y + 20].position.x = width/2 - (double)x*width;
				vertexData[x+2*y + 20].position.y = -depth/2 + (double)y*depth;
				vertexData[x+2*y + 20].position.z = -height/2;
				vertexData[x+2*y + 20].U = x;
				vertexData[x+2*y + 20].V = y;
				vertexData[x+2*y + 20].normal.x = 0;
				vertexData[x+2*y + 20].normal.y = 0;
				vertexData[x+2*y + 20].normal.z = -1;
				vertexData[x+2*y + 20].tangent.x = -1;
				vertexData[x+2*y + 20].tangent.y = 0;
				vertexData[x+2*y + 20].tangent.z = 0;
			}
		}
		setVertexData(vertexData);


		std::vector<unsigned int> indices;
		for (int i = 0; i < 6; i++) {	// for all 6 faces
			//triangle 1:
			indices.push_back(0 + i*4);
			indices.push_back(1 + i*4);
			indices.push_back(2 + i*4);

			//triangle 2:
			indices.push_back(3 + i*4);
			indices.push_back(2 + i*4);
			indices.push_back(1 + i*4);
		}

		setIndices(indices);

		loadVerticesToVertexBufferObject();
	}

	Vector3d Simulation::RGBfromHSV(float H, float S, float V) {
		float C = V*S;
		float X = C*(1 - fabs(fmodf(H*6, 2) - 1));
		float m = V - C;

		Vector3d result;
		if (//0.0/6.0 <= H &&
			H < 1.0/6.0) {
			result.r = C;
			result.g = X;
			result.b = 0;
		} else if (1.0/6.0 <= H && H < 2.0/6.0) {
			result.r = X;
			result.g = C;
			result.b = 0;
		} else if (2.0/6.0 <= H && H < 3.0/6.0) {
			result.r = 0;
			result.g = C;
			result.b = X;
		} else if (3.0/6.0 <= H && H < 4.0/6.0) {
			result.r = 0;
			result.g = X;
			result.b = C;
		} else if (4.0/6.0 <= H && H < 5.0/6.0) {
			result.r = X;
			result.g = 0;
			result.b = C;
		} else if (5.0/6.0 <= H) {// && H < 6.0/6.0) {
			result.r = C;
			result.g = 0;
			result.b = X;
		}
		result.r += m;
		result.g += m;
		result.b += m;

		return result;
	}

	Vector3d Simulation::HSVfromRGB(float R, float G, float B) {
		float Cmax = max(max(R, G), B);
		float Cmin = min(min(R, G), B);
		float delta = Cmax - Cmin;
		Vector3d result;

		if (delta == 0) {
			result.h = 0;
		} else if(Cmax == R) {
			result.h = fmodf((G-B)/delta, 6) / 6.0;
		} else if(Cmax == G) {
			result.h = ((B-R)/delta + 2) / 6.0;
		} else { //if(Cmax == B) {
			result.h = ((R-G)/delta + 4) / 6.0;
		}

		if (Cmax == 0) {
			result.s = 0;
		} else {
			result.s = delta/Cmax;
		}

		result.v = Cmax;

		return result;
	}

#ifdef _cplusplus
}
#endif
