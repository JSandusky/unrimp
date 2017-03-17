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
			// Prepare the resource loader
			MaterialBlueprintResourceLoader* materialBlueprintResourceLoader = static_cast<MaterialBlueprintResourceLoader*>(mInternalResourceManager->acquireResourceLoaderInstance(MaterialBlueprintResourceLoader::TYPE_ID));
			materialBlueprintResourceLoader->initialize(*asset, *materialBlueprintResource);

			// Commit resource streamer asset load request
			ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
			resourceStreamerLoadRequest.resource = materialBlueprintResource;
			resourceStreamerLoadRequest.resourceLoader = materialBlueprintResourceLoader;
			mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);

			// TODO(co) The following is temporary until we have something like a vertex array format resource.
			//          Later on, we can probably just write "mInternalResourceManager->loadResourceByAssetId(assetId, meshResourceId, resourceListener, reload);" and be done in this method.
			// Create default pipeline state caches
			// -> Material blueprints should be loaded by a cache manager upfront so that the following expensive call doesn't cause runtime hiccups
			// -> Runtime hiccups would also be there without fallback pipeline state caches, so there's no real way around
			// -> We must enforce fully loaded material blueprint resource state for this
			materialBlueprintResource->enforceFullyLoaded();

			// TODO(co) We need a central vertex input layout management
			if (20431079 == assetId ||		// "MaterialBlueprint/Compositor/Final"
				2351022722 == assetId ||	// "MaterialBlueprint/Deferred/AmbientCompositor"
				643992955 == assetId ||		// "MaterialBlueprint/Compositor/GaussianBlur"
				3502284972 == assetId ||	// "MaterialBlueprint/Compositor/HdrToLdr"
				1374114827 == assetId ||	// "MaterialBlueprint/Compositor/CalculateLuminance"
				2961560463 == assetId ||	// "MaterialBlueprint/Compositor/AdaptiveLuminance"
				2167712998 == assetId ||	// "MaterialBlueprint/Compositor/HdrToLdrBloomThreshold"
				738992981 == assetId ||		// "MaterialBlueprint/Compositor/Scale"
				1259586825 == assetId ||	// "MaterialBlueprint/Compositor/Atmosphere"
				1378819984 == assetId ||	// "MaterialBlueprint/Compositor/LensFlareThreshold"
				2734089221 == assetId ||	// "MaterialBlueprint/Compositor/LensFlareFeatureGeneration"
				3076058624 == assetId ||	// "MaterialBlueprint/Compositor/ScreenSpaceAmbientOcclusionGeneration"
				2859523677 == assetId ||	// "MaterialBlueprint/Compositor/ScreenSpaceAmbientOcclusionBlur"
				1243189953 == assetId)		// "MaterialBlueprint/Compositor/DepthOfField"
			{
				Renderer::VertexAttributes& vertexAttributes = const_cast<Renderer::VertexAttributes&>(materialBlueprintResource->getVertexAttributes());
				static const Renderer::VertexAttribute vertexAttributesLayout[] =
				{
					{ // Attribute 0
						// Data destination
						Renderer::VertexAttributeFormat::FLOAT_4,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
						"PositionTexCoord",							// name[32] (char)
						"POSITION",									// semanticName[32] (char)
						0,											// semanticIndex (uint32_t)
						// Data source
						0,											// inputSlot (size_t)
						0,											// alignedByteOffset (uint32_t)
						// Data source, instancing part
						0											// instancesPerElement (uint32_t)
					}
				};
				vertexAttributes.numberOfAttributes = static_cast<uint32_t>(glm::countof(vertexAttributesLayout));
				vertexAttributes.attributes = vertexAttributesLayout;
			}
			else if (1042371778 == assetId)	// "MaterialBlueprint/Debug/Gui"
			{
				Renderer::VertexAttributes& vertexAttributes = const_cast<Renderer::VertexAttributes&>(materialBlueprintResource->getVertexAttributes());
				static const Renderer::VertexAttribute vertexAttributesLayout[] =
				{
					{ // Attribute 0
						// Data destination
						Renderer::VertexAttributeFormat::FLOAT_2,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
						"Position",									// name[32] (char)
						"POSITION",									// semanticName[32] (char)
						0,											// semanticIndex (uint32_t)
						// Data source
						0,											// inputSlot (uint32_t)
						0,											// alignedByteOffset (uint32_t)
						// Data source, instancing part
						0											// instancesPerElement (uint32_t)
					},
					{ // Attribute 1
						// Data destination
						Renderer::VertexAttributeFormat::FLOAT_2,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
						"TexCoord",									// name[32] (char)
						"TEXCOORD",									// semanticName[32] (char)
						0,											// semanticIndex (uint32_t)
						// Data source
						0,											// inputSlot (uint32_t)
						sizeof(float) * 2,							// alignedByteOffset (uint32_t)
						// Data source, instancing part
						0											// instancesPerElement (uint32_t)
					},
					{ // Attribute 2
						// Data destination
						Renderer::VertexAttributeFormat::R8G8B8A8_UNORM,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
						"Color",											// name[32] (char)
						"COLOR",											// semanticName[32] (char)
						0,													// semanticIndex (uint32_t)
						// Data source
						0,													// inputSlot (uint32_t)
						sizeof(float) * 2 * 2,								// alignedByteOffset (uint32_t)
						// Data source, instancing part
						0													// instancesPerElement (uint32_t)
					}
				};
				vertexAttributes.numberOfAttributes = static_cast<uint32_t>(glm::countof(vertexAttributesLayout));
				vertexAttributes.attributes = vertexAttributesLayout;
			}

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
					MaterialResource& materialResource = static_cast<MaterialResource&>(materialResourceManager.getResourceByIndex(elementIndex));
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
		mGlobalMaterialProperties.setPropertyById("PastSecondsSinceLastFrame", MaterialPropertyValue::fromFloat(timeManager.getPastSecondsSinceLastFrame()));
		mGlobalMaterialProperties.setPropertyById("GlobalTimeInSeconds", MaterialPropertyValue::fromFloat(timeManager.getGlobalTimeInSeconds()));
	}


	//[-------------------------------------------------------]
	//[ Private virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void MaterialBlueprintResourceManager::releaseResourceLoaderInstance(IResourceLoader& resourceLoader)
	{
		mInternalResourceManager->releaseResourceLoaderInstance(resourceLoader);
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
	}

	MaterialBlueprintResourceManager::~MaterialBlueprintResourceManager()
	{
		// Destroy buffer managers
		if (nullptr != mInstanceBufferManager)
		{
			delete mInstanceBufferManager;
		}
		if (nullptr != mLightBufferManager)
		{
			delete mLightBufferManager;
		}

		// Shutdown material blueprint resource listener (we know there must be such an instance)
		assert(nullptr != mMaterialBlueprintResourceListener);
		mMaterialBlueprintResourceListener->onShutdown(mRendererRuntime);

		// Destroy internal resource manager
		delete mInternalResourceManager;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
