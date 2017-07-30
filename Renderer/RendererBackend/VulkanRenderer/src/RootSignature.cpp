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
#include "VulkanRenderer/RootSignature.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/ResourceGroup.h"
#include "VulkanRenderer/State/SamplerState.h"

#include <Renderer/ILog.h>

#include <array>
#include <vector>
#include <memory.h>


#ifndef VULKANRENDERER_NO_DEBUG
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
			void checkSamplerState(VulkanRenderer::VulkanRenderer& vulkanRenderer, const Renderer::RootSignature& rootSignature, uint32_t samplerRootParameterIndex)
			{
				if (samplerRootParameterIndex >= rootSignature.numberOfParameters)
				{
					RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Vulkan renderer backend sampler root parameter index is out of bounds")
					return;
				}
				const Renderer::RootParameter& samplerRootParameter = rootSignature.parameters[samplerRootParameterIndex];
				if (Renderer::RootParameterType::DESCRIPTOR_TABLE != samplerRootParameter.parameterType)
				{
					RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Vulkan renderer backend sampler root parameter index doesn't point to a descriptor table")
					return;
				}
				if (Renderer::DescriptorRangeType::SAMPLER != reinterpret_cast<const Renderer::DescriptorRange*>(samplerRootParameter.descriptorTable.descriptorRanges)[0].rangeType)
				{
					RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Vulkan renderer backend sampler root parameter index is out of bounds")
					return;
				}
			}


	//[-------------------------------------------------------]
	//[ Anonymous detail namespace                            ]
	//[-------------------------------------------------------]
		} // detail
	}
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RootSignature::RootSignature(VulkanRenderer& vulkanRenderer, const Renderer::RootSignature& rootSignature) :
		IRootSignature(vulkanRenderer),
		mRootSignature(rootSignature),
		mSamplerStates(nullptr),
		mVkPipelineLayout(VK_NULL_HANDLE),
		mVkDescriptorPool(VK_NULL_HANDLE)
	{
		static const uint32_t maxSets = 42;	// TODO(co) We probably need to get this provided from the outside

		// Copy the parameter data
		const uint32_t numberOfRootParameters = mRootSignature.numberOfParameters;
		if (numberOfRootParameters > 0)
		{
			mRootSignature.parameters = new Renderer::RootParameter[numberOfRootParameters];
			Renderer::RootParameter* destinationRootParameters = const_cast<Renderer::RootParameter*>(mRootSignature.parameters);
			memcpy(destinationRootParameters, rootSignature.parameters, sizeof(Renderer::RootParameter) * numberOfRootParameters);

			// Copy the descriptor table data
			for (uint32_t rootParameterIndex = 0; rootParameterIndex < numberOfRootParameters; ++rootParameterIndex)
			{
				Renderer::RootParameter& destinationRootParameter = destinationRootParameters[rootParameterIndex];
				const Renderer::RootParameter& sourceRootParameter = rootSignature.parameters[rootParameterIndex];
				if (Renderer::RootParameterType::DESCRIPTOR_TABLE == destinationRootParameter.parameterType)
				{
					const uint32_t numberOfDescriptorRanges = destinationRootParameter.descriptorTable.numberOfDescriptorRanges;
					destinationRootParameter.descriptorTable.descriptorRanges = reinterpret_cast<uintptr_t>(new Renderer::DescriptorRange[numberOfDescriptorRanges]);
					memcpy(reinterpret_cast<Renderer::DescriptorRange*>(destinationRootParameter.descriptorTable.descriptorRanges), reinterpret_cast<const Renderer::DescriptorRange*>(sourceRootParameter.descriptorTable.descriptorRanges), sizeof(Renderer::DescriptorRange) * numberOfDescriptorRanges);
				}
			}
		}

		{ // Copy the static sampler data
			const uint32_t numberOfStaticSamplers = mRootSignature.numberOfStaticSamplers;
			if (numberOfStaticSamplers > 0)
			{
				mRootSignature.staticSamplers = new Renderer::StaticSampler[numberOfStaticSamplers];
				memcpy(const_cast<Renderer::StaticSampler*>(mRootSignature.staticSamplers), rootSignature.staticSamplers, sizeof(Renderer::StaticSampler) * numberOfStaticSamplers);
			}
		}

		// Initialize sampler state references
		if (numberOfRootParameters > 0)
		{
			mSamplerStates = new SamplerState*[numberOfRootParameters];
			memset(mSamplerStates, 0, sizeof(SamplerState*) * numberOfRootParameters);
		}

		// Create the Vulkan descriptor set layout
		const VkDevice vkDevice = vulkanRenderer.getVulkanContext().getVkDevice();
		VkDescriptorSetLayouts vkDescriptorSetLayouts;
		uint32_t numberOfCombinedImageSamplers = 0;	// "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER"
		uint32_t numberOfUniformBuffers = 0;		// "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER"
		uint32_t numberOfUniformTexelBuffers = 0;	// "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER"
		if (numberOfRootParameters > 0)
		{
			// Fill the Vulkan descriptor set layout bindings
			vkDescriptorSetLayouts.reserve(numberOfRootParameters);
			mVkDescriptorSetLayouts.resize(numberOfRootParameters);
			std::fill(mVkDescriptorSetLayouts.begin(), mVkDescriptorSetLayouts.end(), static_cast<VkDescriptorSetLayout>(VK_NULL_HANDLE));	// TODO(co) Get rid of this
			typedef std::vector<VkDescriptorSetLayoutBinding> VkDescriptorSetLayoutBindings;
			VkDescriptorSetLayoutBindings vkDescriptorSetLayoutBindings;
			vkDescriptorSetLayoutBindings.reserve(numberOfRootParameters);
			for (uint32_t rootParameterIndex = 0; rootParameterIndex < numberOfRootParameters; ++rootParameterIndex)
			{
				vkDescriptorSetLayoutBindings.clear();

				// TODO(co) For now we only support descriptor tables
				const Renderer::RootParameter& rootParameter = rootSignature.parameters[rootParameterIndex];
				if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
				{
					// Process descriptor ranges
					const Renderer::DescriptorRange* descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges);
					for (uint32_t descriptorRangeIndex = 0; descriptorRangeIndex < rootParameter.descriptorTable.numberOfDescriptorRanges; ++descriptorRangeIndex, ++descriptorRange)
					{
						// Evaluate parameter type
						VkDescriptorType vkDescriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
						switch (descriptorRange->rangeType)
						{
							case Renderer::DescriptorRangeType::SRV:
								vkDescriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
								++numberOfCombinedImageSamplers;
								break;

							case Renderer::DescriptorRangeType::UAV:
								// TODO(co) Usage of "UAV" is just a temporary hack
								// RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Vulkan renderer backend: \"Renderer::DescriptorRangeType::UAV\" is currently no supported descriptor range type")
								vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
								++numberOfUniformTexelBuffers;
								break;

							case Renderer::DescriptorRangeType::UBV:
								vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
								++numberOfUniformBuffers;
								break;

							case Renderer::DescriptorRangeType::SAMPLER:
								// Nothing here due to usage of "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER"
								break;

							case Renderer::DescriptorRangeType::NUMBER_OF_RANGE_TYPES:
								RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Vulkan renderer backend: \"Renderer::DescriptorRangeType::NUMBER_OF_RANGE_TYPES\" is no valid descriptor range type")
								break;
						}

						// Evaluate shader visibility
						VkShaderStageFlags vkShaderStageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
						switch (descriptorRange->shaderVisibility)
						{
							case Renderer::ShaderVisibility::ALL:
								vkShaderStageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
								break;

							case Renderer::ShaderVisibility::VERTEX:
								vkShaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
								break;

							case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
								vkShaderStageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
								break;

							case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
								vkShaderStageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
								break;

							case Renderer::ShaderVisibility::GEOMETRY:
								vkShaderStageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;
								break;

							case Renderer::ShaderVisibility::FRAGMENT:
								vkShaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
								break;
						}

						// Add the Vulkan descriptor set layout binding
						if (VK_DESCRIPTOR_TYPE_MAX_ENUM != vkDescriptorType)
						{
							const VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding =
							{
								descriptorRangeIndex,	// binding (uint32_t)
								vkDescriptorType,		// descriptorType (VkDescriptorType)
								1,						// descriptorCount (uint32_t)
								vkShaderStageFlags,		// stageFlags (VkShaderStageFlags)
								nullptr					// pImmutableSamplers (const VkSampler*)
							};
							vkDescriptorSetLayoutBindings.push_back(vkDescriptorSetLayoutBinding);
						}
					}
				}

				// Create the Vulkan descriptor set layout
				if (!vkDescriptorSetLayoutBindings.empty())
				{
					const VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo =
					{
						VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,			// sType (VkStructureType)
						nullptr,														// pNext (const void*)
						0,																// flags (VkDescriptorSetLayoutCreateFlags)
						static_cast<uint32_t>(vkDescriptorSetLayoutBindings.size()),	// bindingCount (uint32_t)
						vkDescriptorSetLayoutBindings.data()							// pBindings (const VkDescriptorSetLayoutBinding*)
					};
					if (vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, nullptr, &mVkDescriptorSetLayouts[rootParameterIndex]) != VK_SUCCESS)
					{
						RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan descriptor set layout")
					}
					vkDescriptorSetLayouts.push_back(mVkDescriptorSetLayouts[rootParameterIndex]);
				}
			}
		}

		{ // Create the Vulkan pipeline layout
			const VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo =
			{
				VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,								// sType (VkStructureType)
				nullptr,																	// pNext (const void*)
				0,																			// flags (VkPipelineLayoutCreateFlags)
				static_cast<uint32_t>(vkDescriptorSetLayouts.size()),						// setLayoutCount (uint32_t)
				vkDescriptorSetLayouts.empty() ? nullptr : vkDescriptorSetLayouts.data(),	// pSetLayouts (const VkDescriptorSetLayout*)
				0,																			// pushConstantRangeCount (uint32_t)
				nullptr																		// pPushConstantRanges (const VkPushConstantRange*)
			};
			if (vkCreatePipelineLayout(vkDevice, &vkPipelineLayoutCreateInfo, nullptr, &mVkPipelineLayout) != VK_SUCCESS)
			{
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan pipeline layout")
			}
		}

		{ // Create the Vulkan descriptor pool
			typedef std::array<VkDescriptorPoolSize, 3> VkDescriptorPoolSizes;
			VkDescriptorPoolSizes vkDescriptorPoolSizes;
			uint32_t numberOfVkDescriptorPoolSizes = 0;

			// "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER"
			if (numberOfCombinedImageSamplers > 0)
			{
				VkDescriptorPoolSize& vkDescriptorPoolSize = vkDescriptorPoolSizes[numberOfVkDescriptorPoolSizes];
				vkDescriptorPoolSize.type			 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;	// type (VkDescriptorType)
				vkDescriptorPoolSize.descriptorCount = maxSets * numberOfCombinedImageSamplers;		// descriptorCount (uint32_t)
				++numberOfVkDescriptorPoolSizes;
			}

			// "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER"
			if (numberOfUniformBuffers > 0)
			{
				VkDescriptorPoolSize& vkDescriptorPoolSize = vkDescriptorPoolSizes[numberOfVkDescriptorPoolSizes];
				vkDescriptorPoolSize.type			 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;	// type (VkDescriptorType)
				vkDescriptorPoolSize.descriptorCount = maxSets * numberOfUniformBuffers;	// descriptorCount (uint32_t)
				++numberOfVkDescriptorPoolSizes;
			}

			// "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER"
			if (numberOfUniformTexelBuffers > 0)
			{
				VkDescriptorPoolSize& vkDescriptorPoolSize = vkDescriptorPoolSizes[numberOfVkDescriptorPoolSizes];
				vkDescriptorPoolSize.type			 = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;	// type (VkDescriptorType)
				vkDescriptorPoolSize.descriptorCount = maxSets * numberOfUniformTexelBuffers;	// descriptorCount (uint32_t)
				++numberOfVkDescriptorPoolSizes;
			}

			// Create the Vulkan descriptor pool
			if (numberOfVkDescriptorPoolSizes > 0)
			{
				const VkDescriptorPoolCreateInfo VkDescriptorPoolCreateInfo =
				{
					VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,		// sType (VkStructureType)
					nullptr,											// pNext (const void*)
					VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,	// flags (VkDescriptorPoolCreateFlags)
					maxSets,											// maxSets (uint32_t)
					numberOfVkDescriptorPoolSizes,						// poolSizeCount (uint32_t)
					vkDescriptorPoolSizes.data()						// pPoolSizes (const VkDescriptorPoolSize*)
				};
				if (vkCreateDescriptorPool(vkDevice, &VkDescriptorPoolCreateInfo, nullptr, &mVkDescriptorPool) != VK_SUCCESS)
				{
					RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan descriptor pool")
				}
			}
		}
	}

	RootSignature::~RootSignature()
	{
		const VkDevice vkDevice = static_cast<VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice();

		// Destroy the Vulkan descriptor pool
		if (VK_NULL_HANDLE != mVkDescriptorPool)
		{
			vkDestroyDescriptorPool(vkDevice, mVkDescriptorPool, nullptr);
		}

		// Destroy the Vulkan pipeline layout
		if (VK_NULL_HANDLE != mVkPipelineLayout)
		{
			vkDestroyPipelineLayout(vkDevice, mVkPipelineLayout, nullptr);
		}

		// Destroy the Vulkan descriptor set layout
		for (VkDescriptorSetLayout vkDescriptorSetLayout : mVkDescriptorSetLayouts)
		{
			if (VK_NULL_HANDLE != vkDescriptorSetLayout)
			{
				vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorSetLayout, nullptr);
			}
		}

		// Release all sampler state references
		if (nullptr != mSamplerStates)
		{
			for (uint32_t rootParameterIndex = 0; rootParameterIndex < mRootSignature.numberOfParameters; ++rootParameterIndex)
			{
				SamplerState* samplerState = mSamplerStates[rootParameterIndex];
				if (nullptr != samplerState)
				{
					samplerState->releaseReference();
				}
			}
			delete [] mSamplerStates;
		}

		// Destroy the root signature data
		if (nullptr != mRootSignature.parameters)
		{
			for (uint32_t rootParameterIndex = 0; rootParameterIndex < mRootSignature.numberOfParameters; ++rootParameterIndex)
			{
				const Renderer::RootParameter& rootParameter = mRootSignature.parameters[rootParameterIndex];
				if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
				{
					delete [] reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges);
				}
			}
			delete [] mRootSignature.parameters;
		}
		delete [] mRootSignature.staticSamplers;
	}

	const SamplerState* RootSignature::getSamplerState(uint32_t samplerRootParameterIndex) const
	{
		// Security checks
		#ifndef VULKANRENDERER_NO_DEBUG
			VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
			detail::checkSamplerState(vulkanRenderer, mRootSignature, samplerRootParameterIndex);
			if (nullptr == mSamplerStates[samplerRootParameterIndex])
			{
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Vulkan renderer backend sampler root parameter index points to no sampler state instance")
				return nullptr;
			}
		#endif
		return mSamplerStates[samplerRootParameterIndex];
	}

	void RootSignature::setSamplerState(uint32_t samplerRootParameterIndex, SamplerState* samplerState) const
	{
		// Security checks
		#ifndef VULKANRENDERER_NO_DEBUG
			detail::checkSamplerState(static_cast<VulkanRenderer&>(getRenderer()), mRootSignature, samplerRootParameterIndex);
		#endif

		// Set sampler state
		SamplerState** samplerStateSlot = &mSamplerStates[samplerRootParameterIndex];
		if (samplerState != *samplerStateSlot)
		{
			if (nullptr != *samplerStateSlot)
			{
				(*samplerStateSlot)->releaseReference();
			}
			(*samplerStateSlot) = samplerState;
			if (nullptr != *samplerStateSlot)
			{
				(*samplerStateSlot)->addReference();
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRootSignature methods       ]
	//[-------------------------------------------------------]
	Renderer::IResourceGroup* RootSignature::createResourceGroup(uint32_t rootParameterIndex, uint32_t numberOfResources, Renderer::IResource** resources)
	{
		// Sanity checks
		assert(VK_NULL_HANDLE != mVkDescriptorPool && "The Vulkan descriptor pool instance must be valid");
		assert(rootParameterIndex < mVkDescriptorSetLayouts.size() && "The root parameter index is out-of-bounds");
		assert(numberOfResources > 0 && "The number of resources must not be zero");
		assert(nullptr != resources && "The resource pointers must be valid");

		// Allocate Vulkan descriptor set
		VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
		if ((*resources)->getResourceType() != Renderer::ResourceType::SAMPLER_STATE)
		{
			VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
			const VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo =
			{
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,	// sType (VkStructureType)
				nullptr,										// pNext (const void*)
				mVkDescriptorPool,								// descriptorPool (VkDescriptorPool)
				1,												// descriptorSetCount (uint32_t)
				&mVkDescriptorSetLayouts[rootParameterIndex]	// pSetLayouts (const VkDescriptorSetLayout*)
			};
			if (vkAllocateDescriptorSets(vulkanRenderer.getVulkanContext().getVkDevice(), &vkDescriptorSetAllocateInfo, &vkDescriptorSet) != VK_SUCCESS)
			{
				RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to allocate the Vulkan descriptor set")
			}
		}

		// Create resource group
		return new ResourceGroup(*this, rootParameterIndex, vkDescriptorSet, numberOfResources, resources);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
