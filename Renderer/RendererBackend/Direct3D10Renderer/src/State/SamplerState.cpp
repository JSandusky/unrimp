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
#include "Direct3D10Renderer/State/SamplerState.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"

#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SamplerState::SamplerState(Direct3D10Renderer& direct3D10Renderer, const Renderer::SamplerState& samplerState) :
		ISamplerState(direct3D10Renderer),
		mD3D10SamplerState(nullptr)
	{
		// Sanity checks
		RENDERER_ASSERT(direct3D10Renderer.getContext(), samplerState.filter != Renderer::FilterMode::UNKNOWN, "Direct3D 10 filter mode must not be unknown")
		RENDERER_ASSERT(direct3D10Renderer.getContext(), samplerState.maxAnisotropy <= direct3D10Renderer.getCapabilities().maximumAnisotropy, "Direct3D 10 maximum anisotropy value violated")

		// Create the Direct3D 10 sampler state
		// -> "Renderer::SamplerState" maps directly to Direct3D 10 & 11, do not change it
		direct3D10Renderer.getD3D10Device()->CreateSamplerState(reinterpret_cast<const D3D10_SAMPLER_DESC*>(&samplerState), &mD3D10SamplerState);

		// Assign a default name to the resource for debugging purposes
		#ifdef RENDERER_DEBUG
			setDebugName("Sampler state");
		#endif
	}

	SamplerState::~SamplerState()
	{
		// Release the Direct3D 10 sampler state
		if (nullptr != mD3D10SamplerState)
		{
			mD3D10SamplerState->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void SamplerState::setDebugName(const char* name)
		{
			// Valid Direct3D 10 sampler state?
			if (nullptr != mD3D10SamplerState)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D10SamplerState->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10SamplerState->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		}
	#else
		void SamplerState::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void SamplerState::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), SamplerState, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
