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
#include "Direct3D11Renderer/Shader/TessellationControlShaderHlsl.h"
#include "Direct3D11Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"

#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TessellationControlShaderHlsl::TessellationControlShaderHlsl(Direct3D11Renderer& direct3D11Renderer, const Renderer::ShaderBytecode& shaderBytecode) :
		ITessellationControlShader(direct3D11Renderer),
		mD3D11HullShader(nullptr)
	{
		// Create the Direct3D 11 hull shader
		direct3D11Renderer.getD3D11Device()->CreateHullShader(shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes(), nullptr, &mD3D11HullShader);

		// Don't assign a default name to the resource for debugging purposes, Direct3D 11 automatically sets a decent default name
	}

	TessellationControlShaderHlsl::TessellationControlShaderHlsl(Direct3D11Renderer& direct3D11Renderer, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode) :
		ITessellationControlShader(direct3D11Renderer),
		mD3D11HullShader(nullptr)
	{
		// Create the Direct3D 11 binary large object for the hull shader
		ID3DBlob* d3dBlob = static_cast<ShaderLanguageHlsl*>(direct3D11Renderer.getShaderLanguage())->loadShaderFromSourcecode("hs_5_0", sourceCode, nullptr);
		if (nullptr != d3dBlob)
		{
			// Create the Direct3D 11 hull shader
			direct3D11Renderer.getD3D11Device()->CreateHullShader(d3dBlob->GetBufferPointer(), d3dBlob->GetBufferSize(), nullptr, &mD3D11HullShader);

			// Return shader bytecode, if requested do to so
			if (nullptr != shaderBytecode)
			{
				shaderBytecode->setBytecodeCopy(static_cast<uint32_t>(d3dBlob->GetBufferSize()), static_cast<uint8_t*>(d3dBlob->GetBufferPointer()));
			}

			// Release the Direct3D 11 shader binary large object
			d3dBlob->Release();
		}

		// Don't assign a default name to the resource for debugging purposes, Direct3D 11 automatically sets a decent default name
	}

	TessellationControlShaderHlsl::~TessellationControlShaderHlsl()
	{
		// Release the Direct3D 11 hull shader
		if (nullptr != mD3D11HullShader)
		{
			mD3D11HullShader->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void TessellationControlShaderHlsl::setDebugName(const char* name)
		{
			// Valid Direct3D 11 hull shader?
			if (nullptr != mD3D11HullShader)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D11HullShader->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11HullShader->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		}
	#else
		void TessellationControlShaderHlsl::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* TessellationControlShaderHlsl::getShaderLanguageName() const
	{
		return ShaderLanguageHlsl::NAME;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void TessellationControlShaderHlsl::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), TessellationControlShaderHlsl, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
