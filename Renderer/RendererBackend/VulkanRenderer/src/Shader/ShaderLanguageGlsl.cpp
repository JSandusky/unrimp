/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include "VulkanRenderer/Shader/ShaderLanguageGlsl.h"
#include "VulkanRenderer/Shader/ProgramGlsl.h"
#include "VulkanRenderer/Shader/VertexShaderGlsl.h"
#include "VulkanRenderer/Shader/GeometryShaderGlsl.h"
#include "VulkanRenderer/Shader/FragmentShaderGlsl.h"
#include "VulkanRenderer/Shader/TessellationControlShaderGlsl.h"
#include "VulkanRenderer/Shader/TessellationEvaluationShaderGlsl.h"
#include "VulkanRenderer/IContext.h"
#include "VulkanRenderer/Extensions.h"
#include "VulkanRenderer/VulkanRenderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageGlsl::NAME = "GLSL";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t ShaderLanguageGlsl::loadShader(uint32_t, const char*)
	{
		// TODO(co) Implement me
		return 0;
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageGlsl::ShaderLanguageGlsl(VulkanRenderer &vulkanRenderer) :
		IShaderLanguage(vulkanRenderer)
	{
		// Nothing here
	}

	ShaderLanguageGlsl::~ShaderLanguageGlsl()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageGlsl::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader *ShaderLanguageGlsl::createVertexShaderFromBytecode(const Renderer::VertexAttributes&, const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		return new VertexShaderGlsl(static_cast<VulkanRenderer&>(getRenderer()), bytecode, numberOfBytes);
	}

	Renderer::IVertexShader *ShaderLanguageGlsl::createVertexShaderFromSourceCode(const Renderer::VertexAttributes&, const char *sourceCode, const char *, const char *, const char *)
	{
		return new VertexShaderGlsl(static_cast<VulkanRenderer&>(getRenderer()), sourceCode);
	}

	Renderer::ITessellationControlShader *ShaderLanguageGlsl::createTessellationControlShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		return new TessellationControlShaderGlsl(static_cast<VulkanRenderer&>(getRenderer()), bytecode, numberOfBytes);
	}

	Renderer::ITessellationControlShader *ShaderLanguageGlsl::createTessellationControlShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		return new TessellationControlShaderGlsl(static_cast<VulkanRenderer&>(getRenderer()), sourceCode);
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageGlsl::createTessellationEvaluationShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		return new TessellationEvaluationShaderGlsl(static_cast<VulkanRenderer&>(getRenderer()), bytecode, numberOfBytes);
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageGlsl::createTessellationEvaluationShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		return new TessellationEvaluationShaderGlsl(static_cast<VulkanRenderer&>(getRenderer()), sourceCode);
	}

	Renderer::IGeometryShader *ShaderLanguageGlsl::createGeometryShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *, const char *, const char *)
	{
		return new GeometryShaderGlsl(static_cast<VulkanRenderer&>(getRenderer()), bytecode, numberOfBytes, gsInputPrimitiveTopology, gsOutputPrimitiveTopology, numberOfOutputVertices);
	}

	Renderer::IGeometryShader *ShaderLanguageGlsl::createGeometryShaderFromSourceCode(const char *sourceCode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *, const char *, const char *)
	{
		return new GeometryShaderGlsl(static_cast<VulkanRenderer&>(getRenderer()), sourceCode, gsInputPrimitiveTopology, gsOutputPrimitiveTopology, numberOfOutputVertices);
	}

	Renderer::IFragmentShader *ShaderLanguageGlsl::createFragmentShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		return new FragmentShaderGlsl(static_cast<VulkanRenderer&>(getRenderer()), bytecode, numberOfBytes);
	}

	Renderer::IFragmentShader *ShaderLanguageGlsl::createFragmentShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		return new FragmentShaderGlsl(static_cast<VulkanRenderer&>(getRenderer()), sourceCode);
	}

	Renderer::IProgram *ShaderLanguageGlsl::createProgram(const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader)
	{
		VulkanRenderer &vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());

		// A shader can be a null pointer, but if it's not the shader and program language must match!
		// -> Optimization: Comparing the shader language name by directly comparing the pointer address of
		//    the name is safe because we know that we always reference to one and the same name address
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		if (nullptr != vertexShader && vertexShader->getShaderLanguageName() != NAME)
		{
			// Error! Vertex shader language mismatch!
		}
		else if (nullptr != tessellationControlShader && tessellationControlShader->getShaderLanguageName() != NAME)
		{
			// Error! Tessellation control shader language mismatch!
		}
		else if (nullptr != tessellationEvaluationShader && tessellationEvaluationShader->getShaderLanguageName() != NAME)
		{
			// Error! Tessellation evaluation shader language mismatch!
		}
		else if (nullptr != geometryShader && geometryShader->getShaderLanguageName() != NAME)
		{
			// Error! Geometry shader language mismatch!
		}
		else if (nullptr != fragmentShader && fragmentShader->getShaderLanguageName() != NAME)
		{
			// Error! Fragment shader language mismatch!
		}
		else
		{
			return new ProgramGlsl(vulkanRenderer, rootSignature, vertexAttributes, static_cast<VertexShaderGlsl*>(vertexShader), static_cast<TessellationControlShaderGlsl*>(tessellationControlShader), static_cast<TessellationEvaluationShaderGlsl*>(tessellationEvaluationShader), static_cast<GeometryShaderGlsl*>(geometryShader), static_cast<FragmentShaderGlsl*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->releaseReference();
		}
		if (nullptr != tessellationControlShader)
		{
			tessellationControlShader->addReference();
			tessellationControlShader->releaseReference();
		}
		if (nullptr != tessellationEvaluationShader)
		{
			tessellationEvaluationShader->addReference();
			tessellationEvaluationShader->releaseReference();
		}
		if (nullptr != geometryShader)
		{
			geometryShader->addReference();
			geometryShader->releaseReference();
		}
		if (nullptr != fragmentShader)
		{
			fragmentShader->addReference();
			fragmentShader->releaseReference();
		}

		// Error!
		return nullptr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
