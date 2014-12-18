/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __ASSIMPMESH_MESH_H__
#define __ASSIMPMESH_MESH_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/Renderer.h>

#include <assimp/vector3.h>	// Required by "assimp/matrix4x4.h" below
#include <assimp/matrix4x4.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct aiNode;
struct aiScene;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Mesh class
*/
class Mesh
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] program
	*    Program instance to use
	*  @param[in] filename
	*    ASCII filename of the mesh to load in, in case of a null pointer nothing is loaded
	*/
	Mesh(Renderer::IProgram &program, const char *filename);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~Mesh();

	/**
	*  @brief
	*    Draw the mesh
	*/
	void draw();


//[-------------------------------------------------------]
//[ Private definitions                                   ]
//[-------------------------------------------------------]
private:
	static const unsigned int NUMBER_OF_COMPONENTS_PER_VERTEX = 14;	///< Number of components per vertex (3 position, 2 texture coordinate, 3 tangent, 3 binormal, 3 normal)


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
private:
	/**
	*  @brief
	*    Get the total number of vertices and indices by using a given Assimp node
	*
	*  @param[in]  assimpScene
	*    Assimp scene
	*  @param[in]  assimpNode
	*    Assimp node to gather the data from
	*  @param[out] numberOfVertices
	*    Receives the number of vertices
	*  @param[out] numberOfIndices
	*    Receives the number of indices
	*/
	void getNumberOfVerticesAndIndicesRecursive(const aiScene &assimpScene, const aiNode &assimpNode, unsigned int &numberOfVertices, unsigned int &numberOfIndices);

	/**
	*  @brief
	*    Fill the mesh data recursively
	*
	*  @param[in]  assimpScene
	*    Assimp scene
	*  @param[in]  assimpNode
	*    Assimp node to gather the data from
	*  @param[in]  vertexBuffer
	*    Vertex buffer to fill
	*  @param[in]  indexBuffer
	*    Index buffer to fill
	*  @param[in]  assimpTransformation
	*    Current absolute Assimp transformation matrix (local to global space)
	*  @param[out] numberOfVertices
	*    Receives the number of processed vertices
	*  @param[out] numberOfIndices
	*    Receives the number of processed indices
	*/
	void fillMeshRecursive(const aiScene &assimpScene, const aiNode &assimpNode, float *vertexBuffer, unsigned short *indexBuffer, const aiMatrix4x4 &assimpTransformation, unsigned int &numberOfVertices, unsigned int &numberOfIndices);


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	unsigned int			  mNumberOfVertices;	///< Number of vertices
	unsigned int			  mNumberOfIndices;		///< Number of indices
	Renderer::IVertexArrayPtr mVertexArray;			///< Vertex array object (VAO), can be a null pointer


};


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __ASSIMPMESH_MESH_H__
