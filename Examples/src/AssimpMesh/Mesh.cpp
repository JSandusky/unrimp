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
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#ifndef NO_ASSIMP


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "AssimpMesh/Mesh.h"
#include "Framework/PlatformTypes.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
Mesh::Mesh(Renderer::IProgram &program, const char *filename) :
	mNumberOfVertices(0),
	mNumberOfIndices(0)
{
	// Create an instance of the Assimp importer class
	Assimp::Importer assimpImporter;

	// Load the given mesh
	// -> "aiProcess_MakeLeftHanded" is added because the rasterizer states directly map to Direct3D
	const aiScene *assimpScene = (nullptr != filename) ? assimpImporter.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_MakeLeftHanded) : nullptr;
	if (nullptr != assimpScene && nullptr != assimpScene->mRootNode)
	{
		// Get the total number of vertices and indices by using the Assimp root node
		getNumberOfVerticesAndIndicesRecursive(*assimpScene, *assimpScene->mRootNode, mNumberOfVertices, mNumberOfIndices);

		// Allocate memory for the local vertex and index buffer data
		float *vertexBufferData = new float[mNumberOfVertices * NUMBER_OF_COMPONENTS_PER_VERTEX];
		uint16_t *indexBufferData = new uint16_t[mNumberOfIndices];

		{ // Fill the mesh data recursively
			uint32_t numberOfFilledVertices = 0;
			uint32_t numberOfFilledIndices  = 0;
			fillMeshRecursive(*assimpScene, *assimpScene->mRootNode, vertexBufferData, indexBufferData, aiMatrix4x4(), numberOfFilledVertices, numberOfFilledIndices);
			mNumberOfVertices = numberOfFilledVertices;
			mNumberOfIndices  = numberOfFilledIndices;
		}

		// Get the used renderer instance
		Renderer::IRenderer &renderer = program.getRenderer();

		{ // Create vertex array object (VAO)
			// Create the vertex buffer object (VBO)
			Renderer::IVertexBufferPtr vertexBuffer(renderer.createVertexBuffer(sizeof(float) * NUMBER_OF_COMPONENTS_PER_VERTEX * mNumberOfVertices, vertexBufferData, Renderer::BufferUsage::STATIC_DRAW));

			// Create the index buffer object (IBO)
			Renderer::IIndexBuffer *indexBuffer = renderer.createIndexBuffer(sizeof(uint16_t) * mNumberOfIndices, Renderer::IndexBufferFormat::UNSIGNED_SHORT, indexBufferData, Renderer::BufferUsage::STATIC_DRAW);

			// Please note: Storing fully featured normal, tangent and binormal is inefficient
			// -> Normal vectors are considered to be normalized, so, we don't need to store all three components as fully featured float
			// -> The binormal can be recalculated within a shader
			// -> In order to keep this sample simple, we do it the classic way without mentioned optimizations

			// Create vertex array object (VAO)
			// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
			// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
			// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
			//    reference of the used vertex buffer objects (VBO). If the reference counter of a
			//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
			// -> The vertex layout is kept simple for renderer API demonstration purposes. In a real-world use-case you might want to use vertex-packing
			//    (QTangents, half precision for positions, short for texture coordinates etc, ) instead of a fat vertex layout.
			const Renderer::VertexArrayAttribute vertexArray[] =
			{
				{ // Attribute 0
					// Data destination
					Renderer::VertexArrayFormat::FLOAT_3,				// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"Position",											// name[64] (char)
					"POSITION",											// semantic[64] (char)
					0,													// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,										// vertexBuffer (Renderer::IVertexBuffer *)
					0,													// offset (uint32_t)
					sizeof(float) * NUMBER_OF_COMPONENTS_PER_VERTEX,	// stride (uint32_t)
					// Data source, instancing part
					0													// instancesPerElement (uint32_t)
				},
				{ // Attribute 1
					// Data destination
					Renderer::VertexArrayFormat::FLOAT_2,				// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"TexCoord",											// name[64] (char)
					"TEXCOORD",											// semantic[64] (char)
					0,													// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,										// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 3,									// offset (uint32_t)
					sizeof(float) * NUMBER_OF_COMPONENTS_PER_VERTEX,	// stride (uint32_t)
					// Data source, instancing part
					0													// instancesPerElement (uint32_t)
				},
				{ // Attribute 2
					// Data destination
					Renderer::VertexArrayFormat::FLOAT_3,				// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"Tangent",											// name[64] (char)
					"TANGENT",											// semantic[64] (char)
					0,													// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,										// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 5,									// offset (uint32_t)
					sizeof(float) * NUMBER_OF_COMPONENTS_PER_VERTEX,	// stride (uint32_t)
					// Data source, instancing part
					0													// instancesPerElement (uint32_t)
				},
				{ // Attribute 3
					// Data destination
					Renderer::VertexArrayFormat::FLOAT_3,				// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"Binormal",											// name[64] (char)
					"BINORMAL",											// semantic[64] (char)
					0,													// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,										// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 8,									// offset (uint32_t)
					sizeof(float) * NUMBER_OF_COMPONENTS_PER_VERTEX,	// stride (uint32_t)
					// Data source, instancing part
					0													// instancesPerElement (uint32_t)
				},
				{ // Attribute 4
					// Data destination
					Renderer::VertexArrayFormat::FLOAT_3,				// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"Normal",											// name[64] (char)
					"NORMAL",											// semantic[64] (char)
					0,													// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,										// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 11,									// offset (uint32_t)
					sizeof(float) * NUMBER_OF_COMPONENTS_PER_VERTEX,	// stride (uint32_t)
					// Data source, instancing part
					0													// instancesPerElement (uint32_t)
				}
			};
			mVertexArray = program.createVertexArray(sizeof(vertexArray) / sizeof(Renderer::VertexArrayAttribute), vertexArray, indexBuffer);
		}

		// Destroy local vertex and input buffer data
		delete [] vertexBufferData;
		delete [] indexBufferData;
	}
	else
	{
		OUTPUT_DEBUG_PRINTF("Failed to load in \"%s\": %s", filename, assimpImporter.GetErrorString())
	}
}

