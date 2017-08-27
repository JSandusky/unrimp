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
#include "Direct3D12Renderer/Texture/Texture2D.h"
#include "Direct3D12Renderer/RenderTarget/SwapChain.h"	// TODO(co) Remove this
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12X.h"
#include "Direct3D12Renderer/Mapping.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"

#include <Renderer/ILog.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture2D::Texture2D(Direct3D12Renderer& direct3D12Renderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage, uint8_t numberOfMultisamples, const Renderer::OptimizedTextureClearValue* optimizedTextureClearValue) :
		ITexture2D(direct3D12Renderer, width, height),
		mDxgiFormat(Mapping::getDirect3D12Format(textureFormat)),
		mD3D12Resource(nullptr),
		mD3D12DescriptorHeap(nullptr)
	{
		// Sanity checks
		assert(numberOfMultisamples == 1 || numberOfMultisamples == 2 || numberOfMultisamples == 4 || numberOfMultisamples == 8);
		assert(numberOfMultisamples == 1 || nullptr == data);
		assert(numberOfMultisamples == 1 || 0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS));
		assert(numberOfMultisamples == 1 || 0 == (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		assert(numberOfMultisamples == 1 || 0 != (flags & Renderer::TextureFlag::RENDER_TARGET));
		assert(0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS) || nullptr != data);
		assert((flags & Renderer::TextureFlag::RENDER_TARGET) == 0 || nullptr == data && "Render target textures can't be filled using provided data");

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D12Renderer)

		ID3D12Device* d3d12Device = direct3D12Renderer.getD3D12Device();

		// TODO(co) Add buffer usage setting support
		// TODO(co) Add "Renderer::TextureFlag::GENERATE_MIPMAPS" support, also for render target textures

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height) : 1;

		// Describe and create a texture 2D
		D3D12_RESOURCE_DESC d3d12ResourceDesc = {};
		d3d12ResourceDesc.MipLevels = static_cast<UINT16>(numberOfMipmaps);
		d3d12ResourceDesc.Format = static_cast<DXGI_FORMAT>(mDxgiFormat);
		d3d12ResourceDesc.Width = width;
		d3d12ResourceDesc.Height = height;
		d3d12ResourceDesc.Flags = (flags & Renderer::TextureFlag::RENDER_TARGET) ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE;
		d3d12ResourceDesc.DepthOrArraySize = 1;
		d3d12ResourceDesc.SampleDesc.Count = numberOfMultisamples;
		d3d12ResourceDesc.SampleDesc.Quality = 0;
		d3d12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		// If we don't pass a clear value, we later on get the following debug message: "ID3D12CommandList::ClearRenderTargetView: The application did not pass any clear value to resource creation. The clear operation is typically slower as a result; but will still clear to the desired value. [ EXECUTION WARNING #820: CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE]"
		D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
		depthOptimizedClearValue.Format = d3d12ResourceDesc.Format;
		if (nullptr != optimizedTextureClearValue)
		{
			memcpy(depthOptimizedClearValue.Color, optimizedTextureClearValue->color, sizeof(float) * 4);
		}

		// TODO(co) This is just first Direct3D 12 texture test, so don't wonder about the nasty synchronization handling
		const CD3DX12_HEAP_PROPERTIES d3d12XHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		if (SUCCEEDED(d3d12Device->CreateCommittedResource(
			&d3d12XHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&d3d12ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			(flags & Renderer::TextureFlag::RENDER_TARGET) ? &depthOptimizedClearValue : nullptr,	// Avoid: "Direct3D 12 error: Failed to create texture 2D resourceD3D12 ERROR: ID3D12Device::CreateCommittedResource: pOptimizedClearValue must be NULL when D3D12_RESOURCE_DESC::Dimension is not D3D12_RESOURCE_DIMENSION_BUFFER and neither D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET nor D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL are set in D3D12_RESOURCE_DESC::Flags. [ STATE_CREATION ERROR #815: CREATERESOURCE_INVALIDCLEARVALUE]"
			IID_PPV_ARGS(&mD3D12Resource))))
		/*
		const CD3DX12_HEAP_PROPERTIES d3d12XHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
		if (SUCCEEDED(d3d12Device->CreateCommittedResource(
			&d3d12XHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&d3d12ResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&mD3D12Resource))))
		*/
		{
			// Describe and create a shader resource view (SRV) heap for the texture
			D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
			d3d12DescriptorHeapDesc.NumDescriptors = 1;
			d3d12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			d3d12DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if (SUCCEEDED(d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&mD3D12DescriptorHeap))))
			{
				// Upload the texture data?
				if (nullptr != data)
				{
					// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
					if (dataContainsMipmaps)
					{
						// Upload all mipmaps
						for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
						{
							// Upload the current mipmap
							const uint32_t bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
							const uint32_t bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
							mD3D12Resource->WriteToSubresource(mipmap, nullptr, data, bytesPerRow, bytesPerSlice);

							// Move on to the next mipmap
							data = static_cast<const uint8_t*>(data) + bytesPerSlice;
							width = std::max(width >> 1, 1u);	// /= 2
							height = std::max(height >> 1, 1u);	// /= 2
						}
					}
					else if (generateMipmaps)
					{
						// TODO(co) Implement me
					}
					else
					{
						// The user only provided us with the base texture, no mipmaps
						const uint32_t bytesPerRow   = Renderer::TextureFormat::getNumberOfBytesPerRow(textureFormat, width);
						const uint32_t bytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
						mD3D12Resource->WriteToSubresource(0, nullptr, data, bytesPerRow, bytesPerSlice);
					}
				}

				// TODO(co) This is just first Direct3D 12 texture test, so don't wonder about the nasty synchronization handling
				/*
				if (nullptr != data)
				{
					ID3D12GraphicsCommandList* d3d12GraphicsCommandList = direct3D12Renderer.getD3D12GraphicsCommandList();
				//	direct3D12Renderer.beginScene();

					// Create an upload heap to load the texture onto the GPU. ComPtr's are CPU objects
					// but this heap needs to stay in scope until the GPU work is complete. We will
					// synchronize with the GPU at the end of this method before the ComPtr is destroyed.
					ID3D12Resource* d3d12ResourceTextureUploadHeap = nullptr;

					{ // Upload the texture data
						const UINT64 uploadBufferSize = GetRequiredIntermediateSize(mD3D12Resource, 0, 1);

						// Create the GPU upload buffer
						const CD3DX12_HEAP_PROPERTIES d3d12XHeapPropertiesUpload(D3D12_HEAP_TYPE_UPLOAD);
						const CD3DX12_RESOURCE_DESC d3d12XResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
						if (SUCCEEDED(d3d12Device->CreateCommittedResource(
							&d3d12XHeapPropertiesUpload,
							D3D12_HEAP_FLAG_NONE,
							&d3d12XResourceDesc,
							D3D12_RESOURCE_STATE_GENERIC_READ,
							nullptr,
							IID_PPV_ARGS(&d3d12ResourceTextureUploadHeap))))
						{
							// Copy data to the intermediate upload heap and then schedule a copy from the upload heap to the texture 2D
							D3D12_SUBRESOURCE_DATA textureData = {};
							textureData.pData = data;
							textureData.RowPitch = static_cast<LONG_PTR>(width * Renderer::TextureFormat::getNumberOfBytesPerElement(textureFormat));
							textureData.SlicePitch = static_cast<LONG_PTR>(textureData.RowPitch * height);

							UpdateSubresources(d3d12GraphicsCommandList, mD3D12Resource, d3d12ResourceTextureUploadHeap, 0, 0, 1, &textureData);

		//					CD3DX12_RESOURCE_BARRIER d3d12XResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(mD3D12Resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		//					d3d12GraphicsCommandList->ResourceBarrier(1, &d3d12XResourceBarrier);
						}
					}
	
	//				direct3D12Renderer.endScene();
					<Swap Chain>->waitForPreviousFrame();
					d3d12ResourceTextureUploadHeap->Release();
				}
				*/

				// Describe and create a SRV for the texture
				D3D12_SHADER_RESOURCE_VIEW_DESC d3d12ShaderResourceViewDesc = {};
				d3d12ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				d3d12ShaderResourceViewDesc.Format = d3d12ResourceDesc.Format;
				d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				d3d12ShaderResourceViewDesc.Texture2D.MipLevels = numberOfMipmaps;
				d3d12Device->CreateShaderResourceView(mD3D12Resource, &d3d12ShaderResourceViewDesc, mD3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			}
			else
			{
				RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to create the Direct3D 12 texture 2D descriptor heap")
			}
		}
		else
		{
			RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to create the Direct3D 12 texture 2D resource")
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("2D texture");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D12Renderer)
	}

	Texture2D::~Texture2D()
	{
		if (nullptr != mD3D12Resource)
		{
			mD3D12Resource->Release();
		}
		if (nullptr != mD3D12DescriptorHeap)
		{
			mD3D12DescriptorHeap->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Texture2D::setDebugName(const char*  name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			if (nullptr != mD3D12Resource)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
			if (nullptr != mD3D12DescriptorHeap)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D12DescriptorHeap->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12DescriptorHeap->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
