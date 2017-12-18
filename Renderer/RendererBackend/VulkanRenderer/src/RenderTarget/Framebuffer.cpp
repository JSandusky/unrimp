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
#include "VulkanRenderer/RenderTarget/Framebuffer.h"
#include "VulkanRenderer/RenderTarget/RenderPass.h"
#include "VulkanRenderer/Texture/Texture2DArray.h"
#include "VulkanRenderer/Texture/Texture2D.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/Context.h>
#include <Renderer/IAllocator.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	#include <vector>
PRAGMA_WARNING_POP
#include <limits.h>	// For "INT_MAX"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Framebuffer::Framebuffer(Renderer::IRenderPass& renderPass, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment) :
		IFramebuffer(renderPass),
		mNumberOfColorTextures(static_cast<RenderPass&>(renderPass).getNumberOfColorAttachments()),
		mColorTextures(nullptr),	// Set below
		mDepthStencilTexture(nullptr),
		mWidth(UINT_MAX),
		mHeight(UINT_MAX),
		mVkRenderPass(static_cast<RenderPass&>(renderPass).getVkRenderPass()),
		mVkFramebuffer(VK_NULL_HANDLE)
	{
		// Vulkan attachment descriptions and views to fill
		std::vector<VkImageView> vkImageViews;
		vkImageViews.resize(mNumberOfColorTextures + ((nullptr != depthStencilFramebufferAttachment) ? 1u : 0u));
		uint32_t currentVkAttachmentDescriptionIndex = 0;

		// Add a reference to the used color textures
		if (mNumberOfColorTextures > 0)
		{
			mColorTextures = RENDERER_MALLOC_TYPED(renderPass.getRenderer().getContext(), Renderer::ITexture*, mNumberOfColorTextures);

			// Loop through all color textures
			Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture, ++colorFramebufferAttachments)
			{
				// Sanity check
				RENDERER_ASSERT(renderPass.getRenderer().getContext(), nullptr != colorFramebufferAttachments->texture, "Invalid Vulkan color framebuffer attachment texture")

				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*colorTexture = colorFramebufferAttachments->texture;
				(*colorTexture)->addReference();

				// Evaluate the color texture type
				VkImageView vkImageView = VK_NULL_HANDLE;
				switch ((*colorTexture)->getResourceType())
				{
					case Renderer::ResourceType::TEXTURE_2D:
					{
						// Sanity check
						RENDERER_ASSERT(renderPass.getRenderer().getContext(), 0 == colorFramebufferAttachments->layerIndex, "Invalid Vulkan color framebuffer attachment layer index")

						// Update the framebuffer width and height if required
						Texture2D* texture2D = static_cast<Texture2D*>(*colorTexture);
						vkImageView = texture2D->getVkImageView();
						if (mWidth > texture2D->getWidth())
						{
							mWidth = texture2D->getWidth();
						}
						if (mHeight > texture2D->getHeight())
						{
							mHeight = texture2D->getHeight();
						}
						break;
					}

					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					{
						// Update the framebuffer width and height if required
						Texture2DArray* texture2DArray = static_cast<Texture2DArray*>(*colorTexture);
						vkImageView = texture2DArray->getVkImageView();
						if (mWidth > texture2DArray->getWidth())
						{
							mWidth = texture2DArray->getWidth();
						}
						if (mHeight > texture2DArray->getHeight())
						{
							mHeight = texture2DArray->getHeight();
						}
						break;
					}

					case Renderer::ResourceType::ROOT_SIGNATURE:
					case Renderer::ResourceType::RESOURCE_GROUP:
					case Renderer::ResourceType::PROGRAM:
					case Renderer::ResourceType::VERTEX_ARRAY:
					case Renderer::ResourceType::RENDER_PASS:
					case Renderer::ResourceType::SWAP_CHAIN:
					case Renderer::ResourceType::FRAMEBUFFER:
					case Renderer::ResourceType::INDEX_BUFFER:
					case Renderer::ResourceType::VERTEX_BUFFER:
					case Renderer::ResourceType::UNIFORM_BUFFER:
					case Renderer::ResourceType::TEXTURE_BUFFER:
					case Renderer::ResourceType::INDIRECT_BUFFER:
					case Renderer::ResourceType::TEXTURE_1D:
					case Renderer::ResourceType::TEXTURE_3D:
					case Renderer::ResourceType::TEXTURE_CUBE:
					case Renderer::ResourceType::PIPELINE_STATE:
					case Renderer::ResourceType::SAMPLER_STATE:
					case Renderer::ResourceType::VERTEX_SHADER:
					case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
					case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
					case Renderer::ResourceType::GEOMETRY_SHADER:
					case Renderer::ResourceType::FRAGMENT_SHADER:
					default:
						// Nothing here
						break;
				}

				// Remember the Vulkan image view
				vkImageViews[currentVkAttachmentDescriptionIndex] = vkImageView;

				// Advance current Vulkan attachment description index
				++currentVkAttachmentDescriptionIndex;
			}
		}

		// Add a reference to the used depth stencil texture
		if (nullptr != depthStencilFramebufferAttachment)
		{
			mDepthStencilTexture = depthStencilFramebufferAttachment->texture;
			RENDERER_ASSERT(renderPass.getRenderer().getContext(), nullptr != mDepthStencilTexture, "Invalid Vulkan depth stencil framebuffer attachment texture")
			mDepthStencilTexture->addReference();

			// Evaluate the depth stencil texture type
			VkImageView vkImageView = VK_NULL_HANDLE;
			switch (mDepthStencilTexture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Sanity check
					RENDERER_ASSERT(renderPass.getRenderer().getContext(), 0 == depthStencilFramebufferAttachment->layerIndex, "Invalid Vulkan depth stencil framebuffer attachment layer index")

					// Update the framebuffer width and height if required
					Texture2D* texture2D = static_cast<Texture2D*>(mDepthStencilTexture);
					vkImageView = texture2D->getVkImageView();
					if (mWidth > texture2D->getWidth())
					{
						mWidth = texture2D->getWidth();
					}
					if (mHeight > texture2D->getHeight())
					{
						mHeight = texture2D->getHeight();
					}
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Update the framebuffer width and height if required
					Texture2DArray* texture2DArray = static_cast<Texture2DArray*>(mDepthStencilTexture);
					vkImageView = texture2DArray->getVkImageView();
					if (mWidth > texture2DArray->getWidth())
					{
						mWidth = texture2DArray->getWidth();
					}
					if (mHeight > texture2DArray->getHeight())
					{
						mHeight = texture2DArray->getHeight();
					}
					break;
				}

				case Renderer::ResourceType::ROOT_SIGNATURE:
				case Renderer::ResourceType::RESOURCE_GROUP:
				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::RENDER_PASS:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::INDIRECT_BUFFER:
				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_3D:
				case Renderer::ResourceType::TEXTURE_CUBE:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				default:
					// Nothing here
					break;
			}

			// Remember the Vulkan image view
			vkImageViews[currentVkAttachmentDescriptionIndex] = vkImageView;
		}

		// Validate the framebuffer width and height
		if (0 == mWidth || UINT_MAX == mWidth)
		{
			RENDERER_ASSERT(renderPass.getRenderer().getContext(), false, "Invalid Vulkan framebuffer width")
			mWidth = 1;
		}
		if (0 == mHeight || UINT_MAX == mHeight)
		{
			RENDERER_ASSERT(renderPass.getRenderer().getContext(), false, "Invalid Vulkan framebuffer height")
			mHeight = 1;
		}

		// Create Vulkan framebuffer
		const VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(renderPass.getRenderer());
		const VkFramebufferCreateInfo vkFramebufferCreateInfo =
		{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,	// sType (VkStructureType)
			nullptr,									// pNext (const void*)
			0,											// flags (VkFramebufferCreateFlags)
			mVkRenderPass,								// renderPass (VkRenderPass)
			static_cast<uint32_t>(vkImageViews.size()),	// attachmentCount (uint32_t)
			vkImageViews.data(),						// pAttachments (const VkImageView*)
			mWidth,										// width (uint32_t)
			mHeight,									// height (uint32_t
			1											// layers (uint32_t)
		};
		if (vkCreateFramebuffer(vulkanRenderer.getVulkanContext().getVkDevice(), &vkFramebufferCreateInfo, vulkanRenderer.getVkAllocationCallbacks(), &mVkFramebuffer) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create Vulkan framebuffer")
		}
		else
		{
			SET_DEFAULT_DEBUG_NAME	// setDebugName("");
		}
	}

	Framebuffer::~Framebuffer()
	{
		const VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
		const VkDevice vkDevice = vulkanRenderer.getVulkanContext().getVkDevice();

		// Destroy Vulkan framebuffer instance
		if (VK_NULL_HANDLE != mVkFramebuffer)
		{
			vkDestroyFramebuffer(vkDevice, mVkFramebuffer, vulkanRenderer.getVkAllocationCallbacks());
		}

		// Release the reference to the used color textures
		if (nullptr != mColorTextures)
		{
			// Release references
			Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture)
			{
				(*colorTexture)->releaseReference();
			}

			// Cleanup
			RENDERER_FREE(getRenderer().getContext(), mColorTextures);
		}

		// Release the reference to the used depth stencil texture
		if (nullptr != mDepthStencilTexture)
		{
			// Release reference
			mDepthStencilTexture->releaseReference();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void Framebuffer::setDebugName(const char* name)
		{
			if (nullptr != vkDebugMarkerSetObjectNameEXT)
			{
				const VkDevice vkDevice = static_cast<const VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice();
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT, (uint64_t)mVkRenderPass, name);
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT, (uint64_t)mVkFramebuffer, name);
			}
		}
	#endif


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void Framebuffer::getWidthAndHeight(uint32_t& width, uint32_t& height) const
	{
		// No fancy implementation in here, just copy over the internal information
		width  = mWidth;
		height = mHeight;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void Framebuffer::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), Framebuffer, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
