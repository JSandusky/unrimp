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
#include "VulkanRenderer/RenderTarget/RenderPass.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/Mapping.h"

#include <Renderer/ILog.h>
#include <Renderer/IAllocator.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	#include <vector>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RenderPass::RenderPass(VulkanRenderer& vulkanRenderer, uint32_t numberOfColorAttachments, const Renderer::TextureFormat::Enum* colorAttachmentTextureFormats, Renderer::TextureFormat::Enum depthStencilAttachmentTextureFormat, uint8_t numberOfMultisamples) :
		IRenderPass(vulkanRenderer),
		mVkRenderPass(VK_NULL_HANDLE),
		mNumberOfColorAttachments(numberOfColorAttachments),
		mDepthStencilAttachmentTextureFormat(depthStencilAttachmentTextureFormat),
		mVkSampleCountFlagBits(Mapping::getVulkanSampleCountFlagBits(vulkanRenderer.getContext(), numberOfMultisamples))
	{
		const bool hasDepthStencilAttachment = (Renderer::TextureFormat::Enum::UNKNOWN != depthStencilAttachmentTextureFormat);

		// Vulkan attachment descriptions
		std::vector<VkAttachmentDescription> vkAttachmentDescriptions;
		vkAttachmentDescriptions.resize(mNumberOfColorAttachments + (hasDepthStencilAttachment ? 1u : 0u));
		uint32_t currentVkAttachmentDescriptionIndex = 0;

		// Handle color attachments
		typedef std::vector<VkAttachmentReference> VkAttachmentReferences;
		VkAttachmentReferences colorVkAttachmentReferences;
		if (mNumberOfColorAttachments > 0)
		{
			colorVkAttachmentReferences.resize(mNumberOfColorAttachments);
			for (uint32_t i = 0; i < mNumberOfColorAttachments; ++i)
			{
				{ // Setup Vulkan color attachment references
					VkAttachmentReference& vkAttachmentReference = colorVkAttachmentReferences[currentVkAttachmentDescriptionIndex];
					vkAttachmentReference.attachment = currentVkAttachmentDescriptionIndex;			// attachment (uint32_t)
					vkAttachmentReference.layout	 = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;	// layout (VkImageLayout)
				}

				{ // Setup Vulkan color attachment description
					VkAttachmentDescription& vkAttachmentDescription = vkAttachmentDescriptions[currentVkAttachmentDescriptionIndex];
					vkAttachmentDescription.flags		   = 0;																// flags (VkAttachmentDescriptionFlags)
					vkAttachmentDescription.format		   = Mapping::getVulkanFormat(colorAttachmentTextureFormats[i]);	// format (VkFormat)
					vkAttachmentDescription.samples		   = mVkSampleCountFlagBits;										// samples (VkSampleCountFlagBits)
					vkAttachmentDescription.loadOp		   = VK_ATTACHMENT_LOAD_OP_CLEAR;									// loadOp (VkAttachmentLoadOp)
					vkAttachmentDescription.storeOp		   = VK_ATTACHMENT_STORE_OP_STORE;									// storeOp (VkAttachmentStoreOp)
					vkAttachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;								// stencilLoadOp (VkAttachmentLoadOp)
					vkAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;								// stencilStoreOp (VkAttachmentStoreOp)
					vkAttachmentDescription.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;										// initialLayout (VkImageLayout)
					vkAttachmentDescription.finalLayout	   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;						// finalLayout (VkImageLayout)
				}

				// Advance current Vulkan attachment description index
				++currentVkAttachmentDescriptionIndex;
			}
		}

		// Handle depth stencil attachments
		const VkAttachmentReference depthVkAttachmentReference =
		{
			currentVkAttachmentDescriptionIndex,				// attachment (uint32_t)
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL	// layout (VkImageLayout)
		};
		if (hasDepthStencilAttachment)
		{
			// Setup Vulkan depth attachment description
			VkAttachmentDescription& vkAttachmentDescription = vkAttachmentDescriptions[currentVkAttachmentDescriptionIndex];
			vkAttachmentDescription.flags		   = 0;																// flags (VkAttachmentDescriptionFlags)
			vkAttachmentDescription.format		   = Mapping::getVulkanFormat(depthStencilAttachmentTextureFormat);	// format (VkFormat)
			vkAttachmentDescription.samples		   = mVkSampleCountFlagBits;										// samples (VkSampleCountFlagBits)
			vkAttachmentDescription.loadOp		   = VK_ATTACHMENT_LOAD_OP_CLEAR;									// loadOp (VkAttachmentLoadOp)
			vkAttachmentDescription.storeOp		   = VK_ATTACHMENT_STORE_OP_STORE;									// storeOp (VkAttachmentStoreOp)
			vkAttachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;								// stencilLoadOp (VkAttachmentLoadOp)
			vkAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;								// stencilStoreOp (VkAttachmentStoreOp)
			vkAttachmentDescription.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;										// initialLayout (VkImageLayout)
			vkAttachmentDescription.finalLayout	   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;				// finalLayout (VkImageLayout)
			// ++currentVkAttachmentDescriptionIndex;	// Not needed since we're the last
		}

		// Create Vulkan create render pass
		const VkSubpassDescription vkSubpassDescription =
		{
			0,																				// flags (VkSubpassDescriptionFlags)
			VK_PIPELINE_BIND_POINT_GRAPHICS,												// pipelineBindPoint (VkPipelineBindPoint)
			0,																				// inputAttachmentCount (uint32_t)
			nullptr,																		// pInputAttachments (const VkAttachmentReference*)
			mNumberOfColorAttachments,														// colorAttachmentCount (uint32_t)
			(mNumberOfColorAttachments > 0) ? colorVkAttachmentReferences.data() : nullptr,	// pColorAttachments (const VkAttachmentReference*)
			nullptr,																		// pResolveAttachments (const VkAttachmentReference*)
			hasDepthStencilAttachment ? &depthVkAttachmentReference : nullptr,				// pDepthStencilAttachment (const VkAttachmentReference*)
			0,																				// preserveAttachmentCount (uint32_t)
			nullptr																			// pPreserveAttachments (const uint32_t*)
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
	}

	RenderPass::~RenderPass()
	{
		// Destroy Vulkan render pass instance
		if (VK_NULL_HANDLE != mVkRenderPass)
		{
			vkDestroyRenderPass(static_cast<VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(), mVkRenderPass, nullptr);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void RenderPass::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), RenderPass, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
