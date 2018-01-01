/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "Direct3D12Renderer/Shader/GeometryShaderHlsl.h"
#include "Direct3D12Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"

#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	GeometryShaderHlsl::GeometryShaderHlsl(Direct3D12Renderer& direct3D12Renderer, const Renderer::ShaderBytecode& shaderBytecode) :
		IGeometryShader(direct3D12Renderer),
		mD3DBlobGeometryShader(nullptr)
	{
		// Backup the geometry shader bytecode
		D3DCreateBlob(shaderBytecode.getNumberOfBytes(), &mD3DBlobGeometryShader);
		memcpy(mD3DBlobGeometryShader->GetBufferPointer(), shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes());
	}

	GeometryShaderHlsl::GeometryShaderHlsl(Direct3D12Renderer& direct3D12Renderer, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode) :
		IGeometryShader(direct3D12Renderer),
		mD3DBlobGeometryShader(nullptr)
	{
		// Create the Direct3D 12 binary large object for the geometry shader
		mD3DBlobGeometryShader = static_cast<ShaderLanguageHlsl*>(direct3D12Renderer.getShaderLanguage())->loadShaderFromSourcecode("gs_5_0", sourceCode, nullptr);

		// Return shader bytecode, if requested do to so
		if (nullptr != shaderBytecode)
		{
			shaderBytecode->setBytecodeCopy(static_cast<uint32_t>(mD3DBlobGeometryShader->GetBufferSize()), static_cast<uint8_t*>(mD3DBlobGeometryShader->GetBufferPointer()));
		}

		// Don't assign a default name to the resource for debugging purposes, Direct3D 12 automatically sets a decent default name
	}

	GeometryShaderHlsl::~GeometryShaderHlsl()
	{
		// Release the Direct3D 12 shader binary large object
		if (nullptr != mD3DBlobGeometryShader)
		{
			mD3DBlobGeometryShader->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* GeometryShaderHlsl::getShaderLanguageName() const
	{
		return ShaderLanguageHlsl::NAME;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void GeometryShaderHlsl::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), GeometryShaderHlsl, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
