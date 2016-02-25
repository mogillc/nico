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

#ifndef MOGI_MESH_H
#define MOGI_MESH_H

#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <string>

#include "mogi/math/mmath.h"

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#include <string.h>
#endif

#include <fstream>
#include <iostream>
#include <vector>

#include "bone.h"
#include "camera.h"
#include "material.h"
#include "mogi/math/node.h"
#include "shader.h"
#include "texture.h"

namespace Mogi {
namespace Simulation {

struct Vector3d {
	float x;
	float y;
	float z;
};

struct VertexData {
	Vector3d position;
	Vector3d normal;
	Vector3d tangent;
	Vector3d color;
	float U, V;
};

/*!
 @class MBmesh
 This class handles vertices and textures for use with OpenGL rendering.
 Vertex objects contain the information and handling needed to render obejcts in
 OpenGL.
 @since 2014-04-17
 */
class MBmesh {
private:
	GLuint vertexBufferObject;
	GLuint indexBufferObject;

	std::vector<VertexData> data;
	std::vector<unsigned int> indices;

	std::vector<Bone*> bones;  // Loaded from mBones

	unsigned int materialIndex;

	std::string objectLocation;

public:
	std::string name;
	std::string fileName;

	void setObjectLocation(const std::string& path);
	void setBones( const std::vector<Bone*>& bones);
	void setMaterialIndex( const int& index );

	void setIndices( const std::vector<unsigned int>& indices );
	void setVertexData( const std::vector<VertexData>& vertexData );
	std::vector<unsigned int>& getIndices( ) { return indices; }
	std::vector<VertexData>& getVertexData( ) { return data; }

	void matchBonesToNodes(Math::Node* rootNode);

	int getNumberOfVertices();

	int getMaterialIndex() { return materialIndex; };

	/*! \brief This sets the color of all vertices in the mesh.
	 
	 This will only be viewable if there is no color map in the material.
	 Once the data is modified, it must be loaded to the vertex buffer obeject
	 by calling loadVerticesToVertexBufferObject().
	 \see loadVerticesToVertexBufferObject
	 \param red The red color, value range: 0 to 1
	 \param green The green color, value range: 0 to 1
	 \param blue The blue color, value range: 0 to 1
	 */
	void setColor(float red, float green, float blue);

	/*! \brief Sends the data to the vertex buffer object.
	 */
	void loadVerticesToVertexBufferObject();

	// Methods:
	int draw(MBshader* shader);
	int drawWithMaterials(MBshader* shader,
			std::vector<MBmaterial*>& materials);

	MBmesh();
	~MBmesh();

	// This generates a function that will initialize a mesh in the same way as this mesh.
	// This is useful when needing to load a same file many times, or when the target
	// platform has no easy way to load files.
	// This was used to generate code for the post processing mesh in Postprocess.cpp.
	void generateCodeFromMesh();

};
}
}
#endif
