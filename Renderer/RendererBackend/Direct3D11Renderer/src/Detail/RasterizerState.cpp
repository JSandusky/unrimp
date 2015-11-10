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
#include "Direct3D11Renderer/Detail/RasterizerState.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"

#include <Renderer/RasterizerStateTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RasterizerState::RasterizerState(Direct3D11Renderer &direct3D11Renderer, const Renderer::RasterizerState &rasterizerState) :
		mD3D11RasterizerState(nullptr)
	{
		// Create the Direct3D 11 rasterizer state
		// -> Thank's to Direct3D 12, "Renderer::RasterizerState" doesn't map directly to Direct3D 10 & 11 - but at least the constants directly still map
		D3D11_RASTERIZER_DESC d3d11RasterizerDesc;
		d3d11RasterizerDesc.FillMode				= static_cast<D3D11_FILL_MODE>(rasterizerState.fillMode);
		d3d11RasterizerDesc.CullMode				= static_cast<D3D11_CULL_MODE>(rasterizerState.cullMode);
		d3d11RasterizerDesc.FrontCounterClockwise	= rasterizerState.frontCounterClockwise;
		d3d11RasterizerDesc.DepthBias				= rasterizerState.depthBias;
		d3d11RasterizerDesc.DepthBiasClamp			= rasterizerState.depthBiasClamp;
		d3d11RasterizerDesc.SlopeScaledDepthBias	= rasterizerState.slopeScaledDepthBias;
		d3d11RasterizerDesc.DepthClipEnable			= rasterizerState.depthClipEnable;
		d3d11RasterizerDesc.ScissorEnable			= rasterizerState.scissorEnable;
		d3d11RasterizerDesc.MultisampleEnable		= rasterizerState.multisampleEnable;
		d3d11RasterizerDesc.AntialiasedLineEnable	= rasterizerState.antialiasedLineEnable;
		direct3D11Renderer.getD3D11Device()->CreateRasterizerState(&d3d11RasterizerDesc, &mD3D11RasterizerState);

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			setDebugName("Rasterizer state");
		#endif
	}

	RasterizerState::~RasterizerState()
	{
		// Release the Direct3D 11 rasterizer state
		if (nullptr != mD3D11RasterizerState)
		{
			mD3D11RasterizerState->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Direct3D11Renderer::IState methods     ]
	//[-------------------------------------------------------]
	void RasterizerState::setDebugName(const char *name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Valid Direct3D 11 rasterizer state?
			if (nullptr != mD3D11RasterizerState)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D11RasterizerState->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11RasterizerState->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
