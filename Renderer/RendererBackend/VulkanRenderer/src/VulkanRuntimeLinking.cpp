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
#define VULKAN_DEFINERUNTIMELINKING

#include "VulkanRenderer/VulkanRuntimeLinking.h"
#include "VulkanRenderer/VulkanRenderer.h"

#include <Renderer/ILog.h>
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
#include <sstream>


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
		typedef std::vector<VkExtensionProperties> VkExtensionPropertiesVector;


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
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

		const char* vkDebugReportObjectTypeToString(VkDebugReportObjectTypeEXT vkDebugReportObjectTypeEXT)
		{
			// Define helper macro
			#define VALUE(value) case value: return #value;

			// Evaluate
			switch (vkDebugReportObjectTypeEXT)
			{
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_KHR_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_MODE_KHR_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_OBJECT_TABLE_NVX_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_KHR_EXT)
				// VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_BEGIN_RANGE_EXT)	- Not possible due to identical value
				// VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_END_RANGE_EXT)		- Not possible due to identical value
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_RANGE_SIZE_EXT)
				VALUE(VK_DEBUG_REPORT_OBJECT_TYPE_MAX_ENUM_EXT)
			}

			// Undefine helper macro
			#undef VALUE

			// Error!
			return nullptr;
		}

		VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
		{
			const Renderer::Context* context = static_cast<const Renderer::Context*>(pUserData);

			// Get log message type
			// -> Vulkan is using a flags combination, map it to our log message type enumeration
			Renderer::ILog::Type type = Renderer::ILog::Type::TRACE;
			if ((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0)
			{
				type = Renderer::ILog::Type::CRITICAL;
			}
			else if ((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0 || (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) != 0)
			{
				type = Renderer::ILog::Type::WARNING;
			}
			else if ((flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0)
			{
				type = Renderer::ILog::Type::INFORMATION;
			}
			else if ((flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) != 0)
			{
				type = Renderer::ILog::Type::DEBUG;
			}

			// Construct the log message
			std::stringstream message;
			message << "Vulkan debug report callback: ";
			message << "Object type: \"" << vkDebugReportObjectTypeToString(objectType) << "\" ";
			message << "Object: \"" << object << "\" ";
			message << "Location: \"" << location << "\" ";
			message << "Message code: \"" << messageCode << "\" ";
			message << "Layer prefix: \"" << pLayerPrefix << "\" ";
			message << "Message: \"" << pMessage << "\" ";

			// Print log message
			context->getLog().print(type, message.str().c_str());

			// The Vulkan call should not be aborted to have the same behavior with and without validation layers enabled
			return VK_FALSE;
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
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	#if defined(__ANDROID__)
		// On Android we need to explicitly select all layers
		#warning "TODO(co) Not tested"
		const uint32_t VulkanRuntimeLinking::NUMBER_OF_VALIDATION_LAYERS = 6;
		const char*    VulkanRuntimeLinking::VALIDATION_LAYER_NAMES[] =
		{
			"VK_LAYER_GOOGLE_threading",
			"VK_LAYER_LUNARG_parameter_validation",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_core_validation",
			"VK_LAYER_LUNARG_swapchain",
			"VK_LAYER_GOOGLE_unique_objects"
		};
	#else
		// On desktop the LunarG loaders exposes a meta layer that contains all layers
		const uint32_t VulkanRuntimeLinking::NUMBER_OF_VALIDATION_LAYERS = 1;
		const char*    VulkanRuntimeLinking::VALIDATION_LAYER_NAMES[] =
		{
			"VK_LAYER_LUNARG_standard_validation"
		};
	#endif


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VulkanRuntimeLinking::VulkanRuntimeLinking(VulkanRenderer& vulkanRenderer, bool enableValidation) :
		mVulkanRenderer(vulkanRenderer),
		mValidationEnabled(enableValidation),
		mVulkanSharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mVkInstance(VK_NULL_HANDLE),
		mVkDebugReportCallbackEXT(VK_NULL_HANDLE),
		mFunctionsRegistered(false),
		mInitialized(false)
	{
		// Nothing here
	}

	VulkanRuntimeLinking::~VulkanRuntimeLinking()
	{
		// Destroy the Vulkan debug report callback
		if (VK_NULL_HANDLE != mVkDebugReportCallbackEXT)
		{
			vkDestroyDebugReportCallbackEXT(mVkInstance, mVkDebugReportCallbackEXT, nullptr);
		}

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
					const VkResult vkResult = createVulkanInstance(mValidationEnabled);
					if (VK_SUCCESS == vkResult)
					{
						// Load instance based Vulkan function pointers
						mFunctionsRegistered = loadVulkanFunctions();

						// Setup debug callback
						if (mFunctionsRegistered && mValidationEnabled)
						{
							setupDebugCallback();
						}
					}
					else
					{
						// Error!
						RENDERER_LOG(mVulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan instance")
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
				RENDERER_LOG(mVulkanRenderer.getContext(), CRITICAL, "Failed to load in the shared Vulkan library \"vulkan-1.dll\"")
			}
		#elif defined LINUX
			mVulkanSharedLibrary = ::dlopen("libvulkan-1.so", RTLD_NOW);
			if (nullptr == mVulkanSharedLibrary)
			{
				RENDERER_LOG(mVulkanRenderer.getContext(), CRITICAL, "Failed to load in the shared Vulkan library \"libvulkan-1.so\"")
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
			#define IMPORT_FUNC(funcName)																																					\
				if (result)																																									\
				{																																											\
					void* symbol = ::GetProcAddress(static_cast<HMODULE>(mVulkanSharedLibrary), #funcName);																					\
					if (nullptr != symbol)																																					\
					{																																										\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																													\
					}																																										\
					else																																									\
					{																																										\
						wchar_t moduleFilename[MAX_PATH];																																	\
						moduleFilename[0] = '\0';																																			\
						::GetModuleFileNameW(static_cast<HMODULE>(mVulkanSharedLibrary), moduleFilename, MAX_PATH);																			\
						RENDERER_LOG(mVulkanRenderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the shared Vulkan library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																						\
					}																																										\
				}
		#elif defined LINUX
			#define IMPORT_FUNC(funcName)																																				\
				if (result)																																								\
				{																																										\
					void* symbol = ::dlsym(mVulkanSharedLibrary, #funcName);																											\
					if (nullptr != symbol)																																				\
					{																																									\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																												\
					}																																									\
					else																																								\
					{																																									\
						link_map *linkMap = nullptr;																																	\
						const char* libraryName = "unknown";																															\
						if (dlinfo(mVulkanSharedLibrary, RTLD_DI_LINKMAP, &linkMap))																									\
						{																																								\
							libraryName = linkMap->l_name;																																\
						}																																								\
						RENDERER_LOG(mVulkanRenderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the shared Vulkan library \"%s\"", #funcName, libraryName)	\
						result = false;																																					\
					}																																									\
				}
		#else
			#error "Unsupported platform"
		#endif

		// Load the entry points
		IMPORT_FUNC(vkCreateInstance);
		IMPORT_FUNC(vkDestroyInstance);
		IMPORT_FUNC(vkGetInstanceProcAddr);
		IMPORT_FUNC(vkEnumerateInstanceExtensionProperties);
		IMPORT_FUNC(vkEnumerateInstanceLayerProperties );

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	VkResult VulkanRuntimeLinking::createVulkanInstance(bool enableValidation)
	{
		// Enable surface extensions depending on OS
		std::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };
		#ifdef VK_USE_PLATFORM_WIN32_KHR
			enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		#elif defined VK_USE_PLATFORM_ANDROID_KHR
			#warning "TODO(co) Not tested"
			enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
		#elif defined VK_USE_PLATFORM_XLIB_KHR
			#warning "TODO(co) Not tested"
			enabledExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
		#elif defined VK_USE_PLATFORM_XCB_KHR
			#warning "TODO(co) Not tested"
			enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
		#else
			#error "Unsupported platform"
		#endif
		if (enableValidation)
		{
			enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		{ // Ensure the extensions we need are supported
			uint32_t propertyCount = 0;
			if ((vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr) != VK_SUCCESS) || (0 == propertyCount))
			{
				RENDERER_LOG(mVulkanRenderer.getContext(), CRITICAL, "Failed to enumerate Vulkan instance extension properties")
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
			::detail::VkExtensionPropertiesVector vkExtensionPropertiesVector(propertyCount);
			if (vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, &vkExtensionPropertiesVector[0]) != VK_SUCCESS)
			{
				RENDERER_LOG(mVulkanRenderer.getContext(), CRITICAL, "Failed to enumerate Vulkan instance extension properties")
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
			for (const char* enabledExtension : enabledExtensions)
			{
				if (!::detail::isExtensionAvailable(enabledExtension, vkExtensionPropertiesVector))
				{
					RENDERER_LOG(mVulkanRenderer.getContext(), CRITICAL, "Couldn't find Vulkan instance extension named \"%s\"", enabledExtension)
					return VK_ERROR_EXTENSION_NOT_PRESENT;
				}
			}
		}

		// TODO(co) Make it possible for the user to provide application related information?
		const VkApplicationInfo vkApplicationInfo =
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,	// sType (VkStructureType)
			nullptr,							// pNext (const void*)
			"Unrimp Application",				// pApplicationName (const char*)
			VK_MAKE_VERSION(0, 0, 0),			// applicationVersion (uint32_t)
			"Unrimp",							// pEngineName (const char*)
			VK_MAKE_VERSION(0, 0, 0),			// engineVersion (uint32_t)
			VK_API_VERSION_1_0					// apiVersion (uint32_t)
		};

		const VkInstanceCreateInfo vkInstanceCreateInfo =
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,					// sType (VkStructureType)
			nullptr,												// pNext (const void*)
			0,														// flags (VkInstanceCreateFlags)
			&vkApplicationInfo,										// pApplicationInfo (const VkApplicationInfo*)
			enableValidation ? NUMBER_OF_VALIDATION_LAYERS : 0,		// enabledLayerCount (uint32_t)
			enableValidation ? VALIDATION_LAYER_NAMES : nullptr,	// ppEnabledLayerNames (const char* const*)
			static_cast<uint32_t>(enabledExtensions.size()),		// enabledExtensionCount (uint32_t)
			enabledExtensions.data()								// ppEnabledExtensionNames (const char* const*)
		};
		VkResult vkResult = vkCreateInstance(&vkInstanceCreateInfo, nullptr, &mVkInstance);
		if (VK_ERROR_LAYER_NOT_PRESENT == vkResult && enableValidation)
		{
			// Error! Since the show must go on, try creating a Vulkan instance without validation enabled...
			RENDERER_LOG(mVulkanRenderer.getContext(), WARNING, "Failed to create the Vulkan instance with validation enabled, layer is not present. Install e.g. the LunarG Vulkan SDK and see e.g. https://vulkan.lunarg.com/doc/view/1.0.51.0/windows/layers.html .")
			mValidationEnabled = false;
			vkResult = createVulkanInstance(mValidationEnabled);
		}

		// Done
		return vkResult;
	}

	bool VulkanRuntimeLinking::loadVulkanFunctions()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		PRAGMA_WARNING_PUSH
		PRAGMA_WARNING_DISABLE_MSVC(4191)	// 'reinterpret_cast': unsafe conversion from 'PFN_vkVoidFunction' to '<x>'
		#define IMPORT_FUNC(funcName)																												\
			if (result)																																\
			{																																		\
				##funcName = reinterpret_cast<PFN_##funcName>(vkGetInstanceProcAddr(mVkInstance, #funcName));										\
				if (nullptr == ##funcName)																											\
				{																																	\
					RENDERER_LOG(mVulkanRenderer.getContext(), CRITICAL, "Failed to load instance based Vulkan function pointer \"%s\"", #funcName)	\
					result = false;																													\
				}																																	\
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
		IMPORT_FUNC(vkCmdClearColorImage);
		IMPORT_FUNC(vkCmdClearDepthStencilImage);
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

		// "VK_EXT_debug_report"-extension
		IMPORT_FUNC(vkCreateDebugReportCallbackEXT);
		IMPORT_FUNC(vkDestroyDebugReportCallbackEXT);

		// "VK_KHR_surface"-extension
		IMPORT_FUNC(vkDestroySurfaceKHR);
		IMPORT_FUNC(vkGetPhysicalDeviceSurfaceSupportKHR);
		IMPORT_FUNC(vkGetPhysicalDeviceSurfaceFormatsKHR);
		IMPORT_FUNC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
		IMPORT_FUNC(vkGetPhysicalDeviceSurfacePresentModesKHR);
		#ifdef VK_USE_PLATFORM_WIN32_KHR
			// "VK_KHR_win32_surface"-extension
			IMPORT_FUNC(vkCreateWin32SurfaceKHR);
		#elif defined VK_USE_PLATFORM_ANDROID_KHR
			// "VK_KHR_android_surface"-extension
			#warning "TODO(co) Not tested"
			IMPORT_FUNC(vkCreateAndroidSurfaceKHR);
		#elif defined VK_USE_PLATFORM_XLIB_KHR
			// "VK_KHR_xlib_surface"-extension
			#warning "TODO(co) Not tested"
			IMPORT_FUNC(vkCreateXlibSurfaceKHR);
		#elif defined VK_USE_PLATFORM_XCB_KHR
			// "VK_KHR_xcb_surface"-extension
			#warning "TODO(co) Not tested"
			IMPORT_FUNC(vkCreateXcbSurfaceKHR);
		#else
			#error "Unsupported platform"
		#endif

		// "VK_KHR_swapchain"-extension
		IMPORT_FUNC(vkCreateSwapchainKHR);
		IMPORT_FUNC(vkDestroySwapchainKHR);
		IMPORT_FUNC(vkGetSwapchainImagesKHR);
		IMPORT_FUNC(vkAcquireNextImageKHR);
		IMPORT_FUNC(vkQueuePresentKHR);

		// Undefine the helper macro
		#undef IMPORT_FUNC
		PRAGMA_WARNING_POP

		// Done
		return result;
	}

	void VulkanRuntimeLinking::setupDebugCallback()
	{
		// Sanity check
		assert(mValidationEnabled && "Do only call this method if validation is enabled");

		// The report flags determine what type of messages for the layers will be displayed
		// -> Use "VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT" to get everything, quite verbose
		const VkDebugReportFlagsEXT vkDebugReportFlagsEXT = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;

		// Setup debug callback
		const VkDebugReportCallbackCreateInfoEXT vkDebugReportCallbackCreateInfoEXT =
		{
			VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,		// sType (VkStructureType)
			nullptr,														// pNext (const void*)
			vkDebugReportFlagsEXT,											// flags (VkDebugReportFlagsEXT)
			::detail::debugReportCallback,									// pfnCallback (PFN_vkDebugReportCallbackEXT)
			const_cast<Renderer::Context*>(&mVulkanRenderer.getContext())	// pUserData (void*)
		};
		if (vkCreateDebugReportCallbackEXT(mVkInstance, &vkDebugReportCallbackCreateInfoEXT, nullptr, &mVkDebugReportCallbackEXT) != VK_SUCCESS)
		{
			RENDERER_LOG(mVulkanRenderer.getContext(), WARNING, "Failed to create the Vulkan debug report callback")
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
