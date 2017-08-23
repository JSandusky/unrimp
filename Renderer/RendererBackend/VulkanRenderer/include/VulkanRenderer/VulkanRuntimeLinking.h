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
#include <Renderer/PlatformTypes.h>

#include "VulkanRenderer/Vulkan.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{
	class VulkanRenderer;
}


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
	*    Vulkan runtime linking for creating and managing the Vulkan instance ("VkInstance")
	*/
	class VulkanRuntimeLinking
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const uint32_t NUMBER_OF_VALIDATION_LAYERS;
		static const char*    VALIDATION_LAYER_NAMES[];


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] vulkanRenderer
		*    Owner Vulkan renderer instance
		*  @param[in] enableValidation
		*    Enable validation?
		*/
		VulkanRuntimeLinking(VulkanRenderer& vulkanRenderer, bool enableValidation);

		/**
		*  @brief
		*    Destructor
		*/
		~VulkanRuntimeLinking();

		/**
		*  @brief
		*    Return whether or not validation is enabled
		*
		*  @return
		*    "true" if validation is enabled, else "false"
		*/
		inline bool isValidationEnabled() const;

		/**
		*  @brief
		*    Return whether or not Vulkan is available
		*
		*  @return
		*    "true" if Vulkan is available, else "false"
		*/
		bool isVulkanAvaiable();

		/**
		*  @brief
		*    Return the Vulkan instance
		*
		*  @return
		*    Vulkan instance
		*/
		inline VkInstance getVkInstance() const;

		/**
		*  @brief
		*    Load the device level Vulkan function entry points
		*
		*  @param[in] vkDevice
		*    Vulkan device instance to load the function entry pointers for
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadDeviceLevelVulkanEntryPoints(VkDevice vkDevice) const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit VulkanRuntimeLinking(const VulkanRuntimeLinking& source) = delete;
		VulkanRuntimeLinking& operator =(const VulkanRuntimeLinking& source) = delete;

		/**
		*  @brief
		*    Load the shared libraries
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadSharedLibraries();

		/**
		*  @brief
		*    Load the global level Vulkan function entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadGlobalLevelVulkanEntryPoints() const;

		/**
		*  @brief
		*    Create the Vulkan instance
		*
		*  @param[in] enableValidation
		*    Enable validation layer? (don't do this for shipped products)
		*
		*  @return
		*    Vulkan instance creation result
		*/
		VkResult createVulkanInstance(bool enableValidation);

		/**
		*  @brief
		*    Load the instance level Vulkan function entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadInstanceLevelVulkanEntryPoints() const;

		/**
		*  @brief
		*    Setup debug callback
		*/
		void setupDebugCallback();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		VulkanRenderer&			 mVulkanRenderer;					///< Owner Vulkan renderer instance
		bool					 mValidationEnabled;				///< Validation enabled?
		void*					 mVulkanSharedLibrary;				///< Vulkan shared library, can be a null pointer
		bool					 mEntryPointsRegistered;			///< Entry points successfully registered?
		VkInstance				 mVkInstance;						///< Vulkan instance, stores all per-application states
		VkDebugReportCallbackEXT mVkDebugReportCallbackEXT;			///< Vulkan debug report callback, can be a null handle
		bool					 mInstanceLevelFunctionsRegistered;	///< Instance level Vulkan function pointers registered?
		bool					 mInitialized;						///< Already initialized?


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer


//[-------------------------------------------------------]
//[ Macros & definitions                                  ]
//[-------------------------------------------------------]
#ifndef FNPTR
	#ifdef VULKAN_DEFINERUNTIMELINKING
		#define FNPTR(name) PFN_##name name;
	#else
		#define FNPTR(name) extern PFN_##name name;
	#endif
#endif

// Global Vulkan function pointers
FNPTR(vkGetInstanceProcAddr)
FNPTR(vkGetDeviceProcAddr)
FNPTR(vkEnumerateInstanceExtensionProperties)
FNPTR(vkEnumerateInstanceLayerProperties)
FNPTR(vkCreateInstance)

