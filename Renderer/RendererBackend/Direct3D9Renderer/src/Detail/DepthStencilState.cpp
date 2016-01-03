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
#include "Direct3D9Renderer/Detail/DepthStencilState.h"
#include "Direct3D9Renderer/d3d9.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	DepthStencilState::DepthStencilState(const Renderer::DepthStencilState &depthStencilState) :
		mDepthStencilState(depthStencilState)
	{
		// Nothing to do in here
	}

	DepthStencilState::~DepthStencilState()
	{
		// Nothing to do in here
	}

	void DepthStencilState::setDirect3D9DepthStencilStates(IDirect3DDevice9 &direct3DDevice9) const
	{
		// Renderer::DepthStencilState::depthEnable
		direct3DDevice9.SetRenderState(D3DRS_ZENABLE, static_cast<DWORD>(mDepthStencilState.depthEnable));

		// Renderer::DepthStencilState::depthWriteMask
		direct3DDevice9.SetRenderState(D3DRS_ZWRITEENABLE, static_cast<DWORD>((Renderer::DepthWriteMask::ALL == mDepthStencilState.depthWriteMask) ? TRUE : FALSE));

		// TODO(co) Map the rest of the depth stencil states
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
