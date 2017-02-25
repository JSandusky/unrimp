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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Core/PackedElementManager.h"
#include "RendererRuntime/Resource/Detail/IResourceManager.h"
#include "RendererRuntime/Resource/SkeletonAnimation/SkeletonAnimationResource.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
	class IResourceListener;
	class SkeletonAnimationController;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t																		   SkeletonAnimationResourceId;	///< POD skeleton animation resource identifier
	typedef PackedElementManager<SkeletonAnimationResource, SkeletonAnimationResourceId, 2048> SkeletonAnimationResources;


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Skeleton animation resource manager
	*/
	class SkeletonAnimationResourceManager : private IResourceManager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;
		friend class SkeletonAnimationController;	// Registers/unregisters itself inside the skeleton animation resource manager


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline const SkeletonAnimationResources& getSkeletonAnimationResources() const;
		RENDERERRUNTIME_API_EXPORT SkeletonAnimationResource* getSkeletonAnimationResourceByAssetId(AssetId assetId) const;	// Considered to be inefficient, avoid method whenever possible
		RENDERERRUNTIME_API_EXPORT SkeletonAnimationResourceId loadSkeletonAnimationResourceByAssetId(AssetId assetId, IResourceListener* resourceListener = nullptr, bool reload = false);	// Asynchronous
		RENDERERRUNTIME_API_EXPORT SkeletonAnimationResourceId createSkeletonAnimationResourceByAssetId(AssetId assetId);	// Skeleton animation resource is not allowed to exist, yet


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
		inline explicit SkeletonAnimationResourceManager(IRendererRuntime& rendererRuntime);
		inline virtual ~SkeletonAnimationResourceManager();
		SkeletonAnimationResourceManager(const SkeletonAnimationResourceManager&) = delete;
		SkeletonAnimationResourceManager& operator=(const SkeletonAnimationResourceManager&) = delete;
		IResourceLoader* acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<SkeletonAnimationController*> SkeletonAnimationControllers;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&			 mRendererRuntime;				///< Renderer runtime instance, do not destroy the instance
		SkeletonAnimationResources   mSkeletonAnimationResources;
		SkeletonAnimationControllers mSkeletonAnimationControllers;	///< Don't destroy the instanced, they are not owned here


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/SkeletonAnimation/SkeletonAnimationResourceManager.inl"
