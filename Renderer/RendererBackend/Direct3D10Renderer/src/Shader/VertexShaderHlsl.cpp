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
#include "Direct3D10Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D10Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"
#include "Direct3D10Renderer/Direct3D10RuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexShaderHlsl::VertexShaderHlsl(Direct3D10Renderer& direct3D10Renderer, const Renderer::ShaderBytecode& shaderBytecode) :
		IVertexShader(direct3D10Renderer),
		mD3DBlobVertexShader(nullptr),
		mD3D10VertexShader(nullptr)
	{
		// Backup the vertex shader bytecode
		D3DCreateBlob(shaderBytecode.getNumberOfBytes(), &mD3DBlobVertexShader);
		memcpy(mD3DBlobVertexShader->GetBufferPointer(), shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes());

		// Create the Direct3D 10 vertex shader
		direct3D10Renderer.getD3D10Device()->CreateVertexShader(shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes(), &mD3D10VertexShader);

		// Don't assign a default name to the resource for debugging purposes, Direct3D 10 automatically sets a decent default name
	}

	VertexShaderHlsl::VertexShaderHlsl(Direct3D10Renderer& direct3D10Renderer, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode) :
		IVertexShader(direct3D10Renderer),
		mD3DBlobVertexShader(nullptr),
		mD3D10VertexShader(nullptr)
	{
		// Create the Direct3D 10 binary large object for the vertex shader
		mD3DBlobVertexShader = static_cast<ShaderLanguageHlsl*>(direct3D10Renderer.getShaderLanguage())->loadShaderFromSourcecode("vs_4_0", sourceCode, nullptr);
		if (nullptr != mD3DBlobVertexShader)
		{
			// Create the Direct3D 10 vertex shader
			direct3D10Renderer.getD3D10Device()->CreateVertexShader(mD3DBlobVertexShader->GetBufferPointer(), mD3DBlobVertexShader->GetBufferSize(), &mD3D10VertexShader);

			// Return shader bytecode, if requested do to so
			if (nullptr != shaderBytecode)
			{
				shaderBytecode->setBytecodeCopy(static_cast<uint32_t>(mD3DBlobVertexShader->GetBufferSize()), static_cast<uint8_t*>(mD3DBlobVertexShader->GetBufferPointer()));
			}
		}

		// Don't assign a default name to the resource for debugging purposes, Direct3D 10 automatically sets a decent default name
	}

	VertexShaderHlsl::~VertexShaderHlsl()
	{
		// Release the Direct3D 10 shader binary large object
		if (nullptr != mD3DBlobVertexShader)
		{
			mD3DBlobVertexShader->Release();
		}

		// Release the Direct3D 10 vertex shader
		if (nullptr != mD3D10VertexShader)
		{
			mD3D10VertexShader->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void VertexShaderHlsl::setDebugName(const char* name)
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			// Valid Direct3D 10 vertex shader?
			if (nullptr != mD3D10VertexShader)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D10VertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10VertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* VertexShaderHlsl::getShaderLanguageName() const
	{
		return ShaderLanguageHlsl::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
