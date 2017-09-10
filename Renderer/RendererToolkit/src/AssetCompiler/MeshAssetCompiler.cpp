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
#include "RendererToolkit/Helper/FileSystemHelper.h"
#include "RendererToolkit/Helper/CacheManager.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"
#include "RendererToolkit/Context.h"

#include <RendererRuntime/Core/Math/Math.h>
#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Core/File/MemoryFile.h>
#include <RendererRuntime/Resource/Mesh/MeshResource.h>
#include <RendererRuntime/Resource/Mesh/Loader/MeshFileFormat.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4061)	// warning C4061: enumerator 'FORCE_32BIT' in switch of enum 'aiMetadataType' is not explicitly handled by a case label
	#include <assimp/scene.h>
	#include <assimp/Importer.hpp>
	#include <assimp/postprocess.h>
	#include <assimp/DefaultLogger.hpp>
PRAGMA_WARNING_POP

#include <mikktspace/mikktspace.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{

	namespace mikktspace
	{


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		SMikkTSpaceContext g_MikkTSpaceContext;


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		int getNumFaces(const SMikkTSpaceContext* pContext)
		{
			const aiMesh* assimpMesh = static_cast<const aiMesh*>(pContext->m_pUserData);
			return static_cast<int>(assimpMesh->mNumFaces);
		}

		int getNumVerticesOfFace(const SMikkTSpaceContext* pContext, const int iFace)
		{
			const aiMesh* assimpMesh = static_cast<const aiMesh*>(pContext->m_pUserData);
			return static_cast<int>(assimpMesh->mFaces[iFace].mNumIndices);
		}

		void getPosition(const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert)
		{
			const aiMesh* assimpMesh = static_cast<const aiMesh*>(pContext->m_pUserData);
			const aiVector3D& assimpVertex = assimpMesh->mVertices[assimpMesh->mFaces[iFace].mIndices[iVert]];
			fvPosOut[0] = assimpVertex.x;
			fvPosOut[1] = assimpVertex.y;
			fvPosOut[2] = assimpVertex.z;
		}

		void getNormal(const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert)
		{
			const aiMesh* assimpMesh = static_cast<const aiMesh*>(pContext->m_pUserData);
			const aiVector3D& assimpNormal = assimpMesh->mNormals[assimpMesh->mFaces[iFace].mIndices[iVert]];
			fvNormOut[0] = assimpNormal.x;
			fvNormOut[1] = assimpNormal.y;
			fvNormOut[2] = assimpNormal.z;
		}

		void getTexCoord(const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert)
		{
			const aiMesh* assimpMesh = static_cast<const aiMesh*>(pContext->m_pUserData);
			const aiVector3D& assimpTexCoord = assimpMesh->mTextureCoords[0][assimpMesh->mFaces[iFace].mIndices[iVert]];
			fvTexcOut[0] = assimpTexCoord.x;
			fvTexcOut[1] = assimpTexCoord.y;
		}

		void setTSpace(const SMikkTSpaceContext* pContext, const float fvTangent[], const float fvBiTangent[], const float, const float, const tbool, const int iFace, const int iVert)
		{
			const aiMesh* assimpMesh = static_cast<const aiMesh*>(pContext->m_pUserData);
			const unsigned int index = assimpMesh->mFaces[iFace].mIndices[iVert];

			{ // Tangent
				aiVector3D& assimpTangent = assimpMesh->mTangents[index];
				assimpTangent.x = fvTangent[0];
				assimpTangent.y = fvTangent[1];
				assimpTangent.z = fvTangent[2];
			}

			{ // Binormal
				aiVector3D& assimpBinormal = assimpMesh->mBitangents[index];
				assimpBinormal.x = fvBiTangent[0];
				assimpBinormal.y = fvBiTangent[1];
				assimpBinormal.z = fvBiTangent[2];
			}
		}


	} // mikktspace

	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const uint8_t NUMBER_OF_BYTES_PER_VERTEX = 28;										///< Number of bytes per vertex (3 float position, 2 float texture coordinate, 4 short QTangent)
		static const uint8_t NUMBER_OF_BYTES_PER_SKINNED_VERTEX = NUMBER_OF_BYTES_PER_VERTEX + 8;	///< Number of bytes per skinned vertex (+4 byte bone indices, +4 byte bone weights)
		typedef std::vector<RendererRuntime::v1Mesh::SubMesh> SubMeshes;
		typedef std::unordered_map<std::string, RendererRuntime::AssetId> MaterialNameToAssetId;


		//[-------------------------------------------------------]
		//[ Classes                                               ]
		//[-------------------------------------------------------]
		class AssimpLogStream : public Assimp::LogStream
		{


		//[-------------------------------------------------------]
		//[ Public data                                           ]
		//[-------------------------------------------------------]
		public:
			inline const std::string& getLastErrorMessage() const
			{
				return mLastErrorMessage;
			}


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			AssimpLogStream()
			{
				// Nothing here
			}

			virtual ~AssimpLogStream() override
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Public virtual Assimp::LogStream methods              ]
		//[-------------------------------------------------------]
		public:
			virtual void write(const char* message) override
			{
				// Ignore "Error, T88896: Failed to compute tangents; need UV data in channel0" since some sub-meshes might have no texture coordinates, worth a hint but no error
				if (nullptr != strstr(message, "Error, T88896: Failed to compute tangents; need UV data in channel0"))
				{
					mLastErrorMessage = message;
					throw std::runtime_error(message);
				}
			}


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			std::string mLastErrorMessage;


		};

		class Skeleton
		{


		//[-------------------------------------------------------]
		//[ Public data                                           ]
		//[-------------------------------------------------------]
		public:
			uint8_t		 numberOfBones;			///< Number of bones
			// Structure-of-arrays (SoA)
			uint8_t*	 boneParentIndices;		///< Cache friendly depth-first rolled up bone parent indices, null pointer only in case of horrible error, free the memory if no longer required
			uint32_t*	 boneIds;				///< Cache friendly depth-first rolled up bone IDs ("RendererRuntime::StringId" on bone name), null pointer only in case of horrible error, don't free the memory because it's owned by "boneParentIndices"
			aiMatrix4x4* localBoneMatrices;		///< Cache friendly depth-first rolled up local bone matrices, null pointer only in case of horrible error, don't free the memory because it's owned by "boneParentIndices"
			aiMatrix4x4* boneOffsetMatrices;	///< Cache friendly depth-first rolled up bone offset matrices (object space to bone space), null pointer only in case of horrible error, free the memory if no longer required


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit Skeleton(uint8_t _numberOfBones, const aiNode& assimpNode) :
				numberOfBones(_numberOfBones),
				boneParentIndices(nullptr),
				boneIds(nullptr),
				localBoneMatrices(nullptr),
				boneOffsetMatrices(nullptr)
			{
				if (numberOfBones > 0)
				{
					// To be able to serialize the data in a single burst we need to have the skeleton data sequential in memory
					uint8_t* skeletonData = new uint8_t[getNumberOfSkeletonDataBytes()];
					boneParentIndices = skeletonData;
					skeletonData += sizeof(uint8_t) * numberOfBones;
					boneIds = reinterpret_cast<uint32_t*>(skeletonData);
					skeletonData += sizeof(uint32_t) * numberOfBones;
					localBoneMatrices = reinterpret_cast<aiMatrix4x4*>(skeletonData);
					skeletonData += sizeof(aiMatrix4x4) * numberOfBones;
					boneOffsetMatrices = reinterpret_cast<aiMatrix4x4*>(skeletonData);

					// OGRE: The scene root node has no name
					if (0 == assimpNode.mName.length)
					{
						if (1 != assimpNode.mNumChildren)
						{
							throw std::runtime_error("There can be only single root bone");
						}
						fillSkeletonRecursive(*assimpNode.mChildren[0], 0, 0);

						// Some Assimp importers like the OGRE one compensate coordinate system differences by setting a root node transform, so we need to take this into account
						localBoneMatrices[0] = assimpNode.mTransformation * localBoneMatrices[0];
					}

					// MD5: The MD5 bones hierarchy is stored inside an Assimp node names "<MD5_Hierarchy>"
					else if (assimpNode.mName == aiString("<MD5_Root>"))
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

								// Some Assimp importers like the MD5 one compensate coordinate system differences by setting a root node transform, so we need to take this into account
								localBoneMatrices[0] = assimpNode.mTransformation * localBoneMatrices[0];
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
				return boneParentIndices;
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
				boneParentIndices[currentBoneIndex] = parentBoneIndex;
				boneIds[currentBoneIndex] = boneId;
				localBoneMatrices[currentBoneIndex] = assimpNode.mTransformation;
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

			// OGRE: The scene root node has no name
			if (0 == assimpNode.mName.length)
			{
				if (1 != assimpNode.mNumChildren)
				{
					throw std::runtime_error("There can be only single root bone");
				}
				numberOfBones = getNumberOfBonesRecursive(assimpNode);
			}

			// MD5: The MD5 bones hierarchy is stored inside an Assimp node names "<MD5_Hierarchy>"
			else if (assimpNode.mName == aiString("<MD5_Root>"))
			{
				for (uint32_t i = 0; i < assimpNode.mNumChildren; ++i)
				{
					const aiNode* assimpChildNode = assimpNode.mChildren[i];
					if (assimpChildNode->mName == aiString("<MD5_Hierarchy>"))
					{
						numberOfBones = getNumberOfBonesRecursive(*assimpChildNode);
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
		*  @param[in] input
		*    Asset compiler input
		*  @param[in] materialNameToAssetId
		*    Material name to asset ID mapping information
		*  @param[in] assimpScene
		*    Assimp scene
		*  @param[in] assimpNode
		*    Assimp node to gather the data from
		*  @param[out] numberOfVertices
		*    Receives the number of vertices
		*  @param[out] numberOfIndices
		*    Receives the number of indices
		*  @param[out] subMeshes
		*    Receives the sub-meshes data
		*/
		void getNumberOfVerticesAndIndicesRecursive(const RendererToolkit::IAssetCompiler::Input& input, const MaterialNameToAssetId& materialNameToAssetId, const aiScene& assimpScene, const aiNode& assimpNode, uint32_t& numberOfVertices, uint32_t& numberOfIndices, SubMeshes& subMeshes)
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
					RendererRuntime::AssetId materialAssetId = RendererRuntime::getUninitialized<RendererRuntime::AssetId>();
					{
						aiString materialName;
						const aiMaterial* assimpMaterial = assimpScene.mMaterials[assimpMesh.mMaterialIndex];
						assimpMaterial->Get(AI_MATKEY_NAME, materialName);
						MaterialNameToAssetId::const_iterator iterator = materialNameToAssetId.find(materialName.C_Str());
						if (materialNameToAssetId.cend() != iterator)
						{
							materialAssetId = iterator->second;
						}
						else
						{
							if (!RendererToolkit::StringHelper::isSourceAssetIdAsString(materialName.C_Str()))
							{
								// If we're in luck, the diffuse texture 0 stores the material name
								assimpMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), materialName);
							}
							if (materialName.length > 0 && nullptr == strstr(materialName.C_Str(), AI_DEFAULT_MATERIAL_NAME))
							{
								materialAssetId = RendererToolkit::StringHelper::getAssetIdByString(materialName.C_Str(), input);
							}
						}
					}

					// Add sub-mesh
					if (RendererRuntime::isInitialized(materialAssetId))
					{
						RendererRuntime::v1Mesh::SubMesh subMesh;
						subMesh.materialAssetId		= materialAssetId;
						subMesh.primitiveTopology	= static_cast<uint8_t>(Renderer::PrimitiveTopology::TRIANGLE_LIST);
						subMesh.startIndexLocation	= previousNumberOfIndices;
						subMesh.numberOfIndices		= numberOfIndices - previousNumberOfIndices;
						subMeshes.push_back(subMesh);
					}
				}
			}

			// Loop through all child nodes recursively
			for (uint32_t i = 0; i < assimpNode.mNumChildren; ++i)
			{
				getNumberOfVerticesAndIndicesRecursive(input, materialNameToAssetId, assimpScene, *assimpNode.mChildren[i], numberOfVertices, numberOfIndices, subMeshes);
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
		void fillMeshRecursive(const aiScene& assimpScene, const aiNode& assimpNode, const Skeleton& skeleton, uint8_t numberOfBytesPerVertex, uint8_t* vertexBuffer, uint32_t* indexBuffer, const aiMatrix4x4& assimpTransformation, uint32_t& numberOfVertices, uint32_t& numberOfIndices)
		{
			// Get the absolute transformation matrix of this Assimp node
			const aiMatrix4x4 currentAssimpTransformation = assimpTransformation * assimpNode.mTransformation;
			const aiMatrix3x3 currentAssimpNormalTransformation = aiMatrix3x3(currentAssimpTransformation);

			// Loop through all meshes this node is using
			for (uint32_t i = 0; i < assimpNode.mNumMeshes; ++i)
			{
				// Get the used mesh
				aiMesh& assimpMesh = *assimpScene.mMeshes[assimpNode.mMeshes[i]];

				// Use "mikktspace" by Morten S. Mikkelsen for semi-standard tangent space generation and overwrite what Assimp calculated (see e.g. https://wiki.blender.org/index.php/Dev:Shading/Tangent_Space_Normal_Maps for background information)
				if (0 != assimpMesh.mNumUVComponents[0])
				{
					mikktspace::g_MikkTSpaceContext.m_pUserData = reinterpret_cast<void*>(&assimpMesh);
					if (genTangSpaceDefault(&mikktspace::g_MikkTSpaceContext) == 0)
					{
						throw std::runtime_error("mikktspace for semi-standard tangent space generation failed");
					}
				}

				// Get the start vertex inside the our vertex buffer
				const uint32_t starVertex = numberOfVertices;

				{ // Loop through the Assimp mesh vertices
					uint8_t* currentVertexBuffer = vertexBuffer + numberOfVertices * numberOfBytesPerVertex;
					for (uint32_t j = 0; j < assimpMesh.mNumVertices; ++j)
					{
						uint8_t* currentVertex = currentVertexBuffer + j * numberOfBytesPerVertex;

						{ // 32 bit position
							// Get the Assimp mesh vertex position
							aiVector3D assimpVertex = assimpMesh.mVertices[j];

							// Transform the Assimp mesh vertex position into global space
							assimpVertex *= currentAssimpTransformation;

							// Set our vertex buffer position
							float* currentVertexBufferFloat = reinterpret_cast<float*>(currentVertex);
							*currentVertexBufferFloat = assimpVertex.x;
							++currentVertexBufferFloat;
							*currentVertexBufferFloat = assimpVertex.y;
							++currentVertexBufferFloat;
							*currentVertexBufferFloat = assimpVertex.z;
							currentVertex += sizeof(float) * 3;
						}

						// 32 bit texture coordinate
						if (0 != assimpMesh.mNumUVComponents[0])
						{
							// Get the Assimp mesh vertex texture coordinate
							aiVector3D assimpTexCoord = assimpMesh.mTextureCoords[0][j];

							// Set our vertex buffer 32 bit texture coordinate
							float* currentVertexBufferFloat = reinterpret_cast<float*>(currentVertex);
							*currentVertexBufferFloat = assimpTexCoord.x;
							++currentVertexBufferFloat;
							*currentVertexBufferFloat = assimpTexCoord.y;
							currentVertex += sizeof(float) * 2;
						}
						else
						{
							// Set our vertex buffer 32 bit texture coordinate
							float* currentVertexBufferFloat = reinterpret_cast<float*>(currentVertex);
							*currentVertexBufferFloat = 0.0f;
							++currentVertexBufferFloat;
							*currentVertexBufferFloat = 0.0f;
							currentVertex += sizeof(float) * 2;
						}

						{ // 16 bit QTangent
						  // - QTangent basing on http://dev.theomader.com/qtangents/ "QTangents" which is basing on
						  //   http://www.crytek.com/cryengine/presentations/spherical-skinning-with-dual-quaternions-and-qtangents "Spherical Skinning with Dual-Quaternions and QTangents"
							// Get the Assimp mesh vertex tangent, binormal and normal
							aiVector3D tangent(1.0f, 0.0f, 0.0f);
							aiVector3D binormal(0.0f, 1.0f, 0.0f);
							aiVector3D normal = assimpMesh.mNormals[j];
							if (0 != assimpMesh.mNumUVComponents[0])
							{
								tangent = assimpMesh.mTangents[j];
								binormal = assimpMesh.mBitangents[j];
							}

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
							short* currentVertexBufferShort = reinterpret_cast<short*>(currentVertex);
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
					uint8_t* currentVertexBuffer = vertexBuffer + numberOfVertices * numberOfBytesPerVertex;
					for (unsigned int bone = 0; bone < assimpMesh.mNumBones; ++bone)
					{
						const aiBone* assimpBone = assimpMesh.mBones[bone];
						const uint32_t boneIndex = skeleton.getBoneIndexByBoneId(RendererRuntime::StringId(assimpBone->mName.C_Str()));
						if (RendererRuntime::isUninitialized(boneIndex))
						{
							throw std::runtime_error(std::string("Invalid Assimp bone name \"") + assimpBone->mName.C_Str() + '\"');
						}
						skeleton.boneOffsetMatrices[boneIndex] = assimpBone->mOffsetMatrix;

						// Loop through the Assimp bone weights
						for (unsigned int weight = 0; weight < assimpBone->mNumWeights; ++weight)
						{
							const aiVertexWeight& assimpVertexWeight = assimpBone->mWeights[weight];

							// Does this vertex still have a free weight slot?
							const uint8_t numberOfVertexWeights = numberOfWeightsPerVertex[assimpVertexWeight.mVertexId];
							if (numberOfVertexWeights < 4)
							{
								uint8_t* currentVertex = currentVertexBuffer + assimpVertexWeight.mVertexId * numberOfBytesPerVertex;

								// Skip 32 bit position, 32 bit texture coordinate and 16 bit QTangent
								currentVertex += sizeof(float) * 3 + sizeof(float) * 2 + sizeof(short) * 4;

								{ // 8 bit bone indices
									uint8_t* boneIndices = currentVertex;
									boneIndices[numberOfVertexWeights] = static_cast<uint8_t>(boneIndex);
									currentVertex += sizeof(uint8_t) * 4;
								}

								{ // 8 bit bone weights
									uint8_t* boneWeights = reinterpret_cast<uint8_t*>(currentVertex);
									boneWeights[numberOfVertexWeights] = static_cast<uint8_t>(assimpVertexWeight.mWeight * 255);
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
				uint32_t* currentIndexBuffer = indexBuffer + numberOfIndices;
				for (uint32_t j = 0; j < assimpMesh.mNumFaces; ++j)
				{
					// Get the Assimp face
					const aiFace& assimpFace = assimpMesh.mFaces[j];

					// Loop through all indices of the Assimp face and set our indices
					for (uint32_t assimpIndex = 0; assimpIndex < assimpFace.mNumIndices; ++assimpIndex, ++currentIndexBuffer)
					{
						//					  Assimp mesh vertex index			 Where the Assimp mesh starts within the our vertex buffer
						*currentIndexBuffer = assimpFace.mIndices[assimpIndex] + starVertex;
					}

					// Update the number if processed indices
					numberOfIndices += assimpFace.mNumIndices;
				}
			}

			// Loop through all child nodes recursively
			for (uint32_t assimpChild = 0; assimpChild < assimpNode.mNumChildren; ++assimpChild)
			{
				fillMeshRecursive(assimpScene, *assimpNode.mChildren[assimpChild], skeleton, numberOfBytesPerVertex, vertexBuffer, indexBuffer, currentAssimpTransformation, numberOfVertices, numberOfIndices);
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

		// Read mesh asset compiler configuration
		const rapidjson::Value& rapidJsonValueMeshAssetCompiler = rapidJsonValueAsset["MeshAssetCompiler"];
		const std::string inputFile = rapidJsonValueMeshAssetCompiler["InputFile"].GetString();

		// Open the input and output file
		const std::string inputFilename = assetInputDirectory + inputFile;
		const std::string assetName = std_filesystem::path(input.assetFilename).stem().generic_string();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".mesh";

		// Ask the cache manager whether or not we need to compile the source file (e.g. source changed or target not there)
		CacheManager::CacheEntries cacheEntries;
		if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.assetFilename, inputFilename, outputAssetFilename, RendererRuntime::v1Mesh::FORMAT_VERSION, cacheEntries))
		{
			RendererRuntime::MemoryFile memoryFile(0, 42 * 1024);

			// Setup "mikktspace" by Morten S. Mikkelsen for semi-standard tangent space generation (see e.g. https://wiki.blender.org/index.php/Dev:Shading/Tangent_Space_Normal_Maps for background information)
			SMikkTSpaceInterface mikkTSpaceInterface;
			mikkTSpaceInterface.m_getNumFaces		   = mikktspace::getNumFaces;
			mikkTSpaceInterface.m_getNumVerticesOfFace = mikktspace::getNumVerticesOfFace;
			mikkTSpaceInterface.m_getPosition		   = mikktspace::getPosition;
			mikkTSpaceInterface.m_getNormal			   = mikktspace::getNormal;
			mikkTSpaceInterface.m_getTexCoord		   = mikktspace::getTexCoord;
			mikkTSpaceInterface.m_setTSpaceBasic	   = nullptr;
			mikkTSpaceInterface.m_setTSpace			   = mikktspace::setTSpace;
			mikktspace::g_MikkTSpaceContext.m_pInterface = &mikkTSpaceInterface;
			mikktspace::g_MikkTSpaceContext.m_pUserData  = nullptr;

			// Startup Assimp logging
			::detail::AssimpLogStream assimpLogStream;
			Assimp::DefaultLogger::create("", Assimp::Logger::NORMAL, aiDefaultLogStream_DEBUGGER);
			Assimp::DefaultLogger::get()->attachStream(&assimpLogStream, Assimp::DefaultLogger::Err);

			// Create an instance of the Assimp importer class
			Assimp::Importer assimpImporter;
			// assimpImporter.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);	// We're using the "aiProcess_LimitBoneWeights"-flag, 4 is already the default value (don't delete this reminder comment)

			// Load the given mesh
			// -> "aiProcess_MakeLeftHanded" is added because the rasterizer states directly map to Direct3D
			// -> We're using "mikktspace" by Morten S. Mikkelsen for semi-standard tangent space generation (see e.g. https://wiki.blender.org/index.php/Dev:Shading/Tangent_Space_Normal_Maps for background information)
			// -> "aiProcess_CalcTangentSpace" from Assimp is still used to allocate internal memory and enable Assimp to perform work regarding e.g. shared vertices
			const aiScene* assimpScene = assimpImporter.ReadFile(inputFilename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder);
			if (nullptr != assimpScene && nullptr != assimpScene->mRootNode)
			{
				// Get the optional material name to asset ID mapping information
				::detail::MaterialNameToAssetId materialNameToAssetId;
				if (rapidJsonValueMeshAssetCompiler.HasMember("MaterialNameToAssetId"))
				{
					const rapidjson::Value& rapidJsonValueMaterialNameToAssetId = rapidJsonValueMeshAssetCompiler["MaterialNameToAssetId"];
					for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIterator = rapidJsonValueMaterialNameToAssetId.MemberBegin(); rapidJsonMemberIterator != rapidJsonValueMaterialNameToAssetId.MemberEnd(); ++rapidJsonMemberIterator)
					{
						materialNameToAssetId.emplace(rapidJsonMemberIterator->name.GetString(), StringHelper::getAssetIdByString(rapidJsonMemberIterator->value.GetString(), input));
					}
				}

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
				::detail::getNumberOfVerticesAndIndicesRecursive(input, materialNameToAssetId, *assimpScene, *assimpScene->mRootNode, numberOfVertices, numberOfIndices, subMeshes);
				if (subMeshes.size() > std::numeric_limits<uint16_t>::max())
				{
					throw std::runtime_error("The maximum number of supported sub-meshes is " + std::to_string(std::numeric_limits<uint16_t>::max()));
				}
				const Renderer::IndexBufferFormat::Enum indexBufferFormat = (numberOfVertices > std::numeric_limits<uint16_t>::max()) ? Renderer::IndexBufferFormat::UNSIGNED_INT : Renderer::IndexBufferFormat::UNSIGNED_SHORT;

				// Is there an optional skeleton?
				const Renderer::VertexAttributes& vertexAttributes = (numberOfBones > 0) ? RendererRuntime::MeshResource::SKINNED_VERTEX_ATTRIBUTES : RendererRuntime::MeshResource::VERTEX_ATTRIBUTES;
				const uint8_t numberOfBytesPerVertex = (numberOfBones > 0) ? ::detail::NUMBER_OF_BYTES_PER_SKINNED_VERTEX : ::detail::NUMBER_OF_BYTES_PER_VERTEX;

				{ // Write down the mesh header
					RendererRuntime::v1Mesh::MeshHeader meshHeader;
					meshHeader.numberOfBytesPerVertex	= numberOfBytesPerVertex;
					meshHeader.numberOfVertices			= numberOfVertices;
					meshHeader.indexBufferFormat		= static_cast<uint8_t>(indexBufferFormat);
					meshHeader.numberOfIndices			= numberOfIndices;
					meshHeader.numberOfVertexAttributes = static_cast<uint8_t>(vertexAttributes.numberOfAttributes);
					meshHeader.numberOfSubMeshes		= static_cast<uint16_t>(subMeshes.size());
					meshHeader.numberOfBones			= skeleton.numberOfBones;
					memoryFile.write(&meshHeader, sizeof(RendererRuntime::v1Mesh::MeshHeader));
				}

				{ // Vertex and index buffer data
					// Allocate memory for the local vertex and index buffer data
					// -> Do also initialize the vertex buffer data with zero to handle not filled vertex bone weights
					uint8_t* vertexBufferData = new uint8_t[numberOfBytesPerVertex * numberOfVertices];
					memset(vertexBufferData, 0, numberOfBytesPerVertex * numberOfVertices);
					uint32_t* indexBufferData = new uint32_t[numberOfIndices];

					{ // Fill the mesh data recursively
						uint32_t numberOfFilledVertices = 0;
						uint32_t numberOfFilledIndices  = 0;
						::detail::fillMeshRecursive(*assimpScene, *assimpScene->mRootNode, skeleton, numberOfBytesPerVertex, vertexBufferData, indexBufferData, aiMatrix4x4(), numberOfFilledVertices, numberOfFilledIndices);
						if (numberOfVertices != numberOfFilledVertices || numberOfIndices != numberOfFilledIndices)
						{
							throw std::runtime_error("Error while recursively filling the mesh data");
						}
					}

					// Write down the vertex and index buffer
					memoryFile.write(vertexBufferData, numberOfBytesPerVertex * numberOfVertices);
					if (Renderer::IndexBufferFormat::UNSIGNED_INT == indexBufferFormat)
					{
						// Dump the 32-bit indices we have in memory
						memoryFile.write(indexBufferData, sizeof(uint32_t) * numberOfIndices);
					}
					else
					{
						// Convert the 32-bit indices we have in memory to 16-bit indices
						uint16_t* shortIndexBufferData = new uint16_t[numberOfIndices];
						for (uint32_t i = 0; i < numberOfIndices; ++i)
						{
							shortIndexBufferData[i] = static_cast<uint16_t>(indexBufferData[i]);
						}
						memoryFile.write(shortIndexBufferData, sizeof(uint16_t) * numberOfIndices);
						delete [] shortIndexBufferData;
					}

					// Destroy local vertex and input buffer data
					delete [] vertexBufferData;
					delete [] indexBufferData;
				}

				// Write down the vertex array attributes
				memoryFile.write(vertexAttributes.attributes, sizeof(Renderer::VertexAttribute) * vertexAttributes.numberOfAttributes);

				// Write down the sub-meshes
				memoryFile.write(subMeshes.data(), sizeof(RendererRuntime::v1Mesh::SubMesh) * subMeshes.size());

				// Write down the optional skeleton
				if (skeleton.numberOfBones > 0)
				{
					const aiMatrix4x4& assimpRootTransformation = assimpScene->mRootNode->mTransformation.Inverse();
					for (uint8_t i = 0; i < skeleton.numberOfBones; ++i)
					{
						// Some Assimp importers like the MD5 one compensate coordinate system differences by setting a root node transform, so we need to take this into account
						skeleton.boneOffsetMatrices[i] = skeleton.boneOffsetMatrices[i] * assimpRootTransformation;

						// Take care of row/column major flipping
						skeleton.localBoneMatrices[i].Transpose();
						skeleton.boneOffsetMatrices[i].Transpose();
					}
					memoryFile.write(skeleton.getSkeletonData(), skeleton.getNumberOfSkeletonDataBytes());
				}
			}
			else
			{
				throw std::runtime_error("Assimp failed to load in the given mesh: " + assimpLogStream.getLastErrorMessage());
			}

			// Write LZ4 compressed output
			memoryFile.writeLz4CompressedDataToFile(RendererRuntime::v1Mesh::FORMAT_TYPE, RendererRuntime::v1Mesh::FORMAT_VERSION, outputAssetFilename, input.context.getFileManager());

			// Store new cache entries or update existing ones
			input.cacheManager.storeOrUpdateCacheEntriesInDatabase(cacheEntries);

			// Shutdown Assimp logging
			Assimp::DefaultLogger::get()->detatchStream(&assimpLogStream, Assimp::DefaultLogger::Err);
			Assimp::DefaultLogger::kill();
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/Mesh/" + assetCategory + '/' + assetName;
			outputAsset(assetIdAsString, outputAssetFilename, outputAssetPackage);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
