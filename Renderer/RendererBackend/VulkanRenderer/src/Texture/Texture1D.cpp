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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "VulkanRenderer/Texture/Texture1D.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/Helper.h"

#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture1D::Texture1D(VulkanRenderer& vulkanRenderer, uint32_t width, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags) :
		ITexture1D(vulkanRenderer, width),
		mVkImage(VK_NULL_HANDLE),
		mVkImageLayout((flags & Renderer::TextureFlag::RENDER_TARGET) ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_PREINITIALIZED),
		mVkDeviceMemory(VK_NULL_HANDLE),
		mVkImageView(VK_NULL_HANDLE)
	{
		Helper::createAndFillVkImage(vulkanRenderer, VK_IMAGE_TYPE_1D, VK_IMAGE_VIEW_TYPE_1D, { width, 1, 1 }, textureFormat, data, flags, 1, mVkImage, mVkDeviceMemory, mVkImageView);
		SET_DEFAULT_DEBUG_NAME	// setDebugName("");
	}

	Texture1D::~Texture1D()
	{
		Helper::destroyAndFreeVkImage(static_cast<VulkanRenderer&>(getRenderer()), mVkImage, mVkDeviceMemory, mVkImageView);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	DEFINE_SET_DEBUG_NAME_TEXTURE(Texture1D)	// void Texture1D::setDebugName(const char* name)


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void Texture1D::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), Texture1D, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
