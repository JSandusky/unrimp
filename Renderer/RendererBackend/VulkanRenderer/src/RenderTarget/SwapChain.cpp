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
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/IContext.h"
#include "VulkanRenderer/Helper.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(VulkanRenderer &vulkanRenderer, handle nativeWindowHandle) :
		ISwapChain(vulkanRenderer),
		mNativeWindowHandle(nativeWindowHandle),
		mVkSurfaceKHR(VK_NULL_HANDLE),
		mVkSwapchainKHR(VK_NULL_HANDLE),
		mSwapchainImageCount(0)
	{
		// Get the Vulkan instance and the Vulkan physical device
		const VkInstance vkInstance = vulkanRenderer.getVulkanRuntimeLinking().getVkInstance();
		const IContext& context = vulkanRenderer.getContext();
		const VkPhysicalDevice vkPhysicalDevice = context.getVkPhysicalDevice();
		const VkDevice vkDevice = context.getVkDevice();

		// Create Vulkan surface instance depending on OS
		#ifdef _WIN32
			VkWin32SurfaceCreateInfoKHR vkWin32SurfaceCreateInfoKHR = {};
			vkWin32SurfaceCreateInfoKHR.sType	  = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			vkWin32SurfaceCreateInfoKHR.hinstance = reinterpret_cast<HINSTANCE>(::GetWindowLongPtr(reinterpret_cast<HWND>(nativeWindowHandle), GWLP_HINSTANCE));
			vkWin32SurfaceCreateInfoKHR.hwnd	  = reinterpret_cast<HWND>(nativeWindowHandle);
			VkResult vkResult = vkCreateWin32SurfaceKHR(vkInstance, &vkWin32SurfaceCreateInfoKHR, nullptr, &mVkSurfaceKHR);
		#else
			#ifdef __ANDROID__
				// TODO(co) Not tested - see https://github.com/SaschaWillems/Vulkan
				VkAndroidSurfaceCreateInfoKHR vkAndroidSurfaceCreateInfoKHR = {};
				vkAndroidSurfaceCreateInfoKHR.sType  = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
				vkAndroidSurfaceCreateInfoKHR.window = window;
				VkResult vkResult = vkCreateAndroidSurfaceKHR(vkInstance, &vkAndroidSurfaceCreateInfoKHR, nullptr, &mVkSurfaceKHR);
			#else
				// TODO(co) Not tested - see https://github.com/SaschaWillems/Vulkan
				VkXcbSurfaceCreateInfoKHR vkXcbSurfaceCreateInfoKHR = {};
				vkXcbSurfaceCreateInfoKHR.sType		 = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
				vkXcbSurfaceCreateInfoKHR.connection = connection;
				vkXcbSurfaceCreateInfoKHR.window	 = window;
				VkResult vkResult = vkCreateXcbSurfaceKHR(vkInstance, &vkXcbSurfaceCreateInfoKHR, nullptr, &mVkSurfaceKHR);
			#endif
		#endif

		// Get list of supported surface formats
		uint32_t surfaceFormatCount = 0;
		vkResult = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, mVkSurfaceKHR, &surfaceFormatCount, nullptr);
	//	assert(!vkResult);
	//	assert(surfaceFormatCount > 0);

		std::vector<VkSurfaceFormatKHR> vkSurfaceFormatKHRs(surfaceFormatCount);
		vkResult = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, mVkSurfaceKHR, &surfaceFormatCount, vkSurfaceFormatKHRs.data());
	//	assert(!vkResult);

		// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
		// there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
		VkFormat colorVkFormat;
		if ((surfaceFormatCount == 1) && (vkSurfaceFormatKHRs[0].format == VK_FORMAT_UNDEFINED))
		{
			colorVkFormat = VK_FORMAT_B8G8R8A8_UNORM;
		}
		else
		{
			// Always select the first available color format
			// If you need a specific format (e.g. SRGB) you'd need to
			// iterate over the list of available surface format and
			// check for it's presence
			colorVkFormat = vkSurfaceFormatKHRs[0].format;
		}
		VkColorSpaceKHR vkColorSpaceKHR = vkSurfaceFormatKHRs[0].colorSpace;

		// Get the width and height of the given native window and ensure they are never ever zero
		// -> See "getSafeWidthAndHeight()"-method comments for details
		uint32_t width  = 1;
		uint32_t height = 1;
		#ifdef _WIN32
		{
			// Get the client rectangle of the given native window
			RECT rect;
			::GetClientRect(reinterpret_cast<HWND>(nativeWindowHandle), &rect);

			// Get the width and height...
			width  = static_cast<uint32_t>(rect.right  - rect.left);
			height = static_cast<uint32_t>(rect.bottom - rect.top);

			// ... and ensure that none of them is ever zero
			if (width < 1)
			{
				width = 1;
			}
			if (height < 1)
			{
				height = 1;
			}
		}
		#endif




		// TODO(co) Move the rest into a method
		VkSwapchainKHR oldVkSwapchainKHR = mVkSwapchainKHR;

		// Get physical device surface properties and formats
		VkSurfaceCapabilitiesKHR vkSurfaceCapabilitiesKHR;
		vkResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, mVkSurfaceKHR, &vkSurfaceCapabilitiesKHR);
	//	assert(!vkResult);

		// Get available present modes
		uint32_t presentModeCount = 0;
		vkResult = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, mVkSurfaceKHR, &presentModeCount, nullptr);
	//	assert(!vkResult);
	//	assert(presentModeCount > 0);

		std::vector<VkPresentModeKHR> vkPresentModeKHRs(presentModeCount);
		vkResult = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, mVkSurfaceKHR, &presentModeCount, vkPresentModeKHRs.data());
	//	assert(!vkResult);

		// Width and height are either both -1, or both not -1.
		VkExtent2D swapchainExtent = {};
		if (vkSurfaceCapabilitiesKHR.currentExtent.width == -1)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = width;
			swapchainExtent.height = height;
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = vkSurfaceCapabilitiesKHR.currentExtent;
			width = vkSurfaceCapabilitiesKHR.currentExtent.width;
			height = vkSurfaceCapabilitiesKHR.currentExtent.height;
		}

		// Prefer mailbox mode if present, it's the lowest latency non-tearing present  mode
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (size_t i = 0; i < presentModeCount; ++i)
		{
			if (vkPresentModeKHRs[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (vkPresentModeKHRs[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		// Determine the number of images
		uint32_t desiredNumberOfSwapchainImages = vkSurfaceCapabilitiesKHR.minImageCount + 1;
		if ((vkSurfaceCapabilitiesKHR.maxImageCount > 0) && (desiredNumberOfSwapchainImages > vkSurfaceCapabilitiesKHR.maxImageCount))
		{
			desiredNumberOfSwapchainImages = vkSurfaceCapabilitiesKHR.maxImageCount;
		}

		VkSurfaceTransformFlagsKHR vkSurfaceTransformFlagsKHR;
		if (vkSurfaceCapabilitiesKHR.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			vkSurfaceTransformFlagsKHR = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else 
		{
			vkSurfaceTransformFlagsKHR = vkSurfaceCapabilitiesKHR.currentTransform;
		}

		VkSwapchainCreateInfoKHR vkSwapchainCreateInfoKHR = {};
		vkSwapchainCreateInfoKHR.sType			  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		vkSwapchainCreateInfoKHR.surface		  = mVkSurfaceKHR;
		vkSwapchainCreateInfoKHR.minImageCount	  = desiredNumberOfSwapchainImages;
		vkSwapchainCreateInfoKHR.imageFormat	  = colorVkFormat;
		vkSwapchainCreateInfoKHR.imageColorSpace  = vkColorSpaceKHR;
		vkSwapchainCreateInfoKHR.imageExtent	  = { swapchainExtent.width, swapchainExtent.height };
		vkSwapchainCreateInfoKHR.imageUsage		  = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		vkSwapchainCreateInfoKHR.preTransform	  = static_cast<VkSurfaceTransformFlagBitsKHR>(vkSurfaceTransformFlagsKHR);
		vkSwapchainCreateInfoKHR.imageArrayLayers = 1;
		vkSwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vkSwapchainCreateInfoKHR.presentMode	  = swapchainPresentMode;
		vkSwapchainCreateInfoKHR.oldSwapchain	  = oldVkSwapchainKHR;
		vkSwapchainCreateInfoKHR.clipped		  = true;
		vkSwapchainCreateInfoKHR.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		vkResult = vkCreateSwapchainKHR(vkDevice, &vkSwapchainCreateInfoKHR, nullptr, &mVkSwapchainKHR);
	//	assert(!vkResult);

		// If an existing swap chain is re-created, destroy the old swap chain
		// This also cleans up all the presentable images
		if (VK_NULL_HANDLE != oldVkSwapchainKHR)
		{
			for (uint32_t i = 0; i < mSwapchainImageCount; ++i)
			{
				vkDestroyImageView(vkDevice, mSwapChainBuffer[i].view, nullptr);
			}
			vkDestroySwapchainKHR(vkDevice, oldVkSwapchainKHR, nullptr);
		}

		vkResult = vkGetSwapchainImagesKHR(vkDevice, mVkSwapchainKHR, &mSwapchainImageCount, nullptr);
	//	assert(!vkResult);

		// Get the swap chain images
		mVkImages.resize(mSwapchainImageCount);
		vkResult = vkGetSwapchainImagesKHR(vkDevice, mVkSwapchainKHR, &mSwapchainImageCount, mVkImages.data());
	//	assert(!vkResult);

		// Get the swap chain buffers containing the image and image view
		mSwapChainBuffer.resize(mSwapchainImageCount);
		for (uint32_t i = 0; i < mSwapchainImageCount; ++i)
		{
			VkImageViewCreateInfo colorAttachmentView = {};
			colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorAttachmentView.format = colorVkFormat;
			colorAttachmentView.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorAttachmentView.subresourceRange.levelCount = 1;
			colorAttachmentView.subresourceRange.layerCount = 1;
			colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;

			mSwapChainBuffer[i].image = mVkImages[i];

			// Transform images from initial (undefined) to present layout
			Helper::setImageLayout(context.getSetupVkCommandBuffer(), mSwapChainBuffer[i].image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);

			colorAttachmentView.image = mSwapChainBuffer[i].image;

			vkResult = vkCreateImageView(vkDevice, &colorAttachmentView, nullptr, &mSwapChainBuffer[i].view);
		//	assert(!vkResult);
		}
	}

	SwapChain::~SwapChain()
	{
		if (VK_NULL_HANDLE != mVkSurfaceKHR)
		{
			const VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
			const VkDevice vkDevice = vulkanRenderer.getContext().getVkDevice();
			for (uint32_t i = 0; i < mSwapchainImageCount; ++i)
			{
				vkDestroyImageView(vkDevice, mSwapChainBuffer[i].view, nullptr);
			}
			if (VK_NULL_HANDLE != mVkSwapchainKHR)
			{
				vkDestroySwapchainKHR(vkDevice, mVkSwapchainKHR, nullptr);
			}
			vkDestroySurfaceKHR(vulkanRenderer.getVulkanRuntimeLinking().getVkInstance(), mVkSurfaceKHR, nullptr);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(uint32_t &width, uint32_t &height) const
	{
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
				VulkanRenderer &vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
				Display *display = static_cast<const ContextLinux&>(vulkanRenderer.getContext()).getDisplay();

				// Get the width and height...
				::Window rootWindow = 0;
				int positionX = 0, positionY = 0;
				unsigned int unsignedWidth = 0, unsignedHeight = 0, border = 0, depth = 0;
				XGetGeometry(display, mNativeWindowHandle, &rootWindow, &positionX, &positionY, &unsignedWidth, &unsignedHeight, &border, &depth);

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
		#ifdef WIN32
			HDC hDC = ::GetDC(reinterpret_cast<HWND>(mNativeWindowHandle));
			::SwapBuffers(hDC);
			::ReleaseDC(reinterpret_cast<HWND>(mNativeWindowHandle), hDC);
		#elif defined LINUX
			VulkanRenderer &vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
			glXSwapBuffers(static_cast<const ContextLinux&>(vulkanRenderer.getContext()).getDisplay(), mNativeWindowHandle);
		#else
			#error "Unsupported platform"
		#endif
	}

	void SwapChain::resizeBuffers()
	{
		// Nothing here
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

	void SwapChain::setWidthAndHeight(uint32_t, uint32_t)
	{
		// TODO(sw) implement me
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
