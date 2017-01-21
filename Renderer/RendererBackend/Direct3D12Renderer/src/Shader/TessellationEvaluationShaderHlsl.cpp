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
#include "Direct3D12Renderer/Shader/TessellationEvaluationShaderHlsl.h"
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
	TessellationEvaluationShaderHlsl::TessellationEvaluationShaderHlsl(Direct3D12Renderer &direct3D12Renderer, const uint8_t *bytecode, uint32_t numberOfBytes) :
		ITessellationEvaluationShader(direct3D12Renderer),
		mD3DBlobDomainShader(nullptr)
	{
		// Backup the domain shader bytecode
		D3DCreateBlob(numberOfBytes, &mD3DBlobDomainShader);
		memcpy(mD3DBlobDomainShader->GetBufferPointer(), bytecode, numberOfBytes);
	}

	TessellationEvaluationShaderHlsl::TessellationEvaluationShaderHlsl(Direct3D12Renderer &direct3D12Renderer, const char *sourceCode) :
		ITessellationEvaluationShader(direct3D12Renderer),
		mD3DBlobDomainShader(nullptr)
	{
		// Create the Direct3D 12 binary large object for the domain shader
		mD3DBlobDomainShader = static_cast<ShaderLanguageHlsl*>(direct3D12Renderer.getShaderLanguage())->loadShader("ds_5_0", sourceCode, nullptr);

		// Don't assign a default name to the resource for debugging purposes, Direct3D 12 automatically sets a decent default name
	}

	TessellationEvaluationShaderHlsl::~TessellationEvaluationShaderHlsl()
	{
		// Release the Direct3D 12 shader binary large object
		if (nullptr != mD3DBlobDomainShader)
		{
			mD3DBlobDomainShader->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char *TessellationEvaluationShaderHlsl::getShaderLanguageName() const
	{
		return ShaderLanguageHlsl::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
