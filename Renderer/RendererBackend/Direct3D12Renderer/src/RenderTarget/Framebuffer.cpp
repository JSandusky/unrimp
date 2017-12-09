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
#include "Direct3D12Renderer/RenderTarget/Framebuffer.h"
#include "Direct3D12Renderer/RenderTarget/RenderPass.h"
#include "Direct3D12Renderer/Texture/Texture2DArray.h"
#include "Direct3D12Renderer/Texture/Texture2D.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Framebuffer::Framebuffer(Renderer::IRenderPass& renderPass, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment) :
		IFramebuffer(renderPass),
		mNumberOfColorTextures(static_cast<RenderPass&>(renderPass).getNumberOfColorAttachments()),
		mColorTextures(nullptr),	// Set below
		mDepthStencilTexture(nullptr),
		mWidth(UINT_MAX),
		mHeight(UINT_MAX),
		mD3D12DescriptorHeapRenderTargetViews(nullptr),
		mD3D12DescriptorHeapDepthStencilView(nullptr)
	{
		// Get the Direct3D 12 device instance
		Direct3D12Renderer& direct3D12Renderer = static_cast<Direct3D12Renderer&>(renderPass.getRenderer());
		ID3D12Device* d3d12Device = direct3D12Renderer.getD3D12Device();

		// Add a reference to the used color textures
		if (mNumberOfColorTextures > 0)
		{
			mColorTextures = new Renderer::ITexture*[mNumberOfColorTextures];
			mD3D12DescriptorHeapRenderTargetViews = new ID3D12DescriptorHeap*[mNumberOfColorTextures];

			// Loop through all color textures
			ID3D12DescriptorHeap **d3d12DescriptorHeapRenderTargetView = mD3D12DescriptorHeapRenderTargetViews;
			Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture, ++colorFramebufferAttachments, ++d3d12DescriptorHeapRenderTargetView)
			{
				// Sanity check
				RENDERER_ASSERT(renderPass.getRenderer().getContext(), nullptr != colorFramebufferAttachments->texture, "Invalid Direct3D 12 color framebuffer attachment texture");

				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*colorTexture = colorFramebufferAttachments->texture;
				(*colorTexture)->addReference();

				// Evaluate the color texture type
				switch ((*colorTexture)->getResourceType())
				{
					case Renderer::ResourceType::TEXTURE_2D:
					{
						// Sanity check
						RENDERER_ASSERT(renderPass.getRenderer().getContext(), 0 == colorFramebufferAttachments->layerIndex, "Invalid Direct3D 12 color framebuffer attachment layer index");

						// Update the framebuffer width and height if required
						Texture2D* texture2D = static_cast<Texture2D*>(*colorTexture);
						if (mWidth > texture2D->getWidth())
						{
							mWidth = texture2D->getWidth();
						}
						if (mHeight > texture2D->getHeight())
						{
							mHeight = texture2D->getHeight();
						}

						// Get the Direct3D 12 resource
						ID3D12Resource *d3d12Resource = texture2D->getD3D12Resource();

						// Create the Direct3D 12 render target view instance
						D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
						d3d12DescriptorHeapDesc.NumDescriptors = 1;
						d3d12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
						if (SUCCEEDED(d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(d3d12DescriptorHeapRenderTargetView))))
						{
							D3D12_RENDER_TARGET_VIEW_DESC d3d12RenderTargetViewDesc = {};
							d3d12RenderTargetViewDesc.Format = static_cast<DXGI_FORMAT>(texture2D->getDxgiFormat());
							d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
							d3d12RenderTargetViewDesc.Texture2D.MipSlice = colorFramebufferAttachments->mipmapIndex;
							d3d12Device->CreateRenderTargetView(d3d12Resource, &d3d12RenderTargetViewDesc, (*d3d12DescriptorHeapRenderTargetView)->GetCPUDescriptorHandleForHeapStart());
						}
						break;
					}

					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					{
						// Update the framebuffer width and height if required
						Texture2DArray* texture2DArray = static_cast<Texture2DArray*>(*colorTexture);
						if (mWidth > texture2DArray->getWidth())
						{
							mWidth = texture2DArray->getWidth();
						}
						if (mHeight > texture2DArray->getHeight())
						{
							mHeight = texture2DArray->getHeight();
						}

						// Get the Direct3D 12 resource
						ID3D12Resource *d3d12Resource = texture2DArray->getD3D12Resource();

						// Create the Direct3D 12 render target view instance
						D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
						d3d12DescriptorHeapDesc.NumDescriptors = 1;
						d3d12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
						if (SUCCEEDED(d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(d3d12DescriptorHeapRenderTargetView))))
						{
							D3D12_RENDER_TARGET_VIEW_DESC d3d12RenderTargetViewDesc = {};
							d3d12RenderTargetViewDesc.Format = static_cast<DXGI_FORMAT>(texture2DArray->getDxgiFormat());
							d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
							d3d12RenderTargetViewDesc.Texture2DArray.MipSlice		 = colorFramebufferAttachments->mipmapIndex;
							d3d12RenderTargetViewDesc.Texture2DArray.FirstArraySlice = colorFramebufferAttachments->layerIndex;
							d3d12RenderTargetViewDesc.Texture2DArray.ArraySize		 = 1;
							d3d12RenderTargetViewDesc.Texture2DArray.PlaneSlice		 = 0;
							d3d12Device->CreateRenderTargetView(d3d12Resource, &d3d12RenderTargetViewDesc, (*d3d12DescriptorHeapRenderTargetView)->GetCPUDescriptorHandleForHeapStart());
						}
						break;
					}

					case Renderer::ResourceType::ROOT_SIGNATURE:
					case Renderer::ResourceType::RESOURCE_GROUP:
					case Renderer::ResourceType::PROGRAM:
					case Renderer::ResourceType::VERTEX_ARRAY:
					case Renderer::ResourceType::RENDER_PASS:
					case Renderer::ResourceType::SWAP_CHAIN:
					case Renderer::ResourceType::FRAMEBUFFER:
					case Renderer::ResourceType::INDEX_BUFFER:
					case Renderer::ResourceType::VERTEX_BUFFER:
					case Renderer::ResourceType::UNIFORM_BUFFER:
					case Renderer::ResourceType::TEXTURE_BUFFER:
					case Renderer::ResourceType::INDIRECT_BUFFER:
					case Renderer::ResourceType::TEXTURE_1D:
					case Renderer::ResourceType::TEXTURE_3D:
					case Renderer::ResourceType::TEXTURE_CUBE:
					case Renderer::ResourceType::PIPELINE_STATE:
					case Renderer::ResourceType::SAMPLER_STATE:
					case Renderer::ResourceType::VERTEX_SHADER:
					case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
					case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
					case Renderer::ResourceType::GEOMETRY_SHADER:
					case Renderer::ResourceType::FRAGMENT_SHADER:
					default:
						RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "The type of the given color texture at index %d is not supported by the Direct3D 12 renderer backend", colorTexture - mColorTextures)
						*d3d12DescriptorHeapRenderTargetView = nullptr;
						break;
				}
			}
		}

		// Add a reference to the used depth stencil texture
		if (nullptr != depthStencilFramebufferAttachment)
		{
			mDepthStencilTexture = depthStencilFramebufferAttachment->texture;
			RENDERER_ASSERT(renderPass.getRenderer().getContext(), nullptr != mDepthStencilTexture, "Invalid Direct3D 12 depth stencil framebuffer attachment texture");
			mDepthStencilTexture->addReference();

			// Evaluate the depth stencil texture type
			switch (mDepthStencilTexture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Sanity check
					RENDERER_ASSERT(renderPass.getRenderer().getContext(), 0 == depthStencilFramebufferAttachment->layerIndex, "Invalid Direct3D 12 depth stencil framebuffer attachment layer index");

					// Update the framebuffer width and height if required
					Texture2D* texture2D = static_cast<Texture2D*>(mDepthStencilTexture);
					if (mWidth > texture2D->getWidth())
					{
						mWidth = texture2D->getWidth();
					}
					if (mHeight > texture2D->getHeight())
					{
						mHeight = texture2D->getHeight();
					}

					// Get the Direct3D 12 resource
					ID3D12Resource *d3d12Resource = texture2D->getD3D12Resource();

					// Create the Direct3D 12 render target view instance
					D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
					d3d12DescriptorHeapDesc.NumDescriptors = 1;
					d3d12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
					if (SUCCEEDED(d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&mD3D12DescriptorHeapDepthStencilView))))
					{
						D3D12_RENDER_TARGET_VIEW_DESC d3d12RenderTargetViewDesc = {};
						d3d12RenderTargetViewDesc.Format = static_cast<DXGI_FORMAT>(texture2D->getDxgiFormat());
						d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
						d3d12RenderTargetViewDesc.Texture2D.MipSlice = depthStencilFramebufferAttachment->mipmapIndex;
						d3d12Device->CreateRenderTargetView(d3d12Resource, &d3d12RenderTargetViewDesc, mD3D12DescriptorHeapDepthStencilView->GetCPUDescriptorHandleForHeapStart());
					}
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Update the framebuffer width and height if required
					Texture2DArray* texture2DArray = static_cast<Texture2DArray*>(mDepthStencilTexture);
					if (mWidth > texture2DArray->getWidth())
					{
						mWidth = texture2DArray->getWidth();
					}
					if (mHeight > texture2DArray->getHeight())
					{
						mHeight = texture2DArray->getHeight();
					}

					// Get the Direct3D 12 resource
					ID3D12Resource *d3d12Resource = texture2DArray->getD3D12Resource();

					// Create the Direct3D 12 render target view instance
					D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
					d3d12DescriptorHeapDesc.NumDescriptors = 1;
					d3d12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
					if (SUCCEEDED(d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&mD3D12DescriptorHeapDepthStencilView))))
					{
						D3D12_RENDER_TARGET_VIEW_DESC d3d12RenderTargetViewDesc = {};
						d3d12RenderTargetViewDesc.Format = static_cast<DXGI_FORMAT>(texture2DArray->getDxgiFormat());
						d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
						d3d12RenderTargetViewDesc.Texture2DArray.MipSlice		 = depthStencilFramebufferAttachment->mipmapIndex;
						d3d12RenderTargetViewDesc.Texture2DArray.FirstArraySlice = depthStencilFramebufferAttachment->layerIndex;
						d3d12RenderTargetViewDesc.Texture2DArray.ArraySize		 = 1;
						d3d12RenderTargetViewDesc.Texture2DArray.PlaneSlice		 = 0;
						d3d12Device->CreateRenderTargetView(d3d12Resource, &d3d12RenderTargetViewDesc, mD3D12DescriptorHeapDepthStencilView->GetCPUDescriptorHandleForHeapStart());
					}
					break;
				}

				case Renderer::ResourceType::ROOT_SIGNATURE:
				case Renderer::ResourceType::RESOURCE_GROUP:
				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::RENDER_PASS:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::INDIRECT_BUFFER:
				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_3D:
				case Renderer::ResourceType::TEXTURE_CUBE:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				default:
					RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "The type of the given depth stencil texture is not supported by the Direct3D 12 renderer backend")
					break;
			}
		}

		// Validate the framebuffer width and height
		if (0 == mWidth || UINT_MAX == mWidth)
		{
			RENDERER_ASSERT(renderPass.getRenderer().getContext(), false, "Invalid Direct3D 12 framebuffer width");
			mWidth = 1;
		}
		if (0 == mHeight || UINT_MAX == mHeight)
		{
			RENDERER_ASSERT(renderPass.getRenderer().getContext(), false, "Invalid Direct3D 12 framebuffer height");
			mHeight = 1;
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("FBO");
		#endif
	}

	Framebuffer::~Framebuffer()
	{
		// Release the reference to the used color textures
		if (nullptr != mD3D12DescriptorHeapRenderTargetViews)
		{
			// Release references
			ID3D12DescriptorHeap **d3d12DescriptorHeapRenderTargetViewsEnd = mD3D12DescriptorHeapRenderTargetViews + mNumberOfColorTextures;
			for (ID3D12DescriptorHeap **d3d12DescriptorHeapRenderTargetView = mD3D12DescriptorHeapRenderTargetViews; d3d12DescriptorHeapRenderTargetView < d3d12DescriptorHeapRenderTargetViewsEnd; ++d3d12DescriptorHeapRenderTargetView)
			{
				(*d3d12DescriptorHeapRenderTargetView)->Release();
			}

			// Cleanup
			delete [] mD3D12DescriptorHeapRenderTargetViews;
		}
		if (nullptr != mColorTextures)
		{
			// Release references
			Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture)
			{
				(*colorTexture)->releaseReference();
			}

			// Cleanup
			delete [] mColorTextures;
		}

		// Release the reference to the used depth stencil texture
		if (nullptr != mD3D12DescriptorHeapDepthStencilView)
		{
			// Release reference
			mD3D12DescriptorHeapDepthStencilView->Release();
		}
		if (nullptr != mDepthStencilTexture)
		{
			// Release reference
			mDepthStencilTexture->releaseReference();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Framebuffer::setDebugName(const char* name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			{ // Assign a debug name to the Direct3D 12 render target view, do also add the index to the name
				const size_t nameLength = strlen(name) + 5;	// Direct3D 12 supports 8 render targets ("D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT", so: One digit + one [ + one ] + one space + terminating zero = 5 characters)
				char* nameWithIndex = new char[nameLength];
				ID3D12DescriptorHeap **d3d12DescriptorHeapRenderTargetViewsEnd = mD3D12DescriptorHeapRenderTargetViews + mNumberOfColorTextures;
				for (ID3D12DescriptorHeap **d3d12DescriptorHeapRenderTargetView = mD3D12DescriptorHeapRenderTargetViews; d3d12DescriptorHeapRenderTargetView < d3d12DescriptorHeapRenderTargetViewsEnd; ++d3d12DescriptorHeapRenderTargetView)
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
					sprintf_s(nameWithIndex, nameLength, "%s [%d]", name, static_cast<uint32_t>(d3d12DescriptorHeapRenderTargetView - mD3D12DescriptorHeapRenderTargetViews));
					(*d3d12DescriptorHeapRenderTargetView)->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					(*d3d12DescriptorHeapRenderTargetView)->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(nameLength), nameWithIndex);
				}
				delete [] nameWithIndex;
			}

			// Assign a debug name to the Direct3D 12 depth stencil view
			if (nullptr != mD3D12DescriptorHeapDepthStencilView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D12DescriptorHeapDepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12DescriptorHeapDepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void Framebuffer::getWidthAndHeight(uint32_t& width, uint32_t& height) const
	{
		// No fancy implementation in here, just copy over the internal information
		width  = mWidth;
		height = mHeight;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
