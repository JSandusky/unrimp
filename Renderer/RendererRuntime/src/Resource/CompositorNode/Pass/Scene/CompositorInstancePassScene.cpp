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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Scene/CompositorInstancePassScene.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Scene/CompositorResourcePassScene.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeInstance.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.h"
#include "RendererRuntime/Resource/Scene/ISceneResource.h"
#include "RendererRuntime/Resource/Scene/Node/ISceneNode.h"
#include "RendererRuntime/Resource/Scene/Item/MeshSceneItem.h"
#include "RendererRuntime/Resource/Scene/Item/CameraSceneItem.h"
#include "RendererRuntime/RenderQueue/RenderQueue.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		// TODO(co) Just a first quick'n'dirty placeholder for the real implementation using spatial scene management and culling
		void draw(const RendererRuntime::IRendererRuntime& rendererRuntime, RendererRuntime::CameraSceneItem& cameraSceneItem)
		{
			Renderer::IRenderer& renderer = rendererRuntime.getRenderer();
			RendererRuntime::RenderQueue renderQueue(rendererRuntime);	// TODO(co) Just a first test, render queue instance will be managed in another way

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

			// Loop through all scene nodes and add renderables to the render queue
			const RendererRuntime::ISceneResource::SceneNodes& sceneNodes = cameraSceneItem.getSceneResource().getSceneNodes();
			const size_t numberOfSceneNodes = sceneNodes.size();
			for (size_t sceneNodeIndex = 0; sceneNodeIndex < numberOfSceneNodes; ++sceneNodeIndex)
			{
				const RendererRuntime::ISceneNode* sceneNode = sceneNodes[sceneNodeIndex];

				// Loop through all scene items attached to the current scene node
				const RendererRuntime::ISceneNode::AttachedSceneItems& attachedSceneItems = sceneNode->getAttachedSceneItems();
				const size_t numberOfAttachedSceneItems = attachedSceneItems.size();
				for (size_t attachedSceneItemIndex = 0; attachedSceneItemIndex < numberOfAttachedSceneItems; ++attachedSceneItemIndex)
				{
					const RendererRuntime::ISceneItem* sceneItem = attachedSceneItems[attachedSceneItemIndex];
					if (sceneItem->getSceneItemTypeId() == RendererRuntime::MeshSceneItem::TYPE_ID)
					{
						renderQueue.addRenderablesFromRenderableManager(0, static_cast<const RendererRuntime::MeshSceneItem*>(sceneItem)->getRenderableManager());
					}
				}
			}

			// Draw render queue
			renderQueue.draw();

			// End debug event
			RENDERER_END_DEBUG_EVENT(&renderer)
		}


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
	//[ Protected virtual RendererRuntime::ICompositorInstancePass methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassScene::execute(CameraSceneItem* cameraSceneItem)
	{
		// TODO(co) Just a first test
		if (nullptr != cameraSceneItem)
		{
			::detail::draw(getCompositorNodeInstance().getCompositorWorkspaceInstance().getRendererRuntime(), *cameraSceneItem);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassScene::CompositorInstancePassScene(const CompositorResourcePassScene& compositorResourcePassScene, const CompositorNodeInstance& compositorNodeInstance) :
		ICompositorInstancePass(compositorResourcePassScene, compositorNodeInstance)
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
