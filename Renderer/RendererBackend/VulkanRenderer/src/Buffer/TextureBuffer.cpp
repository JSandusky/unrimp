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
#include "VulkanRenderer/Buffer/TextureBuffer.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/Mapping.h"
#include "VulkanRenderer/Helper.h"

#include <Renderer/ILog.h>

#include <cstring>	// For "memset()" and "memcpy()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureBuffer::TextureBuffer(VulkanRenderer& vulkanRenderer, uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void* data, Renderer::BufferUsage) :
		ITextureBuffer(vulkanRenderer),
		mVkBuffer(VK_NULL_HANDLE),
		mVkDeviceMemory(VK_NULL_HANDLE),
		mVkBufferView(VK_NULL_HANDLE)
	{
		Helper::createAndAllocateVkBuffer(vulkanRenderer, VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, numberOfBytes, data, mVkBuffer, mVkDeviceMemory);

		// Create Vulkan buffer view
		const VkBufferViewCreateInfo vkBufferViewCreateInfo =
		{
			VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,	// sType (VkStructureType)
			nullptr,									// pNext (const void*)
			0,											// flags (VkBufferViewCreateFlags)
			mVkBuffer,									// buffer (VkBuffer)
			Mapping::getVulkanFormat(textureFormat),	// format (VkFormat)
			0,											// offset (VkDeviceSize)
			VK_WHOLE_SIZE								// range (VkDeviceSize)
		};
		if (vkCreateBufferView(vulkanRenderer.getVulkanContext().getVkDevice(), &vkBufferViewCreateInfo, nullptr, &mVkBufferView) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan buffer view")
		}
	}

	TextureBuffer::~TextureBuffer()
	{
		const VulkanRenderer& vulkanRenderer = static_cast<const VulkanRenderer&>(getRenderer());
		if (VK_NULL_HANDLE != mVkBufferView)
		{
			vkDestroyBufferView(vulkanRenderer.getVulkanContext().getVkDevice(), mVkBufferView, nullptr);
		}
		Helper::destroyAndFreeVkBuffer(vulkanRenderer, mVkBuffer, mVkDeviceMemory);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureBuffer methods       ]
	//[-------------------------------------------------------]
	void TextureBuffer::copyDataFrom(uint32_t numberOfBytes, const void* data)
	{
		// Sanity checks
		assert(0 != numberOfBytes);
		assert(nullptr != data);
		assert(VK_NULL_HANDLE != mVkDeviceMemory);

		// Upload data
		const VulkanRenderer& vulkanRenderer = static_cast<const VulkanRenderer&>(getRenderer());
		const VkDevice vkDevice = vulkanRenderer.getVulkanContext().getVkDevice();
		void* mappedData = nullptr;
		if (vkMapMemory(vkDevice, mVkDeviceMemory, 0, numberOfBytes, 0, &mappedData) == VK_SUCCESS)
		{
			memcpy(mappedData, data, numberOfBytes);
			vkUnmapMemory(vkDevice, mVkDeviceMemory);
		}
		else
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to map the Vulkan memory")
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
