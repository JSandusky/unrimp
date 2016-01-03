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
#include "Direct3D11Renderer/Texture2D.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/Mapping.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"
#include "Direct3D11Renderer/Direct3D11RuntimeLinking.h"

#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture2D::Texture2D(Direct3D11Renderer &direct3D11Renderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage textureUsage) :
		ITexture2D(direct3D11Renderer, width, height),
		mD3D11ShaderResourceViewTexture(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D11Renderer)

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height) : 1;

		// Direct3D 11 2D texture description
		D3D11_TEXTURE2D_DESC d3d11Texture2DDesc;
		d3d11Texture2DDesc.Width			  = width;
		d3d11Texture2DDesc.Height			  = height;
		d3d11Texture2DDesc.MipLevels		  = (generateMipmaps ? 0u : numberOfMipmaps);	// 0 = Let Direct3D 11 allocate the complete mipmap chain for us
		d3d11Texture2DDesc.ArraySize		  = 1;
		d3d11Texture2DDesc.Format			  = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(textureFormat));
		d3d11Texture2DDesc.SampleDesc.Count	  = 1;
		d3d11Texture2DDesc.SampleDesc.Quality = 0;
		d3d11Texture2DDesc.Usage			  = static_cast<D3D11_USAGE>(textureUsage);	// These constants directly map to Direct3D constants, do not change them
		d3d11Texture2DDesc.BindFlags		  = D3D11_BIND_SHADER_RESOURCE;
		d3d11Texture2DDesc.CPUAccessFlags	  = 0;
		d3d11Texture2DDesc.MiscFlags		  = 0;

		// Use this texture as render target?
		if (flags & Renderer::TextureFlag::RENDER_TARGET)
		{
			d3d11Texture2DDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		}

		// Create the Direct3D 11 2D texture instance
		// Did the user provided us with any texture data?
		ID3D11Texture2D *d3d11Texture2D = nullptr;
		if (nullptr != data)
		{
			if (generateMipmaps)
			{
				// Let Direct3D 11 generate the mipmaps for us automatically
				// -> Sadly, it's impossible to use initialization data in this use-case
				direct3D11Renderer.getD3D11Device()->CreateTexture2D(&d3d11Texture2DDesc, nullptr, &d3d11Texture2D);
				if (nullptr != d3d11Texture2D)
				{
					{ // Update Direct3D 11 subresource data of the base-map
						const uint32_t bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
						const uint32_t bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
						direct3D11Renderer.getD3D11DeviceContext()->UpdateSubresource(d3d11Texture2D, 0, nullptr, data, bytesPerRow, bytesPerSlice);
					}

					// Let Direct3D 11 generate the mipmaps for us automatically
					D3DX11FilterTexture(direct3D11Renderer.getD3D11DeviceContext(), d3d11Texture2D, 0, D3DX11_DEFAULT);
				}
			}
			else
			{
				// We don't want dynamic allocations, so we limit the maximum number of mipmaps and hence are able to use the efficient C runtime stack
				static constexpr uint32_t MAXIMUM_NUMBER_OF_MIPMAPS = 15;	// A 16384x16384 texture has 15 mipmaps
				assert(numberOfMipmaps <= MAXIMUM_NUMBER_OF_MIPMAPS);
				D3D11_SUBRESOURCE_DATA d3d11SubresourceData[MAXIMUM_NUMBER_OF_MIPMAPS];

				// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
				if (dataContainsMipmaps)
				{
					// Upload all mipmaps
					for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
					{
						// Upload the current mipmap
						D3D11_SUBRESOURCE_DATA& currentD3d11SubresourceData = d3d11SubresourceData[mipmap];
						currentD3d11SubresourceData.pSysMem			 = data;
						currentD3d11SubresourceData.SysMemPitch		 = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
						currentD3d11SubresourceData.SysMemSlicePitch = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);

						// Move on to the next mipmap
						data = static_cast<uint8_t*>(data) + currentD3d11SubresourceData.SysMemSlicePitch;
						width = std::max(width >> 1, 1u);	// /= 2
						height = std::max(height >> 1, 1u);	// /= 2
					}
				}
				else
				{
					// The user only provided us with the base texture, no mipmaps
					d3d11SubresourceData->pSysMem		   = data;
					d3d11SubresourceData->SysMemPitch	   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
					d3d11SubresourceData->SysMemSlicePitch = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
				}
				direct3D11Renderer.getD3D11Device()->CreateTexture2D(&d3d11Texture2DDesc, d3d11SubresourceData, &d3d11Texture2D);
			}
		}
		else
		{
			// The user did not provide us with texture data
			direct3D11Renderer.getD3D11Device()->CreateTexture2D(&d3d11Texture2DDesc, nullptr, &d3d11Texture2D);
		}

		// Create the Direct3D 11 shader resource view instance
		if (nullptr != d3d11Texture2D)
		{
			// Direct3D 11 shader resource view description
			D3D11_SHADER_RESOURCE_VIEW_DESC d3d11ShaderResourceViewDesc;
			::ZeroMemory(&d3d11ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			d3d11ShaderResourceViewDesc.Format					  = d3d11Texture2DDesc.Format;
			d3d11ShaderResourceViewDesc.ViewDimension			  = D3D11_SRV_DIMENSION_TEXTURE2D;
			d3d11ShaderResourceViewDesc.Texture2D.MipLevels		  = numberOfMipmaps;
			d3d11ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

			// Create the Direct3D 11 shader resource view instance
			direct3D11Renderer.getD3D11Device()->CreateShaderResourceView(d3d11Texture2D, &d3d11ShaderResourceViewDesc, &mD3D11ShaderResourceViewTexture);

			// Release the Direct3D 11 2D texture instance
			d3d11Texture2D->Release();
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			setDebugName("2D texture");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D11Renderer)
	}

	Texture2D::~Texture2D()
	{
		if (nullptr != mD3D11ShaderResourceViewTexture)
		{
			mD3D11ShaderResourceViewTexture->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Texture2D::setDebugName(const char *name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Valid Direct3D 11 shader resource view?
			if (nullptr != mD3D11ShaderResourceViewTexture)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D11ShaderResourceViewTexture->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11ShaderResourceViewTexture->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);

				// Do also set the given debug name to the Direct3D 11 resource referenced by the Direct3D resource view
				// -> In our use case, this resource is tightly coupled with the view
				// -> In principle the user can assign another resource to the view, but our interface documentation
				//    asks the user to not do so, so we ignore this situation when assigning the name
				ID3D11Resource *d3d11Resource = nullptr;
				mD3D11ShaderResourceViewTexture->GetResource(&d3d11Resource);
				if (nullptr != d3d11Resource)
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
					d3d11Resource->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					d3d11Resource->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);

					// Release the Direct3D 11 resource instance
					d3d11Resource->Release();
				}
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
