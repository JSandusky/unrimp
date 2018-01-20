/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include <Renderer/RenderTarget/ISwapChain.h>
#include <Renderer/RendererTypes.h>

#include "VulkanRenderer/VulkanRuntimeLinking.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <vector>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class Context;
}
namespace VulkanRenderer
{
	class VulkanContext;
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
	*    Vulkan swap chain class
	*
	*  @todo
	*    - TODO(co) Add support for debug name (not that important while at the same time more complex to implement here, but lets keep the TODO here to know there's room for improvement)
	*/
	class SwapChain final : public Renderer::ISwapChain
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		static VkFormat findColorVkFormat(const Renderer::Context& context, VkInstance vkInstance, const VulkanContext& vulkanContext);
		static VkFormat findDepthVkFormat(VkPhysicalDevice vkPhysicalDevice);


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderPass
		*    Render pass to use, the swap chain keeps a reference to the render pass
		*  @param[in] windowHandle
		*    Information about the window to render into
		*/
		SwapChain(Renderer::IRenderPass& renderPass, Renderer::WindowHandle windowHandle);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~SwapChain() override;

		/**
		*  @brief
		*    Return the Vulkan render pass
		*
		*  @return
		*    The Vulkan render pass
		*/
		inline VkRenderPass getVkRenderPass() const;

		/**
		*  @brief
		*    Return the current Vulkan image to render color into
		*
		*  @return
		*    The current Vulkan image to render color into
		*/
		inline VkImage getColorCurrentVkImage() const;

		/**
		*  @brief
		*    Return the Vulkan image to render depth into
		*
		*  @return
		*    The Vulkan image to render depth into
		*/
		inline VkImage getDepthVkImage() const;

		/**
		*  @brief
		*    Return the current Vulkan framebuffer to render into
		*
		*  @return
		*    The current Vulkan framebuffer to render into
		*/
		inline VkFramebuffer getCurrentVkFramebuffer() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	public:
		virtual void getWidthAndHeight(uint32_t& width, uint32_t& height) const override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ISwapChain methods           ]
	//[-------------------------------------------------------]
	public:
		virtual handle getNativeWindowHandle() const override;
		virtual void setVerticalSynchronizationInterval(uint32_t synchronizationInterval) override;
		virtual void present() override;
		virtual void resizeBuffers() override;
		virtual bool getFullscreenState() const override;
		virtual void setFullscreenState(bool fullscreen) override;
		inline virtual void setRenderWindow(Renderer::IRenderWindow* renderWindow) override;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit SwapChain(const SwapChain& source) = delete;
		SwapChain& operator =(const SwapChain& source) = delete;
		void createVulkanSwapChain();
		void destroyVulkanSwapChain();
		void acquireNextImage(bool recreateSwapChainIfNeeded);
		void createDepthRenderTarget(const VkExtent2D& vkExtent2D);
		void destroyDepthRenderTarget();


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct SwapChainBuffer
		{
			VkImage		  vkImage		= VK_NULL_HANDLE;	///< Vulkan image, don't destroy since we don't own it
			VkImageView   vkImageView	= VK_NULL_HANDLE;	///< Vulkan image view, destroy if no longer needed
			VkFramebuffer vkFramebuffer	= VK_NULL_HANDLE;	///< Vulkan framebuffer, destroy if no longer needed
		};
		typedef std::vector<SwapChainBuffer> SwapChainBuffers;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Operation system window
		handle					 mNativeWindowHandle;	///< Native window handle window, can be a null handle
		Renderer::IRenderWindow* mRenderWindow;			///< Render window instance, can be a null pointer, don't destroy the instance since we don't own it
		// Vulkan presentation surface
		VkSurfaceKHR mVkSurfaceKHR;	///< Vulkan presentation surface, destroy if no longer needed
		// Vulkan swap chain and color render target related
		VkSwapchainKHR	 mVkSwapchainKHR;				///< Vulkan swap chain, destroy if no longer needed
		VkRenderPass	 mVkRenderPass;					///< Vulkan render pass, destroy if no longer needed (due to "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR" we need an own Vulkan render pass instance)
		SwapChainBuffers mSwapChainBuffer;				///< Swap chain buffer for managing the color render targets
		VkSemaphore		 mImageAvailableVkSemaphore;	///< Vulkan semaphore, destroy if no longer needed
		VkSemaphore		 mRenderingFinishedVkSemaphore;	///< Vulkan semaphore, destroy if no longer needed
		uint32_t		 mCurrentImageIndex;			///< The index of the current Vulkan swap chain image to render into, ~0 if uninitialized
		// Depth render target related
		VkFormat		mDepthVkFormat;	///< Can be "VK_FORMAT_UNDEFINED" if no depth stencil buffer is needed
		VkImage			mDepthVkImage;
		VkDeviceMemory  mDepthVkDeviceMemory;
		VkImageView		mDepthVkImageView;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "VulkanRenderer/RenderTarget/SwapChain.inl"
