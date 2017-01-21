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
#include "Direct3D10Renderer/Texture/Texture2DArray.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/Mapping.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"
#include "Direct3D10Renderer/Direct3D10RuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture2DArray::Texture2DArray(Direct3D10Renderer &direct3D10Renderer, uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage textureUsage) :
		ITexture2DArray(direct3D10Renderer, width, height, numberOfSlices),
		mD3D10ShaderResourceViewTexture(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D10Renderer)

		// Generate mipmaps?
		const bool mipmaps = (flags & Renderer::TextureFlag::GENERATE_MIPMAPS) != 0;

		// Direct3D 10 2D array texture description
		D3D10_TEXTURE2D_DESC d3d10Texture2DDesc;
		d3d10Texture2DDesc.Width			  = width;
		d3d10Texture2DDesc.Height			  = height;
		d3d10Texture2DDesc.MipLevels		  = mipmaps ? 0u : 1u;	// 0 = Let Direct3D 10 allocate the complete mipmap chain for us
		d3d10Texture2DDesc.ArraySize		  = numberOfSlices;
		d3d10Texture2DDesc.Format			  = static_cast<DXGI_FORMAT>(Mapping::getDirect3D10Format(textureFormat));
		d3d10Texture2DDesc.SampleDesc.Count	  = 1;
		d3d10Texture2DDesc.SampleDesc.Quality = 0;
		d3d10Texture2DDesc.Usage			  = static_cast<D3D10_USAGE>(textureUsage);	// These constants directly map to Direct3D constants, do not change them
		d3d10Texture2DDesc.BindFlags		  = D3D10_BIND_SHADER_RESOURCE;
		d3d10Texture2DDesc.CPUAccessFlags	  = 0;
		d3d10Texture2DDesc.MiscFlags		  = 0;

		// Use this texture as render target?
		if (flags & Renderer::TextureFlag::RENDER_TARGET)
		{
			d3d10Texture2DDesc.BindFlags |= D3D10_BIND_RENDER_TARGET;
		}

		// Create the Direct3D 10 2D array texture instance
		// -> Do not provide the data at once or creating mipmaps will get somewhat complicated
		ID3D10Texture2D *d3d10Texture2D = nullptr;
		direct3D10Renderer.getD3D10Device()->CreateTexture2D(&d3d10Texture2DDesc, nullptr, &d3d10Texture2D);
		if (nullptr != d3d10Texture2D)
		{
			// Calculate the number of mipmaps
			const uint32_t numberOfMipmaps = mipmaps ? getNumberOfMipmaps(width, height) : 1;

			// Data given?
			if (nullptr != data)
			{
				{ // Update Direct3D 10 subresource data of the base-map
					const uint32_t  bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
					const uint32_t  bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
					const uint8_t  *dataCurrent   = static_cast<const uint8_t*>(data);
					for (uint32_t arraySlice = 0; arraySlice < numberOfSlices; ++arraySlice, dataCurrent += bytesPerSlice)
					{
						direct3D10Renderer.getD3D10Device()->UpdateSubresource(d3d10Texture2D, D3D10CalcSubresource(0, arraySlice, numberOfMipmaps), nullptr, dataCurrent, bytesPerRow, bytesPerSlice);
					}
				}

				// Let Direct3D 10 generate the mipmaps for us automatically?
				if (mipmaps)
				{
					D3DX10FilterTexture(d3d10Texture2D, 0, D3DX10_DEFAULT);
				}
			}

			// Direct3D 10 shader resource view description
			D3D10_SHADER_RESOURCE_VIEW_DESC d3d10ShaderResourceViewDesc = {};
			d3d10ShaderResourceViewDesc.Format						   = d3d10Texture2DDesc.Format;
			d3d10ShaderResourceViewDesc.ViewDimension				   = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
			d3d10ShaderResourceViewDesc.Texture2DArray.MipLevels	   = numberOfMipmaps;
			d3d10ShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
			d3d10ShaderResourceViewDesc.Texture2DArray.ArraySize	   = numberOfSlices;

			// Create the Direct3D 10 shader resource view instance
			direct3D10Renderer.getD3D10Device()->CreateShaderResourceView(d3d10Texture2D, &d3d10ShaderResourceViewDesc, &mD3D10ShaderResourceViewTexture);

			// Release the Direct3D 10 2D array texture instance
			d3d10Texture2D->Release();
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			setDebugName("2D texture array");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D10Renderer)
	}

	Texture2DArray::~Texture2DArray()
	{
		if (nullptr != mD3D10ShaderResourceViewTexture)
		{
			mD3D10ShaderResourceViewTexture->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Texture2DArray::setDebugName(const char *name)
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			// Valid Direct3D 10 shader resource view?
			if (nullptr != mD3D10ShaderResourceViewTexture)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D10ShaderResourceViewTexture->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10ShaderResourceViewTexture->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);

				// Do also set the given debug name to the Direct3D 10 resource referenced by the Direct3D resource view
				// -> In our use case, this resource is tightly coupled with the view
				// -> In principle the user can assign another resource to the view, but our interface documentation
				//    asks the user to not do so, so we ignore this situation when assigning the name
				ID3D10Resource *d3d10Resource = nullptr;
				mD3D10ShaderResourceViewTexture->GetResource(&d3d10Resource);
				if (nullptr != d3d10Resource)
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
					d3d10Resource->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					d3d10Resource->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);

					// Release the Direct3D 10 resource instance
					d3d10Resource->Release();
				}
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
