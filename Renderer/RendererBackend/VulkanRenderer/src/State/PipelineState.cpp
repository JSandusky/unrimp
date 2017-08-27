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
#include "VulkanRenderer/State/PipelineState.h"
#include "VulkanRenderer/RenderTarget/RenderPass.h"
#include "VulkanRenderer/Shader/ProgramGlsl.h"
#include "VulkanRenderer/Shader/VertexShaderGlsl.h"
#include "VulkanRenderer/Shader/GeometryShaderGlsl.h"
#include "VulkanRenderer/Shader/FragmentShaderGlsl.h"
#include "VulkanRenderer/Shader/TessellationControlShaderGlsl.h"
#include "VulkanRenderer/Shader/TessellationEvaluationShaderGlsl.h"
#include "VulkanRenderer/RootSignature.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/Mapping.h"

#include <Renderer/ILog.h>

#include <array>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		typedef std::array<VkPipelineShaderStageCreateInfo, 5> VkPipelineShaderStageCreateInfos;


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void addVkPipelineShaderStageCreateInfo(VkShaderStageFlagBits vkShaderStageFlagBits, VkShaderModule vkShaderModule, VkPipelineShaderStageCreateInfos& vkPipelineShaderStageCreateInfos, uint32_t stageCount)
		{
			VkPipelineShaderStageCreateInfo& vkPipelineShaderStageCreateInfo = vkPipelineShaderStageCreateInfos[stageCount];
			vkPipelineShaderStageCreateInfo.sType				= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;	// sType (VkStructureType)
			vkPipelineShaderStageCreateInfo.pNext				= nullptr;												// pNext (const void*)
			vkPipelineShaderStageCreateInfo.flags				= 0;													// flags (VkPipelineShaderStageCreateFlags)
			vkPipelineShaderStageCreateInfo.stage				= vkShaderStageFlagBits;								// stage (VkShaderStageFlagBits)
			vkPipelineShaderStageCreateInfo.module				= vkShaderModule;										// module (VkShaderModule)
			vkPipelineShaderStageCreateInfo.pName				= "main";												// pName (const char*)
			vkPipelineShaderStageCreateInfo.pSpecializationInfo	= nullptr;												// pSpecializationInfo (const VkSpecializationInfo*)
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	PipelineState::PipelineState(VulkanRenderer& vulkanRenderer, const Renderer::PipelineState& pipelineState) :
		IPipelineState(vulkanRenderer),
		mProgram(pipelineState.program),
		mVkPipeline(VK_NULL_HANDLE)
	{
		// Add a reference to the given program
		mProgram->addReference();

		// Sanity checks
		assert(nullptr != pipelineState.rootSignature);
		assert(nullptr != pipelineState.renderPass);

		// Our pipeline state needs to be independent of concrete render targets, so we're using dynamic viewport ("VK_DYNAMIC_STATE_VIEWPORT") and scissor ("VK_DYNAMIC_STATE_SCISSOR") states
		const uint32_t width  = 42;
		const uint32_t height = 42;

		// Shaders
		ProgramGlsl* programGlsl = static_cast<ProgramGlsl*>(mProgram);
		uint32_t stageCount = 0;
		::detail::VkPipelineShaderStageCreateInfos vkPipelineShaderStageCreateInfos;
		{
			// Define helper macro
			#define SHADER_STAGE(vkShaderStageFlagBits, shaderGlsl) \
				if (nullptr != shaderGlsl) \
				{ \
					::detail::addVkPipelineShaderStageCreateInfo(vkShaderStageFlagBits, shaderGlsl->getVkShaderModule(), vkPipelineShaderStageCreateInfos, stageCount); \
					++stageCount; \
				}

			// Shader stages
			SHADER_STAGE(VK_SHADER_STAGE_VERTEX_BIT,				  programGlsl->getVertexShaderGlsl())
			SHADER_STAGE(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,	  programGlsl->getTessellationControlShaderGlsl())
			SHADER_STAGE(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, programGlsl->getTessellationEvaluationShaderGlsl())
			SHADER_STAGE(VK_SHADER_STAGE_GEOMETRY_BIT,				  programGlsl->getGeometryShaderGlsl())
			SHADER_STAGE(VK_SHADER_STAGE_FRAGMENT_BIT,				  programGlsl->getFragmentShaderGlsl())

			// Undefine helper macro
			#undef SHADER_STAGE
		}

		// Vertex attributes
		const uint32_t numberOfAttributes = pipelineState.vertexAttributes.numberOfAttributes;
		std::vector<VkVertexInputBindingDescription> vkVertexInputBindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> vkVertexInputAttributeDescriptions(numberOfAttributes);
		for (uint32_t attribute = 0; attribute < numberOfAttributes; ++attribute)
		{
			const Renderer::VertexAttribute* attributes = &pipelineState.vertexAttributes.attributes[attribute];
			const uint32_t inputSlot = attributes->inputSlot;

			{ // Map to Vulkan vertex input binding description
				if (vkVertexInputBindingDescriptions.size() <= inputSlot)
				{
					vkVertexInputBindingDescriptions.resize(inputSlot + 1);
				}
				VkVertexInputBindingDescription& vkVertexInputBindingDescription = vkVertexInputBindingDescriptions[inputSlot];
				vkVertexInputBindingDescription.binding   = inputSlot;
				vkVertexInputBindingDescription.stride    = attributes->strideInBytes;
				vkVertexInputBindingDescription.inputRate = (attributes->instancesPerElement > 0) ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
			}

			{ // Map to Vulkan vertex input attribute description
				VkVertexInputAttributeDescription& vkVertexInputAttributeDescription = vkVertexInputAttributeDescriptions[attribute];
				vkVertexInputAttributeDescription.location = attribute;
				vkVertexInputAttributeDescription.binding  = inputSlot;
				vkVertexInputAttributeDescription.format   = Mapping::getVulkanFormat(attributes->vertexAttributeFormat);
				vkVertexInputAttributeDescription.offset   = attributes->alignedByteOffset;
			}
		}

		// Create the Vulkan graphics pipeline
		// TODO(co) Implement the rest of the value mappings
		const VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,			// sType (VkStructureType)
			nullptr,															// pNext (const void*)
			0,																	// flags (VkPipelineVertexInputStateCreateFlags)
			static_cast<uint32_t>(vkVertexInputBindingDescriptions.size()),		// vertexBindingDescriptionCount (uint32_t)
			vkVertexInputBindingDescriptions.data(),							// pVertexBindingDescriptions (const VkVertexInputBindingDescription*)
			static_cast<uint32_t>(vkVertexInputAttributeDescriptions.size()),	// vertexAttributeDescriptionCount (uint32_t)
			vkVertexInputAttributeDescriptions.data()							// pVertexAttributeDescriptions (const VkVertexInputAttributeDescription*)
		};
		const VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,	// sType (VkStructureType)
			nullptr,														// pNext (const void*)
			0,																// flags (VkPipelineInputAssemblyStateCreateFlags)
			Mapping::getVulkanType(pipelineState.primitiveTopology),		// topology (VkPrimitiveTopology)
			VK_FALSE														// primitiveRestartEnable (VkBool32)
		};
		const VkViewport vkViewport =
		{
			0.0f,						// x (float)
			0.0f,						// y (float)
			static_cast<float>(width),	// width (float)
			static_cast<float>(height),	// height (float)
			0.0f,						// minDepth (float)
			1.0f						// maxDepth (float)
		};
		const VkRect2D scissorVkRect2D =
		{
			{ // offset (VkOffset2D)
				0,	// x (int32_t)
				0	// y (int32_t)
			},
			{ // extent (VkExtent2D)
				width,	// width (uint32_t)
				height	// height (uint32_t)
			}
		};
		const VkPipelineTessellationStateCreateInfo vkPipelineTessellationStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,																																							// sType (VkStructureType)
			nullptr,																																																			// pNext (const void*)
			0,																																																					// flags (VkPipelineTessellationStateCreateFlags)
			(pipelineState.primitiveTopology >= Renderer::PrimitiveTopology::PATCH_LIST_1) ? static_cast<uint32_t>(pipelineState.primitiveTopology) - static_cast<uint32_t>(Renderer::PrimitiveTopology::PATCH_LIST_1) + 1 : 1	// patchControlPoints (uint32_t)
		};
		const VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,	// sType (VkStructureType)
			nullptr,												// pNext (const void*)
			0,														// flags (VkPipelineViewportStateCreateFlags)
			1,														// viewportCount (uint32_t)
			&vkViewport,											// pViewports (const VkViewport*)
			1,														// scissorCount (uint32_t)
			&scissorVkRect2D										// pScissors (const VkRect2D*)
		};
		const float depthBias = static_cast<float>(pipelineState.rasterizerState.depthBias);
		const float depthBiasClamp = pipelineState.rasterizerState.depthBiasClamp;
		const float slopeScaledDepthBias = pipelineState.rasterizerState.slopeScaledDepthBias;
		const VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,																	// sType (VkStructureType)
			nullptr,																													// pNext (const void*)
			0,																															// flags (VkPipelineRasterizationStateCreateFlags)
			static_cast<VkBool32>(pipelineState.rasterizerState.depthClipEnable),														// depthClampEnable (VkBool32)
			VK_FALSE,																													// rasterizerDiscardEnable (VkBool32)
			(Renderer::FillMode::WIREFRAME == pipelineState.rasterizerState.fillMode) ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL,	// polygonMode (VkPolygonMode)
			static_cast<VkCullModeFlags>(static_cast<int>(pipelineState.rasterizerState.cullMode) - 1),									// cullMode (VkCullModeFlags)
			(1 == pipelineState.rasterizerState.frontCounterClockwise) ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE,		// frontFace (VkFrontFace)
			static_cast<VkBool32>(0.0f != depthBias || 0.0f != depthBiasClamp || 0.0f != slopeScaledDepthBias),							// depthBiasEnable (VkBool32)
			depthBias,																													// depthBiasConstantFactor (float)
			depthBiasClamp,																												// depthBiasClamp (float)
			slopeScaledDepthBias,																										// depthBiasSlopeFactor (float)
			1.0f																														// lineWidth (float)
		};
		const VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,	// sType (VkStructureType)
			nullptr,													// pNext (const void*)
			0,															// flags (VkPipelineMultisampleStateCreateFlags)
			VK_SAMPLE_COUNT_1_BIT,										// rasterizationSamples (VkSampleCountFlagBits)
			VK_FALSE,													// sampleShadingEnable (VkBool32)
			0.0f,														// minSampleShading (float)
			nullptr,													// pSampleMask (const VkSampleMask*)
			VK_FALSE,													// alphaToCoverageEnable (VkBool32)
			VK_FALSE													// alphaToOneEnable (VkBool32)
		};
		const VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,												// sType (VkStructureType)
			nullptr,																								// pNext (const void*)
			0,																										// flags (VkPipelineDepthStencilStateCreateFlags)
			static_cast<VkBool32>(0 != pipelineState.depthStencilState.depthEnable),								// depthTestEnable (VkBool32)
			static_cast<VkBool32>(Renderer::DepthWriteMask::ALL == pipelineState.depthStencilState.depthWriteMask),	// depthWriteEnable (VkBool32)
			Mapping::getVulkanComparisonFunc(pipelineState.depthStencilState.depthFunc),							// depthCompareOp (VkCompareOp)
			VK_FALSE,																								// depthBoundsTestEnable (VkBool32)
			static_cast<VkBool32>(0 != pipelineState.depthStencilState.stencilEnable),								// stencilTestEnable (VkBool32)
			{ // front (VkStencilOpState)
				VK_STENCIL_OP_KEEP,																					// failOp (VkStencilOp)
				VK_STENCIL_OP_KEEP,																					// passOp (VkStencilOp)
				VK_STENCIL_OP_KEEP,																					// depthFailOp (VkStencilOp)
				VK_COMPARE_OP_NEVER,																				// compareOp (VkCompareOp)
				0,																									// compareMask (uint32_t)
				0,																									// writeMask (uint32_t)
				0																									// reference (uint32_t)
			},
			{ // back (VkStencilOpState)
				VK_STENCIL_OP_KEEP,																					// failOp (VkStencilOp)
				VK_STENCIL_OP_KEEP,																					// passOp (VkStencilOp)
				VK_STENCIL_OP_KEEP,																					// depthFailOp (VkStencilOp)
				VK_COMPARE_OP_NEVER,																				// compareOp (VkCompareOp)
				0,																									// compareMask (uint32_t)
				0,																									// writeMask (uint32_t)
				0																									// reference (uint32_t)
			},
			0.0f,																									// minDepthBounds (float)
			1.0f																									// maxDepthBounds (float)
		};
		const RenderPass* renderPass = static_cast<const RenderPass*>(pipelineState.renderPass);
		const uint32_t numberOfColorAttachments = renderPass->getNumberOfColorAttachments();
		assert(numberOfColorAttachments < 8);
		assert(numberOfColorAttachments == pipelineState.numberOfRenderTargets);
		std::array<VkPipelineColorBlendAttachmentState, 8> vkPipelineColorBlendAttachmentStates;
		for (uint8_t i = 0; i < numberOfColorAttachments; ++i)
		{
			const Renderer::RenderTargetBlendDesc& renderTargetBlendDesc = pipelineState.blendState.renderTarget[i];
			VkPipelineColorBlendAttachmentState& vkPipelineColorBlendAttachmentState = vkPipelineColorBlendAttachmentStates[i];
			vkPipelineColorBlendAttachmentState.blendEnable			= static_cast<VkBool32>(renderTargetBlendDesc.blendEnable);				// blendEnable (VkBool32)
			vkPipelineColorBlendAttachmentState.srcColorBlendFactor	= Mapping::getVulkanBlendFactor(renderTargetBlendDesc.srcBlend);		// srcColorBlendFactor (VkBlendFactor)
			vkPipelineColorBlendAttachmentState.dstColorBlendFactor	= Mapping::getVulkanBlendFactor(renderTargetBlendDesc.destBlend);		// dstColorBlendFactor (VkBlendFactor)
			vkPipelineColorBlendAttachmentState.colorBlendOp		= Mapping::getVulkanBlendOp(renderTargetBlendDesc.blendOp);				// colorBlendOp (VkBlendOp)
			vkPipelineColorBlendAttachmentState.srcAlphaBlendFactor	= Mapping::getVulkanBlendFactor(renderTargetBlendDesc.srcBlendAlpha);	// srcAlphaBlendFactor (VkBlendFactor)
			vkPipelineColorBlendAttachmentState.dstAlphaBlendFactor	= Mapping::getVulkanBlendFactor(renderTargetBlendDesc.destBlendAlpha);	// dstAlphaBlendFactor (VkBlendFactor)
			vkPipelineColorBlendAttachmentState.alphaBlendOp		= Mapping::getVulkanBlendOp(renderTargetBlendDesc.blendOpAlpha);		// alphaBlendOp (VkBlendOp)
			vkPipelineColorBlendAttachmentState.colorWriteMask		= renderTargetBlendDesc.renderTargetWriteMask;							// colorWriteMask (VkColorComponentFlags)
		}
		const VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,	// sType (VkStructureType)
			nullptr,													// pNext (const void*)
			0,															// flags (VkPipelineColorBlendStateCreateFlags)
			VK_FALSE,													// logicOpEnable (VkBool32)
			VK_LOGIC_OP_COPY,											// logicOp (VkLogicOp)
			numberOfColorAttachments,									// attachmentCount (uint32_t)
			vkPipelineColorBlendAttachmentStates.data(),				// pAttachments (const VkPipelineColorBlendAttachmentState*)
			{ 0.0f, 0.0f, 0.0f, 0.0f }									// blendConstants[4] (float)
		};
		const std::array<VkDynamicState, 2> vkDynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		const VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,	// sType (VkStructureType)
			nullptr,												// pNext (const void*)
			0,														// flags (VkPipelineDynamicStateCreateFlags)
			static_cast<uint32_t>(vkDynamicStates.size()),			// dynamicStateCount (uint32_t)
			vkDynamicStates.data()									// pDynamicStates (const VkDynamicState*)
		};
		const VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo =
		{
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,										// sType (VkStructureType)
			nullptr,																				// pNext (const void*)
			0,																						// flags (VkPipelineCreateFlags)
			stageCount,																				// stageCount (uint32_t)
			vkPipelineShaderStageCreateInfos.data(),												// pStages (const VkPipelineShaderStageCreateInfo*)
			&vkPipelineVertexInputStateCreateInfo,													// pVertexInputState (const VkPipelineVertexInputStateCreateInfo*)
			&vkPipelineInputAssemblyStateCreateInfo,												// pInputAssemblyState (const VkPipelineInputAssemblyStateCreateInfo*)
			&vkPipelineTessellationStateCreateInfo,													// pTessellationState (const VkPipelineTessellationStateCreateInfo*)
			&vkPipelineViewportStateCreateInfo,														// pViewportState (const VkPipelineViewportStateCreateInfo*)
			&vkPipelineRasterizationStateCreateInfo,												// pRasterizationState (const VkPipelineRasterizationStateCreateInfo*)
			&vkPipelineMultisampleStateCreateInfo,													// pMultisampleState (const VkPipelineMultisampleStateCreateInfo*)
			&vkPipelineDepthStencilStateCreateInfo,													// pDepthStencilState (const VkPipelineDepthStencilStateCreateInfo*)
			&vkPipelineColorBlendStateCreateInfo,													// pColorBlendState (const VkPipelineColorBlendStateCreateInfo*)
			&vkPipelineDynamicStateCreateInfo,														// pDynamicState (const VkPipelineDynamicStateCreateInfo*)
			static_cast<const RootSignature*>(pipelineState.rootSignature)->getVkPipelineLayout(),	// layout (VkPipelineLayout)
			renderPass->getVkRenderPass(),															// renderPass (VkRenderPass)
			0,																						// subpass (uint32_t)
			VK_NULL_HANDLE,																			// basePipelineHandle (VkPipeline)
			0																						// basePipelineIndex (int32_t)
		};
		if (vkCreateGraphicsPipelines(vulkanRenderer.getVulkanContext().getVkDevice(), VK_NULL_HANDLE, 1, &vkGraphicsPipelineCreateInfo, nullptr, &mVkPipeline) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan graphics pipeline")
		}
		else
		{
			SET_DEFAULT_DEBUG_NAME	// setDebugName("");
		}
	}

	PipelineState::~PipelineState()
	{
		// Destroy the Vulkan graphics pipeline
		if (VK_NULL_HANDLE != mVkPipeline)
		{
			vkDestroyPipeline(static_cast<VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(), mVkPipeline, nullptr);
		}

		// Release the program reference
		if (nullptr != mProgram)
		{
			mProgram->releaseReference();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifndef VULKANRENDERER_NO_DEBUG
		void PipelineState::setDebugName(const char* name)
		{
			if (nullptr != vkDebugMarkerSetObjectNameEXT)
			{
				Helper::setDebugObjectName(static_cast<const VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(), VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT, mVkPipeline, name);
			}
		}
	#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
