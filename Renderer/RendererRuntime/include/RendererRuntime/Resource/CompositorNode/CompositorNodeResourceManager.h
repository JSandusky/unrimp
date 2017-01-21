/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/PackedElementManager.h"
#include "RendererRuntime/Resource/Detail/IResourceManager.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeResource.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
	class FramebufferManager;
	class ICompositorPassFactory;
	class RenderTargetTextureManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t																   CompositorNodeResourceId;	///< POD compositor node resource identifier
	typedef PackedElementManager<CompositorNodeResource, CompositorNodeResourceId, 32> CompositorNodeResources;


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class CompositorNodeResourceManager : private IResourceManager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;
		friend class IResource; 					// Needed so that inside this classes an static_cast<CompositorNodeResourceManager*>(IResourceManager*) works
		friend class CompositorNodeResourceLoader;	// Needed so that inside this classes an static_cast<CompositorNodeResourceManager*>(IResourceManager*) works


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline IRendererRuntime& getRendererRuntime() const;

		inline const CompositorNodeResources& getCompositorNodeResources() const;
		RENDERERRUNTIME_API_EXPORT CompositorNodeResourceId loadCompositorNodeResourceByAssetId(AssetId assetId, IResourceListener* resourceListener = nullptr, bool reload = false);	// Asynchronous

		inline const ICompositorPassFactory& getCompositorPassFactory() const;
		RENDERERRUNTIME_API_EXPORT void setCompositorPassFactory(const ICompositorPassFactory* compositorPassFactory);

		inline RenderTargetTextureManager& getRenderTargetTextureManager();
		inline FramebufferManager& getFramebufferManager();


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	public:
		inline virtual IResource& getResourceByResourceId(ResourceId resourceId) const override;
		inline virtual IResource* tryGetResourceByResourceId(ResourceId resourceId) const override;
		virtual void reloadResourceByAssetId(AssetId assetId) override;
		virtual void update() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit CompositorNodeResourceManager(IRendererRuntime& rendererRuntime);
		virtual ~CompositorNodeResourceManager();
		CompositorNodeResourceManager(const CompositorNodeResourceManager&) = delete;
		CompositorNodeResourceManager& operator=(const CompositorNodeResourceManager&) = delete;
		IResourceLoader* acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&			  mRendererRuntime;				///< Renderer runtime instance, do not destroy the instance
		CompositorNodeResources		  mCompositorNodeResources;
		const ICompositorPassFactory* mCompositorPassFactory;		///< Compositor pass factory, always valid, do not destroy the instance
		RenderTargetTextureManager*	  mRenderTargetTextureManager;	///< Render target texture manager, always valid, we're responsible for destroying it if we no longer need it
		FramebufferManager*			  mFramebufferManager;			///< Framebuffer manager, always valid, we're responsible for destroying it if we no longer need it


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeResourceManager.inl"
