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
#include "VulkanRenderer/Texture/Texture1D.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/Helper.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture1D::Texture1D(VulkanRenderer& vulkanRenderer, uint32_t width, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t) :
		ITexture1D(vulkanRenderer, width),
		mVkImage(VK_NULL_HANDLE),
		mVkDeviceMemory(VK_NULL_HANDLE),
		mVkImageView(VK_NULL_HANDLE)
	{
		// TODO(co) Add support for "flags"
		Helper::createAndFillVkImage(vulkanRenderer, VK_IMAGE_TYPE_1D, VK_IMAGE_VIEW_TYPE_1D, { width, 1, 1 }, textureFormat, data, mVkImage, mVkDeviceMemory, mVkImageView);
	}

	Texture1D::~Texture1D()
	{
		Helper::destroyAndFreeVkImage(static_cast<VulkanRenderer&>(getRenderer()), mVkImage, mVkDeviceMemory, mVkImageView);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