Mesh::~Mesh()
{
	// The renderer resource pointers are released automatically
	// Nothing to do in here
}

void Mesh::draw()
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
			renderer.iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
		}

		// Render the specified geometric primitive, based on indexing into an array of vertices
		renderer.drawIndexed(0, mNumberOfIndices, 0, 0, mNumberOfVertices);
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
void Mesh::getNumberOfVerticesAndIndicesRecursive(const aiScene &assimpScene, const aiNode &assimpNode, uint32_t &numberOfVertices, uint32_t &numberOfIndices)
{
	// Loop through all meshes this node is using
	for (uint32_t i = 0; i < assimpNode.mNumMeshes; ++i)
	{
		// Get the used mesh
		const aiMesh &assimpMesh = *assimpScene.mMeshes[assimpNode.mMeshes[i]];

		// Update the number of vertices
		numberOfVertices += assimpMesh.mNumVertices;

		// Loop through all mesh faces and update the number of indices
		for (uint32_t j = 0; j < assimpMesh.mNumFaces; ++j)
		{
			numberOfIndices += assimpMesh.mFaces[j].mNumIndices;
		}
	}

	// Loop through all child nodes recursively
	for (uint32_t i = 0; i < assimpNode.mNumChildren; ++i)
	{
		getNumberOfVerticesAndIndicesRecursive(assimpScene, *assimpNode.mChildren[i], numberOfVertices, numberOfIndices);
	}
}

