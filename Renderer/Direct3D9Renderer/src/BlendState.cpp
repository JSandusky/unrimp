/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#include "Direct3D9Renderer/BlendState.h"
#include "Direct3D9Renderer/d3d9.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	BlendState::BlendState(Direct3D9Renderer &direct3D9Renderer, const Renderer::BlendState &blendState) :
		IBlendState(reinterpret_cast<Renderer::IRenderer&>(direct3D9Renderer)),
		mBlendState(blendState)
	{
		// Nothing to do in here
	}

	BlendState::~BlendState()
	{
		// Nothing to do in here
	}

	void BlendState::setDirect3D9BlendStates(IDirect3DDevice9 &direct3DDevice9) const
	{
		direct3DDevice9.SetRenderState(D3DRS_ALPHABLENDENABLE, static_cast<DWORD>(mBlendState.renderTarget[0].blendEnable));

		// TODO(co) Add more blend state options: Due to time limitations for now only fixed build in alpha blend setup in order to see a change
		direct3DDevice9.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		direct3DDevice9.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// TODO(co) Map the rest of the blend states
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void BlendState::setDebugName(const char *)
	{
		// There's no Direct3D 9 resource we could assign a debug name to
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
