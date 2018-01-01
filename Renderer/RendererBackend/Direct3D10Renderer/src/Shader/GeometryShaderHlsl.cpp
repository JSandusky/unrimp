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
#include "Direct3D10Renderer/Shader/GeometryShaderHlsl.h"
#include "Direct3D10Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"

#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	GeometryShaderHlsl::GeometryShaderHlsl(Direct3D10Renderer& direct3D10Renderer, const Renderer::ShaderBytecode& shaderBytecode) :
		IGeometryShader(direct3D10Renderer),
		mD3D10GeometryShader(nullptr)
	{
		// Create the Direct3D 10 geometry shader
		direct3D10Renderer.getD3D10Device()->CreateGeometryShader(shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes(), &mD3D10GeometryShader);

		// Don't assign a default name to the resource for debugging purposes, Direct3D 10 automatically sets a decent default name
	}

	GeometryShaderHlsl::GeometryShaderHlsl(Direct3D10Renderer& direct3D10Renderer, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode) :
		IGeometryShader(direct3D10Renderer),
		mD3D10GeometryShader(nullptr)
	{
		// Create the Direct3D 10 binary large object for the geometry shader
		ID3DBlob* d3dBlob = static_cast<ShaderLanguageHlsl*>(direct3D10Renderer.getShaderLanguage())->loadShaderFromSourcecode("gs_4_0", sourceCode, nullptr);
		if (nullptr != d3dBlob)
		{
			// Create the Direct3D 10 geometry shader
			direct3D10Renderer.getD3D10Device()->CreateGeometryShader(d3dBlob->GetBufferPointer(), d3dBlob->GetBufferSize(), &mD3D10GeometryShader);

			// Return shader bytecode, if requested do to so
			if (nullptr != shaderBytecode)
			{
				shaderBytecode->setBytecodeCopy(static_cast<uint32_t>(d3dBlob->GetBufferSize()), static_cast<uint8_t*>(d3dBlob->GetBufferPointer()));
			}

			// Release the Direct3D 10 shader binary large object
			d3dBlob->Release();
		}

		// Don't assign a default name to the resource for debugging purposes, Direct3D 10 automatically sets a decent default name
	}

	GeometryShaderHlsl::~GeometryShaderHlsl()
	{
		// Release the Direct3D 10 geometry shader
		if (nullptr != mD3D10GeometryShader)
		{
			mD3D10GeometryShader->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void GeometryShaderHlsl::setDebugName(const char* name)
		{
			// Valid Direct3D 10 geometry shader?
			if (nullptr != mD3D10GeometryShader)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D10GeometryShader->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10GeometryShader->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		}
	#else
		void GeometryShaderHlsl::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


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
} // Direct3D10Renderer
