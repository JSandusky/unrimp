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
#include "Direct3D10Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D10Renderer/Shader/ProgramHlsl.h"
#include "Direct3D10Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D10Renderer/Shader/GeometryShaderHlsl.h"
#include "Direct3D10Renderer/Shader/FragmentShaderHlsl.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"
#include "Direct3D10Renderer/Direct3D10RuntimeLinking.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char* ShaderLanguageHlsl::NAME = "HLSL";


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageHlsl::ShaderLanguageHlsl(Direct3D10Renderer& direct3D10Renderer) :
		IShaderLanguage(direct3D10Renderer)
	{
		// Nothing here
	}

	ShaderLanguageHlsl::~ShaderLanguageHlsl()
	{
		// Nothing here
	}

	ID3DBlob* ShaderLanguageHlsl::loadShaderFromSourcecode(const char* shaderModel, const char* sourceCode, const char* entryPoint) const
	{
		// Sanity checks
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != shaderModel, "Invalid Direct3D 10 shader model")
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != sourceCode, "Invalid Direct3D 10 shader source code")

		// Get compile flags
		UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
		switch (getOptimizationLevel())
		{
			case OptimizationLevel::Debug:
				compileFlags |= D3DCOMPILE_DEBUG;
				compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
				break;

			case OptimizationLevel::None:
				compileFlags |= D3DCOMPILE_SKIP_VALIDATION;
				compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
				break;

			case OptimizationLevel::Low:
				compileFlags |= D3DCOMPILE_SKIP_VALIDATION;
				compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
				break;

			case OptimizationLevel::Medium:
				compileFlags |= D3DCOMPILE_SKIP_VALIDATION;
				compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
				break;

			case OptimizationLevel::High:
				compileFlags |= D3DCOMPILE_SKIP_VALIDATION;
				compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL2;
				break;

			case OptimizationLevel::Ultra:
				compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
				break;
		}

		// Compile
		ID3DBlob* d3dBlob = nullptr;
		ID3DBlob* errorD3dBlob = nullptr;
		if (FAILED(D3DCompile(sourceCode, strlen(sourceCode), nullptr, nullptr, nullptr, entryPoint ? entryPoint : "main", shaderModel, compileFlags, 0, &d3dBlob, &errorD3dBlob)))
		{
			if (nullptr != errorD3dBlob)
			{
				if (static_cast<Direct3D10Renderer&>(getRenderer()).getContext().getLog().print(Renderer::ILog::Type::CRITICAL, sourceCode, __FILE__, static_cast<uint32_t>(__LINE__), static_cast<char*>(errorD3dBlob->GetBufferPointer())))
				{
					DEBUG_BREAK;
				}
				errorD3dBlob->Release();
			}
			return nullptr;
		}
		if (nullptr != errorD3dBlob)
		{
			errorD3dBlob->Release();
		}

		// Done
		return d3dBlob;
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
		return RENDERER_NEW(getRenderer().getContext(), VertexShaderHlsl)(static_cast<Direct3D10Renderer&>(getRenderer()), shaderBytecode);
	}

	Renderer::IVertexShader* ShaderLanguageHlsl::createVertexShaderFromSourceCode(const Renderer::VertexAttributes&, const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return RENDERER_NEW(getRenderer().getContext(), VertexShaderHlsl)(static_cast<Direct3D10Renderer&>(getRenderer()), shaderSourceCode.sourceCode, shaderBytecode);
	}

	Renderer::ITessellationControlShader* ShaderLanguageHlsl::createTessellationControlShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// Error! Direct3D 10 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationControlShader* ShaderLanguageHlsl::createTessellationControlShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::ShaderBytecode*)
	{
		// Error! Direct3D 10 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageHlsl::createTessellationEvaluationShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// Error! Direct3D 10 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageHlsl::createTessellationEvaluationShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::ShaderBytecode*)
	{
		// Error! Direct3D 10 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::IGeometryShader* ShaderLanguageHlsl::createGeometryShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfGsOutputVertices", we know there's geometry shader support
		// Ignore "gsInputPrimitiveTopology", it's directly set within HLSL
		// Ignore "gsOutputPrimitiveTopology", it's directly set within HLSL
		// Ignore "numberOfOutputVertices", it's directly set within HLSL
		return RENDERER_NEW(getRenderer().getContext(), GeometryShaderHlsl)(static_cast<Direct3D10Renderer&>(getRenderer()), shaderBytecode);
	}

	Renderer::IGeometryShader* ShaderLanguageHlsl::createGeometryShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t, Renderer::ShaderBytecode* shaderBytecode)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfGsOutputVertices", we know there's geometry shader support
		// Ignore "gsInputPrimitiveTopology", it's directly set within HLSL
		// Ignore "gsOutputPrimitiveTopology", it's directly set within HLSL
		// Ignore "numberOfOutputVertices", it's directly set within HLSL
		return RENDERER_NEW(getRenderer().getContext(), GeometryShaderHlsl)(static_cast<Direct3D10Renderer&>(getRenderer()), shaderSourceCode.sourceCode, shaderBytecode);
	}

	Renderer::IFragmentShader* ShaderLanguageHlsl::createFragmentShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return RENDERER_NEW(getRenderer().getContext(), FragmentShaderHlsl)(static_cast<Direct3D10Renderer&>(getRenderer()), shaderBytecode);
	}

	Renderer::IFragmentShader* ShaderLanguageHlsl::createFragmentShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return RENDERER_NEW(getRenderer().getContext(), FragmentShaderHlsl)(static_cast<Direct3D10Renderer&>(getRenderer()), shaderSourceCode.sourceCode, shaderBytecode);
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
			// Error! Direct3D 10 has no tessellation control shader support.
		}
		else if (nullptr != tessellationEvaluationShader)
		{
			// Error! Direct3D 10 has no tessellation evaluation shader support.
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
			// Create the program
			return RENDERER_NEW(getRenderer().getContext(), ProgramHlsl)(static_cast<Direct3D10Renderer&>(getRenderer()), static_cast<VertexShaderHlsl*>(vertexShader), static_cast<GeometryShaderHlsl*>(geometryShader), static_cast<FragmentShaderHlsl*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->releaseReference();
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
} // Direct3D10Renderer
