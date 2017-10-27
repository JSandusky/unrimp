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
#include "VulkanRenderer/RenderTarget/SwapChain.h"
#include "VulkanRenderer/RenderTarget/RenderPass.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/Mapping.h"
#include "VulkanRenderer/Helper.h"

#include <Renderer/ILog.h>
#include <Renderer/Context.h>

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
	#include <sstream>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		VkSurfaceKHR createPresentationSurface(const VulkanRenderer::VulkanContext& vulkanContext, VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice, Renderer::WindowHandle windoInfo)
		{
			VkSurfaceKHR vkSurfaceKHR = VK_NULL_HANDLE;

			#ifdef VK_USE_PLATFORM_WIN32_KHR
				const VkWin32SurfaceCreateInfoKHR vkWin32SurfaceCreateInfoKHR =
				{
					VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,																		// sType (VkStructureType)
					nullptr,																												// pNext (const void*)
					0,																														// flags (VkWin32SurfaceCreateFlagsKHR)
					reinterpret_cast<HINSTANCE>(::GetWindowLongPtr(reinterpret_cast<HWND>(windoInfo.nativeWindowHandle), GWLP_HINSTANCE)),	// hinstance (HINSTANCE)
					reinterpret_cast<HWND>(windoInfo.nativeWindowHandle)																	// hwnd (HWND)
				};
				if (vkCreateWin32SurfaceKHR(vkInstance, &vkWin32SurfaceCreateInfoKHR, nullptr, &vkSurfaceKHR) != VK_SUCCESS)
				{
					// TODO(co) Can we ensure "vkSurfaceKHR" doesn't get touched by "vkCreateWin32SurfaceKHR()" in case of failure?
					vkSurfaceKHR = VK_NULL_HANDLE;
				}
			#elif defined VK_USE_PLATFORM_ANDROID_KHR
				#warning "TODO(co) Not tested"
				const VkAndroidSurfaceCreateInfoKHR vkAndroidSurfaceCreateInfoKHR =
				{
					VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,				// sType (VkStructureType)
					nullptr,														// pNext (const void*)
					0,																// flags (VkAndroidSurfaceCreateFlagsKHR)
					reinterpret_cast<ANativeWindow*>(windoInfo.nativeWindowHandle)	// window (ANativeWindow*)
				};
				if (vkCreateAndroidSurfaceKHR(vkInstance, &vkAndroidSurfaceCreateInfoKHR, nullptr, &vkSurfaceKHR) != VK_SUCCESS)
				{
					// TODO(co) Can we ensure "vkSurfaceKHR" doesn't get touched by "vkCreateAndroidSurfaceKHR()" in case of failure?
					vkSurfaceKHR = VK_NULL_HANDLE;
				}
			#elif defined VK_USE_PLATFORM_XLIB_KHR || defined VK_USE_PLATFORM_WAYLAND_KHR
				VulkanRenderer::VulkanRenderer& vulkanRenderer = vulkanContext.getVulkanRenderer();
				const Renderer::Context& context = vulkanRenderer.getContext();
				assert(context.getType() == Renderer::Context::ContextType::X11 || context.getType() == Renderer::Context::ContextType::WAYLAND);

				// If the given renderer context is an X11 context use the display connection object provided by the context
				if (context.getType() == Renderer::Context::ContextType::X11)
				{
					const Renderer::X11Context& x11Context = static_cast<const Renderer::X11Context&>(context);
					const VkXlibSurfaceCreateInfoKHR vkXlibSurfaceCreateInfoKHR =
					{
						VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,	// sType (VkStructureType)
						nullptr,										// pNext (const void*)
						0,												// flags (VkXlibSurfaceCreateFlagsKHR)
						x11Context.getDisplay(),						// dpy (Display*)
						windoInfo.nativeWindowHandle					// window (Window)
					};
					if (vkCreateXlibSurfaceKHR(vkInstance, &vkXlibSurfaceCreateInfoKHR, nullptr, &vkSurfaceKHR) != VK_SUCCESS)
					{
						// TODO(co) Can we ensure "vkSurfaceKHR" doesn't get touched by "vkCreateXlibSurfaceKHR()" in case of failure?
						vkSurfaceKHR = VK_NULL_HANDLE;
					}
				}
				else if (context.getType() == Renderer::Context::ContextType::WAYLAND)
				{
					const Renderer::WaylandContext& waylandContext = static_cast<const Renderer::WaylandContext&>(context);
					const VkWaylandSurfaceCreateInfoKHR vkWaylandSurfaceCreateInfoKHR =
					{
						VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,	// sType (VkStructureType)
						nullptr,											// pNext (const void*)
						0,													// flags (VkWaylandSurfaceCreateInfoKHR)
						waylandContext.getDisplay(),						// display (wl_display*)
						windoInfo.waylandSurface							// surface (wl_surface*)
					};
					if (vkCreateWaylandSurfaceKHR(vkInstance, &vkWaylandSurfaceCreateInfoKHR, nullptr, &vkSurfaceKHR) != VK_SUCCESS)
					{
						// TODO(co) Can we ensure "vkSurfaceKHR" doesn't get touched by "vkCreateWaylandSurfaceKHR()" in case of failure?
						vkSurfaceKHR = VK_NULL_HANDLE;
					}
				}
			#elif defined VK_USE_PLATFORM_XCB_KHR
				#error "TODO(co) Complete implementation"
				const VkXcbSurfaceCreateInfoKHR vkXcbSurfaceCreateInfoKHR =
				{
					VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,	// sType (VkStructureType)
					nullptr,										// pNext (const void*)
					0,												// flags (VkXcbSurfaceCreateFlagsKHR)
					TODO(co)										// connection (xcb_connection_t*)
					TODO(co)										// window (xcb_window_t)
				};
				if (vkCreateXcbSurfaceKHR(vkInstance, &vkXcbSurfaceCreateInfoKHR, nullptr, &vkSurfaceKHR) != VK_SUCCESS)
				{
					// TODO(co) Can we ensure "vkSurfaceKHR" doesn't get touched by "vkCreateXcbSurfaceKHR()" in case of failure?
					vkSurfaceKHR = VK_NULL_HANDLE;
				}
			#else
				#error "Unsupported platform"
			#endif

			{ // Sanity check: Does the physical Vulkan device support the Vulkan presentation surface?
			  // TODO(co) Inside our renderer API the swap chain is physical device independent, which is a nice thing usability wise.
			  //          On the other hand, the sanity check here can only detect issues but it would be better to not get into such issues in the first place.
				VkBool32 queuePresentSupport = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, vulkanContext.getGraphicsQueueFamilyIndex(), vkSurfaceKHR, &queuePresentSupport);
				if (VK_FALSE == queuePresentSupport)
				{
					RENDERER_LOG(vulkanContext.getVulkanRenderer().getContext(), CRITICAL, "The created Vulkan presentation surface has no queue present support")
				}
			}

			// Done
			return vkSurfaceKHR;
		}

		uint32_t getNumberOfSwapChainImages(const VkSurfaceCapabilitiesKHR& vkSurfaceCapabilitiesKHR)
		{
			// Set of images defined in a swap chain may not always be available for application to render to:
			// - One may be displayed and one may wait in a queue to be presented
			// - If application wants to use more images at the same time it must ask for more images
			uint32_t numberOfImages = vkSurfaceCapabilitiesKHR.minImageCount + 1;
			if ((vkSurfaceCapabilitiesKHR.maxImageCount > 0) && (numberOfImages > vkSurfaceCapabilitiesKHR.maxImageCount))
			{
				numberOfImages = vkSurfaceCapabilitiesKHR.maxImageCount;
			}
			return numberOfImages;
		}

		VkSurfaceFormatKHR getSwapChainFormat(const Renderer::Context& context, VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR)
		{
			uint32_t surfaceFormatCount = 0;
			if ((vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurfaceKHR, &surfaceFormatCount, nullptr) != VK_SUCCESS) || (0 == surfaceFormatCount))
			{
				RENDERER_LOG(context, CRITICAL, "Failed to get physical Vulkan device surface formats")
				return { VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR };
			}

			std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
			if (vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurfaceKHR, &surfaceFormatCount, surfaceFormats.data()) != VK_SUCCESS)
			{
				RENDERER_LOG(context, CRITICAL, "Failed to get physical Vulkan device surface formats")
				return { VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR };
			}

			// If the list contains only one entry with undefined format it means that there are no preferred surface formats and any can be chosen
			if ((surfaceFormats.size() == 1) && (VK_FORMAT_UNDEFINED == surfaceFormats[0].format))
			{
				return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
			}

			// Check if list contains most widely used R8 G8 B8 A8 format with nonlinear color space
			// -> Not all implementations support RGBA8, some only support BGRA8 formats (e.g. xlib surface under Linux with RADV), so check for both
			for (const VkSurfaceFormatKHR& surfaceFormat : surfaceFormats)
			{
				if (VK_FORMAT_R8G8B8A8_UNORM == surfaceFormat.format || VK_FORMAT_B8G8R8A8_UNORM == surfaceFormat.format)
				{
					return surfaceFormat;
				}
			}

			// Return the first format from the list
			return surfaceFormats[0];
		}

		VkExtent2D getSwapChainExtent(const VkSurfaceCapabilitiesKHR& vkSurfaceCapabilitiesKHR)
		{
			// Special value of surface extent is width == height == -1
			// -> If this is so we define the size by ourselves but it must fit within defined confines, else it's already set to the operation window dimension
			if (-1 == vkSurfaceCapabilitiesKHR.currentExtent.width)
			{
				VkExtent2D swapChainExtent = { 640, 480 };
				if (swapChainExtent.width < vkSurfaceCapabilitiesKHR.minImageExtent.width)
				{
					swapChainExtent.width = vkSurfaceCapabilitiesKHR.minImageExtent.width;
				}
				if (swapChainExtent.height < vkSurfaceCapabilitiesKHR.minImageExtent.height)
				{
					swapChainExtent.height = vkSurfaceCapabilitiesKHR.minImageExtent.height;
				}
				if (swapChainExtent.width > vkSurfaceCapabilitiesKHR.maxImageExtent.width)
				{
					swapChainExtent.width = vkSurfaceCapabilitiesKHR.maxImageExtent.width;
				}
				if (swapChainExtent.height > vkSurfaceCapabilitiesKHR.maxImageExtent.height)
				{
					swapChainExtent.height = vkSurfaceCapabilitiesKHR.maxImageExtent.height;
				}
				return swapChainExtent;
			}

			// Most of the cases we define size of the swap chain images equal to current window's size
			return vkSurfaceCapabilitiesKHR.currentExtent;
		}

		VkImageUsageFlags getSwapChainUsageFlags(const Renderer::Context& context, const VkSurfaceCapabilitiesKHR& vkSurfaceCapabilitiesKHR)
		{
			// Color attachment flag must always be supported. We can define other usage flags but we always need to check if they are supported.
			if (vkSurfaceCapabilitiesKHR.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			{
				return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}

			// Construct the log message
			std::stringstream message;
			message << "VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported by the swap chain: Supported swap chain image usages include:\n";
			message << (vkSurfaceCapabilitiesKHR.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)				? "  VK_IMAGE_USAGE_TRANSFER_SRC\n" : "";
			message << (vkSurfaceCapabilitiesKHR.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)				? "  VK_IMAGE_USAGE_TRANSFER_DST\n" : "";
			message << (vkSurfaceCapabilitiesKHR.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT)					? "  VK_IMAGE_USAGE_SAMPLED\n" : "";
			message << (vkSurfaceCapabilitiesKHR.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT)					? "  VK_IMAGE_USAGE_STORAGE\n" : "";
			message << (vkSurfaceCapabilitiesKHR.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)			? "  VK_IMAGE_USAGE_COLOR_ATTACHMENT\n" : "";
			message << (vkSurfaceCapabilitiesKHR.supportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)	? "  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT\n" : "";
			message << (vkSurfaceCapabilitiesKHR.supportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)		? "  VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT\n" : "";
			message << (vkSurfaceCapabilitiesKHR.supportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)			? "  VK_IMAGE_USAGE_INPUT_ATTACHMENT" : "";

			// Print log message
			RENDERER_LOG(context, CRITICAL, message.str().c_str())

			// Error!
			return static_cast<VkImageUsageFlags>(-1);
		}

		VkSurfaceTransformFlagBitsKHR getSwapChainTransform(const VkSurfaceCapabilitiesKHR& vkSurfaceCapabilitiesKHR)
		{
			// - Sometimes images must be transformed before they are presented (i.e. due to device's orientation being other than default orientation)
			// - If the specified transform is other than current transform, presentation engine will transform image during presentation operation; this operation may hit performance on some platforms
			// - Here we don't want any transformations to occur so if the identity transform is supported use it otherwise just use the same transform as current transform
			return (vkSurfaceCapabilitiesKHR.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : vkSurfaceCapabilitiesKHR.currentTransform;
		}

		VkPresentModeKHR getSwapChainPresentMode(const Renderer::Context& context, VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR)
		{
			uint32_t presentModeCount = 0;
			if ((vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurfaceKHR, &presentModeCount, nullptr) != VK_SUCCESS) || (0 == presentModeCount))
			{
				RENDERER_LOG(context, CRITICAL, "Failed to get physical Vulkan device surface present modes")
				return VK_PRESENT_MODE_MAX_ENUM_KHR;
			}

			std::vector<VkPresentModeKHR> presentModes(presentModeCount);
			if (vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, vkSurfaceKHR, &presentModeCount, presentModes.data()) != VK_SUCCESS)
			{
				RENDERER_LOG(context, CRITICAL, "Failed to get physical Vulkan device surface present modes")
				return VK_PRESENT_MODE_MAX_ENUM_KHR;
			}

			// - FIFO present mode is always available
			// - MAILBOX is the lowest latency V-Sync enabled mode (something like triple-buffering) so use it if available
			for (const VkPresentModeKHR& presentMode : presentModes)
			{
				if (VK_PRESENT_MODE_MAILBOX_KHR == presentMode)
				{
					return presentMode;
				}
			}
			for (const VkPresentModeKHR& presentMode : presentModes)
			{
				if (VK_PRESENT_MODE_FIFO_KHR == presentMode)
				{
					return presentMode;
				}
			}

			// Error!
			RENDERER_LOG(context, CRITICAL, "FIFO present mode is not supported by the Vulkan swap chain")
			return VK_PRESENT_MODE_MAX_ENUM_KHR;
		}

		VkRenderPass createRenderPass(const Renderer::Context& context, VkDevice vkDevice, VkFormat colorVkFormat, VkFormat depthVkFormat, VkSampleCountFlagBits vkSampleCountFlagBits)
		{
			const bool hasDepthStencilAttachment = (VK_FORMAT_UNDEFINED != depthVkFormat);

			// Render pass configuration
			const std::array<VkAttachmentDescription, 2> vkAttachmentDescriptions =
			{{
				{
					0,									// flags (VkAttachmentDescriptionFlags)
					colorVkFormat,						// format (VkFormat)
					vkSampleCountFlagBits,				// samples (VkSampleCountFlagBits)
					VK_ATTACHMENT_LOAD_OP_CLEAR,		// loadOp (VkAttachmentLoadOp)
					VK_ATTACHMENT_STORE_OP_STORE,		// storeOp (VkAttachmentStoreOp)
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,	// stencilLoadOp (VkAttachmentLoadOp)
					VK_ATTACHMENT_STORE_OP_DONT_CARE,	// stencilStoreOp (VkAttachmentStoreOp)
					VK_IMAGE_LAYOUT_UNDEFINED,			// initialLayout (VkImageLayout)
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR		// finalLayout (VkImageLayout)
				},
				{
					0,													// flags (VkAttachmentDescriptionFlags)
					depthVkFormat,										// format (VkFormat)
					vkSampleCountFlagBits,								// samples (VkSampleCountFlagBits)
					VK_ATTACHMENT_LOAD_OP_CLEAR,						// loadOp (VkAttachmentLoadOp)
					VK_ATTACHMENT_STORE_OP_STORE,						// storeOp (VkAttachmentStoreOp)
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,					// stencilLoadOp (VkAttachmentLoadOp)
					VK_ATTACHMENT_STORE_OP_DONT_CARE,					// stencilStoreOp (VkAttachmentStoreOp)
					VK_IMAGE_LAYOUT_UNDEFINED,							// initialLayout (VkImageLayout)
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL	// finalLayout (VkImageLayout)
				}
			}};
			const VkAttachmentReference colorVkAttachmentReference =
			{
				0,											// attachment (uint32_t)
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL	// layout (VkImageLayout)
			};
			const VkAttachmentReference depthVkAttachmentReference =
			{
				1,													// attachment (uint32_t)
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL	// layout (VkImageLayout)
			};
			const VkSubpassDescription vkSubpassDescription =
			{
				0,																	// flags (VkSubpassDescriptionFlags)
				VK_PIPELINE_BIND_POINT_GRAPHICS,									// pipelineBindPoint (VkPipelineBindPoint)
				0,																	// inputAttachmentCount (uint32_t)
				nullptr,															// pInputAttachments (const VkAttachmentReference*)
				1,																	// colorAttachmentCount (uint32_t)
				&colorVkAttachmentReference,										// pColorAttachments (const VkAttachmentReference*)
				nullptr,															// pResolveAttachments (const VkAttachmentReference*)
				hasDepthStencilAttachment ? &depthVkAttachmentReference : nullptr,	// pDepthStencilAttachment (const VkAttachmentReference*)
				0,																	// preserveAttachmentCount (uint32_t)
				nullptr																// pPreserveAttachments (const uint32_t*)
			};
			const VkSubpassDependency vkSubpassDependency =
			{
				VK_SUBPASS_EXTERNAL,														// srcSubpass (uint32_t)
				0,																			// dstSubpass (uint32_t)
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,								// srcStageMask (VkPipelineStageFlags)
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,								// dstStageMask (VkPipelineStageFlags)
				0,																			// srcAccessMask (VkAccessFlags)
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,	// dstAccessMask (VkAccessFlags)
				0																			// dependencyFlags (VkDependencyFlags)
			};
			const VkRenderPassCreateInfo vkRenderPassCreateInfo =
			{
				VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,	// sType (VkStructureType)
				nullptr,									// pNext (const void*)
				0,											// flags (VkRenderPassCreateFlags)
				hasDepthStencilAttachment ? 2u : 1u,		// attachmentCount (uint32_t)
				vkAttachmentDescriptions.data(),			// pAttachments (const VkAttachmentDescription*)
				1,											// subpassCount (uint32_t)
				&vkSubpassDescription,						// pSubpasses (const VkSubpassDescription*)
				1,											// dependencyCount (uint32_t)
				&vkSubpassDependency						// pDependencies (const VkSubpassDependency*)
			};

			// Create render pass
			VkRenderPass vkRenderPass = VK_NULL_HANDLE;
			if (vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, nullptr, &vkRenderPass) != VK_SUCCESS)
			{
				RENDERER_LOG(context, CRITICAL, "Failed to create Vulkan render pass")
			}

			// Done
			return vkRenderPass;
		}

		VkFormat findSupportedVkFormat(VkPhysicalDevice vkPhysicalDevice, const std::vector<VkFormat>& vkFormatCandidates, VkImageTiling vkImageTiling, VkFormatFeatureFlags vkFormatFeatureFlags)
		{
			for (VkFormat vkFormat : vkFormatCandidates)
			{
				VkFormatProperties vkFormatProperties;
				vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, vkFormat, &vkFormatProperties);
				if (VK_IMAGE_TILING_LINEAR == vkImageTiling && (vkFormatProperties.linearTilingFeatures & vkFormatFeatureFlags) == vkFormatFeatureFlags)
				{
					return vkFormat;
				}
				else if (VK_IMAGE_TILING_OPTIMAL == vkImageTiling && (vkFormatProperties.optimalTilingFeatures & vkFormatFeatureFlags) == vkFormatFeatureFlags)
				{
					return vkFormat;
				}
			}

			// Failed to find supported Vulkan depth format
			return VK_FORMAT_UNDEFINED;
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
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	VkFormat SwapChain::findColorVkFormat(const Renderer::Context& context, VkInstance vkInstance, const VulkanContext& vulkanContext)
	{
		const VkPhysicalDevice vkPhysicalDevice = vulkanContext.getVkPhysicalDevice();
		const VkSurfaceKHR vkSurfaceKHR = detail::createPresentationSurface(vulkanContext, vkInstance, vkPhysicalDevice, Renderer::WindowHandle{context.getNativeWindowHandle(), nullptr, nullptr});
		const VkSurfaceFormatKHR desiredVkSurfaceFormatKHR = ::detail::getSwapChainFormat(context, vkPhysicalDevice, vkSurfaceKHR);
		vkDestroySurfaceKHR(vkInstance, vkSurfaceKHR, nullptr);
		return desiredVkSurfaceFormatKHR.format;
	}

	VkFormat SwapChain::findDepthVkFormat(VkPhysicalDevice vkPhysicalDevice)
	{
		return detail::findSupportedVkFormat(vkPhysicalDevice, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(Renderer::IRenderPass& renderPass, Renderer::WindowHandle windowHandle) :
		ISwapChain(renderPass),
		// Operation system window
		mNativeWindowHandle(windowHandle.nativeWindowHandle),
		mRenderWindow(windowHandle.renderWindow),
		// Vulkan presentation surface
		mVkSurfaceKHR(VK_NULL_HANDLE),
		// Vulkan swap chain and color render target related
		mVkSwapchainKHR(VK_NULL_HANDLE),
		mVkRenderPass(VK_NULL_HANDLE),
		mImageAvailableVkSemaphore(VK_NULL_HANDLE),
		mRenderingFinishedVkSemaphore(VK_NULL_HANDLE),
		mCurrentImageIndex(~0u),
		// Depth render target related
		mDepthVkFormat(Mapping::getVulkanFormat(static_cast<RenderPass&>(renderPass).getDepthStencilAttachmentTextureFormat())),
		mDepthVkImage(VK_NULL_HANDLE),
		mDepthVkDeviceMemory(VK_NULL_HANDLE),
		mDepthVkImageView(VK_NULL_HANDLE)
	{
		// Create the Vulkan presentation surface instance depending on the operation system
		VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(renderPass.getRenderer());
		const VulkanContext&   vulkanContext	= vulkanRenderer.getVulkanContext();
		const VkInstance	   vkInstance		= vulkanRenderer.getVulkanRuntimeLinking().getVkInstance();
		const VkPhysicalDevice vkPhysicalDevice	= vulkanContext.getVkPhysicalDevice();
		mVkSurfaceKHR = detail::createPresentationSurface(vulkanContext, vkInstance, vkPhysicalDevice, windowHandle);
		if (VK_NULL_HANDLE != mVkSurfaceKHR)
		{
			// Create the Vulkan swap chain
			createVulkanSwapChain();
		}
		else
		{
			// Error!
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "The swap chain failed to create the Vulkan presentation surface")
		}
	}

	SwapChain::~SwapChain()
	{
		if (VK_NULL_HANDLE != mVkSurfaceKHR)
		{
			destroyVulkanSwapChain();
			vkDestroySurfaceKHR(static_cast<VulkanRenderer&>(getRenderer()).getVulkanRuntimeLinking().getVkInstance(), mVkSurfaceKHR, nullptr);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(uint32_t& width, uint32_t& height) const
	{
		// Return stored width and height when both valid
		if (nullptr != mRenderWindow)
		{
			mRenderWindow->getWidthAndHeight(width, height);
			return;
		}
		#ifdef WIN32
			// Is there a valid native OS window?
			if (NULL_HANDLE != mNativeWindowHandle)
			{
				// Get the width and height
				long swapChainWidth  = 1;
				long swapChainHeight = 1;
				{
					// Get the client rectangle of the native output window
					// -> Don't use the width and height stored in "DXGI_SWAP_CHAIN_DESC" -> "DXGI_MODE_DESC"
					//    because it might have been modified in order to avoid zero values
					RECT rect;
					::GetClientRect(reinterpret_cast<HWND>(mNativeWindowHandle), &rect);

					// Get the width and height...
					swapChainWidth  = rect.right  - rect.left;
					swapChainHeight = rect.bottom - rect.top;

					// ... and ensure that none of them is ever zero
					if (swapChainWidth < 1)
					{
						swapChainWidth = 1;
					}
					if (swapChainHeight < 1)
					{
						swapChainHeight = 1;
					}
				}

				// Write out the width and height
				width  = static_cast<UINT>(swapChainWidth);
				height = static_cast<UINT>(swapChainHeight);
			}
			else
		#elif defined LINUX
			if (NULL_HANDLE != mNativeWindowHandle)
			{
				VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
				const Renderer::Context& context = vulkanRenderer.getContext();
				assert(context.getType() == Renderer::Context::ContextType::X11);

				// If the given renderer context is an X11 context use the display connection object provided by the context
				if (context.getType() == Renderer::Context::ContextType::X11)
				{
					const Renderer::X11Context& x11Context = static_cast<const Renderer::X11Context&>(context);
					Display* display = x11Context.getDisplay();

					// Get the width and height...
					::Window rootWindow = 0;
					int positionX = 0, positionY = 0;
					unsigned int unsignedWidth = 0, unsignedHeight = 0, border = 0, depth = 0;
					if (nullptr != display)
					{
						XGetGeometry(display, mNativeWindowHandle, &rootWindow, &positionX, &positionY, &unsignedWidth, &unsignedHeight, &border, &depth);
					}

					// ... and ensure that none of them is ever zero
					if (unsignedWidth < 1)
					{
						unsignedWidth = 1;
					}
					if (unsignedHeight < 1)
					{
						unsignedHeight = 1;
					}

					// Done
					width = unsignedWidth;
					height = unsignedHeight;
				}
			}
			else
		#else
			#error "Unsupported platform"
		#endif
		{
			// Set known default return values
			width  = 1;
			height = 1;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ISwapChain methods           ]
	//[-------------------------------------------------------]
	handle SwapChain::getNativeWindowHandle() const
	{
		return mNativeWindowHandle;
	}

	void SwapChain::present()
	{
		// TODO(co) "Renderer::IRenderWindow::present()" support
		/*
		if (nullptr != mRenderWindow)
		{
			mRenderWindow->present();
			return;
		}
		*/

		// Get the Vulkan context
		const VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
		const VulkanContext& vulkanContext = vulkanRenderer.getVulkanContext();

		{ // Queue submit
			const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			const VkCommandBuffer vkCommandBuffer = vulkanContext.getVkCommandBuffer();
			const VkSubmitInfo vkSubmitInfo =
			{
				VK_STRUCTURE_TYPE_SUBMIT_INFO,	// sType (VkStructureType)
				nullptr,						// pNext (const void*)
				1,								// waitSemaphoreCount (uint32_t)
				&mImageAvailableVkSemaphore,	// pWaitSemaphores (const VkSemaphore*)
				&waitDstStageMask,				// pWaitDstStageMask (const VkPipelineStageFlags*)
				1,								// commandBufferCount (uint32_t)
				&vkCommandBuffer,				// pCommandBuffers (const VkCommandBuffer*)
				1,								// signalSemaphoreCount (uint32_t)
				&mRenderingFinishedVkSemaphore	// pSignalSemaphores (const VkSemaphore*)
			};
			if (vkQueueSubmit(vulkanContext.getGraphicsVkQueue(), 1, &vkSubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
			{
				// Error!
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Vulkan queue submit failed")
				return;
			}
		}

		{ // Queue present
			const VkPresentInfoKHR vkPresentInfoKHR =
			{
				VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,	// sType (VkStructureType)
				nullptr,							// pNext (const void*)
				1,									// waitSemaphoreCount (uint32_t)
				&mRenderingFinishedVkSemaphore,		// pWaitSemaphores (const VkSemaphore*)
				1,									// swapchainCount (uint32_t)
				&mVkSwapchainKHR,					// pSwapchains (const VkSwapchainKHR*)
				&mCurrentImageIndex,				// pImageIndices (const uint32_t*)
				nullptr								// pResults (VkResult*)
			};
			const VkResult vkResult = vkQueuePresentKHR(vulkanContext.getPresentVkQueue(), &vkPresentInfoKHR);
			if (VK_SUCCESS != vkResult)
			{
				if (VK_ERROR_OUT_OF_DATE_KHR == vkResult || VK_SUBOPTIMAL_KHR == vkResult)
				{
					// Recreate the Vulkan swap chain
					createVulkanSwapChain();
					return;
				}
				else
				{
					// Error!
					RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to present Vulkan queue")
					return;
				}
			}
			vkQueueWaitIdle(vulkanContext.getPresentVkQueue());
		}

		// Acquire next image
		acquireNextImage(true);
	}

	void SwapChain::resizeBuffers()
	{
		// Recreate the Vulkan swap chain
		createVulkanSwapChain();
	}

	bool SwapChain::getFullscreenState() const
	{
		// TODO(co) Implement me
		return false;
	}

	void SwapChain::setFullscreenState(bool)
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void SwapChain::createVulkanSwapChain()
	{
		const Renderer::Context& context = getRenderer().getContext();

		// Get the Vulkan physical device
		const VulkanRenderer&  vulkanRenderer	= static_cast<const VulkanRenderer&>(getRenderer());
		const VulkanContext&   vulkanContext	= vulkanRenderer.getVulkanContext();
		const VkPhysicalDevice vkPhysicalDevice	= vulkanContext.getVkPhysicalDevice();
		const VkDevice		   vkDevice			= vulkanContext.getVkDevice();

		// Sanity checks
		assert(VK_NULL_HANDLE != vkPhysicalDevice);
		assert(VK_NULL_HANDLE != vkDevice);

		// Wait for the Vulkan device to become idle
		vkDeviceWaitIdle(vkDevice);

		// Get Vulkan surface capabilities
		VkSurfaceCapabilitiesKHR vkSurfaceCapabilitiesKHR;
		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, mVkSurfaceKHR, &vkSurfaceCapabilitiesKHR) != VK_SUCCESS)
		{
			RENDERER_LOG(context, CRITICAL, "Failed to get physical Vulkan device surface capabilities")
			return;
		}

		// Get Vulkan swap chain settings
		const uint32_t                      desiredNumberOfImages				 = ::detail::getNumberOfSwapChainImages(vkSurfaceCapabilitiesKHR);
		const VkSurfaceFormatKHR            desiredVkSurfaceFormatKHR			 = ::detail::getSwapChainFormat(context, vkPhysicalDevice, mVkSurfaceKHR);
		const VkExtent2D                    desiredVkExtent2D					 = ::detail::getSwapChainExtent(vkSurfaceCapabilitiesKHR);
		const VkImageUsageFlags             desiredVkImageUsageFlags			 = ::detail::getSwapChainUsageFlags(context, vkSurfaceCapabilitiesKHR);
		const VkSurfaceTransformFlagBitsKHR desiredVkSurfaceTransformFlagBitsKHR = ::detail::getSwapChainTransform(vkSurfaceCapabilitiesKHR);
		const VkPresentModeKHR              desiredVkPresentModeKHR				 = ::detail::getSwapChainPresentMode(context, vkPhysicalDevice, mVkSurfaceKHR);

		// Validate Vulkan swap chain settings
		if (-1 == static_cast<int>(desiredVkImageUsageFlags))
		{
			RENDERER_LOG(context, CRITICAL, "Invalid desired Vulkan image usage flags")
			return;
		}
		if (VK_PRESENT_MODE_MAX_ENUM_KHR == desiredVkPresentModeKHR)
		{
			RENDERER_LOG(context, CRITICAL, "Invalid desired Vulkan presentation mode")
			return;
		}
		if ((0 == desiredVkExtent2D.width) || (0 == desiredVkExtent2D.height))
		{
			// Current surface size is (0, 0) so we can't create a swap chain and render anything (CanRender == false)
			// But we don't wont to kill the application as this situation may occur i.e. when window gets minimized
			destroyVulkanSwapChain();
			return;
		}

		{ // Create Vulkan swap chain
			VkSwapchainKHR newVkSwapchainKHR = VK_NULL_HANDLE;
			const VkSwapchainCreateInfoKHR vkSwapchainCreateInfoKHR =
			{
				VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,	// sType (VkStructureType)
				nullptr,										// pNext (const void*)
				0,												// flags (VkSwapchainCreateFlagsKHR)
				mVkSurfaceKHR,									// surface (VkSurfaceKHR)
				desiredNumberOfImages,							// minImageCount (uint32_t)
				desiredVkSurfaceFormatKHR.format,				// imageFormat (VkFormat)
				desiredVkSurfaceFormatKHR.colorSpace,			// imageColorSpace (VkColorSpaceKHR)
				desiredVkExtent2D,								// imageExtent (VkExtent2D)
				1,												// imageArrayLayers (uint32_t)
				desiredVkImageUsageFlags,						// imageUsage (VkImageUsageFlags)
				VK_SHARING_MODE_EXCLUSIVE,						// imageSharingMode (VkSharingMode)
				0,												// queueFamilyIndexCount (uint32_t)
				nullptr,										// pQueueFamilyIndices (const uint32_t*)
				desiredVkSurfaceTransformFlagBitsKHR,			// preTransform (VkSurfaceTransformFlagBitsKHR)
				VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,				// compositeAlpha (VkCompositeAlphaFlagBitsKHR)
				desiredVkPresentModeKHR,						// presentMode (VkPresentModeKHR)
				VK_TRUE,										// clipped (VkBool32)
				mVkSwapchainKHR									// oldSwapchain (VkSwapchainKHR)
			};
			if (vkCreateSwapchainKHR(vkDevice, &vkSwapchainCreateInfoKHR, nullptr, &newVkSwapchainKHR) != VK_SUCCESS)
			{
				RENDERER_LOG(context, CRITICAL, "Failed to create Vulkan swap chain")
				return;
			}
			destroyVulkanSwapChain();
			mVkSwapchainKHR = newVkSwapchainKHR;
		}

		// Create depth render target
		createDepthRenderTarget(desiredVkExtent2D);

		// Create render pass
		mVkRenderPass = ::detail::createRenderPass(context, vkDevice, desiredVkSurfaceFormatKHR.format, mDepthVkFormat, static_cast<RenderPass&>(getRenderPass()).getVkSampleCountFlagBits());

		// Vulkan swap chain image handling
		if (VK_NULL_HANDLE != mVkRenderPass)
		{
			// Get the swap chain images
			uint32_t swapchainImageCount = 0;
			if (vkGetSwapchainImagesKHR(vkDevice, mVkSwapchainKHR, &swapchainImageCount, nullptr) != VK_SUCCESS)
			{
				RENDERER_LOG(context, CRITICAL, "Failed to get Vulkan swap chain images")
				return;
			}
			std::vector<VkImage> vkImages(swapchainImageCount);
			if (vkGetSwapchainImagesKHR(vkDevice, mVkSwapchainKHR, &swapchainImageCount, vkImages.data()) != VK_SUCCESS)
			{
				RENDERER_LOG(context, CRITICAL, "Failed to get Vulkan swap chain images")
				return;
			}

			// Get the swap chain buffers containing the image and image view
			mSwapChainBuffer.resize(swapchainImageCount);
			const bool hasDepthStencilAttachment = (VK_FORMAT_UNDEFINED != mDepthVkFormat);
			for (uint32_t i = 0; i < swapchainImageCount; ++i)
			{
				SwapChainBuffer& swapChainBuffer = mSwapChainBuffer[i];
				swapChainBuffer.vkImage = vkImages[i];

				// Create the Vulkan image view
				Helper::createVkImageView(vulkanRenderer, swapChainBuffer.vkImage, VK_IMAGE_VIEW_TYPE_2D, 1, 1, desiredVkSurfaceFormatKHR.format, VK_IMAGE_ASPECT_COLOR_BIT, swapChainBuffer.vkImageView);

				{ // Create the Vulkan framebuffer
					const std::array<VkImageView, 2> vkImageViews =
					{
						swapChainBuffer.vkImageView,
						mDepthVkImageView
					};
					const VkFramebufferCreateInfo vkFramebufferCreateInfo =
					{
						VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,	// sType (VkStructureType)
						nullptr,									// pNext (const void*)
						0,											// flags (VkFramebufferCreateFlags)
						mVkRenderPass,								// renderPass (VkRenderPass)
						hasDepthStencilAttachment ? 2u : 1u,		// attachmentCount (uint32_t)
						vkImageViews.data(),						// pAttachments (const VkImageView*)
						desiredVkExtent2D.width,					// width (uint32_t)
						desiredVkExtent2D.height,					// height (uint32_t)
						1											// layers (uint32_t)
					};
					if (vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, nullptr, &swapChainBuffer.vkFramebuffer) != VK_SUCCESS)
					{
						RENDERER_LOG(context, CRITICAL, "Failed to create Vulkan framebuffer")
					}
				}
			}
		}

		{ // Create the Vulkan semaphores
			const VkSemaphoreCreateInfo vkSemaphoreCreateInfo =
			{
				VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,	// sType (VkStructureType)
				nullptr,									// pNext (const void*)
				0											// flags (VkSemaphoreCreateFlags)
			};
			if ((vkCreateSemaphore(vkDevice, &vkSemaphoreCreateInfo, nullptr, &mImageAvailableVkSemaphore) != VK_SUCCESS) ||
				(vkCreateSemaphore(vkDevice, &vkSemaphoreCreateInfo, nullptr, &mRenderingFinishedVkSemaphore) != VK_SUCCESS))
			{
				RENDERER_LOG(context, CRITICAL, "Failed to create Vulkan semaphore")
			}
		}

		// Acquire next image
		acquireNextImage(false);
	}

	void SwapChain::destroyVulkanSwapChain()
	{
		// Destroy Vulkan swap chain
		if (VK_NULL_HANDLE != mVkRenderPass || !mSwapChainBuffer.empty() || VK_NULL_HANDLE != mVkSwapchainKHR)
		{
			const VkDevice vkDevice = static_cast<VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice();
			vkDeviceWaitIdle(vkDevice);
			if (VK_NULL_HANDLE != mVkRenderPass)
			{
				vkDestroyRenderPass(vkDevice, mVkRenderPass, nullptr);
				mVkRenderPass = VK_NULL_HANDLE;
			}
			if (!mSwapChainBuffer.empty())
			{
				for (const SwapChainBuffer& swapChainBuffer : mSwapChainBuffer)
				{
					vkDestroyFramebuffer(vkDevice, swapChainBuffer.vkFramebuffer, nullptr);
					vkDestroyImageView(vkDevice, swapChainBuffer.vkImageView, nullptr);
				}
				mSwapChainBuffer.clear();
			}
			if (VK_NULL_HANDLE != mVkSwapchainKHR)
			{
				vkDestroySwapchainKHR(vkDevice, mVkSwapchainKHR, nullptr);
				mVkSwapchainKHR = VK_NULL_HANDLE;
			}
			if (VK_NULL_HANDLE != mImageAvailableVkSemaphore)
			{
				vkDestroySemaphore(static_cast<VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(), mImageAvailableVkSemaphore, nullptr);
				mImageAvailableVkSemaphore = VK_NULL_HANDLE;
			}
			if (VK_NULL_HANDLE != mRenderingFinishedVkSemaphore)
			{
				vkDestroySemaphore(static_cast<VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(), mRenderingFinishedVkSemaphore, nullptr);
				mRenderingFinishedVkSemaphore = VK_NULL_HANDLE;
			}
		}

		// Destroy depth render target
		destroyDepthRenderTarget();
	}

	void SwapChain::acquireNextImage(bool recreateSwapChainIfNeeded)
	{
		const VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
		const VkResult vkResult = vkAcquireNextImageKHR(vulkanRenderer.getVulkanContext().getVkDevice(), mVkSwapchainKHR, UINT64_MAX, mImageAvailableVkSemaphore, VK_NULL_HANDLE, &mCurrentImageIndex);
		if (VK_SUCCESS != vkResult && VK_SUBOPTIMAL_KHR != vkResult)
		{
			if (VK_ERROR_OUT_OF_DATE_KHR == vkResult)
			{
				// Recreate the Vulkan swap chain
				if (recreateSwapChainIfNeeded)
				{
					createVulkanSwapChain();
				}
			}
			else
			{
				// Error!
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to acquire next Vulkan image from swap chain")
			}
		}
	}

	void SwapChain::createDepthRenderTarget(const VkExtent2D& vkExtent2D)
	{
		if (VK_FORMAT_UNDEFINED != mDepthVkFormat)
		{
			const VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
			Helper::createAndAllocateVkImage(vulkanRenderer, 0, VK_IMAGE_TYPE_2D, { vkExtent2D.width, vkExtent2D.height, 1 }, 1, 1, mDepthVkFormat, static_cast<RenderPass&>(getRenderPass()).getVkSampleCountFlagBits(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mDepthVkImage, mDepthVkDeviceMemory);
			Helper::createVkImageView(vulkanRenderer, mDepthVkImage, VK_IMAGE_VIEW_TYPE_2D, 1, 1, mDepthVkFormat, VK_IMAGE_ASPECT_DEPTH_BIT, mDepthVkImageView);
			Helper::transitionVkImageLayout(vulkanRenderer, mDepthVkImage, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		}
	}

	void SwapChain::destroyDepthRenderTarget()
	{
		if (VK_NULL_HANDLE != mDepthVkImage)
		{
			assert(VK_NULL_HANDLE != mDepthVkDeviceMemory);
			assert(VK_NULL_HANDLE != mDepthVkImageView);
			Helper::destroyAndFreeVkImage(static_cast<VulkanRenderer&>(getRenderer()), mDepthVkImage, mDepthVkDeviceMemory, mDepthVkImageView);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
