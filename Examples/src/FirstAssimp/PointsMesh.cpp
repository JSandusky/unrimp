/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#ifndef NO_ASSIMP


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "FirstAssimp/PointsMesh.h"
#include "Framework/PlatformTypes.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
PointsMesh::PointsMesh(Renderer::IProgram &program, const char *filename) :
	mNumberOfVertices(0)
{
	// Create an instance of the Assimp importer class
	Assimp::Importer assimpImporter;

	// Load the given mesh
	const aiScene *assimpScene = (nullptr != filename) ? assimpImporter.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality) : nullptr;
	if (nullptr != assimpScene && nullptr != assimpScene->mRootNode)
	{
		// Get the total number of vertices by using the Assimp root node
		getNumberOfVerticesRecursive(*assimpScene, *assimpScene->mRootNode, mNumberOfVertices);

		// Allocate memory for the local vertex buffer data
		float *vertexBuffer = new float[mNumberOfVertices * 3];

		{ // Fill the mesh data recursively
			unsigned int numberOfFilledVertices = 0;
			fillMeshRecursive(*assimpScene, *assimpScene->mRootNode, vertexBuffer, aiMatrix4x4(), numberOfFilledVertices);
		}

		{ // Create vertex array object (VAO)
			// Create the vertex buffer object (VBO)
			Renderer::IVertexBufferPtr vertexBuffer(program.getRenderer().createVertexBuffer(sizeof(float) * 3 * mNumberOfVertices, vertexBuffer, Renderer::BufferUsage::STATIC_DRAW));

			// Create vertex array object (VAO)
			// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
			// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
			// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
			//    reference of the used vertex buffer objects (VBO). If the reference counter of a
			//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
			const Renderer::VertexArrayAttribute vertexArray[] =
			{
				{ // Attribute 0
					// Data destination
					Renderer::VertexArrayFormat::FLOAT_3,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"Position",								// nName[64] (char)
					"POSITION",								// semantic[64] (char)
					0,										// semanticIndex (unsigned int)
					// Data source
					vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
					0,										// offset (unsigned int)
					sizeof(float) * 3,						// stride (unsigned int)
					// Data source, instancing part
					0										// instancesPerElement (unsigned int)
				}
			};
			mVertexArray = program.createVertexArray(sizeof(vertexArray) / sizeof(Renderer::VertexArrayAttribute), vertexArray);
		}

		// Destroy local vertex and input buffer data
		delete [] vertexBuffer;
	}
	else
	{
		OUTPUT_DEBUG_PRINTF("Failed to load in \"%s\": %s", filename, assimpImporter.GetErrorString())
	}
}

/**
*  @brief
*    Destructor
*/
PointsMesh::~PointsMesh()
{
	// The renderer resource pointers are released automatically
	// Nothing to do in here
}

/**
*  @brief
*    Draw the mesh
*/
void PointsMesh::draw()
{
	// Valid mesh?
	if (nullptr != mVertexArray)
	{
		// Get the used renderer instance
		Renderer::IRenderer &renderer = mVertexArray->getRenderer();

		{ // Setup input assembly (IA)
			// Set the used vertex array
			renderer.iaSetVertexArray(mVertexArray);

			// Set the primitive topology used for draw calls
			renderer.iaSetPrimitiveTopology(Renderer::PrimitiveTopology::POINT_LIST);
		}

		// Render the specified geometric primitive, based on an array of vertices
		renderer.draw(0, mNumberOfVertices);
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
/**
*  @brief
*    Get the total number of vertices by using a given Assimp node
*/
void PointsMesh::getNumberOfVerticesRecursive(const aiScene &assimpScene, const aiNode &assimpNode, unsigned int &numberOfVertices)
{
	// Loop through all meshes this node is using
	for (unsigned int i = 0; i < assimpNode.mNumMeshes; ++i)
	{
		// Get the used mesh
		const aiMesh &assimpMesh = *assimpScene.mMeshes[assimpNode.mMeshes[i]];

		// Update the number of vertices
		numberOfVertices += assimpMesh.mNumVertices;
	}

	// Loop through all child nodes recursively
	for (unsigned int j = 0; j < assimpNode.mNumChildren; ++j)
	{
		getNumberOfVerticesRecursive(assimpScene, *assimpNode.mChildren[j], numberOfVertices);
	}
}

/**
*  @brief
*    Fill the mesh data recursively
*/
void PointsMesh::fillMeshRecursive(const aiScene &assimpScene, const aiNode &assimpNode, float *vertexBuffer, const aiMatrix4x4 &assimpTransformation, unsigned int &numberOfVertices)
{
	// Get the absolute transformation matrix of this Assimp node
	const aiMatrix4x4 currentAssimpTransformation = assimpTransformation * assimpNode.mTransformation;

	// Loop through all meshes this node is using
	for (unsigned int i = 0; i < assimpNode.mNumMeshes; ++i)
	{
		// Get the used mesh
		const aiMesh &assimpMesh = *assimpScene.mMeshes[assimpNode.mMeshes[i]];

		// Loop through the Assimp mesh vertices
		float *currentVertexBuffer = vertexBuffer + numberOfVertices * 3;
		for (unsigned int j = 0; j < assimpMesh.mNumVertices; ++j)
		{
			// Get the Assimp mesh vertex position
			aiVector3D assimpVertex = assimpMesh.mVertices[j];

			// Transform the Assimp mesh vertex position into global space
			assimpVertex *= currentAssimpTransformation;

			// Set our vertex buffer position
			*currentVertexBuffer = assimpVertex.x;
			++currentVertexBuffer;
			*currentVertexBuffer = assimpVertex.y;
			++currentVertexBuffer;
			*currentVertexBuffer = assimpVertex.z;
			++currentVertexBuffer;
		}
		numberOfVertices += assimpMesh.mNumVertices;
	}

	// Loop through all child nodes recursively
	for (unsigned int i = 0; i < assimpNode.mNumChildren; ++i)
	{
		fillMeshRecursive(assimpScene, *assimpNode.mChildren[i], vertexBuffer, currentAssimpTransformation, numberOfVertices);
	}
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // NO_ASSIMP
