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
#include "Direct3D10Renderer/Buffer/TextureBuffer.h"
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
	TextureBuffer::TextureBuffer(Direct3D10Renderer &direct3D10Renderer, uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data, Renderer::BufferUsage bufferUsage) :
		ITextureBuffer(direct3D10Renderer),
		mD3D10Buffer(nullptr),
		mD3D10ShaderResourceViewTexture(nullptr)
	{
		{ // Buffer part
			// Direct3D 10 buffer description
			D3D10_BUFFER_DESC d3d10BufferDesc;
			d3d10BufferDesc.ByteWidth        = numberOfBytes;
			d3d10BufferDesc.Usage            = static_cast<D3D10_USAGE>(Mapping::getDirect3D10UsageAndCPUAccessFlags(bufferUsage, d3d10BufferDesc.CPUAccessFlags));
			d3d10BufferDesc.BindFlags        = D3D10_BIND_SHADER_RESOURCE;
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

				// Create the Direct3D 10 constant buffer
				direct3D10Renderer.getD3D10Device()->CreateBuffer(&d3d10BufferDesc, &d3d10SubresourceData, &mD3D10Buffer);
			}
			else
			{
				// Create the Direct3D 10 constant buffer
				direct3D10Renderer.getD3D10Device()->CreateBuffer(&d3d10BufferDesc, nullptr, &mD3D10Buffer);
			}
		}

		{ // Shader resource view part
			// Direct3D 10 shader resource view description
			D3D10_SHADER_RESOURCE_VIEW_DESC d3d10ShaderResourceViewDesc = {};
			d3d10ShaderResourceViewDesc.Format				 = static_cast<DXGI_FORMAT>(Mapping::getDirect3D10Format(textureFormat));
			d3d10ShaderResourceViewDesc.ViewDimension		 = D3D10_SRV_DIMENSION_BUFFER;
			d3d10ShaderResourceViewDesc.Buffer.ElementOffset = 0;
			d3d10ShaderResourceViewDesc.Buffer.ElementWidth	 = numberOfBytes / Renderer::TextureFormat::getNumberOfBytesPerElement(textureFormat);

			// Create the Direct3D 10 shader resource view instance
			direct3D10Renderer.getD3D10Device()->CreateShaderResourceView(mD3D10Buffer, &d3d10ShaderResourceViewDesc, &mD3D10ShaderResourceViewTexture);
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			setDebugName("");
		#endif
	}

	TextureBuffer::~TextureBuffer()
	{
		// Release the used resources
		if (nullptr != mD3D10ShaderResourceViewTexture)
		{
			mD3D10ShaderResourceViewTexture->Release();
		}
		if (nullptr != mD3D10Buffer)
		{
			mD3D10Buffer->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#if !defined(DIRECT3D10RENDERER_NO_DEBUG) && !defined(RENDERER_NO_DEBUG)
		void TextureBuffer::setDebugName(const char *name)
		{
			RENDERER_DECORATED_DEBUG_NAME(name, detailedName, "TBO", 6);	// 6 = "TBO: " including terminating zero!

			// Assign a debug name to the shader resource view
			if (nullptr != mD3D10ShaderResourceViewTexture)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D10ShaderResourceViewTexture->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10ShaderResourceViewTexture->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(detailedName)), detailedName);
			}

			// Assign a debug name to the texture buffer
			if (nullptr != mD3D10Buffer)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D10Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10Buffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(detailedName)), detailedName);
			}
		}
	#else
		void TextureBuffer::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureBuffer methods       ]
	//[-------------------------------------------------------]
	void TextureBuffer::copyDataFrom(uint32_t numberOfBytes, const void *data)
	{
		// Check resource pointers
		assert(nullptr != data);
		if (nullptr != mD3D10Buffer)
		{
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&static_cast<Direct3D10Renderer&>(getRenderer()))

			// Update Direct3D 10 subresource data
			void *direct3D10Data = nullptr;
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
