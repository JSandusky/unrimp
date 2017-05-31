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
#include "RendererRuntime/Resource/CompositorNode/Pass/ShadowMap/CompositorInstancePassShadowMap.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeInstance.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorContextData.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/Scene/Item/CameraSceneItem.h"
#include "RendererRuntime/Resource/Scene/Item/LightSceneItem.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/RenderQueue/RenderableManager.h"
#include "RendererRuntime/Core/Math/Math.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ICompositorInstancePass methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassShadowMap::onFillCommandBuffer(const Renderer::IRenderTarget& renderTarget, const CompositorContextData& compositorContextData, Renderer::CommandBuffer& commandBuffer)
	{
		const CameraSceneItem* cameraSceneItem = compositorContextData.getCameraSceneItem();
		const LightSceneItem* lightSceneItem = compositorContextData.getLightSceneItem();

		if (nullptr != mFramebufferPtr && nullptr != cameraSceneItem && cameraSceneItem->getParentSceneNode() && nullptr != lightSceneItem && lightSceneItem->getParentSceneNode())
		{
			const glm::vec3 worldSpaceSunLightDirection = lightSceneItem->getParentSceneNode()->getGlobalTransform().rotation * Math::VEC3_FORWARD;

			// Begin debug event
			COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer)

			// Set render target
			Renderer::Command::SetRenderTarget::create(commandBuffer, mFramebufferPtr);

			{ // Set the viewport and scissor rectangle
				const CompositorResourcePassShadowMap& compositorResourcePassShadowMap = static_cast<const CompositorResourcePassShadowMap&>(getCompositorResourcePass());
				const uint32_t shadowMapSize = compositorResourcePassShadowMap.getShadowMapSize();
				Renderer::Command::SetViewportAndScissorRectangle::create(commandBuffer, 0, 0, shadowMapSize, shadowMapSize);
			}

			{ // Clear the depth buffer of the current render target
				const float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				Renderer::Command::Clear::create(commandBuffer, Renderer::ClearFlag::DEPTH, color, 1.0f, 0);
			}

			// TODO(co) Totally primitive to have something to start with
			// Compute the MVP matrix from the light's point of view
			glm::mat4 depthProjectionMatrix = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, -30.0f, 30.0f);
			glm::mat4 depthViewMatrix = glm::lookAt(worldSpaceSunLightDirection, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			// Set custom camera matrices
			const_cast<CameraSceneItem*>(cameraSceneItem)->setCustomWorldSpaceToViewSpaceMatrix(depthViewMatrix);
			const_cast<CameraSceneItem*>(cameraSceneItem)->setCustomViewSpaceToClipSpaceMatrix(depthProjectionMatrix);

			{ // Render shadow casters
				assert(nullptr != mRenderQueueIndexRange);
				for (const RenderableManager* renderableManager : mRenderQueueIndexRange->renderableManagers)
				{
					// The render queue index range covered by this compositor instance pass scene might be smaller than the range of the
					// cached render queue index range. So, we could add a range check in here to reject renderable managers, but it's not
					// really worth to do so since the render queue only considers renderables inside the render queue range anyway.
					if (renderableManager->getCastShadows())
					{
						mRenderQueue.addRenderablesFromRenderableManager(*renderableManager, true);
					}
				}
				mRenderQueue.fillCommandBuffer(renderTarget, static_cast<const CompositorResourcePassScene&>(getCompositorResourcePass()).getMaterialTechniqueId(), compositorContextData, commandBuffer);
			}

			// Unset custom camera matrices
			const_cast<CameraSceneItem*>(cameraSceneItem)->unsetCustomWorldSpaceToViewSpaceMatrix();
			const_cast<CameraSceneItem*>(cameraSceneItem)->unsetCustomViewSpaceToClipSpaceMatrix();

			// Apply the scale/offset matrix, which transforms from [-1,1]
			// post-projection space to [0,1] UV space
			glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix;
			glm::mat4 depthBiasMVP = Math::getTextureScaleBiasMatrix(getCompositorNodeInstance().getCompositorWorkspaceInstance().getRendererRuntime().getRenderer()) * depthMVP;
			mPassData.shadowMatrix = depthBiasMVP;

			// Reset to previous render target
			// TODO(co) Get rid of this
			Renderer::Command::SetRenderTarget::create(commandBuffer, &const_cast<Renderer::IRenderTarget&>(renderTarget));

			// End debug event
			COMMAND_END_DEBUG_EVENT(commandBuffer)
		}
		else
		{
			// Error!
			assert(false);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassShadowMap::CompositorInstancePassShadowMap(const CompositorResourcePassShadowMap& compositorResourcePassShadowMap, const CompositorNodeInstance& compositorNodeInstance) :
		CompositorInstancePassScene(compositorResourcePassShadowMap, compositorNodeInstance),
		mTextureResourceId(getUninitialized<TextureResourceId>())
	{
		createShadowMapRenderTarget();
	}

	void CompositorInstancePassShadowMap::createShadowMapRenderTarget()
	{
		const CompositorResourcePassShadowMap& compositorResourcePassShadowMap = static_cast<const CompositorResourcePassShadowMap&>(getCompositorResourcePass());
		const IRendererRuntime& rendererRuntime = getCompositorNodeInstance().getCompositorWorkspaceInstance().getRendererRuntime();
		const AssetId assetId = compositorResourcePassShadowMap.getTextureAssetId();

		// Tell the texture resource manager about our render target texture so it can be referenced inside e.g. compositor nodes
		TextureResourceManager& textureResourceManager = rendererRuntime.getTextureResourceManager();
		TextureResource* textureResource = textureResourceManager.getTextureResourceByAssetId(assetId);
		if (nullptr == textureResource)
		{
			const uint32_t shadowMapSize = compositorResourcePassShadowMap.getShadowMapSize();

			// Create the texture instance, but without providing texture data (we use the texture as render target)
			// -> Use the "Renderer::TextureFlag::RENDER_TARGET"-flag to mark this texture as a render target
			// -> Required for Direct3D 9, Direct3D 10, Direct3D 11 and Direct3D 12
			// -> Not required for OpenGL and OpenGL ES 2
			// -> The optimized texture clear value is a Direct3D 12 related option
			Renderer::ITexture* texture = rendererRuntime.getTextureManager().createTexture2DArray(shadowMapSize, shadowMapSize, 2, Renderer::TextureFormat::D32_FLOAT, nullptr, Renderer::TextureFlag::RENDER_TARGET);
			RENDERER_SET_RESOURCE_DEBUG_NAME(texture, "Compositor instance pass shadow map")

			{ // Create the framebuffer object (FBO) instance
				// TODO(co) This is an infrastructure preparation for cascaded shadow maps support
				Renderer::FramebufferAttachment depthStencilFramebufferAttachment(texture, 0, 1);
				mFramebufferPtr = rendererRuntime.getRenderer().createFramebuffer(0, nullptr, &depthStencilFramebufferAttachment);
				RENDERER_SET_RESOURCE_DEBUG_NAME(mFramebufferPtr, "Compositor instance pass shadow map")
			}

			// Create texture resource
			mTextureResourceId = textureResourceManager.createTextureResourceByAssetId(assetId, *texture);
		}
		else
		{
			// This is not allowed to happen
			assert(false);
		}
	}

	void CompositorInstancePassShadowMap::destroyShadowMapRenderTarget()
	{
		assert(isInitialized(mTextureResourceId) && nullptr != mFramebufferPtr);

		// Inform the texture resource manager that our render target texture is gone now
		getCompositorNodeInstance().getCompositorWorkspaceInstance().getRendererRuntime().getTextureResourceManager().destroyTextureResource(mTextureResourceId);

		// Release the framebuffer and other renderer resources referenced by the framebuffer
		mFramebufferPtr = nullptr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
