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
#include "Direct3D11Renderer/Texture/Texture2DArray.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/Mapping.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"
#include "Direct3D11Renderer/Direct3D11RuntimeLinking.h"

#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture2DArray::Texture2DArray(Direct3D11Renderer& direct3D11Renderer, uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage textureUsage) :
		ITexture2DArray(direct3D11Renderer, width, height, numberOfSlices),
		mTextureFormat(textureFormat),
		mNumberOfMultisamples(1),	// TODO(co) Currently no MSAA support for 2D array textures
		mGenerateMipmaps(false),
		mD3D11Texture2D(nullptr),
		mD3D11ShaderResourceViewTexture(nullptr)
	{
		// Sanity checks
		RENDERER_ASSERT(direct3D11Renderer.getContext(), (flags & Renderer::TextureFlag::RENDER_TARGET) == 0 || nullptr == data, "Direct3D 11 render target textures can't be filled using provided data")

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D11Renderer)

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height) : 1;
		mGenerateMipmaps = (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET));

		// Direct3D 11 2D array texture description
		DXGI_FORMAT dxgiFormat = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(textureFormat));
		D3D11_TEXTURE2D_DESC d3d11Texture2DDesc;
		d3d11Texture2DDesc.Width			  = width;
		d3d11Texture2DDesc.Height			  = height;
		d3d11Texture2DDesc.MipLevels		  = (generateMipmaps ? 0u : numberOfMipmaps);	// 0 = Let Direct3D 11 allocate the complete mipmap chain for us
		d3d11Texture2DDesc.ArraySize		  = numberOfSlices;
		d3d11Texture2DDesc.Format			  = dxgiFormat;
		d3d11Texture2DDesc.SampleDesc.Count	  = 1;
		d3d11Texture2DDesc.SampleDesc.Quality = 0;
		d3d11Texture2DDesc.Usage			  = static_cast<D3D11_USAGE>(textureUsage);	// These constants directly map to Direct3D constants, do not change them
		d3d11Texture2DDesc.BindFlags		  = D3D11_BIND_SHADER_RESOURCE;
		d3d11Texture2DDesc.CPUAccessFlags	  = (Renderer::TextureUsage::DYNAMIC == textureUsage) ? D3D11_CPU_ACCESS_WRITE : 0u;
		d3d11Texture2DDesc.MiscFlags		  = mGenerateMipmaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0u;

		// Use this texture as render target?
		const bool isDepthFormat = Renderer::TextureFormat::isDepth(textureFormat);
		if (flags & Renderer::TextureFlag::RENDER_TARGET)
		{
			if (isDepthFormat)
			{
				d3d11Texture2DDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

				// See "Direct3D11Renderer::Texture2D::getTextureFormat()" for details
				d3d11Texture2DDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				dxgiFormat = DXGI_FORMAT_R32_FLOAT;
			}
			else
			{
				d3d11Texture2DDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			}
		}

		// Create the Direct3D 11 2D texture instance
		// Did the user provided us with any texture data?
		if (nullptr != data)
		{
			if (generateMipmaps)
			{
				// Let Direct3D 11 generate the mipmaps for us automatically
				// -> Sadly, it's impossible to use initialization data in this use-case
				direct3D11Renderer.getD3D11Device()->CreateTexture2D(&d3d11Texture2DDesc, nullptr, &mD3D11Texture2D);
				if (nullptr != mD3D11Texture2D)
				{
					ID3D11DeviceContext* d3d11DeviceContext = direct3D11Renderer.getD3D11DeviceContext();

					{ // Update Direct3D 11 subresource data of the base-map
						const uint32_t  bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
						const uint32_t  bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
						for (uint32_t arraySlice = 0; arraySlice < numberOfSlices; ++arraySlice)
						{
							d3d11DeviceContext->UpdateSubresource(mD3D11Texture2D, D3D11CalcSubresource(0, arraySlice, numberOfMipmaps), nullptr, data, bytesPerRow, bytesPerSlice);

							// Move on to the next slice
							data = static_cast<const uint8_t*>(data) + bytesPerSlice;
						}
					}

					// Let Direct3D 11 generate the mipmaps for us automatically
					D3DX11FilterTexture(d3d11DeviceContext, mD3D11Texture2D, 0, D3DX11_DEFAULT);
				}
			}
			else
			{
				// We don't want dynamic allocations, so we limit the maximum number of mipmaps and hence are able to use the efficient C runtime stack
				static const uint32_t MAXIMUM_NUMBER_OF_MIPMAPS = 15;	// A 16384x16384 texture has 15 mipmaps
				static const uint32_t MAXIMUM_NUMBER_OF_SLICES = 10;
				const Renderer::Context& context = direct3D11Renderer.getContext();
				RENDERER_ASSERT(context, numberOfMipmaps <= MAXIMUM_NUMBER_OF_MIPMAPS, "Invalid Direct3D 11 number of mipmaps")
				D3D11_SUBRESOURCE_DATA d3d11SubresourceDataStack[MAXIMUM_NUMBER_OF_SLICES * MAXIMUM_NUMBER_OF_MIPMAPS];
				D3D11_SUBRESOURCE_DATA* d3d11SubresourceData = (numberOfSlices <= MAXIMUM_NUMBER_OF_SLICES) ? d3d11SubresourceDataStack : RENDERER_MALLOC_TYPED(context, D3D11_SUBRESOURCE_DATA, numberOfSlices);

				// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
				if (dataContainsMipmaps)
				{
					// Data layout
					// - Direct3D 11 wants: DDS files are organized in slice-major order, like this:
					//     Slice0: Mip0, Mip1, Mip2, etc.
					//     Slice1: Mip0, Mip1, Mip2, etc.
					//     etc.
					// - The renderer interface provides: CRN and KTX files are organized in mip-major order, like this:
					//     Mip0: Slice0, Slice1, Slice2, Slice3, Slice4, Slice5
					//     Mip1: Slice0, Slice1, Slice2, Slice3, Slice4, Slice5
					//     etc.

					// Upload all mipmaps
					for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
					{
						const uint32_t numberOfBytesPerRow = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
						const uint32_t numberOfBytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
						for (uint32_t arraySlice = 0; arraySlice < numberOfSlices; ++arraySlice)
						{
							// Upload the current slice
							D3D11_SUBRESOURCE_DATA& currentD3d11SubresourceData = d3d11SubresourceData[arraySlice * numberOfMipmaps + mipmap];
							currentD3d11SubresourceData.pSysMem			 = data;
							currentD3d11SubresourceData.SysMemPitch		 = numberOfBytesPerRow;
							currentD3d11SubresourceData.SysMemSlicePitch = 0;	// Only relevant for 3D textures

							// Move on to the next slice
							data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
						}

						// Move on to the next mipmap
						width = std::max(width >> 1, 1u);	// /= 2
						height = std::max(height >> 1, 1u);	// /= 2
					}
				}
				else
				{
					// The user only provided us with the base texture, no mipmaps
					const uint32_t bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
					const uint32_t bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
					for (uint32_t arraySlice = 0; arraySlice < numberOfSlices; ++arraySlice)
					{
						D3D11_SUBRESOURCE_DATA& currentD3d11SubresourceData = d3d11SubresourceData[arraySlice];
						currentD3d11SubresourceData.pSysMem			 = data;
						currentD3d11SubresourceData.SysMemPitch		 = bytesPerRow;
						currentD3d11SubresourceData.SysMemSlicePitch = 0;	// Only relevant for 3D textures

						// Move on to the next slice
						data = static_cast<const uint8_t*>(data) + bytesPerSlice;
					}
				}
				direct3D11Renderer.getD3D11Device()->CreateTexture2D(&d3d11Texture2DDesc, d3d11SubresourceData, &mD3D11Texture2D);
				if (numberOfSlices > MAXIMUM_NUMBER_OF_SLICES)
				{
					RENDERER_FREE(context, d3d11SubresourceData);
				}
			}
		}
		else
		{
			// The user did not provide us with texture data
			direct3D11Renderer.getD3D11Device()->CreateTexture2D(&d3d11Texture2DDesc, nullptr, &mD3D11Texture2D);
		}

		// Create the Direct3D 11 shader resource view instance
		if (nullptr != mD3D11Texture2D)
		{
			// Direct3D 11 shader resource view description
			D3D11_SHADER_RESOURCE_VIEW_DESC d3d11ShaderResourceViewDesc = {};
			d3d11ShaderResourceViewDesc.Format							= dxgiFormat;
			d3d11ShaderResourceViewDesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			d3d11ShaderResourceViewDesc.Texture2DArray.MostDetailedMip	= 0;
			d3d11ShaderResourceViewDesc.Texture2DArray.MipLevels		= numberOfMipmaps;
			d3d11ShaderResourceViewDesc.Texture2DArray.FirstArraySlice	= 0;
			d3d11ShaderResourceViewDesc.Texture2DArray.ArraySize		= numberOfSlices;

			// Create the Direct3D 11 shader resource view instance
			direct3D11Renderer.getD3D11Device()->CreateShaderResourceView(mD3D11Texture2D, &d3d11ShaderResourceViewDesc, &mD3D11ShaderResourceViewTexture);
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			setDebugName("2D texture array");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D11Renderer)
	}

	Texture2DArray::~Texture2DArray()
	{
		if (nullptr != mD3D11ShaderResourceViewTexture)
		{
			mD3D11ShaderResourceViewTexture->Release();
		}
		if (nullptr != mD3D11Texture2D)
		{
			mD3D11Texture2D->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Texture2DArray::setDebugName(const char* name)
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
				// -> In our use case, this resource is tightly coupled with the view
				// -> In principle the user can assign another resource to the view, but our interface documentation
				//    asks the user to not do so, so we ignore this situation when assigning the name
				ID3D11Resource* d3d11Resource = nullptr;
				mD3D11ShaderResourceViewTexture->GetResource(&d3d11Resource);
				if (nullptr != d3d11Resource)
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
					d3d11Resource->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					d3d11Resource->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);

					// Release the Direct3D 11 resource instance
					d3d11Resource->Release();
				}
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void Texture2DArray::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), Texture2DArray, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
