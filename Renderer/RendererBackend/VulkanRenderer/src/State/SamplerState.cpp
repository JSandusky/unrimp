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
#include "VulkanRenderer/State/SamplerState.h"
#include "VulkanRenderer/VulkanRuntimeLinking.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"

#include <Renderer/ILog.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SamplerState::SamplerState(VulkanRenderer& vulkanRenderer, const Renderer::SamplerState& samplerState) :
		ISamplerState(vulkanRenderer),
		mVkSampler(VK_NULL_HANDLE)
	{
		// Sanity checks
		assert(samplerState.filter != Renderer::FilterMode::UNKNOWN && "Filter mode must not be unknown");
		assert(samplerState.maxAnisotropy <= vulkanRenderer.getCapabilities().maximumAnisotropy && "Maximum anisotropy value violated");

		// TODO(co) Map "Renderer::SamplerState" to VkSamplerCreateInfo
		const VkSamplerCreateInfo vkSamplerCreateInfo =
		{
			VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,		// sType (VkStructureType)
			nullptr,									// pNext (const void*)
			0,											// flags (VkSamplerCreateFlags)
			VK_FILTER_LINEAR,							// magFilter (VkFilter)
			VK_FILTER_LINEAR,							// minFilter (VkFilter)
			VK_SAMPLER_MIPMAP_MODE_NEAREST,				// mipmapMode (VkSamplerMipmapMode)
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,		// addressModeU (VkSamplerAddressMode)
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,		// addressModeV (VkSamplerAddressMode)
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,		// addressModeW (VkSamplerAddressMode)
			samplerState.mipLODBias,					// mipLodBias (float)
			VK_FALSE,									// anisotropyEnable (VkBool32)
			1.0f,										// maxAnisotropy (float)
			VK_FALSE,									// compareEnable (VkBool32)
			VK_COMPARE_OP_ALWAYS,						// compareOp (VkCompareOp)
			0.0f,										// minLod (float)
			0.0f,										// maxLod (float)
			VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,	// borderColor (VkBorderColor)
			VK_FALSE									// unnormalizedCoordinates (VkBool32)
		};
		if (vkCreateSampler(vulkanRenderer.getVulkanContext().getVkDevice(), &vkSamplerCreateInfo, nullptr, &mVkSampler) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create Vulkan sampler instance")
		}
	}

	SamplerState::~SamplerState()
	{
		if (VK_NULL_HANDLE != mVkSampler)
		{
			vkDestroySampler(static_cast<VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(), mVkSampler, nullptr);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
