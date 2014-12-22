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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PrecompiledHeader.h"


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#ifndef NO_ASSIMP


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Assimp/AssimpMesh/Mesh.h"
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
			// -> In a real-world use-case you might want to use vertex-packing (QTangents, half precision for positions, short for texture coordinates etc, )
			//    instead of a fat vertex layout. So here's an example.

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
					Renderer::VertexArrayFormat::FLOAT_4,				// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"QTangent",											// name[64] (char)
					"NORMAL",											// semantic[64] (char)
					0,													// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,										// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 5,									// offset (uint32_t)
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

			{ // QTangent
			  // - QTangent basing on http://dev.theomader.com/qtangents/ "QTangents" which is basing on
			  //   http://www.crytek.com/cryengine/presentations/spherical-skinning-with-dual-quaternions-and-qtangents "Spherical Skinning with Dual-Quaternions and QTangents"
				// Get the Assimp mesh vertex tangent, binormal and normal
				aiVector3D tangent = assimpMesh.mTangents[j];
				aiVector3D binormal = assimpMesh.mBitangents[j];
				aiVector3D normal = assimpMesh.mNormals[j];

				// Transform the Assimp mesh vertex data into global space
				tangent *= currentAssimpNormalTransformation;
				binormal *= currentAssimpNormalTransformation;
				normal *= currentAssimpNormalTransformation;

				// Generate tangent frame rotation matrix
				glm::mat3 tangentFrame(
					tangent.x,  tangent.y,  tangent.z,
					binormal.x, binormal.y, binormal.z,
					normal.x,   normal.y,   normal.z
				);

				// Flip y axis in case the tangent frame encodes a reflection
				const float scale = (glm::determinant(tangentFrame) > 0) ? 1.0f : -1.0f;
				tangentFrame[2][0] *= scale;
				tangentFrame[2][1] *= scale;
				tangentFrame[2][2] *= scale;

				glm::quat tangentFrameQuaternion(tangentFrame);

				{ // Make sure we don't end up with 0 as w component
					const float threshold = 0.00001f;
					const float renomalization = sqrt(1.0f - threshold * threshold);

					if (abs(tangentFrameQuaternion.w) < threshold)
					{
						tangentFrameQuaternion.x *= renomalization;
						tangentFrameQuaternion.y *= renomalization;
						tangentFrameQuaternion.z *= renomalization;
						tangentFrameQuaternion.w =  (tangentFrameQuaternion.w > 0) ? threshold : -threshold;
					}
				}

				{ // Encode reflection into quaternion's w element by making sign of w negative if y axis needs to be flipped, positive otherwise
					const float qs = (scale < 0.0f && tangentFrameQuaternion.w > 0.0f) || (scale > 0.0f && tangentFrameQuaternion.w < 0.0f) ? -1.0f : 1.0f;
					tangentFrameQuaternion.x *= qs;
					tangentFrameQuaternion.y *= qs;
					tangentFrameQuaternion.z *= qs;
					tangentFrameQuaternion.w *= qs;
				}

				// Set our vertex buffer qtangent
				*currentVertexBuffer = tangentFrameQuaternion.x;
				++currentVertexBuffer;
				*currentVertexBuffer = tangentFrameQuaternion.y;
				++currentVertexBuffer;
				*currentVertexBuffer = tangentFrameQuaternion.z;
				++currentVertexBuffer;
				*currentVertexBuffer = tangentFrameQuaternion.w;
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
