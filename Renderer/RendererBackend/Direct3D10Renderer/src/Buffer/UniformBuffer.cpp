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
#include "Direct3D10Renderer/Buffer/UniformBuffer.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Mapping.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"

#include <Renderer/IAssert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	UniformBuffer::UniformBuffer(Direct3D10Renderer& direct3D10Renderer, uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage) :
		Renderer::IUniformBuffer(direct3D10Renderer),
		mD3D10Buffer(nullptr)
	{
		{ // Sanity check
			// Check the given number of bytes, if we don't do this we might get told
			//   "... the ByteWidth (value = <x>) must be a multiple of 16 and be less than or equal to 65536"
			// by Direct3D 10
			const uint32_t leftOverBytes = (numberOfBytes % 16);
			if (0 != leftOverBytes)
			{
				// Fix the byte alignment, no assert because other renderer APIs have another alignment (DirectX 12 e.g. 256)
				numberOfBytes += 16 - (numberOfBytes % 16);
			}
		}

		// Direct3D 10 buffer description
		D3D10_BUFFER_DESC d3d10BufferDesc;
		d3d10BufferDesc.ByteWidth		 = numberOfBytes;
		d3d10BufferDesc.Usage			 = static_cast<D3D10_USAGE>(Mapping::getDirect3D10UsageAndCPUAccessFlags(bufferUsage, d3d10BufferDesc.CPUAccessFlags));
		d3d10BufferDesc.BindFlags		 = D3D10_BIND_CONSTANT_BUFFER;
		//d3d10BufferDesc.CPUAccessFlags = <filled above>;
		d3d10BufferDesc.MiscFlags		 = 0;

		// Data given?
		if (nullptr != data)
		{
			// Direct3D 10 subresource data
			D3D10_SUBRESOURCE_DATA d3d10SubresourceData;
			d3d10SubresourceData.pSysMem          = data;
			d3d10SubresourceData.SysMemPitch      = 0;
			d3d10SubresourceData.SysMemSlicePitch = 0;

			// Create the Direct3D 10 constant buffer
			direct3D10Renderer.getD3D10Device()->CreateBuffer(&d3d10BufferDesc, &d3d10SubresourceData, &mD3D10Buffer);
		}
		else
		{
			// Create the Direct3D 10 constant buffer
			direct3D10Renderer.getD3D10Device()->CreateBuffer(&d3d10BufferDesc, nullptr, &mD3D10Buffer);
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			setDebugName("");
		#endif
	}

	UniformBuffer::~UniformBuffer()
	{
		// Release the Direct3D 10 constant buffer
		if (nullptr != mD3D10Buffer)
		{
			mD3D10Buffer->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#if !defined(DIRECT3D10RENDERER_NO_DEBUG) && !defined(RENDERER_NO_DEBUG)
		void UniformBuffer::setDebugName(const char* name)
		{
			// Valid Direct3D 10 uniform buffer?
			if (nullptr != mD3D10Buffer)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				RENDERER_DECORATED_DEBUG_NAME(name, detailedName, "UBO", 6);	// 6 = "UBO: " including terminating zero!
				mD3D10Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(detailedName)), detailedName);
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
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != data, "Invalid Direct3D 10 uniform buffer data")
		if (nullptr != mD3D10Buffer)
		{
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&static_cast<Direct3D10Renderer&>(getRenderer()))

			// Update Direct3D 10 subresource data
			void* direct3D10Data = nullptr;
			const HRESULT hResult = mD3D10Buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &direct3D10Data);
			if (S_OK == hResult)
			{
				memcpy(direct3D10Data, data, numberOfBytes);
				mD3D10Buffer->Unmap();
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(&static_cast<Direct3D10Renderer&>(getRenderer()))
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
