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
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/VulkanRenderer.h"

#include <Renderer/ILog.h>

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
		typedef std::vector<VkExtensionProperties> VkExtensionPropertiesVector;


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void enumeratePhysicalDevices(VulkanRenderer::VulkanRenderer& vulkanRenderer, VkInstance vkInstance, VkPhysicalDevices& vkPhysicalDevices)
		{
			// Get the number of available physical devices
			uint32_t physicalDeviceCount = 0;
			VkResult vkResult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
			if (VK_SUCCESS == vkResult)
			{
				if (physicalDeviceCount > 0)
				{
					// Enumerate physical devices
					vkPhysicalDevices.resize(physicalDeviceCount);
					vkResult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, vkPhysicalDevices.data());
					if (VK_SUCCESS != vkResult)
					{
						// Error!
						RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to enumerate physical Vulkan devices")
					}
				}
				else
				{
					// Error!
					RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "There are no physical Vulkan devices")
				}
			}
			else
			{
				// Error!
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to get the number of physical Vulkan devices")
			}
		}

		bool isExtensionAvailable(const char* extensionName, const VkExtensionPropertiesVector& vkExtensionPropertiesVector)
		{
			for (const VkExtensionProperties& vkExtensionProperties : vkExtensionPropertiesVector)
			{
				if (strcmp(vkExtensionProperties.extensionName, extensionName) == 0)
				{
					// The extension is available
					return true;
				}
			}

			// The extension isn't available
			return false;
		}

		VkPhysicalDevice selectPhysicalDevice(VulkanRenderer::VulkanRenderer& vulkanRenderer, const VkPhysicalDevices& vkPhysicalDevices)
		{
			// TODO(co) I'am sure this selection can be improved (rating etc.)
			for (const VkPhysicalDevice& vkPhysicalDevice : vkPhysicalDevices)
			{
				{ // Reject physical Vulkan devices basing on swap chain support
					// Get number of device extensions
					uint32_t propertyCount = 0;
					if ((vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &propertyCount, nullptr) != VK_SUCCESS) || (0 == propertyCount))
					{
						// Reject physical Vulkan device
						continue;
					}

					// Get of device extensions
					VkExtensionPropertiesVector vkExtensionPropertiesVector(propertyCount);
					if (vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &propertyCount, vkExtensionPropertiesVector.data()) != VK_SUCCESS)
					{
						// Reject physical Vulkan device
						continue;
					}

					// Check device extensions
					const std::array<const char*, 1> deviceExtensions =
					{
						VK_KHR_SWAPCHAIN_EXTENSION_NAME
					};
					bool rejectDevice = false;
					for (const char* deviceExtension : deviceExtensions)
					{
						if (!isExtensionAvailable(deviceExtension, vkExtensionPropertiesVector))
						{
							rejectDevice = true;
							break;
						}
					}
					if (rejectDevice)
					{
						// Reject physical Vulkan device
						continue;
					}
				}

				{ // Reject physical Vulkan devices basing on supported API version and some basic limits
					VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
					vkGetPhysicalDeviceProperties(vkPhysicalDevice, &vkPhysicalDeviceProperties);
					const uint32_t majorVersion = VK_VERSION_MAJOR(vkPhysicalDeviceProperties.apiVersion);
					if ((majorVersion < 1) || (vkPhysicalDeviceProperties.limits.maxImageDimension2D < 4096))
					{
						// Reject physical Vulkan device
						continue;
					}
				}

				// Reject physical Vulkan devices basing on supported queue family
				uint32_t queueFamilyPropertyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyPropertyCount, nullptr);
				if (0 == queueFamilyPropertyCount)
				{
					// Reject physical Vulkan device
					continue;
				}
				std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());
				for (uint32_t i = 0; i < queueFamilyPropertyCount; ++i)
				{
					if ((queueFamilyProperties[i].queueCount > 0) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
					{
						// Select physical Vulkan device
						return vkPhysicalDevice;
					}
				}
			}

			// Error!
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to select a physical Vulkan device")
			return VK_NULL_HANDLE;
		}

		VkResult createVkDevice(VkPhysicalDevice vkPhysicalDevice, const VkDeviceQueueCreateInfo& vkDeviceQueueCreateInfo, bool enableValidation, VkDevice& vkDevice)
		{
			const std::array<const char*, 1> enabledExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			const VkDeviceCreateInfo vkDeviceCreateInfo =
			{
				VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,														// sType (VkStructureType)
				nullptr,																					// pNext (const void*)
				0,																							// flags (VkDeviceCreateFlags)
				1,																							// queueCreateInfoCount (uint32_t)
				&vkDeviceQueueCreateInfo,																	// pQueueCreateInfos (const VkDeviceQueueCreateInfo*)
				enableValidation ? VulkanRenderer::VulkanRuntimeLinking::NUMBER_OF_VALIDATION_LAYERS : 0,	// enabledLayerCount (uint32_t)
				enableValidation ? VulkanRenderer::VulkanRuntimeLinking::VALIDATION_LAYER_NAMES : nullptr,	// ppEnabledLayerNames (const char* const*)
				enabledExtensions.empty() ? 0 : static_cast<uint32_t>(enabledExtensions.size()),			// enabledExtensionCount (uint32_t)
				enabledExtensions.empty() ? nullptr : enabledExtensions.data(),								// ppEnabledExtensionNames (const char* const*)
				nullptr																						// pEnabledFeatures (const VkPhysicalDeviceFeatures*)
			};
			return vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &vkDevice);
		}

		VkDevice createVkDevice(VulkanRenderer::VulkanRenderer& vulkanRenderer, VkPhysicalDevice vkPhysicalDevice, bool enableValidation, uint32_t& graphicsQueueFamilyIndex, uint32_t& presentQueueFamilyIndex)
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
						// Create logical Vulkan device instance
						const std::array<float, 1> queuePriorities = { 0.0f };
						const VkDeviceQueueCreateInfo vkDeviceQueueCreateInfo =
						{
							VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,	// sType (VkStructureType)
							nullptr,									// pNext (const void*)
							0,											// flags (VkDeviceQueueCreateFlags)
							graphicsQueueIndex,							// queueFamilyIndex (uint32_t)
							1,											// queueCount (uint32_t)
							queuePriorities.data()						// pQueuePriorities (const float*)
						};
						VkResult vkResult = createVkDevice(vkPhysicalDevice, vkDeviceQueueCreateInfo, enableValidation, vkDevice);
						if (VK_ERROR_LAYER_NOT_PRESENT == vkResult && enableValidation)
						{
							// Error! Since the show must go on, try creating a Vulkan device instance without validation enabled...
							RENDERER_LOG(vulkanRenderer.getContext(), WARNING, "Failed to create the Vulkan device instance with validation enabled, layer is not present")
							vkResult = createVkDevice(vkPhysicalDevice, vkDeviceQueueCreateInfo, false, vkDevice);
						}
						// TODO(co) Error handling: Evaluate "vkResult"?
						graphicsQueueFamilyIndex = graphicsQueueIndex;
						presentQueueFamilyIndex = graphicsQueueIndex;	// TODO(co) Handle the case of the graphics queue doesn't support present

						// We're done, get us out of the loop
						graphicsQueueIndex = queueFamilyPropertyCount;
					}
				}
			}
			else
			{
				// Error!
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to get physical Vulkan device queue family properties")
			}

			// Done
			return vkDevice;
		}

		VkCommandPool createVkCommandPool(VulkanRenderer::VulkanRenderer& vulkanRenderer, VkDevice vkDevice, uint32_t graphicsQueueFamilyIndex)
		{
			VkCommandPool vkCommandPool = VK_NULL_HANDLE;

			// Create Vulkan command pool instance
			const VkCommandPoolCreateInfo vkCommandPoolCreateInfo =
			{
				VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,			// sType (VkStructureType)
				nullptr,											// pNext (const void*)
				VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,	// flags (VkCommandPoolCreateFlags)
				graphicsQueueFamilyIndex,							/// queueFamilyIndex (uint32_t)
			};
			const VkResult vkResult = vkCreateCommandPool(vkDevice, &vkCommandPoolCreateInfo, nullptr, &vkCommandPool);
			if (VK_SUCCESS != vkResult)
			{
				// Error!
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create Vulkan command pool instance")
			}

			// Done
			return vkCommandPool;
		}

		VkCommandBuffer createVkCommandBuffer(VulkanRenderer::VulkanRenderer& vulkanRenderer, VkDevice vkDevice, VkCommandPool vkCommandPool)
		{
			VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;

			// Create Vulkan command buffer instance
			const VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo =
			{
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,	// sType (VkStructureType)
				nullptr,										// pNext (const void*)
				vkCommandPool,									// commandPool (VkCommandPool)
				VK_COMMAND_BUFFER_LEVEL_PRIMARY,				// level (VkCommandBufferLevel)
				1												// commandBufferCount (uint32_t)
			};
			VkResult vkResult = vkAllocateCommandBuffers(vkDevice, &vkCommandBufferAllocateInfo, &vkCommandBuffer);
			if (VK_SUCCESS != vkResult)
			{
				// Error!
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create Vulkan command buffer instance")
			}

			// Done
			return vkCommandBuffer;
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
	VulkanContext::VulkanContext(VulkanRenderer& vulkanRenderer) :
		mVulkanRenderer(vulkanRenderer),
		mVkPhysicalDevice(VK_NULL_HANDLE),
		mVkDevice(VK_NULL_HANDLE),
		mGraphicsQueueFamilyIndex(~0u),
		mPresentQueueFamilyIndex(~0u),
		mGraphicsVkQueue(VK_NULL_HANDLE),
		mPresentVkQueue(VK_NULL_HANDLE),
		mVkCommandPool(VK_NULL_HANDLE),
		mVkCommandBuffer(VK_NULL_HANDLE)
	{
		const VulkanRuntimeLinking& vulkanRuntimeLinking = mVulkanRenderer.getVulkanRuntimeLinking();

		{ // Get the physical Vulkan device this context should use
			detail::VkPhysicalDevices vkPhysicalDevices;
			::detail::enumeratePhysicalDevices(mVulkanRenderer, vulkanRuntimeLinking.getVkInstance(), vkPhysicalDevices);
			if (!vkPhysicalDevices.empty())
			{
				mVkPhysicalDevice = ::detail::selectPhysicalDevice(mVulkanRenderer, vkPhysicalDevices);
			}
		}

		// Create the logical Vulkan device instance
		if (VK_NULL_HANDLE != mVkPhysicalDevice)
		{
			mVkDevice = ::detail::createVkDevice(mVulkanRenderer, mVkPhysicalDevice, vulkanRuntimeLinking.isValidationEnabled(), mGraphicsQueueFamilyIndex, mPresentQueueFamilyIndex);
			if (VK_NULL_HANDLE != mVkDevice)
			{
				// Get the Vulkan device graphics queue that command buffers are submitted to
				vkGetDeviceQueue(mVkDevice, mGraphicsQueueFamilyIndex, 0, &mGraphicsVkQueue);
				if (VK_NULL_HANDLE != mGraphicsVkQueue)
				{
					// Get the Vulkan device present queue
					vkGetDeviceQueue(mVkDevice, mPresentQueueFamilyIndex, 0, &mPresentVkQueue);
					if (VK_NULL_HANDLE != mPresentVkQueue)
					{
						// Create Vulkan command pool instance
						mVkCommandPool = ::detail::createVkCommandPool(mVulkanRenderer, mVkDevice, mGraphicsQueueFamilyIndex);
						if (VK_NULL_HANDLE != mVkCommandPool)
						{
							// Create Vulkan command buffer instance
							mVkCommandBuffer = ::detail::createVkCommandBuffer(mVulkanRenderer, mVkDevice, mVkCommandPool);
						}
						else
						{
							// Error!
							RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create Vulkan command pool instance")
						}
					}
				}
				else
				{
					// Error!
					RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to get the Vulkan device graphics queue that command buffers are submitted to")
				}
			}
		}
	}

	VulkanContext::~VulkanContext()
	{
		if (VK_NULL_HANDLE != mVkDevice)
		{
			if (VK_NULL_HANDLE != mVkCommandPool)
			{
				if (VK_NULL_HANDLE != mVkCommandBuffer)
				{
					vkFreeCommandBuffers(mVkDevice, mVkCommandPool, 1, &mVkCommandBuffer);
				}
				vkDestroyCommandPool(mVkDevice, mVkCommandPool, nullptr);
			}
			vkDeviceWaitIdle(mVkDevice);
			vkDestroyDevice(mVkDevice, nullptr);
		}
	}

	uint32_t VulkanContext::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags vkMemoryPropertyFlags) const
	{
		VkPhysicalDeviceMemoryProperties vkPhysicalDeviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &vkPhysicalDeviceMemoryProperties);
		for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; ++i)
		{
			if ((typeFilter & (1 << i)) && (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & vkMemoryPropertyFlags) == vkMemoryPropertyFlags)
			{
				return i;
			}
		}

		// Error!
		RENDERER_LOG(mVulkanRenderer.getContext(), CRITICAL, "Failed to find suitable Vulkan memory type")
		return ~0u;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
