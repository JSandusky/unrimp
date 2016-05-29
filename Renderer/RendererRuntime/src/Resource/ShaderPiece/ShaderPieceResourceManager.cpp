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
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResourceManager.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResource.h"
#include "RendererRuntime/Resource/ShaderPiece/Loader/ShaderPieceResourceLoader.h"
#include "RendererRuntime/Resource/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <assert.h>
#include <unordered_set>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	ShaderPieceResource* ShaderPieceResourceManager::loadShaderPieceResourceByAssetId(AssetId assetId, bool reload)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			ShaderPieceResource* shaderPieceResource = nullptr;
			const size_t numberOfResources = mResources.size();
			for (size_t i = 0; i < numberOfResources; ++i)
			{
				ShaderPieceResource* currentShaderPieceResource = mResources[i];
				if (currentShaderPieceResource->getAssetId() == assetId)
				{
					shaderPieceResource = currentShaderPieceResource;

					// Get us out of the loop
					i = mResources.size();
				}
			}

			// Create the resource instance
			bool load = reload;
			if (nullptr == shaderPieceResource)
			{
				shaderPieceResource = new ShaderPieceResource(assetId);
				shaderPieceResource->setAssetId(assetId);
				mResources.push_back(shaderPieceResource);
				load = true;
			}

			// Load the resource, if required
			if (load)
			{
				// Prepare the resource loader
				ShaderPieceResourceLoader* shaderPieceResourceLoader = static_cast<ShaderPieceResourceLoader*>(acquireResourceLoaderInstance(ShaderPieceResourceLoader::TYPE_ID));
				shaderPieceResourceLoader->initialize(*asset, *shaderPieceResource);

				// Commit resource streamer asset load request
				ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
				resourceStreamerLoadRequest.resource = shaderPieceResource;
				resourceStreamerLoadRequest.resourceLoader = shaderPieceResourceLoader;
				mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
			}

			// TODO(co) No raw pointers in here
			return shaderPieceResource;
		}

		// Error!
		return nullptr;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void ShaderPieceResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		for (size_t i = 0; i < mResources.size(); ++i)
		{
			ShaderPieceResource* shaderPieceResource = mResources[i];
			if (shaderPieceResource->getAssetId() == assetId)
			{
				loadShaderPieceResourceByAssetId(assetId, true);
				break;
			}
		}
	}

	void ShaderPieceResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	ShaderPieceResourceManager::ShaderPieceResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime)
	{
		// Nothing in here
	}

	ShaderPieceResourceManager::~ShaderPieceResourceManager()
	{
		// Nothing in here
	}

	IResourceLoader* ShaderPieceResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own shader piece format
			assert(resourceLoaderTypeId == ShaderPieceResourceLoader::TYPE_ID);
			resourceLoader = new ShaderPieceResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
