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
#include "Direct3D10Renderer/State/BlendState.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"

#include <Renderer/State/BlendStateTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	BlendState::BlendState(Direct3D10Renderer &direct3D10Renderer, const Renderer::BlendState &blendState) :
		mD3D10BlendState(nullptr)
	{
		// Create the Direct3D 10 blend state
		const D3D10_BLEND_DESC d3d10BlendDesc =
		{
			blendState.alphaToCoverageEnable,										// AlphaToCoverageEnable (BOOL)
			blendState.renderTarget[0].blendEnable,									// BlendEnable[0] (BOOL)
			blendState.renderTarget[1].blendEnable,									// BlendEnable[1] (BOOL)
			blendState.renderTarget[2].blendEnable,									// BlendEnable[2] (BOOL)
			blendState.renderTarget[3].blendEnable,									// BlendEnable[3] (BOOL)
			blendState.renderTarget[4].blendEnable,									// BlendEnable[4] (BOOL)
			blendState.renderTarget[5].blendEnable,									// BlendEnable[5] (BOOL)
			blendState.renderTarget[6].blendEnable,									// BlendEnable[6] (BOOL)
			blendState.renderTarget[7].blendEnable,									// BlendEnable[7] (BOOL)
			static_cast<D3D10_BLEND>(blendState.renderTarget[0].srcBlend),			// SrcBlend (D3D10_BLEND)
			static_cast<D3D10_BLEND>(blendState.renderTarget[0].destBlend),			// DestBlend (D3D10_BLEND)
			static_cast<D3D10_BLEND_OP>(blendState.renderTarget[0].blendOp),		// BlendOp (D3D10_BLEND_OP)
			static_cast<D3D10_BLEND>(blendState.renderTarget[0].srcBlendAlpha),		// SrcBlendAlpha (D3D10_BLEND)
			static_cast<D3D10_BLEND>(blendState.renderTarget[0].destBlendAlpha),	// DestBlendAlpha (D3D10_BLEND)
			static_cast<D3D10_BLEND_OP>(blendState.renderTarget[0].blendOpAlpha),	// BlendOpAlpha (D3D10_BLEND_OP)
			blendState.renderTarget[0].renderTargetWriteMask,						// RenderTargetWriteMask[0] (UINT8)
			blendState.renderTarget[1].renderTargetWriteMask,						// RenderTargetWriteMask[1] (UINT8)
			blendState.renderTarget[2].renderTargetWriteMask,						// RenderTargetWriteMask[2] (UINT8)
			blendState.renderTarget[3].renderTargetWriteMask,						// RenderTargetWriteMask[3] (UINT8)
			blendState.renderTarget[4].renderTargetWriteMask,						// RenderTargetWriteMask[4] (UINT8)
			blendState.renderTarget[5].renderTargetWriteMask,						// RenderTargetWriteMask[5] (UINT8)
			blendState.renderTarget[6].renderTargetWriteMask,						// RenderTargetWriteMask[6] (UINT8)
			blendState.renderTarget[7].renderTargetWriteMask						// RenderTargetWriteMask[7] (UINT8)
		};
		direct3D10Renderer.getD3D10Device()->CreateBlendState(&d3d10BlendDesc, &mD3D10BlendState);

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			setDebugName("Blend state");
		#endif
	}

	BlendState::~BlendState()
	{
		// Release the Direct3D 10 blend state
		if (nullptr != mD3D10BlendState)
		{
			mD3D10BlendState->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Direct3D10Renderer::IState methods     ]
	//[-------------------------------------------------------]
	void BlendState::setDebugName(const char *name)
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			// Valid Direct3D 10 blend state?
			if (nullptr != mD3D10BlendState)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D10BlendState->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10BlendState->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
