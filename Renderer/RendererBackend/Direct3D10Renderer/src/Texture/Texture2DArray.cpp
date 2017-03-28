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
		mGenerateMipmaps(false),
		mD3D10Texture2D(nullptr),
		mD3D10ShaderResourceViewTexture(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D10Renderer)

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height) : 1;
		mGenerateMipmaps = (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET));

		// Direct3D 10 2D array texture description
		const DXGI_FORMAT dxgiFormat = static_cast<DXGI_FORMAT>(Mapping::getDirect3D10Format(textureFormat));
		D3D10_TEXTURE2D_DESC d3d10Texture2DDesc;
		d3d10Texture2DDesc.Width			  = width;
		d3d10Texture2DDesc.Height			  = height;
		d3d10Texture2DDesc.MipLevels		  = (generateMipmaps ? 0u : numberOfMipmaps);	// 0 = Let Direct3D 10 allocate the complete mipmap chain for us
		d3d10Texture2DDesc.ArraySize		  = numberOfSlices;
		d3d10Texture2DDesc.Format			  = dxgiFormat;
		d3d10Texture2DDesc.SampleDesc.Count	  = 1;
		d3d10Texture2DDesc.SampleDesc.Quality = 0;
		d3d10Texture2DDesc.Usage			  = static_cast<D3D10_USAGE>(textureUsage);	// These constants directly map to Direct3D constants, do not change them
		d3d10Texture2DDesc.BindFlags		  = D3D10_BIND_SHADER_RESOURCE;
		d3d10Texture2DDesc.CPUAccessFlags	  = (Renderer::TextureUsage::DYNAMIC == textureUsage) ? D3D10_CPU_ACCESS_WRITE : 0u;
		d3d10Texture2DDesc.MiscFlags		  = mGenerateMipmaps ? D3D10_RESOURCE_MISC_GENERATE_MIPS : 0u;

		// Use this texture as render target?
		if (flags & Renderer::TextureFlag::RENDER_TARGET)
		{
			d3d10Texture2DDesc.BindFlags |= D3D10_BIND_RENDER_TARGET;
		}

		// Create the Direct3D 10 2D texture instance
		// Did the user provided us with any texture data?
		ID3D10Device *d3d10Device = direct3D10Renderer.getD3D10Device();
		if (nullptr != data)
		{
			if (generateMipmaps)
			{
				// Let Direct3D 10 generate the mipmaps for us automatically
				// -> Sadly, it's impossible to use initialization data in this use-case
				d3d10Device->CreateTexture2D(&d3d10Texture2DDesc, nullptr, &mD3D10Texture2D);
				if (nullptr != mD3D10Texture2D)
				{
					{ // Update Direct3D 10 subresource data of the base-map
						const uint32_t  bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
						const uint32_t  bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
						for (uint32_t arraySlice = 0; arraySlice < numberOfSlices; ++arraySlice)
						{
							d3d10Device->UpdateSubresource(mD3D10Texture2D, D3D10CalcSubresource(0, arraySlice, numberOfMipmaps), nullptr, data, bytesPerRow, bytesPerSlice);

							// Move on to the next slice
							data = static_cast<const uint8_t*>(data) + bytesPerSlice;
						}
					}

					// Let Direct3D 10 generate the mipmaps for us automatically
					D3DX10FilterTexture(mD3D10Texture2D, 0, D3DX10_DEFAULT);
				}
			}
			else
			{
				// We don't want dynamic allocations, so we limit the maximum number of mipmaps and hence are able to use the efficient C runtime stack
				static const uint32_t MAXIMUM_NUMBER_OF_MIPMAPS = 15;	// A 16384x16384 texture has 15 mipmaps
				static const uint32_t MAXIMUM_NUMBER_OF_SLICES = 10;
				assert(numberOfMipmaps <= MAXIMUM_NUMBER_OF_MIPMAPS);
				D3D10_SUBRESOURCE_DATA d3d10SubresourceDataStack[MAXIMUM_NUMBER_OF_SLICES * MAXIMUM_NUMBER_OF_MIPMAPS];
				D3D10_SUBRESOURCE_DATA* d3d10SubresourceData = (numberOfSlices <= MAXIMUM_NUMBER_OF_SLICES) ? d3d10SubresourceDataStack : new D3D10_SUBRESOURCE_DATA[numberOfSlices];

				// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
				if (dataContainsMipmaps)
				{
					// Upload all mipmaps
					uint32_t d3d10SubresourceDataIndex = 0;
					for (uint32_t arraySlice = 0; arraySlice < numberOfSlices; ++arraySlice)
					{
						uint32_t currentWidth = width;
						uint32_t currentHeight = height;
						for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
						{
							// Upload the current mipmap
							D3D10_SUBRESOURCE_DATA& currentD3d10SubresourceData = d3d10SubresourceData[d3d10SubresourceDataIndex];
							currentD3d10SubresourceData.pSysMem			 = data;
							currentD3d10SubresourceData.SysMemPitch		 = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, currentWidth);
							currentD3d10SubresourceData.SysMemSlicePitch = 0;	// Only relevant for 3D textures

							// Move on to the next mipmap
							++d3d10SubresourceDataIndex;
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
					for (uint32_t arraySlice = 0; arraySlice < numberOfSlices; ++arraySlice)
					{
						D3D10_SUBRESOURCE_DATA& currentD3d10SubresourceData = d3d10SubresourceData[arraySlice];
						currentD3d10SubresourceData.pSysMem			 = data;
						currentD3d10SubresourceData.SysMemPitch		 = bytesPerRow;
						currentD3d10SubresourceData.SysMemSlicePitch = 0;	// Only relevant for 3D textures

						// Move on to the next slice
						data = static_cast<const uint8_t*>(data) + bytesPerSlice;
					}
				}
				d3d10Device->CreateTexture2D(&d3d10Texture2DDesc, d3d10SubresourceData, &mD3D10Texture2D);
				if (numberOfSlices > MAXIMUM_NUMBER_OF_SLICES)
				{
					delete [] d3d10SubresourceData;
				}
			}
		}
		else
		{
			// The user did not provide us with texture data
			d3d10Device->CreateTexture2D(&d3d10Texture2DDesc, nullptr, &mD3D10Texture2D);
		}

		// Create the Direct3D 10 shader resource view instance
		if (nullptr != mD3D10Texture2D)
		{
			// Direct3D 10 shader resource view description
			D3D10_SHADER_RESOURCE_VIEW_DESC d3d10ShaderResourceViewDesc = {};
			d3d10ShaderResourceViewDesc.Format							= dxgiFormat;
			d3d10ShaderResourceViewDesc.ViewDimension					= D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
			d3d10ShaderResourceViewDesc.Texture2DArray.MipLevels		= numberOfMipmaps;
			d3d10ShaderResourceViewDesc.Texture2DArray.MostDetailedMip	= 0;
			d3d10ShaderResourceViewDesc.Texture2DArray.ArraySize		= numberOfSlices;

			// Create the Direct3D 10 shader resource view instance
			direct3D10Renderer.getD3D10Device()->CreateShaderResourceView(mD3D10Texture2D, &d3d10ShaderResourceViewDesc, &mD3D10ShaderResourceViewTexture);
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
