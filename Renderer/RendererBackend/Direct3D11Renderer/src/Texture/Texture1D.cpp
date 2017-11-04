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
#include "Direct3D11Renderer/Texture/Texture1D.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/Mapping.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"
#include "Direct3D11Renderer/Direct3D11RuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture1D::Texture1D(Direct3D11Renderer& direct3D11Renderer, uint32_t width, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage textureUsage) :
		ITexture1D(direct3D11Renderer, width),
		mTextureFormat(textureFormat),
		mGenerateMipmaps(false),
		mD3D11Texture1D(nullptr),
		mD3D11ShaderResourceViewTexture(nullptr)
	{
		// Sanity checks
		assert(0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS) || nullptr != data);
		assert(((flags & Renderer::TextureFlag::RENDER_TARGET) == 0 || nullptr == data) && "Render target textures can't be filled using provided data");

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D11Renderer)

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width) : 1;
		mGenerateMipmaps = (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET));

		// Direct3D 11 1D texture description
		DXGI_FORMAT dxgiFormat = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(textureFormat));
		D3D11_TEXTURE1D_DESC d3d11Texture1DDesc;
		d3d11Texture1DDesc.Width		  = width;
		d3d11Texture1DDesc.MipLevels	  = (generateMipmaps ? 0u : numberOfMipmaps);	// 0 = Let Direct3D 11 allocate the complete mipmap chain for us
		d3d11Texture1DDesc.ArraySize	  = 1;
		d3d11Texture1DDesc.Format		  = dxgiFormat;
		d3d11Texture1DDesc.Usage		  = static_cast<D3D11_USAGE>(textureUsage);	// These constants directly map to Direct3D constants, do not change them
		d3d11Texture1DDesc.BindFlags	  = D3D11_BIND_SHADER_RESOURCE;
		d3d11Texture1DDesc.CPUAccessFlags = (Renderer::TextureUsage::DYNAMIC == textureUsage) ? D3D11_CPU_ACCESS_WRITE : 0u;
		d3d11Texture1DDesc.MiscFlags	  = mGenerateMipmaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0u;

		// Use this texture as render target?
		const bool isDepthFormat = Renderer::TextureFormat::isDepth(textureFormat);
		if (flags & Renderer::TextureFlag::RENDER_TARGET)
		{
			if (isDepthFormat)
			{
				d3d11Texture1DDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

				// See "Direct3D11Renderer::Texture1D::getTextureFormat()" for details
				d3d11Texture1DDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				dxgiFormat = DXGI_FORMAT_R32_FLOAT;
			}
			else
			{
				d3d11Texture1DDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			}
		}

		// Create the Direct3D 11 1D texture instance
		// Did the user provided us with any texture data?
		if (nullptr != data)
		{
			if (generateMipmaps)
			{
				// Let Direct3D 11 generate the mipmaps for us automatically
				// -> Sadly, it's impossible to use initialization data in this use-case
				direct3D11Renderer.getD3D11Device()->CreateTexture1D(&d3d11Texture1DDesc, nullptr, &mD3D11Texture1D);
				if (nullptr != mD3D11Texture1D)
				{
					{ // Update Direct3D 11 subresource data of the base-map
						const uint32_t bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
						const uint32_t bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, 1);
						direct3D11Renderer.getD3D11DeviceContext()->UpdateSubresource(mD3D11Texture1D, 0, nullptr, data, bytesPerRow, bytesPerSlice);
					}

					// Let Direct3D 11 generate the mipmaps for us automatically
					D3DX11FilterTexture(direct3D11Renderer.getD3D11DeviceContext(), mD3D11Texture1D, 0, D3DX11_DEFAULT);
				}
			}
			else
			{
				// We don't want dynamic allocations, so we limit the maximum number of mipmaps and hence are able to use the efficient C runtime stack
				static const uint32_t MAXIMUM_NUMBER_OF_MIPMAPS = 15;	// A 16384x16384 texture has 15 mipmaps
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
						currentD3d11SubresourceData.SysMemSlicePitch = 0;	// Only relevant for 3D textures

						// Move on to the next mipmap
						data = static_cast<const uint8_t*>(data) + currentD3d11SubresourceData.SysMemPitch;
						width = std::max(width >> 1, 1u);	// /= 2
					}
				}
				else
				{
					// The user only provided us with the base texture, no mipmaps
					d3d11SubresourceData->pSysMem		   = data;
					d3d11SubresourceData->SysMemPitch	   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
					d3d11SubresourceData->SysMemSlicePitch = 0;	// Only relevant for 3D textures
				}
				direct3D11Renderer.getD3D11Device()->CreateTexture1D(&d3d11Texture1DDesc, d3d11SubresourceData, &mD3D11Texture1D);
			}
		}
		else
		{
			// The user did not provide us with texture data
			direct3D11Renderer.getD3D11Device()->CreateTexture1D(&d3d11Texture1DDesc, nullptr, &mD3D11Texture1D);
		}

		// Create the Direct3D 11 shader resource view instance
		if (nullptr != mD3D11Texture1D)
		{
			// Direct3D 11 shader resource view description
			D3D11_SHADER_RESOURCE_VIEW_DESC d3d11ShaderResourceViewDesc = {};
			d3d11ShaderResourceViewDesc.Format					  = dxgiFormat;
			d3d11ShaderResourceViewDesc.ViewDimension			  = D3D11_SRV_DIMENSION_TEXTURE1D;
			d3d11ShaderResourceViewDesc.Texture1D.MipLevels		  = numberOfMipmaps;
			d3d11ShaderResourceViewDesc.Texture1D.MostDetailedMip = 0;

			// Create the Direct3D 11 shader resource view instance
			direct3D11Renderer.getD3D11Device()->CreateShaderResourceView(mD3D11Texture1D, &d3d11ShaderResourceViewDesc, &mD3D11ShaderResourceViewTexture);
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			setDebugName("1D texture");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D11Renderer)
	}

	Texture1D::~Texture1D()
	{
		if (nullptr != mD3D11ShaderResourceViewTexture)
		{
			mD3D11ShaderResourceViewTexture->Release();
		}
		if (nullptr != mD3D11Texture1D)
		{
			mD3D11Texture1D->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Texture1D::setDebugName(const char* name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Valid Direct3D 11 shader resource view?
			if (nullptr != mD3D11ShaderResourceViewTexture)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D11ShaderResourceViewTexture->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11ShaderResourceViewTexture->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);

				// Do also set the given debug name to the Direct3D 11 resource referenced by the Direct3D resource view
				if (nullptr != mD3D11Texture1D)
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
					mD3D11Texture1D->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					mD3D11Texture1D->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
				}
			}
		#endif
	}

	void* Texture1D::getInternalResourceHandle() const
	{
		return mD3D11Texture1D;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
