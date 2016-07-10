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
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResource.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Loader/ShaderBlueprintResourceLoader.h"
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
	// TODO(co) Work-in-progress
	ShaderBlueprintResourceId ShaderBlueprintResourceManager::loadShaderBlueprintResourceByAssetId(AssetId assetId, IResourceListener* resourceListener, bool reload)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			ShaderBlueprintResource* shaderBlueprintResource = nullptr;
			{
				const uint32_t numberOfElements = mShaderBlueprintResources.getNumberOfElements();
				for (uint32_t i = 0; i < numberOfElements; ++i)
				{
					ShaderBlueprintResource& currentShaderBlueprintResource = mShaderBlueprintResources.getElementByIndex(i);
					if (currentShaderBlueprintResource.getAssetId() == assetId)
					{
						shaderBlueprintResource = &currentShaderBlueprintResource;

						// Get us out of the loop
						i = numberOfElements;
					}
				}
			}

			// Create the resource instance
			bool load = reload;
			if (nullptr == shaderBlueprintResource)
			{
				shaderBlueprintResource = &mShaderBlueprintResources.addElement();
				shaderBlueprintResource->setAssetId(assetId);
				load = true;
			}
			if (nullptr != shaderBlueprintResource && nullptr != resourceListener)
			{
				shaderBlueprintResource->addResourceListener(*resourceListener);
			}

			// Load the resource, if required
			if (load)
			{
				// Prepare the resource loader
				ShaderBlueprintResourceLoader* shaderBlueprintResourceLoader = static_cast<ShaderBlueprintResourceLoader*>(acquireResourceLoaderInstance(ShaderBlueprintResourceLoader::TYPE_ID));
				shaderBlueprintResourceLoader->initialize(*asset, *shaderBlueprintResource);

				// Commit resource streamer asset load request
				ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
				resourceStreamerLoadRequest.resource = shaderBlueprintResource;
				resourceStreamerLoadRequest.resourceLoader = shaderBlueprintResourceLoader;
				mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
			}

			// Done
			return shaderBlueprintResource->getId();
		}

		// Error!
		return getUninitialized<ShaderBlueprintResourceId>();
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void ShaderBlueprintResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mShaderBlueprintResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			if (mShaderBlueprintResources.getElementByIndex(i).getAssetId() == assetId)
			{
				loadShaderBlueprintResourceByAssetId(assetId, nullptr, true);
				break;
			}
		}
	}

	void ShaderBlueprintResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	IResourceLoader* ShaderBlueprintResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own shader blueprint format
			assert(resourceLoaderTypeId == ShaderBlueprintResourceLoader::TYPE_ID);
			resourceLoader = new ShaderBlueprintResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
