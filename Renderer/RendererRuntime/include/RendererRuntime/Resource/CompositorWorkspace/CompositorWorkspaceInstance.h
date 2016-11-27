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


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderTarget;
}
namespace RendererRuntime
{
	class CameraSceneItem;
	class IRendererRuntime;
	class IndirectBufferManager;
	class CompositorNodeInstance;
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


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class CompositorWorkspaceInstance : protected IResourceListener
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT CompositorWorkspaceInstance(IRendererRuntime& rendererRuntime, AssetId compositorWorkspaceAssetId);
		RENDERERRUNTIME_API_EXPORT virtual ~CompositorWorkspaceInstance();
		inline const IRendererRuntime& getRendererRuntime() const;
		inline IndirectBufferManager& getIndirectBufferManager() const;
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
		Renderer::IRenderTarget*	  mExecutionRenderTarget;				///< Only valid during compositor workspace instance execution
		CompositorWorkspaceResourceId mCompositorWorkspaceResourceId;
		CompositorNodeInstances		  mSequentialCompositorNodeInstances;	///< We're responsible to destroy the compositor node instances if we no longer need them


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.inl"
