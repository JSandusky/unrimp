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
#include "Direct3D10Renderer/Texture/TextureCube.h"
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
	TextureCube::TextureCube(Direct3D10Renderer &direct3D10Renderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *, uint32_t, Renderer::TextureUsage) :
		ITextureCube(direct3D10Renderer, width, height),
		mTextureFormat(textureFormat),
		mGenerateMipmaps(false),
		mD3D10TextureCube(nullptr),
		mD3D10ShaderResourceViewTexture(nullptr)
	{
		// TODO(co) Implement Direct3D 10 cube texture
		/*
		// Sanity checks
		assert(0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS) || nullptr != data);

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D10Renderer)

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height) : 1;
		mGenerateMipmaps = (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET));

		// Direct3D 10 cube texture description
		DXGI_FORMAT dxgiFormat = static_cast<DXGI_FORMAT>(Mapping::getDirect3D10Format(textureFormat));
		D3D10_TEXTURECUBE_DESC d3d10TextureCubeDesc;
		d3d10TextureCubeDesc.Width				= width;
		d3d10TextureCubeDesc.Height				= height;
		d3d10TextureCubeDesc.MipLevels			= (generateMipmaps ? 0u : numberOfMipmaps);	// 0 = Let Direct3D 10 allocate the complete mipmap chain for us
		d3d10TextureCubeDesc.ArraySize			= 1;
		d3d10TextureCubeDesc.Format				= dxgiFormat;
		d3d10TextureCubeDesc.SampleDesc.Count	= 1;
		d3d10TextureCubeDesc.SampleDesc.Quality	= 0;
		d3d10TextureCubeDesc.Usage				= static_cast<D3D10_USAGE>(textureUsage);	// These constants directly map to Direct3D constants, do not change them
		d3d10TextureCubeDesc.BindFlags			= D3D10_BIND_SHADER_RESOURCE;
		d3d10TextureCubeDesc.CPUAccessFlags		= 0;
		d3d10TextureCubeDesc.MiscFlags			= (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET)) ? D3D10_RESOURCE_MISC_GENERATE_MIPS : 0u;

		// Use this texture as render target?
		const bool isDepthFormat = Renderer::TextureFormat::isDepth(textureFormat);
		if (flags & Renderer::TextureFlag::RENDER_TARGET)
		{
			if (isDepthFormat)
			{
				d3d10TextureCubeDesc.BindFlags |= D3D10_BIND_DEPTH_STENCIL;

				// See "Direct3D10Renderer::TextureCube::getTextureFormat()" for details
				d3d10TextureCubeDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				dxgiFormat = DXGI_FORMAT_R32_FLOAT;
			}
			else
			{
				d3d10TextureCubeDesc.BindFlags |= D3D10_BIND_RENDER_TARGET;
			}
		}

		// Create the Direct3D 10 cube texture instance
		// Did the user provided us with any texture data?
		if (nullptr != data)
		{
			if (generateMipmaps)
			{
				// Let Direct3D 10 generate the mipmaps for us automatically
				// -> Sadly, it's impossible to use initialization data in this use-case
				direct3D10Renderer.getD3D10Device()->CreateTextureCube(&d3d10TextureCubeDesc, nullptr, &mD3D10TextureCube);
				if (nullptr != mD3D10TextureCube)
				{
					{ // Update Direct3D 10 subresource data of the base-map
						const uint32_t bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
						const uint32_t bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
						direct3D10Renderer.getD3D10Device()->UpdateSubresource(mD3D10TextureCube, 0, nullptr, data, bytesPerRow, bytesPerSlice);
					}

					// Let Direct3D 10 generate the mipmaps for us automatically
					D3DX10FilterTexture(mD3D10TextureCube, 0, D3DX10_DEFAULT);
				}
			}
			else
			{
				// We don't want dynamic allocations, so we limit the maximum number of mipmaps and hence are able to use the efficient C runtime stack
				static const uint32_t MAXIMUM_NUMBER_OF_MIPMAPS = 15;	// A 16384x16384 texture has 15 mipmaps
				assert(numberOfMipmaps <= MAXIMUM_NUMBER_OF_MIPMAPS);
				D3D10_SUBRESOURCE_DATA d3d10SubresourceData[MAXIMUM_NUMBER_OF_MIPMAPS];

				// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
				if (dataContainsMipmaps)
				{
					// Upload all mipmaps
					for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
					{
						// Upload the current mipmap
						D3D10_SUBRESOURCE_DATA& currentD3d10SubresourceData = d3d10SubresourceData[mipmap];
						currentD3d10SubresourceData.pSysMem			 = data;
						currentD3d10SubresourceData.SysMemPitch		 = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
						currentD3d10SubresourceData.SysMemSlicePitch = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);

						// Move on to the next mipmap
						data = static_cast<const uint8_t*>(data) + currentD3d10SubresourceData.SysMemSlicePitch;
						width = std::max(width >> 1, 1u);	// /= 2
						height = std::max(height >> 1, 1u);	// /= 2
					}
				}
				else
				{
					// The user only provided us with the base texture, no mipmaps
					d3d10SubresourceData->pSysMem		   = data;
					d3d10SubresourceData->SysMemPitch	   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
					d3d10SubresourceData->SysMemSlicePitch = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
				}
				direct3D10Renderer.getD3D10Device()->CreateTextureCube(&d3d10TextureCubeDesc, d3d10SubresourceData, &mD3D10TextureCube);
			}
		}
		else
		{
			// The user did not provide us with texture data
			direct3D10Renderer.getD3D10Device()->CreateTextureCube(&d3d10TextureCubeDesc, nullptr, &mD3D10TextureCube);
		}

		// Create the Direct3D 10 shader resource view instance
		if (nullptr != mD3D10TextureCube)
		{
			// Direct3D 10 shader resource view description
			D3D10_SHADER_RESOURCE_VIEW_DESC d3d10ShaderResourceViewDesc = {};
			d3d10ShaderResourceViewDesc.Format						= dxgiFormat;
			d3d10ShaderResourceViewDesc.ViewDimension				= D3D10_SRV_DIMENSION_TEXTURECUBE;
			d3d10ShaderResourceViewDesc.TextureCube.MipLevels		= numberOfMipmaps;
			d3d10ShaderResourceViewDesc.TextureCube.MostDetailedMip	= 0;

			// Create the Direct3D 10 shader resource view instance
			direct3D10Renderer.getD3D10Device()->CreateShaderResourceView(mD3D10TextureCube, &d3d10ShaderResourceViewDesc, &mD3D10ShaderResourceViewTexture);
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			setDebugName("Cube texture");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D10Renderer)
		*/
	}

	TextureCube::~TextureCube()
	{
		if (nullptr != mD3D10ShaderResourceViewTexture)
		{
			mD3D10ShaderResourceViewTexture->Release();
		}
		// TODO(co) Implement Direct3D 10 cube texture
		/*
		if (nullptr != mD3D10TextureCube)
		{
			mD3D10TextureCube->Release();
		}
		*/
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void TextureCube::setDebugName(const char *name)
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
				if (nullptr != mD3D10TextureCube)
				{
					// TODO(co) Implement Direct3D 10 cube texture
					/*
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
					mD3D10TextureCube->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					mD3D10TextureCube->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
					*/
				}
			}
		#endif
	}

	void* TextureCube::getInternalResourceHandle() const
	{
		return mD3D10TextureCube;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer