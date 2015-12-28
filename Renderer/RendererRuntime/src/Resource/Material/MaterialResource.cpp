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
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResource.h"
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/Shader/ShaderBuilder.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <algorithm>

#include <assert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	namespace detail
	{
		struct OrderByMaterialPropertyId
		{
			inline bool operator()(const MaterialProperty& left, MaterialPropertyId right) const
			{
				return (left.getMaterialPropertyId() < right);
			}

			inline bool operator()(MaterialPropertyId left, const MaterialProperty& right) const
			{
				return (left < right.getMaterialPropertyId());
			}
		};
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialResource::MaterialResource(ResourceId resourceId) :
		IResource(resourceId),
		mMaterialBlueprintResource(nullptr),
		mPipelineState(Renderer::PipelineStateBuilder()),
		mPipelineStateObject(nullptr)
	{
		// Nothing here
	}

	Renderer::IPipelineState* MaterialResource::getPipelineStateObject()
	{
		if (nullptr == mPipelineStateObject && nullptr != mMaterialBlueprintResource && mMaterialBlueprintResource->isFullyLoaded())
		{
			Renderer::IRootSignature* rootSignature = mMaterialBlueprintResource->getRootSignature();
			Renderer::IRenderer& renderer = rootSignature->getRenderer();

			// Decide which shader language should be used (for example "GLSL" or "HLSL")
			Renderer::IShaderLanguagePtr shaderLanguage(renderer.getShaderLanguage());
			if (nullptr != shaderLanguage)
			{
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
						"NORMAL",									// semanticName[32] (char)
						0,											// semanticIndex (uint32_t)
						// Data source
						0,											// inputSlot (uint32_t)
						sizeof(float) * 3 + sizeof(short) * 2,		// alignedByteOffset (uint32_t)
						// Data source, instancing part
						0											// instancesPerElement (uint32_t)
					}
				};
				const Renderer::VertexAttributes vertexAttributes(sizeof(vertexAttributesLayout) / sizeof(Renderer::VertexAttribute), vertexAttributesLayout);

				// Gather shader properties from static material properties
				ShaderProperties shaderProperties;
				{
					const size_t numberOfMaterialProperties = mSortedMaterialPropertyVector.size();
					for (size_t i = 0; i < numberOfMaterialProperties; ++i)
					{
						const MaterialProperty& materialProperty = mSortedMaterialPropertyVector[i];
						if (materialProperty.getUsage() == MaterialProperty::Usage::STATIC)
						{
							shaderProperties.setPropertyValue(materialProperty.getMaterialPropertyId(), materialProperty.getBooleanValue());
						}
					}
				}

				// Create the vertex shader
				Renderer::IVertexShader* vertexShader = nullptr;
				{
					ShaderBuilder shaderBuilder;
					vertexShader = shaderLanguage->createVertexShaderFromSourceCode(shaderBuilder.createSourceCode(*mMaterialBlueprintResource->mVertexShaderBlueprint, shaderProperties).c_str());
				}

				// Create the fragment shader
				Renderer::IFragmentShader* fragmentShader = nullptr;
				{
					ShaderBuilder shaderBuilder;
					fragmentShader = shaderLanguage->createFragmentShaderFromSourceCode(shaderBuilder.createSourceCode(*mMaterialBlueprintResource->mFragmentShaderBlueprint, shaderProperties).c_str());
				}

				// Create the program
				Renderer::IProgram* program = shaderLanguage->createProgram(*rootSignature, vertexAttributes, vertexShader, fragmentShader);

				// Is there a valid program?
				if (nullptr != program)
				{
					// Start with the pipeline state of the material blueprint resource
					mPipelineState = mMaterialBlueprintResource->getPipelineState();

					// Setup the dynamic part of the pipeline state
					mPipelineState.rootSignature	= rootSignature;
					mPipelineState.program			= program;
					mPipelineState.vertexAttributes = vertexAttributes;

					// Create the pipeline state object (PSO)
					mPipelineStateObject = renderer.createPipelineState(mPipelineState);
				}
			}
		}

		// Done
		return mPipelineStateObject;
	}

	void MaterialResource::releasePipelineState()
	{
		mTextures.clear();	// TODO(co) Cleanup
		if (nullptr != mPipelineStateObject)
		{
			mPipelineStateObject->release();
			mPipelineStateObject = nullptr;
		}
	}

	bool MaterialResource::setGraphicsRootDescriptorTable(const RendererRuntime::IRendererRuntime& rendererRuntime)
	{
		assert(nullptr != mMaterialBlueprintResource);

		// TODO(co) This is experimental and will certainly look different when everything is in place
		Renderer::IRenderer& renderer = rendererRuntime.getRenderer();

		{ // Graphics root descriptor table: Set sampler states
			const MaterialBlueprintResource::SamplerStates& samplerStates = mMaterialBlueprintResource->getSamplerStates();
			const size_t numberOfSamplerStates = samplerStates.size();
			for (size_t i = 0; i < numberOfSamplerStates; ++i)
			{
				const MaterialBlueprintResource::SamplerState& samplerState = samplerStates[i];
				renderer.setGraphicsRootDescriptorTable(samplerState.samplerRootParameterIndex, samplerState.samplerStatePtr);
			}
		}

		// Need for gathering the textures now?
		if (mTextures.empty())
		{
			TextureResourceManager& textureResourceManager = rendererRuntime.getTextureResourceManager();
			const MaterialBlueprintResource::Textures& textures = mMaterialBlueprintResource->getTextures();
			const size_t numberOfTextures = textures.size();
			for (size_t i = 0; i < numberOfTextures; ++i)
			{
				const MaterialBlueprintResource::Texture& blueprintTexture = textures[i];

				// Start with the material blueprint textures
				Texture texture;
				texture.textureRootParameterIndex = blueprintTexture.textureRootParameterIndex;
				texture.textureAssetId = blueprintTexture.textureAssetId;
				texture.materialPropertyId = blueprintTexture.materialPropertyId;
				texture.textureResource = blueprintTexture.textureResource;	// TODO(co) Implement decent resource management

				// Apply material specific modifications
				if (0 != texture.materialPropertyId)
				{
					// Figure out the material property value
					const MaterialPropertyId materialPropertyId = texture.materialPropertyId;
					SortedMaterialPropertyVector::const_iterator iterator = std::lower_bound(mSortedMaterialPropertyVector.cbegin(), mSortedMaterialPropertyVector.cend(), materialPropertyId, detail::OrderByMaterialPropertyId());
					if (iterator != mSortedMaterialPropertyVector.end())
					{
						MaterialProperty* materialProperty = iterator._Ptr;
						if (materialProperty->getMaterialPropertyId() == materialPropertyId)
						{
							// TODO(co) Error handling: Usage mismatch etc.
							texture.textureAssetId = materialProperty->getAssetIdValue();
							texture.textureResource = textureResourceManager.loadTextureResourceByAssetId(texture.textureAssetId);	// TODO(co) Implement decent resource management
						}
					}
				}

				// Insert texture
				mTextures.push_back(texture);
			}
		}

		{ // Graphics root descriptor table: Set textures
			const size_t numberOfTextures = mTextures.size();
			for (size_t i = 0; i < numberOfTextures; ++i)
			{
				const Texture& texture = mTextures[i];

				// Due to background texture loading, some textures might not be ready, yet
				// TODO(co) Add dummy textures so rendering also works when textures are not ready, yet
				if (nullptr != texture.textureResource)
				{
					Renderer::ITexturePtr texturePtr = texture.textureResource->getTexture();
					if (nullptr != texturePtr)
					{
						renderer.setGraphicsRootDescriptorTable(texture.textureRootParameterIndex, texturePtr);
					}
				}
			}
		}

		// Done
		return true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
