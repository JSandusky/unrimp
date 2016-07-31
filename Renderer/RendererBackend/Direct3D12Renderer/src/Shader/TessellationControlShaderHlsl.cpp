/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "Direct3D12Renderer/Shader/TessellationControlShaderHlsl.h"
#include "Direct3D12Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TessellationControlShaderHlsl::TessellationControlShaderHlsl(Direct3D12Renderer &direct3D12Renderer, const uint8_t *bytecode, uint32_t numberOfBytes) :
		ITessellationControlShader(direct3D12Renderer),
		mD3DBlobHullShader(nullptr)
	{
		// Backup the hull shader bytecode
		D3DCreateBlob(numberOfBytes, &mD3DBlobHullShader);
		memcpy(mD3DBlobHullShader->GetBufferPointer(), bytecode, numberOfBytes);
	}

	TessellationControlShaderHlsl::TessellationControlShaderHlsl(Direct3D12Renderer &direct3D12Renderer, const char *sourceCode) :
		ITessellationControlShader(direct3D12Renderer),
		mD3DBlobHullShader(nullptr)
	{
		// Create the Direct3D 12 binary large object for the hull shader
		mD3DBlobHullShader = static_cast<ShaderLanguageHlsl*>(direct3D12Renderer.getShaderLanguage())->loadShader("hs_5_0", sourceCode, nullptr);

		// Don't assign a default name to the resource for debugging purposes, Direct3D 12 automatically sets a decent default name
	}

	TessellationControlShaderHlsl::~TessellationControlShaderHlsl()
	{
		// Release the Direct3D 12 shader binary large object
		if (nullptr != mD3DBlobHullShader)
		{
			mD3DBlobHullShader->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char *TessellationControlShaderHlsl::getShaderLanguageName() const
	{
		return ShaderLanguageHlsl::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
