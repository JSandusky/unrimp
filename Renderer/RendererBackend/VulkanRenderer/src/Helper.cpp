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
#include "VulkanRenderer/Helper.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"

#include <Renderer/ILog.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	void Helper::setImageLayout(VkCommandBuffer vkCommandBuffer, VkImage vkImage, VkImageLayout oldVkImageLayout, VkImageLayout newVkImageLayout, VkImageAspectFlags vkImageAspectFlags)
	{
		const VkImageSubresourceRange vkImageSubresourceRange =
		{
			vkImageAspectFlags,	// aspectMask (VkImageAspectFlags)
			0,					// baseMipLevel (uint32_t)
			1,					// levelCount (uint32_t)
			0,					// baseArrayLayer (uint32_t)
			1					// layerCount (uint32_t)
		};
		setImageLayout(vkCommandBuffer, vkImage, oldVkImageLayout, newVkImageLayout, vkImageSubresourceRange);
	}

	void Helper::setImageLayout(VkCommandBuffer vkCommandBuffer, VkImage vkImage, VkImageLayout oldVkImageLayout, VkImageLayout newVkImageLayout, VkImageSubresourceRange vkImageSubresourceRange)
	{
		// Create an image memory barrier for changing the layout of an image and put it into an active command buffer
		// -> See chapter 11.4 "Image Layout" for details
		// -> Basing on https://github.com/SaschaWillems/Vulkan - "Examples and demos for the new Vulkan API" - "vkTools::setImageLayout()" from Sascha Willems

		// Create an image barrier object
		VkImageMemoryBarrier vkImageMemoryBarrier = {};
		vkImageMemoryBarrier.sType				 = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		vkImageMemoryBarrier.oldLayout			 = oldVkImageLayout;
		vkImageMemoryBarrier.newLayout			 = newVkImageLayout;
		vkImageMemoryBarrier.image				 = vkImage;
		vkImageMemoryBarrier.subresourceRange	 = vkImageSubresourceRange;

		{ // Source layouts (old)
			// Undefined layout
			// Only allowed as initial layout!
			// Make sure any writes to the image have been finished
			if (VK_IMAGE_LAYOUT_PREINITIALIZED == oldVkImageLayout)
			{
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}

			// Old layout is color attachment
			// Make sure any writes to the color buffer have been finished
			if (VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL == oldVkImageLayout)
			{
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			}

			// Old layout is depth/stencil attachment
			// Make sure any writes to the depth/stencil buffer have been finished
			if (VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL == oldVkImageLayout)
			{
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			}

			// Old layout is transfer source
			// Make sure any reads from the image have been finished
			if (VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL == oldVkImageLayout)
			{
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			}

			// Old layout is shader read (sampler, input attachment)
			// Make sure any shader reads from the image have been finished
			if (VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL == oldVkImageLayout)
			{
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			}
		}

		{ // Target layouts (new)
			// New layout is transfer destination (copy, blit)
			// Make sure any copies to the image have been finished
			if (VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == newVkImageLayout)
			{
				vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			}

			// New layout is transfer source (copy, blit)
			// Make sure any reads from and writes to the image have been finished
			if (VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL == newVkImageLayout)
			{
				vkImageMemoryBarrier.srcAccessMask = vkImageMemoryBarrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
				vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			}

			// New layout is color attachment
			// Make sure any writes to the color buffer have been finished
			if (VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL == newVkImageLayout)
			{
				vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			}

			// New layout is depth attachment
			// Make sure any writes to depth/stencil buffer have been finished
			if (VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL == newVkImageLayout)
			{
				vkImageMemoryBarrier.dstAccessMask = vkImageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			}

			// New layout is shader read (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL == newVkImageLayout)
			{
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			}
		}

		// Put barrier inside command buffer
		vkCmdPipelineBarrier(vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vkImageMemoryBarrier);
	}

	void Helper::createAndAllocateVkBuffer(const VulkanRenderer& vulkanRenderer, VkBufferUsageFlagBits vkBufferUsageFlagBits, uint32_t numberOfBytes, const void* data, VkBuffer& vkBuffer, VkDeviceMemory& vkDeviceMemory)
	{
		const VulkanContext& vulkanContext = vulkanRenderer.getVulkanContext();
		const VkDevice vkDevice = vulkanContext.getVkDevice();

		// Create the Vulkan buffer
		const VkBufferCreateInfo vkBufferCreateInfo =
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,					// sType (VkStructureType)
			nullptr,												// pNext (const void*)
			0,														// flags (VkBufferCreateFlags)
			numberOfBytes,											// size (VkDeviceSize)
			static_cast<VkBufferUsageFlags>(vkBufferUsageFlagBits),	// usage (VkBufferUsageFlags)
			VK_SHARING_MODE_EXCLUSIVE,								// sharingMode (VkSharingMode)
			0,														// queueFamilyIndexCount (uint32_t)
			nullptr													// pQueueFamilyIndices (const uint32_t*)
		};
		if (vkCreateBuffer(vkDevice, &vkBufferCreateInfo, nullptr, &vkBuffer) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan buffer")
		}

		// Allocate memory for the Vulkan buffer
		VkMemoryRequirements vkMemoryRequirements = {};
		vkGetBufferMemoryRequirements(vkDevice, vkBuffer, &vkMemoryRequirements);
		const VkMemoryAllocateInfo vkMemoryAllocateInfo =
		{
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,																											// sType (VkStructureType)
			nullptr,																																		// pNext (const void*)
			vkMemoryRequirements.size,																														// allocationSize (VkDeviceSize)
			vulkanContext.findMemoryType(vkMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)	// memoryTypeIndex (uint32_t)
		};
		if (vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, nullptr, &vkDeviceMemory) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to allocate the Vulkan buffer memory")
		}

		// Bind and fill memory
		vkBindBufferMemory(vkDevice, vkBuffer, vkDeviceMemory, 0);
		if (nullptr != data)
		{
			void* mappedData = nullptr;
			vkMapMemory(vkDevice, vkDeviceMemory, 0, vkBufferCreateInfo.size, 0, &mappedData);
				memcpy(mappedData, data, static_cast<size_t>(vkBufferCreateInfo.size));
			vkUnmapMemory(vkDevice, vkDeviceMemory);
		}
	}

	void Helper::destroyAndFreeVkBuffer(const VulkanRenderer& vulkanRenderer, VkBuffer& vkBuffer, VkDeviceMemory& vkDeviceMemory)
	{
		if (VK_NULL_HANDLE != vkBuffer)
		{
			const VkDevice vkDevice = vulkanRenderer.getVulkanContext().getVkDevice();
			vkDestroyBuffer(vkDevice, vkBuffer, nullptr);
			if (VK_NULL_HANDLE != vkDeviceMemory)
			{
				vkFreeMemory(vkDevice, vkDeviceMemory, nullptr);
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
