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
#include "RendererRuntime/Resource/Material/Loader/MaterialResourceLoader.h"
#include "RendererRuntime/Resource/Material/Loader/MaterialFileFormat.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
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
	const ResourceLoaderTypeId MaterialResourceLoader::TYPE_ID("material");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	ResourceLoaderTypeId MaterialResourceLoader::getResourceLoaderTypeId() const
	{
		return TYPE_ID;
	}

	void MaterialResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream inputFileStream(mAsset.assetFilename, std::ios::binary);

			// Read in the material header
			v1Material::Header materialHeader;
			inputFileStream.read(reinterpret_cast<char*>(&materialHeader), sizeof(v1Material::Header));
			mMaterialBlueprintAssetId = materialHeader.materialBlueprintAssetId;

			// TODO(co) Material properties
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load material asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void MaterialResourceLoader::onProcessing()
	{
		// Nothing here
	}

	void MaterialResourceLoader::onRendererBackendDispatch()
	{
		// Get the used material blueprint resource
		mMaterialResource->mMaterialBlueprintResource = mRendererRuntime.getMaterialBlueprintResourceManager().loadMaterialBlueprintResourceByAssetId(mMaterialBlueprintAssetId);

		// TODO(co)
		{ // Load in the diffuse, emissive, normal and specular texture
			// -> The tangent space normal map is stored with three components, two would be enough to recalculate the third component within the fragment shader
			// -> The specular map could be put into the alpha channel of the diffuse map instead of storing it as an individual texture
			RendererRuntime::TextureResourceManager& textureResourceManager = mRendererRuntime.getTextureResourceManager();
			mMaterialResource->mDiffuseTextureResource  = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_Diffuse");
			mMaterialResource->mNormalTextureResource   = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_Illumination");
			mMaterialResource->mSpecularTextureResource = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_norm");
			mMaterialResource->mEmissiveTextureResource = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_spec");
		}

		{ // Create sampler state
			Renderer::SamplerState samplerStateSettings = Renderer::ISamplerState::getDefaultSamplerState();
			samplerStateSettings.addressU = Renderer::TextureAddressMode::WRAP;
			samplerStateSettings.addressV = Renderer::TextureAddressMode::WRAP;
			mMaterialResource->mSamplerState = mRendererRuntime.getRenderer().createSamplerState(samplerStateSettings);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
