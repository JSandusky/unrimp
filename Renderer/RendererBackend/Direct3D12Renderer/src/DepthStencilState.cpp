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
#include "Direct3D12Renderer/DepthStencilState.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"

#include <Renderer/DepthStencilStateTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	DepthStencilState::DepthStencilState(Direct3D12Renderer &direct3D12Renderer, const Renderer::DepthStencilState &) :
		IDepthStencilState(direct3D12Renderer)
		// mD3D12DepthStencilState(nullptr)	// TODO(co) Direct3D 12
	{
		// Create the Direct3D 12 depth stencil state
		// -> "Renderer::DepthStencilState" maps directly to Direct3D 10 & 11 && 12, do not change it
		// TODO(co) Direct3D 12
		// direct3D12Renderer.getD3D12Device()->CreateDepthStencilState(reinterpret_cast<const D3D12_DEPTH_STENCIL_DESC*>(&depthStencilState), &mD3D12DepthStencilState);

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("Depth stencil state");
		#endif
	}

	DepthStencilState::~DepthStencilState()
	{
		// Release the Direct3D 12 depth stencil state
		// TODO(co) Direct3D 12
		/*
		if (nullptr != mD3D12DepthStencilState)
		{
			mD3D12DepthStencilState->Release();
		}
		*/
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void DepthStencilState::setDebugName(const char *)
	{
		// TODO(co) Direct3D 12
		/*
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Valid Direct3D 12 depth stencil state?
			if (nullptr != mD3D12DepthStencilState)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12DepthStencilState->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12DepthStencilState->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
		*/
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
