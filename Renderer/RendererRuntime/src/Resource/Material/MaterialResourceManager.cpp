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
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Material/Loader/MaterialResourceLoader.h"
#include "RendererRuntime/Resource/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	MaterialResourceId MaterialResourceManager::loadMaterialResourceByAssetId(AssetId assetId, bool reload)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			MaterialResource* materialResource = nullptr;
			{
				const uint32_t numberOfElements = mMaterialResources.getNumberOfElements();
				for (uint32_t i = 0; i < numberOfElements; ++i)
				{
					MaterialResource& currentMaterialResource = mMaterialResources.getElementByIndex(i);
					if (currentMaterialResource.getAssetId() == assetId)
					{
						materialResource = &currentMaterialResource;

						// Get us out of the loop
						i = numberOfElements;
					}
				}
			}

			// Create the resource instance
			bool load = reload;
			if (nullptr == materialResource)
			{
				materialResource = &mMaterialResources.addElement();
				materialResource->setAssetId(assetId);
				load = true;
			}

			// Load the resource, if required
			if (load)
			{
				// Prepare the resource loader
				MaterialResourceLoader* materialResourceLoader = static_cast<MaterialResourceLoader*>(acquireResourceLoaderInstance(MaterialResourceLoader::TYPE_ID));
				materialResourceLoader->initialize(*asset, *materialResource);

				// Commit resource streamer asset load request
				ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
				resourceStreamerLoadRequest.resource = materialResource;
				resourceStreamerLoadRequest.resourceLoader = materialResourceLoader;
				mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
			}

			// Done
			return materialResource->getId();
		}

		// Error!
		return getUninitialized<MaterialResourceId>();
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void MaterialResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mMaterialResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			if (mMaterialResources.getElementByIndex(i).getAssetId() == assetId)
			{
				loadMaterialResourceByAssetId(assetId, true);
				break;
			}
		}
	}

	void MaterialResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	IResourceLoader* MaterialResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own material format
			assert(resourceLoaderTypeId == MaterialResourceLoader::TYPE_ID);
			resourceLoader = new MaterialResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
