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
#include "Direct3D11Renderer/VertexShaderHlsl.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"
#include "Direct3D11Renderer/Direct3D11RuntimeLinking.h"
#include "Direct3D11Renderer/ShaderLanguageHlsl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexShaderHlsl::VertexShaderHlsl(Direct3D11Renderer &direct3D11Renderer, const uint8_t *bytecode, uint32_t numberOfBytes) :
		VertexShader(direct3D11Renderer),
		mD3DBlobVertexShader(nullptr),
		mD3D11VertexShader(nullptr)
	{
		// Backup the vertex shader bytecode
		D3DCreateBlob(numberOfBytes, &mD3DBlobVertexShader);
		memcpy(mD3DBlobVertexShader->GetBufferPointer(), bytecode, numberOfBytes);

		// Create the Direct3D 11 vertex shader
		direct3D11Renderer.getD3D11Device()->CreateVertexShader(bytecode, numberOfBytes, nullptr, &mD3D11VertexShader);

		// Don't assign a default name to the resource for debugging purposes, Direct3D 11 automatically sets a decent default name
	}

	VertexShaderHlsl::VertexShaderHlsl(Direct3D11Renderer &direct3D11Renderer, const char *sourceCode) :
		VertexShader(direct3D11Renderer),
		mD3DBlobVertexShader(nullptr),
		mD3D11VertexShader(nullptr)
	{
		// Create the Direct3D 11 binary large object for the vertex shader
		mD3DBlobVertexShader = ShaderLanguageHlsl::loadShader("vs_5_0", sourceCode, nullptr);
		if (nullptr != mD3DBlobVertexShader)
		{
			// Create the Direct3D 11 vertex shader
			direct3D11Renderer.getD3D11Device()->CreateVertexShader(mD3DBlobVertexShader->GetBufferPointer(), mD3DBlobVertexShader->GetBufferSize(), nullptr, &mD3D11VertexShader);
		}

		// Don't assign a default name to the resource for debugging purposes, Direct3D 11 automatically sets a decent default name
	}

	VertexShaderHlsl::~VertexShaderHlsl()
	{
		// Release the Direct3D 11 shader binary large object
		if (nullptr != mD3DBlobVertexShader)
		{
			mD3DBlobVertexShader->Release();
		}

		// Release the Direct3D 11 vertex shader
		if (nullptr != mD3D11VertexShader)
		{
			mD3D11VertexShader->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void VertexShaderHlsl::setDebugName(const char *name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Valid Direct3D 11 vertex shader?
			if (nullptr != mD3D11VertexShader)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D11VertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11VertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char *VertexShaderHlsl::getShaderLanguageName() const
	{
		return ShaderLanguageHlsl::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
