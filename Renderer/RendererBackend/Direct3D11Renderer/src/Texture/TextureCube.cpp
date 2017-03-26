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
#include "Direct3D11Renderer/Texture/TextureCube.h"
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
	TextureCube::TextureCube(Direct3D11Renderer &direct3D11Renderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage textureUsage) :
		ITextureCube(direct3D11Renderer, width, height),
		mTextureFormat(textureFormat),
		mGenerateMipmaps(false),
		mD3D11TextureCube(nullptr),
		mD3D11ShaderResourceViewTexture(nullptr)
	{
		static const uint32_t NUMBER_OF_SLICES = 6;	// In Direct3D 11, a cube map is a 2D array texture with six slices

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D11Renderer)

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height) : 1;
		mGenerateMipmaps = (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET));

		// Direct3D 11 2D array texture description
		const DXGI_FORMAT dxgiFormat = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(textureFormat));
		D3D11_TEXTURE2D_DESC d3d11Texture2DDesc;
		d3d11Texture2DDesc.Width			  = width;
		d3d11Texture2DDesc.Height			  = height;
		d3d11Texture2DDesc.MipLevels		  = (generateMipmaps ? 0u : numberOfMipmaps);	// 0 = Let Direct3D 11 allocate the complete mipmap chain for us
		d3d11Texture2DDesc.ArraySize		  = NUMBER_OF_SLICES;
		d3d11Texture2DDesc.Format			  = dxgiFormat;
		d3d11Texture2DDesc.SampleDesc.Count	  = 1;
		d3d11Texture2DDesc.SampleDesc.Quality = 0;
		d3d11Texture2DDesc.Usage			  = static_cast<D3D11_USAGE>(textureUsage);	// These constants directly map to Direct3D constants, do not change them
		d3d11Texture2DDesc.BindFlags		  = D3D11_BIND_SHADER_RESOURCE;
		d3d11Texture2DDesc.CPUAccessFlags	  = (Renderer::TextureUsage::DYNAMIC == textureUsage) ? D3D11_CPU_ACCESS_WRITE : 0u;
		d3d11Texture2DDesc.MiscFlags		  = (mGenerateMipmaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0u) | D3D11_RESOURCE_MISC_TEXTURECUBE;

		// Use this texture as render target?
		if (flags & Renderer::TextureFlag::RENDER_TARGET)
		{
			d3d11Texture2DDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		}

		// Create the Direct3D 11 2D texture instance
		// Did the user provided us with any texture data?
		if (nullptr != data)
		{
			if (generateMipmaps)
			{
				// Let Direct3D 11 generate the mipmaps for us automatically
				// -> Sadly, it's impossible to use initialization data in this use-case
				direct3D11Renderer.getD3D11Device()->CreateTexture2D(&d3d11Texture2DDesc, nullptr, &mD3D11TextureCube);
				if (nullptr != mD3D11TextureCube)
				{
					ID3D11DeviceContext* d3d11DeviceContext = direct3D11Renderer.getD3D11DeviceContext();

					{ // Update Direct3D 11 subresource data of the base-map
						const uint32_t  bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
						const uint32_t  bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
						for (uint32_t arraySlice = 0; arraySlice < NUMBER_OF_SLICES; ++arraySlice)
						{
							d3d11DeviceContext->UpdateSubresource(mD3D11TextureCube, D3D11CalcSubresource(0, arraySlice, numberOfMipmaps), nullptr, data, bytesPerRow, bytesPerSlice);

							// Move on to the next slice
							data = static_cast<const uint8_t*>(data) + bytesPerSlice;
						}
					}

					// Let Direct3D 11 generate the mipmaps for us automatically
					D3DX11FilterTexture(d3d11DeviceContext, mD3D11TextureCube, 0, D3DX11_DEFAULT);
				}
			}
			else
			{
				// We don't want dynamic allocations, so we limit the maximum number of mipmaps and hence are able to use the efficient C runtime stack
				static const uint32_t MAXIMUM_NUMBER_OF_MIPMAPS = 15;	// A 16384x16384 texture has 15 mipmaps
				assert(numberOfMipmaps <= MAXIMUM_NUMBER_OF_MIPMAPS);
				D3D11_SUBRESOURCE_DATA d3d11SubresourceData[NUMBER_OF_SLICES * MAXIMUM_NUMBER_OF_MIPMAPS];

				// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
				if (dataContainsMipmaps)
				{
					// Upload all mipmaps
					uint32_t d3d11SubresourceDataIndex = 0;
					for (uint32_t arraySlice = 0; arraySlice < NUMBER_OF_SLICES; ++arraySlice)
					{
						uint32_t currentWidth = width;
						uint32_t currentHeight = height;
						for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
						{
							// Upload the current mipmap
							D3D11_SUBRESOURCE_DATA& currentD3d11SubresourceData = d3d11SubresourceData[d3d11SubresourceDataIndex];
							currentD3d11SubresourceData.pSysMem			 = data;
							currentD3d11SubresourceData.SysMemPitch		 = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, currentWidth);
							currentD3d11SubresourceData.SysMemSlicePitch = 0;	// Only relevant for 3D textures

							// Move on to the next mipmap
							++d3d11SubresourceDataIndex;
							data = static_cast<const uint8_t*>(data) + Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, currentWidth, currentHeight);
							currentWidth = std::max(currentWidth >> 1, 1u);		// /= 2
							currentHeight = std::max(currentHeight >> 1, 1u);	// /= 2
						}
					}
				}
				else
				{
					// The user only provided us with the base texture, no mipmaps
					const uint32_t bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
					const uint32_t bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
					for (uint32_t arraySlice = 0; arraySlice < NUMBER_OF_SLICES; ++arraySlice)
					{
						D3D11_SUBRESOURCE_DATA& currentD3d11SubresourceData = d3d11SubresourceData[arraySlice];
						currentD3d11SubresourceData.pSysMem			 = data;
						currentD3d11SubresourceData.SysMemPitch		 = bytesPerRow;
						currentD3d11SubresourceData.SysMemSlicePitch = 0;	// Only relevant for 3D textures

						// Move on to the next slice
						data = static_cast<const uint8_t*>(data) + bytesPerSlice;
					}
				}
				direct3D11Renderer.getD3D11Device()->CreateTexture2D(&d3d11Texture2DDesc, d3d11SubresourceData, &mD3D11TextureCube);
			}
		}
		else
		{
			// The user did not provide us with texture data
			direct3D11Renderer.getD3D11Device()->CreateTexture2D(&d3d11Texture2DDesc, nullptr, &mD3D11TextureCube);
		}

		// Create the Direct3D 11 shader resource view instance
		if (nullptr != mD3D11TextureCube)
		{
			// Direct3D 11 shader resource view description
			D3D11_SHADER_RESOURCE_VIEW_DESC d3d11ShaderResourceViewDesc = {};
			d3d11ShaderResourceViewDesc.Format						= dxgiFormat;
			d3d11ShaderResourceViewDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURECUBE;
			d3d11ShaderResourceViewDesc.TextureCube.MipLevels		= numberOfMipmaps;
			d3d11ShaderResourceViewDesc.TextureCube.MostDetailedMip	= 0;

			// Create the Direct3D 11 shader resource view instance
			direct3D11Renderer.getD3D11Device()->CreateShaderResourceView(mD3D11TextureCube, &d3d11ShaderResourceViewDesc, &mD3D11ShaderResourceViewTexture);
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			setDebugName("Cube texture");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D11Renderer)
	}

	TextureCube::~TextureCube()
	{
		if (nullptr != mD3D11ShaderResourceViewTexture)
		{
			mD3D11ShaderResourceViewTexture->Release();
		}
		if (nullptr != mD3D11TextureCube)
		{
			mD3D11TextureCube->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void TextureCube::setDebugName(const char *name)
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
				if (nullptr != mD3D11TextureCube)
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
					mD3D11TextureCube->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					mD3D11TextureCube->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
				}
			}
		#endif
	}

	void* TextureCube::getInternalResourceHandle() const
	{
		return mD3D11TextureCube;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
