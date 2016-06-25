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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <glm/detail/setup.hpp>	// For "glm::countof()"


// Disable warnings
// TODO(co) See "RendererRuntime::MaterialBlueprintResource::MaterialBlueprintResource()": How the heck should we avoid such a situation without using complicated solutions like a pointer to an instance? (= more individual allocations/deallocations)
#pragma warning(disable: 4355)	// warning C4355: 'this': used in base member initializer list


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		// TODO(co) We need a central vertex input layout management
		// Vertex input layout
		const Renderer::VertexAttribute vertexAttributesLayout[] =
		{
			{ // Attribute 0
				// Data destination
				Renderer::VertexAttributeFormat::FLOAT_3,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
				"Position",									// name[32] (char)
				"POSITION",									// semanticName[32] (char)
				0,											// semanticIndex (uint32_t)
				// Data source
				0,											// inputSlot (uint32_t)
				0,											// alignedByteOffset (uint32_t)
				// Data source, instancing part
				0											// instancesPerElement (uint32_t)
			},
			{ // Attribute 1
				// Data destination
				Renderer::VertexAttributeFormat::SHORT_2,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
				"TexCoord",									// name[32] (char)
				"TEXCOORD",									// semanticName[32] (char)
				0,											// semanticIndex (uint32_t)
				// Data source
				0,											// inputSlot (uint32_t)
				sizeof(float) * 3,							// alignedByteOffset (uint32_t)
				// Data source, instancing part
				0											// instancesPerElement (uint32_t)
			},
			{ // Attribute 2
				// Data destination
				Renderer::VertexAttributeFormat::SHORT_4,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
				"QTangent",									// name[32] (char)
				"TEXCOORD",									// semanticName[32] (char)
				1,											// semanticIndex (uint32_t)
				// Data source
				0,											// inputSlot (uint32_t)
				sizeof(float) * 3 + sizeof(short) * 2,		// alignedByteOffset (uint32_t)
				// Data source, instancing part
				0											// instancesPerElement (uint32_t)
			}
		};


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		bool isFullyLoaded(const RendererRuntime::ShaderPieceResourceManager& shaderPieceResourceManager, const RendererRuntime::ShaderBlueprintResourceManager& shaderBlueprintResourceManager, const RendererRuntime::ShaderBlueprintResourceId shaderBlueprintResourceId)
		{
			// Check shader blueprint
			const RendererRuntime::ShaderBlueprintResource* shaderBlueprintResource = shaderBlueprintResourceManager.getShaderBlueprintResources().tryGetElementById(shaderBlueprintResourceId);
			if (nullptr == shaderBlueprintResource || RendererRuntime::IResource::LoadingState::LOADED != shaderBlueprintResource->getLoadingState())
			{
				// Not fully loaded
				return false;
			}

			{ // Check included shader piece resources
				const RendererRuntime::ShaderBlueprintResource::IncludeShaderPieceResourceIds& includeShaderPieceResources = shaderBlueprintResource->getIncludeShaderPieceResourceIds();
				const RendererRuntime::ShaderPieceResources& shaderPieceResources = shaderPieceResourceManager.getShaderPieceResources();
				const size_t numberOfShaderPieces = includeShaderPieceResources.size();
				for (size_t i = 0; i < numberOfShaderPieces; ++i)
				{
					const RendererRuntime::ShaderPieceResource* shaderPieceResource = shaderPieceResources.tryGetElementById(includeShaderPieceResources[i]);
					if (nullptr == shaderPieceResource || RendererRuntime::IResource::LoadingState::LOADED != shaderPieceResource->getLoadingState())
					{
						// Not fully loaded
						return false;
					}
				}
			}

			// Fully loaded
			return true;
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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	bool MaterialBlueprintResource::isFullyLoaded() const
	{
		// Check uniform buffers
		if (nullptr == mPassUniformBuffer || nullptr ==  mMaterialUniformBuffer || nullptr ==  mInstanceUniformBuffer)
		{
			// Not fully loaded
			return false;
		}

		// Get the shader piece resource manager instance
		const ShaderPieceResourceManager& shaderPieceResourceManager = getMaterialBlueprintResourceManager().getRendererRuntime().getShaderPieceResourceManager();
		const ShaderBlueprintResourceManager& shaderBlueprintResourceManager = getMaterialBlueprintResourceManager().getRendererRuntime().getShaderBlueprintResourceManager();

		// Check the rest
		// TODO(co) Handle the other shader types
		return (IResource::LoadingState::LOADED == getLoadingState() && nullptr != mRootSignaturePtr && ::detail::isFullyLoaded(shaderPieceResourceManager, shaderBlueprintResourceManager, mShaderBlueprintResourceId[static_cast<uint8_t>(ShaderType::Vertex)]) && ::detail::isFullyLoaded(shaderPieceResourceManager, shaderBlueprintResourceManager, mShaderBlueprintResourceId[static_cast<uint8_t>(ShaderType::Fragment)]));
	}

	void MaterialBlueprintResource::fillUnknownUniformBuffers()
	{
		assert(nullptr != mMaterialBlueprintResourceManager);
		const MaterialProperties& globalMaterialProperties = mMaterialBlueprintResourceManager->getGlobalMaterialProperties();

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mMaterialBlueprintResourceManager->getMaterialBlueprintResourceListener();
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
						//  -> GLSL is even more restrictive, with aligning e.g. float2 to an offset divisible by 2 * 4 bytes (float2 size) and float3 to an offset divisible by 4 * 4 bytes (float4 size -- yes, there is no actual float3 alignment) -> Such more extensive error checking is handled by the renderer toolkit
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
		assert(nullptr != mMaterialBlueprintResourceManager);
		assert(nullptr != mPassUniformBuffer);
		assert(1 == mPassUniformBuffer->numberOfElements);

		const MaterialProperties& globalMaterialProperties = mMaterialBlueprintResourceManager->getGlobalMaterialProperties();

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mMaterialBlueprintResourceManager->getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillPass(mMaterialBlueprintResourceManager->getRendererRuntime(), worldSpaceToViewSpaceTransform);

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
		assert(nullptr != mMaterialBlueprintResourceManager);
		assert(nullptr != mMaterialUniformBuffer);

		const MaterialProperties& globalMaterialProperties = mMaterialBlueprintResourceManager->getGlobalMaterialProperties();

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mMaterialBlueprintResourceManager->getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillMaterial();

		// TODO(co) Optimization: Do only update changed materials

		// Update the scratch buffer
		ScratchBuffer& scratchBuffer = mMaterialUniformBuffer->scratchBuffer;
		{
			const UniformBufferElementProperties& uniformBufferElementProperties = mMaterialUniformBuffer->uniformBufferElementProperties;
			const size_t numberOfUniformBufferElementProperties = uniformBufferElementProperties.size();
			const size_t numberOfLinkedMaterialTechniques = mLinkedMaterialTechniques.size();
			const uint32_t numberOfBytesPerElement = mMaterialUniformBuffer->uniformBufferNumberOfBytes / mMaterialUniformBuffer->numberOfElements;
			for (uint32_t materialIndex = 0; materialIndex < numberOfLinkedMaterialTechniques; ++materialIndex)
			{
				const MaterialTechnique* materialTechnique = mLinkedMaterialTechniques[materialIndex];
				assert(materialTechnique->getMaterialUniformBufferIndex() == materialIndex);

				const MaterialProperties& materialProperties = materialTechnique->getMaterialResource().getMaterialProperties();
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
						const MaterialProperty* materialProperty = materialProperties.getPropertyById(uniformBufferElementProperty.getReferenceValue());
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

	void MaterialBlueprintResource::fillInstanceUniformBuffer(const Transform& objectSpaceToWorldSpaceTransform, MaterialTechnique& materialTechnique)
	{
		assert(nullptr != mMaterialBlueprintResourceManager);
		assert(getId() == materialTechnique.getMaterialBlueprintResourceId());
		assert(nullptr != mInstanceUniformBuffer);
		assert(1 == mInstanceUniformBuffer->numberOfElements);	// TODO(co) Implement automatic instancing

		const MaterialProperties& globalMaterialProperties = mMaterialBlueprintResourceManager->getGlobalMaterialProperties();

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mMaterialBlueprintResourceManager->getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillInstance(objectSpaceToWorldSpaceTransform, materialTechnique);

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

		// Update the uniform buffer by using our scratch buffer
		mInstanceUniformBuffer->uniformBufferPtr->copyDataFrom(scratchBuffer.size(), scratchBuffer.data());
	}

	void MaterialBlueprintResource::bindToRenderer() const
	{
		Renderer::IRenderer& renderer = mRootSignaturePtr->getRenderer();

		// Set the used graphics root signature
		renderer.setGraphicsRootSignature(mRootSignaturePtr);

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
	MaterialBlueprintResource::MaterialBlueprintResource() :
		IResource(getUninitialized<MaterialBlueprintResourceId>()),
		mMaterialBlueprintResourceManager(nullptr),
		mPipelineStateCacheManager(*this),
		mVertexAttributes(glm::countof(::detail::vertexAttributesLayout), ::detail::vertexAttributesLayout),
		mPipelineState(Renderer::PipelineStateBuilder())
	{
		memset(mShaderBlueprintResourceId, static_cast<int>(getUninitialized<ShaderBlueprintResourceId>()), sizeof(ShaderBlueprintResourceId) * NUMBER_OF_SHADER_TYPES);
	}

	MaterialBlueprintResource::MaterialBlueprintResource(MaterialBlueprintResourceId materialBlueprintResourceId) :
		IResource(materialBlueprintResourceId),
		mMaterialBlueprintResourceManager(nullptr),
		mPipelineStateCacheManager(*this),
		mVertexAttributes(glm::countof(::detail::vertexAttributesLayout), ::detail::vertexAttributesLayout),
		mPipelineState(Renderer::PipelineStateBuilder())
	{
		memset(mShaderBlueprintResourceId, static_cast<int>(getUninitialized<ShaderBlueprintResourceId>()), sizeof(ShaderBlueprintResourceId) * NUMBER_OF_SHADER_TYPES);
	}

	void MaterialBlueprintResource::linkMaterialTechnique(MaterialTechnique& materialTechnique)
	{
		LinkedMaterialTechniques::const_iterator iterator = std::find(mLinkedMaterialTechniques.cbegin(), mLinkedMaterialTechniques.cend(), &materialTechnique);
		if (mLinkedMaterialTechniques.cend() == iterator)
		{
			materialTechnique.mMaterialUniformBufferIndex = mLinkedMaterialTechniques.size();
			mLinkedMaterialTechniques.push_back(&materialTechnique);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
