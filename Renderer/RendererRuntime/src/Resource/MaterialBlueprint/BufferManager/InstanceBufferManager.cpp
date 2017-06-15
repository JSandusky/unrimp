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
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/InstanceBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/MaterialBlueprintResourceListener.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/Skeleton/SkeletonResourceManager.h"
#include "RendererRuntime/Resource/Skeleton/SkeletonResource.h"
#include "RendererRuntime/RenderQueue/Renderable.h"
#include "RendererRuntime/RenderQueue/RenderableManager.h"
#include "RendererRuntime/Core/Math/Transform.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <algorithm>


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
		static uint32_t DEFAULT_UNIFORM_BUFFER_NUMBER_OF_BYTES = 64 * 1024;		// 64 KiB
		static uint32_t DEFAULT_TEXTURE_BUFFER_NUMBER_OF_BYTES = 512 * 1024;	// 512 KiB


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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	InstanceBufferManager::InstanceBufferManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mStartInstanceLocation(0),
		mUniformBuffer(nullptr),
		mTextureBuffer(nullptr),
		mMapped(false),
		mStartUniformBufferPointer(nullptr),
		mCurrentUniformBufferPointer(nullptr),
		mStartTextureBufferPointer(nullptr),
		mCurrentTextureBufferPointer(nullptr)
	{
		Renderer::IBufferManager& bufferManager = rendererRuntime.getBufferManager();

		// Create uniform buffer instance
		const uint32_t maximumUniformBufferSize = std::min(rendererRuntime.getRenderer().getCapabilities().maximumUniformBufferSize, ::detail::DEFAULT_UNIFORM_BUFFER_NUMBER_OF_BYTES);
		mUniformBuffer = bufferManager.createUniformBuffer(maximumUniformBufferSize, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
		RENDERER_SET_RESOURCE_DEBUG_NAME(mUniformBuffer, "Instance buffer manager")

		// Create texture buffer instance
		const uint32_t maximumTextureBufferSize = std::min(rendererRuntime.getRenderer().getCapabilities().maximumTextureBufferSize, ::detail::DEFAULT_TEXTURE_BUFFER_NUMBER_OF_BYTES);
		mTextureBuffer = bufferManager.createTextureBuffer(maximumTextureBufferSize, Renderer::TextureFormat::R32G32B32A32F, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
		RENDERER_SET_RESOURCE_DEBUG_NAME(mTextureBuffer, "Instance buffer manager")
	}

	InstanceBufferManager::~InstanceBufferManager()
	{
		// Release uniform and texture buffer instances
		mUniformBuffer->releaseReference();
		mTextureBuffer->releaseReference();
	}

	uint32_t InstanceBufferManager::fillBuffer(PassBufferManager* passBufferManager, const MaterialBlueprintResource::UniformBuffer& instanceUniformBuffer, const Renderable& renderable, MaterialTechnique& materialTechnique)
	{
		// TODO(co) This is just a placeholder implementation until "RendererRuntime::InstanceBufferManager" is ready
		if (!mMapped)
		{
			Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();
			Renderer::MappedSubresource mappedSubresource;
			renderer.map(*mUniformBuffer, 0, Renderer::MapType::WRITE_DISCARD, 0, mappedSubresource);
			mStartUniformBufferPointer = mCurrentUniformBufferPointer = static_cast<uint8_t*>(mappedSubresource.data);
			renderer.map(*mTextureBuffer, 0, Renderer::MapType::WRITE_DISCARD, 0, mappedSubresource);
			mStartTextureBufferPointer = mCurrentTextureBufferPointer = static_cast<float*>(mappedSubresource.data);
			mMapped = true;
		}

		// TODO(co) Implement automatic instancing
		assert(1 == instanceUniformBuffer.numberOfElements);

		const Transform& objectSpaceToWorldSpaceTransform = renderable.getRenderableManager().getTransform();
		const MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
		const MaterialProperties& globalMaterialProperties = materialBlueprintResourceManager.getGlobalMaterialProperties();
		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = materialBlueprintResourceManager.getMaterialBlueprintResourceListener();
		static const PassBufferManager::PassData passData = {};
		materialBlueprintResourceListener.beginFillInstance((nullptr != passBufferManager) ? passBufferManager->getPassData() : passData, objectSpaceToWorldSpaceTransform, materialTechnique);

		{ // Update the uniform buffer
			const MaterialBlueprintResource::UniformBufferElementProperties& uniformBufferElementProperties = instanceUniformBuffer.uniformBufferElementProperties;
			const size_t numberOfUniformBufferElementProperties = uniformBufferElementProperties.size();
			for (size_t i = 0, numberOfPackageBytes = 0; i < numberOfUniformBufferElementProperties; ++i)
			{
				const MaterialProperty& uniformBufferElementProperty = uniformBufferElementProperties[i];

				// Get value type number of bytes
				const uint32_t valueTypeNumberOfBytes = uniformBufferElementProperty.getValueTypeNumberOfBytes(uniformBufferElementProperty.getValueType());

				// Handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
				if (0 != numberOfPackageBytes && numberOfPackageBytes + valueTypeNumberOfBytes > 16)
				{
					// Move the buffer pointer to the location of the next aligned package and restart the package bytes counter
					mCurrentUniformBufferPointer += 4 * 4 - numberOfPackageBytes;
					numberOfPackageBytes = 0;
				}
				numberOfPackageBytes += valueTypeNumberOfBytes % 16;

				// Copy the property value into the buffer
				const MaterialProperty::Usage usage = uniformBufferElementProperty.getUsage();
				if (MaterialProperty::Usage::INSTANCE_REFERENCE == usage)	// Most likely the case, so check this first
				{
					const uint32_t instanceTextureBufferStartIndex = static_cast<uint32_t>(mCurrentTextureBufferPointer - mStartTextureBufferPointer) / 4;	// /4 since the texture buffer is working with float4
					if (!materialBlueprintResourceListener.fillInstanceValue(uniformBufferElementProperty.getReferenceValue(), mCurrentUniformBufferPointer, valueTypeNumberOfBytes, instanceTextureBufferStartIndex))
					{
						// Error, can't resolve reference
						assert(false);
					}
				}
				else if (MaterialProperty::Usage::GLOBAL_REFERENCE == usage)
				{
					// Referencing a global material property inside an instance uniform buffer doesn't make really sense performance wise, but don't forbid it

					// Figure out the global material property value
					const MaterialProperty* materialProperty = globalMaterialProperties.getPropertyById(uniformBufferElementProperty.getReferenceValue());
					if (nullptr != materialProperty)
					{
						// TODO(co) Error handling: Usage mismatch, value type mismatch etc.
						memcpy(mCurrentUniformBufferPointer, materialProperty->getData(), valueTypeNumberOfBytes);
					}
					else
					{
						// Try global material property reference fallback
						materialProperty = materialBlueprintResourceManager.getById(materialTechnique.getMaterialBlueprintResourceId()).getMaterialProperties().getPropertyById(uniformBufferElementProperty.getReferenceValue());
						if (nullptr != materialProperty)
						{
							// TODO(co) Error handling: Usage mismatch, value type mismatch etc.
							memcpy(mCurrentUniformBufferPointer, materialProperty->getData(), valueTypeNumberOfBytes);
						}
						else
						{
							// Error, can't resolve reference
							assert(false);	// RendererRuntime::PassBufferManager::fillBuffer(): Failed to fill pass uniform buffer element " << i << " by using unknown global material property
						}
					}
				}
				else if (!uniformBufferElementProperty.isReferenceUsage())	// TODO(co) Performance: Think about such tests, the toolkit should already take care of this so we have well known verified runtime data
				{
					// Referencing a static uniform buffer element property inside an instance uniform buffer doesn't make really sense performance wise, but don't forbid it

					// Just copy over the property value
					memcpy(mCurrentUniformBufferPointer, uniformBufferElementProperty.getData(), valueTypeNumberOfBytes);
				}
				else
				{
					// Error, invalid property
					assert(false);
				}

				// Next property
				mCurrentUniformBufferPointer += valueTypeNumberOfBytes;
			}
		}

		{ // Update the texture buffer
			// TODO(co) Check "InstanceTextureBuffer" value

			{ // "POSITION_ROTATION_SCALE"-semantic
				// xyz position
				memcpy(mCurrentTextureBufferPointer, glm::value_ptr(objectSpaceToWorldSpaceTransform.position), sizeof(float) * 3);
				mCurrentTextureBufferPointer += 4;

				// xyzw rotation quaternion
				// -> xyz would be sufficient since the rotation quaternion is normalized and we could reconstruct w inside the shader.
				//    Since we have to work with float4 and currently have room to spare, there's no need for the rotation quaternion reduction.
				memcpy(mCurrentTextureBufferPointer, glm::value_ptr(objectSpaceToWorldSpaceTransform.rotation), sizeof(float) * 4);
				mCurrentTextureBufferPointer += 4;

				// xyz scale
				memcpy(mCurrentTextureBufferPointer, glm::value_ptr(objectSpaceToWorldSpaceTransform.scale), sizeof(float) * 3);
				mCurrentTextureBufferPointer += 4;
			}

			// Do we also need to pass on bone transform matrices?
			const SkeletonResourceId skeletonResourceId = renderable.getSkeletonResourceId();
			if (isInitialized(skeletonResourceId))
			{
				const SkeletonResource& skeletonResource = mRendererRuntime.getSkeletonResourceManager().getById(skeletonResourceId);
				const uint8_t numberOfBones = skeletonResource.getNumberOfBones();
				assert(0 != numberOfBones);	// Each skeleton must have at least one bone
				const glm::mat3x4* boneSpaceMatrices = skeletonResource.getBoneSpaceMatrices();
				assert(nullptr != boneSpaceMatrices);
				// TODO(co) Bone transform matrices can consume up to 16 KiB, our texture buffer is at least 64 KiB. So, currently no security checks required, but will be later on.
				const size_t numberOfBytes = sizeof(glm::mat3x4) * numberOfBones;
				memcpy(mCurrentTextureBufferPointer, boneSpaceMatrices, numberOfBytes);
				mCurrentTextureBufferPointer += numberOfBytes / sizeof(float);
			}
		}

		// Done
		++mStartInstanceLocation;
		return mStartInstanceLocation - 1;
	}

	void InstanceBufferManager::fillCommandBuffer(const MaterialBlueprintResource& materialBlueprintResource, Renderer::CommandBuffer& commandBuffer)
	{
		{ // Instance uniform buffer
			const MaterialBlueprintResource::UniformBuffer* instanceUniformBuffer = materialBlueprintResource.getInstanceUniformBuffer();
			if (nullptr != instanceUniformBuffer)
			{
				Renderer::Command::SetGraphicsRootDescriptorTable::create(commandBuffer, instanceUniformBuffer->rootParameterIndex, mUniformBuffer);
			}
		}

		{ // Instance texture buffer
			const MaterialBlueprintResource::TextureBuffer* instanceTextureBuffer = materialBlueprintResource.getInstanceTextureBuffer();
			if (nullptr != instanceTextureBuffer)
			{
				Renderer::Command::SetGraphicsRootDescriptorTable::create(commandBuffer, instanceTextureBuffer->rootParameterIndex, mTextureBuffer);
			}
		}
	}

	void InstanceBufferManager::onPreCommandBufferExecution()
	{
		// TODO(co) This is just a placeholder implementation until "RendererRuntime::InstanceBufferManager" is ready
		if (mMapped)
		{
			Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();
			renderer.unmap(*mUniformBuffer, 0);
			renderer.unmap(*mTextureBuffer, 0);
			mMapped = false;
			mStartUniformBufferPointer = nullptr;
			mCurrentUniformBufferPointer = nullptr;
			mStartTextureBufferPointer = nullptr;
			mCurrentTextureBufferPointer = nullptr;
			mStartInstanceLocation = 0;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
