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
#include "Direct3D11Renderer/State/DepthStencilState.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"

#include <Renderer/State/DepthStencilStateTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	DepthStencilState::DepthStencilState(Direct3D11Renderer &direct3D11Renderer, const Renderer::DepthStencilState &depthStencilState) :
		mD3D11DepthStencilState(nullptr)
	{
		// Create the Direct3D 11 depth stencil state
		// -> "Renderer::DepthStencilState" maps directly to Direct3D 10 & 11 & 12, do not change it
		direct3D11Renderer.getD3D11Device()->CreateDepthStencilState(reinterpret_cast<const D3D11_DEPTH_STENCIL_DESC*>(&depthStencilState), &mD3D11DepthStencilState);

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			setDebugName("Depth stencil state");
		#endif
	}

	DepthStencilState::~DepthStencilState()
	{
		// Release the Direct3D 11 depth stencil state
		if (nullptr != mD3D11DepthStencilState)
		{
			mD3D11DepthStencilState->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Direct3D11Renderer::IState methods     ]
	//[-------------------------------------------------------]
	void DepthStencilState::setDebugName(const char *name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Valid Direct3D 11 depth stencil state?
			if (nullptr != mD3D11DepthStencilState)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D11DepthStencilState->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11DepthStencilState->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
