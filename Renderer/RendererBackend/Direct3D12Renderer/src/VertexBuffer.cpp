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
#include "Direct3D12Renderer/VertexBuffer.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12X.h"
#include "Direct3D12Renderer/Mapping.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexBuffer::VertexBuffer(Direct3D12Renderer &direct3D12Renderer, uint32_t numberOfBytes, const void *data, Renderer::BufferUsage::Enum) :
		IVertexBuffer(direct3D12Renderer),
		mD3D12Resource(nullptr)
	{
		// TODO(co) This is only meant for the Direct3D 12 renderer backend kickoff.
		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.

		// TODO(co) Add buffer usage setting support

		const CD3DX12_HEAP_PROPERTIES d3d12XHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC d3d12XResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(numberOfBytes);
		if (SUCCEEDED(direct3D12Renderer.getD3D12Device()->CreateCommittedResource(
			&d3d12XHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&d3d12XResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mD3D12Resource))))
		{
			// Data given?
			if (nullptr != data)
			{
				// Copy the data to the vertex buffer
				UINT8* pVertexDataBegin;
				CD3DX12_RANGE readRange(0, 0);	// We do not intend to read from this resource on the CPU
				if (SUCCEEDED(mD3D12Resource->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin))))
				{
					memcpy(pVertexDataBegin, data, numberOfBytes);
					mD3D12Resource->Unmap(0, nullptr);
				}
			}
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("VBO");
		#endif
	}

	VertexBuffer::~VertexBuffer()
	{
		if (nullptr != mD3D12Resource)
		{
			mD3D12Resource->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void VertexBuffer::setDebugName(const char *name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Valid Direct3D 12 vertex buffer?
			if (nullptr != mD3D12Resource)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
