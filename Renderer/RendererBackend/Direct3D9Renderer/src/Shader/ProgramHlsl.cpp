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
#include "Direct3D9Renderer/Shader/ProgramHlsl.h"
#include "Direct3D9Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D9Renderer/Shader/FragmentShaderHlsl.h"
#include "Direct3D9Renderer/d3d9.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"

#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramHlsl::ProgramHlsl(Direct3D9Renderer& direct3D9Renderer, VertexShaderHlsl* vertexShaderHlsl, FragmentShaderHlsl* fragmentShaderHlsl) :
		IProgram(direct3D9Renderer),
		mDirect3D9Renderer(&direct3D9Renderer),
		mVertexShaderHlsl(vertexShaderHlsl),
		mFragmentShaderHlsl(fragmentShaderHlsl),
		mDirect3DDevice9(nullptr),
		mD3DXConstantTable(nullptr)
	{
		// Add references to the provided shaders
		if (nullptr != mVertexShaderHlsl)
		{
			mVertexShaderHlsl->addReference();

			// Valid Direct3D 9 vertex shader?
			IDirect3DVertexShader9* direct3DVertexShader9 = mVertexShaderHlsl->getDirect3DVertexShader9();
			if (nullptr != direct3DVertexShader9)
			{
				// Get the Direct3D 9 device
				// -> The "IDirect3DVertexShader9::GetDevice()"-method documentation does not mention whether
				//    or not the Direct3D 9 device reference counter is increased automatically
				// -> The "IDirect3DResource9::GetDevice()"-method documentation on the other hand states
				//    that the Direct3D 9 device reference counter is increased automatically
				// -> So, I just have to assume that Direct3D 9 has a consistent interface, hopefully...
				direct3DVertexShader9->GetDevice(&mDirect3DDevice9);

				// Get the Direct3D 9 constant table and acquire our reference
				mD3DXConstantTable = mVertexShaderHlsl->getD3DXConstantTable();
				if (nullptr != mD3DXConstantTable)
				{
					mD3DXConstantTable->AddRef();
				}
			}
		}
		if (nullptr != mFragmentShaderHlsl)
		{
			mFragmentShaderHlsl->addReference();

			// If required, get the Direct3D 9 device
			// -> See reference counter behaviour documentation above
			if (nullptr == mDirect3DDevice9 && mFragmentShaderHlsl->getDirect3DPixelShader9())
			{
				mFragmentShaderHlsl->getDirect3DPixelShader9()->GetDevice(&mDirect3DDevice9);
			}

			// If required, get the Direct3D 9 constant table and acquire our reference
			if (nullptr == mD3DXConstantTable)
			{
				mD3DXConstantTable = mFragmentShaderHlsl->getD3DXConstantTable();
				if (nullptr != mD3DXConstantTable)
				{
					mD3DXConstantTable->AddRef();
				}
			}
		}
	}

	ProgramHlsl::~ProgramHlsl()
	{
		// Release the Direct3D 9 constant table
		if (nullptr != mD3DXConstantTable)
		{
			mD3DXConstantTable->Release();
		}

		// Release the shader references
		if (nullptr != mVertexShaderHlsl)
		{
			mVertexShaderHlsl->releaseReference();
		}
		if (nullptr != mFragmentShaderHlsl)
		{
			mFragmentShaderHlsl->releaseReference();
		}

		// Release our Direct3D 9 device reference
		if (nullptr != mDirect3DDevice9)
		{
			mDirect3DDevice9->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void ProgramHlsl::setDebugName(const char*)
	{
		// In here we could assign the given debug name to all shaders assigned to the program,
		// but this might end up within a naming chaos due to overwriting possible already set
		// names... don't do this...
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	handle ProgramHlsl::getUniformHandle(const char* uniformName)
	{
		// Get the uniform handle
		if (nullptr != mVertexShaderHlsl && nullptr != mVertexShaderHlsl->getD3DXConstantTable())
		{
			const D3DXHANDLE d3dXHandle = mVertexShaderHlsl->getD3DXConstantTable()->GetConstantByName(nullptr, uniformName);
			if (nullptr != d3dXHandle)
			{
				// Done
				return reinterpret_cast<handle>(d3dXHandle);
			}
		}
		if (nullptr != mFragmentShaderHlsl && nullptr != mFragmentShaderHlsl->getD3DXConstantTable())
		{
			const D3DXHANDLE d3dXHandle = mFragmentShaderHlsl->getD3DXConstantTable()->GetConstantByName(nullptr, uniformName);
			if (nullptr != d3dXHandle)
			{
				// Done
				return reinterpret_cast<handle>(d3dXHandle);
			}
		}

		// Error!
		return NULL_HANDLE;
	}

	void ProgramHlsl::setUniform1f(handle uniformHandle, float value)
	{
		// Set the uniform
		if (nullptr != mDirect3DDevice9)
		{
			mD3DXConstantTable->SetFloat(mDirect3DDevice9, reinterpret_cast<D3DXHANDLE>(uniformHandle), value);
		}
	}

	void ProgramHlsl::setUniform2fv(handle uniformHandle, const float* value)
	{
		// Set the uniform
		if (nullptr != mDirect3DDevice9)
		{
			mD3DXConstantTable->SetFloatArray(mDirect3DDevice9, reinterpret_cast<D3DXHANDLE>(uniformHandle), value, 2);
		}
	}

	void ProgramHlsl::setUniform3fv(handle uniformHandle, const float* value)
	{
		// Set the uniform
		if (nullptr != mDirect3DDevice9)
		{
			mD3DXConstantTable->SetFloatArray(mDirect3DDevice9, reinterpret_cast<D3DXHANDLE>(uniformHandle), value, 3);
		}
	}

	void ProgramHlsl::setUniform4fv(handle uniformHandle, const float* value)
	{
		// Set the uniform
		if (nullptr != mDirect3DDevice9)
		{
			mD3DXConstantTable->SetFloatArray(mDirect3DDevice9, reinterpret_cast<D3DXHANDLE>(uniformHandle), value, 4);
		}
	}

	void ProgramHlsl::setUniformMatrix3fv(handle uniformHandle, const float* value)
	{
		// Set the uniform
		if (nullptr != mDirect3DDevice9)
		{
			mD3DXConstantTable->SetFloatArray(mDirect3DDevice9, reinterpret_cast<D3DXHANDLE>(uniformHandle), value, 3 * 3);
		}
	}

	void ProgramHlsl::setUniformMatrix4fv(handle uniformHandle, const float* value)
	{
		// Set the uniform
		if (nullptr != mDirect3DDevice9)
		{
			mD3DXConstantTable->SetFloatArray(mDirect3DDevice9, reinterpret_cast<D3DXHANDLE>(uniformHandle), value, 4 * 4);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void ProgramHlsl::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), ProgramHlsl, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
