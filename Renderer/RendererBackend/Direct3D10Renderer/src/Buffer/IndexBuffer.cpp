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
#include "Direct3D10Renderer/Buffer/IndexBuffer.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Mapping.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndexBuffer::IndexBuffer(Direct3D10Renderer& direct3D10Renderer, uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data, Renderer::BufferUsage bufferUsage) :
		IIndexBuffer(direct3D10Renderer),
		mD3D10Buffer(nullptr),
		mDXGIFormat(DXGI_FORMAT_UNKNOWN)
	{
		// "Renderer::IndexBufferFormat::UNSIGNED_CHAR" is not supported by Direct3D 10
		if (Renderer::IndexBufferFormat::UNSIGNED_CHAR == indexBufferFormat)
		{
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: \"Renderer::IndexBufferFormat::UnsignedChar\" is not supported by Direct3D 10")
		}
		else
		{
			// Set the DXGI format
			mDXGIFormat = Mapping::getDirect3D10Format(indexBufferFormat);

			// Direct3D 10 buffer description
			D3D10_BUFFER_DESC d3d10BufferDesc;
			d3d10BufferDesc.ByteWidth        = numberOfBytes;
			d3d10BufferDesc.Usage            = static_cast<D3D10_USAGE>(Mapping::getDirect3D10UsageAndCPUAccessFlags(bufferUsage, d3d10BufferDesc.CPUAccessFlags));
			d3d10BufferDesc.BindFlags        = D3D10_BIND_INDEX_BUFFER;
			//d3d10BufferDesc.CPUAccessFlags = <filled above>;
			d3d10BufferDesc.MiscFlags        = 0;

			// Data given?
			if (nullptr != data)
			{
				// Direct3D 10 subresource data
				D3D10_SUBRESOURCE_DATA d3d10SubresourceData;
				d3d10SubresourceData.pSysMem          = data;
				d3d10SubresourceData.SysMemPitch      = 0;
				d3d10SubresourceData.SysMemSlicePitch = 0;

				// Create the Direct3D 10 index buffer
				direct3D10Renderer.getD3D10Device()->CreateBuffer(&d3d10BufferDesc, &d3d10SubresourceData, &mD3D10Buffer);
			}
			else
			{
				// Create the Direct3D 10 index buffer
				direct3D10Renderer.getD3D10Device()->CreateBuffer(&d3d10BufferDesc, nullptr, &mD3D10Buffer);
			}
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			setDebugName("");
		#endif
	}

	IndexBuffer::~IndexBuffer()
	{
		if (nullptr != mD3D10Buffer)
		{
			mD3D10Buffer->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#if !defined(DIRECT3D10RENDERER_NO_DEBUG) && !defined(RENDERER_NO_DEBUG)
		void IndexBuffer::setDebugName(const char* name)
		{
			// Valid Direct3D 10 index buffer?
			if (nullptr != mD3D10Buffer)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				RENDERER_DECORATED_DEBUG_NAME(name, detailedName, "IBO", 6);	// 6 = "IBO: " including terminating zero!
				mD3D10Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(detailedName)), detailedName);
			}
		}
	#else
		void IndexBuffer::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
