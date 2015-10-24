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
#include "Direct3D12Renderer/UniformBuffer.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12.h"
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
	UniformBuffer::UniformBuffer(Direct3D12Renderer &direct3D12Renderer, uint32_t numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage) :
		Renderer::IUniformBuffer(direct3D12Renderer),
		mD3D12Buffer(nullptr)
	{
		// TODO(co) Uniform buffer byte alignment: Is it fine to support the user in here (ease of use) or is this 100% the responsibility of the user? (possible negative side effects)
		// Check the given number of bytes, if we don't do this we might get told
		//   "... the ByteWidth (value = <x>) must be a multiple of 16 and be less than or equal to 65536"
		// by Direct3D 12
		const uint32_t leftOverBytes = (numberOfBytes % 16);
		if (0 != leftOverBytes)
		{
			// Fix the byte alignment
			numberOfBytes += 16 - (numberOfBytes % 16);
		}

		// Direct3D 12 buffer description
		D3D12_BUFFER_DESC d3d12BufferDesc;
		d3d12BufferDesc.ByteWidth           = numberOfBytes;
		d3d12BufferDesc.Usage               = static_cast<D3D12_USAGE>(Mapping::getDirect3D12UsageAndCPUAccessFlags(bufferUsage, d3d12BufferDesc.CPUAccessFlags));
		d3d12BufferDesc.BindFlags           = D3D12_BIND_CONSTANT_BUFFER;
		//d3d12BufferDescd3d12BufferDesc.CPUAccessFlags    = <filled above>;
		d3d12BufferDesc.MiscFlags           = 0;
		d3d12BufferDesc.StructureByteStride = 0;

		// Data given?
		if (nullptr != data)
		{
			// Direct3D 12 subresource data
			D3D12_SUBRESOURCE_DATA d3d12SubresourceData;
			d3d12SubresourceData.pSysMem          = data;
			d3d12SubresourceData.SysMemPitch      = 0;
			d3d12SubresourceData.SysMemSlicePitch = 0;

			// Create the Direct3D 12 constant buffer
			direct3D12Renderer.getD3D12Device()->CreateBuffer(&d3d12BufferDesc, &d3d12SubresourceData, &mD3D12Buffer);
		}
		else
		{
			// Create the Direct3D 12 constant buffer
			direct3D12Renderer.getD3D12Device()->CreateBuffer(&d3d12BufferDesc, nullptr, &mD3D12Buffer);
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("UBO");
		#endif
	}

	UniformBuffer::~UniformBuffer()
	{
		// Release the Direct3D 12 constant buffer
		if (nullptr != mD3D12Buffer)
		{
			mD3D12Buffer->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void UniformBuffer::setDebugName(const char *name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Valid Direct3D 12 uniform buffer?
			if (nullptr != mD3D12Buffer)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IUniformBuffer methods       ]
	//[-------------------------------------------------------]
	void UniformBuffer::copyDataFrom(uint32_t numberOfBytes, const void *data)
	{
		// Check resource pointers
		if (nullptr != mD3D12Buffer && nullptr != data)
		{
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&getRenderer())

			// Get the Direct3D 12 device context
			ID3D12DeviceContext *d3d12DeviceContext = static_cast<Direct3D12Renderer&>(getRenderer()).getD3D12DeviceContext();

			// Update Direct3D 12 subresource data
			// -> Don't use (might fail): d3d12DeviceContext->UpdateSubresource(mD3D12Buffer, 0, nullptr, data, 0, 0);
			D3D12_MAPPED_SUBRESOURCE d3d12MappedSubresource;
			const HRESULT hResult = d3d12DeviceContext->Map(mD3D12Buffer, 0, D3D12_MAP_WRITE_DISCARD, 0, &d3d12MappedSubresource);
			if (S_OK == hResult)
			{
				memcpy(d3d12MappedSubresource.pData, data, numberOfBytes);
				d3d12DeviceContext->Unmap(mD3D12Buffer, 0);
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(&getRenderer())
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
