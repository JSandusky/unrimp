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
#include "RendererRuntime/Resource/ResourceStreamer.h"
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
	TextureResource* TextureResourceManager::loadTextureResourceByAssetId(AssetId assetId, bool reload)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			TextureResource* textureResource = nullptr;
			const size_t numberOfResources = mResources.size();
			for (size_t i = 0; i < numberOfResources; ++i)
			{
				TextureResource* currentTextureResource = mResources[i];
				if (currentTextureResource->getAssetId() == assetId)
				{
					textureResource = currentTextureResource;

					// Get us out of the loop
					i = mResources.size();
				}
			}

			// Create the resource instance
			bool load = reload;
			if (nullptr == textureResource)
			{
				textureResource = new TextureResource(assetId);
				textureResource->setAssetId(assetId);
				mResources.push_back(textureResource);
				load = true;
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

			// TODO(co) No raw pointers in here
			return textureResource;
		}

		// Error!
		return nullptr;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void TextureResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		for (size_t i = 0; i < mResources.size(); ++i)
		{
			if (mResources[i]->getAssetId() == assetId)
			{
				loadTextureResourceByAssetId(assetId, true);
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
		// Nothing in here
	}

	TextureResourceManager::~TextureResourceManager()
	{
		// TODO(co) Implement decent resource handling
		for (size_t i = 0; i < mResources.size(); ++i)
		{
			delete mResources[i];
		}
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
