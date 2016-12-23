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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/StringId.h"
#include "RendererRuntime/Resource/IResourceListener.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class CameraSceneItem;
	class IRendererRuntime;
	class RenderableManager;
	class IndirectBufferManager;
	class CompositorNodeInstance;
	class ICompositorInstancePass;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId AssetId;						///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"
	typedef uint32_t CompositorWorkspaceResourceId;	///< POD compositor workspace resource identifier
	typedef StringId CompositorPassTypeId;			///< Compositor pass type identifier, internally just a POD "uint32_t"


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Compositor workspace instance
	*
	*  @remarks
	*    Compositors can get quite complex with a lot of individual compositor passes which several of them rendering portions of the scene.
	*    We really only want to perform expensive culling once for a compositor workspace frame rendering. Some renderable managers might never
	*    get rendered because none of the renderables is inside a render queue index range touched by the compositor passes. As a result,
	*    an compositor workspace instance keeps a list of render queue index ranges covered by the compositor instance passes. Before compositor
	*    instance passes are executed, a culling step is performed gathering all renderable managers which should currently be taken into account
	*    during rendering. The result of this culling step is that each render queue index range has renderable managers to consider assigned to them.
	*    Executed compositor instances passes only access this prepared render queue index information to fill their render queues.
	*/
	class CompositorWorkspaceInstance : protected IResourceListener
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<RenderableManager*> RenderableManagers;

		struct RenderQueueIndexRange
		{
			uint8_t			   minimumRenderQueueIndex;	///< Fixed during runtime
			uint8_t			   maximumRenderQueueIndex;	///< Fixed during runtime
			RenderableManagers renderableManagers;		///< Dynamic during runtime

			RenderQueueIndexRange(uint8_t _minimumRenderQueueIndex, uint8_t _maximumRenderQueueIndex) :
				minimumRenderQueueIndex(_minimumRenderQueueIndex),
				maximumRenderQueueIndex(_maximumRenderQueueIndex)
			{}
		};
		typedef std::vector<RenderQueueIndexRange> RenderQueueIndexRanges;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT CompositorWorkspaceInstance(IRendererRuntime& rendererRuntime, AssetId compositorWorkspaceAssetId);
		RENDERERRUNTIME_API_EXPORT virtual ~CompositorWorkspaceInstance();
		inline const IRendererRuntime& getRendererRuntime() const;
		inline IndirectBufferManager& getIndirectBufferManager() const;
		inline float getResolutionScale() const;
		inline void setResolutionScale(float resolutionScale);	// Changes are considered to be expensive since internal renderer resources might need to be updated when rendering the next time
		inline const RenderQueueIndexRanges& getRenderQueueIndexRanges() const;	// Renderable manager pointers are only considered to be safe directly after the "RendererRuntime::CompositorWorkspaceInstance::execute()" call
		RENDERERRUNTIME_API_EXPORT const RenderQueueIndexRange* getRenderQueueIndexRangeByRenderQueueIndex(uint8_t renderQueueIndex) const;	// Can be a null pointer, don't destroy the instance
		RENDERERRUNTIME_API_EXPORT const ICompositorInstancePass* getFirstCompositorInstancePassByCompositorPassTypeId(CompositorPassTypeId compositorPassTypeId) const;
		RENDERERRUNTIME_API_EXPORT void execute(Renderer::IRenderTarget& renderTarget, CameraSceneItem* cameraSceneItem);
		inline Renderer::IRenderTarget* getExecutionRenderTarget() const;	// Only valid during compositor workspace instance execution


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	protected:
		virtual void onLoadingStateChange(const IResource& resource) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		CompositorWorkspaceInstance() = delete;
		CompositorWorkspaceInstance(const CompositorWorkspaceInstance&) = delete;
		CompositorWorkspaceInstance& operator=(const CompositorWorkspaceInstance&) = delete;
		void destroySequentialCompositorNodeInstances();
		void createFramebuffersAndRenderTargetTextures(const Renderer::IRenderTarget& mainRenderTarget);
		void destroyFramebuffersAndRenderTargetTextures();
		void clearRenderQueueIndexRangesRenderableManagers();
		void gatherRenderQueueIndexRangesRenderableManagers(CameraSceneItem& cameraSceneItem);	// A naive method name would be "culling", this is considered to be an expensive method call


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<CompositorNodeInstance*> CompositorNodeInstances;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&			  mRendererRuntime;
		IndirectBufferManager&		  mIndirectBufferManager;
		float						  mResolutionScale;
		uint32_t					  mRenderTargetWidth;
		uint32_t					  mRenderTargetHeight;
		Renderer::IRenderTarget*	  mExecutionRenderTarget;				///< Only valid during compositor workspace instance execution
		CompositorWorkspaceResourceId mCompositorWorkspaceResourceId;
		CompositorNodeInstances		  mSequentialCompositorNodeInstances;	///< We're responsible to destroy the compositor node instances if we no longer need them
		bool						  mFramebufferManagerInitialized;
		RenderQueueIndexRanges		  mRenderQueueIndexRanges;				///< The render queue index ranges layout is fixed during runtime
		Renderer::CommandBuffer		  mCommandBuffer;						///< Command buffer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.inl"
