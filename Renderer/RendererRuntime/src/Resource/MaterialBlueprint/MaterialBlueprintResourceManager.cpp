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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintResourceLoader.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/MaterialBlueprintResourceListener.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/InstanceBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/LightBufferManager.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/Detail/ResourceManagerTemplate.h"
#include "RendererRuntime/Core/Time/TimeManager.h"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		struct MaterialBlueprintCacheEntry
		{
			RendererRuntime::AssetId materialBlueprintAssetId;
			uint32_t				 numberOfBytes;
		};


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
	void MaterialBlueprintResourceManager::loadMaterialBlueprintResourceByAssetId(AssetId assetId, MaterialBlueprintResourceId& materialBlueprintResourceId, IResourceListener* resourceListener, bool reload)
	{
		// Get or create the instance
		MaterialBlueprintResource* materialBlueprintResource = mInternalResourceManager->getResourceByAssetId(assetId);

		// Create the resource instance
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		bool load = (reload && nullptr != asset);
		if (nullptr == materialBlueprintResource && nullptr != asset)
		{
			materialBlueprintResource = &mInternalResourceManager->getResources().addElement();
			materialBlueprintResource->setResourceManager(this);
			materialBlueprintResource->setAssetId(assetId);
			load = true;
		}

		// Before connecting a resource listener, ensure we set the output resource ID at once so it can already directly be used inside the resource listener
		if (nullptr != materialBlueprintResource)
		{
			materialBlueprintResourceId = materialBlueprintResource->getId();
			if (nullptr != resourceListener)
			{
				materialBlueprintResource->connectResourceListener(*resourceListener);
			}
		}
		else
		{
			materialBlueprintResourceId = getUninitialized<MaterialBlueprintResourceId>();
		}

		// Load the resource, if required
		if (load)
		{
			// Commit resource streamer asset load request
			mRendererRuntime.getResourceStreamer().commitLoadRequest(ResourceStreamer::LoadRequest(*asset, MaterialBlueprintResourceLoader::TYPE_ID, reload, *materialBlueprintResource));

			// TODO(co) Currently material blueprint resource loading is a blocking process.
			//          Later on, we can probably just write "mInternalResourceManager->loadResourceByAssetId(assetId, meshResourceId, resourceListener, reload);" and be done in this method.
			// Create default pipeline state caches
			// -> Material blueprints should be loaded by a cache manager upfront so that the following expensive call doesn't cause runtime hiccups
			// -> Runtime hiccups would also be there without fallback pipeline state caches, so there's no real way around
			// -> We must enforce fully loaded material blueprint resource state for this
			materialBlueprintResource->enforceFullyLoaded();
			materialBlueprintResource->createPipelineStateCaches(true);
		}
	}

	void MaterialBlueprintResourceManager::setMaterialBlueprintResourceListener(IMaterialBlueprintResourceListener* materialBlueprintResourceListener)
	{
		// There must always be a valid material blueprint resource listener instance
		if (mMaterialBlueprintResourceListener != materialBlueprintResourceListener)
		{
			// We know there must be a currently set material blueprint resource listener
			assert(nullptr != mMaterialBlueprintResourceListener);
			mMaterialBlueprintResourceListener->onShutdown(mRendererRuntime);
			mMaterialBlueprintResourceListener = (nullptr != materialBlueprintResourceListener) ? materialBlueprintResourceListener : &::detail::defaultMaterialBlueprintResourceListener;
			mMaterialBlueprintResourceListener->onStartup(mRendererRuntime);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	uint32_t MaterialBlueprintResourceManager::getNumberOfResources() const
	{
		return mInternalResourceManager->getResources().getNumberOfElements();
	}

	IResource& MaterialBlueprintResourceManager::getResourceByIndex(uint32_t index) const
	{
		return mInternalResourceManager->getResources().getElementByIndex(index);
	}

	IResource& MaterialBlueprintResourceManager::getResourceByResourceId(ResourceId resourceId) const
	{
		return mInternalResourceManager->getResources().getElementById(resourceId);
	}

	IResource* MaterialBlueprintResourceManager::tryGetResourceByResourceId(ResourceId resourceId) const
	{
		return mInternalResourceManager->getResources().tryGetElementById(resourceId);
	}

	void MaterialBlueprintResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mInternalResourceManager->getResources().getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			MaterialBlueprintResource& materialBlueprintResource = mInternalResourceManager->getResources().getElementByIndex(i);
			if (materialBlueprintResource.getAssetId() == assetId)
			{
				MaterialBlueprintResourceId materialBlueprintResourceId = getUninitialized<MaterialBlueprintResourceId>();
				loadMaterialBlueprintResourceByAssetId(assetId, materialBlueprintResourceId, nullptr, true);

				// TODO(co) Cleanup: Update all influenced material resources, probably also other material stuff has to be updated
				materialBlueprintResource.getPipelineStateCacheManager().clearCache();
				materialBlueprintResource.mTextures.clear();
				const MaterialResourceManager& materialResourceManager = mRendererRuntime.getMaterialResourceManager();
				const uint32_t numberOfMaterialResources = materialResourceManager.getNumberOfResources();
				for (uint32_t elementIndex = 0; elementIndex < numberOfMaterialResources; ++elementIndex)
				{
					MaterialResource& materialResource = materialResourceManager.getByIndex(elementIndex);
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
		const TimeManager& timeManager = mRendererRuntime.getTimeManager();
		mGlobalMaterialProperties.setPropertyById("GlobalPastSecondsSinceLastFrame", MaterialPropertyValue::fromFloat(timeManager.getPastSecondsSinceLastFrame()));
		mGlobalMaterialProperties.setPropertyById("GlobalTimeInSeconds", MaterialPropertyValue::fromFloat(timeManager.getGlobalTimeInSeconds()));
	}


	//[-------------------------------------------------------]
	//[ Private virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	IResourceLoader* MaterialBlueprintResourceManager::createResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		return mInternalResourceManager->createResourceLoaderInstance(resourceLoaderTypeId);
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MaterialBlueprintResourceManager::MaterialBlueprintResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mMaterialBlueprintResourceListener(&::detail::defaultMaterialBlueprintResourceListener),
		mInstanceBufferManager(nullptr),
		mLightBufferManager(nullptr)
	{
		// Create internal resource manager
		mInternalResourceManager = new ResourceManagerTemplate<MaterialBlueprintResource, MaterialBlueprintResourceLoader, MaterialBlueprintResourceId, 64>(rendererRuntime, *this);

		// Startup material blueprint resource listener
		mMaterialBlueprintResourceListener->onStartup(mRendererRuntime);

		// Create buffer managers
		const Renderer::Capabilities& capabilities = rendererRuntime.getRenderer().getCapabilities();
		if (capabilities.maximumUniformBufferSize > 0 && capabilities.maximumTextureBufferSize > 0)
		{
			mInstanceBufferManager = new InstanceBufferManager(rendererRuntime);
			mLightBufferManager = new LightBufferManager(rendererRuntime);
		}

		// Update at once to have all managed global material properties known from the start
		update();
		mGlobalMaterialProperties.setPropertyById("GlobalNumberOfMultisamples", MaterialPropertyValue::fromInteger(0));
	}

	MaterialBlueprintResourceManager::~MaterialBlueprintResourceManager()
	{
		// Destroy buffer managers, if needed
		delete mInstanceBufferManager;
		delete mLightBufferManager;

		// Shutdown material blueprint resource listener (we know there must be such an instance)
		assert(nullptr != mMaterialBlueprintResourceListener);
		mMaterialBlueprintResourceListener->onShutdown(mRendererRuntime);

		// Destroy internal resource manager
		delete mInternalResourceManager;
	}

	void MaterialBlueprintResourceManager::clearPipelineStateObjectCache()
	{
		// Loop through all material blueprint resources and clear the cache entries
		const uint32_t numberOfElements = mInternalResourceManager->getResources().getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			mInternalResourceManager->getResources().getElementByIndex(i).clearPipelineStateObjectCache();
		}
	}

	void MaterialBlueprintResourceManager::loadPipelineStateObjectCache(IFile& file)
	{
		// Read the pipeline state object cache header which consists of information about the contained material blueprint resources
		uint32_t numberOfElements = 0;
		file.read(&numberOfElements, sizeof(uint32_t));
		std::vector< ::detail::MaterialBlueprintCacheEntry> materialBlueprintCacheEntries;
		materialBlueprintCacheEntries.resize(numberOfElements);
		file.read(materialBlueprintCacheEntries.data(), sizeof(::detail::MaterialBlueprintCacheEntry) * numberOfElements);

		// Loop through all material blueprint resources and read the cache entries
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			// TODO(co) Currently material blueprint resource loading is a blocking process
			const ::detail::MaterialBlueprintCacheEntry& materialBlueprintCacheEntry = materialBlueprintCacheEntries[i];
			MaterialBlueprintResourceId materialBlueprintResourceId = getUninitialized<MaterialBlueprintResourceId>();
			loadMaterialBlueprintResourceByAssetId(materialBlueprintCacheEntry.materialBlueprintAssetId, materialBlueprintResourceId);
			if (isInitialized(materialBlueprintResourceId))
			{
				mInternalResourceManager->getResources().getElementById(materialBlueprintResourceId).loadPipelineStateObjectCache(file);
			}
			else
			{
				assert(false && "The pipeline state object cache contains an unknown material blueprint asset");
				file.skip(materialBlueprintCacheEntry.numberOfBytes);
			}
		}
	}

	bool MaterialBlueprintResourceManager::doesPipelineStateObjectCacheNeedSaving() const
	{
		const uint32_t numberOfElements = mInternalResourceManager->getResources().getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			if (mInternalResourceManager->getResources().getElementByIndex(i).doesPipelineStateObjectCacheNeedSaving())
			{
				// Cache saving needed
				return true;
			}
		}

		// No cache saving needed
		return false;
	}

	void MaterialBlueprintResourceManager::savePipelineStateObjectCache(MemoryFile& memoryFile)
	{
		// Write the pipeline state object cache header which consists of information about the contained material blueprint resources
		const uint32_t numberOfElements = mInternalResourceManager->getResources().getNumberOfElements();
		memoryFile.write(&numberOfElements, sizeof(uint32_t));
		uint32_t firstMaterialBlueprintCacheEntryIndex = 0;
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			::detail::MaterialBlueprintCacheEntry materialBlueprintCacheEntry;
			materialBlueprintCacheEntry.materialBlueprintAssetId = mInternalResourceManager->getResources().getElementByIndex(i).getAssetId();
			materialBlueprintCacheEntry.numberOfBytes			 = 0;	// At this point in time we don't know the number of bytes the material blueprint cache entry consumes
			memoryFile.write(&materialBlueprintCacheEntry, sizeof(::detail::MaterialBlueprintCacheEntry));
			if (0 == firstMaterialBlueprintCacheEntryIndex)
			{
				firstMaterialBlueprintCacheEntryIndex = static_cast<uint32_t>(memoryFile.getNumberOfBytes() - sizeof(::detail::MaterialBlueprintCacheEntry));
			}
		}
		::detail::MaterialBlueprintCacheEntry* firstMaterialBlueprintCacheEntry = reinterpret_cast< ::detail::MaterialBlueprintCacheEntry*>(&memoryFile.getByteVector()[firstMaterialBlueprintCacheEntryIndex]);

		// Loop through all material blueprint resources and write the cache entries
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			const uint32_t fileStart = static_cast<uint32_t>(memoryFile.getNumberOfBytes());
			mInternalResourceManager->getResources().getElementByIndex(i).savePipelineStateObjectCache(memoryFile);
			firstMaterialBlueprintCacheEntry[i].numberOfBytes = static_cast<uint32_t>(memoryFile.getNumberOfBytes() - fileStart);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
