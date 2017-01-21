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
#include "Direct3D9Renderer/State/RasterizerState.h"
#include "Direct3D9Renderer/d3d9.h"

#include <Renderer/State/RasterizerStateTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RasterizerState::RasterizerState(const Renderer::RasterizerState &rasterizerState) :
		mDirect3DFillMode(0),	// Set below
		mDirect3DCullMode(0),	// Set below
		mDirect3DDepthBias(*(reinterpret_cast<const DWORD*>(&rasterizerState.depthBias))),	// Direct3D 9 type is float, but has to be handed over by using DWORD
		mDirect3DSlopeScaledDepthBias(*(reinterpret_cast<const DWORD*>(&rasterizerState.slopeScaledDepthBias))),	// Direct3D 9 type is float, but has to be handed over by using DWORD
		mDirect3DScissorEnable(static_cast<unsigned long>(rasterizerState.scissorEnable)),
		mDirect3DMultisampleEnable(static_cast<unsigned long>(rasterizerState.multisampleEnable)),
		mDirect3DAntialiasedLineEnable(static_cast<unsigned long>(rasterizerState.antialiasedLineEnable))
	{
		// Renderer::RasterizerState::fillMode
		switch (rasterizerState.fillMode)
		{
			// Wireframe
			case Renderer::FillMode::WIREFRAME:
				mDirect3DFillMode = D3DFILL_WIREFRAME;
				break;

			// Solid
			default:
			case Renderer::FillMode::SOLID:
				mDirect3DFillMode = D3DFILL_SOLID;
				break;
		}

		// Renderer::RasterizerState::cullMode
		// Renderer::RasterizerState::frontCounterClockwise
		switch (rasterizerState.cullMode)
		{
			// No culling
			default:
			case Renderer::CullMode::NONE:
				mDirect3DCullMode = D3DCULL_NONE;
				break;

			// Selects clockwise polygons as front-facing
			case Renderer::CullMode::FRONT:
				mDirect3DCullMode = rasterizerState.frontCounterClockwise ? D3DCULL_CCW : D3DCULL_CW;
				break;

			// Selects counterclockwise polygons as front-facing
			case Renderer::CullMode::BACK:
				mDirect3DCullMode = rasterizerState.frontCounterClockwise ? D3DCULL_CW : D3DCULL_CCW;
				break;
		}
	}

	RasterizerState::~RasterizerState()
	{
		// Nothing here
	}

	void RasterizerState::setDirect3D9RasterizerStates(IDirect3DDevice9 &direct3DDevice9) const
	{
		// Renderer::RasterizerState::fillMode
		direct3DDevice9.SetRenderState(D3DRS_FILLMODE, mDirect3DFillMode);

		// Renderer::RasterizerState::cullMode
		// Renderer::RasterizerState::frontCounterClockwise
		direct3DDevice9.SetRenderState(D3DRS_CULLMODE, mDirect3DCullMode);

		// RasterizerState::depthBias
		direct3DDevice9.SetRenderState(D3DRS_DEPTHBIAS, mDirect3DDepthBias);

		// RasterizerState::depthBiasClamp
		// -> Not available in Direct3D 9

		// RasterizerState::slopeScaledDepthBias
		direct3DDevice9.SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, mDirect3DSlopeScaledDepthBias);

		// RasterizerState::depthClipEnable
		// TODO(co) Supported in Direct3D 9? I assume it's not...

		// RasterizerState::scissorEnable
		direct3DDevice9.SetRenderState(D3DRS_SCISSORTESTENABLE, mDirect3DScissorEnable);

		// RasterizerState::multisampleEnable
		direct3DDevice9.SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, mDirect3DMultisampleEnable);

		// RasterizerState::antialiasedLineEnable
		direct3DDevice9.SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, mDirect3DAntialiasedLineEnable);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
