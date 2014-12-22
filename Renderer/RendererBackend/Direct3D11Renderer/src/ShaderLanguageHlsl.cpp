/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "Direct3D11Renderer/ShaderLanguageHlsl.h"
#include "Direct3D11Renderer/ProgramHlsl.h"
#include "Direct3D11Renderer/UniformBuffer.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"
#include "Direct3D11Renderer/VertexShaderHlsl.h"
#include "Direct3D11Renderer/GeometryShaderHlsl.h"
#include "Direct3D11Renderer/FragmentShaderHlsl.h"
#include "Direct3D11Renderer/Direct3D11RuntimeLinking.h"
#include "Direct3D11Renderer/TessellationControlShaderHlsl.h"
#include "Direct3D11Renderer/TessellationEvaluationShaderHlsl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageHlsl::NAME = "HLSL";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	ID3DBlob *ShaderLanguageHlsl::loadShader(const char *shaderModel, const char *shaderSource, const char *entryPoint)
	{
		// TODO(co) Cleanup
		ID3DBlob *d3dBlob;
	
		HRESULT hr = S_OK;

		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
		// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		shaderFlags |= D3DCOMPILE_DEBUG;
	#endif

		ID3DBlob *errorBlob;
		hr = D3DX11CompileFromMemory(shaderSource, strlen(shaderSource), nullptr, nullptr, nullptr, entryPoint ? entryPoint : "main", shaderModel, 
			shaderFlags, 0, nullptr, &d3dBlob, &errorBlob, nullptr );

		if (FAILED(hr))
		{
			if( errorBlob != nullptr )
				OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
			if( errorBlob ) errorBlob->Release();
			return nullptr;
		}
		if (nullptr != errorBlob ) errorBlob->Release();

		// Done
		return d3dBlob;
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageHlsl::ShaderLanguageHlsl(Direct3D11Renderer &direct3D11Renderer) :
		ShaderLanguage(direct3D11Renderer)
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

	Renderer::IVertexShader *ShaderLanguageHlsl::createVertexShader(const char *sourceCode, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShaderHlsl(getDirect3D11Renderer(), sourceCode);
	}

	Renderer::ITessellationControlShader *ShaderLanguageHlsl::createTessellationControlShader(const char *sourceCode, const char *, const char *, const char *)
	{
		// "hull shader" in Direct3D terminology

		// There's no need to check for "Renderer::Capabilities::maximumNumberOfPatchVertices", we know there's tessellation control shader support
		return new TessellationControlShaderHlsl(getDirect3D11Renderer(), sourceCode);
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageHlsl::createTessellationEvaluationShader(const char *sourceCode, const char *, const char *, const char *)
	{
		// "domain shader" in Direct3D terminology

		// There's no need to check for "Renderer::Capabilities::maximumNumberOfPatchVertices", we know there's tessellation evaluation shader support
		return new TessellationEvaluationShaderHlsl(getDirect3D11Renderer(), sourceCode);
	}

	Renderer::IGeometryShader *ShaderLanguageHlsl::createGeometryShader(const char *sourceCode, Renderer::GsInputPrimitiveTopology::Enum, Renderer::GsOutputPrimitiveTopology::Enum, uint32_t, const char *, const char *, const char *)
	{
		// Ignore "gsInputPrimitiveTopology", it's directly set within HLSL
		// Ignore "gsOutputPrimitiveTopology", it's directly set within HLSL
		// Ignore "numberOfOutputVertices", it's directly set within HLSL

		// There's no need to check for "Renderer::Capabilities::maximumNumberOfGsOutputVertices", we know there's geometry shader support
		return new GeometryShaderHlsl(getDirect3D11Renderer(), sourceCode);
	}

	Renderer::IFragmentShader *ShaderLanguageHlsl::createFragmentShader(const char *sourceCode, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShaderHlsl(getDirect3D11Renderer(), sourceCode);
	}

	Renderer::IProgram *ShaderLanguageHlsl::createProgram(Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader)
	{
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
			// Create the program
			return new ProgramHlsl(getDirect3D11Renderer(), static_cast<VertexShaderHlsl*>(vertexShader), static_cast<TessellationControlShaderHlsl*>(tessellationControlShader), static_cast<TessellationEvaluationShaderHlsl*>(tessellationEvaluationShader), static_cast<GeometryShaderHlsl*>(geometryShader), static_cast<FragmentShaderHlsl*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->release();
		}
		if (nullptr != tessellationControlShader)
		{
			tessellationControlShader->addReference();
			tessellationControlShader->release();
		}
		if (nullptr != tessellationEvaluationShader)
		{
			tessellationEvaluationShader->addReference();
			tessellationEvaluationShader->release();
		}
		if (nullptr != geometryShader)
		{
			geometryShader->addReference();
			geometryShader->release();
		}
		if (nullptr != fragmentShader)
		{
			fragmentShader->addReference();
			fragmentShader->release();
		}

		// Error!
		return nullptr;
	}

	Renderer::IUniformBuffer *ShaderLanguageHlsl::createUniformBuffer(uint32_t numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new UniformBuffer(getDirect3D11Renderer(), numberOfBytes, data, bufferUsage);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
