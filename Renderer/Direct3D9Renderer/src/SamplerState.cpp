/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "Direct3D9Renderer/SamplerState.h"
#include "Direct3D9Renderer/d3d9.h"
#include "Direct3D9Renderer/Mapping.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SamplerState::SamplerState(Direct3D9Renderer &direct3D9Renderer, const Renderer::SamplerState &samplerState) :
		ISamplerState(reinterpret_cast<Renderer::IRenderer&>(direct3D9Renderer)),
		mDirect3D9MagFilterMode(Mapping::getDirect3D9MagFilterMode(samplerState.filter)),
		mDirect3D9MinFilterMode(Mapping::getDirect3D9MinFilterMode(samplerState.filter)),
		mDirect3D9MipFilterMode((0.0f == samplerState.maxLOD) ? D3DTEXF_NONE : Mapping::getDirect3D9MipFilterMode(samplerState.filter)),	// In case "Renderer::SamplerState::maxLOD" is zero, disable mipmapping in order to ensure a correct behaviour when using Direct3D 9, float equal check is valid in here
		mDirect3D9AddressModeU(Mapping::getDirect3D9TextureAddressMode(samplerState.addressU)),
		mDirect3D9AddressModeV(Mapping::getDirect3D9TextureAddressMode(samplerState.addressV)),
		mDirect3D9AddressModeW(Mapping::getDirect3D9TextureAddressMode(samplerState.addressW)),
		mDirect3D9MipLODBias(*(reinterpret_cast<const DWORD*>(&samplerState.mipLODBias))),	// Direct3D 9 type is float, but has to be handed over by using DWORD
		mDirect3D9MaxAnisotropy(samplerState.maxAnisotropy),
		mDirect3DBorderColor(0),	// Set below
		mDirect3DMaxMipLevel((samplerState.minLOD > 0.0f) ? static_cast<unsigned long>(samplerState.minLOD) : 0)	// Direct3D 9 type is unsigned long, lookout the Direct3D 9 name is twisted and implies "Renderer::SamplerState::maxLOD" but it's really "Renderer::SamplerState::minLOD"
	{
		{ // Renderer::SamplerState::borderColor[4]
			// For Direct3D 9, the clear color must be between [0..1]
			float normalizedColor[4] = { samplerState.borderColor[0], samplerState.borderColor[1], samplerState.borderColor[2], samplerState.borderColor[3] } ;
			for (int i = 0; i < 4; ++i)
			{
				if (normalizedColor[i] < 0.0f)
				{
					normalizedColor[i] = 0.0f;
				}
				if (normalizedColor[i] > 1.0f)
				{
					normalizedColor[i] = 1.0f;
				}
			}
			#ifndef RENDERER_NO_DEBUG
				if (normalizedColor[0] != samplerState.borderColor[0] || normalizedColor[1] != samplerState.borderColor[1] || normalizedColor[2] != samplerState.borderColor[2] || normalizedColor[3] != samplerState.borderColor[3])
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 warning: The given border color was clamped to [0, 1] because Direct3D 9 does not support values outside this range")
				}
			#endif
			mDirect3DBorderColor = D3DCOLOR_COLORVALUE(normalizedColor[0], normalizedColor[1], normalizedColor[2], normalizedColor[3]);
		}
	}

	SamplerState::~SamplerState()
	{
		// Nothing to do in here
	}

	void SamplerState::setDirect3D9SamplerStates(uint32_t sampler, IDirect3DDevice9 &direct3DDevice9) const
	{
		// "IDirect3DDevice9::SetSamplerState()"-documentation: "D3DSAMPLERSTATETYPE Enumerated Type" at MSDN http://msdn.microsoft.com/en-us/library/windows/desktop/bb172602%28v=vs.85%29.aspx

		// Renderer::SamplerState::filter
		direct3DDevice9.SetSamplerState(sampler, D3DSAMP_MAGFILTER, mDirect3D9MagFilterMode);
		direct3DDevice9.SetSamplerState(sampler, D3DSAMP_MINFILTER, mDirect3D9MinFilterMode);
		direct3DDevice9.SetSamplerState(sampler, D3DSAMP_MIPFILTER, mDirect3D9MipFilterMode);

		// Renderer::SamplerState::addressU
		direct3DDevice9.SetSamplerState(sampler, D3DSAMP_ADDRESSU, mDirect3D9AddressModeU);

		// Renderer::SamplerState::addressV
		direct3DDevice9.SetSamplerState(sampler, D3DSAMP_ADDRESSV, mDirect3D9AddressModeV);

		// Renderer::SamplerState::addressW
		direct3DDevice9.SetSamplerState(sampler, D3DSAMP_ADDRESSW, mDirect3D9AddressModeW);

		// Renderer::SamplerState::mipLODBias
		direct3DDevice9.SetSamplerState(sampler, D3DSAMP_MIPMAPLODBIAS, mDirect3D9MipLODBias);

		// Renderer::SamplerState::maxAnisotropy
		direct3DDevice9.SetSamplerState(sampler, D3DSAMP_MAXANISOTROPY, mDirect3D9MaxAnisotropy);

		// Renderer::SamplerState::comparisonFunc
		// -> Not available in Direct3D 9

		// Renderer::SamplerState::borderColor[4]
		direct3DDevice9.SetSamplerState(sampler, D3DSAMP_BORDERCOLOR, mDirect3DBorderColor);

		// Renderer::SamplerState::minLOD
		direct3DDevice9.SetSamplerState(sampler, D3DSAMP_MAXMIPLEVEL, mDirect3DMaxMipLevel);

		// Renderer::SamplerState::maxLOD
		// -> Not available in Direct3D 9
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void SamplerState::setDebugName(const char *)
	{
		// There's no Direct3D 9 resource we could assign a debug name to
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
