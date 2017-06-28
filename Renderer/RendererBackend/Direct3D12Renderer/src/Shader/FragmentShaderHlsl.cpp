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
#include "Direct3D12Renderer/Shader/FragmentShaderHlsl.h"
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
	FragmentShaderHlsl::FragmentShaderHlsl(Direct3D12Renderer& direct3D12Renderer, const Renderer::ShaderBytecode& shaderBytecode) :
		IFragmentShader(direct3D12Renderer),
		mD3DBlobFragmentShader(nullptr)
	{
		// Backup the fragment shader bytecode
		D3DCreateBlob(shaderBytecode.getNumberOfBytes(), &mD3DBlobFragmentShader);
		memcpy(mD3DBlobFragmentShader->GetBufferPointer(), shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes());
	}

	FragmentShaderHlsl::FragmentShaderHlsl(Direct3D12Renderer& direct3D12Renderer, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode) :
		IFragmentShader(direct3D12Renderer),
		mD3DBlobFragmentShader(nullptr)
	{
		// Create the Direct3D 12 binary large object for the fragment shader
		mD3DBlobFragmentShader = static_cast<ShaderLanguageHlsl*>(direct3D12Renderer.getShaderLanguage())->loadShaderFromSourcecode("ps_5_0", sourceCode, nullptr);

		// Return shader bytecode, if requested do to so
		if (nullptr != shaderBytecode)
		{
			shaderBytecode->setBytecodeCopy(static_cast<uint32_t>(mD3DBlobFragmentShader->GetBufferSize()), static_cast<uint8_t*>(mD3DBlobFragmentShader->GetBufferPointer()));
		}

		// Don't assign a default name to the resource for debugging purposes, Direct3D 12 automatically sets a decent default name
	}

	FragmentShaderHlsl::~FragmentShaderHlsl()
	{
		// Release the Direct3D 12 shader binary large object
		if (nullptr != mD3DBlobFragmentShader)
		{
			mD3DBlobFragmentShader->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* FragmentShaderHlsl::getShaderLanguageName() const
	{
		return ShaderLanguageHlsl::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
