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
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintResourceLoader.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/MaterialBlueprintResourceListener.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/InstanceBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/LightBufferManager.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"


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

			// Done
			return materialBlueprintResource->getId();
		}

		// Error!
		return getUninitialized<MaterialBlueprintResourceId>();
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
		mInstanceBufferManager(nullptr),
		mLightBufferManager(nullptr)
	{
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
