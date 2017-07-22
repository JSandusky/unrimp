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
#include "VulkanRenderer/Mapping.h"

#include <Renderer/ILog.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	VkCommandBuffer Helper::beginSingleTimeCommands(const VulkanRenderer& vulkanRenderer)
	{
		// Create and begin Vulkan command buffer
		VkCommandBuffer vkCommandBuffer = vulkanRenderer.getVulkanContext().createVkCommandBuffer();
		const VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,	// sType (VkStructureType)
			nullptr,										// pNext (const void*)
			0,												// flags (VkCommandBufferUsageFlags)
			nullptr											// pInheritanceInfo (const VkCommandBufferInheritanceInfo*)
		};
		if (vkBeginCommandBuffer(vkCommandBuffer, &vkCommandBufferBeginInfo) == VK_SUCCESS)
		{
			// Done
			return vkCommandBuffer;
		}
		else
		{
			// Error!
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to begin Vulkan command buffer instance")
			return VK_NULL_HANDLE;
		}
	}

	void Helper::endSingleTimeCommands(const VulkanRenderer& vulkanRenderer, VkCommandBuffer vkCommandBuffer)
	{
		const VulkanContext& vulkanContext = vulkanRenderer.getVulkanContext();
		const VkQueue vkQueue = vulkanContext.getGraphicsVkQueue();

		// End Vulkan command buffer
		vkEndCommandBuffer(vkCommandBuffer);

		// Submit Vulkan command buffer
		const VkSubmitInfo vkSubmitInfo =
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,	// sType (VkStructureType)
			nullptr,						// pNext (const void*)
			0,								// waitSemaphoreCount (uint32_t)
			nullptr,						// pWaitSemaphores (const VkSemaphore*)
			nullptr,						// pWaitDstStageMask (const VkPipelineStageFlags*)
			1,								// commandBufferCount (uint32_t)
			&vkCommandBuffer,				// pCommandBuffers (const VkCommandBuffer*)
			0,								// signalSemaphoreCount (uint32_t)
			nullptr							// pSignalSemaphores (const VkSemaphore*)
		};
		if (vkQueueSubmit(vkQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			// Error!
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Vulkan queue submit failed")
			return;
		}
		if (vkQueueWaitIdle(vkQueue) != VK_SUCCESS)
		{
			// Error!
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Vulkan Queue wait idle failed")
			return;
		}

		// Destroy Vulkan command buffer
		vulkanContext.destroyVkCommandBuffer(vkCommandBuffer);
	}

	void Helper::transitionVkImageLayout(const VulkanRenderer& vulkanRenderer, VkImage vkImage, VkImageLayout oldVkImageLayout, VkImageLayout newVkImageLayout)
	{
		// Create and begin Vulkan command buffer
		VkCommandBuffer vkCommandBuffer = beginSingleTimeCommands(vulkanRenderer);

		// Vulkan image memory barrier
		VkImageMemoryBarrier vkImageMemoryBarrier =
		{
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,	// sType (VkStructureType)
			nullptr,								// pNext (const void*)
			0,										// srcAccessMask (VkAccessFlags)
			0,										// dstAccessMask (VkAccessFlags)
			oldVkImageLayout,						// oldLayout (VkImageLayout)
			newVkImageLayout,						// newLayout (VkImageLayout)
			VK_QUEUE_FAMILY_IGNORED,				// srcQueueFamilyIndex (uint32_t)
			VK_QUEUE_FAMILY_IGNORED,				// dstQueueFamilyIndex (uint32_t)
			vkImage,								// image (VkImage)
			{ // subresourceRange (VkImageSubresourceRange)
				VK_IMAGE_ASPECT_COLOR_BIT,	// aspectMask (VkImageAspectFlags)
				0,							// baseMipLevel (uint32_t)
				1,							// levelCount (uint32_t)
				0,							// baseArrayLayer (uint32_t)
				1							// layerCount (uint32_t)
			}
		};
		if (VK_IMAGE_LAYOUT_PREINITIALIZED == oldVkImageLayout && VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == newVkImageLayout)
		{
			vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		else if (VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == oldVkImageLayout && VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL == newVkImageLayout)
		{
			vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}
		else
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Unsupported Vulkan image layout transition")
		}
		vkCmdPipelineBarrier(vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vkImageMemoryBarrier);

		// End and destroy Vulkan command buffer
		endSingleTimeCommands(vulkanRenderer, vkCommandBuffer);
	}

	void Helper::createAndAllocateVkBuffer(const VulkanRenderer& vulkanRenderer, VkBufferUsageFlagBits vkBufferUsageFlagBits, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkDeviceSize numberOfBytes, const void* data, VkBuffer& vkBuffer, VkDeviceMemory& vkDeviceMemory)
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
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,															// sType (VkStructureType)
			nullptr,																						// pNext (const void*)
			vkMemoryRequirements.size,																		// allocationSize (VkDeviceSize)
			vulkanContext.findMemoryTypeIndex(vkMemoryRequirements.memoryTypeBits, vkMemoryPropertyFlags)	// memoryTypeIndex (uint32_t)
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

	void Helper::createAndFillVkImage(const VulkanRenderer& vulkanRenderer, VkImageType vkImageType, VkImageViewType vkImageViewType, const VkExtent3D& vkExtent3D, Renderer::TextureFormat::Enum textureFormat, const void* data, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory, VkImageView& vkImageView)
	{
		// Get Vulkan format
		const VkFormat vkFormat = Mapping::getVulkanFormat(textureFormat);

		// Create Vulkan staging buffer
		VkBuffer stagingVkBuffer;
		VkDeviceMemory stagingVkDeviceMemory;
		const uint32_t numberOfBytes = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, vkExtent3D.width, vkExtent3D.height) * vkExtent3D.depth;
		createAndAllocateVkBuffer(vulkanRenderer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, numberOfBytes, data, stagingVkBuffer, stagingVkDeviceMemory);

		// Create and fill Vulkan image
		createAndAllocateVkImage(vulkanRenderer, vkImageType, vkExtent3D, vkFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkImage, vkDeviceMemory);
		transitionVkImageLayout(vulkanRenderer, vkImage, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			copyVkBufferToVkImage(vulkanRenderer, stagingVkBuffer, vkImage, vkExtent3D);
		transitionVkImageLayout(vulkanRenderer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		// Destroy Vulkan staging buffer
		destroyAndFreeVkBuffer(vulkanRenderer, stagingVkBuffer, stagingVkDeviceMemory);

		// Create the Vulkan image view
		createVkImageView(vulkanRenderer, vkImage, vkImageViewType, vkFormat, vkImageView);
	}

	void Helper::createAndAllocateVkImage(const VulkanRenderer& vulkanRenderer, VkImageType vkImageType, const VkExtent3D& vkExtent3D, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory)
	{
		const VulkanContext& vulkanContext = vulkanRenderer.getVulkanContext();
		const VkDevice vkDevice = vulkanContext.getVkDevice();

		{ // Create Vulkan image
			const VkImageCreateInfo vkImageCreateInfo =
			{
				VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,	// sType (VkStructureType)
				nullptr,								// pNext (const void*)
				0,										// flags (VkImageCreateFlags)
				vkImageType,							// imageType (VkImageType)
				vkFormat,								// format (VkFormat)
				vkExtent3D,								// extent (VkExtent3D)
				1,										// mipLevels (uint32_t)
				1,										// arrayLayers (uint32_t)
				VK_SAMPLE_COUNT_1_BIT,					// samples (VkSampleCountFlagBits)
				vkImageTiling,							// tiling (VkImageTiling)
				vkImageUsageFlags,						// usage (VkImageUsageFlags)
				VK_SHARING_MODE_EXCLUSIVE,				// sharingMode (VkSharingMode)
				0,										// queueFamilyIndexCount (uint32_t)
				nullptr,								// pQueueFamilyIndices (const uint32_t*)
				VK_IMAGE_LAYOUT_PREINITIALIZED			// initialLayout (VkImageLayout)
			};
			if (vkCreateImage(vkDevice, &vkImageCreateInfo, nullptr, &vkImage) != VK_SUCCESS)
			{
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan image")
			}
		}

		{ // Allocate Vulkan memory
			VkMemoryRequirements vkMemoryRequirements = {};
			vkGetImageMemoryRequirements(vkDevice, vkImage, &vkMemoryRequirements);
			const VkMemoryAllocateInfo vkMemoryAllocateInfo =
			{
				VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,															// sType (VkStructureType)
				nullptr,																						// pNext (const void*)
				vkMemoryRequirements.size,																		// allocationSize (VkDeviceSize)
				vulkanContext.findMemoryTypeIndex(vkMemoryRequirements.memoryTypeBits, vkMemoryPropertyFlags)	// memoryTypeIndex (uint32_t)
			};
			if (vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, nullptr, &vkDeviceMemory) != VK_SUCCESS)
			{
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to allocate the Vulkan memory")
			}
			if (vkBindImageMemory(vkDevice, vkImage, vkDeviceMemory, 0) != VK_SUCCESS)
			{
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to bind the Vulkan image memory")
			}
		}
	}

	void Helper::copyVkBufferToVkImage(const VulkanRenderer& vulkanRenderer, VkBuffer vkBuffer, VkImage vkImage, const VkExtent3D& vkExtent3D)
	{
		// Create and begin Vulkan command buffer
		VkCommandBuffer vkCommandBuffer = beginSingleTimeCommands(vulkanRenderer);

		// Copy Vulkan buffer to Vulkan image
		const VkBufferImageCopy vkBufferImageCopy =
		{
			0,	// bufferOffset (VkDeviceSize)
			0,	// bufferRowLength (uint32_t)
			0,	// bufferImageHeight (uint32_t)
			{ // imageSubresource (VkImageSubresourceLayers)
				VK_IMAGE_ASPECT_COLOR_BIT,	// aspectMask (VkImageAspectFlags)
				0,							// mipLevel (uint32_t)
				0,							// baseArrayLayer (uint32_t)
				1							// layerCount (uint32_t)
			},
			{ 0, 0, 0 },					// imageOffset (VkOffset3D)
			vkExtent3D						// imageExtent (VkExtent3D)
		};
		vkCmdCopyBufferToImage(vkCommandBuffer, vkBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufferImageCopy);

		// End and destroy Vulkan command buffer
		endSingleTimeCommands(vulkanRenderer, vkCommandBuffer);
	}

	void Helper::destroyAndFreeVkImage(const VulkanRenderer& vulkanRenderer, VkImage vkImage, VkDeviceMemory vkDeviceMemory)
	{
		if (VK_NULL_HANDLE != vkImage)
		{
			const VkDevice vkDevice = vulkanRenderer.getVulkanContext().getVkDevice();
			vkDestroyImage(vkDevice, vkImage, nullptr);
			if (VK_NULL_HANDLE != vkDeviceMemory)
			{
				vkFreeMemory(vkDevice, vkDeviceMemory, nullptr);
			}
		}
	}

	void Helper::destroyAndFreeVkImage(const VulkanRenderer& vulkanRenderer, VkImage vkImage, VkDeviceMemory vkDeviceMemory, VkImageView vkImageView)
	{
		if (VK_NULL_HANDLE != vkImageView)
		{
			vkDestroyImageView(vulkanRenderer.getVulkanContext().getVkDevice(), vkImageView, nullptr);
		}
		destroyAndFreeVkImage(vulkanRenderer, vkImage, vkDeviceMemory);
	}

	void Helper::createVkImageView(const VulkanRenderer& vulkanRenderer, VkImage vkImage, VkImageViewType vkImageViewType, VkFormat vkFormat, VkImageView& vkImageView)
	{
		const VkImageViewCreateInfo vkImageViewCreateInfo =
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,	// sType (VkStructureType)
			nullptr,									// pNext (const void*)
			0,											// flags (VkImageViewCreateFlags)
			vkImage,									// image (VkImage)
			vkImageViewType,							// viewType (VkImageViewType)
			vkFormat,									// format (VkFormat)
			{ // components (VkComponentMapping)
				VK_COMPONENT_SWIZZLE_IDENTITY,			// r (VkComponentSwizzle)
				VK_COMPONENT_SWIZZLE_IDENTITY,			// g (VkComponentSwizzle)
				VK_COMPONENT_SWIZZLE_IDENTITY,			// b (VkComponentSwizzle)
				VK_COMPONENT_SWIZZLE_IDENTITY			// a (VkComponentSwizzle)
			},
			{ // subresourceRange (VkImageSubresourceRange)
				VK_IMAGE_ASPECT_COLOR_BIT,				// aspectMask (VkImageAspectFlags)
				0,										// baseMipLevel (uint32_t)
				1,										// levelCount (uint32_t)
				0,										// baseArrayLayer (uint32_t)
				1										// layerCount (uint32_t)
			}
		};
		if (vkCreateImageView(vulkanRenderer.getVulkanContext().getVkDevice(), &vkImageViewCreateInfo, nullptr, &vkImageView) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create Vulkan image view")
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
