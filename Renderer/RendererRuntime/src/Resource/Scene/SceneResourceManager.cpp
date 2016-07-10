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
#include "RendererRuntime/Resource/Scene/SceneResourceManager.h"
#include "RendererRuntime/Resource/Scene/SceneResource.h"
#include "RendererRuntime/Resource/Scene/Factory/SceneFactory.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneResourceLoader.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <assert.h>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		static const RendererRuntime::SceneFactory defaultSceneFactory;


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
	ISceneResource* SceneResourceManager::loadSceneResourceByAssetId(AssetId assetId, IResourceListener* resourceListener, bool reload)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			ISceneResource* sceneResource = nullptr;
			const size_t numberOfResources = mSceneResources.size();
			for (size_t i = 0; i < numberOfResources; ++i)
			{
				ISceneResource* currentSceneResource = mSceneResources[i];
				if (currentSceneResource->getAssetId() == assetId)
				{
					sceneResource = currentSceneResource;

					// Get us out of the loop
					i = mSceneResources.size();
				}
			}

			// Create the resource instance
			bool load = reload;
			if (nullptr == sceneResource)
			{
				assert(nullptr != mSceneFactory);
				sceneResource = mSceneFactory->createSceneResource(SceneResource::TYPE_ID, mRendererRuntime, assetId);
				sceneResource->setAssetId(assetId);
				mSceneResources.push_back(sceneResource);
				load = true;
			}
			if (nullptr != sceneResource && nullptr != resourceListener)
			{
				sceneResource->addResourceListener(*resourceListener);
			}

			// Load the resource, if required
			if (load)
			{
				// Prepare the resource loader
				SceneResourceLoader* sceneResourceLoader = static_cast<SceneResourceLoader*>(acquireResourceLoaderInstance(SceneResourceLoader::TYPE_ID));
				sceneResourceLoader->initialize(*asset, *sceneResource);

				// Commit resource streamer asset load request
				ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
				resourceStreamerLoadRequest.resource = sceneResource;
				resourceStreamerLoadRequest.resourceLoader = sceneResourceLoader;
				mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
			}

			// TODO(co) No raw pointers in here
			return sceneResource;
		}

		// Error!
		return nullptr;
	}

	void SceneResourceManager::setSceneFactory(const ISceneFactory* sceneFactory)
	{
		// There must always be a valid scene factory instance
		mSceneFactory = (nullptr != sceneFactory) ? sceneFactory : &::detail::defaultSceneFactory;

		// Tell the scene resource instances about the new scene factory in town
		const size_t numberOfSceneResources = mSceneResources.size();
		for (size_t i = 0; i < numberOfSceneResources; ++i)
		{
			mSceneResources[i]->mSceneFactory = mSceneFactory;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void SceneResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const size_t numberOfSceneResources = mSceneResources.size();
		for (size_t i = 0; i < numberOfSceneResources; ++i)
		{
			ISceneResource* sceneResource = mSceneResources[i];
			if (sceneResource->getAssetId() == assetId)
			{
				sceneResource->destroyAllSceneNodesAndItems();
				loadSceneResourceByAssetId(assetId, nullptr, true);
				break;
			}
		}
	}

	void SceneResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	SceneResourceManager::SceneResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mSceneFactory(&::detail::defaultSceneFactory)
	{
		// Nothing in here
	}

	IResourceLoader* SceneResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own scene format
			assert(resourceLoaderTypeId == SceneResourceLoader::TYPE_ID);
			resourceLoader = new SceneResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
