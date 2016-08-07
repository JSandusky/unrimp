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
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintResourceLoader.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintFileFormat.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId MaterialBlueprintResourceLoader::TYPE_ID("material_blueprint");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void MaterialBlueprintResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream inputFileStream(mAsset.assetFilename, std::ios::binary);

			// Read in the material blueprint header
			v1MaterialBlueprint::Header materialBlueprintHeader;
			inputFileStream.read(reinterpret_cast<char*>(&materialBlueprintHeader), sizeof(v1MaterialBlueprint::Header));

			{ // Read properties
				// TODO(co) Get rid of the evil const-cast
				MaterialProperties::SortedPropertyVector& sortedPropertyVector = const_cast<MaterialProperties::SortedPropertyVector&>(mMaterialBlueprintResource->mMaterialProperties.getSortedPropertyVector());
				sortedPropertyVector.resize(materialBlueprintHeader.numberOfProperties);
				inputFileStream.read(reinterpret_cast<char*>(sortedPropertyVector.data()), sizeof(MaterialProperty) * materialBlueprintHeader.numberOfProperties);
			}

			{ // Read visual importance of shader properties
				// TODO(co) Get rid of the evil const-cast
				ShaderProperties::SortedPropertyVector& sortedPropertyVector = const_cast<ShaderProperties::SortedPropertyVector&>(mMaterialBlueprintResource->mVisualImportanceOfShaderProperties.getSortedPropertyVector());
				sortedPropertyVector.resize(materialBlueprintHeader.numberOfShaderCombinationProperties);
				inputFileStream.read(reinterpret_cast<char*>(sortedPropertyVector.data()), sizeof(ShaderProperties::Property) * materialBlueprintHeader.numberOfShaderCombinationProperties);
			}

			{ // Read maximum integer value of shader properties
				// TODO(co) Get rid of the evil const-cast
				ShaderProperties::SortedPropertyVector& sortedPropertyVector = const_cast<ShaderProperties::SortedPropertyVector&>(mMaterialBlueprintResource->mMaximumIntegerValueOfShaderProperties.getSortedPropertyVector());
				sortedPropertyVector.resize(materialBlueprintHeader.numberOfIntegerShaderCombinationProperties);
				inputFileStream.read(reinterpret_cast<char*>(sortedPropertyVector.data()), sizeof(ShaderProperties::Property) * materialBlueprintHeader.numberOfIntegerShaderCombinationProperties);
			}

			{ // Read in the root signature
				// Read in the root signature header
				v1MaterialBlueprint::RootSignatureHeader rootSignatureHeader;
				inputFileStream.read(reinterpret_cast<char*>(&rootSignatureHeader), sizeof(v1MaterialBlueprint::RootSignatureHeader));

				// Allocate memory for the temporary data
				if (mMaximumNumberOfRootParameters < rootSignatureHeader.numberOfRootParameters)
				{
					delete [] mRootParameters;
					mMaximumNumberOfRootParameters = rootSignatureHeader.numberOfRootParameters;
					mRootParameters = new Renderer::RootParameter[mMaximumNumberOfRootParameters];
				}
				if (mMaximumNumberOfDescriptorRanges < rootSignatureHeader.numberOfDescriptorRanges)
				{
					delete [] mDescriptorRanges;
					mMaximumNumberOfDescriptorRanges = rootSignatureHeader.numberOfDescriptorRanges;
					mDescriptorRanges = new Renderer::DescriptorRange[mMaximumNumberOfDescriptorRanges];
				}

				// Load in the root parameters
				inputFileStream.read(reinterpret_cast<char*>(mRootParameters), sizeof(Renderer::RootParameter) * rootSignatureHeader.numberOfRootParameters);

				// Load in the descriptor ranges
				inputFileStream.read(reinterpret_cast<char*>(mDescriptorRanges), sizeof(Renderer::DescriptorRange) * rootSignatureHeader.numberOfDescriptorRanges);

				// Prepare our temporary root signature
				mRootSignature.numberOfParameters	  = rootSignatureHeader.numberOfRootParameters;
				mRootSignature.parameters			  = mRootParameters;
				mRootSignature.numberOfStaticSamplers = rootSignatureHeader.numberOfStaticSamplers;
				mRootSignature.staticSamplers		  = nullptr;	// TODO(co) Add support for static samplers
				mRootSignature.flags				  = static_cast<Renderer::RootSignatureFlags::Enum>(rootSignatureHeader.flags);

				{ // Tell the temporary root signature about the descriptor ranges
					Renderer::DescriptorRange* descriptorRange = mDescriptorRanges;
					for (uint32_t i = 0; i < rootSignatureHeader.numberOfRootParameters; ++i)
					{
						Renderer::RootParameter& rootParameter = mRootParameters[i];
						if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
						{
							rootParameter.descriptorTable.descriptorRanges = descriptorRange;
							descriptorRange += rootParameter.descriptorTable.numberOfDescriptorRanges;
						}
					}
				}
			}

			{ // Read in the pipeline state
				// Read in the shader blueprints
				inputFileStream.read(reinterpret_cast<char*>(&mShaderBlueprintAssetId), sizeof(AssetId) * NUMBER_OF_SHADER_TYPES);

				// TODO(co) The first few bytes are unused and there are probably byte alignment issues which can come up. On the other hand, this solution is wonderful simple.
				// Read in the pipeline state
				inputFileStream.read(reinterpret_cast<char*>(&mMaterialBlueprintResource->mPipelineState), sizeof(Renderer::PipelineState));
			}

			{ // Read in the uniform buffers
				MaterialBlueprintResource::UniformBuffers& uniformBuffers = mMaterialBlueprintResource->mUniformBuffers;
				uniformBuffers.resize(materialBlueprintHeader.numberOfUniformBuffers);
				for (uint32_t i = 0; i < materialBlueprintHeader.numberOfUniformBuffers; ++i)
				{
					MaterialBlueprintResource::UniformBuffer& uniformBuffer = uniformBuffers[i];

					// Read in the uniform buffer header
					v1MaterialBlueprint::UniformBufferHeader uniformBufferHeader;
					inputFileStream.read(reinterpret_cast<char*>(&uniformBufferHeader), sizeof(v1MaterialBlueprint::UniformBufferHeader));
					uniformBuffer.rootParameterIndex		 = uniformBufferHeader.rootParameterIndex;
					uniformBuffer.uniformBufferUsage		 = uniformBufferHeader.uniformBufferUsage;
					uniformBuffer.numberOfElements			 = uniformBufferHeader.numberOfElements;
					uniformBuffer.uniformBufferNumberOfBytes = uniformBufferHeader.uniformBufferNumberOfBytes;

					// Read in the uniform buffer property elements
					MaterialBlueprintResource::UniformBufferElementProperties& uniformBufferElementProperties = uniformBuffer.uniformBufferElementProperties;
					uniformBufferElementProperties.resize(uniformBufferHeader.numberOfElementProperties);
					inputFileStream.read(reinterpret_cast<char*>(uniformBufferElementProperties.data()), sizeof(MaterialProperty) * uniformBufferHeader.numberOfElementProperties);
				}
			}

			{ // Read in the sampler states
				// Allocate memory for the temporary data
				if (mMaximumNumberOfMaterialBlueprintSamplerStates < materialBlueprintHeader.numberOfSamplerStates)
				{
					delete [] mMaterialBlueprintSamplerStates;
					mMaximumNumberOfMaterialBlueprintSamplerStates = materialBlueprintHeader.numberOfSamplerStates;
					mMaterialBlueprintSamplerStates = new v1MaterialBlueprint::SamplerState[mMaximumNumberOfMaterialBlueprintSamplerStates];
				}

				// Read in the sampler states
				inputFileStream.read(reinterpret_cast<char*>(mMaterialBlueprintSamplerStates), sizeof(v1MaterialBlueprint::SamplerState) * materialBlueprintHeader.numberOfSamplerStates);

				// Allocate material blueprint resource sampler states
				mMaterialBlueprintResource->mSamplerStates.resize(materialBlueprintHeader.numberOfSamplerStates);
			}

			{ // Read in the textures
				// Allocate memory for the temporary data
				if (mMaximumNumberOfMaterialBlueprintTextures < materialBlueprintHeader.numberOfTextures)
				{
					delete [] mMaterialBlueprintTextures;
					mMaximumNumberOfMaterialBlueprintTextures = materialBlueprintHeader.numberOfTextures;
					mMaterialBlueprintTextures = new v1MaterialBlueprint::Texture[mMaximumNumberOfMaterialBlueprintTextures];
				}

				// Read in the textures
				inputFileStream.read(reinterpret_cast<char*>(mMaterialBlueprintTextures), sizeof(v1MaterialBlueprint::Texture) * materialBlueprintHeader.numberOfTextures);

				// Allocate material blueprint resource textures
				mMaterialBlueprintResource->mTextures.resize(materialBlueprintHeader.numberOfTextures);
			}
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load material blueprint asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void MaterialBlueprintResourceLoader::onRendererBackendDispatch()
	{
		Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();

		// Create the root signature
		mMaterialBlueprintResource->mRootSignaturePtr = renderer.createRootSignature(mRootSignature);

		{ // Get the used shader blueprint resources
			ShaderBlueprintResourceManager& shaderBlueprintResourceManager = mRendererRuntime.getShaderBlueprintResourceManager();
			for (uint8_t i = 0; i < NUMBER_OF_SHADER_TYPES; ++i)
			{
				mMaterialBlueprintResource->mShaderBlueprintResourceId[i] = shaderBlueprintResourceManager.loadShaderBlueprintResourceByAssetId(mShaderBlueprintAssetId[i]);
			}
		}

		{ // Create the uniform buffer renderer resources
			MaterialBlueprintResource::UniformBuffers& uniformBuffers = mMaterialBlueprintResource->mUniformBuffers;
			const size_t numberOfUniformBuffers = uniformBuffers.size();
			for (size_t i = 0; i < numberOfUniformBuffers; ++i)
			{
				// Create the uniform buffer renderer resource (GPU alignment is handled by the renderer backend)
				MaterialBlueprintResource::UniformBuffer& uniformBuffer = uniformBuffers[i];
				uniformBuffer.scratchBuffer.resize(uniformBuffer.uniformBufferNumberOfBytes);
				uniformBuffer.uniformBufferPtr = renderer.createUniformBuffer(uniformBuffer.uniformBufferNumberOfBytes, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);

				// Ease-of-use direct access
				if (MaterialBlueprintResource::UniformBufferUsage::PASS == uniformBuffer.uniformBufferUsage)
				{
					mMaterialBlueprintResource->mPassUniformBuffer = &uniformBuffer;
				}
				else if (MaterialBlueprintResource::UniformBufferUsage::MATERIAL == uniformBuffer.uniformBufferUsage)
				{
					mMaterialBlueprintResource->mMaterialUniformBuffer = &uniformBuffer;
				}
				else if (MaterialBlueprintResource::UniformBufferUsage::INSTANCE == uniformBuffer.uniformBufferUsage)
				{
					mMaterialBlueprintResource->mInstanceUniformBuffer = &uniformBuffer;
				}
			}
		}

		{ // Create the sampler states
			MaterialBlueprintResource::SamplerStates& samplerStates = mMaterialBlueprintResource->mSamplerStates;
			const size_t numberOfSamplerStates = samplerStates.size();
			const v1MaterialBlueprint::SamplerState* materialBlueprintSamplerState = mMaterialBlueprintSamplerStates;
			for (size_t i = 0; i < numberOfSamplerStates; ++i, ++materialBlueprintSamplerState)
			{
				MaterialBlueprintResource::SamplerState& samplerState = samplerStates[i];
				samplerState.rootParameterIndex = materialBlueprintSamplerState->rootParameterIndex;
				samplerState.samplerStatePtr = renderer.createSamplerState(*materialBlueprintSamplerState);
			}
		}

		{ // Get the textures
			TextureResourceManager& textureResourceManager = mRendererRuntime.getTextureResourceManager();
			MaterialBlueprintResource::Textures& textures = mMaterialBlueprintResource->mTextures;
			const size_t numberOfTextures = textures.size();
			const v1MaterialBlueprint::Texture* materialBlueprintTexture = mMaterialBlueprintTextures;
			for (size_t i = 0; i < numberOfTextures; ++i, ++materialBlueprintTexture)
			{
				MaterialBlueprintResource::Texture& texture = textures[i];
				texture.rootParameterIndex = materialBlueprintTexture->rootParameterIndex;
				const MaterialProperty& materialProperty = texture.materialProperty = materialBlueprintTexture->materialProperty;
				if (materialProperty.getValueType() == MaterialPropertyValue::ValueType::TEXTURE_ASSET_ID)
				{
					texture.textureResourceId = textureResourceManager.loadTextureResourceByAssetId(materialProperty.getTextureAssetIdValue());
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MaterialBlueprintResourceLoader::~MaterialBlueprintResourceLoader()
	{
		// Free temporary data
		delete [] mRootParameters;
		delete [] mDescriptorRanges;
		delete [] mMaterialBlueprintSamplerStates;
		delete [] mMaterialBlueprintTextures;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
