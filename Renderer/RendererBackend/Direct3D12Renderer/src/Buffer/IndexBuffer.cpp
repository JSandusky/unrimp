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
#include "Direct3D12Renderer/Buffer/IndexBuffer.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12X.h"
#include "Direct3D12Renderer/Mapping.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"

#include <Renderer/ILog.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndexBuffer::IndexBuffer(Direct3D12Renderer& direct3D12Renderer, uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data, Renderer::BufferUsage) :
		IIndexBuffer(direct3D12Renderer),
		mD3D12Resource(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D12Renderer)

		// "Renderer::IndexBufferFormat::UnsignedChar" is not supported by Direct3D 12
		// TODO(co) Check this, there's "DXGI_FORMAT_R8_UINT" which might work in Direct3D 12
		if (Renderer::IndexBufferFormat::UNSIGNED_CHAR == indexBufferFormat)
		{
			RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "\"Renderer::IndexBufferFormat::UNSIGNED_CHAR\" is not supported by Direct3D 12")
			mD3D12IndexBufferView.BufferLocation = 0;
			mD3D12IndexBufferView.SizeInBytes	 = 0;
			mD3D12IndexBufferView.Format		 = DXGI_FORMAT_UNKNOWN;
		}
		else
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
					// Copy the data to the index buffer
					UINT8* pIndexDataBegin;
					CD3DX12_RANGE readRange(0, 0);	// We do not intend to read from this resource on the CPU
					if (SUCCEEDED(mD3D12Resource->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin))))
					{
						memcpy(pIndexDataBegin, data, numberOfBytes);
						mD3D12Resource->Unmap(0, nullptr);
					}
					else
					{
						RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to map Direct3D 12 index buffer")
					}
				}

				// Fill the Direct3D 12 index buffer view
				mD3D12IndexBufferView.BufferLocation = mD3D12Resource->GetGPUVirtualAddress();
				mD3D12IndexBufferView.SizeInBytes	 = numberOfBytes;
				mD3D12IndexBufferView.Format		 = static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(indexBufferFormat));
			}
			else
			{
				RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to create Direct3D 12 index buffer resource")
				mD3D12IndexBufferView.BufferLocation = 0;
				mD3D12IndexBufferView.SizeInBytes	 = 0;
				mD3D12IndexBufferView.Format		 = DXGI_FORMAT_UNKNOWN;
			}
		}

		// Assign a default name to the resource for debugging purposes
		#ifdef RENDERER_DEBUG
			setDebugName("");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D12Renderer)
	}

	IndexBuffer::~IndexBuffer()
	{
		if (nullptr != mD3D12Resource)
		{
			mD3D12Resource->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void IndexBuffer::setDebugName(const char* name)
		{
			// Valid Direct3D 12 index buffer?
			if (nullptr != mD3D12Resource)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				RENDERER_DECORATED_DEBUG_NAME(name, detailedName, "IBO", 6);	// 6 = "IBO: " including terminating zero!
				mD3D12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(detailedName)), detailedName);
			}
		}
	#else
		void IndexBuffer::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void IndexBuffer::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), IndexBuffer, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
