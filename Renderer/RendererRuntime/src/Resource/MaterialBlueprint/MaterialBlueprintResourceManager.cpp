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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintResourceLoader.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/MaterialBlueprintResourceListener.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/InstanceUniformBufferManager.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <cassert>


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
		static RendererRuntime::MaterialBlueprintResourceListener defaultMaterialBlueprintResourceListener;


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
	// TODO(co) Work-in-progress
	MaterialResourceId MaterialBlueprintResourceManager::loadMaterialBlueprintResourceByAssetId(AssetId assetId, IResourceListener* resourceListener, bool reload)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			MaterialBlueprintResource* materialBlueprintResource = nullptr;
			{
				const uint32_t numberOfElements = mMaterialBlueprintResources.getNumberOfElements();
				for (uint32_t i = 0; i < numberOfElements; ++i)
				{
					MaterialBlueprintResource& currentMaterialBlueprintResource = mMaterialBlueprintResources.getElementByIndex(i);
					if (currentMaterialBlueprintResource.getAssetId() == assetId)
					{
						materialBlueprintResource = &currentMaterialBlueprintResource;

						// Get us out of the loop
						i = numberOfElements;
					}
				}
			}

			// Create the resource instance
			bool load = reload;
			if (nullptr == materialBlueprintResource)
			{
				materialBlueprintResource = &mMaterialBlueprintResources.addElement();
				materialBlueprintResource->setResourceManager(this);
				materialBlueprintResource->setAssetId(assetId);
				load = true;
			}
			if (nullptr != materialBlueprintResource && nullptr != resourceListener)
			{
				materialBlueprintResource->connectResourceListener(*resourceListener);
			}

			// Load the resource, if required
			if (load)
			{
				// Prepare the resource loader
				MaterialBlueprintResourceLoader* materialBlueprintResourceLoader = static_cast<MaterialBlueprintResourceLoader*>(acquireResourceLoaderInstance(MaterialBlueprintResourceLoader::TYPE_ID));
				materialBlueprintResourceLoader->initialize(*asset, *materialBlueprintResource);

				// Commit resource streamer asset load request
				ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
				resourceStreamerLoadRequest.resource = materialBlueprintResource;
				resourceStreamerLoadRequest.resourceLoader = materialBlueprintResourceLoader;
				mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);

				// TODO(co) The following doesn't feel right, but is sufficient for now. Review it later on.
				// Create default pipeline state caches
				// -> Material blueprints should be loaded by a cache manager upfront so that the following expensive call doesn't cause runtime hiccups
				// -> Runtime hiccups would also be there without fallback pipeline state caches, so there's no real way around
				// -> We must enforce fully loaded material blueprint resource state for this
				materialBlueprintResource->enforceFullyLoaded();
				materialBlueprintResource->createPipelineStateCaches(true);
			}

			// Done
			return materialBlueprintResource->getId();
		}

		// Error!
		return getUninitialized<MaterialBlueprintResourceId>();
	}

	void MaterialBlueprintResourceManager::setMaterialBlueprintResourceListener(IMaterialBlueprintResourceListener* materialBlueprintResourceListener)
	{
		// There must always be a valid material blueprint resource listener instance
		mMaterialBlueprintResourceListener = (nullptr != materialBlueprintResourceListener) ? materialBlueprintResourceListener : &::detail::defaultMaterialBlueprintResourceListener;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void MaterialBlueprintResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mMaterialBlueprintResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			MaterialBlueprintResource& materialBlueprintResource = mMaterialBlueprintResources.getElementByIndex(i);
			if (materialBlueprintResource.getAssetId() == assetId)
			{
				loadMaterialBlueprintResourceByAssetId(assetId, nullptr, true);

				// TODO(co) Cleanup: Update all influenced material resources, probably also other material stuff has to be updated
				materialBlueprintResource.getPipelineStateCacheManager().clearCache();
				materialBlueprintResource.mTextures.clear();
				const MaterialResources& materialResources = mRendererRuntime.getMaterialResourceManager().getMaterialResources();
				const uint32_t numberOfMaterialResources = materialResources.getNumberOfElements();
				for (uint32_t elementIndex = 0; elementIndex < numberOfMaterialResources; ++elementIndex)
				{
					MaterialResource& materialResource = materialResources.getElementByIndex(elementIndex);
					// TODO(co)
				//	if (materialResource->getMaterialBlueprintResource() == materialBlueprintResource)
					{
						materialResource.releaseTextures();
					}
				}

				break;
			}
		}
	}

	void MaterialBlueprintResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MaterialBlueprintResourceManager::MaterialBlueprintResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mMaterialBlueprintResourceListener(&::detail::defaultMaterialBlueprintResourceListener),
		mInstanceUniformBufferManager(new InstanceUniformBufferManager(rendererRuntime))
	{
		// Nothing here
	}

	MaterialBlueprintResourceManager::~MaterialBlueprintResourceManager()
	{
		delete mInstanceUniformBufferManager;
	}

	IResourceLoader* MaterialBlueprintResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own material blueprint format
			assert(resourceLoaderTypeId == MaterialBlueprintResourceLoader::TYPE_ID);
			resourceLoader = new MaterialBlueprintResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
