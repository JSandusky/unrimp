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
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Mesh/Loader/MeshResourceLoader.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	MeshResourceId MeshResourceManager::loadMeshResourceByAssetId(AssetId assetId, IResourceListener* resourceListener, bool reload)
	{
		MeshResourceId meshResourceId = getUninitialized<MeshResourceId>();

		// Get or create the instance
		MeshResource* meshResource = nullptr;
		{
			const uint32_t numberOfElements = mMeshResources.getNumberOfElements();
			for (uint32_t i = 0; i < numberOfElements; ++i)
			{
				MeshResource& currentMeshResource = mMeshResources.getElementByIndex(i);
				if (currentMeshResource.getAssetId() == assetId)
				{
					meshResource = &currentMeshResource;

					// Get us out of the loop
					i = numberOfElements;
				}
			}
		}

		// Create the resource instance
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		bool load = (reload && nullptr != asset);
		if (nullptr == meshResource && nullptr != asset)
		{
			meshResource = &mMeshResources.addElement();
			meshResource->setResourceManager(this);
			meshResource->setAssetId(assetId);
			load = true;
		}
		if (nullptr != meshResource)
		{
			if (nullptr != resourceListener)
			{
				meshResource->connectResourceListener(*resourceListener);
			}
			meshResourceId = meshResource->getId();
		}

		// Load the resource, if required
		if (load)
		{
			// Prepare the resource loader
			MeshResourceLoader* meshResourceLoader = static_cast<MeshResourceLoader*>(acquireResourceLoaderInstance(MeshResourceLoader::TYPE_ID));
			meshResourceLoader->initialize(*asset, *meshResource);

			// Commit resource streamer asset load request
			ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
			resourceStreamerLoadRequest.resource = meshResource;
			resourceStreamerLoadRequest.resourceLoader = meshResourceLoader;
			mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
		}

		// Done
		return meshResourceId;
	}

	MeshResourceId MeshResourceManager::createEmptyMeshResourceByAssetId(AssetId assetId)
	{
		// Mesh resource is not allowed to exist, yet
		assert(isUninitialized(loadMeshResourceByAssetId(assetId)));

		// Create the mesh resource instance
		MeshResource& meshResource = mMeshResources.addElement();
		meshResource.setResourceManager(this);
		meshResource.setAssetId(assetId);

		// Done
		return meshResource.getId();
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void MeshResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mMeshResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			if (mMeshResources.getElementByIndex(i).getAssetId() == assetId)
			{
				loadMeshResourceByAssetId(assetId, nullptr, true);
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
