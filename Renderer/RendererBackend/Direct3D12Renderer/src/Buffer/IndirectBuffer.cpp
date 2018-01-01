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
#include "Direct3D12Renderer/Buffer/IndirectBuffer.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12.h"
#include "Direct3D12Renderer/Mapping.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"

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
	IndirectBuffer::IndirectBuffer(Direct3D12Renderer& direct3D12Renderer, uint32_t numberOfBytes, const void* data, Renderer::BufferUsage) :
		IIndirectBuffer(direct3D12Renderer),
		mNumberOfBytes(numberOfBytes),
		mData(nullptr)
		// TODO(co) Direct3D 12 update
	//	mD3D12Buffer(nullptr),
	//	mD3D12ShaderResourceViewIndirect(nullptr)
	{
		// TODO(co) Direct3D 12 update
		if (mNumberOfBytes > 0)
		{
			mData = RENDERER_MALLOC_TYPED(direct3D12Renderer.getContext(), uint8_t, mNumberOfBytes);
			if (nullptr != data)
			{
				memcpy(mData, data, mNumberOfBytes);
			}
		}
		else
		{
			RENDERER_ASSERT(direct3D12Renderer.getContext(), nullptr == data, "Invalid Direct3D 12 indirect buffer data")
		}

		/*
		{ // Buffer part
			// Direct3D 12 buffer description
			D3D12_BUFFER_DESC d3d12BufferDesc;
			d3d12BufferDesc.ByteWidth           = numberOfBytes;
			d3d12BufferDesc.Usage               = static_cast<D3D12_USAGE>(Mapping::getDirect3D12UsageAndCPUAccessFlags(bufferUsage, d3d12BufferDesc.CPUAccessFlags));
			d3d12BufferDesc.BindFlags           = D3D12_BIND_SHADER_RESOURCE;
			//d3d12BufferDesc.CPUAccessFlags    = <filled above>;
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
		}

		{ // Shader resource view part
			// Direct3D 12 shader resource view description
			D3D12_SHADER_RESOURCE_VIEW_DESC d3d12ShaderResourceViewDesc = {};
			d3d12ShaderResourceViewDesc.Format				 = static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(textureFormat));
			d3d12ShaderResourceViewDesc.ViewDimension		 = D3D12_SRV_DIMENSION_BUFFER;
			d3d12ShaderResourceViewDesc.Buffer.ElementOffset = 0;
			d3d12ShaderResourceViewDesc.Buffer.ElementWidth	 = numberOfBytes / Renderer::TextureFormat::getNumberOfBytesPerElement(textureFormat);

			// Create the Direct3D 12 shader resource view instance
			direct3D12Renderer.getD3D12Device()->CreateShaderResourceView(mD3D12Buffer, &d3d12ShaderResourceViewDesc, &mD3D12ShaderResourceViewTexture);
		}

		// Assign a default name to the resource for debugging purposes
		#ifdef RENDERER_DEBUG
			setDebugName("");
		#endif
		*/
	}

	IndirectBuffer::~IndirectBuffer()
	{
		RENDERER_FREE(getRenderer().getContext(), mData);

		// TODO(co) Direct3D 12 update
		/*
		// Release the used resources
		if (nullptr != mD3D12ShaderResourceViewIndirect)
		{
			mD3D12ShaderResourceViewIndirect->Release();
			mD3D12ShaderResourceViewIndirect = nullptr;
		}
		if (nullptr != mD3D12Buffer)
		{
			mD3D12Buffer->Release();
			mD3D12Buffer = nullptr;
		}
		*/
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void IndirectBuffer::setDebugName(const char*)
		{
			// TODO(co) Direct3D 12 update
			/*
			RENDERER_DECORATED_DEBUG_NAME(name, detailedName, "IndirectBufferObject", 23);	// 23 = "IndirectBufferObject: " including terminating zero

			// Assign a debug name to the shader resource view
			if (nullptr != mD3D12ShaderResourceViewIndirect)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D12ShaderResourceViewIndirect->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12ShaderResourceViewIndirect->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(detailedName)), detailedName);
			}

			// Assign a debug name to the Indirect buffer
			if (nullptr != mD3D12Buffer)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D12Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(detailedName)), detailedName);
			}
		*/
		}
	#else
		void IndirectBuffer::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IIndirectBuffer methods      ]
	//[-------------------------------------------------------]
	void IndirectBuffer::copyDataFrom(uint32_t numberOfBytes, const void* data)
	{
		RENDERER_ASSERT(getRenderer().getContext(), numberOfBytes <= mNumberOfBytes, "Invalid Direct3D 12 indirect buffer data")
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != data, "Invalid Direct3D 12 indirect buffer data")
		memcpy(mData, data, numberOfBytes);

		// TODO(co) Direct3D 12 update
		/*
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
		*/
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void IndirectBuffer::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), IndirectBuffer, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