void Mesh::fillMeshRecursive(const aiScene &assimpScene, const aiNode &assimpNode, float *vertexBuffer, uint16_t *indexBuffer, const aiMatrix4x4 &assimpTransformation, uint32_t &numberOfVertices, uint32_t &numberOfIndices)
{
	// Get the absolute transformation matrix of this Assimp node
	const aiMatrix4x4 currentAssimpTransformation = assimpTransformation * assimpNode.mTransformation;
	const aiMatrix3x3 currentAssimpNormalTransformation = aiMatrix3x3(currentAssimpTransformation);

	// Loop through all meshes this node is using
	for (uint32_t i = 0; i < assimpNode.mNumMeshes; ++i)
	{
		// Get the used mesh
		const aiMesh &assimpMesh = *assimpScene.mMeshes[assimpNode.mMeshes[i]];

		// Get the start vertex inside the our vertex buffer
		const uint32_t starVertex = numberOfVertices;

		// Loop through the Assimp mesh vertices
		float *currentVertexBuffer = vertexBuffer + numberOfVertices * NUMBER_OF_COMPONENTS_PER_VERTEX;
		for (uint32_t j = 0; j < assimpMesh.mNumVertices; ++j)
		{
			{ // Position
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

			{ // Texture coordinate
				// Get the Assimp mesh vertex texture coordinate
				aiVector3D assimpTexCoord = assimpMesh.mTextureCoords[0][j];

				// Set our vertex buffer texture coordinate
				*currentVertexBuffer = assimpTexCoord.x;
				++currentVertexBuffer;
				*currentVertexBuffer = assimpTexCoord.y;
				++currentVertexBuffer;
			}

			{ // Tangent
				// Get the Assimp mesh vertex tangent
				aiVector3D assimpTangent = assimpMesh.mTangents[j];

				// Transform the Assimp mesh vertex tangent into global space
				assimpTangent *= currentAssimpNormalTransformation;

				// Set our vertex buffer tangent
				*currentVertexBuffer = assimpTangent.x;
				++currentVertexBuffer;
				*currentVertexBuffer = assimpTangent.y;
				++currentVertexBuffer;
				*currentVertexBuffer = assimpTangent.z;
				++currentVertexBuffer;
			}

			{ // Binormal
				// Get the Assimp mesh vertex binormal
				aiVector3D assimpBinormal = assimpMesh.mBitangents[j];

				// Transform the Assimp mesh vertex binormal into global space
				assimpBinormal *= currentAssimpNormalTransformation;

				// Set our vertex buffer binormal
				*currentVertexBuffer = assimpBinormal.x;
				++currentVertexBuffer;
				*currentVertexBuffer = assimpBinormal.y;
				++currentVertexBuffer;
				*currentVertexBuffer = assimpBinormal.z;
				++currentVertexBuffer;
			}

			{ // Normal
				// Get the Assimp mesh vertex normal
				aiVector3D assimpNormal = assimpMesh.mNormals[j];

				// Transform the Assimp mesh vertex normal into global space
				assimpNormal *= currentAssimpNormalTransformation;

				// Set our vertex buffer normal
				*currentVertexBuffer = assimpNormal.x;
				++currentVertexBuffer;
				*currentVertexBuffer = assimpNormal.y;
				++currentVertexBuffer;
				*currentVertexBuffer = assimpNormal.z;
				++currentVertexBuffer;
			}
		}
		numberOfVertices += assimpMesh.mNumVertices;

		// Loop through all Assimp mesh faces
		uint16_t *currentIndexBuffer = indexBuffer + numberOfIndices;
		for (uint32_t j = 0; j < assimpMesh.mNumFaces; ++j)
		{
			// Get the Assimp face
			const aiFace &assimpFace = assimpMesh.mFaces[j];

			// Loop through all indices of the Assimp face and set our indices
			for (uint32_t assimpIndex = 0; assimpIndex < assimpFace.mNumIndices; ++assimpIndex, ++currentIndexBuffer)
			{
				//					  Assimp mesh vertex inde	 								 Where the Assimp mesh starts within the our vertex buffer
				*currentIndexBuffer = static_cast<uint16_t>(assimpFace.mIndices[assimpIndex] + starVertex);
			}

			// Update the number if processed indices
			numberOfIndices += assimpFace.mNumIndices;
		}
	}

	// Loop through all child nodes recursively
	for (uint32_t assimpChild = 0; assimpChild < assimpNode.mNumChildren; ++assimpChild)
	{
		fillMeshRecursive(assimpScene, *assimpNode.mChildren[assimpChild], vertexBuffer, indexBuffer, currentAssimpTransformation, numberOfVertices, numberOfIndices);
	}
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // NO_ASSIMP
