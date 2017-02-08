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
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/Resource/Texture/Loader/CrnTextureResourceLoader.h"
#include "RendererRuntime/Resource/Texture/Loader/KtxTextureResourceLoader.h"
#include "RendererRuntime/Resource/Texture/Loader/DdsTextureResourceLoader.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void createDefaultDynamicTextureAssets(RendererRuntime::IRendererRuntime& rendererRuntime, RendererRuntime::TextureResourceManager& textureResourceManager)
		{
			Renderer::ITextureManager& textureManager = rendererRuntime.getTextureManager();

			// White RGB texture
			const uint8_t whiteRgbData[] = { 255, 255, 255, 255 };
			Renderer::ITexturePtr whiteRgb1DTexturePtr(textureManager.createTexture1D(1, Renderer::TextureFormat::R8G8B8A8, whiteRgbData));
			RENDERER_SET_RESOURCE_DEBUG_NAME(whiteRgb1DTexturePtr, "White 1D RGB texture")
			Renderer::ITexturePtr whiteRgb2DTexturePtr(textureManager.createTexture2D(1, 1, Renderer::TextureFormat::R8G8B8A8, whiteRgbData));
			RENDERER_SET_RESOURCE_DEBUG_NAME(whiteRgb2DTexturePtr, "White 2D RGB texture")

			// Normal map identity texture
			const uint8_t normalMapIdentityData[] = { 128, 128, 255, 255 };
			Renderer::ITexturePtr normalMapIdentity2DTexturePtr(textureManager.createTexture2D(1, 1, Renderer::TextureFormat::R8G8B8A8, normalMapIdentityData));
			RENDERER_SET_RESOURCE_DEBUG_NAME(normalMapIdentity2DTexturePtr, "2D normal map identity texture")

			// White alpha texture
			const uint8_t whiteAData[] = { 255 };
			Renderer::ITexturePtr whiteA2DTexturePtr(textureManager.createTexture2D(1, 1, Renderer::TextureFormat::A8, whiteAData));
			RENDERER_SET_RESOURCE_DEBUG_NAME(whiteA2DTexturePtr, "White 2D alpha texture")

			// Black RGB texture
			const uint8_t blackRgbData[] = { 0, 0, 0, 0 };
			Renderer::ITexturePtr blackRgb1DTexturePtr(textureManager.createTexture1D(1, Renderer::TextureFormat::R8G8B8A8, blackRgbData));
			RENDERER_SET_RESOURCE_DEBUG_NAME(blackRgb1DTexturePtr, "Black 1D RGB texture")
			Renderer::ITexturePtr blackRgb2DTexturePtr(textureManager.createTexture2D(1, 1, Renderer::TextureFormat::R8G8B8A8, blackRgbData));
			RENDERER_SET_RESOURCE_DEBUG_NAME(blackRgb2DTexturePtr, "Black 2D RGB texture")

			// Create default dynamic texture assets
			textureResourceManager.createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/WhiteMap1D",			*whiteRgb1DTexturePtr);
			textureResourceManager.createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/WhiteMap2D",			*whiteRgb2DTexturePtr);
			textureResourceManager.createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/BlackMap1D",			*blackRgb1DTexturePtr);
			textureResourceManager.createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/BlackMap2D",			*blackRgb2DTexturePtr);
			textureResourceManager.createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/IdentityDiffuseMap2D",	*whiteRgb2DTexturePtr);
			textureResourceManager.createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/IdentityAlphaMap2D",	*whiteA2DTexturePtr);
			textureResourceManager.createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/IdentityNormalMap2D",	*normalMapIdentity2DTexturePtr);
			textureResourceManager.createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/IdentitySpecularMap2D",	*whiteA2DTexturePtr);
			textureResourceManager.createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/IdentityEmissiveMap2D",	*blackRgb2DTexturePtr);
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
	TextureResource* TextureResourceManager::getTextureResourceByAssetId(AssetId assetId) const
	{
		TextureResource* textureResource = nullptr;

		// TODO(co) Just a quick'n'dirty implementation
		const uint32_t numberOfElements = mTextureResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			TextureResource& currentTextureResource = mTextureResources.getElementByIndex(i);
			if (currentTextureResource.getAssetId() == assetId)
			{
				textureResource = &currentTextureResource;

				// Get us out of the loop
				i = numberOfElements;
			}
		}

		// Done
		return textureResource;
	}

	// TODO(co) Work-in-progress
	TextureResourceId TextureResourceManager::loadTextureResourceByAssetId(AssetId assetId, IResourceListener* resourceListener, AssetId fallbackTextureAssetId, bool rgbHardwareGammaCorrection, bool reload)
	{
		TextureResourceId textureResourceId = getUninitialized<TextureResourceId>();

		// Check whether or not the texture resource already exists
		TextureResource* textureResource = getTextureResourceByAssetId(assetId);

		// Create the resource instance
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		bool load = (reload && nullptr != asset);
		if (nullptr == textureResource && nullptr != asset)
		{
			textureResource = &mTextureResources.addElement();
			textureResource->setResourceManager(this);
			textureResource->setAssetId(assetId);
			textureResource->mRgbHardwareGammaCorrection = rgbHardwareGammaCorrection;
			load = true;
		}
		if (nullptr != textureResource)
		{
			if (nullptr != resourceListener)
			{
				textureResource->connectResourceListener(*resourceListener);
			}
			textureResourceId = textureResource->getId();
		}

		// Load the resource, if required
		if (load)
		{
			// Prepare the resource loader
			// -> The totally primitive texture resource loader type detection is sufficient for now
			const char* filenameExtension = strrchr(&asset->assetFilename[0], '.');
			if (nullptr != filenameExtension)
			{
				ITextureResourceLoader* textureResourceLoader = static_cast<ITextureResourceLoader*>(acquireResourceLoaderInstance(StringId(filenameExtension + 1)));
				if (nullptr != textureResourceLoader)
				{
					textureResourceLoader->initialize(*asset, *textureResource);

					// Commit resource streamer asset load request
					ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
					resourceStreamerLoadRequest.resource = textureResource;
					resourceStreamerLoadRequest.resourceLoader = textureResourceLoader;
					mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);

					// Since it might take a moment to load the texture resource, we'll use a fallback placeholder renderer texture resource so we don't have to wait until the real thing is there
					if (isInitialized(fallbackTextureAssetId))
					{
						const TextureResource* fallbackTextureResource = getTextureResourceByAssetId(fallbackTextureAssetId);
						if (nullptr != fallbackTextureResource)
						{
							textureResource->mTexture = fallbackTextureResource->getTexture();
							textureResource->setLoadingState(IResource::LoadingState::LOADED);
						}
						else
						{
							// Error! Fallback texture asset ID not found.
							assert(false);
						}
					}
					else
					{
						// Hiccups / lags warning: There should always be a fallback texture asset ID (better be safe than sorry)
						assert(false);
					}
				}
			}
			else
			{
				// TODO(co) Error handling
			}
		}

		// Done
		return textureResourceId;
	}

	TextureResourceId TextureResourceManager::createTextureResourceByAssetId(AssetId assetId, Renderer::ITexture& texture, bool rgbHardwareGammaCorrection)
	{
		// Texture resource is not allowed to exist, yet
		assert(isUninitialized(loadTextureResourceByAssetId(assetId)));

		// Create the texture resource instance
		TextureResource& textureResource = mTextureResources.addElement();
		textureResource.setResourceManager(this);
		textureResource.setAssetId(assetId);
		textureResource.mRgbHardwareGammaCorrection = rgbHardwareGammaCorrection;	// TODO(co) We might need to extend "Renderer::ITexture" so we can readback the texture format
		textureResource.mTexture = &texture;

		// Done
		setResourceLoadingState(textureResource, IResource::LoadingState::LOADED);
		return textureResource.getId();
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void TextureResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mTextureResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			const TextureResource& textureResource = mTextureResources.getElementByIndex(i);
			if (textureResource.getAssetId() == assetId)
			{
				loadTextureResourceByAssetId(assetId, nullptr, getUninitialized<AssetId>(), textureResource.isRgbHardwareGammaCorrection(), true);
				break;
			}
		}
	}

	void TextureResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	TextureResourceManager::TextureResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime)
	{
		::detail::createDefaultDynamicTextureAssets(rendererRuntime, *this);
	}

	IResourceLoader* TextureResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			if (resourceLoaderTypeId == CrnTextureResourceLoader::TYPE_ID)
			{
				resourceLoader = new CrnTextureResourceLoader(*this, mRendererRuntime);
			}
			else if (resourceLoaderTypeId == KtxTextureResourceLoader::TYPE_ID)
			{
				resourceLoader = new KtxTextureResourceLoader(*this, mRendererRuntime);
			}
			else if (resourceLoaderTypeId == DdsTextureResourceLoader::TYPE_ID)
			{
				resourceLoader = new DdsTextureResourceLoader(*this, mRendererRuntime);
			}
			else
			{
				// TODO(co) Error handling
				assert(false);
			}
			if (nullptr != resourceLoader)
			{
				mUsedResourceLoaderInstances.push_back(resourceLoader);
			}
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
