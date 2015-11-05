/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageHlsl::NAME = "HLSL";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	ID3DXBuffer *ShaderLanguageHlsl::loadShader(const char *shaderModel, const char *shaderSource, const char *entryPoint, ID3DXConstantTable **d3dXConstantTable)
	{
		// TODO(co) Error handling
		DWORD shaderFlags = 0;
		#ifdef _DEBUG
			// Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
			// Setting this flag improves the shader debugging experience, but still allows 
			// the shaders to be optimized and to run exactly the way they will run in 
			// the release configuration of this program.
			shaderFlags |= D3DXSHADER_DEBUG;
		#endif
		ID3DXBuffer *d3dXBuffer = nullptr;
		ID3DXBuffer *d3dXBufferErrorMessages = nullptr;
		if (D3D_OK != D3DXCompileShader(shaderSource, strlen(shaderSource), nullptr, nullptr, entryPoint ? entryPoint : "main", shaderModel, shaderFlags, &d3dXBuffer, &d3dXBufferErrorMessages, d3dXConstantTable))
		{
			OutputDebugStringA(static_cast<char*>(d3dXBufferErrorMessages->GetBufferPointer()));
			d3dXBufferErrorMessages->Release();
		}

		// Done
		return d3dXBuffer;
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageHlsl::ShaderLanguageHlsl(Direct3D9Renderer &direct3D9Renderer) :
		ShaderLanguage(direct3D9Renderer),
		mDirect3D9Renderer(&direct3D9Renderer)
	{
		// Nothing to do in here
	}

	ShaderLanguageHlsl::~ShaderLanguageHlsl()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageHlsl::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader *ShaderLanguageHlsl::createVertexShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShaderHlsl(*mDirect3D9Renderer, bytecode, numberOfBytes);
	}

	Renderer::IVertexShader *ShaderLanguageHlsl::createVertexShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShaderHlsl(*mDirect3D9Renderer, sourceCode);
	}

	Renderer::ITessellationControlShader *ShaderLanguageHlsl::createTessellationControlShaderFromBytecode(const uint8_t *, uint32_t)
	{
		// Error! Direct3D 9 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationControlShader *ShaderLanguageHlsl::createTessellationControlShaderFromSourceCode(const char *, const char *, const char *, const char *)
	{
		// Error! Direct3D 9 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageHlsl::createTessellationEvaluationShaderFromBytecode(const uint8_t *, uint32_t)
	{
		// Error! Direct3D 9 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageHlsl::createTessellationEvaluationShaderFromSourceCode(const char *, const char *, const char *, const char *)
	{
		// Error! Direct3D 9 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::IGeometryShader *ShaderLanguageHlsl::createGeometryShaderFromBytecode(const uint8_t *, uint32_t, Renderer::GsInputPrimitiveTopology::Enum, Renderer::GsOutputPrimitiveTopology::Enum, uint32_t, const char *, const char *, const char *)
	{
		// Error! Direct3D 9 has no geometry shader support.
		return nullptr;
	}

	Renderer::IGeometryShader *ShaderLanguageHlsl::createGeometryShaderFromSourceCode(const char *, Renderer::GsInputPrimitiveTopology::Enum, Renderer::GsOutputPrimitiveTopology::Enum, uint32_t, const char *, const char *, const char *)
	{
		// Error! Direct3D 9 has no geometry shader support.
		return nullptr;
	}

	Renderer::IFragmentShader *ShaderLanguageHlsl::createFragmentShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShaderHlsl(*mDirect3D9Renderer, bytecode, numberOfBytes);
	}

	Renderer::IFragmentShader *ShaderLanguageHlsl::createFragmentShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShaderHlsl(*mDirect3D9Renderer, sourceCode);
	}

	Renderer::IProgram *ShaderLanguageHlsl::createProgram(const Renderer::VertexAttributes&, Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader)
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
			return new ProgramHlsl(*mDirect3D9Renderer, static_cast<VertexShaderHlsl*>(vertexShader), static_cast<FragmentShaderHlsl*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->release();
		}
		if (nullptr != fragmentShader)
		{
			fragmentShader->addReference();
			fragmentShader->release();
		}

		// Error!
		return nullptr;
	}

	Renderer::IUniformBuffer *ShaderLanguageHlsl::createUniformBuffer(uint32_t, const void *, Renderer::BufferUsage::Enum)
	{
		// Error! Direct3D 9 has no uniform buffer support.
		return nullptr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
