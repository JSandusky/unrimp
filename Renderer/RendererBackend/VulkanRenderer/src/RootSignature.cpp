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
#include "VulkanRenderer/RootSignature.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/ResourceGroup.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>

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
	#include <vector>
	#include <memory.h>
PRAGMA_WARNING_POP


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
		mVkPipelineLayout(VK_NULL_HANDLE),
		mVkDescriptorPool(VK_NULL_HANDLE)
	{
		static const uint32_t maxSets = 4242;	// TODO(co) We probably need to get this provided from the outside

		// Copy the parameter data
		const Renderer::Context& context = vulkanRenderer.getContext();
		const uint32_t numberOfRootParameters = mRootSignature.numberOfParameters;
		if (numberOfRootParameters > 0)
		{
			mRootSignature.parameters = RENDERER_MALLOC_TYPED(context, Renderer::RootParameter, numberOfRootParameters);
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
					destinationRootParameter.descriptorTable.descriptorRanges = reinterpret_cast<uintptr_t>(RENDERER_MALLOC_TYPED(context, Renderer::DescriptorRange, numberOfDescriptorRanges));
					memcpy(reinterpret_cast<Renderer::DescriptorRange*>(destinationRootParameter.descriptorTable.descriptorRanges), reinterpret_cast<const Renderer::DescriptorRange*>(sourceRootParameter.descriptorTable.descriptorRanges), sizeof(Renderer::DescriptorRange) * numberOfDescriptorRanges);
				}
			}
		}

		{ // Copy the static sampler data
			const uint32_t numberOfStaticSamplers = mRootSignature.numberOfStaticSamplers;
			if (numberOfStaticSamplers > 0)
			{
				mRootSignature.staticSamplers = RENDERER_MALLOC_TYPED(context, Renderer::StaticSampler, numberOfStaticSamplers);
				memcpy(const_cast<Renderer::StaticSampler*>(mRootSignature.staticSamplers), rootSignature.staticSamplers, sizeof(Renderer::StaticSampler) * numberOfStaticSamplers);
			}
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
					if (vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, vulkanRenderer.getVkAllocationCallbacks(), &mVkDescriptorSetLayouts[rootParameterIndex]) != VK_SUCCESS)
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
			if (vkCreatePipelineLayout(vkDevice, &vkPipelineLayoutCreateInfo, vulkanRenderer.getVkAllocationCallbacks(), &mVkPipelineLayout) != VK_SUCCESS)
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
				if (vkCreateDescriptorPool(vkDevice, &VkDescriptorPoolCreateInfo, vulkanRenderer.getVkAllocationCallbacks(), &mVkDescriptorPool) != VK_SUCCESS)
				{
					RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan descriptor pool")
				}
			}
		}

		SET_DEFAULT_DEBUG_NAME	// setDebugName("");
	}

	RootSignature::~RootSignature()
	{
		const VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
		const VkDevice vkDevice = vulkanRenderer.getVulkanContext().getVkDevice();

		// Destroy the Vulkan descriptor pool
		if (VK_NULL_HANDLE != mVkDescriptorPool)
		{
			vkDestroyDescriptorPool(vkDevice, mVkDescriptorPool, vulkanRenderer.getVkAllocationCallbacks());
		}

		// Destroy the Vulkan pipeline layout
		if (VK_NULL_HANDLE != mVkPipelineLayout)
		{
			vkDestroyPipelineLayout(vkDevice, mVkPipelineLayout, vulkanRenderer.getVkAllocationCallbacks());
		}

		// Destroy the Vulkan descriptor set layout
		for (VkDescriptorSetLayout vkDescriptorSetLayout : mVkDescriptorSetLayouts)
		{
			if (VK_NULL_HANDLE != vkDescriptorSetLayout)
			{
				vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorSetLayout, vulkanRenderer.getVkAllocationCallbacks());
			}
		}

		// Destroy the root signature data
		const Renderer::Context& context = getRenderer().getContext();
		if (nullptr != mRootSignature.parameters)
		{
			for (uint32_t rootParameterIndex = 0; rootParameterIndex < mRootSignature.numberOfParameters; ++rootParameterIndex)
			{
				const Renderer::RootParameter& rootParameter = mRootSignature.parameters[rootParameterIndex];
				if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
				{
					RENDERER_FREE(context, reinterpret_cast<Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges));
				}
			}
			RENDERER_FREE(context, const_cast<Renderer::RootParameter*>(mRootSignature.parameters));
		}
		RENDERER_FREE(context, const_cast<Renderer::StaticSampler*>(mRootSignature.staticSamplers));
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void RootSignature::setDebugName(const char* name)
		{
			if (nullptr != vkDebugMarkerSetObjectNameEXT)
			{
				const VkDevice vkDevice = static_cast<const VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice();
				for (VkDescriptorSetLayout vkDescriptorSetLayout : mVkDescriptorSetLayouts)
				{
					Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT, (uint64_t)vkDescriptorSetLayout, name);
				}
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT, (uint64_t)mVkPipelineLayout, name);
				Helper::setDebugObjectName(vkDevice, VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT, (uint64_t)mVkDescriptorPool, name);
			}
		}
	#endif


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRootSignature methods       ]
	//[-------------------------------------------------------]
	Renderer::IResourceGroup* RootSignature::createResourceGroup(uint32_t rootParameterIndex, uint32_t numberOfResources, Renderer::IResource** resources, Renderer::ISamplerState** samplerStates)
	{
		VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
		const Renderer::Context& context = vulkanRenderer.getContext();

		// Sanity checks
		RENDERER_ASSERT(context, VK_NULL_HANDLE != mVkDescriptorPool, "The Vulkan descriptor pool instance must be valid")
		RENDERER_ASSERT(context, rootParameterIndex < mVkDescriptorSetLayouts.size(), "The Vulkan root parameter index is out-of-bounds")
		RENDERER_ASSERT(context, numberOfResources > 0, "The number of Vulkan resources must not be zero")
		RENDERER_ASSERT(context, nullptr != resources, "The Vulkan resource pointers must be valid")

		// Allocate Vulkan descriptor set
		VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
		if ((*resources)->getResourceType() != Renderer::ResourceType::SAMPLER_STATE)
		{
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
				RENDERER_LOG(context, CRITICAL, "Failed to allocate the Vulkan descriptor set")
			}
		}

		// Create resource group
		return RENDERER_NEW(context, ResourceGroup)(*this, vkDescriptorSet, numberOfResources, resources, samplerStates);
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void RootSignature::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), RootSignature, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
