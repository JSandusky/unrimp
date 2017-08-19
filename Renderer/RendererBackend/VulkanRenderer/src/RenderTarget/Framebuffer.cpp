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
#include "VulkanRenderer/Texture/Texture2DArray.h"
#include "VulkanRenderer/Texture/Texture2D.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"

#include <Renderer/ILog.h>
#include <Renderer/Context.h>

#include <vector>
#include <limits.h>	// For "INT_MAX"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Framebuffer::Framebuffer(VulkanRenderer& vulkanRenderer, uint32_t numberOfColorFramebufferAttachments, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment) :
		IFramebuffer(vulkanRenderer),
		mNumberOfColorTextures(numberOfColorFramebufferAttachments),
		mColorTextures(nullptr),	// Set below
		mDepthStencilTexture(nullptr),
		mWidth(UINT_MAX),
		mHeight(UINT_MAX),
		mRenderPass(vulkanRenderer, mVkRenderPass, numberOfColorFramebufferAttachments, nullptr != depthStencilFramebufferAttachment),
		mVkRenderPass(VK_NULL_HANDLE),
		mVkFramebuffer(VK_NULL_HANDLE)
	{
		mRenderPass.addReference();

		// Vulkan attachment descriptions and views to fill
		std::vector<VkAttachmentDescription> vkAttachmentDescriptions;
		vkAttachmentDescriptions.resize(mNumberOfColorTextures + ((nullptr != depthStencilFramebufferAttachment) ? 1u : 0u));
		std::vector<VkImageView> vkImageViews;
		vkImageViews.resize(vkAttachmentDescriptions.size());
		uint32_t currentVkAttachmentDescriptionIndex = 0;

		// Add a reference to the used color textures
		typedef std::vector<VkAttachmentReference> VkAttachmentReferences;
		VkAttachmentReferences colorVkAttachmentReferences;
		if (mNumberOfColorTextures > 0)
		{
			colorVkAttachmentReferences.resize(mNumberOfColorTextures);
			mColorTextures = new Renderer::ITexture*[mNumberOfColorTextures];

			// Loop through all color textures
			Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture, ++colorFramebufferAttachments)
			{
				// Sanity check
				assert(nullptr != colorFramebufferAttachments->texture);

				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*colorTexture = colorFramebufferAttachments->texture;
				(*colorTexture)->addReference();

				// Evaluate the color texture type
				VkImageView vkImageView = VK_NULL_HANDLE;
				VkFormat vkFormat = VK_FORMAT_UNDEFINED;
				switch ((*colorTexture)->getResourceType())
				{
					case Renderer::ResourceType::TEXTURE_2D:
					{
						// Sanity check
						assert(0 == colorFramebufferAttachments->layerIndex);

						// Update the framebuffer width and height if required
						Texture2D* texture2D = static_cast<Texture2D*>(*colorTexture);
						vkImageView = texture2D->getVkImageView();
						vkFormat = texture2D->getVkFormat();
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
						vkFormat = texture2DArray->getVkFormat();
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

				{ // Setup Vulkan color attachment references
					VkAttachmentReference& vkAttachmentReference = colorVkAttachmentReferences[currentVkAttachmentDescriptionIndex];
					vkAttachmentReference.attachment = currentVkAttachmentDescriptionIndex;			// attachment (uint32_t)
					vkAttachmentReference.layout	 = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;	// layout (VkImageLayout)
				}

				{ // Setup Vulkan color attachment description
					VkAttachmentDescription& vkAttachmentDescription = vkAttachmentDescriptions[currentVkAttachmentDescriptionIndex];
					vkAttachmentDescription.flags		   = 0;											// flags (VkAttachmentDescriptionFlags)
					vkAttachmentDescription.format		   = vkFormat;									// format (VkFormat)
					vkAttachmentDescription.samples		   = VK_SAMPLE_COUNT_1_BIT;						// samples (VkSampleCountFlagBits)
					vkAttachmentDescription.loadOp		   = VK_ATTACHMENT_LOAD_OP_CLEAR;				// loadOp (VkAttachmentLoadOp)
					vkAttachmentDescription.storeOp		   = VK_ATTACHMENT_STORE_OP_STORE;				// storeOp (VkAttachmentStoreOp)
					vkAttachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;			// stencilLoadOp (VkAttachmentLoadOp)
					vkAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;			// stencilStoreOp (VkAttachmentStoreOp)
					vkAttachmentDescription.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;					// initialLayout (VkImageLayout)
					vkAttachmentDescription.finalLayout	   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;	// finalLayout (VkImageLayout)
				}

				// Advance current Vulkan attachment description index
				++currentVkAttachmentDescriptionIndex;
			}
		}

		// Add a reference to the used depth stencil texture
		const VkAttachmentReference depthVkAttachmentReference =
		{
			currentVkAttachmentDescriptionIndex,				// attachment (uint32_t)
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL	// layout (VkImageLayout)
		};
		if (nullptr != depthStencilFramebufferAttachment)
		{
			mDepthStencilTexture = depthStencilFramebufferAttachment->texture;
			assert(nullptr != mDepthStencilTexture);
			mDepthStencilTexture->addReference();

			// Evaluate the depth stencil texture type
			VkImageView vkImageView = VK_NULL_HANDLE;
			VkFormat vkFormat = VK_FORMAT_UNDEFINED;
			switch (mDepthStencilTexture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Sanity check
					assert(0 == depthStencilFramebufferAttachment->layerIndex);

					// Update the framebuffer width and height if required
					Texture2D* texture2D = static_cast<Texture2D*>(mDepthStencilTexture);
					vkImageView = texture2D->getVkImageView();
					vkFormat = texture2D->getVkFormat();
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
					vkFormat = texture2DArray->getVkFormat();
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

			{ // Setup Vulkan depth attachment description
				VkAttachmentDescription& vkAttachmentDescription = vkAttachmentDescriptions[currentVkAttachmentDescriptionIndex];
				vkAttachmentDescription.flags		   = 0;													// flags (VkAttachmentDescriptionFlags)
				vkAttachmentDescription.format		   = vkFormat;											// format (VkFormat)
				vkAttachmentDescription.samples		   = VK_SAMPLE_COUNT_1_BIT;								// samples (VkSampleCountFlagBits)
				vkAttachmentDescription.loadOp		   = VK_ATTACHMENT_LOAD_OP_CLEAR;						// loadOp (VkAttachmentLoadOp)
				vkAttachmentDescription.storeOp		   = VK_ATTACHMENT_STORE_OP_STORE;						// storeOp (VkAttachmentStoreOp)
				vkAttachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;					// stencilLoadOp (VkAttachmentLoadOp)
				vkAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;					// stencilStoreOp (VkAttachmentStoreOp)
				vkAttachmentDescription.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;							// initialLayout (VkImageLayout)
				vkAttachmentDescription.finalLayout	   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;	// finalLayout (VkImageLayout)
				// ++currentVkAttachmentDescriptionIndex;	// Not needed since we're the last
			}
		}

		// Validate the framebuffer width and height
		if (0 == mWidth || UINT_MAX == mWidth)
		{
			assert(false);
			mWidth = 1;
		}
		if (0 == mHeight || UINT_MAX == mHeight)
		{
			assert(false);
			mHeight = 1;
		}

		// Create Vulkan create render pass
		const VkSubpassDescription vkSubpassDescription =
		{
			0,																						// flags (VkSubpassDescriptionFlags)
			VK_PIPELINE_BIND_POINT_GRAPHICS,														// pipelineBindPoint (VkPipelineBindPoint)
			0,																						// inputAttachmentCount (uint32_t)
			nullptr,																				// pInputAttachments (const VkAttachmentReference*)
			mNumberOfColorTextures,																	// colorAttachmentCount (uint32_t)
			(mNumberOfColorTextures > 0) ? colorVkAttachmentReferences.data() : nullptr,			// pColorAttachments (const VkAttachmentReference*)
			nullptr,																				// pResolveAttachments (const VkAttachmentReference*)
			(nullptr != depthStencilFramebufferAttachment) ? &depthVkAttachmentReference : nullptr,	// pDepthStencilAttachment (const VkAttachmentReference*)
			0,																						// preserveAttachmentCount (uint32_t)
			nullptr																					// pPreserveAttachments (const uint32_t*)
		};
		const std::array<VkSubpassDependency, 2> vkSubpassDependencies =
		{{
			{
				VK_SUBPASS_EXTERNAL,														// srcSubpass (uint32_t)
				0,																			// dstSubpass (uint32_t)
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,										// srcStageMask (VkPipelineStageFlags)
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,								// dstStageMask (VkPipelineStageFlags)
				VK_ACCESS_MEMORY_READ_BIT,													// srcAccessMask (VkAccessFlags)
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,	// dstAccessMask (VkAccessFlags)
				VK_DEPENDENCY_BY_REGION_BIT													// dependencyFlags (VkDependencyFlags)
			},
			{
				0,																			// srcSubpass (uint32_t)
				VK_SUBPASS_EXTERNAL,														// dstSubpass (uint32_t)
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,								// srcStageMask (VkPipelineStageFlags)
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,										// dstStageMask (VkPipelineStageFlags)
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,	// srcAccessMask (VkAccessFlags)
				VK_ACCESS_MEMORY_READ_BIT,													// dstAccessMask (VkAccessFlags)
				VK_DEPENDENCY_BY_REGION_BIT													// dependencyFlags (VkDependencyFlags)
			}
		}};
		const VkRenderPassCreateInfo vkRenderPassCreateInfo =
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,				// sType (VkStructureType)
			nullptr,												// pNext (const void*)
			0,														// flags (VkRenderPassCreateFlags)
			static_cast<uint32_t>(vkAttachmentDescriptions.size()),	// attachmentCount (uint32_t)
			vkAttachmentDescriptions.data(),						// pAttachments (const VkAttachmentDescription*)
			1,														// subpassCount (uint32_t)
			&vkSubpassDescription,									// pSubpasses (const VkSubpassDescription*)
			static_cast<uint32_t>(vkSubpassDependencies.size()),	// dependencyCount (uint32_t)
			vkSubpassDependencies.data()							// pDependencies (const VkSubpassDependency*)
		};
		const VkDevice vkDevice = vulkanRenderer.getVulkanContext().getVkDevice();
		const Renderer::Context& context = vulkanRenderer.getContext();
		if (vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, nullptr, &mVkRenderPass) != VK_SUCCESS)
		{
			RENDERER_LOG(context, CRITICAL, "Failed to create Vulkan render pass")
		}

		// Create Vulkan framebuffer
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
		if (vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, nullptr, &mVkFramebuffer) != VK_SUCCESS)
		{
			RENDERER_LOG(context, CRITICAL, "Failed to create Vulkan framebuffer")
		}
	}

	Framebuffer::~Framebuffer()
	{
		const VkDevice vkDevice = static_cast<VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice();

		// Destroy Vulkan framebuffer instance
		if (VK_NULL_HANDLE != mVkFramebuffer)
		{
			vkDestroyFramebuffer(vkDevice, mVkFramebuffer, nullptr);
		}

		// Destroy Vulkan render pass instance
		if (VK_NULL_HANDLE != mVkRenderPass)
		{
			vkDestroyRenderPass(vkDevice, mVkRenderPass, nullptr);
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
			delete [] mColorTextures;
		}

		// Release the reference to the used depth stencil texture
		if (nullptr != mDepthStencilTexture)
		{
			// Release reference
			mDepthStencilTexture->releaseReference();
		}
	}


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
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
