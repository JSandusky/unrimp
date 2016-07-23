/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "Direct3D12Renderer/UniformBuffer.h"
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
	UniformBuffer::UniformBuffer(Direct3D12Renderer &direct3D12Renderer, uint32_t numberOfBytes, const void *data, Renderer::BufferUsage) :
		Renderer::IUniformBuffer(direct3D12Renderer),
		mD3D12Resource(nullptr),
		mD3D12DescriptorHeap(nullptr),
		mMappedData(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&getRenderer())

		ID3D12Device* d3d12Device = direct3D12Renderer.getD3D12Device();

		// Constant buffer size is required to be 256-byte aligned
		// - See "ID3D12Device::CreateConstantBufferView method": https://msdn.microsoft.com/de-de/library/windows/desktop/dn788659%28v=vs.85%29.aspx
		// - No assert because other renderer APIs have another alignment (DirectX 11 e.g. 16)
		const uint32_t numberOfBytesOnGpu = (numberOfBytes + 255) & ~255;

		// TODO(co) Add buffer usage setting support

		const CD3DX12_HEAP_PROPERTIES d3d12XHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC d3d12XResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(numberOfBytesOnGpu);
		if (SUCCEEDED(d3d12Device->CreateCommittedResource(
			&d3d12XHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&d3d12XResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mD3D12Resource))))
		{
			// Describe and create a constant buffer view (CBV) descriptor heap.
			// Flags indicate that this descriptor heap can be bound to the pipeline 
			// and that descriptors contained in it can be referenced by a root table.
			D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
			d3d12DescriptorHeapDesc.NumDescriptors = 1;
			d3d12DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			d3d12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			if (SUCCEEDED(d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&mD3D12DescriptorHeap))))
			{
				// Describe and create a constant buffer view
				D3D12_CONSTANT_BUFFER_VIEW_DESC d3dConstantBufferViewDesc = {};
				d3dConstantBufferViewDesc.BufferLocation = mD3D12Resource->GetGPUVirtualAddress();
				d3dConstantBufferViewDesc.SizeInBytes = numberOfBytesOnGpu;
				d3d12Device->CreateConstantBufferView(&d3dConstantBufferViewDesc, mD3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

				CD3DX12_RANGE readRange(0, 0);	// We do not intend to read from this resource on the CPU
				if (SUCCEEDED(mD3D12Resource->Map(0, &readRange, reinterpret_cast<void**>(&mMappedData))))
				{
					// Data given?
					if (nullptr != data)
					{
						memcpy(mMappedData, &data, numberOfBytes);
					}
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to map uniform buffer")
				}
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create uniform buffer descriptor heap")
			}
		}
		else
		{
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create uniform buffer resource")
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("UBO");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&getRenderer())
	}

	UniformBuffer::~UniformBuffer()
	{
		// Release the Direct3D 12 constant buffer
		if (nullptr != mD3D12Resource)
		{
			mD3D12Resource->Release();
		}
		if (nullptr != mD3D12DescriptorHeap)
		{
			mD3D12DescriptorHeap->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void UniformBuffer::setDebugName(const char *name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Valid Direct3D 12 uniform buffer?
			if (nullptr != mD3D12Resource)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
			if (nullptr != mD3D12DescriptorHeap)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12DescriptorHeap->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12DescriptorHeap->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IUniformBuffer methods       ]
	//[-------------------------------------------------------]
	void UniformBuffer::copyDataFrom(uint32_t numberOfBytes, const void *data)
	{
		// Check resource pointers
		if (nullptr != mMappedData && nullptr != data)
		{
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&getRenderer())

			// Copy data
			memcpy(mMappedData, data, numberOfBytes);

			// End debug event
			RENDERER_END_DEBUG_EVENT(&getRenderer())
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
