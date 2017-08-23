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

#include "VulkanRenderer/VulkanContext.h"


//[-------------------------------------------------------]
//[ Global helper macros                                  ]
//[-------------------------------------------------------]
#ifdef VULKANRENDERER_NO_DEBUG
	#define DECLARE_SET_DEBUG_NAME
	#define SET_DEFAULT_DEBUG_NAME
	#define DEFINE_SET_DEBUG_NAME_VKBUFFER_VKDEVICEMEMORY
	#define DEFINE_SET_DEBUG_NAME_TEXTURE
	#define DEFINE_SET_DEBUG_NAME_SHADER_MODULE
#else
	#define DECLARE_SET_DEBUG_NAME virtual void setDebugName(const char* name) override;
	#define SET_DEFAULT_DEBUG_NAME setDebugName("");	// Assign a default name to the resource for debugging purposes
	#define DEFINE_SET_DEBUG_NAME_VKBUFFER_VKDEVICEMEMORY(className, decoration, numberOfDecorationCharacters) \
		void className::setDebugName(const char* name) \
		{ \
			if (nullptr != vkDebugMarkerSetObjectNameEXT) \
			{ \
				RENDERER_DECORATED_DEBUG_NAME(name, detailedName, decoration, numberOfDecorationCharacters); \
				const VkDevice vkDevice = static_cast<const VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(); \
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, mVkBuffer, detailedName); \
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT, mVkDeviceMemory, detailedName); \
			} \
		}
	#define DEFINE_SET_DEBUG_NAME_TEXTURE(className) \
		void className::setDebugName(const char* name) \
		{ \
			if (nullptr != vkDebugMarkerSetObjectNameEXT) \
			{ \
				const VkDevice vkDevice = static_cast<const VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(); \
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, mVkImage, name); \
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT, mVkDeviceMemory, name); \
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT, mVkImageView, name); \
			} \
		}
	#define DEFINE_SET_DEBUG_NAME_SHADER_MODULE(className) \
		void className::setDebugName(const char* name) \
		{ \
			if (nullptr != vkDebugMarkerSetObjectNameEXT) \
			{ \
				Helper::setDebugObjectName(static_cast<const VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(), VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT, mVkShaderModule, name); \
			} \
		}
#endif


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
		static void transitionVkImageLayout(const VulkanRenderer& vulkanRenderer, VkCommandBuffer vkCommandBuffer, VkImage vkImage, VkFormat vkFormat, uint32_t levelCount, uint32_t layerCount, VkImageLayout oldVkImageLayout, VkImageLayout newVkImageLayout);

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
		static VkFormat createAndFillVkImage(const VulkanRenderer& vulkanRenderer, VkImageType vkImageType, VkImageViewType vkImageViewType, const VkExtent3D& vkExtent3D, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory, VkImageView& vkImageView);
		static void createAndAllocateVkImage(const VulkanRenderer& vulkanRenderer, VkImageCreateFlags vkImageCreateFlags, VkImageType vkImageType, const VkExtent3D& vkExtent3D, uint32_t mipLevels, uint32_t arrayLayers, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory);
		static void destroyAndFreeVkImage(const VulkanRenderer& vulkanRenderer, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory);
		static void destroyAndFreeVkImage(const VulkanRenderer& vulkanRenderer, VkImage& vkImage, VkDeviceMemory& vkDeviceMemory, VkImageView& vkImageView);
		static void createVkImageView(const VulkanRenderer& vulkanRenderer, VkImage vkImage, VkImageViewType vkImageViewType, uint32_t levelCount, uint32_t layerCount, VkFormat vkFormat, VkImageAspectFlags vkImageAspectFlags, VkImageView& vkImageView);

		//[-------------------------------------------------------]
		//[ Debug                                                 ]
		//[-------------------------------------------------------]
		#ifndef VULKANRENDERER_NO_DEBUG
			static void setDebugObjectName(VkDevice vkDevice, VkDebugReportObjectTypeEXT vkDebugReportObjectTypeEXT, uint64_t object, const char* objectName);
		#endif


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
