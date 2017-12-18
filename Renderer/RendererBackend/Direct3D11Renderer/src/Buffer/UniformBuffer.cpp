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
#include "Direct3D11Renderer/Buffer/UniformBuffer.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Mapping.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"

#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	UniformBuffer::UniformBuffer(Direct3D11Renderer& direct3D11Renderer, uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage) :
		Renderer::IUniformBuffer(direct3D11Renderer),
		mD3D11Buffer(nullptr)
	{
		{ // Sanity check
			// Check the given number of bytes, if we don't do this we might get told
			//   "... the ByteWidth (value = <x>) must be a multiple of 16 and be less than or equal to 65536"
			// by Direct3D 11
			const uint32_t leftOverBytes = (numberOfBytes % 16);
			if (0 != leftOverBytes)
			{
				// Fix the byte alignment, no assert because other renderer APIs have another alignment (DirectX 12 e.g. 256)
				numberOfBytes += 16 - (numberOfBytes % 16);
			}
		}

		// Direct3D 11 buffer description
		D3D11_BUFFER_DESC d3d11BufferDesc;
		d3d11BufferDesc.ByteWidth           = numberOfBytes;
		d3d11BufferDesc.Usage               = static_cast<D3D11_USAGE>(Mapping::getDirect3D11UsageAndCPUAccessFlags(bufferUsage, d3d11BufferDesc.CPUAccessFlags));
		d3d11BufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
		//d3d11BufferDescd3d11BufferDesc.CPUAccessFlags    = <filled above>;
		d3d11BufferDesc.MiscFlags           = 0;
		d3d11BufferDesc.StructureByteStride = 0;

		// Data given?
		if (nullptr != data)
		{
			// Direct3D 11 subresource data
			D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
			d3d11SubresourceData.pSysMem          = data;
			d3d11SubresourceData.SysMemPitch      = 0;
			d3d11SubresourceData.SysMemSlicePitch = 0;

			// Create the Direct3D 11 constant buffer
			direct3D11Renderer.getD3D11Device()->CreateBuffer(&d3d11BufferDesc, &d3d11SubresourceData, &mD3D11Buffer);
		}
		else
		{
			// Create the Direct3D 11 constant buffer
			direct3D11Renderer.getD3D11Device()->CreateBuffer(&d3d11BufferDesc, nullptr, &mD3D11Buffer);
		}

		// Assign a default name to the resource for debugging purposes
		#ifdef RENDERER_DEBUG
			setDebugName("");
		#endif
	}

	UniformBuffer::~UniformBuffer()
	{
		// Release the Direct3D 11 constant buffer
		if (nullptr != mD3D11Buffer)
		{
			mD3D11Buffer->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void UniformBuffer::setDebugName(const char* name)
		{
			// Valid Direct3D 11 uniform buffer?
			if (nullptr != mD3D11Buffer)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				RENDERER_DECORATED_DEBUG_NAME(name, detailedName, "UBO", 6);	// 6 = "UBO: " including terminating zero
				mD3D11Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(detailedName)), detailedName);
			}
		}
	#else
		void UniformBuffer::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IUniformBuffer methods       ]
	//[-------------------------------------------------------]
	void UniformBuffer::copyDataFrom(uint32_t numberOfBytes, const void* data)
	{
		// Check resource pointers
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != data, "Invalid Direct3D 11 uniform buffer data")
		if (nullptr != mD3D11Buffer)
		{
			Direct3D11Renderer& direct3D11Renderer = static_cast<Direct3D11Renderer&>(getRenderer());

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D11Renderer)

			// Get the Direct3D 11 device context
			ID3D11DeviceContext* d3d11DeviceContext = direct3D11Renderer.getD3D11DeviceContext();

			// Update Direct3D 11 subresource data
			// -> Don't use (might fail): d3d11DeviceContext->UpdateSubresource(mD3D11Buffer, 0, nullptr, data, 0, 0);
			D3D11_MAPPED_SUBRESOURCE d3d11MappedSubresource;
			const HRESULT hResult = d3d11DeviceContext->Map(mD3D11Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11MappedSubresource);
			if (S_OK == hResult)
			{
				memcpy(d3d11MappedSubresource.pData, data, numberOfBytes);
				d3d11DeviceContext->Unmap(mD3D11Buffer, 0);
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(&direct3D11Renderer)
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void UniformBuffer::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), UniformBuffer, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
