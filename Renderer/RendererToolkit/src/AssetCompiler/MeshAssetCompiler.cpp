/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Core/Math/Math.h>
#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/Mesh/MeshResource.h>
#include <RendererRuntime/Resource/Mesh/Loader/MeshFileFormat.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4061)	// warning C4061: enumerator 'FORCE_32BIT' in switch of enum 'aiMetadataType' is not explicitly handled by a case label
	#include <assimp/scene.h>
	#include <assimp/Importer.hpp>
	#include <assimp/postprocess.h>
PRAGMA_WARNING_POP

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

#include <memory>
#include <fstream>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const uint8_t NUMBER_OF_BYTES_PER_VERTEX = 28;										///< Number of bytes per vertex (3 float position, 2 float texture coordinate, 4 short QTangent)
		static const uint8_t NUMBER_OF_BYTES_PER_SKINNED_VERTEX = NUMBER_OF_BYTES_PER_VERTEX + 20;	///< Number of bytes per skinned vertex (+4 byte bone indices, +4 float bone weights)
		typedef std::vector<RendererRuntime::v1Mesh::SubMesh> SubMeshes;


		//[-------------------------------------------------------]
		//[ Classes                                               ]
		//[-------------------------------------------------------]
		class Skeleton
		{


		//[-------------------------------------------------------]
		//[ Public data                                           ]
		//[-------------------------------------------------------]
		public:
			uint8_t		 numberOfBones;		///< Number of bones
			// Structure-of-arrays (SoA)
			uint8_t*	 boneParents;		///< Cache friendly depth-first rolled up bone parents, null pointer only in case of horrible error, free the memory if no longer required
			uint32_t*	 boneIds;			///< Cache friendly depth-first rolled up bone IDs ("RendererRuntime::StringId" on bone name), null pointer only in case of horrible error, don't free the memory because it's owned by "boneParents"
			aiMatrix4x4* localBonePoses;	///< Cache friendly depth-first rolled up local bone poses, null pointer only in case of horrible error, don't free the memory because it's owned by "boneParents"
			aiMatrix4x4* boneOffsetMatrix;	///< Cache friendly depth-first rolled up bone offset matrix (object space to bone space), null pointer only in case of horrible error, free the memory if no longer required


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit Skeleton(uint8_t _numberOfBones, const aiNode& assimpNode) :
				numberOfBones(_numberOfBones),
				boneParents(nullptr),
				boneIds(nullptr),
				localBonePoses(nullptr),
				boneOffsetMatrix(nullptr)
			{
				if (numberOfBones > 0)
				{
					// To be able to serialize the data in a single burst we need to have the skeleton data sequential in memory
					uint8_t* skeletonData = new uint8_t[getNumberOfSkeletonDataBytes()];
					boneParents = skeletonData;
					skeletonData += sizeof(uint8_t) * numberOfBones;
					boneIds = reinterpret_cast<uint32_t*>(skeletonData);
					skeletonData += sizeof(uint32_t) * numberOfBones;
					localBonePoses = reinterpret_cast<aiMatrix4x4*>(skeletonData);
					skeletonData += sizeof(aiMatrix4x4) * numberOfBones;
					boneOffsetMatrix = reinterpret_cast<aiMatrix4x4*>(skeletonData);

					// MD5: The MD5 bones hierarchy is stored inside an Assimp node names "<MD5_Hierarchy>"
					if (assimpNode.mName == aiString("<MD5_Root>"))
					{
						for (uint32_t i = 0; i < assimpNode.mNumChildren; ++i)
						{
							const aiNode* assimpChildNode = assimpNode.mChildren[i];
							if (assimpChildNode->mName == aiString("<MD5_Hierarchy>"))
							{
								if (1 != assimpChildNode->mNumChildren)
								{
									throw std::runtime_error("\"<MD5_Hierarchy>\" can only have a single root bone");
								}
								fillSkeletonRecursive(*assimpChildNode->mChildren[0], 0, 0);
								break;
							}
						}
					}
				}
			}

			~Skeleton()
			{
				delete [] getSkeletonData();
			}

			uint32_t getNumberOfSkeletonDataBytes() const
			{
				return (sizeof(uint8_t) + sizeof(uint32_t) + sizeof(aiMatrix4x4) * 2) * numberOfBones;
			}

			const uint8_t* getSkeletonData() const
			{
				return boneParents;
			}

			uint32_t getBoneIndexByBoneId(uint32_t boneId) const
			{
				// TODO(co) Maybe it makes sense to store the bone IDs in some order to speed up the following
				for (uint8_t boneIndex = 0; boneIndex < numberOfBones; ++boneIndex)
				{
					if (boneIds[boneIndex] == boneId)
					{
						return boneIndex;
					}
				}
				return RendererRuntime::getUninitialized<uint32_t>();
			}


		//[-------------------------------------------------------]
		//[ Private methods                                       ]
		//[-------------------------------------------------------]
		private:
			uint8_t fillSkeletonRecursive(const aiNode& assimpNode, uint8_t parentBoneIndex, uint8_t currentBoneIndex)
			{
				// Sanity check
				const uint32_t boneId = RendererRuntime::StringId(assimpNode.mName.C_Str());
				if (RendererRuntime::isInitialized(getBoneIndexByBoneId(boneId)))
				{
					throw std::runtime_error(std::string("Assimp bone name \"") + assimpNode.mName.C_Str() + "\" is not unique");
				}

				// Gather bone data
				boneParents[currentBoneIndex] = parentBoneIndex;
				boneIds[currentBoneIndex] = boneId;
				localBonePoses[currentBoneIndex] = assimpNode.mTransformation;
				parentBoneIndex = currentBoneIndex;
				++currentBoneIndex;

				// Loop through the child bones
				for (uint32_t i = 0; i < assimpNode.mNumChildren; ++i)
				{
					currentBoneIndex = fillSkeletonRecursive(*assimpNode.mChildren[i], parentBoneIndex, currentBoneIndex);
				}

				// Done
				return currentBoneIndex;
			}


		};


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Get the number of bones recursive
		*
		*  @param[in] assimpNode
		*    Assimp node to gather the data from
		*
		*  @return
		*    The number of bones
		*/
		uint32_t getNumberOfBonesRecursive(const aiNode& assimpNode)
		{
			// Loop through all child nodes recursively
			uint32_t numberOfBones = assimpNode.mNumChildren;
			for (uint32_t i = 0; i < assimpNode.mNumChildren; ++i)
			{
				numberOfBones += getNumberOfBonesRecursive(*assimpNode.mChildren[i]);
			}

			// Done
			return numberOfBones;
		}

		/**
		*  @brief
		*    Get the number of bones
		*
		*  @param[in] assimpNode
		*    Assimp node to gather the data from
		*
		*  @return
		*    The number of bones
		*/
		uint32_t getNumberOfBones(const aiNode& assimpNode)
		{
			uint32_t numberOfBones = 0;

			// MD5: The MD5 bones hierarchy is stored inside an Assimp node names "<MD5_Hierarchy>"
			if (assimpNode.mName == aiString("<MD5_Root>"))
			{
				for (uint32_t i = 0; i < assimpNode.mNumChildren; ++i)
				{
					const aiNode* assimpChildNode = assimpNode.mChildren[i];
					if (assimpChildNode->mName == aiString("<MD5_Hierarchy>"))
					{
						numberOfBones = ::detail::getNumberOfBonesRecursive(*assimpChildNode);
						break;
					}
				}
			}

			// Done
			return numberOfBones;
		}

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
		*  @param[out] subMeshes
		*    Receives the sub-meshes data
		*/
		void getNumberOfVerticesAndIndicesRecursive(const RendererToolkit::IAssetCompiler::Input& input, const aiScene& assimpScene, const aiNode& assimpNode, uint32_t& numberOfVertices, uint32_t& numberOfIndices, SubMeshes& subMeshes)
		{
			// Loop through all meshes this node is using
			for (uint32_t i = 0; i < assimpNode.mNumMeshes; ++i)
			{
				// Get the used mesh
				const aiMesh& assimpMesh = *assimpScene.mMeshes[assimpNode.mMeshes[i]];

				// Update the number of vertices
				numberOfVertices += assimpMesh.mNumVertices;

				// Loop through all mesh faces and update the number of indices
				const uint32_t previousNumberOfIndices = numberOfIndices;
				for (uint32_t j = 0; j < assimpMesh.mNumFaces; ++j)
				{
					numberOfIndices += assimpMesh.mFaces[j].mNumIndices;
				}

				{ // Add sub-mesh
					// Get the source material asset ID
					aiString materialName;
					assimpScene.mMaterials[assimpMesh.mMaterialIndex]->Get(AI_MATKEY_NAME, materialName);

					// In case the Assimp "aiProcess_RemoveRedundantMaterials"-flag is set materials might get merged, we need to take this into account
					if (nullptr != strstr(materialName.C_Str(), "JoinedMaterial_"))
					{
						// If we're in luck, the diffuse texture 0 stores the material name
						assimpScene.mMaterials[assimpMesh.mMaterialIndex]->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), materialName);

						// The Assimp MD5 mesh loader modifies "shader" inside "MD5Loader.cpp"
						const char* diffuseExtension = strstr(materialName.C_Str(), "_d.tga");
						if (nullptr != diffuseExtension)
						{
							// Const-casts are evil in general, but in this situation in here this is the most-simple solution to cut off "_d.tga"
							*const_cast<char*>(diffuseExtension) = '\0';
						}
					}

					// Add sub-mesh
					RendererRuntime::v1Mesh::SubMesh subMesh;
					subMesh.materialAssetId		= RendererToolkit::StringHelper::getAssetIdByString(materialName.C_Str(), input);
					subMesh.primitiveTopology	= static_cast<uint8_t>(Renderer::PrimitiveTopology::TRIANGLE_LIST);
					subMesh.startIndexLocation	= previousNumberOfIndices;
					subMesh.numberOfIndices		= numberOfIndices - previousNumberOfIndices;
					subMeshes.push_back(subMesh);
				}
			}

			// Loop through all child nodes recursively
			for (uint32_t i = 0; i < assimpNode.mNumChildren; ++i)
			{
				getNumberOfVerticesAndIndicesRecursive(input, assimpScene, *assimpNode.mChildren[i], numberOfVertices, numberOfIndices, subMeshes);
			}
		}

		/**
		*  @brief
		*    Fill the mesh data recursively
		*
		*  @param[in]  assimpScene
		*    Assimp scene
		*  @param[in]  assimpNode
		*    Assimp node to gather the data from
		*  @param[in]  skeleton
		*    Skeleton instance
		*  @param[in]  invertNormals
		*    Invert normals?
		*  @param[in]  numberOfBytesPerVertex
		*    Number of bytes per vertex
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
		void fillMeshRecursive(const aiScene &assimpScene, const aiNode &assimpNode, const Skeleton& skeleton, bool invertNormals, uint8_t numberOfBytesPerVertex, uint8_t *vertexBuffer, uint16_t *indexBuffer, const aiMatrix4x4 &assimpTransformation, uint32_t &numberOfVertices, uint32_t &numberOfIndices)
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

				{ // Loop through the Assimp mesh vertices
					uint8_t *currentVertexBuffer = vertexBuffer + numberOfVertices * numberOfBytesPerVertex;
					for (uint32_t j = 0; j < assimpMesh.mNumVertices; ++j)
					{
						uint8_t *currentVertex = currentVertexBuffer + j * numberOfBytesPerVertex;

						{ // 32 bit position
							// Get the Assimp mesh vertex position
							aiVector3D assimpVertex = assimpMesh.mVertices[j];

							// Transform the Assimp mesh vertex position into global space
							assimpVertex *= currentAssimpTransformation;

							// Set our vertex buffer position
							float *currentVertexBufferFloat = reinterpret_cast<float*>(currentVertex);
							*currentVertexBufferFloat = assimpVertex.x;
							++currentVertexBufferFloat;
							*currentVertexBufferFloat = assimpVertex.y;
							++currentVertexBufferFloat;
							*currentVertexBufferFloat = assimpVertex.z;
							currentVertex += sizeof(float) * 3;
						}

						{ // 32 bit texture coordinate
							// Get the Assimp mesh vertex texture coordinate
							aiVector3D assimpTexCoord = assimpMesh.mTextureCoords[0][j];

							// Set our vertex buffer 32 bit texture coordinate
							float *currentVertexBufferFloat = reinterpret_cast<float*>(currentVertex);
							*currentVertexBufferFloat = assimpTexCoord.x;
							++currentVertexBufferFloat;
							*currentVertexBufferFloat = assimpTexCoord.y;
							currentVertex += sizeof(float) * 2;
						}

						{ // 16 bit QTangent
						  // - QTangent basing on http://dev.theomader.com/qtangents/ "QTangents" which is basing on
						  //   http://www.crytek.com/cryengine/presentations/spherical-skinning-with-dual-quaternions-and-qtangents "Spherical Skinning with Dual-Quaternions and QTangents"
							// Get the Assimp mesh vertex tangent, binormal and normal
							aiVector3D tangent = assimpMesh.mTangents[j];
							aiVector3D binormal = assimpMesh.mBitangents[j];
							aiVector3D normal = invertNormals ? -assimpMesh.mNormals[j] : assimpMesh.mNormals[j];

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

							// Calculate tangent frame quaternion
							const glm::quat tangentFrameQuaternion = RendererRuntime::Math::calculateTangentFrameQuaternion(tangentFrame);

							// Set our vertex buffer 16 bit QTangent
							short *currentVertexBufferShort = reinterpret_cast<short*>(currentVertex);
							*currentVertexBufferShort = static_cast<short>(tangentFrameQuaternion.x * SHRT_MAX);
							++currentVertexBufferShort;
							*currentVertexBufferShort = static_cast<short>(tangentFrameQuaternion.y * SHRT_MAX);
							++currentVertexBufferShort;
							*currentVertexBufferShort = static_cast<short>(tangentFrameQuaternion.z * SHRT_MAX);
							++currentVertexBufferShort;
							*currentVertexBufferShort = static_cast<short>(tangentFrameQuaternion.w * SHRT_MAX);
							currentVertex += sizeof(short) * 4;
						}
					}
				}

				// Process the Assimp bones, if there are any to start with
				if (assimpMesh.mNumBones > 0)
				{
					std::vector<uint8_t> numberOfWeightsPerVertex;
					numberOfWeightsPerVertex.resize(assimpMesh.mNumVertices);
					memset(numberOfWeightsPerVertex.data(), 0, sizeof(uint8_t) * assimpMesh.mNumVertices);

					// Loop through the Assimp bones
					uint8_t *currentVertexBuffer = vertexBuffer + numberOfVertices * numberOfBytesPerVertex;
					for (unsigned int bone = 0; bone < assimpMesh.mNumBones; ++bone)
					{
						const aiBone* assimpBone = assimpMesh.mBones[bone];
						const uint32_t boneIndex = skeleton.getBoneIndexByBoneId(RendererRuntime::StringId(assimpBone->mName.C_Str()));
						if (RendererRuntime::isUninitialized(boneIndex))
						{
							throw std::runtime_error(std::string("Invalid Assimp bone name \"") + assimpBone->mName.C_Str() + '\"');
						}
						skeleton.boneOffsetMatrix[boneIndex] = assimpBone->mOffsetMatrix;

						// Loop through the Assimp bone weights
						for (unsigned int weight = 0; weight < assimpBone->mNumWeights; ++weight)
						{
							const aiVertexWeight& assimpVertexWeight = assimpBone->mWeights[weight];

							// Does this vertex still have a free weight slot?
							const uint8_t numberOfVertexWeights = numberOfWeightsPerVertex[assimpVertexWeight.mVertexId];
							if (numberOfVertexWeights < 4)
							{
								uint8_t *currentVertex = currentVertexBuffer + assimpVertexWeight.mVertexId * numberOfBytesPerVertex;

								// Skip 32 bit position, 32 bit texture coordinate and 16 bit QTangent
								currentVertex += sizeof(float) * 3 + sizeof(float) * 2 + sizeof(short) * 4;

								{ // 8 bit bone indices
									uint8_t* boneIndices = currentVertex;
									boneIndices[numberOfVertexWeights] = static_cast<uint8_t>(boneIndex);
									currentVertex += sizeof(uint8_t) * 4;
								}

								{ // 32 bit bone weights
									float* boneWeights = reinterpret_cast<float*>(currentVertex);
									boneWeights[numberOfVertexWeights] = assimpVertexWeight.mWeight;
								}

								// Update the number of vertex weights
								++numberOfWeightsPerVertex[assimpVertexWeight.mVertexId];
							}
						}
					}
				}

				// Update the number of processed vertices
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
				fillMeshRecursive(assimpScene, *assimpNode.mChildren[assimpChild], skeleton, invertNormals, numberOfBytesPerVertex, vertexBuffer, indexBuffer, currentAssimpTransformation, numberOfVertices, numberOfIndices);
			}
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId MeshAssetCompiler::TYPE_ID("Mesh");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MeshAssetCompiler::MeshAssetCompiler()
	{
		// Nothing here
	}

	MeshAssetCompiler::~MeshAssetCompiler()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId MeshAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void MeshAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory	= input.assetOutputDirectory;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read configuration
		// TODO(co) Add required properties
		std::string inputFile;
		bool invertNormals = false;
		{
			// Read mesh asset compiler configuration
			const rapidjson::Value& rapidJsonValueMeshAssetCompiler = rapidJsonValueAsset["MeshAssetCompiler"];
			inputFile = rapidJsonValueMeshAssetCompiler["InputFile"].GetString();
			JsonHelper::optionalBooleanProperty(rapidJsonValueMeshAssetCompiler, "InvertNormals", invertNormals);
		}

		// Open the input and output file
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".mesh";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		// Create an instance of the Assimp importer class
		Assimp::Importer assimpImporter;
		// assimpImporter.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);	// We're using the "aiProcess_LimitBoneWeights"-flag, 4 is already the default value (don't delete this reminder comment)

		// Load the given mesh
		// -> "aiProcess_MakeLeftHanded" is added because the rasterizer states directly map to Direct3D
		const aiScene *assimpScene = assimpImporter.ReadFile(assetInputDirectory + inputFile, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_MakeLeftHanded);
		if (nullptr != assimpScene && nullptr != assimpScene->mRootNode)
		{
			// Get the number of bones and skeleton
			const uint32_t numberOfBones = ::detail::getNumberOfBones(*assimpScene->mRootNode);
			if (numberOfBones > 255)
			{
				throw std::runtime_error("Maximum number of supported bones is 255");
			}
			::detail::Skeleton skeleton(static_cast<uint8_t>(numberOfBones), *assimpScene->mRootNode);

			// Get the total number of vertices and indices by using the Assimp root node
			uint32_t numberOfVertices = 0;
			uint32_t numberOfIndices = 0;
			::detail::SubMeshes subMeshes;
			::detail::getNumberOfVerticesAndIndicesRecursive(input, *assimpScene, *assimpScene->mRootNode, numberOfVertices, numberOfIndices, subMeshes);

			// Is there an optional skeleton?
			const Renderer::VertexAttributes& vertexAttributes = (numberOfBones > 0) ? RendererRuntime::MeshResource::SKINNED_VERTEX_ATTRIBUTES : RendererRuntime::MeshResource::VERTEX_ATTRIBUTES;
			const uint8_t numberOfBytesPerVertex = (numberOfBones > 0) ? ::detail::NUMBER_OF_BYTES_PER_SKINNED_VERTEX : ::detail::NUMBER_OF_BYTES_PER_VERTEX;

			{ // Mesh header
				RendererRuntime::v1Mesh::Header meshHeader;
				meshHeader.formatType				= RendererRuntime::v1Mesh::FORMAT_TYPE;
				meshHeader.formatVersion			= RendererRuntime::v1Mesh::FORMAT_VERSION;
				meshHeader.numberOfBytesPerVertex	= numberOfBytesPerVertex;
				meshHeader.numberOfVertices			= numberOfVertices;
				meshHeader.indexBufferFormat		= Renderer::IndexBufferFormat::UNSIGNED_SHORT;	// TODO(co) Support of 32 bit indices if there are too many vertices
				meshHeader.numberOfIndices			= numberOfIndices;
				meshHeader.numberOfVertexAttributes = static_cast<uint8_t>(vertexAttributes.numberOfAttributes);
				meshHeader.numberOfSubMeshes		= static_cast<uint8_t>(subMeshes.size());
				meshHeader.numberOfBones			= skeleton.numberOfBones;

				// Write down the mesh header
				outputFileStream.write(reinterpret_cast<const char*>(&meshHeader), sizeof(RendererRuntime::v1Mesh::Header));
			}

			{ // Vertex and index buffer data
				// Allocate memory for the local vertex and index buffer data
				// -> Do also initialize the vertex buffer data with zero to handle not filled vertex bone weights
				uint8_t *vertexBufferData = new uint8_t[numberOfBytesPerVertex * numberOfVertices];
				memset(vertexBufferData, 0, numberOfBytesPerVertex * numberOfVertices);
				uint16_t *indexBufferData = new uint16_t[numberOfIndices];

				{ // Fill the mesh data recursively
					uint32_t numberOfFilledVertices = 0;
					uint32_t numberOfFilledIndices  = 0;
					::detail::fillMeshRecursive(*assimpScene, *assimpScene->mRootNode, skeleton, invertNormals, numberOfBytesPerVertex, vertexBufferData, indexBufferData, aiMatrix4x4(), numberOfFilledVertices, numberOfFilledIndices);

					// TODO(co) ?
					numberOfVertices = numberOfFilledVertices;
					numberOfIndices  = numberOfFilledIndices;
				}

				// Write down the vertex and index buffer
				outputFileStream.write(reinterpret_cast<const char*>(vertexBufferData), numberOfBytesPerVertex * numberOfVertices);
				outputFileStream.write(reinterpret_cast<const char*>(indexBufferData), static_cast<std::streamsize>(sizeof(uint16_t) * numberOfIndices));

				// Destroy local vertex and input buffer data
				delete [] vertexBufferData;
				delete [] indexBufferData;
			}

			// Write down the vertex array attributes
			outputFileStream.write(reinterpret_cast<const char*>(vertexAttributes.attributes), static_cast<std::streamsize>(sizeof(Renderer::VertexAttribute) * vertexAttributes.numberOfAttributes));

			// Write down the sub-meshes
			outputFileStream.write(reinterpret_cast<const char*>(subMeshes.data()), static_cast<std::streamsize>(sizeof(RendererRuntime::v1Mesh::SubMesh) * subMeshes.size()));

			// Write down the optional skeleton
			if (skeleton.numberOfBones > 0)
			{
				for (uint8_t i = 0; i < skeleton.numberOfBones; ++i)
				{
					skeleton.localBonePoses[i].Transpose();
					skeleton.boneOffsetMatrix[i].Transpose();
				}
				outputFileStream.write(reinterpret_cast<const char*>(skeleton.getSkeletonData()), static_cast<std::streamsize>(skeleton.getNumberOfSkeletonDataBytes()));
			}
		}
		else
		{
			throw std::runtime_error("Assimp failed to load in the given mesh");
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/Mesh/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::Asset outputAsset;
			outputAsset.assetId = StringHelper::getAssetIdByString(assetIdAsString.c_str());
			strcpy(outputAsset.assetFilename, outputAssetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
