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
#include "RendererRuntime/Vr/OpenVR/Loader/OpenVRTextureResourceLoader.h"
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
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const uint32_t NUMBER_OF_BYTES_PER_VERTEX = sizeof(float) * 3 + sizeof(float) * 2 + sizeof(short) * 4;


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		RendererRuntime::AssetId setupRenderModelDiffuseTexture(const RendererRuntime::IRendererRuntime& rendererRuntime, const vr::RenderModel_t& vrRenderModel)
		{
			// Check whether or not we need to generate the runtime mesh asset right now
			RendererRuntime::AssetId assetId = RendererRuntime::VrManagerOpenVR::diffuseTextureIdToAssetId(vrRenderModel.diffuseTextureId);
			RendererRuntime::TextureResourceId textureResourceId = RendererRuntime::getUninitialized<RendererRuntime::TextureResourceId>();
			const bool rgbHardwareGammaCorrection = true;	// TODO(co) It must be possible to set the property name from the outside: Ask the material blueprint whether or not hardware gamma correction should be used
			rendererRuntime.getTextureResourceManager().loadTextureResourceByAssetId(assetId, "Unrimp/Texture/DynamicByCode/IdentityDiffuseMap2D", textureResourceId, nullptr, rgbHardwareGammaCorrection, false, RendererRuntime::OpenVRTextureResourceLoader::TYPE_ID);

			// Done
			return assetId;
		}

		RendererRuntime::MaterialResourceId setupRenderModelMaterial(const RendererRuntime::IRendererRuntime& rendererRuntime, RendererRuntime::MaterialResourceId vrDeviceMaterialResourceId, vr::TextureID_t vrTextureId, RendererRuntime::AssetId diffuseTextureAssetId)
		{
			// Get the texture name and convert it into an runtime material asset ID
			const std::string materialName = "OpenVR_" + std::to_string(vrTextureId);
			RendererRuntime::AssetId materialAssetId = RendererRuntime::StringId(materialName.c_str());

			// Check whether or not we need to generate the runtime material asset right now
			RendererRuntime::MaterialResourceManager& materialResourceManager = rendererRuntime.getMaterialResourceManager();
			RendererRuntime::MaterialResourceId materialResourceId = materialResourceManager.getMaterialResourceIdByAssetId(materialAssetId);
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
		// Load the render model
		const std::string& renderModelName = getRenderModelName();
		vr::IVRRenderModels* vrRenderModels = vr::VRRenderModels();
		vr::EVRRenderModelError vrRenderModelError = vr::VRRenderModelError_Loading;
		while (vrRenderModelError == vr::VRRenderModelError_Loading)
		{
			vrRenderModelError = vrRenderModels->LoadRenderModel_Async(renderModelName.c_str(), &mVrRenderModel);
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

		{ // Get the vertex buffer and index buffer data
			// Tell the mesh resource about the number of vertices and indices
			const uint32_t numberOfVertices = mVrRenderModel->unVertexCount;
			const uint32_t numberOfIndices = mVrRenderModel->unTriangleCount * 3;
			mMeshResource->setNumberOfVertices(numberOfVertices);
			mMeshResource->setNumberOfIndices(numberOfIndices);

			{ // Fill the vertex buffer data
				const uint32_t numberOfBytes = numberOfVertices * ::detail::NUMBER_OF_BYTES_PER_VERTEX;
				mVertexBufferData.resize(numberOfBytes);
				uint8_t* currentVertexBufferData = mVertexBufferData.data();
				const vr::RenderModel_Vertex_t* currentVrRenderModelVertex = mVrRenderModel->rVertexData;
				mTangentsData.resize(mVrRenderModel->unVertexCount);
				calculateTangentArrayOfRenderModel();
				for (uint32_t i = 0; i < numberOfVertices; ++i, ++currentVrRenderModelVertex)
				{
					{ // 32 bit position
						float* position = reinterpret_cast<float*>(currentVertexBufferData);
						position[0] = currentVrRenderModelVertex->vPosition.v[0];
						position[1] = currentVrRenderModelVertex->vPosition.v[1];
						position[2] = -currentVrRenderModelVertex->vPosition.v[2];
						currentVertexBufferData += sizeof(float) * 3;
					}

					{ // 32 bit texture coordinate
						float* textureCoordinate = reinterpret_cast<float*>(currentVertexBufferData);
						textureCoordinate[0] = currentVrRenderModelVertex->rfTextureCoord[0];
						textureCoordinate[1] = currentVrRenderModelVertex->rfTextureCoord[1];
						currentVertexBufferData += sizeof(float) * 2;
					}

					{ // 16 bit QTangent
						// Get the mesh vertex normal, tangent and binormal
						const glm::vec3 normal(currentVrRenderModelVertex->vNormal.v[0], currentVrRenderModelVertex->vNormal.v[1], currentVrRenderModelVertex->vNormal.v[2]);
						const glm::vec4& tangent = mTangentsData[i];
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
						const glm::quat tangentFrameQuaternion = Math::calculateTangentFrameQuaternion(tangentFrame);

						// Set our vertex buffer 16 bit QTangent
						short* qTangent = reinterpret_cast<short*>(currentVertexBufferData);
						qTangent[0] = static_cast<short>(tangentFrameQuaternion.x * SHRT_MAX);
						qTangent[1] = static_cast<short>(tangentFrameQuaternion.y * SHRT_MAX);
						qTangent[2] = static_cast<short>(tangentFrameQuaternion.z * SHRT_MAX);
						qTangent[3] = static_cast<short>(tangentFrameQuaternion.w * SHRT_MAX);
						currentVertexBufferData += sizeof(short) * 4;
					}
				}
			}

			{ // Fill the index buffer data
			  // -> We need to flip the vertex winding so we don't need to modify rasterizer states
				mIndexBufferData.resize(numberOfIndices);
				uint16_t* currentIndexBufferData = mIndexBufferData.data();
				for (uint32_t i = 0; i < mVrRenderModel->unTriangleCount; ++i, currentIndexBufferData += 3)
				{
					const uint32_t offset = i * 3;
					currentIndexBufferData[0] = mVrRenderModel->rIndexData[offset + 2];
					currentIndexBufferData[1] = mVrRenderModel->rIndexData[offset + 1];
					currentIndexBufferData[2] = mVrRenderModel->rIndexData[offset + 0];
				}
			}
		}

		// Can we create the renderer resource asynchronous as well?
		if (mRendererRuntime.getRenderer().getCapabilities().nativeMultiThreading)
		{
			mVertexArray = createVertexArray();
		}
	}

	bool OpenVRMeshResourceLoader::onDispatch()
	{
		// Create vertex array object (VAO)
		mMeshResource->setVertexArray(mRendererRuntime.getRenderer().getCapabilities().nativeMultiThreading ? mVertexArray : createVertexArray());

		{ // Create sub-meshes
			// Load the render model texture and setup the material asset
			// -> We don't care if loading of the diffuse texture fails in here, isn't that important and the show must go on
			const AssetId diffuseTextureAssetId = ::detail::setupRenderModelDiffuseTexture(mRendererRuntime, *mVrRenderModel);
			const MaterialResourceId materialResourceId = ::detail::setupRenderModelMaterial(mRendererRuntime, static_cast<const VrManagerOpenVR&>(mRendererRuntime.getVrManager()).getVrDeviceMaterialResourceId(), mVrRenderModel->diffuseTextureId, diffuseTextureAssetId);

			// Tell the mesh resource about the sub-mesh
			mMeshResource->getSubMeshes().push_back(SubMesh(materialResourceId, Renderer::PrimitiveTopology::TRIANGLE_LIST, 0, mMeshResource->getNumberOfIndices()));
		}

		// Free the render model
		if (nullptr != mVrRenderModel)
		{
			vr::VRRenderModels()->FreeRenderModel(mVrRenderModel);
			mVrRenderModel = nullptr;
		}

		// Fully loaded?
		return true;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void OpenVRMeshResourceLoader::calculateTangentArrayOfRenderModel()
	{
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
		const uint32_t numberOfTriangles = mVrRenderModel->unTriangleCount;
		const uint32_t numberOfVertices = mVrRenderModel->unVertexCount;
		mTemporaryTangentsData.resize(numberOfVertices * 2);
		glm::vec3* tan1 = mTemporaryTangentsData.data();
		glm::vec3* tan2 = mTemporaryTangentsData.data() + numberOfVertices;
		memset(tan1, 0, numberOfVertices * sizeof(glm::vec3) * 2);
		const uint16_t* currentIndex = mVrRenderModel->rIndexData;
		const vr::RenderModel_Vertex_t* vrRenderModelVertex = mVrRenderModel->rVertexData;
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
		glm::vec4* tangents = mTangentsData.data();
		for (uint32_t i = 0; i < numberOfVertices; ++i)
		{
			const vr::HmdVector3_t& vrHmdVector3 = vrRenderModelVertex[i].vNormal;
			const glm::vec3 n(vrHmdVector3.v[0], vrHmdVector3.v[1], -vrHmdVector3.v[2]);
			const glm::vec3& t = tan1[i];

			// Gram-Schmidt orthogonalize and calculate handedness
			tangents[i] = glm::vec4(glm::normalize(t - n * glm::dot(n, t)), (glm::dot(glm::cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f);
		}
	}

	Renderer::IVertexArray* OpenVRMeshResourceLoader::createVertexArray() const
	{
		Renderer::IBufferManager& bufferManager = mRendererRuntime.getBufferManager();

		// Create the vertex buffer
		Renderer::IVertexBuffer* vertexBuffer = bufferManager.createVertexBuffer(static_cast<uint32_t>(mVertexBufferData.size()), mVertexBufferData.data(), Renderer::BufferUsage::STATIC_DRAW);
		RENDERER_SET_RESOURCE_DEBUG_NAME(vertexBuffer, getRenderModelName().c_str())

		// Create the index buffer
		Renderer::IIndexBuffer* indexBuffer = bufferManager.createIndexBuffer(static_cast<uint32_t>(mIndexBufferData.size() * sizeof(uint16_t)), Renderer::IndexBufferFormat::UNSIGNED_SHORT, mIndexBufferData.data(), Renderer::BufferUsage::STATIC_DRAW);
		RENDERER_SET_RESOURCE_DEBUG_NAME(indexBuffer, getRenderModelName().c_str())

		// Create vertex array object (VAO)
		const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
		{
			{ // Vertex buffer 0
				vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
				::detail::NUMBER_OF_BYTES_PER_VERTEX	// strideInBytes (uint32_t)
			}
		};
		Renderer::IVertexArray* vertexArray = bufferManager.createVertexArray(MeshResource::VERTEX_ATTRIBUTES, static_cast<uint32_t>(glm::countof(vertexArrayVertexBuffers)), vertexArrayVertexBuffers, indexBuffer);
		RENDERER_SET_RESOURCE_DEBUG_NAME(vertexArray, getRenderModelName().c_str())

		// Done
		return vertexArray;
	}

	const std::string& OpenVRMeshResourceLoader::getRenderModelName() const
	{
		// OpenVR render model names can get awful long due to absolute path information, so, we need to store them inside a separate list and tell the asset just about the render model name index
		const VrManagerOpenVR::RenderModelNames& renderModelNames = static_cast<const VrManagerOpenVR&>(mRendererRuntime.getVrManager()).getRenderModelNames();
		const uint32_t renderModelNameIndex = static_cast<uint32_t>(std::atoi(getAsset().assetFilename));
		assert(renderModelNameIndex < static_cast<uint32_t>(renderModelNames.size()));
		return renderModelNames[renderModelNameIndex];
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
