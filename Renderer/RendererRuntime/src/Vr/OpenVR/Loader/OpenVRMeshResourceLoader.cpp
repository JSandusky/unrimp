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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Vr/OpenVR/Loader/OpenVRMeshResourceLoader.h"
#include "RendererRuntime/Resource/Mesh/MeshResource.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Vr/OpenVR/VrManagerOpenVR.h"
#include "RendererRuntime/Core/Math/Math.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <openvr/openvr.h>

#include <chrono>
#include <thread>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		// Lengyel, Eric. "Computing Tangent Space Basis Vectors for an Arbitrary Mesh". Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html
		// -> Copy found at: https://fenix.tecnico.ulisboa.pt/downloadFile/845043405449073/Tangent%20Space%20Calculation.pdf
		// -> Don't remove this commented code style adjusted copy below, it's the base of an adjusted function and we should be able to have the original as reference
		/*
		struct Triangle
		{
			uint16_t index[3];
		};
		void calculateTangentArray(uint32_t numberOfVertices, const glm::vec3* positions, const glm::vec3* normals, const glm::vec2* textureCoordinates, uint32_t numberOfTriangles, const Triangle* triangles, glm::vec4* tangents)
		{
			glm::vec3* tan1 = new glm::vec3[numberOfVertices * 2];
			glm::vec3* tan2 = tan1 + numberOfVertices;
			memset(tan1, 0, numberOfVertices * sizeof(glm::vec3) * 2);
			for (uint32_t i = 0; i < numberOfTriangles; ++i)
			{
				const uint16_t i1 = triangles->index[0];
				const uint16_t i2 = triangles->index[1];
				const uint16_t i3 = triangles->index[2];
				const glm::vec3& v1 = positions[i1];
				const glm::vec3& v2 = positions[i2];
				const glm::vec3& v3 = positions[i3];
				const glm::vec2& w1 = textureCoordinates[i1];
				const glm::vec2& w2 = textureCoordinates[i2];
				const glm::vec2& w3 = textureCoordinates[i3];
				const float x1 = v2.x - v1.x;
				const float x2 = v3.x - v1.x;
				const float y1 = v2.y - v1.y;
				const float y2 = v3.y - v1.y;
				const float z1 = v2.z - v1.z;
				const float z2 = v3.z - v1.z;
				const float s1 = w2.x - w1.x;
				const float s2 = w3.x - w1.x;
				const float t1 = w2.y - w1.y;
				const float t2 = w3.y - w1.y;
				const float r = 1.0f / (s1 * t2 - s2 * t1);
				const glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				const glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);
				tan1[i1] += sdir;
				tan1[i2] += sdir;
				tan1[i3] += sdir;
				tan2[i1] += tdir;
				tan2[i2] += tdir;
				tan2[i3] += tdir;
				++triangles;
			}
			for (uint32_t i = 0; i < numberOfVertices; ++i)
			{
				const glm::vec3& n = normals[i];
				const glm::vec3& t = tan1[i];

				// Gram-Schmidt orthogonalize and calculate handedness
				tangents[i] = glm::vec4(glm::normalize(t - n * glm::dot(n, t)), (glm::dot(glm::cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f);
			}
			delete [] tan1;
		}
		*/
		// Function of "Computing Tangent Space Basis Vectors for an Arbitrary Mesh" adjusted to "vr::RenderModel_t" to not need to move around too much data just for computation
		void calculateTangentArrayOfRenderModel(vr::RenderModel_t& vrRenderModel, glm::vec4* tangents)
		{
			const uint32_t numberOfTriangles = vrRenderModel.unTriangleCount;
			const uint32_t numberOfVertices = vrRenderModel.unVertexCount;
			glm::vec3* tan1 = new glm::vec3[numberOfVertices * 2];
			glm::vec3* tan2 = tan1 + numberOfVertices;
			memset(tan1, 0, numberOfVertices * sizeof(glm::vec3) * 2);
			const uint16_t* currentIndex = vrRenderModel.rIndexData;
			const vr::RenderModel_Vertex_t* vrRenderModelVertex = vrRenderModel.rVertexData;
			for (uint32_t i = 0; i < numberOfTriangles; ++i, currentIndex += 3)
			{
				const uint16_t i1 = currentIndex[0];
				const uint16_t i2 = currentIndex[1];
				const uint16_t i3 = currentIndex[2];
				const vr::HmdVector3_t& v1 = vrRenderModelVertex[i1].vPosition;
				const vr::HmdVector3_t& v2 = vrRenderModelVertex[i2].vPosition;
				const vr::HmdVector3_t& v3 = vrRenderModelVertex[i3].vPosition;
				const float* w1 = vrRenderModelVertex[i1].rfTextureCoord;
				const float* w2 = vrRenderModelVertex[i2].rfTextureCoord;
				const float* w3 = vrRenderModelVertex[i3].rfTextureCoord;
				const float x1 = v2.v[0] - v1.v[0];
				const float x2 = v3.v[0] - v1.v[0];
				const float y1 = v2.v[1] - v1.v[1];
				const float y2 = v3.v[1] - v1.v[1];
				const float z1 = -v2.v[2] - -v1.v[2];
				const float z2 = -v3.v[2] - -v1.v[2];
				const float s1 = w2[0] - w1[0];
				const float s2 = w3[0] - w1[0];
				const float t1 = w2[1] - w1[1];
				const float t2 = w3[1] - w1[1];
				const float r = 1.0f / (s1 * t2 - s2 * t1);
				const glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				const glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);
				tan1[i1] += sdir;
				tan1[i2] += sdir;
				tan1[i3] += sdir;
				tan2[i1] += tdir;
				tan2[i2] += tdir;
				tan2[i3] += tdir;
			}
			for (uint32_t i = 0; i < numberOfVertices; ++i)
			{
				const vr::HmdVector3_t& vrHmdVector3 = vrRenderModelVertex[i].vNormal;
				const glm::vec3 n(vrHmdVector3.v[0], vrHmdVector3.v[1], -vrHmdVector3.v[2]);
				const glm::vec3& t = tan1[i];

				// Gram-Schmidt orthogonalize and calculate handedness
				tangents[i] = glm::vec4(glm::normalize(t - n * glm::dot(n, t)), (glm::dot(glm::cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f);
			}
			delete [] tan1;
		}

		RendererRuntime::AssetId setupRenderModelDiffuseTexture(const RendererRuntime::IRendererRuntime& rendererRuntime, const std::string& renderModelName, const vr::RenderModel_t& vrRenderModel)
		{
			// Get the texture name and convert it into an runtime texture asset ID
			const std::string diffuseTextureName = "OpenVR_" + std::to_string(vrRenderModel.diffuseTextureId);
			RendererRuntime::AssetId diffuseTextureAssetId = RendererRuntime::StringId(diffuseTextureName.c_str());

			// Check whether or not we need to generate the runtime texture asset right now
			RendererRuntime::TextureResourceManager& textureResourceManager = rendererRuntime.getTextureResourceManager();
			const bool rgbHardwareGammaCorrection = true;	// TODO(co) It must be possible to set the property name from the outside: Ask the material blueprint whether or not hardware gamma correction should be used
			RendererRuntime::TextureResourceId textureResourceId = RendererRuntime::getUninitialized<RendererRuntime::TextureResourceId>();
			textureResourceManager.loadTextureResourceByAssetId(diffuseTextureAssetId, RendererRuntime::getUninitialized<RendererRuntime::AssetId>(), textureResourceId, nullptr, rgbHardwareGammaCorrection);
			if (RendererRuntime::isUninitialized(textureResourceId))
			{
				// Load the render model texture
				vr::IVRRenderModels* vrRenderModels = vr::VRRenderModels();
				vr::RenderModel_TextureMap_t* vrRenderModelTextureMap = nullptr;
				vr::EVRRenderModelError vrRenderModelError = vr::VRRenderModelError_Loading;
				while (vrRenderModelError == vr::VRRenderModelError_Loading)
				{
					vrRenderModelError = vrRenderModels->LoadTexture_Async(vrRenderModel.diffuseTextureId, &vrRenderModelTextureMap);
					if (vrRenderModelError == vr::VRRenderModelError_Loading)
					{
						using namespace std::chrono_literals;
						std::this_thread::sleep_for(1ms);
					}
				}
				if (vr::VRRenderModelError_None != vrRenderModelError)
				{
					RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF("Error: Unable to load OpenVR diffuse texture %d of render model \"%s\": %s", vrRenderModel.diffuseTextureId, renderModelName.c_str(), vrRenderModels->GetRenderModelErrorNameFromEnum(vrRenderModelError));
					std::ignore = renderModelName;
					return RendererRuntime::getUninitialized<RendererRuntime::AssetId>();
				}

				// Create the renderer texture instance
				Renderer::ITexture2D* texture2D = rendererRuntime.getTextureManager().createTexture2D(vrRenderModelTextureMap->unWidth, vrRenderModelTextureMap->unHeight, rgbHardwareGammaCorrection ? Renderer::TextureFormat::R8G8B8A8_SRGB : Renderer::TextureFormat::R8G8B8A8, static_cast<const void*>(vrRenderModelTextureMap->rubTextureMapData), Renderer::TextureFlag::GENERATE_MIPMAPS);
				RENDERER_SET_RESOURCE_DEBUG_NAME(texture2D, diffuseTextureName.c_str())

				// We need to generate the runtime texture asset right now
				// -> Takes over the given 2D texture
				textureResourceId = textureResourceManager.createTextureResourceByAssetId(diffuseTextureAssetId, *texture2D, rgbHardwareGammaCorrection);

				// Free the render model texture
				vrRenderModels->FreeTexture(vrRenderModelTextureMap);
			}

			// Done
			return diffuseTextureAssetId;
		}

		RendererRuntime::MaterialResourceId setupRenderModelMaterial(const RendererRuntime::IRendererRuntime& rendererRuntime, RendererRuntime::MaterialResourceId vrDeviceMaterialResourceId, vr::TextureID_t vrTextureId, RendererRuntime::AssetId diffuseTextureAssetId)
		{
			// Get the texture name and convert it into an runtime material asset ID
			const std::string materialName = "OpenVR_" + std::to_string(vrTextureId);
			RendererRuntime::AssetId materialAssetId = RendererRuntime::StringId(materialName.c_str());

			// Check whether or not we need to generate the runtime material asset right now
			RendererRuntime::MaterialResourceManager& materialResourceManager = rendererRuntime.getMaterialResourceManager();
			RendererRuntime::MaterialResourceId materialResourceId = RendererRuntime::getUninitialized<RendererRuntime::MaterialResourceId>();
			materialResourceManager.loadMaterialResourceByAssetId(materialAssetId, materialResourceId);
			if (RendererRuntime::isUninitialized(materialResourceId))
			{
				// We need to generate the runtime material asset right now
				materialResourceId = materialResourceManager.createMaterialResourceByCloning(vrDeviceMaterialResourceId, materialAssetId);
				if (RendererRuntime::isInitialized(materialResourceId))
				{
					RendererRuntime::MaterialResource* materialResource = materialResourceManager.tryGetById(materialResourceId);
					if (nullptr != materialResource)
					{
						// TODO(co) It must be possible to set the property name from the outside
						materialResource->setPropertyById("DiffuseMap", RendererRuntime::MaterialPropertyValue::fromTextureAssetId(diffuseTextureAssetId));
						materialResource->setPropertyById("UseDiffuseMap", RendererRuntime::MaterialPropertyValue::fromBoolean(true));
					}
				}
			}

			// Done
			return materialResourceId;
		}

		void setupRenderModel(const RendererRuntime::IRendererRuntime& rendererRuntime, const std::string& renderModelName, RendererRuntime::MeshResource& meshResource, RendererRuntime::MaterialResourceId vrDeviceMaterialResourceId)
		{
			vr::IVRRenderModels* vrRenderModels = vr::VRRenderModels();

			// Load the render model
			vr::RenderModel_t* vrRenderModel = nullptr;
			vr::EVRRenderModelError vrRenderModelError = vr::VRRenderModelError_Loading;
			while (vrRenderModelError == vr::VRRenderModelError_Loading)
			{
				vrRenderModelError = vrRenderModels->LoadRenderModel_Async(renderModelName.c_str(), &vrRenderModel);
				if (vrRenderModelError == vr::VRRenderModelError_Loading)
				{
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(1ms);
				}
			}
			if (vr::VRRenderModelError_None != vrRenderModelError)
			{
				RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF("Error: Unable to load OpenVR render model \"%s\": %s", renderModelName.c_str(), vrRenderModels->GetRenderModelErrorNameFromEnum(vrRenderModelError));
				return;
			}

			// Load the render model texture
			// -> We don't care if loading of the diffuse texture fails in here, isn't that important and the show must go on
			const RendererRuntime::AssetId diffuseTextureAssetId = setupRenderModelDiffuseTexture(rendererRuntime, renderModelName, *vrRenderModel);

			// Setup the material asset
			const RendererRuntime::MaterialResourceId materialResourceId = setupRenderModelMaterial(rendererRuntime, vrDeviceMaterialResourceId, vrRenderModel->diffuseTextureId, diffuseTextureAssetId);

			{ // Fill the mesh resource
				// Tell the mesh resource about the number of vertices and indices
				const uint32_t numberOfVertices = vrRenderModel->unVertexCount;
				const uint32_t numberOfIndices = vrRenderModel->unTriangleCount * 3;
				meshResource.setNumberOfVertices(numberOfVertices);
				meshResource.setNumberOfIndices(numberOfIndices);

				{ // Tell the mesh resource about the vertex array
					Renderer::IBufferManager& bufferManager = rendererRuntime.getBufferManager();

					// Create the vertex buffer
					Renderer::IVertexBuffer* vertexBuffer = nullptr;
					const uint32_t numberOfBytesPerVertex = sizeof(float) * 3 + sizeof(float) * 2 + sizeof(short) * 4;
					{
						const uint32_t numberOfBytes = numberOfVertices * numberOfBytesPerVertex;
						uint8_t* temp = new uint8_t[numberOfBytes];
						uint8_t* currentTemp = temp;
						const vr::RenderModel_Vertex_t* currentVrRenderModelVertex = vrRenderModel->rVertexData;
						glm::vec4* tangents = new glm::vec4[vrRenderModel->unVertexCount];
						::detail::calculateTangentArrayOfRenderModel(*vrRenderModel, tangents);
						for (uint32_t i = 0; i < numberOfVertices; ++i, ++currentVrRenderModelVertex)
						{
							{ // 32 bit position
								float* position = reinterpret_cast<float*>(currentTemp);
								position[0] = currentVrRenderModelVertex->vPosition.v[0];
								position[1] = currentVrRenderModelVertex->vPosition.v[1];
								position[2] = -currentVrRenderModelVertex->vPosition.v[2];
								currentTemp += sizeof(float) * 3;
							}

							{ // 32 bit texture coordinate
								float* textureCoordinate = reinterpret_cast<float*>(currentTemp);
								textureCoordinate[0] = currentVrRenderModelVertex->rfTextureCoord[0];
								textureCoordinate[1] = currentVrRenderModelVertex->rfTextureCoord[1];
								currentTemp += sizeof(float) * 2;
							}

							{ // 16 bit QTangent
								// Get the mesh vertex normal, tangent and binormal
								const glm::vec3 normal(currentVrRenderModelVertex->vNormal.v[0], currentVrRenderModelVertex->vNormal.v[1], currentVrRenderModelVertex->vNormal.v[2]);
								const glm::vec4& tangent = tangents[i];
								glm::vec3 binormal = glm::cross(normal, glm::vec3(tangent));
								if (tangent.w < 0.0f)
								{
									binormal = -binormal;
								}

								// Generate tangent frame rotation matrix
								glm::mat3 tangentFrame(
									tangent.x,  tangent.y,  tangent.z,
									binormal.x, binormal.y, binormal.z,
									normal.x,   normal.y,   normal.z
								);

								// Calculate tangent frame quaternion
								const glm::quat tangentFrameQuaternion = RendererRuntime::Math::calculateTangentFrameQuaternion(tangentFrame);

								// Set our vertex buffer 16 bit QTangent
								short* qTangent = reinterpret_cast<short*>(currentTemp);
								qTangent[0] = static_cast<short>(tangentFrameQuaternion.x * SHRT_MAX);
								qTangent[1] = static_cast<short>(tangentFrameQuaternion.y * SHRT_MAX);
								qTangent[2] = static_cast<short>(tangentFrameQuaternion.z * SHRT_MAX);
								qTangent[3] = static_cast<short>(tangentFrameQuaternion.w * SHRT_MAX);
								currentTemp += sizeof(short) * 4;
							}
						}
						vertexBuffer = bufferManager.createVertexBuffer(numberOfBytes, temp, Renderer::BufferUsage::STATIC_DRAW);
						RENDERER_SET_RESOURCE_DEBUG_NAME(vertexBuffer, renderModelName.c_str())
						delete [] temp;
						delete [] tangents;
					}

					// Create the index buffer
					// -> We need to flip the vertex winding so we don't need to modify rasterizer states
					Renderer::IIndexBuffer* indexBuffer = nullptr;
					{
						uint16_t* temp = new uint16_t[numberOfIndices];
						uint16_t* currentTemp = temp;
						for (uint32_t i = 0; i < vrRenderModel->unTriangleCount; ++i, currentTemp += 3)
						{
							const uint32_t offset = i * 3;
							currentTemp[0] = vrRenderModel->rIndexData[offset + 2];
							currentTemp[1] = vrRenderModel->rIndexData[offset + 1];
							currentTemp[2] = vrRenderModel->rIndexData[offset + 0];
						}
						indexBuffer = bufferManager.createIndexBuffer(numberOfIndices * sizeof(uint16_t), Renderer::IndexBufferFormat::UNSIGNED_SHORT, temp, Renderer::BufferUsage::STATIC_DRAW);
						RENDERER_SET_RESOURCE_DEBUG_NAME(indexBuffer, renderModelName.c_str())
						delete [] temp;
					}

					{ // Create vertex array object (VAO)
						const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
						{
							{ // Vertex buffer 0
								vertexBuffer,			// vertexBuffer (Renderer::IVertexBuffer *)
								numberOfBytesPerVertex	// strideInBytes (uint32_t)
							}
						};
						Renderer::IVertexArray* vertexArray = bufferManager.createVertexArray(RendererRuntime::MeshResource::VERTEX_ATTRIBUTES, static_cast<uint32_t>(glm::countof(vertexArrayVertexBuffers)), vertexArrayVertexBuffers, indexBuffer);
						meshResource.setVertexArray(vertexArray);
						RENDERER_SET_RESOURCE_DEBUG_NAME(vertexArray, renderModelName.c_str())
					}
				}

				// Tell the mesh resource about the sub-mesh
				meshResource.getSubMeshes().push_back(RendererRuntime::SubMesh(materialResourceId, Renderer::PrimitiveTopology::TRIANGLE_LIST, 0, meshResource.getNumberOfIndices()));
			}

			// Free the render model
			vrRenderModels->FreeRenderModel(vrRenderModel);
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId OpenVRMeshResourceLoader::TYPE_ID("openvr_mesh");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void OpenVRMeshResourceLoader::onProcessing()
	{
		// TODO(co) Implement me
		NOP;
	}

	bool OpenVRMeshResourceLoader::onDispatch()
	{
		// TODO(co) Implement asyncrounous render model loading

		// OpenVR render model names can get awful long due to absolute path information, so, we need to store them inside a separate list and tell the asset just about the render model name index
		const VrManagerOpenVR& vrManagerOpenVR = static_cast<const VrManagerOpenVR&>(mRendererRuntime.getVrManager());
		const VrManagerOpenVR::RenderModelNames& renderModelNames = vrManagerOpenVR.getRenderModelNames();
		const uint32_t renderModelNameIndex = static_cast<uint32_t>(std::atoi(getAsset().assetFilename));
		assert(renderModelNameIndex < static_cast<uint32_t>(renderModelNames.size()));
		::detail::setupRenderModel(mRendererRuntime, renderModelNames[renderModelNameIndex], *mMeshResource, vrManagerOpenVR.getVrDeviceMaterialResourceId());

		// Fully loaded?
		return true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