// Instance based Vulkan function pointers
FNPTR(vkDestroyInstance)
FNPTR(vkEnumeratePhysicalDevices)
FNPTR(vkEnumerateDeviceLayerProperties)
FNPTR(vkEnumerateDeviceExtensionProperties)
FNPTR(vkGetPhysicalDeviceQueueFamilyProperties)
FNPTR(vkGetPhysicalDeviceFeatures)
FNPTR(vkGetPhysicalDeviceFormatProperties)
FNPTR(vkGetPhysicalDeviceMemoryProperties)
FNPTR(vkGetPhysicalDeviceProperties)
FNPTR(vkCreateDevice)
// "VK_EXT_debug_report"-extension
FNPTR(vkCreateDebugReportCallbackEXT)
FNPTR(vkDestroyDebugReportCallbackEXT)
// "VK_KHR_surface"-extension
FNPTR(vkDestroySurfaceKHR)
FNPTR(vkGetPhysicalDeviceSurfaceSupportKHR)
FNPTR(vkGetPhysicalDeviceSurfaceFormatsKHR)
FNPTR(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
FNPTR(vkGetPhysicalDeviceSurfacePresentModesKHR)
#ifdef VK_USE_PLATFORM_WIN32_KHR
	// "VK_KHR_win32_surface"-extension
	FNPTR(vkCreateWin32SurfaceKHR)
#elif defined VK_USE_PLATFORM_ANDROID_KHR
	// "VK_KHR_android_surface"-extension
	#warning "TODO(co) Not tested"
	FNPTR(vkCreateAndroidSurfaceKHR)
#elif defined VK_USE_PLATFORM_XLIB_KHR || defined VK_USE_PLATFORM_WAYLAND_KHR
	#if defined VK_USE_PLATFORM_XLIB_KHR
		// "VK_KHR_xlib_surface"-extension
		FNPTR(vkCreateXlibSurfaceKHR)
	#endif
	#if defined VK_USE_PLATFORM_WAYLAND_KHR
		// "VK_KHR_wayland_surface"-extension
		FNPTR(vkCreateWaylandSurfaceKHR)
	#endif
#elif defined VK_USE_PLATFORM_XCB_KHR
	// "VK_KHR_xcb_surface"-extension
	#warning "TODO(co) Not tested"
	FNPTR(vkCreateXcbSurfaceKHR)
#else
	#error "Unsupported platform"
#endif

// Device based Vulkan function pointers
FNPTR(vkDestroyDevice)
FNPTR(vkCreateShaderModule)
FNPTR(vkDestroyShaderModule)
FNPTR(vkCreateBuffer)
FNPTR(vkDestroyBuffer)
FNPTR(vkMapMemory)
FNPTR(vkUnmapMemory)
FNPTR(vkCreateBufferView)
FNPTR(vkDestroyBufferView)
FNPTR(vkAllocateMemory)
FNPTR(vkFreeMemory)
FNPTR(vkGetBufferMemoryRequirements)
FNPTR(vkBindBufferMemory)
FNPTR(vkCreateRenderPass)
FNPTR(vkDestroyRenderPass)
FNPTR(vkCreateImage)
FNPTR(vkDestroyImage)
FNPTR(vkGetImageSubresourceLayout)
FNPTR(vkGetImageMemoryRequirements)
FNPTR(vkBindImageMemory)
FNPTR(vkCreateImageView)
FNPTR(vkDestroyImageView)
FNPTR(vkCreateSampler)
FNPTR(vkDestroySampler)
FNPTR(vkCreateSemaphore)
FNPTR(vkDestroySemaphore)
FNPTR(vkCreateFence)
FNPTR(vkDestroyFence)
FNPTR(vkWaitForFences)
FNPTR(vkCreateCommandPool)
FNPTR(vkDestroyCommandPool)
FNPTR(vkAllocateCommandBuffers)
FNPTR(vkFreeCommandBuffers)
FNPTR(vkBeginCommandBuffer)
FNPTR(vkEndCommandBuffer)
FNPTR(vkGetDeviceQueue)
FNPTR(vkQueueSubmit)
FNPTR(vkQueueWaitIdle)
FNPTR(vkDeviceWaitIdle)
FNPTR(vkCreateFramebuffer)
FNPTR(vkDestroyFramebuffer)
FNPTR(vkCreatePipelineCache)
FNPTR(vkDestroyPipelineCache)
FNPTR(vkCreatePipelineLayout)
FNPTR(vkDestroyPipelineLayout)
FNPTR(vkCreateGraphicsPipelines)
FNPTR(vkCreateComputePipelines)
FNPTR(vkDestroyPipeline)
FNPTR(vkCreateDescriptorPool)
FNPTR(vkDestroyDescriptorPool)
FNPTR(vkCreateDescriptorSetLayout)
FNPTR(vkDestroyDescriptorSetLayout)
FNPTR(vkAllocateDescriptorSets)
FNPTR(vkFreeDescriptorSets)
FNPTR(vkUpdateDescriptorSets)
FNPTR(vkCreateQueryPool)
FNPTR(vkDestroyQueryPool)
FNPTR(vkGetQueryPoolResults)
FNPTR(vkCmdBeginQuery)
FNPTR(vkCmdEndQuery)
FNPTR(vkCmdResetQueryPool)
FNPTR(vkCmdCopyQueryPoolResults)
FNPTR(vkCmdPipelineBarrier)
FNPTR(vkCmdBeginRenderPass)
FNPTR(vkCmdEndRenderPass)
FNPTR(vkCmdExecuteCommands)
FNPTR(vkCmdCopyImage)
FNPTR(vkCmdBlitImage)
FNPTR(vkCmdCopyBufferToImage)
FNPTR(vkCmdClearAttachments)
FNPTR(vkCmdCopyBuffer)
FNPTR(vkCmdBindDescriptorSets)
FNPTR(vkCmdBindPipeline)
FNPTR(vkCmdSetViewport)
FNPTR(vkCmdSetScissor)
FNPTR(vkCmdSetLineWidth)
FNPTR(vkCmdSetDepthBias)
FNPTR(vkCmdPushConstants)
FNPTR(vkCmdBindIndexBuffer)
FNPTR(vkCmdBindVertexBuffers)
FNPTR(vkCmdDraw)
FNPTR(vkCmdDrawIndexed)
FNPTR(vkCmdDrawIndirect)
FNPTR(vkCmdDrawIndexedIndirect)
FNPTR(vkCmdDispatch)
FNPTR(vkCmdClearColorImage)
FNPTR(vkCmdClearDepthStencilImage)
// "VK_EXT_debug_marker"-extension
FNPTR(vkDebugMarkerSetObjectTagEXT)
FNPTR(vkDebugMarkerSetObjectNameEXT)
FNPTR(vkCmdDebugMarkerBeginEXT)
FNPTR(vkCmdDebugMarkerEndEXT)
FNPTR(vkCmdDebugMarkerInsertEXT)
// "VK_KHR_swapchain"-extension
FNPTR(vkCreateSwapchainKHR)
FNPTR(vkDestroySwapchainKHR)
FNPTR(vkGetSwapchainImagesKHR)
FNPTR(vkAcquireNextImageKHR)
FNPTR(vkQueuePresentKHR)


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "VulkanRenderer/VulkanRuntimeLinking.inl"
