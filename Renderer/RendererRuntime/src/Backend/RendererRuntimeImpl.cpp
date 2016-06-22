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
#include "RendererRuntime/Backend/RendererRuntimeImpl.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/Core/Thread/ThreadManager.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Scene/SceneResourceManager.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResourceManager.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Skeleton/SkeletonResourceManager.h"
#include "RendererRuntime/Resource/Compositor/CompositorResourceManager.h"
#include "RendererRuntime/DebugGui/Detail/DebugGuiManagerWindows.h"
#include "RendererRuntime/Vr/VrManager.h"

#include <glm/detail/setup.hpp>	// For "glm::countof()"

#include <cstring>


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
RENDERERRUNTIME_FUNCTION_EXPORT RendererRuntime::IRendererRuntime *createRendererRuntimeInstance(Renderer::IRenderer &renderer)
{
	return new RendererRuntime::RendererRuntimeImpl(renderer);
}


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
		// Vertex input layout
		const Renderer::VertexAttribute VertexAttributesLayout[] =
		{
			{ // Attribute 0
				// Data destination
				Renderer::VertexAttributeFormat::FLOAT_3,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
				"Position",									// name[32] (char)
				"POSITION",									// semanticName[32] (char)
				0,											// semanticIndex (uint32_t)
				// Data source
				0,											// inputSlot (uint32_t)
				0,											// alignedByteOffset (uint32_t)
				// Data source, instancing part
				0											// instancesPerElement (uint32_t)
			}
		};
		const Renderer::VertexAttributes VertexAttributes(glm::countof(VertexAttributesLayout), VertexAttributesLayout);


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
	RendererRuntimeImpl::RendererRuntimeImpl(Renderer::IRenderer &renderer)
	{
		// Backup the given renderer
		mRenderer = &renderer;

		// Add our renderer reference
		mRenderer->addReference();

		// Create the core manager instances
		mThreadManager = new ThreadManager();
		mAssetManager = new AssetManager(*this);

		// Create the resource manager instances
		mResourceStreamer = new ResourceStreamer(*this);
		mTextureResourceManager = new TextureResourceManager(*this);
		mShaderPieceResourceManager = new ShaderPieceResourceManager(*this);
		mShaderBlueprintResourceManager = new ShaderBlueprintResourceManager(*this);
		mMaterialBlueprintResourceManager = new MaterialBlueprintResourceManager(*this);
		mMaterialResourceManager = new MaterialResourceManager(*this);
		mSkeletonResourceManager = new SkeletonResourceManager(*this);
		mMeshResourceManager = new MeshResourceManager(*this);
		mSceneResourceManager = new SceneResourceManager(*this);
		mCompositorResourceManager = new CompositorResourceManager(*this);

		// Register the resource managers inside the resource managers list
		mResourceManagers.push_back(mTextureResourceManager);
		mResourceManagers.push_back(mShaderPieceResourceManager);
		mResourceManagers.push_back(mShaderBlueprintResourceManager);
		mResourceManagers.push_back(mMaterialBlueprintResourceManager);
		mResourceManagers.push_back(mMaterialResourceManager);
		mResourceManagers.push_back(mSkeletonResourceManager);
		mResourceManagers.push_back(mMeshResourceManager);
		mResourceManagers.push_back(mSceneResourceManager);
		mResourceManagers.push_back(mCompositorResourceManager);

		// Create the optional manager instances
		#ifdef WIN32
			mDebugGuiManager = new DebugGuiManagerWindows(*this);
		#else
			#error "Unsupported platform"
		#endif
		mVrManager = new VrManager(*this);
	}

	RendererRuntimeImpl::~RendererRuntimeImpl()
	{
		// Destroy the optional manager instances
		delete mVrManager;
		delete mDebugGuiManager;

		{ // Destroy the resource manager instances in reverse order
			const int numberOfResourceManagers = static_cast<int>(mResourceManagers.size());
			for (int i = numberOfResourceManagers - 1; i >= 0; --i)
			{
				delete mResourceManagers[static_cast<size_t>(i)];
			}
		}

		// Destroy the core manager instances
		delete mAssetManager;
		delete mThreadManager;

		// Release our renderer reference
		mRenderer->release();
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IRendererRuntime methods ]
	//[-------------------------------------------------------]
	void RendererRuntimeImpl::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) If required later on, change this method to a "were's one, there are many"-signature (meaning passing multiple asset IDs at once)
		std::unique_lock<std::mutex> resourcesToReloadMutexLock(mResourcesToReloadMutex);
		mResourcesToReload.insert(assetId);
	}

	void RendererRuntimeImpl::update()
	{
		{ // Handle resource reloading requests
			std::unique_lock<std::mutex> resourcesToReloadMutexLock(mResourcesToReloadMutex);
			if (!mResourcesToReload.empty())
			{
				const size_t numberOfResourceManagers = mResourceManagers.size();
				for (uint32_t assetId : mResourcesToReload)
				{
					// Inform the individual resource manager instances
					for (size_t i = 0; i < numberOfResourceManagers; ++i)
					{
						mResourceManagers[i]->reloadResourceByAssetId(assetId);
					}
				}
				mResourcesToReload.clear();
			}
		}

		// Resource streamer update
		mResourceStreamer->rendererBackendDispatch();

		// Inform the individual resource manager instances
		const size_t numberOfResourceManagers = mResourceManagers.size();
		for (size_t i = 0; i < numberOfResourceManagers; ++i)
		{
			mResourceManagers[i]->update();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
