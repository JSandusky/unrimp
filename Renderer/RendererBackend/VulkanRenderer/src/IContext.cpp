/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "VulkanRenderer/IContext.h"
#include "VulkanRenderer/VulkanRenderer.h"

#include <array>
#include <vector>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		typedef std::vector<VkPhysicalDevice> VkPhysicalDevices;


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void enumeratePhysicalDevices(VkInstance vkInstance, VkPhysicalDevices& vkPhysicalDevices)
		{
			// Get the number of available physical devices
			uint32_t physicalDeviceCount = 0;
			VkResult vkResult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
			if (vkResult == VK_SUCCESS)
			{
				if (physicalDeviceCount > 0)
				{
					// Enumerate physical devices
					vkPhysicalDevices.resize(physicalDeviceCount);
					vkResult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, vkPhysicalDevices.data());
					if (vkResult != VK_SUCCESS)
					{
						// Error!
						RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: Failed to enumerate physical devices")
					}
				}
				else
				{
					// Error!
					RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: There are no physical devices")
				}
			}
			else
			{
				// Error!
				RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: Failed to get the number of physical devices")
			}
		}

		VkResult createVkDevice(VkPhysicalDevice vkPhysicalDevice, const VkDeviceQueueCreateInfo& vkDeviceQueueCreateInfo, bool enableValidation, VkDevice& vkDevice)
		{
			std::vector<const char*> enabledExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

			VkDeviceCreateInfo vkDeviceCreateInfo = {};
			vkDeviceCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			vkDeviceCreateInfo.queueCreateInfoCount = 1;
			vkDeviceCreateInfo.pQueueCreateInfos	= &vkDeviceQueueCreateInfo;
			if (!enabledExtensions.empty())
			{
				vkDeviceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(enabledExtensions.size());
				vkDeviceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
			}
			if (enableValidation)
			{
				vkDeviceCreateInfo.enabledLayerCount   = VulkanRenderer::VulkanRuntimeLinking::NUMBER_OF_VALIDATION_LAYERS;
				vkDeviceCreateInfo.ppEnabledLayerNames = VulkanRenderer::VulkanRuntimeLinking::VALIDATION_LAYER_NAMES;
			}

			return vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &vkDevice);
		}

		VkDevice createVkDevice(VkPhysicalDevice vkPhysicalDevice, bool enableValidation, VkQueue &graphicsVkQueue)
		{
			VkDevice vkDevice = VK_NULL_HANDLE;

			// Get physical device queue family properties
			uint32_t queueFamilyPropertyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyPropertyCount, nullptr);
			if (queueFamilyPropertyCount > 0)
			{
				std::vector<VkQueueFamilyProperties> vkQueueFamilyProperties;
				vkQueueFamilyProperties.resize(queueFamilyPropertyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyPropertyCount, vkQueueFamilyProperties.data());

				// Find a queue that supports graphics operations
				uint32_t graphicsQueueIndex = 0;
				for (; graphicsQueueIndex < queueFamilyPropertyCount; ++graphicsQueueIndex)
				{
					if (vkQueueFamilyProperties[graphicsQueueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					{
						// Create Vulkan device instance
						std::array<float, 1> queuePriorities = { 0.0f };
						VkDeviceQueueCreateInfo vkDeviceQueueCreateInfo = {};
						vkDeviceQueueCreateInfo.sType			 = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
						vkDeviceQueueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
						vkDeviceQueueCreateInfo.queueCount		 = 1;
						vkDeviceQueueCreateInfo.pQueuePriorities = queuePriorities.data();
						VkResult vkResult = createVkDevice(vkPhysicalDevice, vkDeviceQueueCreateInfo, enableValidation, vkDevice);
						if (vkResult == VK_ERROR_LAYER_NOT_PRESENT && enableValidation)
						{
							// Error! Since the show must go on, try creating a Vulkan device instance without validation enabled...
							RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: Failed to create the Vulkan device instance with validation enabled, layer is not present")
							vkResult = createVkDevice(vkPhysicalDevice, vkDeviceQueueCreateInfo, false, vkDevice);
						}
						// TODO(co) Error handling: Evaluate "vkResult"?

						// Get the to the Vulkan device graphics queue that command buffers are submitted to
						vkGetDeviceQueue(vkDevice, graphicsQueueIndex, 0, &graphicsVkQueue);

						// We're done, get us out of the loop
						graphicsQueueIndex = queueFamilyPropertyCount;
					}
				}
			}
			else
			{
				// Error!
				RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: Failed to get physical device queue family properties")
			}

			// Done
			return vkDevice;
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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IContext::~IContext()
	{
		// Destroy the Vulkan device instance
		if (VK_NULL_HANDLE != mVkDevice)
		{
			vkDestroyDevice(mVkDevice, nullptr);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	IContext::IContext(VulkanRenderer& vulkanRenderer) :
		mVkPhysicalDevice(VK_NULL_HANDLE),
		mVkDevice(VK_NULL_HANDLE),
		mGraphicsVkQueue(VK_NULL_HANDLE)
	{
		{ // Get the physical Vulkan device this context should use
			detail::VkPhysicalDevices vkPhysicalDevices;
			::detail::enumeratePhysicalDevices(vulkanRenderer.getVulkanRuntimeLinking().getVkInstance(), vkPhysicalDevices);
			if (!vkPhysicalDevices.empty())
			{
				// TODO(co) For now, we always use the first found physical Vulkan device
				mVkPhysicalDevice = vkPhysicalDevices[0];
			}
		}

		// Create the Vulkan device instance
		if (nullptr != mVkPhysicalDevice)
		{
			// TODO(co) For now, the Vulkan validation layer is always enabled by default
			mVkDevice = ::detail::createVkDevice(mVkPhysicalDevice, true, mGraphicsVkQueue);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
