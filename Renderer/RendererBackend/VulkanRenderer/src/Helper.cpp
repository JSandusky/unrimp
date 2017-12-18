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
#include <Renderer/IAssert.h>
#include <Renderer/Texture/ITexture.h>

#include <cstring>	// For "memcpy()"
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	#include <vector>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		bool hasVkFormatStencilComponent(VkFormat vkFormat)
		{
			return (VK_FORMAT_D32_SFLOAT_S8_UINT == vkFormat || VK_FORMAT_D24_UNORM_S8_UINT == vkFormat);
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	}
}


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

	void Helper::transitionVkImageLayout(const VulkanRenderer& vulkanRenderer, VkImage vkImage, VkImageAspectFlags vkImageAspectFlags, VkImageLayout oldVkImageLayout, VkImageLayout newVkImageLayout)
	{
		// Create and begin Vulkan command buffer
		VkCommandBuffer vkCommandBuffer = beginSingleTimeCommands(vulkanRenderer);

		// Vulkan image memory barrier
		transitionVkImageLayout(vulkanRenderer, vkCommandBuffer, vkImage, vkImageAspectFlags, 1, 1, oldVkImageLayout, newVkImageLayout);

		// End and destroy Vulkan command buffer
		endSingleTimeCommands(vulkanRenderer, vkCommandBuffer);
	}

	void Helper::transitionVkImageLayout(const VulkanRenderer& vulkanRenderer, VkCommandBuffer vkCommandBuffer, VkImage vkImage, VkImageAspectFlags vkImageAspectFlags, uint32_t levelCount, uint32_t layerCount, VkImageLayout oldVkImageLayout, VkImageLayout newVkImageLayout)
	{
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
				vkImageAspectFlags,	// aspectMask (VkImageAspectFlags)
				0,					// baseMipLevel (uint32_t)
				levelCount,			// levelCount (uint32_t)
				0,					// baseArrayLayer (uint32_t)
				layerCount			// layerCount (uint32_t)
			}
		};

		// "srcAccessMask" and "dstAccessMask" configuration
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
		else if (VK_IMAGE_LAYOUT_UNDEFINED == oldVkImageLayout && VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL == newVkImageLayout)
		{
			vkImageMemoryBarrier.srcAccessMask = 0;
			vkImageMemoryBarrier.dstAccessMask = (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
		}
		else
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Unsupported Vulkan image layout transition")
		}

		// Create Vulkan pipeline barrier command
		vkCmdPipelineBarrier(vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vkImageMemoryBarrier);
	}

	void Helper::transitionVkImageLayout(const VulkanRenderer& vulkanRenderer, VkCommandBuffer vkCommandBuffer, VkImage vkImage, VkImageLayout oldVkImageLayout, VkImageLayout newVkImageLayout, VkImageSubresourceRange vkImageSubresourceRange, VkPipelineStageFlags sourceVkPipelineStageFlags, VkPipelineStageFlags destinationVkPipelineStageFlags)
	{
		// Basing on https://github.com/SaschaWillems/Vulkan/tree/master

		// Create an image barrier object
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
			vkImageSubresourceRange					// subresourceRange (VkImageSubresourceRange)
		};

		// Source layouts (old)
		// -> Source access mask controls actions that have to be finished on the old layout before it will be transitioned to the new layout
		switch (oldVkImageLayout)
		{
			case VK_IMAGE_LAYOUT_UNDEFINED:
				// Image layout is undefined (or does not matter)
				// Only valid as initial layout
				// No flags required, listed only for completeness
				vkImageMemoryBarrier.srcAccessMask = 0;
				break;

			case VK_IMAGE_LAYOUT_PREINITIALIZED:
				// Image is preinitialized
				// Only valid as initial layout for linear images, preserves memory contents
				// Make sure host writes have been finished
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				// Image is a color attachment
				// Make sure any writes to the color buffer have been finished
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				// Image is a depth/stencil attachment
				// Make sure any writes to the depth/stencil buffer have been finished
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				// Image is a transfer source 
				// Make sure any reads from the image have been finished
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				// Image is a transfer destination
				// Make sure any writes to the image have been finished
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				// Image is read by a shader
				// Make sure any shader reads from the image have been finished
				vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
			case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR:
			case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR:
			// case VK_IMAGE_LAYOUT_BEGIN_RANGE:	not possible
			// case VK_IMAGE_LAYOUT_END_RANGE:		not possible
			case VK_IMAGE_LAYOUT_RANGE_SIZE:
			case VK_IMAGE_LAYOUT_MAX_ENUM:
			default:
				// Other source layouts aren't handled (yet)
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Unsupported Vulkan image old layout transition")
				break;
		}

		// Target layouts (new)
		// -> Destination access mask controls the dependency for the new image layout
		switch (newVkImageLayout)
		{
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				// Image will be used as a transfer destination
				// Make sure any writes to the image have been finished
				vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				// Image will be used as a transfer source
				// Make sure any reads from the image have been finished
				vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				// Image will be used as a color attachment
				// Make sure any writes to the color buffer have been finished
				vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				// Image layout will be used as a depth/stencil attachment
				// Make sure any writes to depth/stencil buffer have been finished
				vkImageMemoryBarrier.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				// Image will be read in a shader (sampler, input attachment)
				// Make sure any writes to the image have been finished
				if (vkImageMemoryBarrier.srcAccessMask == 0)
				{
					vkImageMemoryBarrier.srcAccessMask = (VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT);
				}
				vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_UNDEFINED:
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
			case VK_IMAGE_LAYOUT_PREINITIALIZED:
			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
			case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR:
			case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR:
			// case VK_IMAGE_LAYOUT_BEGIN_RANGE:	not possible
			// case VK_IMAGE_LAYOUT_END_RANGE:		not possible
			case VK_IMAGE_LAYOUT_RANGE_SIZE:
			case VK_IMAGE_LAYOUT_MAX_ENUM:
			default:
				// Other source layouts aren't handled (yet)
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Unsupported Vulkan image new layout transition")
				break;
		}

		// Put barrier inside setup command buffer
		vkCmdPipelineBarrier(vkCommandBuffer, sourceVkPipelineStageFlags, destinationVkPipelineStageFlags, 0, 0, nullptr, 0, nullptr, 1, &vkImageMemoryBarrier);
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
		if (vkCreateBuffer(vkDevice, &vkBufferCreateInfo, vulkanRenderer.getVkAllocationCallbacks(), &vkBuffer) != VK_SUCCESS)
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
		if (vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, vulkanRenderer.getVkAllocationCallbacks(), &vkDeviceMemory) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to allocate the Vulkan buffer memory")
		}

		// Bind and fill memory
		vkBindBufferMemory(vkDevice, vkBuffer, vkDeviceMemory, 0);
		if (nullptr != data)
		{
			void* mappedData = nullptr;
			if (vkMapMemory(vkDevice, vkDeviceMemory, 0, vkBufferCreateInfo.size, 0, &mappedData) == VK_SUCCESS)
			{
				memcpy(mappedData, data, static_cast<size_t>(vkBufferCreateInfo.size));
				vkUnmapMemory(vkDevice, vkDeviceMemory);
			}
			else
			{
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to map the Vulkan memory")
			}
		}
	}

	void Helper::destroyAndFreeVkBuffer(const VulkanRenderer& vulkanRenderer, VkBuffer& vkBuffer, VkDeviceMemory& vkDeviceMemory)
	{
		if (VK_NULL_HANDLE != vkBuffer)
		{
			const VkDevice vkDevice = vulkanRenderer.getVulkanContext().getVkDevice();
			vkDestroyBuffer(vkDevice, vkBuffer, vulkanRenderer.getVkAllocationCallbacks());
			if (VK_NULL_HANDLE != vkDeviceMemory)
			{
				vkFreeMemory(vkDevice, vkDeviceMemory, vulkanRenderer.getVkAllocationCallbacks());
			}
		}
	}

	VkFormat Helper::createAndFillVkImage(const VulkanRenderer& vulkanRenderer, VkImageType vkImageType, VkImageViewType vkImageViewType, const VkExtent3D& vkExtent3D, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, uint8_t numberOfMultisamples, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory, VkImageView& vkImageView)
	{
		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? Renderer::ITexture::getNumberOfMipmaps(vkExtent3D.width, vkExtent3D.height) : 1;

		// Get Vulkan image usage flags
		RENDERER_ASSERT(vulkanRenderer.getContext(), (flags & Renderer::TextureFlag::RENDER_TARGET) == 0 || nullptr == data, "Vulkan render target textures can't be filled using provided data")
		const bool isDepthTextureFormat = Renderer::TextureFormat::isDepth(textureFormat);
		VkImageUsageFlags vkImageUsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		if (flags & Renderer::TextureFlag::RENDER_TARGET)
		{
			if (isDepthTextureFormat)
			{
				vkImageUsageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}
			else
			{
				vkImageUsageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}
		}
		if (generateMipmaps)
		{
			vkImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		// Get Vulkan format
		const VkFormat vkFormat   = Mapping::getVulkanFormat(textureFormat);
		const bool     layered    = (VK_IMAGE_VIEW_TYPE_2D_ARRAY == vkImageViewType || VK_IMAGE_VIEW_TYPE_CUBE == vkImageViewType);
		const uint32_t layerCount = layered ? vkExtent3D.depth : 1;
		const uint32_t depth	  = layered ? 1 : vkExtent3D.depth;
		const VkSampleCountFlagBits vkSampleCountFlagBits = Mapping::getVulkanSampleCountFlagBits(vulkanRenderer.getContext(), numberOfMultisamples);
		VkImageAspectFlags vkImageAspectFlags = isDepthTextureFormat ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		if (::detail::hasVkFormatStencilComponent(vkFormat))
		{
			vkImageAspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		// Calculate the number of bytes
		uint32_t numberOfBytes = 0;
		if (dataContainsMipmaps)
		{
			uint32_t currentWidth  = vkExtent3D.width;
			uint32_t currentHeight = vkExtent3D.height;
			uint32_t currentDepth  = depth;
			for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
			{
				numberOfBytes += Renderer::TextureFormat::getNumberOfBytesPerSlice(static_cast<Renderer::TextureFormat::Enum>(textureFormat), currentWidth, currentHeight) * currentDepth;
				currentWidth  = std::max(currentWidth  >> 1, 1u);	// /= 2
				currentHeight = std::max(currentHeight >> 1, 1u);	// /= 2
				currentDepth  = std::max(currentDepth  >> 1, 1u);	// /= 2
			}
			numberOfBytes *= vkExtent3D.depth;
		}
		else
		{
			numberOfBytes = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, vkExtent3D.width, vkExtent3D.height) * vkExtent3D.depth;
		}

		{ // Create and fill Vulkan image
			const VkImageCreateFlags vkImageCreateFlags = (VK_IMAGE_VIEW_TYPE_CUBE == vkImageViewType) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0u;
			createAndAllocateVkImage(vulkanRenderer, vkImageCreateFlags, vkImageType, VkExtent3D{vkExtent3D.width, vkExtent3D.height, depth}, numberOfMipmaps, layerCount, vkFormat, vkSampleCountFlagBits, VK_IMAGE_TILING_OPTIMAL, vkImageUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkImage, vkDeviceMemory);
		}

		// Create the Vulkan image view
		createVkImageView(vulkanRenderer, vkImage, vkImageViewType, numberOfMipmaps, layerCount, vkFormat, vkImageAspectFlags, vkImageView);

		// Upload all mipmaps
		if (nullptr != data)
		{
			// Create Vulkan staging buffer
			VkBuffer stagingVkBuffer = VK_NULL_HANDLE;
			VkDeviceMemory stagingVkDeviceMemory = VK_NULL_HANDLE;
			createAndAllocateVkBuffer(vulkanRenderer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, numberOfBytes, data, stagingVkBuffer, stagingVkDeviceMemory);

			{ // Upload all mipmaps
				// Create and begin Vulkan command buffer
				VkCommandBuffer vkCommandBuffer = beginSingleTimeCommands(vulkanRenderer);
				transitionVkImageLayout(vulkanRenderer, vkCommandBuffer, vkImage, vkImageAspectFlags, numberOfMipmaps, layerCount, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

				// Upload all mipmaps
				uint32_t bufferOffset  = 0;
				uint32_t currentWidth  = vkExtent3D.width;
				uint32_t currentHeight = vkExtent3D.height;
				uint32_t currentDepth  = depth;

				// Allocate list of VkBufferImageCopy and setup VkBufferImageCopy data for each mipmap level
				const uint32_t numberOfUploadedMipmaps = generateMipmaps ? 1 : numberOfMipmaps;
				std::vector<VkBufferImageCopy> vkBufferImageCopyList;
				vkBufferImageCopyList.reserve(numberOfUploadedMipmaps);
				for (uint32_t mipmap = 0; mipmap < numberOfUploadedMipmaps; ++mipmap)
				{
					vkBufferImageCopyList.push_back({
						bufferOffset,									// bufferOffset (VkDeviceSize)
						0,												// bufferRowLength (uint32_t)
						0,												// bufferImageHeight (uint32_t)
						{ // imageSubresource (VkImageSubresourceLayers)
							vkImageAspectFlags,							// aspectMask (VkImageAspectFlags)
							mipmap,										// mipLevel (uint32_t)
							0,											// baseArrayLayer (uint32_t)
							layerCount									// layerCount (uint32_t)
						},
						{ 0, 0, 0 },									// imageOffset (VkOffset3D)
						{ currentWidth, currentHeight, currentDepth }	// imageExtent (VkExtent3D)
					});

					// Move on to the next mipmap
					bufferOffset += Renderer::TextureFormat::getNumberOfBytesPerSlice(static_cast<Renderer::TextureFormat::Enum>(textureFormat), currentWidth, currentHeight) * currentDepth;
					currentWidth  = std::max(currentWidth >> 1, 1u);	// /= 2
					currentHeight = std::max(currentHeight >> 1, 1u);	// /= 2
					currentDepth  = std::max(currentDepth >> 1, 1u);	// /= 2
				}

				// Copy Vulkan buffer to Vulkan image
				vkCmdCopyBufferToImage(vkCommandBuffer, stagingVkBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(vkBufferImageCopyList.size()), vkBufferImageCopyList.data());

				// End and destroy Vulkan command buffer
				transitionVkImageLayout(vulkanRenderer, vkCommandBuffer, vkImage, vkImageAspectFlags, numberOfMipmaps, layerCount, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				endSingleTimeCommands(vulkanRenderer, vkCommandBuffer);
			}

			// Destroy Vulkan staging buffer
			destroyAndFreeVkBuffer(vulkanRenderer, stagingVkBuffer, stagingVkDeviceMemory);

			// Generate a complete texture mip-chain at runtime from a base image using image blits and proper image barriers
			// -> Basing on https://github.com/SaschaWillems/Vulkan/tree/master/texturemipmapgen
			// -> We copy down the whole mip chain doing a blit from mip-1 to mip. An alternative way would be to always blit from the first mip level and sample that one down.
			// TODO(co) Some GPUs also offer "asynchronous transfer queues" (check for queue families with only the "VK_QUEUE_TRANSFER_BIT" set) that may be used to speed up such operations
			if (generateMipmaps)
			{
				#ifdef RENDERER_DEBUG
				{
					// Get device properties for the requested Vulkan texture format
					VkFormatProperties vkFormatProperties;
					vkGetPhysicalDeviceFormatProperties(vulkanRenderer.getVulkanContext().getVkPhysicalDevice(), vkFormat, &vkFormatProperties);

					// Mip-chain generation requires support for blit source and destination
					RENDERER_ASSERT(vulkanRenderer.getContext(), vkFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT, "Invalid Vulkan optimal tiling features")
					RENDERER_ASSERT(vulkanRenderer.getContext(), vkFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT, "Invalid Vulkan optimal tiling features")
				}
				#endif

				// Create and begin Vulkan command buffer
				VkCommandBuffer vkCommandBuffer = beginSingleTimeCommands(vulkanRenderer);

				// Copy down mips from n-1 to n
				for (uint32_t i = 1; i < numberOfMipmaps; ++i)
				{
					const VkImageBlit VkImageBlit =
					{
						{ // srcSubresource (VkImageSubresourceLayers)
							vkImageAspectFlags,	// aspectMask (VkImageAspectFlags)
							i - 1,				// mipLevel (uint32_t)
							0,					// baseArrayLayer (uint32_t)
							layerCount			// layerCount (uint32_t)
						},
						{ // srcOffsets[2] (VkOffset3D)
							{ 0, 0, 0 },
							{ std::max(int32_t(vkExtent3D.width >> (i - 1)), 1), std::max(int32_t(vkExtent3D.height >> (i - 1)), 1), 1 }
						},
						{ // dstSubresource (VkImageSubresourceLayers)
							vkImageAspectFlags,	// aspectMask (VkImageAspectFlags)
							i,					// mipLevel (uint32_t)
							0,					// baseArrayLayer (uint32_t)
							layerCount			// layerCount (uint32_t)
						},
						{ // dstOffsets[2] (VkOffset3D)
							{ 0, 0, 0 },
							{ std::max(int32_t(vkExtent3D.width >> i), 1), std::max(int32_t(vkExtent3D.height >> i), 1), 1 }
						}
					};
					const VkImageSubresourceRange vkImageSubresourceRange =
					{
						vkImageAspectFlags,	// aspectMask (VkImageAspectFlags)
						i,					// baseMipLevel (uint32_t)
						1,					// levelCount (uint32_t)
						0,					// baseArrayLayer (uint32_t)
						layerCount			// layerCount (uint32_t)
					};

					// Transition current mip level to transfer destination
					transitionVkImageLayout(vulkanRenderer, vkCommandBuffer, vkImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vkImageSubresourceRange, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT);

					// Blit from previous level
					vkCmdBlitImage(vkCommandBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &VkImageBlit, VK_FILTER_LINEAR);

					// Transition current mip level to transfer source for read in next iteration
					transitionVkImageLayout(vulkanRenderer, vkCommandBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vkImageSubresourceRange, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
				}

				{ // After the loop, all mip layers are in "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL"-layout, so transition all to "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL"-layout
					const VkImageSubresourceRange vkImageSubresourceRange =
					{
						vkImageAspectFlags,		// aspectMask (VkImageAspectFlags)
						1,						// baseMipLevel (uint32_t)
						numberOfMipmaps - 1,	// levelCount (uint32_t)
						0,						// baseArrayLayer (uint32_t)
						layerCount				// layerCount (uint32_t)
					};
					transitionVkImageLayout(vulkanRenderer, vkCommandBuffer, vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, vkImageSubresourceRange, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
				}

				// End and destroy Vulkan command buffer
				endSingleTimeCommands(vulkanRenderer, vkCommandBuffer);
			}
		}

		// Done
		return vkFormat;
	}

	void Helper::createAndAllocateVkImage(const VulkanRenderer& vulkanRenderer, VkImageCreateFlags vkImageCreateFlags, VkImageType vkImageType, const VkExtent3D& vkExtent3D, uint32_t mipLevels, uint32_t arrayLayers, VkFormat vkFormat, VkSampleCountFlagBits vkSampleCountFlagBits, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory)
	{
		const VulkanContext& vulkanContext = vulkanRenderer.getVulkanContext();
		const VkDevice vkDevice = vulkanContext.getVkDevice();

		{ // Create Vulkan image
			const VkImageCreateInfo vkImageCreateInfo =
			{
				VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,	// sType (VkStructureType)
				nullptr,								// pNext (const void*)
				vkImageCreateFlags,						// flags (VkImageCreateFlags)
				vkImageType,							// imageType (VkImageType)
				vkFormat,								// format (VkFormat)
				vkExtent3D,								// extent (VkExtent3D)
				mipLevels,								// mipLevels (uint32_t)
				arrayLayers,							// arrayLayers (uint32_t)
				vkSampleCountFlagBits,					// samples (VkSampleCountFlagBits)
				vkImageTiling,							// tiling (VkImageTiling)
				vkImageUsageFlags,						// usage (VkImageUsageFlags)
				VK_SHARING_MODE_EXCLUSIVE,				// sharingMode (VkSharingMode)
				0,										// queueFamilyIndexCount (uint32_t)
				nullptr,								// pQueueFamilyIndices (const uint32_t*)
				VK_IMAGE_LAYOUT_PREINITIALIZED			// initialLayout (VkImageLayout)
			};
			if (vkCreateImage(vkDevice, &vkImageCreateInfo, vulkanRenderer.getVkAllocationCallbacks(), &vkImage) != VK_SUCCESS)
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
			if (vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, vulkanRenderer.getVkAllocationCallbacks(), &vkDeviceMemory) != VK_SUCCESS)
			{
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to allocate the Vulkan memory")
			}
			if (vkBindImageMemory(vkDevice, vkImage, vkDeviceMemory, 0) != VK_SUCCESS)
			{
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to bind the Vulkan image memory")
			}
		}
	}

	void Helper::destroyAndFreeVkImage(const VulkanRenderer& vulkanRenderer, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory)
	{
		if (VK_NULL_HANDLE != vkImage)
		{
			const VkDevice vkDevice = vulkanRenderer.getVulkanContext().getVkDevice();
			vkDestroyImage(vkDevice, vkImage, vulkanRenderer.getVkAllocationCallbacks());
			vkImage = VK_NULL_HANDLE;
			if (VK_NULL_HANDLE != vkDeviceMemory)
			{
				vkFreeMemory(vkDevice, vkDeviceMemory, vulkanRenderer.getVkAllocationCallbacks());
				vkDeviceMemory = VK_NULL_HANDLE;
			}
		}
	}

	void Helper::destroyAndFreeVkImage(const VulkanRenderer& vulkanRenderer, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory, VkImageView& vkImageView)
	{
		if (VK_NULL_HANDLE != vkImageView)
		{
			vkDestroyImageView(vulkanRenderer.getVulkanContext().getVkDevice(), vkImageView, vulkanRenderer.getVkAllocationCallbacks());
			vkImageView = VK_NULL_HANDLE;
		}
		destroyAndFreeVkImage(vulkanRenderer, vkImage, vkDeviceMemory);
	}

	void Helper::createVkImageView(const VulkanRenderer& vulkanRenderer, VkImage vkImage, VkImageViewType vkImageViewType, uint32_t levelCount, uint32_t layerCount, VkFormat vkFormat, VkImageAspectFlags vkImageAspectFlags, VkImageView& vkImageView)
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
				vkImageAspectFlags,						// aspectMask (VkImageAspectFlags)
				0,										// baseMipLevel (uint32_t)
				levelCount,								// levelCount (uint32_t)
				0,										// baseArrayLayer (uint32_t)
				layerCount								// layerCount (uint32_t)
			}
		};
		if (vkCreateImageView(vulkanRenderer.getVulkanContext().getVkDevice(), &vkImageViewCreateInfo, vulkanRenderer.getVkAllocationCallbacks(), &vkImageView) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create Vulkan image view")
		}
	}

	#ifdef RENDERER_DEBUG
		void Helper::setDebugObjectName(VkDevice vkDevice, VkDebugReportObjectTypeEXT vkDebugReportObjectTypeEXT, uint64_t object, const char* objectName)
		{
			if (nullptr != vkDebugMarkerSetObjectNameEXT)
			{
				const VkDebugMarkerObjectNameInfoEXT vkDebugMarkerObjectNameInfoEXT =
				{
					VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT,	// sType (VkStructureType)
					nullptr,												// pNext (const void*)
					vkDebugReportObjectTypeEXT,								// objectType (VkDebugReportObjectTypeEXT)
					object,													// object (uint64_t)
					objectName												// pObjectName (const char*)
				};
				vkDebugMarkerSetObjectNameEXT(vkDevice, &vkDebugMarkerObjectNameInfoEXT);
			}
		}
	#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
