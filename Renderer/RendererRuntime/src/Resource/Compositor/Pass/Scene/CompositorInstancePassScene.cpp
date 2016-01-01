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
#include "RendererRuntime/Resource/Compositor/Pass/Scene/CompositorInstancePassScene.h"
#include "RendererRuntime/Resource/Compositor/Pass/Scene/CompositorResourcePassScene.h"
#include "RendererRuntime/Resource/Compositor/CompositorInstanceNode.h"
#include "RendererRuntime/Resource/Compositor/CompositorInstance.h"
#include "RendererRuntime/Resource/Scene/ISceneResource.h"
#include "RendererRuntime/Resource/Scene/Node/ISceneNode.h"
#include "RendererRuntime/Resource/Scene/Item/MeshSceneItem.h"
#include "RendererRuntime/Resource/Scene/Item/CameraSceneItem.h"
#include "RendererRuntime/Resource/Mesh/MeshResource.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Shader/ShaderProperties.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Global functions in anonymous namespace               ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{
		RendererRuntime::MaterialResource* mMaterialResource = nullptr;

		void initialize(const RendererRuntime::IRendererRuntime& rendererRuntime)
		{
			// Load the material resource
			mMaterialResource = rendererRuntime.getMaterialResourceManager().loadMaterialResourceByAssetId("Example/Material/Character/FirstMesh");
		}

		void deinitialize()
		{
			// Nothing here
		}

		void draw(const RendererRuntime::IRendererRuntime& rendererRuntime, RendererRuntime::CameraSceneItem& cameraSceneItem)
		{
			Renderer::IRenderer& renderer = rendererRuntime.getRenderer();

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

			RendererRuntime::MaterialBlueprintResource* materialBlueprintResource = mMaterialResource->getMaterialBlueprintResource();
			if (nullptr != materialBlueprintResource && materialBlueprintResource->isFullyLoaded())
			{
				// TODO(co) Pass shader properties
				RendererRuntime::ShaderProperties shaderProperties;

				Renderer::IPipelineState* pipelineState = materialBlueprintResource->getPipelineStateCacheManager().getPipelineStateObject(shaderProperties, *mMaterialResource);
				if (nullptr != pipelineState)
				{
					// Fill the unknown uniform buffers
					materialBlueprintResource->fillUnknownUniformBuffers();

					// Fill the pass uniform buffer
					// TODO(co) Camera usage
					const RendererRuntime::Transform worldSpaceToViewSpaceTransform;
					materialBlueprintResource->fillPassUniformBuffer(worldSpaceToViewSpaceTransform);

					// Fill the material uniform buffer
					materialBlueprintResource->fillMaterialUniformBuffer();

					// Bind the material blueprint resource to the used renderer
					materialBlueprintResource->bindToRenderer();

					// Bind the material resource to the used renderer
					mMaterialResource->bindToRenderer(rendererRuntime);

					// Set the used pipeline state object (PSO)
					renderer.setPipelineState(pipelineState);

					// Loop through all scene nodes
					const RendererRuntime::ISceneResource::SceneNodes& sceneNodes = cameraSceneItem.getSceneResource().getSceneNodes();
					const size_t numberOfSceneNodes = sceneNodes.size();
					for (size_t sceneNodeIndex = 0; sceneNodeIndex < numberOfSceneNodes; ++sceneNodeIndex)
					{
						RendererRuntime::ISceneNode* sceneNode = sceneNodes[sceneNodeIndex];
						const RendererRuntime::Transform& transform = sceneNode->getTransform();

						// Loop through all scene items attached to the current scene node
						const RendererRuntime::ISceneNode::AttachedSceneItems& attachedSceneItems = sceneNode->getAttachedSceneItems();
						const size_t numberOfAttachedSceneItems = attachedSceneItems.size();
						for (size_t attachedSceneItemIndex = 0; attachedSceneItemIndex < numberOfAttachedSceneItems; ++attachedSceneItemIndex)
						{
							RendererRuntime::ISceneItem* sceneItem = attachedSceneItems[attachedSceneItemIndex];
							if (sceneItem->getSceneItemTypeId() == RendererRuntime::MeshSceneItem::TYPE_ID)
							{
								// Fill the instance uniform buffer
								materialBlueprintResource->fillInstanceUniformBuffer(transform, *mMaterialResource);

								// Draw mesh instance
								RendererRuntime::MeshSceneItem* meshSceneItem = static_cast<RendererRuntime::MeshSceneItem*>(sceneItem);
								RendererRuntime::MeshResource* meshResource = meshSceneItem->getMeshResource();
								if (nullptr != meshResource)
								{
									meshResource->draw();
								}
							}
						}
					}
				}

				// End debug event
				RENDERER_END_DEBUG_EVENT(&renderer)
			}
		}
	}
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
			::detail::draw(getCompositorInstanceNode().getCompositorInstance().getRendererRuntime(), *cameraSceneItem);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassScene::CompositorInstancePassScene(const CompositorResourcePassScene& compositorResourcePassScene, const CompositorInstanceNode& compositorInstanceNode) :
		ICompositorInstancePass(compositorResourcePassScene, compositorInstanceNode)
	{
		// TODO(co) Just a first test
		::detail::initialize(getCompositorInstanceNode().getCompositorInstance().getRendererRuntime());
	}

	CompositorInstancePassScene::~CompositorInstancePassScene()
	{
		// TODO(co) Just a first test
		::detail::deinitialize();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
