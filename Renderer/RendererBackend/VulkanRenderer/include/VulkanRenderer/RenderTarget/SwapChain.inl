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
#include <Renderer/IAssert.h>
#include <Renderer/IRenderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline VkRenderPass SwapChain::getVkRenderPass() const
	{
		return mVkRenderPass;
	}

	inline VkImage SwapChain::getColorCurrentVkImage() const
	{
		RENDERER_ASSERT(getRenderer().getContext(), ~0u != mCurrentImageIndex, "Invalid index of the current Vulkan swap chain image to render into (Vulkan swap chain creation failed?)");
		RENDERER_ASSERT(getRenderer().getContext(), mCurrentImageIndex < mSwapChainBuffer.size(), "Out-of-bounds index of the current Vulkan swap chain image to render into");
		return mSwapChainBuffer[mCurrentImageIndex].vkImage;
	}

	inline VkImage SwapChain::getDepthVkImage() const
	{
		return mDepthVkImage;
	}

	inline VkFramebuffer SwapChain::getCurrentVkFramebuffer() const
	{
		RENDERER_ASSERT(getRenderer().getContext(), ~0u != mCurrentImageIndex, "Invalid index of the current Vulkan swap chain image to render into (Vulkan swap chain creation failed?)");
		RENDERER_ASSERT(getRenderer().getContext(), mCurrentImageIndex < mSwapChainBuffer.size(), "Out-of-bounds index of the current Vulkan swap chain image to render into");
		return mSwapChainBuffer[mCurrentImageIndex].vkFramebuffer;
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ISwapChain methods           ]
	//[-------------------------------------------------------]
	inline void SwapChain::setRenderWindow(Renderer::IRenderWindow* renderWindow)
	{
		mRenderWindow = renderWindow;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
