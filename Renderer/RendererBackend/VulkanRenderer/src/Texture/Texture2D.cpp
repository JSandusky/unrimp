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
#include "VulkanRenderer/Texture/Texture2D.h"
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
	Texture2D::Texture2D(VulkanRenderer& vulkanRenderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, uint8_t numberOfMultisamples) :
		ITexture2D(vulkanRenderer, width, height),
		mVrVulkanTextureData{},
		mVkImageLayout((flags & Renderer::TextureFlag::RENDER_TARGET) ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_PREINITIALIZED),
		mVkDeviceMemory(VK_NULL_HANDLE),
		mVkImageView(VK_NULL_HANDLE)
	{
		mVrVulkanTextureData.m_nFormat = Helper::createAndFillVkImage(vulkanRenderer, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, { width, height, 1 }, textureFormat, data, flags, numberOfMultisamples, mVrVulkanTextureData.m_nImage, mVkDeviceMemory, mVkImageView);
		SET_DEFAULT_DEBUG_NAME	// setDebugName("");

		// Fill the rest of the "VRVulkanTextureData_t"-structure
		const VulkanContext& vulkanContext = vulkanRenderer.getVulkanContext();
		const VulkanRuntimeLinking& vulkanRuntimeLinking = vulkanRenderer.getVulkanRuntimeLinking();
																								// m_nImage (VkImage) was set by "VulkanRenderer::Helper::createAndFillVkImage()" above
		mVrVulkanTextureData.m_pDevice			 = vulkanContext.getVkDevice();					// m_pDevice (VkDevice)
		mVrVulkanTextureData.m_pPhysicalDevice	 = vulkanContext.getVkPhysicalDevice();			// m_pPhysicalDevice (VkPhysicalDevice)
		mVrVulkanTextureData.m_pInstance		 = vulkanRuntimeLinking.getVkInstance();		// m_pInstance (VkInstance)
		mVrVulkanTextureData.m_pQueue			 = vulkanContext.getGraphicsVkQueue();			// m_pQueue (VkQueue)
		mVrVulkanTextureData.m_nQueueFamilyIndex = vulkanContext.getGraphicsQueueFamilyIndex();	// m_nQueueFamilyIndex (uint32_t)
		mVrVulkanTextureData.m_nWidth			 = width;										// m_nWidth (uint32_t)
		mVrVulkanTextureData.m_nHeight			 = height;										// m_nHeight (uint32_t)
																								// m_nFormat (VkFormat)  was set by "VulkanRenderer::Helper::createAndFillVkImage()" above
		mVrVulkanTextureData.m_nSampleCount		 = numberOfMultisamples;						// m_nSampleCount (uint32_t)
	}

	Texture2D::~Texture2D()
	{
		Helper::destroyAndFreeVkImage(static_cast<VulkanRenderer&>(getRenderer()), mVrVulkanTextureData.m_nImage, mVkDeviceMemory, mVkImageView);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void Texture2D::setDebugName(const char* name)
		{
			if (nullptr != vkDebugMarkerSetObjectNameEXT)
			{
				const VkDevice vkDevice = static_cast<const VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice();
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, (uint64_t)mVrVulkanTextureData.m_nImage, name);
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT, (uint64_t)mVkDeviceMemory, name);
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT, (uint64_t)mVkImageView, name);
			}
		}
	#endif


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void Texture2D::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), Texture2D, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
