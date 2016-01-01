/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResource.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResource.h"


// Disable warnings
// TODO(co) See "RendererRuntime::MaterialBlueprintResource::MaterialBlueprintResource()": How the heck should we avoid such a situation without using complicated solutions like a pointer to an instance? (= more individual allocations/deallocations)
#pragma warning(disable: 4355)	// warning C4355: 'this': used in base member initializer list


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	namespace detail
	{

		bool isFullyLoaded(const ShaderBlueprintResource* shaderBlueprint)
		{
			// Check shader blueprint
			if (nullptr == shaderBlueprint || IResource::LoadingState::LOADED != shaderBlueprint->getLoadingState())
			{
				// Not fully loaded
				return false;
			}

			{ // Check included shader piece resources
				const ShaderBlueprintResource::IncludeShaderPieceResources& includeShaderPieceResources = shaderBlueprint->getIncludeShaderPieceResources();
				const size_t numberOfShaderPieces = includeShaderPieceResources.size();
				for (size_t i = 0; i < numberOfShaderPieces; ++i)
				{
					const ShaderPieceResource* shaderPieceResource = includeShaderPieceResources[i];
					if (nullptr == shaderPieceResource || IResource::LoadingState::LOADED != shaderPieceResource->getLoadingState())
					{
						// Not fully loaded
						return false;
					}
				}
			}

			// Fully loaded
			return true;
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialBlueprintResource::MaterialBlueprintResource(MaterialBlueprintResourceManager& materialBlueprintResourceManager, ResourceId resourceId) :
		IResource(resourceId),
		mMaterialBlueprintResourceManager(materialBlueprintResourceManager),
		mPipelineStateCacheManager(*this),
		mRootSignature(nullptr),
		mPipelineState(Renderer::PipelineStateBuilder()),
		mVertexShaderBlueprint(nullptr),
		mTessellationControlShaderBlueprint(nullptr),
		mTessellationEvaluationShaderBlueprint(nullptr),
		mGeometryShaderBlueprint(nullptr),
		mFragmentShaderBlueprint(nullptr)
	{
		// Nothing here
	}

	MaterialBlueprintResource::~MaterialBlueprintResource()
	{
		if (nullptr != mRootSignature)
		{
			mRootSignature->release();
		}
	}

	bool MaterialBlueprintResource::isFullyLoaded() const
	{
		// Check uniform buffers
		if (nullptr == mPassUniformBuffer || nullptr ==  mMaterialUniformBuffer || nullptr ==  mInstanceUniformBuffer)
		{
			// Not fully loaded
			return false;
		}

		// Check the rest
		return (IResource::LoadingState::LOADED == getLoadingState() && nullptr != mRootSignature && detail::isFullyLoaded(mVertexShaderBlueprint) && detail::isFullyLoaded(mFragmentShaderBlueprint));
	}

	void MaterialBlueprintResource::fillUnknownUniformBuffers()
	{
		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mMaterialBlueprintResourceManager.getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillUnknown();

		const size_t numberOfUniformBuffers = mUniformBuffers.size();
		for (size_t uniformBufferIndex = 0; uniformBufferIndex < numberOfUniformBuffers; ++uniformBufferIndex)
		{
			UniformBuffer& uniformBuffer = mUniformBuffers[uniformBufferIndex];
			if (uniformBuffer.uniformBufferUsage == UniformBufferUsage::UNKNOWN)
			{
				assert(1 == uniformBuffer.numberOfElements);

				// Update the scratch buffer
				ScratchBuffer& scratchBuffer = uniformBuffer.scratchBuffer;
				{
					uint8_t* scratchBufferPointer = scratchBuffer.data();
					const UniformBufferElementProperties& uniformBufferElementProperties = uniformBuffer.uniformBufferElementProperties;
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
						if (MaterialProperty::Usage::UNKNOWN_REFERENCE == usage)	// Most likely the case, so check this first
						{
							if (!materialBlueprintResourceListener.fillUnknownValue(uniformBufferElementProperty.getReferenceValue(), scratchBufferPointer, valueTypeNumberOfBytes))
							{
								// Error, can't resolve reference
								assert(false);
							}
						}
						else if (MaterialProperty::Usage::GLOBAL_REFERENCE == usage)
						{
							// Figure out the global material property value
							const MaterialProperty* materialProperty = mMaterialBlueprintResourceManager.getGlobalMaterialPropertyById(uniformBufferElementProperty.getReferenceValue());
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

				// Update the uniform buffer by using our scratch buffer
				uniformBuffer.uniformBufferPtr->copyDataFrom(scratchBuffer.size(), scratchBuffer.data());
			}
		}
	}

	void MaterialBlueprintResource::fillPassUniformBuffer(const Transform& worldSpaceToViewSpaceTransform)
	{
		assert(nullptr != mPassUniformBuffer);
		assert(1 == mPassUniformBuffer->numberOfElements);

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mMaterialBlueprintResourceManager.getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillPass(mPassUniformBuffer->uniformBufferPtr->getRenderer(), worldSpaceToViewSpaceTransform);

		// Update the scratch buffer
		ScratchBuffer& scratchBuffer = mPassUniformBuffer->scratchBuffer;
		{
			uint8_t* scratchBufferPointer = scratchBuffer.data();
			const UniformBufferElementProperties& uniformBufferElementProperties = mPassUniformBuffer->uniformBufferElementProperties;
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
				if (MaterialProperty::Usage::PASS_REFERENCE == usage)	// Most likely the case, so check this first
				{
					if (!materialBlueprintResourceListener.fillPassValue(uniformBufferElementProperty.getReferenceValue(), scratchBufferPointer, valueTypeNumberOfBytes))
					{
						// Error, can't resolve reference
						assert(false);
					}
				}
				else if (MaterialProperty::Usage::GLOBAL_REFERENCE == usage)
				{
					// Figure out the global material property value
					const MaterialProperty* materialProperty = mMaterialBlueprintResourceManager.getGlobalMaterialPropertyById(uniformBufferElementProperty.getReferenceValue());
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

		// Update the uniform buffer by using our scratch buffer
		mPassUniformBuffer->uniformBufferPtr->copyDataFrom(scratchBuffer.size(), scratchBuffer.data());
	}

	void MaterialBlueprintResource::fillMaterialUniformBuffer()
	{
		assert(nullptr != mMaterialUniformBuffer);

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mMaterialBlueprintResourceManager.getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillMaterial();

		// TODO(co) Optimization: Do only update changed materials

		// Update the scratch buffer
		ScratchBuffer& scratchBuffer = mMaterialUniformBuffer->scratchBuffer;
		{
			const UniformBufferElementProperties& uniformBufferElementProperties = mMaterialUniformBuffer->uniformBufferElementProperties;
			const size_t numberOfUniformBufferElementProperties = uniformBufferElementProperties.size();
			const size_t numberOfLinkedMaterialResources = mLinkedMaterialResources.size();
			const uint32_t numberOfBytesPerElement = mMaterialUniformBuffer->uniformBufferNumberOfBytes / mMaterialUniformBuffer->numberOfElements;
			for (uint32_t materialIndex = 0; materialIndex < numberOfLinkedMaterialResources; ++materialIndex)
			{
				const MaterialResource* materialResource = mLinkedMaterialResources[materialIndex];
				assert(materialResource->getMaterialUniformBufferIndex() == materialIndex);

				uint8_t* scratchBufferPointer = scratchBuffer.data() + numberOfBytesPerElement * materialIndex;

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
					if (MaterialProperty::Usage::MATERIAL_REFERENCE == usage)	// Most likely the case, so check this first
					{
						// Figure out the material property value
						const MaterialProperty* materialProperty = materialResource->getMaterialPropertyById(uniformBufferElementProperty.getReferenceValue());
						if (nullptr != materialProperty)
						{
							// TODO(co) Error handling: Usage mismatch, value type mismatch etc.
							memcpy(scratchBufferPointer, materialProperty->getData(), valueTypeNumberOfBytes);
						}
						else if (!materialBlueprintResourceListener.fillMaterialValue(uniformBufferElementProperty.getReferenceValue(), scratchBufferPointer, valueTypeNumberOfBytes))
						{
							// Error, can't resolve reference
							assert(false);
						}
					}
					else if (MaterialProperty::Usage::GLOBAL_REFERENCE == usage)
					{
						// Referencing a global material property inside a material uniform buffer doesn't make really sense performance wise, but don't forbid it
	
						// Figure out the global material property value
						const MaterialProperty* materialProperty = mMaterialBlueprintResourceManager.getGlobalMaterialPropertyById(uniformBufferElementProperty.getReferenceValue());
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
						// Referencing a static material property inside an material uniform buffer doesn't make really sense performance wise, but don't forbid it

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
		}

		// Update the uniform buffer by using our scratch buffer
		mMaterialUniformBuffer->uniformBufferPtr->copyDataFrom(scratchBuffer.size(), scratchBuffer.data());
	}

	void MaterialBlueprintResource::fillInstanceUniformBuffer(const Transform& objectSpaceToWorldSpaceTransform, MaterialResource& materialResource)
	{
		assert(this == materialResource.getMaterialBlueprintResource());
		assert(nullptr != mInstanceUniformBuffer);
		assert(1 == mInstanceUniformBuffer->numberOfElements);	// TODO(co) Implement automatic instancing

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mMaterialBlueprintResourceManager.getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillInstance(objectSpaceToWorldSpaceTransform, materialResource);

		// Update the scratch buffer
		ScratchBuffer& scratchBuffer = mInstanceUniformBuffer->scratchBuffer;
		{
			uint8_t* scratchBufferPointer = scratchBuffer.data();
			const UniformBufferElementProperties& uniformBufferElementProperties = mInstanceUniformBuffer->uniformBufferElementProperties;
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
					const MaterialProperty* materialProperty = mMaterialBlueprintResourceManager.getGlobalMaterialPropertyById(uniformBufferElementProperty.getReferenceValue());
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

		// Update the uniform buffer by using our scratch buffer
		mInstanceUniformBuffer->uniformBufferPtr->copyDataFrom(scratchBuffer.size(), scratchBuffer.data());
	}

	void MaterialBlueprintResource::bindToRenderer() const
	{
		Renderer::IRenderer& renderer = mRootSignature->getRenderer();

		// Set the used graphics root signature
		renderer.setGraphicsRootSignature(mRootSignature);

		{ // Graphics root descriptor table: Set our uniform buffers
			const size_t numberOfUniformBuffers = mUniformBuffers.size();
			for (size_t i = 0; i < numberOfUniformBuffers; ++i)
			{
				const UniformBuffer& uniformBuffer = mUniformBuffers[i];
				renderer.setGraphicsRootDescriptorTable(uniformBuffer.rootParameterIndex, uniformBuffer.uniformBufferPtr);
			}
		}

		{ // Graphics root descriptor table: Set our sampler states
			const size_t numberOfSamplerStates = mSamplerStates.size();
			for (size_t i = 0; i < numberOfSamplerStates; ++i)
			{
				const SamplerState& samplerState = mSamplerStates[i];
				renderer.setGraphicsRootDescriptorTable(samplerState.rootParameterIndex, samplerState.samplerStatePtr);
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void MaterialBlueprintResource::linkedMaterialResource(MaterialResource& materialResource)
	{
		LinkedMaterialResources::const_iterator iterator = std::find(mLinkedMaterialResources.cbegin(), mLinkedMaterialResources.cend(), &materialResource);
		if (mLinkedMaterialResources.cend() == iterator)
		{
			materialResource.mMaterialUniformBufferIndex = mLinkedMaterialResources.size();
			mLinkedMaterialResources.push_back(&materialResource);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
