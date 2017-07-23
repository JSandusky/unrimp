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
#include "VulkanRenderer/Texture/Texture3D.h"
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
	Texture3D::Texture3D(VulkanRenderer& vulkanRenderer, uint32_t width, uint32_t height, uint32_t depth, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags) :
		ITexture3D(vulkanRenderer, width, height, depth),
		mVkImage(VK_NULL_HANDLE),
		mVkDeviceMemory(VK_NULL_HANDLE),
		mVkImageView(VK_NULL_HANDLE)
	{
		Helper::createAndFillVkImage(vulkanRenderer, VK_IMAGE_TYPE_3D, VK_IMAGE_VIEW_TYPE_3D, { width, height, depth }, textureFormat, data, flags, mVkImage, mVkDeviceMemory, mVkImageView);
	}

	Texture3D::~Texture3D()
	{
		Helper::destroyAndFreeVkImage(static_cast<VulkanRenderer&>(getRenderer()), mVkImage, mVkDeviceMemory, mVkImageView);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITexture3D methods           ]
	//[-------------------------------------------------------]
	void Texture3D::copyDataFrom(uint32_t, const void*)
	{
		// TODO(co) Implement me
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
