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
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/PassBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	PassBufferManager::PassBufferManager(IRendererRuntime& rendererRuntime, const MaterialBlueprintResource& materialBlueprintResource) :
		mRendererRuntime(rendererRuntime),
		mBufferManager(rendererRuntime.getBufferManager()),
		mMaterialBlueprintResource(materialBlueprintResource),
		mMaterialBlueprintResourceManager(rendererRuntime.getMaterialBlueprintResourceManager()),
		mCurrentUniformBufferIndex(0)
	{
		const MaterialBlueprintResource::UniformBuffer* passUniformBuffer = mMaterialBlueprintResource.getPassUniformBuffer();
		if (nullptr != passUniformBuffer)
		{
			mScratchBuffer.resize(passUniformBuffer->uniformBufferNumberOfBytes);
		}
	}

	PassBufferManager::~PassBufferManager()
	{
		// Destroy all uniform buffers
		for (Renderer::IUniformBuffer* uniformBuffer : mUniformBuffers)
		{
			uniformBuffer->releaseReference();
		}
	}

	void PassBufferManager::fillBuffer(const Renderer::IRenderTarget& renderTarget, const CompositorContextData& compositorContextData)
	{
		// Even if there's no pass uniform buffer, there must still be a pass buffer manager filling "RendererRuntime::PassBufferManager::PassData" which is used to fill the instances texture buffer

		// Tell the material blueprint resource listener that we're about to fill a pass uniform buffer
		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mMaterialBlueprintResourceManager.getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillPass(mRendererRuntime, renderTarget, compositorContextData, mPassData);

		// Get the pass uniform buffer containing the description of the element properties
		const MaterialBlueprintResource::UniformBuffer* passUniformBuffer = mMaterialBlueprintResource.getPassUniformBuffer();
		if (nullptr != passUniformBuffer)
		{
			// Startup the pass uniform buffer update
			uint8_t* scratchBufferPointer = mScratchBuffer.data();

			{ // Fill the pass uniform buffer by using the material blueprint resource
				const MaterialProperties& globalMaterialProperties = mMaterialBlueprintResourceManager.getGlobalMaterialProperties();
				const MaterialBlueprintResource::UniformBufferElementProperties& uniformBufferElementProperties = passUniformBuffer->uniformBufferElementProperties;
				const size_t numberOfUniformBufferElementProperties = uniformBufferElementProperties.size();
				for (size_t i = 0, numberOfPackageBytes = 0; i < numberOfUniformBufferElementProperties; ++i)
				{
					const MaterialProperty& uniformBufferElementProperty = uniformBufferElementProperties[i];

					// Get value type number of bytes
					const uint32_t valueTypeNumberOfBytes = uniformBufferElementProperty.getValueTypeNumberOfBytes(uniformBufferElementProperty.getValueType());

					// Handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
					if (0 != numberOfPackageBytes && numberOfPackageBytes + valueTypeNumberOfBytes > 16)
					{
						// Move the current buffer pointer to the location of the next aligned package and restart the package bytes counter
						scratchBufferPointer += 4 * 4 - numberOfPackageBytes;
						numberOfPackageBytes = 0;
					}
					numberOfPackageBytes += valueTypeNumberOfBytes % 16;

					// Copy the property value into the current buffer
					const MaterialProperty::Usage usage = uniformBufferElementProperty.getUsage();
					if (MaterialProperty::Usage::PASS_REFERENCE == usage)	// Most likely the case, so check this first
					{
						if (!materialBlueprintResourceListener.fillPassValue(uniformBufferElementProperty.getReferenceValue(), scratchBufferPointer, valueTypeNumberOfBytes))
						{
							// Error, can't resolve reference
							assert(false);	// RendererRuntime::PassBufferManager::fillBuffer(): Material blueprint resource listener failed to fill pass uniform buffer element " << i
						}
					}
					else if (MaterialProperty::Usage::GLOBAL_REFERENCE == usage)
					{
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
							assert(false);	// RendererRuntime::PassBufferManager::fillBuffer(): Failed to fill pass uniform buffer element " << i << " by using unknown global material property
						}
					}
					else if (MaterialProperty::Usage::MATERIAL_REFERENCE == usage)
					{
						// Figure out the material property value
						const MaterialProperty* materialProperty = mMaterialBlueprintResource.getMaterialProperties().getPropertyById(uniformBufferElementProperty.getReferenceValue());
						if (nullptr != materialProperty)
						{
							// TODO(co) Error handling: Usage mismatch, value type mismatch etc.
							memcpy(scratchBufferPointer, materialProperty->getData(), valueTypeNumberOfBytes);
						}
						else if (!materialBlueprintResourceListener.fillMaterialValue(uniformBufferElementProperty.getReferenceValue(), scratchBufferPointer, valueTypeNumberOfBytes))
						{
							// Error, can't resolve reference
							assert(false);	// RendererRuntime::PassBufferManager::uploadToConstBuffer(): Failed to resolve material property value reference
						}
					}
					else if (!uniformBufferElementProperty.isReferenceUsage())
					{
						// Just copy over the property value
						memcpy(scratchBufferPointer, uniformBufferElementProperty.getData(), valueTypeNumberOfBytes);
					}
					else
					{
						// Error, invalid property
						assert(false);	// RendererRuntime::PassBufferManager::fillBuffer(): Failed to fill pass uniform buffer element " << i << " due to unsupported element property usage
					}

					// Next property
					scratchBufferPointer += valueTypeNumberOfBytes;
				}
			}

			// Update the uniform buffer by using our scratch buffer
			if (mCurrentUniformBufferIndex >= static_cast<uint32_t>(mUniformBuffers.size()))
			{
				Renderer::IUniformBuffer* uniformBuffer = mBufferManager.createUniformBuffer(passUniformBuffer->uniformBufferNumberOfBytes, mScratchBuffer.data(), Renderer::BufferUsage::DYNAMIC_DRAW);
				mUniformBuffers.push_back(uniformBuffer);
				RENDERER_SET_RESOURCE_DEBUG_NAME(uniformBuffer, "Pass buffer manager")
			}
			else
			{
				mUniformBuffers[mCurrentUniformBufferIndex]->copyDataFrom(static_cast<uint32_t>(mScratchBuffer.size()), mScratchBuffer.data());
			}
			++mCurrentUniformBufferIndex;
		}
	}

	void PassBufferManager::fillCommandBuffer(Renderer::CommandBuffer& commandBuffer) const
	{
		if (!mUniformBuffers.empty())
		{
			const MaterialBlueprintResource::UniformBuffer* passUniformBuffer = mMaterialBlueprintResource.getPassUniformBuffer();
			if (nullptr != passUniformBuffer)
			{
				Renderer::Command::SetGraphicsRootDescriptorTable::create(commandBuffer, passUniformBuffer->rootParameterIndex, mUniformBuffers[mCurrentUniformBufferIndex - 1]);
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
