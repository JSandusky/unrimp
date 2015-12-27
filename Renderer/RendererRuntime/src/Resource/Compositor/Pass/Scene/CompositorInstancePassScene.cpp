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
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <Renderer/Public/Renderer.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4201)	// warning C4201: nonstandard extension used: nameless struct/union
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtx/quaternion.hpp>
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Global functions in anonymous namespace               ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{
		Renderer::IUniformBufferPtr			  mUniformBuffer;			///< Uniform buffer object (UBO), can be a null pointer
		RendererRuntime::MaterialResource*	  mMaterialResource;

		void initialize(const RendererRuntime::IRendererRuntime& rendererRuntime)
		{
			Renderer::IRenderer& renderer = rendererRuntime.getRenderer();

			// Decide which shader language should be used (for example "GLSL" or "HLSL")
			Renderer::IShaderLanguagePtr shaderLanguage(renderer.getShaderLanguage());
			if (nullptr != shaderLanguage)
			{
				// Create uniform buffers
				// -> Direct3D 9 and OpenGL ES 2 do not support uniform buffers
				// -> Direct3D 10, 11 and 12 do not support individual uniforms
				// -> The renderer is just a light weight abstraction layer, so we need to handle the differences
				if ((0 == strcmp(renderer.getName(), "Direct3D10") || 0 == strcmp(renderer.getName(), "Direct3D11") || 0 == strcmp(renderer.getName(), "Direct3D12")))
				{
					// Allocate enough memory for two 4x4 floating point matrices
					mUniformBuffer = shaderLanguage->createUniformBuffer(2 * 4 * 4 * sizeof(float), nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
				}

				// Load the material resource
				mMaterialResource = rendererRuntime.getMaterialResourceManager().loadMaterialResourceByAssetId("Example/Material/Character/FirstMesh");
			}
		}

		void deinitialize()
		{
			// Release the used resources
			mUniformBuffer = nullptr;
		}

		void draw(const RendererRuntime::IRendererRuntime& rendererRuntime, RendererRuntime::CameraSceneItem& cameraSceneItem)
		{
			Renderer::IRenderer& renderer = rendererRuntime.getRenderer();

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

			// Set the viewport and get the aspect ratio
			float aspectRatio = 4.0f / 3.0f;
			{
				// Get the render target with and height
				uint32_t width  = 1;
				uint32_t height = 1;
				Renderer::IRenderTarget *renderTarget = renderer.omGetRenderTarget();
				if (nullptr != renderTarget)
				{
					renderTarget->getWidthAndHeight(width, height);

					// Get the aspect ratio
					aspectRatio = static_cast<float>(width) / height;
				}
			}

			const RendererRuntime::MaterialBlueprintResource* materialBlueprintResource = mMaterialResource->getMaterialBlueprintResource();
			if (nullptr != materialBlueprintResource)
			{
				// Set the used graphics root signature
				renderer.setGraphicsRootSignature(materialBlueprintResource->getRootSignature());

				// Graphics root descriptor table: Set our uniform buffer
				renderer.setGraphicsRootDescriptorTable(0, mUniformBuffer);

				// Graphics root descriptor table: Set material specific tables
				if (mMaterialResource->setGraphicsRootDescriptorTable(rendererRuntime))
				{
					Renderer::IPipelineState* pipelineState = mMaterialResource->getPipelineStateObject();
					if (nullptr != pipelineState)
					{
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
									RendererRuntime::MeshSceneItem* meshSceneItem = static_cast<RendererRuntime::MeshSceneItem*>(sceneItem);

									{ // Set uniform
										// Calculate the object space to clip space matrix
										glm::mat4 viewSpaceToClipSpace		= glm::perspective(45.0f, aspectRatio, 0.1f, 100.f);
										glm::mat4 viewTranslate				= glm::translate(glm::mat4(1.0f), transform.position);
										glm::mat4 worldSpaceToViewSpace		= viewTranslate * glm::toMat4(transform.rotation);
										glm::mat4 objectSpaceToWorldSpace	= glm::scale(glm::mat4(1.0f), transform.scale);
										glm::mat4 objectSpaceToViewSpace	= worldSpaceToViewSpace * objectSpaceToWorldSpace;
										glm::mat4 objectSpaceToClipSpace	= viewSpaceToClipSpace * objectSpaceToViewSpace;

										// Upload the uniform data
										// -> Two versions: One using an uniform buffer and one setting an individual uniform
										if (nullptr != mUniformBuffer)
										{
											struct UniformBlockDynamicVs
											{
												float objectSpaceToClipSpaceMatrix[4 * 4];	// Object space to clip space matrix
												float objectSpaceToViewSpaceMatrix[4 * 4];	// Object space to view space matrix
											};
											UniformBlockDynamicVs uniformBlockDynamicVS;
											memcpy(uniformBlockDynamicVS.objectSpaceToClipSpaceMatrix, glm::value_ptr(objectSpaceToClipSpace), sizeof(float) * 4 * 4);

											// TODO(co) float3x3 (currently there are alignment issues when using Direct3D, have a look into possible solutions)
											glm::mat3 objectSpaceToViewSpace3x3 = glm::mat3(objectSpaceToViewSpace);
											objectSpaceToViewSpace = glm::mat4(objectSpaceToViewSpace3x3);
											memcpy(uniformBlockDynamicVS.objectSpaceToViewSpaceMatrix, glm::value_ptr(objectSpaceToViewSpace), sizeof(float) * 4 * 4);

											// Copy data
											mUniformBuffer->copyDataFrom(sizeof(UniformBlockDynamicVs), &uniformBlockDynamicVS);
										}
									}

									// Draw mesh instance
									RendererRuntime::MeshResource* meshResource = meshSceneItem->getMeshResource();
									if (nullptr != meshResource)
									{
										meshResource->draw();
									}
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
