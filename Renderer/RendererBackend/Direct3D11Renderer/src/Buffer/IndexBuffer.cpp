/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include "Direct3D11Renderer/Buffer/IndexBuffer.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Mapping.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndexBuffer::IndexBuffer(Direct3D11Renderer &direct3D11Renderer, uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage bufferUsage) :
		IIndexBuffer(direct3D11Renderer),
		mD3D11Buffer(nullptr),
		mDXGIFormat(DXGI_FORMAT_UNKNOWN)
	{
		// "Renderer::IndexBufferFormat::UnsignedChar" is not supported by Direct3D 11
		// -> See "Input assembler index buffer resources" ("D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER"): https://msdn.microsoft.com/en-us/library/windows/desktop/ff471325%28v=vs.85%29.aspx
		if (Renderer::IndexBufferFormat::UNSIGNED_CHAR == indexBufferFormat)
		{
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: \"Renderer::IndexBufferFormat::UNSIGNED_CHAR\" is not supported by Direct3D 11")
		}
		else
		{
			// Set the DXGI format
			mDXGIFormat = Mapping::getDirect3D11Format(indexBufferFormat);

			// Direct3D 11 buffer description
			D3D11_BUFFER_DESC d3d11BufferDesc;
			d3d11BufferDesc.ByteWidth           = numberOfBytes;
			d3d11BufferDesc.Usage               = static_cast<D3D11_USAGE>(Mapping::getDirect3D11UsageAndCPUAccessFlags(bufferUsage, d3d11BufferDesc.CPUAccessFlags));
			d3d11BufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
			//d3d11BufferDesc.CPUAccessFlags    = <filled above>;
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

				// Create the Direct3D 11 index buffer
				direct3D11Renderer.getD3D11Device()->CreateBuffer(&d3d11BufferDesc, &d3d11SubresourceData, &mD3D11Buffer);
			}
			else
			{
				// Create the Direct3D 11 index buffer
				direct3D11Renderer.getD3D11Device()->CreateBuffer(&d3d11BufferDesc, nullptr, &mD3D11Buffer);
			}
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			setDebugName("IBO");
		#endif
	}

	IndexBuffer::~IndexBuffer()
	{
		if (nullptr != mD3D11Buffer)
		{
			mD3D11Buffer->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void IndexBuffer::setDebugName(const char *name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Valid Direct3D 11 index buffer?
			if (nullptr != mD3D11Buffer)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D11Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
