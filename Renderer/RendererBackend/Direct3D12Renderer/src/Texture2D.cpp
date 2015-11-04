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
#include "Direct3D12Renderer/Texture2D.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12X.h"
#include "Direct3D12Renderer/Mapping.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"
#include "Direct3D12Renderer/SwapChain.h"	// TODO(co) Remove this

#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{

	// TODO(co) First Direct3D 12 texture test
	ID3D12DescriptorHeap* g_mD3D12DescriptorHeapTexture = nullptr;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture2D::Texture2D(Direct3D12Renderer &direct3D12Renderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t, Renderer::TextureUsage::Enum) :
		ITexture2D(direct3D12Renderer, width, height),
		mD3D12Resource(nullptr),
		mD3D12DescriptorHeap(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&getRenderer())

		ID3D12Device* d3d12Device = direct3D12Renderer.getD3D12Device();

		// TODO(co) Add buffer usage setting support
		// TODO(co) Add mipmap support
		// TODO(co) Add compressed upload support

		// Describe and create a texture 2D
		D3D12_RESOURCE_DESC d3d12ResourceDesc = {};
		d3d12ResourceDesc.MipLevels = 1;
		d3d12ResourceDesc.Format = static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(textureFormat));
		d3d12ResourceDesc.Width = width;
		d3d12ResourceDesc.Height = height;
		d3d12ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		d3d12ResourceDesc.DepthOrArraySize = 1;
		d3d12ResourceDesc.SampleDesc.Count = 1;
		d3d12ResourceDesc.SampleDesc.Quality = 0;
		d3d12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		// TODO(co) This is just first Direct3D 12 texture test, so don't wonder about the nasty synchronization handling
		const CD3DX12_HEAP_PROPERTIES d3d12XHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		if (SUCCEEDED(d3d12Device->CreateCommittedResource(
			&d3d12XHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&d3d12ResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
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
					const UINT srcRowPitch = width * Renderer::TextureFormat::getNumberOfBytesPerElement(textureFormat);
					mD3D12Resource->WriteToSubresource(0, nullptr, data, srcRowPitch, srcRowPitch * height);
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
//					direct3D12Renderer.mMainSwapChain->waitForPreviousFrame();
					d3d12ResourceTextureUploadHeap->Release();
				}
				*/

				// Describe and create a SRV for the texture
				D3D12_SHADER_RESOURCE_VIEW_DESC d3d12ShaderResourceViewDesc = {};
				d3d12ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				d3d12ShaderResourceViewDesc.Format = d3d12ResourceDesc.Format;
				d3d12ShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				d3d12ShaderResourceViewDesc.Texture2D.MipLevels = 1;
				d3d12Device->CreateShaderResourceView(mD3D12Resource, &d3d12ShaderResourceViewDesc, mD3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create texture 2D descriptor heap")
			}
		}
		else
		{
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create texture 2D resource")
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("2D texture");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&getRenderer())


		// TODO(co) First Direct3D 12 texture test
		g_mD3D12DescriptorHeapTexture = mD3D12DescriptorHeap;


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
	void Texture2D::setDebugName(const char * name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			if (nullptr != mD3D12Resource)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12Resource->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
			if (nullptr != mD3D12DescriptorHeap)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12DescriptorHeap->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12DescriptorHeap->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
