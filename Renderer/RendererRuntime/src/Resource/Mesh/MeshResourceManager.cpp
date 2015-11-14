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
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Mesh/MeshResource.h"
#include "RendererRuntime/Resource/Mesh/Loader/MeshResourceLoader.h"
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
	// TODO(co) Work-in-progress
	MeshResource* MeshResourceManager::loadMeshResourceByAssetId(Renderer::IRenderer& renderer, AssetId assetId)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			mRenderer = &renderer;

			// Get or create the instance
			MeshResource* meshResource = nullptr;
			const size_t numberOfResources = mResources.size();
			for (size_t i = 0; i < numberOfResources; ++i)
			{
				MeshResource* currentMeshResource = mResources[i];
				if (currentMeshResource->getResourceId() == assetId)
				{
					meshResource = currentMeshResource;

					// Get us out of the loop
					i = mResources.size();
				}
			}

			// Create the resource instance
			if (nullptr == meshResource)
			{
				meshResource = new MeshResource(assetId);
				mResources.push_back(meshResource);

				{
					// Prepare the resource loader
					MeshResourceLoader* meshResourceLoader = static_cast<MeshResourceLoader*>(acquireResourceLoaderInstance(MeshResourceLoader::TYPE_ID));
					meshResourceLoader->initialize(*asset, *meshResource, renderer);

					// Commit resource streamer asset load request
					ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
					resourceStreamerLoadRequest.resource = meshResource;
					resourceStreamerLoadRequest.resourceLoader = meshResourceLoader;
					mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
				}
			}

			// TODO(co) No raw pointers in here
			return meshResource;
		}

		// Error!
		return nullptr;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void MeshResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		for (size_t i = 0; i < mResources.size(); ++i)
		{
			if (mResources[i]->getResourceId() == assetId)
			{
				loadMeshResourceByAssetId(*mRenderer, assetId);
				break;
			}
		}
	}

	void MeshResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MeshResourceManager::MeshResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mRenderer(nullptr)
	{
		// Nothing in here
	}

	MeshResourceManager::~MeshResourceManager()
	{
		// TODO(co) Implement decent resource handling
		for (size_t i = 0; i < mResources.size(); ++i)
		{
			delete mResources[i];
		}
	}

	IResourceLoader* MeshResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own mesh format
			assert(resourceLoaderTypeId == MeshResourceLoader::TYPE_ID);
			resourceLoader = new MeshResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
