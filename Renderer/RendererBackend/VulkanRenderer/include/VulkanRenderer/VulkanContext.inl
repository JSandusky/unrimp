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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	bool VulkanContext::isInitialized() const
	{
		return (VK_NULL_HANDLE != mVkCommandBuffer);
	}

	inline VulkanRenderer& VulkanContext::getVulkanRenderer() const
	{
		return mVulkanRenderer;
	}

	inline VkPhysicalDevice VulkanContext::getVkPhysicalDevice() const
	{
		return mVkPhysicalDevice;
	}

	inline VkDevice VulkanContext::getVkDevice() const
	{
		return mVkDevice;
	}

	inline uint32_t VulkanContext::getGraphicsQueueFamilyIndex() const
	{
		return mGraphicsQueueFamilyIndex;
	}

	inline uint32_t VulkanContext::getPresentQueueFamilyIndex() const
	{
		return mPresentQueueFamilyIndex;
	}

	inline VkQueue VulkanContext::getGraphicsVkQueue() const
	{
		return mGraphicsVkQueue;
	}

	inline VkQueue VulkanContext::getPresentVkQueue() const
	{
		return mPresentVkQueue;
	}

	inline VkCommandPool VulkanContext::getVkCommandPool() const
	{
		return mVkCommandPool;
	}

	inline VkCommandBuffer VulkanContext::getVkCommandBuffer() const
	{
		return mVkCommandBuffer;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer