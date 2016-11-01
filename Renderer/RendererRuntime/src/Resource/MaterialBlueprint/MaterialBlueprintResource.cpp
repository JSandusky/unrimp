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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/MaterialUniformBufferManager.h"
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResourceManager.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <glm/detail/setup.hpp>	// For "glm::countof()"

#include <chrono>
#include <thread>


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
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		typedef std::vector<RendererRuntime::ShaderPropertyId> ShaderPropertyIds;


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
		//[ Classes                                               ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Internal helper class to iterate through all shader combinations
		*/
		class ShaderCombinationIterator
		{
		public:
			explicit ShaderCombinationIterator(size_t reserveSize)
			{
				mNumberOfPropertyValuesByPropertyIndex.reserve(reserveSize);
				mCurrentCombination.reserve(reserveSize);
			}

			void clear()
			{
				mNumberOfPropertyValuesByPropertyIndex.clear();
				mCurrentCombination.clear();
			}

			void addBoolProperty()
			{
				addIntegerProperty(2);
			}

			void addIntegerProperty(uint32_t numberOfIntegerValues)
			{
				mNumberOfPropertyValuesByPropertyIndex.push_back(numberOfIntegerValues);
			}

			bool getCurrentCombinationBoolProperty(size_t index) const
			{
				return (getCurrentCombinationIntegerProperty(index) > 0);
			}

			uint32_t getCurrentCombinationIntegerProperty(size_t index) const
			{
				assert(index < mCurrentCombination.size());
				return mCurrentCombination[index];
			}

			void startIterate()
			{
				// Start with every property value set to zero
				mCurrentCombination.resize(mNumberOfPropertyValuesByPropertyIndex.size());
				memset(mCurrentCombination.data(), 0, sizeof(uint32_t) * mNumberOfPropertyValuesByPropertyIndex.size());
			}

			bool iterate()
			{
				// Just a sanity check, in case someone forgot to start iterating first
				assert(mCurrentCombination.size() == mNumberOfPropertyValuesByPropertyIndex.size());

				for (size_t index = 0; index < mCurrentCombination.size(); ++index)
				{
					uint32_t& propertyValue = mCurrentCombination[index];
					++propertyValue;
					if (propertyValue < mNumberOfPropertyValuesByPropertyIndex[index])
					{
						// Went up by one, result is valid, so everything is fine
						return true;
					}
					else
					{
						// We have to go to the next property now and increase that one; but first reset this one here to zero again
						propertyValue = 0;
					}
				}

				// We're done with iterating, every property is at its maximum
				return false;
			}
		private:
			std::vector<uint32_t> mNumberOfPropertyValuesByPropertyIndex;
			std::vector<uint32_t> mCurrentCombination;
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
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		// Optimization: To avoid constant allocations/deallocations, use a static instance (not multi-threading safe, of course)
		static RendererRuntime::ShaderProperties g_ShaderPropertiesScratchBuffer(128);
		static ShaderCombinationIterator		 g_ShaderCombinationIterator(128);
		static ShaderPropertyIds				 g_ShaderPropertyIds(128);


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
	const int32_t MaterialBlueprintResource::MANDATORY_SHADER_PROPERTY = std::numeric_limits<int32_t>::max();


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void MaterialBlueprintResource::optimizeShaderProperties(ShaderProperties& shaderProperties) const
	{
		// Optimization: To avoid constant allocations/deallocations, use a static instance (not multi-threading safe, of course)
		ShaderProperties& optimizedShaderProperties = ::detail::g_ShaderPropertiesScratchBuffer;
		optimizedShaderProperties.clear();

		// Gather relevant shader properties
		for (const ShaderProperties::Property& property : shaderProperties.getSortedPropertyVector())
		{
			if (0 != property.value && mVisualImportanceOfShaderProperties.hasPropertyValue(property.shaderPropertyId))
			{
				optimizedShaderProperties.setPropertyValue(property.shaderPropertyId, property.value);
			}
		}

		// Done
		shaderProperties = optimizedShaderProperties;
	}

	bool MaterialBlueprintResource::isFullyLoaded() const
	{
		// Check uniform buffers
		if (nullptr == mPassUniformBuffer || nullptr ==  mMaterialUniformBuffer || nullptr ==  mInstanceUniformBuffer)
		{
			// Not fully loaded
			return false;
		}

		// Get the shader piece resource manager instance
		const IRendererRuntime& rendererRuntime = getResourceManager<MaterialBlueprintResourceManager>().getRendererRuntime();
		const ShaderPieceResourceManager& shaderPieceResourceManager = rendererRuntime.getShaderPieceResourceManager();
		const ShaderBlueprintResourceManager& shaderBlueprintResourceManager = rendererRuntime.getShaderBlueprintResourceManager();

		// Check the rest
		// TODO(co) Handle the other shader types
		return (IResource::LoadingState::LOADED == getLoadingState() && nullptr != mRootSignaturePtr && ::detail::isFullyLoaded(shaderPieceResourceManager, shaderBlueprintResourceManager, mShaderBlueprintResourceId[static_cast<uint8_t>(ShaderType::Vertex)]) && ::detail::isFullyLoaded(shaderPieceResourceManager, shaderBlueprintResourceManager, mShaderBlueprintResourceId[static_cast<uint8_t>(ShaderType::Fragment)]));
	}

	void MaterialBlueprintResource::enforceFullyLoaded()
	{
		// TODO(co) Implement more efficient solution: We need to extend "Runtime::ResourceStreamer" to request emergency immediate processing of requested resources
		ResourceStreamer& resourceStreamer = getResourceManager<MaterialBlueprintResourceManager>().getRendererRuntime().getResourceStreamer();
		while (!isFullyLoaded())
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1ms);
			resourceStreamer.rendererBackendDispatch();
		}
	}

	void MaterialBlueprintResource::fillUnknownUniformBuffers()
	{
		const MaterialBlueprintResourceManager& materialBlueprintResourceManager = getResourceManager<MaterialBlueprintResourceManager>();
		const MaterialProperties& globalMaterialProperties = materialBlueprintResourceManager.getGlobalMaterialProperties();

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = materialBlueprintResourceManager.getMaterialBlueprintResourceListener();
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

	void MaterialBlueprintResource::fillInstanceUniformBuffer(const Transform& objectSpaceToWorldSpaceTransform, MaterialTechnique& materialTechnique)
	{
		// TODO(co) This is just a placeholder implementation until "RendererRuntime::InstanceUniformBufferManager" is ready

		assert(getId() == materialTechnique.getMaterialBlueprintResourceId());
		assert(nullptr != mInstanceUniformBuffer);
		assert(1 == mInstanceUniformBuffer->numberOfElements);	// TODO(co) Implement automatic instancing

		const MaterialBlueprintResourceManager& materialBlueprintResourceManager = getResourceManager<MaterialBlueprintResourceManager>();
		const MaterialProperties& globalMaterialProperties = materialBlueprintResourceManager.getGlobalMaterialProperties();

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = materialBlueprintResourceManager.getMaterialBlueprintResourceListener();
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
			if (nullptr != mPassUniformBufferManager)
			{
				mPassUniformBufferManager->bindToRenderer();
			}
			for (size_t i = 0; i < numberOfUniformBuffers; ++i)
			{
				const UniformBuffer& uniformBuffer = mUniformBuffers[i];
				if (uniformBuffer.uniformBufferPtr.getPointer() != nullptr)
				{
					renderer.setGraphicsRootDescriptorTable(uniformBuffer.rootParameterIndex, uniformBuffer.uniformBufferPtr);
				}
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

		// It's valid if a material blueprint resource doesn't contain a material uniform buffer (usually the case for compositor material blueprint resources)
		if (nullptr != mMaterialUniformBufferManager)
		{
			mMaterialUniformBufferManager->resetLastBoundPool();
		}
	}

	void MaterialBlueprintResource::createPipelineStateCaches(bool mandatoryOnly)
	{
		// Material blueprint resource must be fully loaded, meaning also all referenced shader resources
		assert(isFullyLoaded());

		// TODO(co) Fill dynamic shader pieces
		DynamicShaderPieces dynamicShaderPieces[NUMBER_OF_SHADER_TYPES];

		// Optimization: To avoid constant allocations/deallocations, use a static instance (not multi-threading safe, of course)
		::detail::ShaderCombinationIterator& shaderCombinationIterator = ::detail::g_ShaderCombinationIterator;
		ShaderProperties& shaderProperties = ::detail::g_ShaderPropertiesScratchBuffer;
		::detail::ShaderPropertyIds& shaderPropertyIds = ::detail::g_ShaderPropertyIds;
		shaderCombinationIterator.clear();
		shaderPropertyIds.clear();

		{ // Gather all mandatory shader combination properties
			const MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector = mMaterialProperties.getSortedPropertyVector();
			for (const MaterialProperty& materialProperty : sortedMaterialPropertyVector)
			{
				const MaterialPropertyId materialPropertyId = materialProperty.getMaterialPropertyId();
				if (materialProperty.getUsage() == MaterialProperty::Usage::SHADER_COMBINATION && (!mandatoryOnly || mVisualImportanceOfShaderProperties.getPropertyValueUnsafe(materialPropertyId) == MANDATORY_SHADER_PROPERTY))
				{
					switch (materialProperty.getValueType())
					{
						case MaterialProperty::ValueType::BOOLEAN:
							shaderPropertyIds.push_back(materialProperty.getMaterialPropertyId());	// Shader property ID and material property ID are identical, so this is valid
							shaderCombinationIterator.addBoolProperty();
							break;

						case MaterialProperty::ValueType::INTEGER:
							shaderPropertyIds.push_back(materialProperty.getMaterialPropertyId());	// Shader property ID and material property ID are identical, so this is valid
							shaderCombinationIterator.addIntegerProperty(static_cast<uint32_t>(getMaximumIntegerValueOfShaderProperty(materialPropertyId)));
							break;

						case MaterialProperty::ValueType::UNKNOWN:
						case MaterialProperty::ValueType::INTEGER_2:
						case MaterialProperty::ValueType::INTEGER_3:
						case MaterialProperty::ValueType::INTEGER_4:
						case MaterialProperty::ValueType::FLOAT:
						case MaterialProperty::ValueType::FLOAT_2:
						case MaterialProperty::ValueType::FLOAT_3:
						case MaterialProperty::ValueType::FLOAT_4:
						case MaterialProperty::ValueType::FLOAT_3_3:
						case MaterialProperty::ValueType::FLOAT_4_4:
						case MaterialProperty::ValueType::FILL_MODE:
						case MaterialProperty::ValueType::CULL_MODE:
						case MaterialProperty::ValueType::CONSERVATIVE_RASTERIZATION_MODE:
						case MaterialProperty::ValueType::DEPTH_WRITE_MASK:
						case MaterialProperty::ValueType::STENCIL_OP:
						case MaterialProperty::ValueType::COMPARISON_FUNC:
						case MaterialProperty::ValueType::BLEND:
						case MaterialProperty::ValueType::BLEND_OP:
						case MaterialProperty::ValueType::FILTER_MODE:
						case MaterialProperty::ValueType::TEXTURE_ADDRESS_MODE:
						case MaterialProperty::ValueType::TEXTURE_ASSET_ID:
						case MaterialProperty::ValueType::COMPOSITOR_TEXTURE_REFERENCE:
						default:
							// Unsupported shader combination material property value type
							assert(false);
							break;
					}
				}
			}
		}

		{ // Create the pipeline state caches
			const uint32_t numberOfShaderProperties = static_cast<uint32_t>(shaderPropertyIds.size());
			shaderCombinationIterator.startIterate();
			do
			{
				// Set the current shader properties combination
				// -> The value always starts with 0 and has no holes in enumeration
				shaderProperties.clear();
				for (uint32_t i = 0; i < numberOfShaderProperties; ++i)
				{
					const uint32_t value = shaderCombinationIterator.getCurrentCombinationIntegerProperty(i);
					if (value != 0)
					{
						shaderProperties.setPropertyValue(shaderPropertyIds[i], static_cast<int32_t>(value));
					}
				}

				// Create the current pipeline state cache instances for the material blueprint
				const Renderer::IPipelineStatePtr pipelineStatePtr = mPipelineStateCacheManager.getPipelineStateCacheByCombination(shaderProperties, dynamicShaderPieces, true);
				assert(nullptr != pipelineStatePtr);	// TODO(co) Decent error handling
			}
			while (shaderCombinationIterator.iterate());
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MaterialBlueprintResource::MaterialBlueprintResource() :
		mPipelineStateCacheManager(*this),
		mVertexAttributes(glm::countof(::detail::vertexAttributesLayout), ::detail::vertexAttributesLayout),
		mPipelineState(Renderer::PipelineStateBuilder()),
		mPassUniformBuffer(nullptr),
		mMaterialUniformBuffer(nullptr),
		mInstanceUniformBuffer(nullptr),
		mPassUniformBufferManager(nullptr),
		mMaterialUniformBufferManager(nullptr)
	{
		memset(mShaderBlueprintResourceId, static_cast<int>(getUninitialized<ShaderBlueprintResourceId>()), sizeof(ShaderBlueprintResourceId) * NUMBER_OF_SHADER_TYPES);
	}

	MaterialBlueprintResource::~MaterialBlueprintResource()
	{
		if (nullptr != mPassUniformBufferManager)
		{
			delete mPassUniformBufferManager;
		}
		if (nullptr != mMaterialUniformBufferManager)
		{
			delete mMaterialUniformBufferManager;
		}

		// TODO(co) Sanity checks
		/*
		PipelineStateCacheManager			 mPipelineStateCacheManager;
		MaterialProperties					 mMaterialProperties;
		ShaderProperties					 mVisualImportanceOfShaderProperties;	///< Every shader property known to the material blueprint has a visual importance entry in here
		ShaderProperties					 mMaximumIntegerValueOfShaderProperties;
		Renderer::VertexAttributes			 mVertexAttributes;
		Renderer::IRootSignaturePtr			 mRootSignaturePtr;						///< Root signature, can be a null pointer
		Renderer::PipelineState				 mPipelineState;
		ShaderBlueprintResourceId			 mShaderBlueprintResourceId[NUMBER_OF_SHADER_TYPES];
		// Resource
		UniformBuffers mUniformBuffers;
		SamplerStates  mSamplerStates;
		Textures	   mTextures;
		// Ease-of-use direct access
		UniformBuffer* mPassUniformBuffer;		///< Can be a null pointer, don't destroy the instance
		UniformBuffer* mMaterialUniformBuffer;	///< Can be a null pointer, don't destroy the instance
		UniformBuffer* mInstanceUniformBuffer;	///< Can be a null pointer, don't destroy the instance
		*/
	}

	void MaterialBlueprintResource::initializeElement(MaterialBlueprintResourceId materialBlueprintResourceId)
	{
		// TODO(co) Sanity checks

		// Call base implementation
		IResource::initializeElement(materialBlueprintResourceId);
	}

	void MaterialBlueprintResource::deinitializeElement()
	{
		// TODO(co) Reset everything
		memset(mShaderBlueprintResourceId, static_cast<int>(getUninitialized<ShaderBlueprintResourceId>()), sizeof(ShaderBlueprintResourceId) * NUMBER_OF_SHADER_TYPES);

		// Call base implementation
		IResource::deinitializeElement();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
