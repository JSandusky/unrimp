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
#include "RendererRuntime/Backend/RendererRuntimeImpl.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/Core/Time/TimeManager.h"
#include "RendererRuntime/Core/Thread/ThreadManager.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Scene/SceneResourceManager.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResourceManager.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCompiler.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Skeleton/SkeletonResourceManager.h"
#include "RendererRuntime/Resource/SkeletonAnimation/SkeletonAnimationResourceManager.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeResourceManager.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceResourceManager.h"
#ifdef WIN32
	#include "RendererRuntime/DebugGui/Detail/DebugGuiManagerWindows.h"
#elif LINUX
	#include "RendererRuntime/DebugGui/Detail/DebugGuiManagerLinux.h"
#endif
#include "RendererRuntime/Vr/OpenVR/VrManagerOpenVR.h"

#include <cstring>


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
RENDERERRUNTIME_FUNCTION_EXPORT RendererRuntime::IRendererRuntime *createRendererRuntimeInstance(Renderer::IRenderer &renderer, RendererRuntime::IFileManager& fileManager)
{
	return new RendererRuntime::RendererRuntimeImpl(renderer, fileManager);
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RendererRuntimeImpl::RendererRuntimeImpl(Renderer::IRenderer &renderer, IFileManager& fileManager)
	{
		// Backup the given renderer and add our reference
		mRenderer = &renderer;
		mRenderer->addReference();

		// Create the buffer and texture manager instance and add our reference
		mBufferManager = mRenderer->createBufferManager();
		mBufferManager->addReference();
		mTextureManager = mRenderer->createTextureManager();
		mTextureManager->addReference();

		// Backup the given file manager instance
		mFileManager = &fileManager;

		// Create the core manager instances
		mThreadManager = new ThreadManager();
		mAssetManager = new AssetManager(*this);
		mTimeManager = new TimeManager();

		// Create the resource manager instances
		mResourceStreamer = new ResourceStreamer(*this);
		mTextureResourceManager = new TextureResourceManager(*this);
		mShaderPieceResourceManager = new ShaderPieceResourceManager(*this);
		mShaderBlueprintResourceManager = new ShaderBlueprintResourceManager(*this);
		mMaterialBlueprintResourceManager = new MaterialBlueprintResourceManager(*this);
		mMaterialResourceManager = new MaterialResourceManager(*this);
		mSkeletonResourceManager = new SkeletonResourceManager(*this);
		mSkeletonAnimationResourceManager = new SkeletonAnimationResourceManager(*this);
		mMeshResourceManager = new MeshResourceManager(*this);
		mSceneResourceManager = new SceneResourceManager(*this);
		mCompositorNodeResourceManager = new CompositorNodeResourceManager(*this);
		mCompositorWorkspaceResourceManager = new CompositorWorkspaceResourceManager(*this);

		// Register the resource managers inside the resource managers list
		mResourceManagers.push_back(mTextureResourceManager);
		mResourceManagers.push_back(mShaderPieceResourceManager);
		mResourceManagers.push_back(mShaderBlueprintResourceManager);
		mResourceManagers.push_back(mMaterialBlueprintResourceManager);
		mResourceManagers.push_back(mMaterialResourceManager);
		mResourceManagers.push_back(mSkeletonResourceManager);
		mResourceManagers.push_back(mSkeletonAnimationResourceManager);
		mResourceManagers.push_back(mMeshResourceManager);
		mResourceManagers.push_back(mSceneResourceManager);
		mResourceManagers.push_back(mCompositorNodeResourceManager);
		mResourceManagers.push_back(mCompositorWorkspaceResourceManager);

		// Misc
		mPipelineStateCompiler = new PipelineStateCompiler(*this);

		// Create the optional manager instances
		#ifdef WIN32
			mDebugGuiManager = new DebugGuiManagerWindows(*this);
		#elif LINUX
			mDebugGuiManager = new DebugGuiManagerLinux(*this);
			// TODO(sw) Implement an linux manager for this (or we use generally sdl?)
			// TODO(co) No SDL inside the renderer runtime, it's beyond it's scope. It would be valid to define an abstract
			//          minimalistic input interface which e.g. in concrete applications using renderer runtime use SDL. On
			//          the other hand, this would be overkill for the super minimalistic debug GUI. Would prefer a self contained
			//          solution to not have to many external dependencies making it harder to use renderer runtime in projects.
		#else
			#error "Unsupported platform"
		#endif

		#ifndef ANDROID
			// TODO(sw) For now no OpenVR support under Android
			mVrManager = new VrManagerOpenVR(*this);
		#endif
	}

	RendererRuntimeImpl::~RendererRuntimeImpl()
	{
		// Before doing anything else, ensure the resource streamer has no more work to do
		mResourceStreamer->flushAllQueues();

		// Destroy the optional manager instances
		delete mVrManager;
		delete mDebugGuiManager;

		// Destroy misc
		delete mPipelineStateCompiler;

		{ // Destroy the resource manager instances in reverse order
			const int numberOfResourceManagers = static_cast<int>(mResourceManagers.size());
			for (int i = numberOfResourceManagers - 1; i >= 0; --i)
			{
				delete mResourceManagers[static_cast<size_t>(i)];
			}
		}
		delete mResourceStreamer;

		// Destroy the core manager instances
		delete mTimeManager;
		delete mAssetManager;
		delete mThreadManager;

		// Release the texture and buffer manager instance
		mTextureManager->releaseReference();
		mBufferManager->releaseReference();

		// Release our renderer reference
		mRenderer->releaseReference();
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
		// Update the time manager
		mTimeManager->update();

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

		// Pipeline state compiler and resource streamer update
		mPipelineStateCompiler->dispatch();
		mResourceStreamer->dispatch();

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
