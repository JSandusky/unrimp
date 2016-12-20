/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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

		// TODO(co) Add support for persistent mapped buffers. For now, the big picture has to be OK so first focus on that.
		static uint32_t DEFAULT_TEXTURE_BUFFER_NUMBER_OF_BYTES = 64 * 1024;	// 64 KiB
		// static uint32_t DEFAULT_TEXTURE_BUFFER_NUMBER_OF_BYTES = 512 * 1024;	// 512 KiB


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
		mUniformBuffer(nullptr),
		mTextureBuffer(nullptr)
	{
		Renderer::IBufferManager& bufferManager = rendererRuntime.getBufferManager();

		// Create uniform buffer instance
		mUniformScratchBuffer.resize(std::min(rendererRuntime.getRenderer().getCapabilities().maximumUniformBufferSize, ::detail::DEFAULT_UNIFORM_BUFFER_NUMBER_OF_BYTES));
		mUniformBuffer = bufferManager.createUniformBuffer(mUniformScratchBuffer.size(), nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);

		// Create texture buffer instance
		mTextureScratchBuffer.resize(std::min(rendererRuntime.getRenderer().getCapabilities().maximumTextureBufferSize, ::detail::DEFAULT_TEXTURE_BUFFER_NUMBER_OF_BYTES));
		mTextureBuffer = bufferManager.createTextureBuffer(mTextureScratchBuffer.size(), Renderer::TextureFormat::R32G32B32A32F, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
	}

	InstanceBufferManager::~InstanceBufferManager()
	{
		// Release uniform and texture buffer instances
		mUniformBuffer->release();
		mTextureBuffer->release();
	}

	void InstanceBufferManager::fillBuffer(PassBufferManager& passBufferManager, const MaterialBlueprintResource::UniformBuffer* instanceUniformBuffer, const MaterialBlueprintResource::TextureBuffer*, const Transform& objectSpaceToWorldSpaceTransform, MaterialTechnique& materialTechnique, Renderer::CommandBuffer& commandBuffer)
	{
		// TODO(co) This is just a placeholder implementation until "RendererRuntime::InstanceBufferManager" is ready

		// TODO(co) Currently uniform buffer is required, but some material blueprints (usually compositor related) don't have uniform buffers
		if (nullptr == instanceUniformBuffer)
		{
			return;
		}

		// TODO(co) Implement automatic instancing
		assert(1 == instanceUniformBuffer->numberOfElements);

		const MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
		const MaterialProperties& globalMaterialProperties = materialBlueprintResourceManager.getGlobalMaterialProperties();

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = materialBlueprintResourceManager.getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillInstance(passBufferManager.getPassData(), objectSpaceToWorldSpaceTransform, materialTechnique);

		{ // Update the uniform scratch buffer
			uint8_t* scratchBufferPointer = mUniformScratchBuffer.data();
			const MaterialBlueprintResource::UniformBufferElementProperties& uniformBufferElementProperties = instanceUniformBuffer->uniformBufferElementProperties;
			const size_t numberOfUniformBufferElementProperties = uniformBufferElementProperties.size();
			for (size_t i = 0, numberOfPackageBytes = 0; i < numberOfUniformBufferElementProperties; ++i)
			{
				const MaterialProperty& uniformBufferElementProperty = uniformBufferElementProperties[i];

				// Get value type number of bytes
				const uint32_t valueTypeNumberOfBytes = uniformBufferElementProperty.getValueTypeNumberOfBytes(uniformBufferElementProperty.getValueType());

				// Handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
				if (0 != numberOfPackageBytes && numberOfPackageBytes + valueTypeNumberOfBytes > 16)
				{
					// Move the scratch buffer pointer to the location of the next aligned package and restart the package bytes counter
					scratchBufferPointer += 4 * 4 - numberOfPackageBytes;
					numberOfPackageBytes = 0;
				}
				numberOfPackageBytes += valueTypeNumberOfBytes % 16;

				// Copy the property value into the scratch buffer
				const MaterialProperty::Usage usage = uniformBufferElementProperty.getUsage();
				if (MaterialProperty::Usage::INSTANCE_REFERENCE == usage)	// Most likely the case, so check this first
				{
					if (!materialBlueprintResourceListener.fillInstanceValue(uniformBufferElementProperty.getReferenceValue(), scratchBufferPointer, valueTypeNumberOfBytes))
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
						memcpy(scratchBufferPointer, materialProperty->getData(), valueTypeNumberOfBytes);
					}
					else
					{
						// Error, can't resolve reference
						assert(false);
					}
				}
				else if (!uniformBufferElementProperty.isReferenceUsage())	// TODO(co) Performance: Think about such tests, the toolkit should already take care of this so we have well known verified runtime data
				{
					// Referencing a static uniform buffer element property inside an instance uniform buffer doesn't make really sense performance wise, but don't forbid it

					// Just copy over the property value
					memcpy(scratchBufferPointer, uniformBufferElementProperty.getData(), valueTypeNumberOfBytes);
				}
				else
				{
					// Error, invalid property
					assert(false);
				}

				// Next property
				scratchBufferPointer += valueTypeNumberOfBytes;
			}
		}

		{ // Update the texture scratch buffer
			// TODO(co) Check "InstanceTextureBuffer" value, "@OBJECT_SPACE_TO_WORLD_SPACE_MATRIX" here
			float* scratchBufferPointer = reinterpret_cast<float*>(mTextureScratchBuffer.data());
			glm::mat4 objectSpaceToWorldSpaceMatrix;
			objectSpaceToWorldSpaceTransform.getAsMatrix(objectSpaceToWorldSpaceMatrix);
			objectSpaceToWorldSpaceMatrix = glm::transpose(objectSpaceToWorldSpaceMatrix);
			memcpy(scratchBufferPointer, glm::value_ptr(objectSpaceToWorldSpaceMatrix), sizeof(float) * 4 * 4);
		}

		// Update the uniform and texture buffer by using our scratch buffer
		// TODO(co) This is just a dummy implementation, so full uniform buffer size to stress the command buffer
		Renderer::Command::CopyUniformBufferData::create(commandBuffer, mUniformBuffer, mUniformScratchBuffer.size(), mUniformScratchBuffer.data());
		Renderer::Command::CopyTextureBufferData::create(commandBuffer, mTextureBuffer, mTextureScratchBuffer.size(), mTextureScratchBuffer.data());
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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
