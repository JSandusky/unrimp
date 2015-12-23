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
#include "Direct3D12Renderer/Texture2DArray.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/Mapping.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture2DArray::Texture2DArray(Direct3D12Renderer& direct3D12Renderer, uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum, void*, uint32_t, Renderer::TextureUsage) :
		ITexture2DArray(direct3D12Renderer, width, height, numberOfSlices)
	//	mD3D12ShaderResourceViewTexture(nullptr)	// TODO(co) Direct3D 12 update
	{
		// TODO(co) Direct3D 12 update
		/*
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D12Renderer)

		// Generate mipmaps?
		const bool mipmaps = (flags & Renderer::TextureFlag::GENERATE_MIPMAPS) != 0;

		// Direct3D 12 2D array texture description
		D3D12_TEXTURE2D_DESC d3d12Texture2DDesc;
		d3d12Texture2DDesc.Width			  = width;
		d3d12Texture2DDesc.Height			  = height;
		d3d12Texture2DDesc.MipLevels		  = mipmaps ? 0u : 1u;	// 0 = Let Direct3D 12 allocate the complete mipmap chain for us
		d3d12Texture2DDesc.ArraySize		  = numberOfSlices;
		d3d12Texture2DDesc.Format			  = static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(textureFormat));
		d3d12Texture2DDesc.SampleDesc.Count	  = 1;
		d3d12Texture2DDesc.SampleDesc.Quality = 0;
		d3d12Texture2DDesc.Usage			  = static_cast<D3D12_USAGE>(textureUsage);	// These constants directly map to Direct3D constants, do not change them
		d3d12Texture2DDesc.BindFlags		  = D3D12_BIND_SHADER_RESOURCE;
		d3d12Texture2DDesc.CPUAccessFlags	  = 0;
		d3d12Texture2DDesc.MiscFlags		  = 0;

		// Use this texture as render target?
		if (flags & Renderer::TextureFlag::RENDER_TARGET)
		{
			d3d12Texture2DDesc.BindFlags |= D3D12_BIND_RENDER_TARGET;
		}

		// Create the Direct3D 12 2D array texture instance
		// -> Do not provide the data at once or creating mipmaps will get somewhat complicated
		ID3D12Texture2D *d3d12Texture2D = nullptr;
		direct3D12Renderer.getD3D12Device()->CreateTexture2D(&d3d12Texture2DDesc, nullptr, &d3d12Texture2D);
		if (nullptr != d3d12Texture2D)
		{
			// Calculate the number of mipmaps
			const uint32_t numberOfMipmaps = mipmaps ? getNumberOfMipmaps(width, height) : 1;

			// Data given?
			if (nullptr != data)
			{
				{ // Update Direct3D 12 subresource data of the base-map
					const uint32_t  bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
					const uint32_t  bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
					const uint8_t  *dataCurrent   = static_cast<uint8_t*>(data);
					for (uint32_t arraySlice = 0; arraySlice < numberOfSlices; ++arraySlice, dataCurrent += bytesPerSlice)
					{
						direct3D12Renderer.getD3D12DeviceContext()->UpdateSubresource(d3d12Texture2D, D3D12CalcSubresource(0, arraySlice, numberOfMipmaps), nullptr, dataCurrent, bytesPerRow, bytesPerSlice);
					}
				}

				// Let Direct3D 12 generate the mipmaps for us automatically?
				if (mipmaps)
				{
					// TODO(co) Direct3D 12 update
					// D3DX12FilterTexture(direct3D12Renderer.getD3D12DeviceContext(), d3d12Texture2D, 0, D3DX12_DEFAULT);
				}
			}

			// Direct3D 12 shader resource view description
			D3D12_SHADER_RESOURCE_VIEW_DESC d3d12ShaderResourceViewDesc;
			::ZeroMemory(&d3d12ShaderResourceViewDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
			d3d12ShaderResourceViewDesc.Format							= d3d12Texture2DDesc.Format;
			d3d12ShaderResourceViewDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			d3d12ShaderResourceViewDesc.Texture2DArray.MipLevels		= numberOfMipmaps;
			d3d12ShaderResourceViewDesc.Texture2DArray.MostDetailedMip	= 0;
			d3d12ShaderResourceViewDesc.Texture2DArray.ArraySize		= numberOfSlices;

			// Create the Direct3D 12 shader resource view instance
			direct3D12Renderer.getD3D12Device()->CreateShaderResourceView(d3d12Texture2D, &d3d12ShaderResourceViewDesc, &mD3D12ShaderResourceViewTexture);

			// Release the Direct3D 12 2D array texture instance
			d3d12Texture2D->Release();
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("2D texture array");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D12Renderer)
		*/
	}

	Texture2DArray::~Texture2DArray()
	{
		// TODO(co) Direct3D 12 update
		/*
		if (nullptr != mD3D12ShaderResourceViewTexture)
		{
			mD3D12ShaderResourceViewTexture->Release();
		}
		*/
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Texture2DArray::setDebugName(const char *)
	{
		// TODO(co) Direct3D 12 update
		/*
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Valid Direct3D 12 shader resource view?
			if (nullptr != mD3D12ShaderResourceViewTexture)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12ShaderResourceViewTexture->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12ShaderResourceViewTexture->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);

				// Do also set the given debug name to the Direct3D 12 resource referenced by the Direct3D resource view
				// -> In our use case, this resource is tightly coupled with the view
				// -> In principle the user can assign another resource to the view, but our interface documentation
				//    asks the user to not do so, so we ignore this situation when assigning the name
				ID3D12Resource *d3d12Resource = nullptr;
				mD3D12ShaderResourceViewTexture->GetResource(&d3d12Resource);
				if (nullptr != d3d12Resource)
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
					d3d12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					d3d12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);

					// Release the Direct3D 12 resource instance
					d3d12Resource->Release();
				}
			}
		#endif
		*/
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
