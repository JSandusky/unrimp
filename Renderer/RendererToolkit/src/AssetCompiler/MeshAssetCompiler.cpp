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
#include "RendererToolkit/AssetCompiler/MeshAssetCompiler.h"

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#include <assimp/scene.h>
	#include <assimp/Importer.hpp>
	#include <assimp/postprocess.h>
#pragma warning(pop)

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4668)	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#pragma warning(disable: 4201)	// warning C4201: nonstandard extension used: nameless struct/union
	#include <glm/glm.hpp>
	#include <glm/gtc/quaternion.hpp>
#pragma warning(pop)

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4127)	// warning C4127: conditional expression is constant
	#pragma warning(disable: 4244)	// warning C4244: 'argument': conversion from '<x>' to '<y>', possible loss of data
	#pragma warning(disable: 4266)	// warning C4266: '<x>': no override available for virtual member function from base '<y>'; function is hidden
	#pragma warning(disable: 4365)	// warning C4365: 'return': conversion from '<x>' to '<y>', signed/unsigned mismatch
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#pragma warning(disable: 4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	#pragma warning(disable: 4619)	// warning C4619: #pragma warning: there is no warning number '<x>'
	#pragma warning(disable: 4668)	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <Poco/JSON/Parser.h>
#pragma warning(pop)

