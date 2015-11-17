/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererRuntime/Resource/Compositor/CompositorInstance.h"
#include "RendererRuntime/Resource/Compositor/CompositorResource.h"
#include "RendererRuntime/Resource/Compositor/CompositorInstanceNode.h"
#include "RendererRuntime/Resource/Compositor/CompositorResourceNode.h"
#include "RendererRuntime/Resource/Compositor/CompositorResourceManager.h"
#include "RendererRuntime/Resource/Compositor/Pass/ICompositorPassFactory.h"
#include "RendererRuntime/Resource/Compositor/Pass/ICompositorResourcePass.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	CompositorInstance::CompositorInstance(IRendererRuntime& rendererRuntime, AssetId compositorAssetId, Renderer::IRenderTarget& renderTarget) :
		mRendererRuntime(rendererRuntime),
		mRenderTarget(renderTarget),
		mCompositorResource(nullptr)
	{
		// Add reference to the render target
		mRenderTarget.addReference();

		// Load the compositor resource
		mCompositorResource = rendererRuntime.getCompositorResourceManager().loadCompositorResourceByAssetId(compositorAssetId, this);
	}

	CompositorInstance::~CompositorInstance()
	{
		// Release reference from the render target
		mRenderTarget.release();

		// Cleanup
		destroySequentialCompositorInstanceNodes();
	}

	const IRendererRuntime& CompositorInstance::getRendererRuntime() const
	{
		return mRendererRuntime;
	}

	void CompositorInstance::execute(SceneItemCamera* sceneItemCamera)
	{
		// Is the compositor resource ready?
		if (nullptr != mCompositorResource && mCompositorResource->getLoadingState() == IResource::LoadingState::LOADED)
		{
			Renderer::IRenderer& renderer = mRenderTarget.getRenderer();

			// Begin scene rendering
			// -> Required for Direct3D 9 and Direct3D 12
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			if (renderer.beginScene())
			{
				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

				// Make the main swap chain to the current render target
				renderer.omSetRenderTarget(&mRenderTarget);

				{ // Since Direct3D 12 is command list based, the viewport and scissor rectangle
					// must be set in every draw call to work with all supported renderer APIs
					// Get the window size
					uint32_t width  = 0;
					uint32_t height = 0;
					mRenderTarget.getWidthAndHeight(width, height);

					// Set the viewport and scissor rectangle
					renderer.rsSetViewportAndScissorRectangle(0, 0, width, height);
				}

				{ // Draw
					// TODO(co)
					const size_t numberOfSequentialCompositorInstanceNodes = mSequentialCompositorInstanceNodes.size();
					for (size_t i = 0; i < numberOfSequentialCompositorInstanceNodes; ++i)
					{
						mSequentialCompositorInstanceNodes[i]->execute(sceneItemCamera);
					}
				}

				// End scene rendering
				// -> Required for Direct3D 9 and Direct3D 12
				// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
				renderer.endScene();
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(&renderer)

			// In case the render target is a swap chain, present the content of the current back buffer
			if (mRenderTarget.getResourceType() == Renderer::ResourceType::SWAP_CHAIN)
			{
				static_cast<Renderer::ISwapChain&>(mRenderTarget).present();
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	void CompositorInstance::onLoadingStateChange(IResource::LoadingState::Enum loadingState)
	{
		if (loadingState == IResource::LoadingState::LOADED)
		{
			// TODO(co) Just a first test
			const ICompositorPassFactory* compositorPassFactory = mRendererRuntime.getCompositorResourceManager().getCompositorPassFactory();

			destroySequentialCompositorInstanceNodes();

			// Compositor resource nodes
			const CompositorResource::CompositorResourceNodes& compositorResourceNodes = mCompositorResource->getCompositorResourceNodes();
			const size_t numberOfCompositorResourceNodes = compositorResourceNodes.size();
			for (size_t nodeIndex = 0; nodeIndex < numberOfCompositorResourceNodes; ++nodeIndex)
			{
				// Get the compositor resource node instance
				const CompositorResourceNode* compositorResourceNode = compositorResourceNodes[nodeIndex];

				// Create the compositor instance node instance
				CompositorInstanceNode* compositorInstanceNode = new CompositorInstanceNode(*compositorResourceNode, *this);
				mSequentialCompositorInstanceNodes.push_back(compositorInstanceNode);

				{ // Compositor resource node targets
					const CompositorResourceNode::CompositorResourceTargets& compositorResourceTargets = compositorResourceNode->getCompositorResourceTargets();
					const size_t numberOfCompositorResourceTargets = compositorResourceTargets.size();
					for (size_t targetIndex = 0; targetIndex < numberOfCompositorResourceTargets; ++targetIndex)
					{
						// Get the compositor resource target instance
						const CompositorResourceTarget& compositorResourceTarget = compositorResourceTargets[targetIndex];

						{ // Compositor resource node target passes
							const CompositorResourceTarget::CompositorResourcePasses& compositorResourcePasses = compositorResourceTarget.getCompositorResourcePasses();
							const size_t numberOfCompositorResourcePasses = compositorResourcePasses.size();
							for (size_t passIndex = 0; passIndex < numberOfCompositorResourcePasses; ++passIndex)
							{
								// Get the compositor resource target instance
								const ICompositorResourcePass* compositorResourcePass = compositorResourcePasses[passIndex];

								// Create the compositor instance pass
								if (nullptr != compositorResourcePass)
								{
									compositorInstanceNode->mCompositorInstancePasses.push_back(compositorPassFactory->createCompositorInstancePass(*compositorResourcePass, *compositorInstanceNode));
								}
							}
						}
					}
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void CompositorInstance::destroySequentialCompositorInstanceNodes()
	{
		const size_t numberOfSequentialCompositorInstanceNodes = mSequentialCompositorInstanceNodes.size();
		for (size_t i = 0; i < numberOfSequentialCompositorInstanceNodes; ++i)
		{
			delete mSequentialCompositorInstanceNodes[i];
		}
		mSequentialCompositorInstanceNodes.clear();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
