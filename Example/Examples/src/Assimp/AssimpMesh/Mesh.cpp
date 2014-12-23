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
		uint8_t *vertexBufferData = new uint8_t[mNumberOfVertices * NUMBER_OF_BYTES_PER_VERTEX];
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
			Renderer::IVertexBufferPtr vertexBuffer(renderer.createVertexBuffer(NUMBER_OF_BYTES_PER_VERTEX * mNumberOfVertices, vertexBufferData, Renderer::BufferUsage::STATIC_DRAW));

			// Create the index buffer object (IBO)
			Renderer::IIndexBuffer *indexBuffer = renderer.createIndexBuffer(sizeof(uint16_t) * mNumberOfIndices, Renderer::IndexBufferFormat::UNSIGNED_SHORT, indexBufferData, Renderer::BufferUsage::STATIC_DRAW);

			// Compact vertex format:
			// - Position: We could use 16 bit positions, but then would have to make a distinction between small meshes were this is sufficient
			//   and bigger meshes were this will introduce artifacts. We want to keep it simple, so we don't go there.
			// - Texture coordinate: 16 bit unsigned normalized texture coordinates are usualy sufficient
			// - Tangent space: Please note: Storing fully featured normal, tangent and binormal is inefficient
			//   -> Normal vectors are considered to be normalized, so, we don't need to store all three components as fully featured float
			//   -> The binormal can be recalculated within a shader
			//   -> In a real-world use-case you might want to use vertex-packing (QTangents, half precision for positions, short for texture coordinates etc, )
			//      instead of a fat vertex layout. So here's an example.
			//   -> 16 bit QTangent is sufficient

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
					"Position",								// name[32] (char)
					"POSITION",								// semantic[32] (char)
					0,										// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
					0,										// offset (uint32_t)
					NUMBER_OF_BYTES_PER_VERTEX,				// stride (uint32_t)
					// Data source, instancing part
					0										// instancesPerElement (uint32_t)
				},
				{ // Attribute 1
					// Data destination
					Renderer::VertexArrayFormat::SHORT_2,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"TexCoord",								// name[32] (char)
					"TEXCOORD",								// semantic[32] (char)
					0,										// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 3,						// offset (uint32_t)
					NUMBER_OF_BYTES_PER_VERTEX,				// stride (uint32_t)
					// Data source, instancing part
					0										// instancesPerElement (uint32_t)
				},
				{ // Attribute 2
					// Data destination
					Renderer::VertexArrayFormat::SHORT_4,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"QTangent",								// name[32] (char)
					"NORMAL",								// semantic[32] (char)
					0,										// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 3 + sizeof(short) * 2,	// offset (uint32_t)
					NUMBER_OF_BYTES_PER_VERTEX,				// stride (uint32_t)
					// Data source, instancing part
					0										// instancesPerElement (uint32_t)
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

void Mesh::fillMeshRecursive(const aiScene &assimpScene, const aiNode &assimpNode, uint8_t *vertexBuffer, uint16_t *indexBuffer, const aiMatrix4x4 &assimpTransformation, uint32_t &numberOfVertices, uint32_t &numberOfIndices)
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
		uint8_t *currentVertexBuffer = vertexBuffer + numberOfVertices * NUMBER_OF_BYTES_PER_VERTEX;
		for (uint32_t j = 0; j < assimpMesh.mNumVertices; ++j)
		{
			{ // 32 bit position
				// Get the Assimp mesh vertex position
				aiVector3D assimpVertex = assimpMesh.mVertices[j];

				// Transform the Assimp mesh vertex position into global space
				assimpVertex *= currentAssimpTransformation;

				// Set our vertex buffer position
				float *currentVertexBufferFloat = reinterpret_cast<float*>(currentVertexBuffer);
				*currentVertexBufferFloat = assimpVertex.x;
				++currentVertexBufferFloat;
				*currentVertexBufferFloat = assimpVertex.y;
				++currentVertexBufferFloat;
				*currentVertexBufferFloat = assimpVertex.z;
				currentVertexBuffer += sizeof(float) * 3;
			}

			{ // 16 bit texture coordinate
				// Get the Assimp mesh vertex texture coordinate
				aiVector3D assimpTexCoord = assimpMesh.mTextureCoords[0][j];

				// Set our vertex buffer 16 bit texture coordinate
				short *currentVertexBufferShort = reinterpret_cast<short*>(currentVertexBuffer);
				*currentVertexBufferShort = static_cast<short>(assimpTexCoord.x * SHRT_MAX);
				++currentVertexBufferShort;
				*currentVertexBufferShort = static_cast<short>(assimpTexCoord.y * SHRT_MAX);
				currentVertexBuffer += sizeof(short) * 2;
			}

			{ // 16 bit QTangent
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
					const float threshold = 1.0f / SHRT_MAX; // 16 bit quantization QTangent
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

				// Set our vertex buffer 16 bit qtangent
				short *currentVertexBufferShort = reinterpret_cast<short*>(currentVertexBuffer);
				*currentVertexBufferShort = static_cast<short>(tangentFrameQuaternion.x * SHRT_MAX);
				++currentVertexBufferShort;
				*currentVertexBufferShort = static_cast<short>(tangentFrameQuaternion.y * SHRT_MAX);
				++currentVertexBufferShort;
				*currentVertexBufferShort = static_cast<short>(tangentFrameQuaternion.z * SHRT_MAX);
				++currentVertexBufferShort;
				*currentVertexBufferShort = static_cast<short>(tangentFrameQuaternion.w * SHRT_MAX);
				currentVertexBuffer += sizeof(short) * 4;
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