#include <memory>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MeshAssetCompiler::MeshAssetCompiler()
	{
	}

	MeshAssetCompiler::~MeshAssetCompiler()
	{
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	bool MeshAssetCompiler::compile(std::istream& istream, std::ostream& ostream, std::istream& jsonConfiguration)
	{
		// Read configuration
		// TODO(co) Add required properties
		uint32_t test = 0;
		{
			// Parse JSON
			Poco::JSON::Parser jsonParser;
			jsonParser.parse(jsonConfiguration);
			Poco::JSON::Object::Ptr jsonRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();
		
			{ // Check whether or not the configuration format matches
				Poco::JSON::Object::Ptr jsonFormatObject = jsonRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
				if (jsonFormatObject->get("Type").convert<std::string>() != "MeshCompiler")
				{
					throw std::exception("Invalid JSON format type, must be \"MeshCompiler\"");
				}
				if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
				{
					throw std::exception("Invalid JSON format version, must be 1");
				}
			}

			// Read configuration
			Poco::JSON::Object::Ptr jsonConfigurationObject = jsonRootObject->get("Configuration").extract<Poco::JSON::Object::Ptr>();
			test = jsonConfigurationObject->optValue<uint32_t>("Test", test);
		}

		// Get file size and file data
		istream.seekg(0, std::istream::end);
		const size_t numberOfBytes = static_cast<size_t>(istream.tellg());
		istream.seekg(0, std::istream::beg);
		std::unique_ptr<uint8_t[]> buffer = std::unique_ptr<uint8_t[]>(new uint8_t[numberOfBytes]);
		istream.read((char*)buffer.get(), numberOfBytes);

		// Create an instance of the Assimp importer class
		Assimp::Importer assimpImporter;

		// Load the given mesh
		// -> "aiProcess_MakeLeftHanded" is added because the rasterizer states directly map to Direct3D
		const aiScene *assimpScene = assimpImporter.ReadFileFromMemory(buffer.get(), numberOfBytes, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_MakeLeftHanded);
		if (nullptr != assimpScene && nullptr != assimpScene->mRootNode)
		{
			// Get the total number of vertices and indices by using the Assimp root node
			uint32_t numberOfVertices = 0;
			uint32_t numberOfIndices = 0;
			getNumberOfVerticesAndIndicesRecursive(*assimpScene, *assimpScene->mRootNode, numberOfVertices, numberOfIndices);

			// TODO(co) Will change when skinned
			uint8_t numberOfVertexArrayAttributes = 3;

			{ // Mesh header
				// Fill the mesh header
				// -> Mesh file format content:
				//    - Mesh header
				//    - Vertex and index buffer data
				//    - Vertex array attribute definitions
				struct MeshHeader
				{
					uint32_t formatType;
					uint16_t formatVersion;
					uint8_t  numberOfBytesPerVertex;
					uint32_t numberOfVertices;
					uint8_t  indexBufferFormat;
					uint32_t numberOfIndices;
					uint8_t  numberOfVertexArrayAttributes;
				};
				MeshHeader meshHeader;
				meshHeader.formatType					 = 2;	// TODO(co) String hash of "Mesh"
				meshHeader.formatVersion				 = 1;
				meshHeader.numberOfBytesPerVertex		 = NUMBER_OF_BYTES_PER_VERTEX;
				meshHeader.numberOfVertices				 = numberOfVertices;
				meshHeader.indexBufferFormat			 = Renderer::IndexBufferFormat::UNSIGNED_SHORT;
				meshHeader.numberOfIndices				 = numberOfIndices;
				meshHeader.numberOfVertexArrayAttributes = numberOfVertexArrayAttributes;

				// Write down the mesh header
				ostream.write(reinterpret_cast<const char*>(&meshHeader), sizeof(MeshHeader));
			}

			{ // Vertex and index buffer data
				// Allocate memory for the local vertex and index buffer data
				uint8_t *vertexBufferData = new uint8_t[NUMBER_OF_BYTES_PER_VERTEX * numberOfVertices];
				uint16_t *indexBufferData = new uint16_t[numberOfIndices];

				{ // Fill the mesh data recursively
					uint32_t numberOfFilledVertices = 0;
					uint32_t numberOfFilledIndices  = 0;
					fillMeshRecursive(*assimpScene, *assimpScene->mRootNode, vertexBufferData, indexBufferData, aiMatrix4x4(), numberOfFilledVertices, numberOfFilledIndices);

					// TODO(co) ?
					numberOfVertices = numberOfFilledVertices;
					numberOfIndices  = numberOfFilledIndices;
				}

				// Write down the vertex and index buffer
				ostream.write(reinterpret_cast<const char*>(vertexBufferData), NUMBER_OF_BYTES_PER_VERTEX * numberOfVertices);
				ostream.write(reinterpret_cast<const char*>(indexBufferData), sizeof(uint16_t) * numberOfIndices);

				// Destroy local vertex and input buffer data
				delete [] vertexBufferData;
				delete [] indexBufferData;
			}

			{ // Vertex array attributes
				// TODO(co) If we would use a "IVertexBuffer"-handle instead of pointer inside "Renderer::VertexArrayAttribute", we could just dump the stuff and still be 32/64 bit compatible
				struct VertexArrayAttribute
				{
					uint8_t vertexArrayFormat;
					char	name[32];
					char	semantic[32];
					uint8_t semanticIndex;
					uint8_t offset;
					uint8_t stride;
				};

				const VertexArrayAttribute vertexArray[] =
				{
					{ // Attribute 0
						// Data destination
						Renderer::VertexArrayFormat::FLOAT_3,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
						"Position",								// name[32] (char)
						"POSITION",								// semantic[32] (char)
						0,										// semanticIndex (uint32_t)
						// Data source
						0,										// offset (uint32_t)
						NUMBER_OF_BYTES_PER_VERTEX				// stride (uint32_t)
					},
					{ // Attribute 1
						// Data destination
						Renderer::VertexArrayFormat::SHORT_2,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
						"TexCoord",								// name[32] (char)
						"TEXCOORD",								// semantic[32] (char)
						0,										// semanticIndex (uint32_t)
						// Data source
						sizeof(float) * 3,						// offset (uint32_t)
						NUMBER_OF_BYTES_PER_VERTEX				// stride (uint32_t)
					},
					{ // Attribute 2
						// Data destination
						Renderer::VertexArrayFormat::SHORT_4,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
						"QTangent",								// name[32] (char)
						"NORMAL",								// semantic[32] (char)
						0,										// semanticIndex (uint32_t)
						// Data source
						sizeof(float) * 3 + sizeof(short) * 2,	// offset (uint32_t)
						NUMBER_OF_BYTES_PER_VERTEX				// stride (uint32_t)
					}
				};

				// Write down the vertex array attributes
				ostream.write(reinterpret_cast<const char*>(vertexArray), sizeof(VertexArrayAttribute) * numberOfVertexArrayAttributes);
			}
		}
		else
		{
			throw std::exception("ASSIMP failed to load in the given mesh");
		}

		// Done
		return true;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void MeshAssetCompiler::getNumberOfVerticesAndIndicesRecursive(const aiScene &assimpScene, const aiNode &assimpNode, uint32_t &numberOfVertices, uint32_t &numberOfIndices)
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

	void MeshAssetCompiler::fillMeshRecursive(const aiScene &assimpScene, const aiNode &assimpNode, uint8_t *vertexBuffer, uint16_t *indexBuffer, const aiMatrix4x4 &assimpTransformation, uint32_t &numberOfVertices, uint32_t &numberOfIndices)
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
					//					  Assimp mesh vertex index	 								 Where the Assimp mesh starts within the our vertex buffer
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
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
