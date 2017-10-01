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

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'return': conversion from 'int' to 'std::char_traits<wchar_t>::int_type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'std::codecvt_base': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::codecvt_base': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4774)	// warning C4774: 'sprintf_s' : format string expected in argument 3 is not a string literal
	PRAGMA_WARNING_DISABLE_MSVC(5026)	// warning C5026: 'std::_Generic_error_category': move constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'std::_Generic_error_category': move assignment operator was implicitly defined as deleted
	#include <array>
PRAGMA_WARNING_POP
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	#include <vector>
PRAGMA_WARNING_POP
#include <cstring>	// For "strcmp()"


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

		VkPhysicalDevice selectPhysicalDevice(VulkanRenderer::VulkanRenderer& vulkanRenderer, const VkPhysicalDevices& vkPhysicalDevices, bool& enableDebugMarker)
		{
			// TODO(co) I'am sure this selection can be improved (rating etc.)
			for (const VkPhysicalDevice& vkPhysicalDevice : vkPhysicalDevices)
			{
				// Get of device extensions
				uint32_t propertyCount = 0;
				if ((vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &propertyCount, nullptr) != VK_SUCCESS) || (0 == propertyCount))
				{
					// Reject physical Vulkan device
					continue;
				}
				VkExtensionPropertiesVector vkExtensionPropertiesVector(propertyCount);
				if (vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &propertyCount, vkExtensionPropertiesVector.data()) != VK_SUCCESS)
				{
					// Reject physical Vulkan device
					continue;
				}

				{ // Reject physical Vulkan devices basing on swap chain support
					// Check device extensions
					const std::array<const char*, 2> deviceExtensions =
					{
						VK_KHR_SWAPCHAIN_EXTENSION_NAME,
						VK_KHR_MAINTENANCE1_EXTENSION_NAME	// We want to be able to specify a negative viewport height, this way we don't have to apply "<output position>.y = -<output position>.y" inside vertex shaders to compensate for the Vulkan coordinate system
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
						// Check whether or not the "VK_EXT_debug_marker"-extension is available
						// -> The "VK_EXT_debug_marker"-extension is only available when the application gets started by tools like RenderDoc ( https://renderdoc.org/ )
						// -> See "Offline debugging in Vulkan with VK_EXT_debug_marker and RenderDoc" - https://www.saschawillems.de/?page_id=2017
						if (enableDebugMarker)
						{
							// Check whether or not the "VK_EXT_debug_marker"-extension is available
							if (isExtensionAvailable(VK_EXT_DEBUG_MARKER_EXTENSION_NAME, vkExtensionPropertiesVector))
							{
								// TODO(co) Currently, when trying to use RenderDoc ( https://renderdoc.org/ ) while having Vulkan debug layers enabled, RenderDoc crashes
								// -> Windows 10 x64
								// -> Radeon software 17.7.2
								// -> GPU: AMD 290X
								// -> LunarG® Vulkan™ SDK 1.0.54.0
								// -> Tried RenderDoc v0.91 as well as "Nightly v0.x @ 2017-08-21 (Win x64)" ("RenderDoc_2017_08_21_177d595d_64.zip")
								if (vulkanRenderer.getVulkanRuntimeLinking().isValidationEnabled())
								{
									enableDebugMarker = false;
									RENDERER_LOG(vulkanRenderer.getContext(), WARNING, "Vulkan validation layers are enabled: If you want to use debug markers (\"VK_EXT_debug_marker\"-extension) please disable the validation layers")
								}
							}
							else
							{
								// Silently disable debug marker
								enableDebugMarker = false;
							}
						}

						// Select physical Vulkan device
						return vkPhysicalDevice;
					}
				}
			}

			// Error!
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to select a physical Vulkan device")
			return VK_NULL_HANDLE;
		}

		VkResult createVkDevice(VulkanRenderer::VulkanRenderer& vulkanRenderer, VkPhysicalDevice vkPhysicalDevice, const VkDeviceQueueCreateInfo& vkDeviceQueueCreateInfo, bool enableValidation, bool enableDebugMarker, VkDevice& vkDevice)
		{
			// See http://vulkan.gpuinfo.org/listfeatures.php to check out GPU hardware capabilities
			const std::array<const char*, 3> enabledExtensions =
			{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
				VK_KHR_MAINTENANCE1_EXTENSION_NAME,	// We want to be able to specify a negative viewport height, this way we don't have to apply "<output position>.y = -<output position>.y" inside vertex shaders to compensate for the Vulkan coordinate system
				VK_EXT_DEBUG_MARKER_EXTENSION_NAME
			};
			const VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures =
			{
				VK_FALSE,	// robustBufferAccess (VkBool32)
				VK_FALSE,	// fullDrawIndexUint32 (VkBool32)
				VK_FALSE,	// imageCubeArray (VkBool32)
				VK_FALSE,	// independentBlend (VkBool32)
				VK_TRUE,	// geometryShader (VkBool32)
				VK_TRUE,	// tessellationShader (VkBool32)
				VK_FALSE,	// sampleRateShading (VkBool32)
				VK_FALSE,	// dualSrcBlend (VkBool32)
				VK_FALSE,	// logicOp (VkBool32)
				VK_TRUE,	// multiDrawIndirect (VkBool32)
				VK_FALSE,	// drawIndirectFirstInstance (VkBool32)
				VK_TRUE,	// depthClamp (VkBool32)
				VK_FALSE,	// depthBiasClamp (VkBool32)
				VK_TRUE,	// fillModeNonSolid (VkBool32)
				VK_FALSE,	// depthBounds (VkBool32)
				VK_FALSE,	// wideLines (VkBool32)
				VK_FALSE,	// largePoints (VkBool32)
				VK_FALSE,	// alphaToOne (VkBool32)
				VK_FALSE,	// multiViewport (VkBool32)
				VK_TRUE,	// samplerAnisotropy (VkBool32)
				VK_FALSE,	// textureCompressionETC2 (VkBool32)
				VK_FALSE,	// textureCompressionASTC_LDR (VkBool32)
				VK_TRUE,	// textureCompressionBC (VkBool32)
				VK_FALSE,	// occlusionQueryPrecise (VkBool32)
				VK_FALSE,	// pipelineStatisticsQuery (VkBool32)
				VK_FALSE,	// vertexPipelineStoresAndAtomics (VkBool32)
				VK_FALSE,	// fragmentStoresAndAtomics (VkBool32)
				VK_FALSE,	// shaderTessellationAndGeometryPointSize (VkBool32)
				VK_FALSE,	// shaderImageGatherExtended (VkBool32)
				VK_FALSE,	// shaderStorageImageExtendedFormats (VkBool32)
				VK_FALSE,	// shaderStorageImageMultisample (VkBool32)
				VK_FALSE,	// shaderStorageImageReadWithoutFormat (VkBool32)
				VK_FALSE,	// shaderStorageImageWriteWithoutFormat (VkBool32)
				VK_FALSE,	// shaderUniformBufferArrayDynamicIndexing (VkBool32)
				VK_FALSE,	// shaderSampledImageArrayDynamicIndexing (VkBool32)
				VK_FALSE,	// shaderStorageBufferArrayDynamicIndexing (VkBool32)
				VK_FALSE,	// shaderStorageImageArrayDynamicIndexing (VkBool32)
				VK_FALSE,	// shaderClipDistance (VkBool32)
				VK_FALSE,	// shaderCullDistance (VkBool32)
				VK_FALSE,	// shaderFloat64 (VkBool32)
				VK_FALSE,	// shaderInt64 (VkBool32)
				VK_FALSE,	// shaderInt16 (VkBool32)
				VK_FALSE,	// shaderResourceResidency (VkBool32)
				VK_FALSE,	// shaderResourceMinLod (VkBool32)
				VK_FALSE,	// sparseBinding (VkBool32)
				VK_FALSE,	// sparseResidencyBuffer (VkBool32)
				VK_FALSE,	// sparseResidencyImage2D (VkBool32)
				VK_FALSE,	// sparseResidencyImage3D (VkBool32)
				VK_FALSE,	// sparseResidency2Samples (VkBool32)
				VK_FALSE,	// sparseResidency4Samples (VkBool32)
				VK_FALSE,	// sparseResidency8Samples (VkBool32)
				VK_FALSE,	// sparseResidency16Samples (VkBool32)
				VK_FALSE,	// sparseResidencyAliased (VkBool32)
				VK_FALSE,	// variableMultisampleRate (VkBool32)
				VK_FALSE	// inheritedQueries (VkBool32)
			};
			const VkDeviceCreateInfo vkDeviceCreateInfo =
			{
				VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,														// sType (VkStructureType)
				nullptr,																					// pNext (const void*)
				0,																							// flags (VkDeviceCreateFlags)
				1,																							// queueCreateInfoCount (uint32_t)
				&vkDeviceQueueCreateInfo,																	// pQueueCreateInfos (const VkDeviceQueueCreateInfo*)
				enableValidation ? VulkanRenderer::VulkanRuntimeLinking::NUMBER_OF_VALIDATION_LAYERS : 0,	// enabledLayerCount (uint32_t)
				enableValidation ? VulkanRenderer::VulkanRuntimeLinking::VALIDATION_LAYER_NAMES : nullptr,	// ppEnabledLayerNames (const char* const*)
				enableDebugMarker ? 3u : 2u,																// enabledExtensionCount (uint32_t)
				enabledExtensions.empty() ? nullptr : enabledExtensions.data(),								// ppEnabledExtensionNames (const char* const*)
				&vkPhysicalDeviceFeatures																	// pEnabledFeatures (const VkPhysicalDeviceFeatures*)
			};
			const VkResult vkResult = vkCreateDevice(vkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &vkDevice);
			if (VK_SUCCESS == vkResult && enableDebugMarker)
			{
				// Get "VK_EXT_debug_marker"-extension function pointers

				// Define a helper macro
				PRAGMA_WARNING_PUSH
				PRAGMA_WARNING_DISABLE_MSVC(4191)	// 'reinterpret_cast': unsafe conversion from 'PFN_vkVoidFunction' to '<x>'
				#define IMPORT_FUNC(funcName)																											\
					funcName = reinterpret_cast<PFN_##funcName>(vkGetDeviceProcAddr(vkDevice, #funcName));												\
					if (nullptr == funcName)																											\
					{																																	\
						RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to load instance based Vulkan function pointer \"%s\"", #funcName)	\
					}																																	\

				// "VK_EXT_debug_marker"-extension
				IMPORT_FUNC(vkDebugMarkerSetObjectTagEXT);
				IMPORT_FUNC(vkDebugMarkerSetObjectNameEXT);
				IMPORT_FUNC(vkCmdDebugMarkerBeginEXT);
				IMPORT_FUNC(vkCmdDebugMarkerEndEXT);
				IMPORT_FUNC(vkCmdDebugMarkerInsertEXT);

				// Undefine the helper macro
				#undef IMPORT_FUNC
				PRAGMA_WARNING_POP
			}

			// Done
			return vkResult;
		}

		VkDevice createVkDevice(VulkanRenderer::VulkanRenderer& vulkanRenderer, VkPhysicalDevice vkPhysicalDevice, bool enableValidation, bool enableDebugMarker, uint32_t& graphicsQueueFamilyIndex, uint32_t& presentQueueFamilyIndex)
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
						VkResult vkResult = createVkDevice(vulkanRenderer, vkPhysicalDevice, vkDeviceQueueCreateInfo, enableValidation, enableDebugMarker, vkDevice);
						if (VK_ERROR_LAYER_NOT_PRESENT == vkResult && enableValidation)
						{
							// Error! Since the show must go on, try creating a Vulkan device instance without validation enabled...
							RENDERER_LOG(vulkanRenderer.getContext(), WARNING, "Failed to create the Vulkan device instance with validation enabled, layer is not present")
							vkResult = createVkDevice(vulkanRenderer, vkPhysicalDevice, vkDeviceQueueCreateInfo, false, enableDebugMarker, vkDevice);
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

		// Get the physical Vulkan device this context should use
		bool enableDebugMarker = true;	// TODO(co) Make it possible to setup from the outside whether or not the "VK_EXT_debug_marker"-extension should be used (e.g. retail shipped games might not want to have this enabled)
		{
			detail::VkPhysicalDevices vkPhysicalDevices;
			::detail::enumeratePhysicalDevices(mVulkanRenderer, vulkanRuntimeLinking.getVkInstance(), vkPhysicalDevices);
			if (!vkPhysicalDevices.empty())
			{
				mVkPhysicalDevice = ::detail::selectPhysicalDevice(mVulkanRenderer, vkPhysicalDevices, enableDebugMarker);
			}
		}

		// Create the logical Vulkan device instance
		if (VK_NULL_HANDLE != mVkPhysicalDevice)
		{
			mVkDevice = ::detail::createVkDevice(mVulkanRenderer, mVkPhysicalDevice, vulkanRuntimeLinking.isValidationEnabled(), enableDebugMarker, mGraphicsQueueFamilyIndex, mPresentQueueFamilyIndex);
			if (VK_NULL_HANDLE != mVkDevice)
			{
				// Load device based instance level Vulkan function pointers
				if (mVulkanRenderer.getVulkanRuntimeLinking().loadDeviceLevelVulkanEntryPoints(mVkDevice))
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

	uint32_t VulkanContext::findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags vkMemoryPropertyFlags) const
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

	VkCommandBuffer VulkanContext::createVkCommandBuffer() const
	{
		return ::detail::createVkCommandBuffer(mVulkanRenderer, mVkDevice, mVkCommandPool);
	}

	void VulkanContext::destroyVkCommandBuffer(VkCommandBuffer vkCommandBuffer) const
	{
		if (VK_NULL_HANDLE != mVkCommandBuffer)
		{
			vkFreeCommandBuffers(mVkDevice, mVkCommandPool, 1, &vkCommandBuffer);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
