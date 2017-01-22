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
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintResourceLoader.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintFileFormat.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/PassBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/MaterialBufferManager.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Core/File/IFile.h"
#include "RendererRuntime/IRendererRuntime.h"


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
	void MaterialBlueprintResourceLoader::onDeserialization(IFile& file)
	{
		// Read in the material blueprint header
		v1MaterialBlueprint::Header materialBlueprintHeader;
		file.read(&materialBlueprintHeader, sizeof(v1MaterialBlueprint::Header));

		{ // Read properties
			// TODO(co) Get rid of the evil const-cast
			MaterialProperties::SortedPropertyVector& sortedPropertyVector = const_cast<MaterialProperties::SortedPropertyVector&>(mMaterialBlueprintResource->mMaterialProperties.getSortedPropertyVector());
			sortedPropertyVector.resize(materialBlueprintHeader.numberOfProperties);
			file.read(sortedPropertyVector.data(), sizeof(MaterialProperty) * materialBlueprintHeader.numberOfProperties);
		}

		{ // Read visual importance of shader properties
			// TODO(co) Get rid of the evil const-cast
			ShaderProperties::SortedPropertyVector& sortedPropertyVector = const_cast<ShaderProperties::SortedPropertyVector&>(mMaterialBlueprintResource->mVisualImportanceOfShaderProperties.getSortedPropertyVector());
			sortedPropertyVector.resize(materialBlueprintHeader.numberOfShaderCombinationProperties);
			file.read(sortedPropertyVector.data(), sizeof(ShaderProperties::Property) * materialBlueprintHeader.numberOfShaderCombinationProperties);
		}

		{ // Read maximum integer value of shader properties
			// TODO(co) Get rid of the evil const-cast
			ShaderProperties::SortedPropertyVector& sortedPropertyVector = const_cast<ShaderProperties::SortedPropertyVector&>(mMaterialBlueprintResource->mMaximumIntegerValueOfShaderProperties.getSortedPropertyVector());
			sortedPropertyVector.resize(materialBlueprintHeader.numberOfIntegerShaderCombinationProperties);
			file.read(sortedPropertyVector.data(), sizeof(ShaderProperties::Property) * materialBlueprintHeader.numberOfIntegerShaderCombinationProperties);
		}

		{ // Read in the root signature
			// Read in the root signature header
			v1MaterialBlueprint::RootSignatureHeader rootSignatureHeader;
			file.read(&rootSignatureHeader, sizeof(v1MaterialBlueprint::RootSignatureHeader));

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
			file.read(mRootParameters, sizeof(Renderer::RootParameter) * rootSignatureHeader.numberOfRootParameters);

			// Load in the descriptor ranges
			file.read(mDescriptorRanges, sizeof(Renderer::DescriptorRange) * rootSignatureHeader.numberOfDescriptorRanges);

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
						rootParameter.descriptorTable.descriptorRanges = reinterpret_cast<uintptr_t>(descriptorRange);
						descriptorRange += rootParameter.descriptorTable.numberOfDescriptorRanges;
					}
				}
			}
		}

		{ // Read in the pipeline state
			// Read in the shader blueprints
			file.read(&mShaderBlueprintAssetId, sizeof(AssetId) * NUMBER_OF_SHADER_TYPES);

			// Read in the pipeline state
			file.read(&mMaterialBlueprintResource->mPipelineState, sizeof(Renderer::SerializedPipelineState));
			mMaterialBlueprintResource->mPipelineState.rootSignature = nullptr;
			mMaterialBlueprintResource->mPipelineState.program = nullptr;
			mMaterialBlueprintResource->mPipelineState.vertexAttributes.numberOfAttributes = 0;
			mMaterialBlueprintResource->mPipelineState.vertexAttributes.attributes = nullptr;
		}

		{ // Read in the uniform buffers
			MaterialBlueprintResource::UniformBuffers& uniformBuffers = mMaterialBlueprintResource->mUniformBuffers;
			uniformBuffers.resize(materialBlueprintHeader.numberOfUniformBuffers);
			for (uint32_t i = 0; i < materialBlueprintHeader.numberOfUniformBuffers; ++i)
			{
				MaterialBlueprintResource::UniformBuffer& uniformBuffer = uniformBuffers[i];

				// Read in the uniform buffer header
				v1MaterialBlueprint::UniformBufferHeader uniformBufferHeader;
				file.read(&uniformBufferHeader, sizeof(v1MaterialBlueprint::UniformBufferHeader));
				uniformBuffer.rootParameterIndex		 = uniformBufferHeader.rootParameterIndex;
				uniformBuffer.bufferUsage				 = uniformBufferHeader.bufferUsage;
				uniformBuffer.numberOfElements			 = uniformBufferHeader.numberOfElements;
				uniformBuffer.uniformBufferNumberOfBytes = uniformBufferHeader.uniformBufferNumberOfBytes;

				// Read in the uniform buffer property elements
				MaterialBlueprintResource::UniformBufferElementProperties& uniformBufferElementProperties = uniformBuffer.uniformBufferElementProperties;
				uniformBufferElementProperties.resize(uniformBufferHeader.numberOfElementProperties);
				file.read(uniformBufferElementProperties.data(), sizeof(MaterialProperty) * uniformBufferHeader.numberOfElementProperties);
			}
		}

		{ // Read in the texture buffers
			MaterialBlueprintResource::TextureBuffers& textureBuffers = mMaterialBlueprintResource->mTextureBuffers;
			textureBuffers.resize(materialBlueprintHeader.numberOfTextureBuffers);
			for (uint32_t i = 0; i < materialBlueprintHeader.numberOfTextureBuffers; ++i)
			{
				MaterialBlueprintResource::TextureBuffer& textureBuffer = textureBuffers[i];

				// Read in the texture buffer header
				v1MaterialBlueprint::TextureBufferHeader textureBufferHeader;
				file.read(&textureBufferHeader, sizeof(v1MaterialBlueprint::TextureBufferHeader));
				textureBuffer.materialPropertyValue		 = textureBufferHeader.materialPropertyValue;
				textureBuffer.rootParameterIndex		 = textureBufferHeader.rootParameterIndex;
				textureBuffer.bufferUsage				 = textureBufferHeader.bufferUsage;
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
			file.read(mMaterialBlueprintSamplerStates, sizeof(v1MaterialBlueprint::SamplerState) * materialBlueprintHeader.numberOfSamplerStates);

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
			file.read(mMaterialBlueprintTextures, sizeof(v1MaterialBlueprint::Texture) * materialBlueprintHeader.numberOfTextures);

			// Allocate material blueprint resource textures
			mMaterialBlueprintResource->mTextures.resize(materialBlueprintHeader.numberOfTextures);
		}
	}

	bool MaterialBlueprintResourceLoader::onDispatch()
	{
		Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();

		// Create the root signature
		mMaterialBlueprintResource->mRootSignaturePtr = renderer.createRootSignature(mRootSignature);
		RENDERER_SET_RESOURCE_DEBUG_NAME(mMaterialBlueprintResource->mRootSignaturePtr, getAsset().assetFilename)

		{ // Get the used shader blueprint resources
			ShaderBlueprintResourceManager& shaderBlueprintResourceManager = mRendererRuntime.getShaderBlueprintResourceManager();
			for (uint8_t i = 0; i < NUMBER_OF_SHADER_TYPES; ++i)
			{
				mMaterialBlueprintResource->mShaderBlueprintResourceId[i] = shaderBlueprintResourceManager.loadShaderBlueprintResourceByAssetId(mShaderBlueprintAssetId[i]);
			}
		}

		{ // Gather ease-of-use direct access to resources
			MaterialBlueprintResource::UniformBuffers& uniformBuffers = mMaterialBlueprintResource->mUniformBuffers;
			const size_t numberOfUniformBuffers = uniformBuffers.size();
			for (size_t i = 0; i < numberOfUniformBuffers; ++i)
			{
				MaterialBlueprintResource::UniformBuffer& uniformBuffer = uniformBuffers[i];
				switch (uniformBuffer.bufferUsage)
				{
					case MaterialBlueprintResource::BufferUsage::UNKNOWN:
						// Nothing here
						break;

					case MaterialBlueprintResource::BufferUsage::PASS:
						mMaterialBlueprintResource->mPassUniformBuffer = &uniformBuffer;
						break;

					case MaterialBlueprintResource::BufferUsage::MATERIAL:
						mMaterialBlueprintResource->mMaterialUniformBuffer = &uniformBuffer;
						break;

					case MaterialBlueprintResource::BufferUsage::INSTANCE:
						mMaterialBlueprintResource->mInstanceUniformBuffer = &uniformBuffer;
						break;

					case MaterialBlueprintResource::BufferUsage::LIGHT:
						// Error!
						assert(false);
						break;
				}
			}
		}

		{ // Gather ease-of-use direct access to resources
			MaterialBlueprintResource::TextureBuffers& textureBuffers = mMaterialBlueprintResource->mTextureBuffers;
			const size_t numberOfTextureBuffers = textureBuffers.size();
			for (size_t i = 0; i < numberOfTextureBuffers; ++i)
			{
				MaterialBlueprintResource::TextureBuffer& textureBuffer = textureBuffers[i];
				switch (textureBuffer.bufferUsage)
				{
					case MaterialBlueprintResource::BufferUsage::UNKNOWN:
					case MaterialBlueprintResource::BufferUsage::PASS:
					case MaterialBlueprintResource::BufferUsage::MATERIAL:
						// Nothing here
						break;

					case MaterialBlueprintResource::BufferUsage::INSTANCE:
						mMaterialBlueprintResource->mInstanceTextureBuffer = &textureBuffer;
						break;

					case MaterialBlueprintResource::BufferUsage::LIGHT:
						mMaterialBlueprintResource->mLightTextureBuffer = &textureBuffer;
						break;
				}
			}
		}

		// Create pass buffer manager
		if (nullptr != mMaterialBlueprintResource->mPassBufferManager)
		{
			delete mMaterialBlueprintResource->mPassBufferManager;
		}
		mMaterialBlueprintResource->mPassBufferManager = new PassBufferManager(mRendererRuntime, *mMaterialBlueprintResource);

		// Create material buffer manager
		if (nullptr != mMaterialBlueprintResource->mMaterialBufferManager)
		{
			delete mMaterialBlueprintResource->mMaterialBufferManager;
			mMaterialBlueprintResource->mMaterialBufferManager = nullptr;
		}
		{ // It's valid if a material blueprint resource doesn't contain a material uniform buffer (usually the case for compositor material blueprint resources)
			const MaterialBlueprintResource::UniformBuffer* uniformBuffer = mMaterialBlueprintResource->getMaterialUniformBuffer();
			if (nullptr != uniformBuffer && mRendererRuntime.getRenderer().getCapabilities().maximumUniformBufferSize > 0)
			{
				mMaterialBlueprintResource->mMaterialBufferManager = new MaterialBufferManager(mRendererRuntime, *mMaterialBlueprintResource);
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
				RENDERER_SET_RESOURCE_DEBUG_NAME(samplerState.samplerStatePtr, getAsset().assetFilename)
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
				texture.rgbHardwareGammaCorrection = materialBlueprintTexture->rgbHardwareGammaCorrection;
				if (materialProperty.getValueType() == MaterialPropertyValue::ValueType::TEXTURE_ASSET_ID)
				{
					texture.textureResourceId = textureResourceManager.loadTextureResourceByAssetId(materialProperty.getTextureAssetIdValue(), nullptr, texture.rgbHardwareGammaCorrection);
				}
			}
		}

		// Fully loaded?
		return isFullyLoaded();
	}

	bool MaterialBlueprintResourceLoader::isFullyLoaded()
	{
		// We only demand that all referenced shader blueprint resources are loaded, not yet loaded texture resources can be handled during runtime
		const ShaderBlueprintResourceManager& shaderBlueprintResourceManager = mRendererRuntime.getShaderBlueprintResourceManager();
		for (uint8_t i = 0; i < NUMBER_OF_SHADER_TYPES; ++i)
		{
			const ShaderBlueprintResourceId shaderBlueprintResourceId = mMaterialBlueprintResource->mShaderBlueprintResourceId[i];
			if (isInitialized(shaderBlueprintResourceId) && IResource::LoadingState::LOADED != shaderBlueprintResourceManager.getResourceByResourceId(shaderBlueprintResourceId).getLoadingState())
			{
				// Not fully loaded
				return false;
			}
		}

		// Fully loaded
		return true;
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
