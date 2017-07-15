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
#include "VulkanRenderer/State/BlendState.h"
#include "VulkanRenderer/State/RasterizerState.h"
#include "VulkanRenderer/State/DepthStencilState.h"
#include "VulkanRenderer/RenderTarget/SwapChain.h"
#include "VulkanRenderer/Shader/ProgramGlsl.h"
#include "VulkanRenderer/Shader/VertexShaderGlsl.h"
#include "VulkanRenderer/Shader/FragmentShaderGlsl.h"
#include "VulkanRenderer/RootSignature.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"

#include <array>


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
		mRasterizerState(new RasterizerState(pipelineState.rasterizerState)),
		mDepthStencilState(new DepthStencilState(pipelineState.depthStencilState)),
		mBlendState(new BlendState(pipelineState.blendState)),
		mVkPipeline(VK_NULL_HANDLE)
	{
		// Add a reference to the given program
		mProgram->addReference();

		// Sanity checks
		assert(nullptr != pipelineState.rootSignature);
		assert(nullptr != vulkanRenderer.getMainSwapChain());

		// TODO(co) For now, we get some information from the main swap chain
		const SwapChain* swapChain = static_cast<SwapChain*>(vulkanRenderer.getMainSwapChain());
		uint32_t width = 0;
		uint32_t height = 0;
		swapChain->getWidthAndHeight(width, height);

		// Shaders
		// TODO(co) Handle all shaders correctly (all stages, null pointer checks etc.)
		ProgramGlsl* programGlsl = static_cast<ProgramGlsl*>(mProgram);
		std::vector<VkPipelineShaderStageCreateInfo> vkPipelineShaderStageCreateInfos =
		{
			{ // Vertex shader
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,		// sType (VkStructureType)
				nullptr,													// pNext (const void*)
				0,															// flags (VkPipelineShaderStageCreateFlags)
				VK_SHADER_STAGE_VERTEX_BIT,									// stage (VkShaderStageFlagBits)
				programGlsl->getVertexShaderGlsl()->getVkShaderModule(),	// module (VkShaderModule)
				"main",														// pName (const char*)
				nullptr														// pSpecializationInfo (const VkSpecializationInfo*)
			},
			{ // Fragment shader
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,		// sType (VkStructureType)
				nullptr,													// pNext (const void*)
				0,															// flags (VkPipelineShaderStageCreateFlags)
				VK_SHADER_STAGE_FRAGMENT_BIT,								// stage (VkShaderStageFlagBits)
				programGlsl->getFragmentShaderGlsl()->getVkShaderModule(),	// module (VkShaderModule)
				"main",														// pName (const char*)
				nullptr														// pSpecializationInfo (const VkSpecializationInfo*)
			}
		};

		// Create the Vulkan graphics pipeline
		// TODO(co) Implement "VkPipeline" creation, this here is just a dummy for the first triangle on screen
		const VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,	// sType (VkStructureType)
			nullptr,													// pNext (const void*)
			0,															// flags (VkPipelineVertexInputStateCreateFlags)
			0,															// vertexBindingDescriptionCount (uint32_t)
			nullptr,													// pVertexBindingDescriptions (const VkVertexInputBindingDescription*)
			0,															// vertexAttributeDescriptionCount (uint32_t)
			nullptr														// pVertexAttributeDescriptions (const VkVertexInputAttributeDescription*)
		};
		const VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,	// sType (VkStructureType)
			nullptr,														// pNext (const void*)
			0,																// flags (VkPipelineInputAssemblyStateCreateFlags)
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,							// topology (VkPrimitiveTopology)
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
		const VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,	// sType (VkStructureType)
			nullptr,													// pNext (const void*)
			0,															// flags (VkPipelineRasterizationStateCreateFlags)
			VK_FALSE,													// depthClampEnable (VkBool32)
			VK_FALSE,													// rasterizerDiscardEnable (VkBool32)
			VK_POLYGON_MODE_FILL,										// polygonMode (VkPolygonMode)
			VK_CULL_MODE_BACK_BIT,										// cullMode (VkCullModeFlags)
			VK_FRONT_FACE_CLOCKWISE,									// frontFace (VkFrontFace)
			VK_FALSE,													// depthBiasEnable (VkBool32)
			0.0f,														// depthBiasConstantFactor (float)
			0.0f,														// depthBiasClamp (float)
			0.0f,														// depthBiasSlopeFactor (float)
			1.0f														// lineWidth (float)
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
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,	// sType (VkStructureType)
			nullptr,													// pNext (const void*)
			0,															// flags (VkPipelineDepthStencilStateCreateFlags)
			VK_FALSE,													// depthTestEnable (VkBool32)
			VK_FALSE,													// depthWriteEnable (VkBool32)
			VK_COMPARE_OP_LESS,											// depthCompareOp (VkCompareOp)
			VK_FALSE,													// depthBoundsTestEnable (VkBool32)
			VK_FALSE,													// stencilTestEnable (VkBool32)
			{ // front (VkStencilOpState)
				VK_STENCIL_OP_KEEP,										// failOp (VkStencilOp)
				VK_STENCIL_OP_KEEP,										// passOp (VkStencilOp)
				VK_STENCIL_OP_KEEP,										// depthFailOp (VkStencilOp)
				VK_COMPARE_OP_NEVER,									// compareOp (VkCompareOp)
				0,														// compareMask (uint32_t)
				0,														// writeMask (uint32_t)
				0														// reference (uint32_t)
			},
			{ // back (VkStencilOpState)
				VK_STENCIL_OP_KEEP,										// failOp (VkStencilOp)
				VK_STENCIL_OP_KEEP,										// passOp (VkStencilOp)
				VK_STENCIL_OP_KEEP,										// depthFailOp (VkStencilOp)
				VK_COMPARE_OP_NEVER,									// compareOp (VkCompareOp)
				0,														// compareMask (uint32_t)
				0,														// writeMask (uint32_t)
				0														// reference (uint32_t)
			},
			0.0f,														// minDepthBounds (float)
			1.0f														// maxDepthBounds (float)
		};
		const VkPipelineColorBlendAttachmentState vkPipelineColorBlendAttachmentState =
		{
			VK_FALSE,																									// blendEnable (VkBool32)
			VK_BLEND_FACTOR_ZERO,																						// srcColorBlendFactor (VkBlendFactor)
			VK_BLEND_FACTOR_ZERO,																						// dstColorBlendFactor (VkBlendFactor)
			VK_BLEND_OP_ADD,																							// colorBlendOp (VkBlendOp)
			VK_BLEND_FACTOR_ZERO,																						// srcAlphaBlendFactor (VkBlendFactor)
			VK_BLEND_FACTOR_ZERO,																						// dstAlphaBlendFactor (VkBlendFactor)
			VK_BLEND_OP_ADD,																							// alphaBlendOp (VkBlendOp)
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT	// colorWriteMask (VkColorComponentFlags)
		};
		const VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo =
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,	// sType (VkStructureType)
			nullptr,													// pNext (const void*)
			0,															// flags (VkPipelineColorBlendStateCreateFlags)
			VK_FALSE,													// logicOpEnable (VkBool32)
			VK_LOGIC_OP_COPY,											// logicOp (VkLogicOp)
			1,															// attachmentCount (uint32_t)
			&vkPipelineColorBlendAttachmentState,						// pAttachments (const VkPipelineColorBlendAttachmentState*)
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
			static_cast<uint32_t>(vkPipelineShaderStageCreateInfos.size()),							// stageCount (uint32_t)
			vkPipelineShaderStageCreateInfos.data(),												// pStages (const VkPipelineShaderStageCreateInfo*)
			&vkPipelineVertexInputStateCreateInfo,													// pVertexInputState (const VkPipelineVertexInputStateCreateInfo*)
			&vkPipelineInputAssemblyStateCreateInfo,												// pInputAssemblyState (const VkPipelineInputAssemblyStateCreateInfo*)
			nullptr,																				// pTessellationState (const VkPipelineTessellationStateCreateInfo*)
			&vkPipelineViewportStateCreateInfo,														// pViewportState (const VkPipelineViewportStateCreateInfo*)
			&vkPipelineRasterizationStateCreateInfo,												// pRasterizationState (const VkPipelineRasterizationStateCreateInfo*)
			&vkPipelineMultisampleStateCreateInfo,													// pMultisampleState (const VkPipelineMultisampleStateCreateInfo*)
			&vkPipelineDepthStencilStateCreateInfo,													// pDepthStencilState (const VkPipelineDepthStencilStateCreateInfo*)
			&vkPipelineColorBlendStateCreateInfo,													// pColorBlendState (const VkPipelineColorBlendStateCreateInfo*)
			&vkPipelineDynamicStateCreateInfo,														// pDynamicState (const VkPipelineDynamicStateCreateInfo*)
			static_cast<const RootSignature*>(pipelineState.rootSignature)->getVkPipelineLayout(),	// layout (VkPipelineLayout)
			swapChain->getVkRenderPass(),															// renderPass (VkRenderPass)
			0,																						// subpass (uint32_t)
			VK_NULL_HANDLE,																			// basePipelineHandle (VkPipeline)
			0																						// basePipelineIndex (int32_t)
		};
		if (vkCreateGraphicsPipelines(vulkanRenderer.getVulkanContext().getVkDevice(), VK_NULL_HANDLE, 1, &vkGraphicsPipelineCreateInfo, nullptr, &mVkPipeline) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan graphics pipeline")
		}
	}

	PipelineState::~PipelineState()
	{
		// Destroy the Vulkan graphics pipeline
		if (VK_NULL_HANDLE != mVkPipeline)
		{
			vkDestroyPipeline(static_cast<VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(), mVkPipeline, nullptr);
		}

		// Destroy states
		delete mRasterizerState;
		delete mDepthStencilState;
		delete mBlendState;

		// Release the program reference
		if (nullptr != mProgram)
		{
			mProgram->releaseReference();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
