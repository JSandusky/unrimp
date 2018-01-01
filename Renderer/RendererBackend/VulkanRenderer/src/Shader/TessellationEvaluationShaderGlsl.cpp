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
#include "VulkanRenderer/Shader/TessellationEvaluationShaderGlsl.h"
#include "VulkanRenderer/Shader/ShaderLanguageGlsl.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"

#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TessellationEvaluationShaderGlsl::TessellationEvaluationShaderGlsl(VulkanRenderer& vulkanRenderer, const Renderer::ShaderBytecode& shaderBytecode):
		ITessellationEvaluationShader(vulkanRenderer),
		mVkShaderModule(ShaderLanguageGlsl::createVkShaderModuleFromBytecode(vulkanRenderer, shaderBytecode))
	{
		SET_DEFAULT_DEBUG_NAME	// setDebugName("");
	}

	TessellationEvaluationShaderGlsl::TessellationEvaluationShaderGlsl(VulkanRenderer& vulkanRenderer, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode) :
		ITessellationEvaluationShader(vulkanRenderer),
		mVkShaderModule(ShaderLanguageGlsl::createVkShaderModuleFromSourceCode(vulkanRenderer, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, sourceCode, shaderBytecode))
	{
		SET_DEFAULT_DEBUG_NAME	// setDebugName("");
	}

	TessellationEvaluationShaderGlsl::~TessellationEvaluationShaderGlsl()
	{
		if (VK_NULL_HANDLE != mVkShaderModule)
		{
			const VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
			vkDestroyShaderModule(vulkanRenderer.getVulkanContext().getVkDevice(), mVkShaderModule, vulkanRenderer.getVkAllocationCallbacks());
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	DEFINE_SET_DEBUG_NAME_SHADER_MODULE(TessellationEvaluationShaderGlsl)	// void TessellationEvaluationShaderGlsl::setDebugName(const char* name)


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* TessellationEvaluationShaderGlsl::getShaderLanguageName() const
	{
		return ShaderLanguageGlsl::NAME;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void TessellationEvaluationShaderGlsl::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), TessellationEvaluationShaderGlsl, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
