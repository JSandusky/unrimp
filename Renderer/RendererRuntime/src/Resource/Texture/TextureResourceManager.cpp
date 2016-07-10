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
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/Resource/Texture/Loader/CrnTextureResourceLoader.h"
#include "RendererRuntime/Resource/Texture/Loader/KtxTextureResourceLoader.h"
#include "RendererRuntime/Resource/Texture/Loader/DdsTextureResourceLoader.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <assert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureResourceId TextureResourceManager::loadTextureResourceByAssetId(AssetId assetId, IResourceListener* resourceListener, bool reload)
	{
		TextureResourceId textureResourceId = getUninitialized<TextureResourceId>();

		// Get or create the instance
		TextureResource* textureResource = nullptr;
		{
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
		}

		// Create the resource instance
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		bool load = (reload && nullptr != asset);
		if (nullptr == textureResource && nullptr != asset)
		{
			textureResource = &mTextureResources.addElement();
			textureResource->setAssetId(assetId);
			load = true;
		}
		if (nullptr != textureResource)
		{
			if (nullptr != resourceListener)
			{
				textureResource->addResourceListener(*resourceListener);
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

	TextureResourceId TextureResourceManager::createTextureResourceByAssetId(AssetId assetId, Renderer::ITexture& texture)
	{
		// Texture resource is not allowed to exist, yet
		assert(isUninitialized(loadTextureResourceByAssetId(assetId)));

		// Create the texture resource instance
		TextureResource& textureResource = mTextureResources.addElement();
		textureResource.setAssetId(assetId);
		textureResource.mTexture = &texture;

		// Done
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
			if (mTextureResources.getElementByIndex(i).getAssetId() == assetId)
			{
				loadTextureResourceByAssetId(assetId, nullptr, true);
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
