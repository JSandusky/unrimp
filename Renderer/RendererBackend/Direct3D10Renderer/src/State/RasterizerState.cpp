/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include "Direct3D10Renderer/State/RasterizerState.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"

#include <Renderer/State/RasterizerStateTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RasterizerState::RasterizerState(Direct3D10Renderer &direct3D10Renderer, const Renderer::RasterizerState &rasterizerState) :
		mD3D10RasterizerState(nullptr)
	{
		// Create the Direct3D 10 rasterizer state
		// -> Thank's to Direct3D 12, "Renderer::RasterizerState" doesn't map directly to Direct3D 10 & 11 - but at least the constants directly still map
		D3D10_RASTERIZER_DESC d3d10RasterizerDesc;
		d3d10RasterizerDesc.FillMode				= static_cast<D3D10_FILL_MODE>(rasterizerState.fillMode);
		d3d10RasterizerDesc.CullMode				= static_cast<D3D10_CULL_MODE>(rasterizerState.cullMode);
		d3d10RasterizerDesc.FrontCounterClockwise	= rasterizerState.frontCounterClockwise;
		d3d10RasterizerDesc.DepthBias				= rasterizerState.depthBias;
		d3d10RasterizerDesc.DepthBiasClamp			= rasterizerState.depthBiasClamp;
		d3d10RasterizerDesc.SlopeScaledDepthBias	= rasterizerState.slopeScaledDepthBias;
		d3d10RasterizerDesc.DepthClipEnable			= rasterizerState.depthClipEnable;
		d3d10RasterizerDesc.ScissorEnable			= rasterizerState.scissorEnable;
		d3d10RasterizerDesc.MultisampleEnable		= rasterizerState.multisampleEnable;
		d3d10RasterizerDesc.AntialiasedLineEnable	= rasterizerState.antialiasedLineEnable;
		direct3D10Renderer.getD3D10Device()->CreateRasterizerState(&d3d10RasterizerDesc, &mD3D10RasterizerState);

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			setDebugName("Rasterizer state");
		#endif
	}

	RasterizerState::~RasterizerState()
	{
		// Release the Direct3D 10 rasterizer state
		if (nullptr != mD3D10RasterizerState)
		{
			mD3D10RasterizerState->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Direct3D10Renderer::IState methods     ]
	//[-------------------------------------------------------]
	void RasterizerState::setDebugName(const char *name)
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			// Valid Direct3D 10 rasterizer state?
			if (nullptr != mD3D10RasterizerState)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D10RasterizerState->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10RasterizerState->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
