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
#include "VulkanRenderer/Buffer/IndirectBuffer.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/Helper.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>

#include <cstring>	// For "memcpy()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndirectBuffer::IndirectBuffer(VulkanRenderer& vulkanRenderer, uint32_t numberOfBytes, const void* data, Renderer::BufferUsage) :
		IIndirectBuffer(vulkanRenderer),
		mVkBuffer(VK_NULL_HANDLE),
		mVkDeviceMemory(VK_NULL_HANDLE)
	{
		Helper::createAndAllocateVkBuffer(vulkanRenderer, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, numberOfBytes, data, mVkBuffer, mVkDeviceMemory);
		SET_DEFAULT_DEBUG_NAME	// setDebugName("");
	}

	IndirectBuffer::~IndirectBuffer()
	{
		Helper::destroyAndFreeVkBuffer(static_cast<const VulkanRenderer&>(getRenderer()), mVkBuffer, mVkDeviceMemory);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IIndirectBuffer methods      ]
	//[-------------------------------------------------------]
	void IndirectBuffer::copyDataFrom(uint32_t numberOfBytes, const void* data)
	{
		// Sanity checks
		RENDERER_ASSERT(getRenderer().getContext(), 0 != numberOfBytes, "Invalid Vulkan indirect buffer data");
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != data, "Invalid Vulkan indirect buffer data");
		RENDERER_ASSERT(getRenderer().getContext(), VK_NULL_HANDLE != mVkDeviceMemory, "Invalid Vulkan device memory");

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
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	DEFINE_SET_DEBUG_NAME_VKBUFFER_VKDEVICEMEMORY(IndirectBuffer, "IndirectBufferObject", 23)	// void IndirectBuffer::setDebugName(const char* name)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
