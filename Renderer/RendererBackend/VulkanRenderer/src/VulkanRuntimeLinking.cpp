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
#define VULKAN_DEFINERUNTIMELINKING

#include "VulkanRenderer/VulkanRuntimeLinking.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_PRINTF()"
#ifdef WIN32
	#include <Renderer/WindowsHeader.h>
#elif defined LINUX
	#include <Renderer/LinuxHeader.h>

	#include <dlfcn.h>
	#include <link.h>
	#include <iostream>
#else
	#error "Unsupported platform"
#endif

#include <vector>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VulkanRuntimeLinking::VulkanRuntimeLinking() :
		mVulkanSharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mVkInstance(VK_NULL_HANDLE),
		mFunctionsRegistered(false),
		mInitialized(false)
	{
		// Nothing to do in here
	}

	VulkanRuntimeLinking::~VulkanRuntimeLinking()
	{
		// Destroy the Vulkan instance
		if (VK_NULL_HANDLE != mVkInstance)
		{
			vkDestroyInstance(mVkInstance, nullptr);
		}

		// Destroy the shared library instances
		#ifdef WIN32
			if (nullptr != mVulkanSharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mVulkanSharedLibrary));
			}
		#elif defined LINUX
			if (nullptr != mVulkanSharedLibrary)
			{
				::dlclose(mVulkanSharedLibrary);
			}
		#else
			#error "Unsupported platform"
		#endif
	}

	bool VulkanRuntimeLinking::isVulkanAvaiable()
	{
		// Already initialized?
		if (!mInitialized)
		{
			// We're now initialized
			mInitialized = true;

			// Load the shared libraries
			if (loadSharedLibraries())
			{
				// Load the Vulkan entry points
				mEntryPointsRegistered = loadVulkanEntryPoints();
				if (mEntryPointsRegistered)
				{
					// Create the Vulkan instance
					// TODO(co) For now, the Vulkan validation layer is always enabled by default
					const VkResult vkResult = createVulkanInstance(true);
					if (vkResult == VK_SUCCESS)
					{
						// Load instance based Vulkan function pointers
						mFunctionsRegistered = loadVulkanFunctions();
					}
					else
					{
						// Error!
						RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: Failed to create the Vulkan instance")
					}
				}
			}
		}

		// Entry points successfully registered?
		return (mEntryPointsRegistered && (VK_NULL_HANDLE != mVkInstance) && mFunctionsRegistered);
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool VulkanRuntimeLinking::loadSharedLibraries()
	{
		// Load the shared library
		#ifdef WIN32
			mVulkanSharedLibrary = ::LoadLibraryExA("vulkan-1.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr == mVulkanSharedLibrary)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Vulkan error: Failed to load in the shared library \"vulkan-1.dll\"\n")
			}
		#elif defined LINUX
			mVulkanSharedLibrary = ::dlopen("libvulkan-1.so", RTLD_NOW);
			if (nullptr == mVulkanSharedLibrary)
			{
				std::cout<<"Vulkan error: Failed to load in the shared library \"libvulkan-1.so\"\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
			}
		#else
			#error "Unsupported platform"
		#endif

		// Done
		return (nullptr != mVulkanSharedLibrary);
	}

	bool VulkanRuntimeLinking::loadVulkanEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																																			\
				if (result)																																							\
				{																																									\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mVulkanSharedLibrary), #funcName);																			\
					if (nullptr != symbol)																																			\
					{																																								\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																											\
					}																																								\
					else																																							\
					{																																								\
						wchar_t moduleFilename[MAX_PATH];																															\
						moduleFilename[0] = '\0';																																	\
						::GetModuleFileNameW(static_cast<HMODULE>(mVulkanSharedLibrary), moduleFilename, MAX_PATH);																	\
						RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: Failed to locate the entry point \"%s\" within the Vulkan shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																				\
					}																																								\
				}
		#elif defined LINUX
			#define IMPORT_FUNC(funcName)																																			\
				if (result)																																							\
				{																																									\
					void *symbol = ::dlsym(mVulkanSharedLibrary, #funcName);																										\
					if (nullptr != symbol)																																			\
					{																																								\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																											\
					}																																								\
					else																																							\
					{																																								\
						link_map *linkMap = nullptr;																																\
						const char* libraryName = "unknown";																														\
						if (dlinfo(mVulkanSharedLibrary, RTLD_DI_LINKMAP, &linkMap))																								\
						{																																							\
							libraryName = linkMap->l_name;																															\
						}																																							\
						std::cout << "Vulkan error: Failed to locate the entry point \"" << #funcName << "\" within the Vulkan shared library \"" << libraryName << "\"\n";		/* TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead*/ \
						result = false;																																				\
					}																																								\
				}
		#else
			#error "Unsupported platform"
		#endif

		// Load the entry points
		IMPORT_FUNC(vkCreateInstance);
		IMPORT_FUNC(vkDestroyInstance);
		IMPORT_FUNC(vkGetInstanceProcAddr);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	VkResult VulkanRuntimeLinking::createVulkanInstance(bool enableValidation)
	{
		const char* validationLayerNames[] =
		{
			"VK_LAYER_GOOGLE_threading",
			"VK_LAYER_LUNARG_mem_tracker",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_draw_state",
			"VK_LAYER_LUNARG_param_checker",
			"VK_LAYER_LUNARG_swapchain",
			"VK_LAYER_LUNARG_device_limits",
			"VK_LAYER_LUNARG_image",
			"VK_LAYER_GOOGLE_unique_objects",
		};
		const uint32_t validationLayerCount = 9;	// TODO(co) Introduce and use "countof()"

		// TODO(co) Make it possible for the user to provide application related information?
		VkApplicationInfo vkApplicationInfo = {};
		vkApplicationInfo.sType			   = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		vkApplicationInfo.pApplicationName = "Unrimp Application";
		vkApplicationInfo.pEngineName	   = "Unrimp";
		vkApplicationInfo.apiVersion		= VK_MAKE_VERSION(1, 0, 2);	// TODO(co) Temporary workaround for drivers not supporting SDK 1.0.3 upon launch, use VK_API_VERSION

		// Enable surface extensions depending on OS
		std::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };
		#if defined(_WIN32)
			enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		#elif defined(__ANDROID__)
			enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
		#elif defined(__linux__)
			enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
		#endif

		VkInstanceCreateInfo vkInstanceCreateInfo = {};
		vkInstanceCreateInfo.sType			  = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		vkInstanceCreateInfo.pNext			  = NULL;
		vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
		if (enableValidation)
		{
			enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
		vkInstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
		vkInstanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
		if (enableValidation)
		{
			vkInstanceCreateInfo.enabledLayerCount = validationLayerCount;
			vkInstanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
		}
		VkResult vkResult = vkCreateInstance(&vkInstanceCreateInfo, nullptr, &mVkInstance);
		if (vkResult == VK_ERROR_LAYER_NOT_PRESENT && enableValidation)
		{
			// Error! Since the show must go on, try creating a Vulkan instance without validation enabled...
			RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: Failed to create the Vulkan instance with validation enabled, layer is not present")
			vkResult = createVulkanInstance(false);
		}

		// Done
		return vkResult;
	}

	bool VulkanRuntimeLinking::loadVulkanFunctions()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#pragma warning(push)
		#pragma warning(disable: 4191)	// 'reinterpret_cast': unsafe conversion from 'PFN_vkVoidFunction' to '<x>'
		#define IMPORT_FUNC(funcName)																										\
			if (result)																														\
			{																																\
				##funcName = reinterpret_cast<PFN_##funcName>(vkGetInstanceProcAddr(mVkInstance, #funcName));								\
				if (NULL == ##funcName)																										\
				{																															\
					RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: Failed to load instance based Vulkan function pointer \"%s\"", #funcName)	\
					result = false;																											\
				}																															\
			}

		// Load the function pointers
		IMPORT_FUNC(vkEnumeratePhysicalDevices);
		IMPORT_FUNC(vkGetPhysicalDeviceProperties);
		IMPORT_FUNC(vkEnumerateDeviceLayerProperties);
		IMPORT_FUNC(vkEnumerateDeviceExtensionProperties);
		IMPORT_FUNC(vkGetPhysicalDeviceQueueFamilyProperties);
		IMPORT_FUNC(vkGetPhysicalDeviceFeatures);
		IMPORT_FUNC(vkCreateDevice);
		IMPORT_FUNC(vkGetPhysicalDeviceFormatProperties);
		IMPORT_FUNC(vkGetPhysicalDeviceMemoryProperties);
		IMPORT_FUNC(vkCmdPipelineBarrier);
		IMPORT_FUNC(vkCreateShaderModule);
		IMPORT_FUNC(vkCreateBuffer);
		IMPORT_FUNC(vkGetBufferMemoryRequirements);
		IMPORT_FUNC(vkMapMemory);
		IMPORT_FUNC(vkUnmapMemory);
		IMPORT_FUNC(vkBindBufferMemory);
		IMPORT_FUNC(vkDestroyBuffer);
		IMPORT_FUNC(vkAllocateMemory);
		IMPORT_FUNC(vkFreeMemory);
		IMPORT_FUNC(vkCreateRenderPass);
		IMPORT_FUNC(vkCmdBeginRenderPass);
		IMPORT_FUNC(vkCmdEndRenderPass);
		IMPORT_FUNC(vkCmdExecuteCommands);
		IMPORT_FUNC(vkCreateImage);
		IMPORT_FUNC(vkGetImageMemoryRequirements);
		IMPORT_FUNC(vkCreateImageView);
		IMPORT_FUNC(vkDestroyImageView);
		IMPORT_FUNC(vkBindImageMemory);
		IMPORT_FUNC(vkGetImageSubresourceLayout);
		IMPORT_FUNC(vkCmdCopyImage);
		IMPORT_FUNC(vkCmdBlitImage);
		IMPORT_FUNC(vkDestroyImage);
		IMPORT_FUNC(vkCmdClearAttachments);
		IMPORT_FUNC(vkCmdCopyBuffer);
		IMPORT_FUNC(vkCreateSampler);
		IMPORT_FUNC(vkDestroySampler);
		IMPORT_FUNC(vkCreateSemaphore);
		IMPORT_FUNC(vkDestroySemaphore);
		IMPORT_FUNC(vkCreateFence);
		IMPORT_FUNC(vkDestroyFence);
		IMPORT_FUNC(vkWaitForFences);
		IMPORT_FUNC(vkCreateCommandPool);
		IMPORT_FUNC(vkDestroyCommandPool);
		IMPORT_FUNC(vkAllocateCommandBuffers);
		IMPORT_FUNC(vkBeginCommandBuffer);
		IMPORT_FUNC(vkEndCommandBuffer);
		IMPORT_FUNC(vkGetDeviceQueue);
		IMPORT_FUNC(vkQueueSubmit);
		IMPORT_FUNC(vkQueueWaitIdle);
		IMPORT_FUNC(vkDeviceWaitIdle);
		IMPORT_FUNC(vkCreateFramebuffer);
		IMPORT_FUNC(vkCreatePipelineCache);
		IMPORT_FUNC(vkCreatePipelineLayout);
		IMPORT_FUNC(vkCreateGraphicsPipelines);
		IMPORT_FUNC(vkCreateComputePipelines);
		IMPORT_FUNC(vkCreateDescriptorPool);
		IMPORT_FUNC(vkCreateDescriptorSetLayout);
		IMPORT_FUNC(vkAllocateDescriptorSets);
		IMPORT_FUNC(vkUpdateDescriptorSets);
		IMPORT_FUNC(vkCmdBindDescriptorSets);
		IMPORT_FUNC(vkCmdBindPipeline);
		IMPORT_FUNC(vkCmdBindVertexBuffers);
		IMPORT_FUNC(vkCmdBindIndexBuffer);
		IMPORT_FUNC(vkCmdSetViewport);
		IMPORT_FUNC(vkCmdSetScissor);
		IMPORT_FUNC(vkCmdSetLineWidth);
		IMPORT_FUNC(vkCmdSetDepthBias);
		IMPORT_FUNC(vkCmdPushConstants);
		IMPORT_FUNC(vkCmdDrawIndexed);
		IMPORT_FUNC(vkCmdDraw);
		IMPORT_FUNC(vkCmdDispatch);
		IMPORT_FUNC(vkDestroyPipeline);
		IMPORT_FUNC(vkDestroyPipelineLayout);
		IMPORT_FUNC(vkDestroyDescriptorSetLayout);
		IMPORT_FUNC(vkDestroyDevice);
		IMPORT_FUNC(vkDestroyDescriptorPool);
		IMPORT_FUNC(vkFreeCommandBuffers);
		IMPORT_FUNC(vkDestroyRenderPass);
		IMPORT_FUNC(vkDestroyFramebuffer);
		IMPORT_FUNC(vkDestroyShaderModule);
		IMPORT_FUNC(vkDestroyPipelineCache);
		IMPORT_FUNC(vkCreateQueryPool);
		IMPORT_FUNC(vkDestroyQueryPool);
		IMPORT_FUNC(vkGetQueryPoolResults);
		IMPORT_FUNC(vkCmdBeginQuery);
		IMPORT_FUNC(vkCmdEndQuery);
		IMPORT_FUNC(vkCmdResetQueryPool);
		IMPORT_FUNC(vkCmdCopyQueryPoolResults);
		#if defined(__ANDROID__)
			IMPORT_FUNC(vkCreateAndroidSurfaceKHR);
			IMPORT_FUNC(vkDestroySurfaceKHR);
		#endif

		// Undefine the helper macro
		#undef IMPORT_FUNC
		#pragma warning(pop)

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
