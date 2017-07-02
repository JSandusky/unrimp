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
#include "Direct3D9Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D9Renderer/Shader/ProgramHlsl.h"
#include "Direct3D9Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D9Renderer/Shader/FragmentShaderHlsl.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"
#include "Direct3D9Renderer/Direct3D9RuntimeLinking.h"

#include <Renderer/ILog.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char* ShaderLanguageHlsl::NAME = "HLSL";


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageHlsl::ShaderLanguageHlsl(Direct3D9Renderer& direct3D9Renderer) :
		IShaderLanguage(direct3D9Renderer)
	{
		// Nothing here
	}

	ShaderLanguageHlsl::~ShaderLanguageHlsl()
	{
		// Nothing here
	}

	ID3DXBuffer* ShaderLanguageHlsl::loadShaderFromSourcecode(const char* shaderModel, const char* shaderSource, const char* entryPoint, ID3DXConstantTable** d3dXConstantTable) const
	{
		// TODO(co) Error handling

		// Get compile flags
		UINT compileFlags = D3DXSHADER_IEEE_STRICTNESS;
		switch (getOptimizationLevel())
		{
			case OptimizationLevel::Debug:
				compileFlags |= D3DXSHADER_DEBUG;
				compileFlags |= D3DXSHADER_SKIPOPTIMIZATION;
				break;

			case OptimizationLevel::None:
				compileFlags |= D3DXSHADER_SKIPVALIDATION;
				compileFlags |= D3DXSHADER_SKIPOPTIMIZATION;
				break;

			case OptimizationLevel::Low:
				compileFlags |= D3DXSHADER_SKIPVALIDATION;
				compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL0;
				break;

			case OptimizationLevel::Medium:
				compileFlags |= D3DXSHADER_SKIPVALIDATION;
				compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL1;
				break;

			case OptimizationLevel::High:
				compileFlags |= D3DXSHADER_SKIPVALIDATION;
				compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL2;
				break;

			case OptimizationLevel::Ultra:
				compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL3;
				break;
		}

		ID3DXBuffer* d3dXBuffer = nullptr;
		ID3DXBuffer* d3dXBufferErrorMessages = nullptr;
		if (D3D_OK != D3DXCompileShader(shaderSource, static_cast<UINT>(strlen(shaderSource)), nullptr, nullptr, entryPoint ? entryPoint : "main", shaderModel, compileFlags, &d3dXBuffer, &d3dXBufferErrorMessages, d3dXConstantTable))
		{
			RENDERER_LOG(static_cast<Direct3D9Renderer&>(getRenderer()).getContext(), CRITICAL, static_cast<char*>(d3dXBufferErrorMessages->GetBufferPointer()))
			d3dXBufferErrorMessages->Release();
		}

		// Done
		return d3dXBuffer;
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char* ShaderLanguageHlsl::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader* ShaderLanguageHlsl::createVertexShaderFromBytecode(const Renderer::VertexAttributes&, const Renderer::ShaderBytecode& shaderBytecode)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShaderHlsl(static_cast<Direct3D9Renderer&>(getRenderer()), shaderBytecode);
	}

	Renderer::IVertexShader* ShaderLanguageHlsl::createVertexShaderFromSourceCode(const Renderer::VertexAttributes&, const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShaderHlsl(static_cast<Direct3D9Renderer&>(getRenderer()), shaderSourceCode.sourceCode, shaderBytecode);
	}

	Renderer::ITessellationControlShader* ShaderLanguageHlsl::createTessellationControlShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// Error! Direct3D 9 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationControlShader* ShaderLanguageHlsl::createTessellationControlShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::ShaderBytecode*)
	{
		// Error! Direct3D 9 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageHlsl::createTessellationEvaluationShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// Error! Direct3D 9 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageHlsl::createTessellationEvaluationShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::ShaderBytecode*)
	{
		// Error! Direct3D 9 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::IGeometryShader* ShaderLanguageHlsl::createGeometryShaderFromBytecode(const Renderer::ShaderBytecode&, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t)
	{
		// Error! Direct3D 9 has no geometry shader support.
		return nullptr;
	}

	Renderer::IGeometryShader* ShaderLanguageHlsl::createGeometryShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t, Renderer::ShaderBytecode*)
	{
		// Error! Direct3D 9 has no geometry shader support.
		return nullptr;
	}

	Renderer::IFragmentShader* ShaderLanguageHlsl::createFragmentShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShaderHlsl(static_cast<Direct3D9Renderer&>(getRenderer()), shaderBytecode);
	}

	Renderer::IFragmentShader* ShaderLanguageHlsl::createFragmentShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShaderHlsl(static_cast<Direct3D9Renderer&>(getRenderer()), shaderSourceCode.sourceCode, shaderBytecode);
	}

	Renderer::IProgram* ShaderLanguageHlsl::createProgram(const Renderer::IRootSignature&, const Renderer::VertexAttributes&, Renderer::IVertexShader* vertexShader, Renderer::ITessellationControlShader* tessellationControlShader, Renderer::ITessellationEvaluationShader* tessellationEvaluationShader, Renderer::IGeometryShader* geometryShader, Renderer::IFragmentShader* fragmentShader)
	{
		// A shader can be a null pointer, but if it's not the shader and program language must match!
		// -> Optimization: Comparing the shader language name by directly comparing the pointer address of
		//    the name is safe because we know that we always reference to one and the same name address
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		if (nullptr != vertexShader && vertexShader->getShaderLanguageName() != NAME)
		{
			// Error! Vertex shader language mismatch!
		}
		else if (nullptr != tessellationControlShader)
		{
			// Error! Direct3D 9 has no tessellation control shader support.
		}
		else if (nullptr != tessellationEvaluationShader)
		{
			// Error! Direct3D 9 has no tessellation evaluation shader support.
		}
		else if (nullptr != geometryShader)
		{
			// Error! Direct3D 9 has no geometry shader support.
		}
		else if (nullptr != fragmentShader && fragmentShader->getShaderLanguageName() != NAME)
		{
			// Error! Fragment shader language mismatch!
		}
		else
		{
			// Create the program
			return new ProgramHlsl(static_cast<Direct3D9Renderer&>(getRenderer()), static_cast<VertexShaderHlsl*>(vertexShader), static_cast<FragmentShaderHlsl*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->releaseReference();
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
} // Direct3D9Renderer
