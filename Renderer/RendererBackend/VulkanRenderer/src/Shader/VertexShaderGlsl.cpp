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
#include "VulkanRenderer/Shader/VertexShaderGlsl.h"
#include "VulkanRenderer/Shader/ShaderLanguageGlsl.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexShaderGlsl::VertexShaderGlsl(VulkanRenderer& vulkanRenderer, const Renderer::ShaderBytecode& shaderBytecode) :
		IVertexShader(vulkanRenderer),
		mVkShaderModule(ShaderLanguageGlsl::createVkShaderModuleFromBytecode(vulkanRenderer, shaderBytecode))
	{
		// Nothing here
	}

	VertexShaderGlsl::VertexShaderGlsl(VulkanRenderer& vulkanRenderer, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode) :
		IVertexShader(vulkanRenderer),
		mVkShaderModule(ShaderLanguageGlsl::createVkShaderModuleFromSourceCode(vulkanRenderer, VK_SHADER_STAGE_VERTEX_BIT, sourceCode, shaderBytecode))
	{
		// Nothing here
	}

	VertexShaderGlsl::~VertexShaderGlsl()
	{
		if (VK_NULL_HANDLE != mVkShaderModule)
		{
			vkDestroyShaderModule(static_cast<VulkanRenderer&>(getRenderer()).getVulkanContext().getVkDevice(), mVkShaderModule, nullptr);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* VertexShaderGlsl::getShaderLanguageName() const
	{
		return ShaderLanguageGlsl::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
