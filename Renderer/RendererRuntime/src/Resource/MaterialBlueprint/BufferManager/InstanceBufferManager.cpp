﻿/*********************************************************\
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
		mMaximumUniformBufferSize(std::min(rendererRuntime.getRenderer().getCapabilities().maximumUniformBufferSize, ::detail::DEFAULT_UNIFORM_BUFFER_NUMBER_OF_BYTES)),
		mMaximumTextureBufferSize(std::min(rendererRuntime.getRenderer().getCapabilities().maximumTextureBufferSize, ::detail::DEFAULT_TEXTURE_BUFFER_NUMBER_OF_BYTES)),
		// Current instance buffer related data
		mCurrentInstanceBufferIndex(getUninitialized<size_t>()),
		mCurrentInstanceBuffer(nullptr),
		mStartUniformBufferPointer(nullptr),
		mCurrentUniformBufferPointer(nullptr),
		mStartTextureBufferPointer(nullptr),
		mCurrentTextureBufferPointer(nullptr),
		mStartInstanceLocation(0)
	{
		// There must always be at least one instance buffer instance
		createInstanceBuffer();
	}

	InstanceBufferManager::~InstanceBufferManager()
	{
		// Release uniform and texture buffer instances
		for (InstanceBuffer& instanceBuffer : mInstanceBuffers)
		{
			if (nullptr != instanceBuffer.resourceGroup)
			{
				instanceBuffer.resourceGroup->releaseReference();
			}
			instanceBuffer.uniformBuffer->releaseReference();
			instanceBuffer.textureBuffer->releaseReference();
		}
	}

	void InstanceBufferManager::startupBufferFilling(const MaterialBlueprintResource& materialBlueprintResource, Renderer::CommandBuffer& commandBuffer)
	{
		// Sanity check
		assert(nullptr != mCurrentInstanceBuffer);

		// Map the current instance buffer
		mapCurrentInstanceBuffer();

		// Get buffer pointers
		const MaterialBlueprintResource::UniformBuffer* instanceUniformBuffer = materialBlueprintResource.getInstanceUniformBuffer();
		const MaterialBlueprintResource::TextureBuffer* instanceTextureBuffer = materialBlueprintResource.getInstanceTextureBuffer();
		if (nullptr != instanceUniformBuffer)
		{
			// Sanity checks
			assert(nullptr != instanceTextureBuffer);
			assert(instanceUniformBuffer->rootParameterIndex == instanceTextureBuffer->rootParameterIndex);

			// Create resource group, if needed
			// TODO(co) We probably need to move the instance buffer manager into the material blueprint resource
			if (nullptr == mCurrentInstanceBuffer->resourceGroup)
			{
				Renderer::IResource* resources[2] = { mCurrentInstanceBuffer->uniformBuffer, mCurrentInstanceBuffer->textureBuffer };
				mCurrentInstanceBuffer->resourceGroup = materialBlueprintResource.getRootSignaturePtr()->createResourceGroup(instanceUniformBuffer->rootParameterIndex, static_cast<uint32_t>(glm::countof(resources)), resources);
				RENDERER_SET_RESOURCE_DEBUG_NAME(mCurrentInstanceBuffer->resourceGroup, "Instance buffer manager")
				mCurrentInstanceBuffer->resourceGroup->addReference();
			}

			// Set resource group
			Renderer::Command::SetGraphicsResourceGroup::create(commandBuffer, instanceUniformBuffer->rootParameterIndex, mCurrentInstanceBuffer->resourceGroup);
		}
	}

	uint32_t InstanceBufferManager::fillBuffer(const MaterialBlueprintResource& materialBlueprintResource, PassBufferManager* passBufferManager, const MaterialBlueprintResource::UniformBuffer& instanceUniformBuffer, const Renderable& renderable, MaterialTechnique& materialTechnique, Renderer::CommandBuffer& commandBuffer)
	{
		// Sanity checks
		assert(nullptr != mCurrentInstanceBuffer);
		assert(nullptr != mStartUniformBufferPointer);
		assert(nullptr != mCurrentUniformBufferPointer);
		assert(nullptr != mStartTextureBufferPointer);
		assert(nullptr != mCurrentTextureBufferPointer);
		// assert(0 == mStartInstanceLocation); -> Not done by intent
		assert(MaterialBlueprintResource::BufferUsage::INSTANCE == instanceUniformBuffer.bufferUsage && "Currently only the uniform buffer instance buffer usage is supported");

		// Get relevant data
		const Transform& objectSpaceToWorldSpaceTransform = renderable.getRenderableManager().getTransform();
		const MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
		const MaterialProperties& globalMaterialProperties = materialBlueprintResourceManager.getGlobalMaterialProperties();
		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = materialBlueprintResourceManager.getMaterialBlueprintResourceListener();
		const MaterialBlueprintResource::UniformBufferElementProperties& uniformBufferElementProperties = instanceUniformBuffer.uniformBufferElementProperties;
		const size_t numberOfUniformBufferElementProperties = uniformBufferElementProperties.size();
		const SkeletonResourceId skeletonResourceId = renderable.getSkeletonResourceId();
		const SkeletonResource* skeletonResource = isInitialized(skeletonResourceId) ? &mRendererRuntime.getSkeletonResourceManager().getById(skeletonResourceId) : nullptr;
		static const PassBufferManager::PassData passData = {};
		materialBlueprintResourceListener.beginFillInstance((nullptr != passBufferManager) ? passBufferManager->getPassData() : passData, objectSpaceToWorldSpaceTransform, materialTechnique);

		{ // Handle instance buffer overflow
			// Calculate number of additionally needed uniform buffer bytes
			uint32_t newNeededUniformBufferSize = 0;
			for (size_t i = 0, numberOfPackageBytes = 0; i < numberOfUniformBufferElementProperties; ++i)
			{
				const MaterialProperty& uniformBufferElementProperty = uniformBufferElementProperties[i];

				// Get value type number of bytes
				const uint32_t valueTypeNumberOfBytes = uniformBufferElementProperty.getValueTypeNumberOfBytes(uniformBufferElementProperty.getValueType());

				// Handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
				if (0 != numberOfPackageBytes && numberOfPackageBytes + valueTypeNumberOfBytes > 16)
				{
					// Move the buffer pointer to the location of the next aligned package and restart the package bytes counter
					newNeededUniformBufferSize += static_cast<uint32_t>(sizeof(float) * 4 - numberOfPackageBytes);
					numberOfPackageBytes = 0;
				}
				numberOfPackageBytes += valueTypeNumberOfBytes % 16;

				// Next property
				newNeededUniformBufferSize += valueTypeNumberOfBytes;
			}

			// Calculate number of additionally needed texture buffer bytes
			uint32_t newNeededTextureBufferSize = sizeof(float) * 4 * 3;	// xyz position (float4) + xyzw rotation quaternion (float4) + xyz scale (float4)
			if (nullptr != skeletonResource)
			{
				const uint8_t numberOfBones = skeletonResource->getNumberOfBones();
				assert(0 != numberOfBones && "Each skeleton must have at least one bone");
				const uint32_t numberOfBytes = sizeof(glm::mat3x4) * numberOfBones;
				assert(numberOfBytes <= mMaximumTextureBufferSize && "The skeleton has too many bones for the available maximum texture buffer size");
				newNeededTextureBufferSize += numberOfBytes;
			}

			// Detect and handle instance buffer overflow
			const uint32_t totalNeededUniformBufferSize = (static_cast<uint32_t>(mCurrentUniformBufferPointer - mStartUniformBufferPointer) + newNeededUniformBufferSize);
			const uint32_t totalNeededTextureBufferSize = (static_cast<uint32_t>(mCurrentTextureBufferPointer - mStartTextureBufferPointer) * sizeof(float) + newNeededTextureBufferSize);
			if (totalNeededUniformBufferSize > mMaximumUniformBufferSize || totalNeededTextureBufferSize > mMaximumTextureBufferSize)
			{
				createInstanceBuffer();
				startupBufferFilling(materialBlueprintResource, commandBuffer);
			}
		}

		// Fill the uniform buffer
		for (size_t i = 0, numberOfPackageBytes = 0; i < numberOfUniformBufferElementProperties; ++i)
		{
			const MaterialProperty& uniformBufferElementProperty = uniformBufferElementProperties[i];

			// Get value type number of bytes
			const uint32_t valueTypeNumberOfBytes = uniformBufferElementProperty.getValueTypeNumberOfBytes(uniformBufferElementProperty.getValueType());

			// Handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
			if (0 != numberOfPackageBytes && numberOfPackageBytes + valueTypeNumberOfBytes > 16)
			{
				// Move the buffer pointer to the location of the next aligned package and restart the package bytes counter
				mCurrentUniformBufferPointer += sizeof(float) * 4 - numberOfPackageBytes;
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

		{ // Fill the texture buffer
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
			if (nullptr != skeletonResource)
			{
				const uint8_t numberOfBones = skeletonResource->getNumberOfBones();
				assert(0 != numberOfBones && "Each skeleton must have at least one bone");
				const glm::mat3x4* boneSpaceMatrices = skeletonResource->getBoneSpaceMatrices();
				assert(nullptr != boneSpaceMatrices);
				const size_t numberOfBytes = sizeof(glm::mat3x4) * numberOfBones;
				assert(numberOfBytes <= mMaximumTextureBufferSize && "The skeleton has too many bones for the available maximum texture buffer size");
				memcpy(mCurrentTextureBufferPointer, boneSpaceMatrices, numberOfBytes);
				mCurrentTextureBufferPointer += numberOfBytes / sizeof(float);
			}
		}

		// Done
		++mStartInstanceLocation;
		return mStartInstanceLocation - 1;
	}

	void InstanceBufferManager::onPreCommandBufferExecution()
	{
		// Unmap the current instance buffer and reset the current instance buffer to the first instance
		if (isInitialized(mCurrentInstanceBufferIndex))
		{
			unmapCurrentInstanceBuffer();
			mCurrentInstanceBufferIndex = 0;
			mCurrentInstanceBuffer = &mInstanceBuffers[mCurrentInstanceBufferIndex];
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void InstanceBufferManager::createInstanceBuffer()
	{
		Renderer::IBufferManager& bufferManager = mRendererRuntime.getBufferManager();

		// Before doing anything else: Unmap the current instance buffer
		unmapCurrentInstanceBuffer();

		// Update current instance buffer
		mCurrentInstanceBufferIndex = isInitialized(mCurrentInstanceBufferIndex) ? (mCurrentInstanceBufferIndex + 1) : 0;
		if (mCurrentInstanceBufferIndex >= mInstanceBuffers.size())
		{
			// Create uniform buffer instance
			Renderer::IUniformBuffer* uniformBuffer = bufferManager.createUniformBuffer(mMaximumUniformBufferSize, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
			RENDERER_SET_RESOURCE_DEBUG_NAME(uniformBuffer, "Instance buffer manager")
			uniformBuffer->addReference();

			// Create texture buffer instance
			Renderer::ITextureBuffer* textureBuffer = bufferManager.createTextureBuffer(mMaximumTextureBufferSize, Renderer::TextureFormat::R32G32B32A32F, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
			RENDERER_SET_RESOURCE_DEBUG_NAME(textureBuffer, "Instance buffer manager")
			textureBuffer->addReference();

			// Create instance buffer instance
			mInstanceBuffers.emplace_back(*uniformBuffer, *textureBuffer);
		}
		mCurrentInstanceBuffer = &mInstanceBuffers[mCurrentInstanceBufferIndex];
	}

	void InstanceBufferManager::mapCurrentInstanceBuffer()
	{
		if (nullptr != mCurrentInstanceBuffer && !mCurrentInstanceBuffer->mapped)
		{
			// Sanity checks: Only one mapped instance buffer at a time
			assert(nullptr == mStartUniformBufferPointer);
			assert(nullptr == mCurrentUniformBufferPointer);
			assert(nullptr == mStartTextureBufferPointer);
			assert(nullptr == mCurrentTextureBufferPointer);
			assert(0 == mStartInstanceLocation);

			// Map instance buffer
			Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();
			Renderer::MappedSubresource mappedSubresource;
			renderer.map(*mCurrentInstanceBuffer->uniformBuffer, 0, Renderer::MapType::WRITE_DISCARD, 0, mappedSubresource);
			mStartUniformBufferPointer = mCurrentUniformBufferPointer = static_cast<uint8_t*>(mappedSubresource.data);
			renderer.map(*mCurrentInstanceBuffer->textureBuffer, 0, Renderer::MapType::WRITE_DISCARD, 0, mappedSubresource);
			mStartTextureBufferPointer = mCurrentTextureBufferPointer = static_cast<float*>(mappedSubresource.data);
			mCurrentInstanceBuffer->mapped = true;
		}
	}

	void InstanceBufferManager::unmapCurrentInstanceBuffer()
	{
		if (nullptr != mCurrentInstanceBuffer && mCurrentInstanceBuffer->mapped)
		{
			// Sanity checks
			assert(nullptr != mStartUniformBufferPointer);
			assert(nullptr != mCurrentUniformBufferPointer);
			assert(nullptr != mStartTextureBufferPointer);
			assert(nullptr != mCurrentTextureBufferPointer);
			// assert(0 == mStartInstanceLocation); -> Not done by intent

			// Unmap instance buffer
			Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();
			renderer.unmap(*mCurrentInstanceBuffer->uniformBuffer, 0);
			renderer.unmap(*mCurrentInstanceBuffer->textureBuffer, 0);
			mCurrentInstanceBuffer->mapped = false;
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
