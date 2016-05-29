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
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderProperties.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		// TODO(co) Just a first quick'n'dirty placeholder for the real implementation using a render queue

		void draw(const RendererRuntime::IRendererRuntime& rendererRuntime, RendererRuntime::CameraSceneItem& cameraSceneItem)
		{
			Renderer::IRenderer& renderer = rendererRuntime.getRenderer();
			const RendererRuntime::MaterialResources& materialResources = rendererRuntime.getMaterialResourceManager().getMaterialResources();

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

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
						// Draw mesh instance
						RendererRuntime::MeshSceneItem* meshSceneItem = static_cast<RendererRuntime::MeshSceneItem*>(sceneItem);
						RendererRuntime::MeshResource* meshResource = meshSceneItem->getMeshResource();
						if (nullptr != meshResource)
						{
							// Setup input assembly (IA): Set the used vertex array
							Renderer::IVertexArrayPtr vertexArrayPtr = meshResource->getVertexArrayPtr();
							if (nullptr != vertexArrayPtr)
							{
								renderer.iaSetVertexArray(vertexArrayPtr);
								const uint32_t numberOfVertices = meshResource->getNumberOfVertices();

								// Loop through all sub-meshes
								const RendererRuntime::SubMeshes& subMeshes = meshResource->getSubMeshes();
								for (const RendererRuntime::SubMesh& subMesh : subMeshes)
								{
									// Material resource
									const RendererRuntime::MaterialResource* materialResource = materialResources.tryGetElementById(subMesh.getMaterialResourceId());
									if (nullptr != materialResource)
									{
										RendererRuntime::MaterialTechnique* materialTechnique = materialResource->getMaterialTechniqueById("Default");
										if (nullptr != materialTechnique)
										{
											RendererRuntime::MaterialBlueprintResource* materialBlueprintResource = materialTechnique->getMaterialBlueprintResource();
											if (nullptr != materialBlueprintResource && materialBlueprintResource->isFullyLoaded())
											{
												// TODO(co) Pass shader properties
												RendererRuntime::ShaderProperties shaderProperties;

												Renderer::IPipelineStatePtr pipelineStatePtr = materialBlueprintResource->getPipelineStateCacheManager().getPipelineStateObjectPtr(shaderProperties, materialResource->getMaterialProperties());
												if (nullptr != pipelineStatePtr)
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

													// Bind the material technique to the used renderer
													materialTechnique->bindToRenderer(rendererRuntime);

													// Set the used pipeline state object (PSO)
													renderer.setPipelineState(pipelineStatePtr);

													// Fill the instance uniform buffer
													materialBlueprintResource->fillInstanceUniformBuffer(transform, *materialTechnique);

													// Setup input assembly (IA): Set the primitive topology used for draw calls
													renderer.iaSetPrimitiveTopology(subMesh.getPrimitiveTopology());

													// Render the specified geometric primitive, based on indexing into an array of vertices
													renderer.drawIndexed(subMesh.getStartIndexLocation(), subMesh.getNumberOfIndices(), 0, 0, numberOfVertices);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

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
			::detail::draw(getCompositorInstanceNode().getCompositorInstance().getRendererRuntime(), *cameraSceneItem);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassScene::CompositorInstancePassScene(const CompositorResourcePassScene& compositorResourcePassScene, const CompositorInstanceNode& compositorInstanceNode) :
		ICompositorInstancePass(compositorResourcePassScene, compositorInstanceNode)
	{
		// Nothing here
	}

	CompositorInstancePassScene::~CompositorInstancePassScene()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
