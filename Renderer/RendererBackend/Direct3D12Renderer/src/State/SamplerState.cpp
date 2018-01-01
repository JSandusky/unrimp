/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "Direct3D12Renderer/State/SamplerState.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12X.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SamplerState::SamplerState(Direct3D12Renderer& direct3D12Renderer, const Renderer::SamplerState& samplerState) :
		ISamplerState(direct3D12Renderer),
		mD3D12DescriptorHeap(nullptr)
	{
		// Sanity checks
		RENDERER_ASSERT(direct3D12Renderer.getContext(), samplerState.filter != Renderer::FilterMode::UNKNOWN, "Direct3D 12 filter mode must not be unknown")
		RENDERER_ASSERT(direct3D12Renderer.getContext(), samplerState.maxAnisotropy <= direct3D12Renderer.getCapabilities().maximumAnisotropy, "Maximum Direct3D 12 anisotropy value violated")

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D12Renderer)

		ID3D12Device* d3d12Device = direct3D12Renderer.getD3D12Device();

		// Describe and create a sampler object descriptor heap.
		// Flags indicate that this descriptor heap can be bound to the pipeline 
		// and that descriptors contained in it can be referenced by a root table.
		D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
		d3d12DescriptorHeapDesc.NumDescriptors = 1;
		d3d12DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		d3d12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		if (SUCCEEDED(d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&mD3D12DescriptorHeap))))
		{
			// Create the sampler
			d3d12Device->CreateSampler(reinterpret_cast<const D3D12_SAMPLER_DESC*>(&samplerState), mD3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		}
		else
		{
			RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to create the Direct3D 12 sampler state descriptor heap")
		}

		// Assign a default name to the resource for debugging purposes
		#ifdef RENDERER_DEBUG
			setDebugName("Sampler state");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D12Renderer)
	}

	SamplerState::~SamplerState()
	{
		// Release the Direct3D 12 sampler state
		if (nullptr != mD3D12DescriptorHeap)
		{
			mD3D12DescriptorHeap->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void SamplerState::setDebugName(const char* name)
		{
			// Valid Direct3D 12 sampler state?
			if (nullptr != mD3D12DescriptorHeap)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D12DescriptorHeap->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12DescriptorHeap->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
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
} // Direct3D12Renderer
