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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/Texture/TextureTypes.h>

#include "VulkanRenderer/VulkanRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Vulkan helper
	*/
	class Helper
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Command                                               ]
		//[-------------------------------------------------------]
		static VkCommandBuffer beginSingleTimeCommands(const VulkanRenderer& vulkanRenderer);
		static void endSingleTimeCommands(const VulkanRenderer& vulkanRenderer, VkCommandBuffer vkCommandBuffer);

		//[-------------------------------------------------------]
		//[ Transition                                            ]
		//[-------------------------------------------------------]
		static void transitionVkImageLayout(const VulkanRenderer& vulkanRenderer, VkImage vkImage, VkFormat vkFormat, VkImageLayout oldVkImageLayout, VkImageLayout newVkImageLayout);
		static void transitionVkImageLayout(const VulkanRenderer& vulkanRenderer, VkCommandBuffer vkCommandBuffer, VkImage vkImage, VkFormat vkFormat, uint32_t levelCount, VkImageLayout oldVkImageLayout, VkImageLayout newVkImageLayout);

		//[-------------------------------------------------------]
		//[ Buffer                                                ]
		//[-------------------------------------------------------]
		// TODO(co) Trivial implementation to have something to start with. Need to use more clever memory management and stating buffers later on.
		static void createAndAllocateVkBuffer(const VulkanRenderer& vulkanRenderer, VkBufferUsageFlagBits vkBufferUsageFlagBits, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkDeviceSize numberOfBytes, const void* data, VkBuffer& vkBuffer, VkDeviceMemory& vkDeviceMemory);
		static void destroyAndFreeVkBuffer(const VulkanRenderer& vulkanRenderer, VkBuffer& vkBuffer, VkDeviceMemory& vkDeviceMemory);

		//[-------------------------------------------------------]
		//[ Image                                                 ]
		//[-------------------------------------------------------]
		// TODO(co) Trivial implementation to have something to start with. Need to use more clever memory management and stating buffers later on.
		static void createAndFillVkImage(const VulkanRenderer& vulkanRenderer, VkImageType vkImageType, VkImageViewType vkImageViewType, const VkExtent3D& vkExtent3D, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory, VkImageView& vkImageView);
		static void createAndAllocateVkImage(const VulkanRenderer& vulkanRenderer, VkImageType vkImageType, const VkExtent3D& vkExtent3D, uint32_t mipLevels, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory);
		static void destroyAndFreeVkImage(const VulkanRenderer& vulkanRenderer, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory);
		static void destroyAndFreeVkImage(const VulkanRenderer& vulkanRenderer, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory, VkImageView& vkImageView);
		static void createVkImageView(const VulkanRenderer& vulkanRenderer, VkImage vkImage, VkImageViewType vkImageViewType, uint32_t mipLevels, VkFormat vkFormat, VkImageAspectFlags vkImageAspectFlags, VkImageView& vkImageView);


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
