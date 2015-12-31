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
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResource.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResource.h"
#include "RendererRuntime/Core/Transform.h"

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtx/quaternion.hpp>
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	namespace detail
	{

		// Define constants
		#define DEFINE_CONSTANT(name) static const StringId name(#name);
			DEFINE_CONSTANT(WORLD_SPACE_TO_VIEW_SPACE_MATRIX)
			DEFINE_CONSTANT(WORLD_SPACE_TO_CLIP_SPACE_MATRIX)
			DEFINE_CONSTANT(OBJECT_SPACE_TO_WORLD_SPACE_MATRIX)
			DEFINE_CONSTANT(MATERIAL_INDEX)
		#undef DEFINE_CONSTANT

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
	MaterialBlueprintResource::MaterialBlueprintResource(ResourceId resourceId) :
		IResource(resourceId),
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

	void MaterialBlueprintResource::fillPassUniformBuffer(const Transform& worldSpaceToViewSpaceTransform) const
	{
		assert(nullptr != mPassUniformBuffer);
		assert(1 == mPassUniformBuffer->numberOfElements);

		Renderer::IRenderer& renderer = mPassUniformBuffer->uniformBufferPtr->getRenderer();

		// Get the aspect ratio
		float aspectRatio = 4.0f / 3.0f;
		{
			// Get the render target with and height
			uint32_t width  = 1;
			uint32_t height = 1;
			Renderer::IRenderTarget *renderTarget = renderer.omGetRenderTarget();
			if (nullptr != renderTarget)
			{
				renderTarget->getWidthAndHeight(width, height);

				// Get the aspect ratio
				aspectRatio = static_cast<float>(width) / height;
			}
		}

		// Calculate standard matrices
		// TODO(co) Tiny optimization for later on: Calculate only the matrices required
		const glm::mat4 viewSpaceToClipSpaceMatrix	= glm::perspective(45.0f, aspectRatio, 0.1f, 100.f);	// TODO(co) Use dynamic values
		const glm::mat4 viewTranslateMatrix			= glm::translate(glm::mat4(1.0f), worldSpaceToViewSpaceTransform.position);
		const glm::mat4 worldSpaceToViewSpaceMatrix	= viewTranslateMatrix * glm::toMat4(worldSpaceToViewSpaceTransform.rotation);
		const glm::mat4 worldSpaceToClipSpaceMatrix	= viewSpaceToClipSpaceMatrix * worldSpaceToViewSpaceMatrix;

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
				if (MaterialProperty::Usage::REFERENCE == uniformBufferElementProperty.getUsage())
				{
					// Resolve the property reference
					// TODO(co) Add more of those automatic values, maybe even a listener interface to allow customization
					const uint32_t referenceValue = uniformBufferElementProperty.getReferenceValue();
					if (detail::WORLD_SPACE_TO_VIEW_SPACE_MATRIX == referenceValue)
					{
						memcpy(scratchBufferPointer, glm::value_ptr(worldSpaceToViewSpaceMatrix), valueTypeNumberOfBytes);
					}
					else if (detail::WORLD_SPACE_TO_CLIP_SPACE_MATRIX == referenceValue)
					{
						memcpy(scratchBufferPointer, glm::value_ptr(worldSpaceToClipSpaceMatrix), valueTypeNumberOfBytes);
					}

					// TODO(co) This is already special and better handled by customized listener
					else if (StringId("VIEW_SPACE_SUN_LIGHT_DIRECTION") == referenceValue)
					{
						glm::vec3 viewSpaceLightDirection(0.5f, 0.5f, 1.0f);
						viewSpaceLightDirection = glm::normalize(viewSpaceLightDirection);
						memcpy(scratchBufferPointer, glm::value_ptr(viewSpaceLightDirection), valueTypeNumberOfBytes);
					}
				}
				else
				{
					// Just copy over the property value
					memcpy(scratchBufferPointer, uniformBufferElementProperty.getData(), valueTypeNumberOfBytes);
				}

				// Next property
				scratchBufferPointer += valueTypeNumberOfBytes;
			}
		}

		// Update the uniform buffer by using our scratch buffer
		mPassUniformBuffer->uniformBufferPtr->copyDataFrom(scratchBuffer.size(), scratchBuffer.data());
	}

	void MaterialBlueprintResource::fillMaterialUniformBuffer() const
	{
		assert(nullptr != mMaterialUniformBuffer);

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
					if (MaterialProperty::Usage::REFERENCE == uniformBufferElementProperty.getUsage())
					{
						// Resolve the property reference
						const uint32_t referenceValue = uniformBufferElementProperty.getReferenceValue();

						// Figure out the material property value
						const MaterialProperty* materialProperty = materialResource->getMaterialPropertyById(referenceValue);
						if (nullptr != materialProperty)
						{
							// TODO(co) Error handling: Usage mismatch, value type mismatch etc.
							memcpy(scratchBufferPointer, materialProperty->getData(), valueTypeNumberOfBytes);
						}
					}
					else
					{
						// Just copy over the property value
						memcpy(scratchBufferPointer, uniformBufferElementProperty.getData(), valueTypeNumberOfBytes);
					}

					// Next property
					scratchBufferPointer += valueTypeNumberOfBytes;
				}
			}
		}

		// Update the uniform buffer by using our scratch buffer
		mMaterialUniformBuffer->uniformBufferPtr->copyDataFrom(scratchBuffer.size(), scratchBuffer.data());
	}

	void MaterialBlueprintResource::fillInstanceUniformBuffer(const Transform& objectSpaceToWorldSpaceTransform, MaterialResource& materialResource) const
	{
		assert(this == materialResource.getMaterialBlueprintResource());
		assert(nullptr != mInstanceUniformBuffer);
		assert(1 == mInstanceUniformBuffer->numberOfElements);	// TODO(co) Implement automatic instancing

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
				if (MaterialProperty::Usage::REFERENCE == uniformBufferElementProperty.getUsage())
				{
					// Resolve the property reference
					// TODO(co) Add more of those automatic values, maybe even a listener interface to allow customization
					const uint32_t referenceValue = uniformBufferElementProperty.getReferenceValue();
					if (detail::OBJECT_SPACE_TO_WORLD_SPACE_MATRIX == referenceValue)
					{
						glm::mat4 objectSpaceToWorldSpaceMatrix;
						objectSpaceToWorldSpaceTransform.getAsMatrix(objectSpaceToWorldSpaceMatrix);
						memcpy(scratchBufferPointer, glm::value_ptr(objectSpaceToWorldSpaceMatrix), valueTypeNumberOfBytes);
					}
					else if (detail::MATERIAL_INDEX == referenceValue)
					{
						const int materialIndex = static_cast<int>(materialResource.getMaterialUniformBufferIndex());
						memcpy(scratchBufferPointer, &materialIndex, valueTypeNumberOfBytes);
					}
				}
				else
				{
					// Just copy over the property value
					memcpy(scratchBufferPointer, uniformBufferElementProperty.getData(), valueTypeNumberOfBytes);
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
				renderer.setGraphicsRootDescriptorTable(uniformBuffer.uniformBufferRootParameterIndex, uniformBuffer.uniformBufferPtr);
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
