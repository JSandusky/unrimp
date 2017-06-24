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
#include "Direct3D11Renderer/RenderTarget/Framebuffer.h"
#include "Direct3D11Renderer/Texture/Texture2D.h"
#include "Direct3D11Renderer/Texture/Texture2DArray.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Mapping.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"

#include <stdio.h>	// For "sprintf_s()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Framebuffer::Framebuffer(Direct3D11Renderer& direct3D11Renderer, uint32_t numberOfColorFramebufferAttachments, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment) :
		IFramebuffer(direct3D11Renderer),
		mNumberOfColorTextures(numberOfColorFramebufferAttachments),
		mColorTextures(nullptr),	// Set below
		mDepthStencilTexture(nullptr),
		mWidth(UINT_MAX),
		mHeight(UINT_MAX),
		mGenerateMipmaps(false),
		mD3D11RenderTargetViews(nullptr),
		mD3D11DepthStencilView(nullptr)
	{
		// The Direct3D 11 "ID3D11DeviceContext::OMSetRenderTargets method"-documentation at MSDN http://msdn.microsoft.com/en-us/library/windows/desktop/ff476464%28v=vs.85%29.aspx
		// says the following about the framebuffer width and height when using multiple render targets
		//   "All render targets must have the same size in all dimensions (width and height, and depth for 3D or array size for *Array types)"
		// So, in here I use the smallest width and height as the size of the framebuffer and let Direct3D 11 handle the rest regarding errors.

		// Add a reference to the used color textures
		if (mNumberOfColorTextures > 0)
		{
			mColorTextures = new Renderer::ITexture*[mNumberOfColorTextures];
			mD3D11RenderTargetViews = new ID3D11RenderTargetView*[mNumberOfColorTextures];

			// Loop through all color textures
			ID3D11RenderTargetView** d3d11RenderTargetView = mD3D11RenderTargetViews;
			Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture, ++colorFramebufferAttachments, ++d3d11RenderTargetView)
			{
				// Valid entry?
				if (nullptr != colorFramebufferAttachments->texture)
				{
					// TODO(co) Add security check: Is the given resource one of the currently used renderer?
					*colorTexture = colorFramebufferAttachments->texture;
					(*colorTexture)->addReference();

					// Evaluate the color texture type
					switch ((*colorTexture)->getResourceType())
					{
						case Renderer::ResourceType::TEXTURE_2D:
						{
							// Sanity check
							assert(0 == colorFramebufferAttachments->layerIndex);

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

							// Create the Direct3D 11 render target view instance
							D3D11_RENDER_TARGET_VIEW_DESC d3d11RenderTargetViewDesc = {};
							d3d11RenderTargetViewDesc.Format			 = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(texture2D->getTextureFormat()));
							d3d11RenderTargetViewDesc.ViewDimension		 = (texture2D->getNumberOfMultisamples() > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
							d3d11RenderTargetViewDesc.Texture2D.MipSlice = colorFramebufferAttachments->mipmapIndex;
							direct3D11Renderer.getD3D11Device()->CreateRenderTargetView(texture2D->getD3D11Texture2D(), &d3d11RenderTargetViewDesc, d3d11RenderTargetView);

							// Generate mipmaps?
							if (texture2D->getGenerateMipmaps())
							{
								mGenerateMipmaps = true;
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

							// Create the Direct3D 11 render target view instance
							D3D11_RENDER_TARGET_VIEW_DESC d3d11RenderTargetViewDesc = {};
							d3d11RenderTargetViewDesc.Format			 = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(texture2DArray->getTextureFormat()));
							d3d11RenderTargetViewDesc.ViewDimension		 = (texture2DArray->getNumberOfMultisamples() > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
							d3d11RenderTargetViewDesc.Texture2DArray.MipSlice		 = colorFramebufferAttachments->mipmapIndex;
							d3d11RenderTargetViewDesc.Texture2DArray.FirstArraySlice = colorFramebufferAttachments->layerIndex;
							d3d11RenderTargetViewDesc.Texture2DArray.ArraySize		 = 1;
							direct3D11Renderer.getD3D11Device()->CreateRenderTargetView(texture2DArray->getD3D11Texture2D(), &d3d11RenderTargetViewDesc, d3d11RenderTargetView);

							// Generate mipmaps?
							if (texture2DArray->getGenerateMipmaps())
							{
								mGenerateMipmaps = true;
							}
							break;
						}

						case Renderer::ResourceType::ROOT_SIGNATURE:
						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
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
							RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 11 error: The type of the given color texture at index %d is not supported", colorTexture - mColorTextures)
							*d3d11RenderTargetView = nullptr;
							break;
					}
				}
				else
				{
					*colorTexture = nullptr;
					*d3d11RenderTargetView = nullptr;
				}
			}
		}

		// Add a reference to the used depth stencil texture
		if (nullptr != depthStencilFramebufferAttachment)
		{
			mDepthStencilTexture = depthStencilFramebufferAttachment->texture;
			assert(nullptr != mDepthStencilTexture);
			mDepthStencilTexture->addReference();

			// Evaluate the depth stencil texture type
			switch (mDepthStencilTexture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Sanity check
					assert(0 == depthStencilFramebufferAttachment->layerIndex);

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

					// Create the Direct3D 11 render target view instance
					D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDesc = {};
					d3d11DepthStencilViewDesc.Format			 = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(texture2D->getTextureFormat()));
					d3d11DepthStencilViewDesc.ViewDimension		 = (texture2D->getNumberOfMultisamples() > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
					d3d11DepthStencilViewDesc.Texture2D.MipSlice = depthStencilFramebufferAttachment->mipmapIndex;
					direct3D11Renderer.getD3D11Device()->CreateDepthStencilView(texture2D->getD3D11Texture2D(), &d3d11DepthStencilViewDesc, &mD3D11DepthStencilView);

					// Generate mipmaps?
					if (texture2D->getGenerateMipmaps())
					{
						mGenerateMipmaps = true;
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

					// Create the Direct3D 11 render target view instance
					D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDesc = {};
					d3d11DepthStencilViewDesc.Format			 = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(texture2DArray->getTextureFormat()));
					d3d11DepthStencilViewDesc.ViewDimension		 = (texture2DArray->getNumberOfMultisamples() > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
					d3d11DepthStencilViewDesc.Texture2DArray.MipSlice		 = depthStencilFramebufferAttachment->mipmapIndex;
					d3d11DepthStencilViewDesc.Texture2DArray.FirstArraySlice = depthStencilFramebufferAttachment->layerIndex;
					d3d11DepthStencilViewDesc.Texture2DArray.ArraySize		 = 1;
					direct3D11Renderer.getD3D11Device()->CreateDepthStencilView(texture2DArray->getD3D11Texture2D(), &d3d11DepthStencilViewDesc, &mD3D11DepthStencilView);

					// Generate mipmaps?
					if (texture2DArray->getGenerateMipmaps())
					{
						mGenerateMipmaps = true;
					}
					break;
				}

				case Renderer::ResourceType::ROOT_SIGNATURE:
				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
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
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: The type of the given depth stencil texture is not supported")
					break;
			}
		}

		// Validate the framebuffer width and height
		if (0 == mWidth || UINT_MAX == mWidth)
		{
			assert(false);
			mWidth = 1;
		}
		if (0 == mHeight || UINT_MAX == mHeight)
		{
			assert(false);
			mHeight = 1;
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			setDebugName("FBO");
		#endif
	}

	Framebuffer::~Framebuffer()
	{
		// Release the reference to the used color textures
		if (nullptr != mD3D11RenderTargetViews)
		{
			// Release references
			ID3D11RenderTargetView** d3d11RenderTargetViewsEnd = mD3D11RenderTargetViews + mNumberOfColorTextures;
			for (ID3D11RenderTargetView** d3d11RenderTargetView = mD3D11RenderTargetViews; d3d11RenderTargetView < d3d11RenderTargetViewsEnd; ++d3d11RenderTargetView)
			{
				// Valid entry?
				if (nullptr != *d3d11RenderTargetView)
				{
					(*d3d11RenderTargetView)->Release();
				}
			}

			// Cleanup
			delete [] mD3D11RenderTargetViews;
		}
		if (nullptr != mColorTextures)
		{
			// Release references
			Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture)
			{
				// Valid entry?
				if (nullptr != *colorTexture)
				{
					(*colorTexture)->releaseReference();
				}
			}

			// Cleanup
			delete [] mColorTextures;
		}

		// Release the reference to the used depth stencil texture
		if (nullptr != mD3D11DepthStencilView)
		{
			// Release reference
			mD3D11DepthStencilView->Release();
		}
		if (nullptr != mDepthStencilTexture)
		{
			// Release reference
			mDepthStencilTexture->releaseReference();
		}
	}

	void Framebuffer::generateMipmaps(ID3D11DeviceContext& d3d11DeviceContext) const
	{
		// Sanity check
		assert(mGenerateMipmaps);

		// TODO(co) Complete, currently only 2D textures are supported
		Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
		for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture)
		{
			// Valid entry?
			if (nullptr != *colorTexture && (*colorTexture)->getResourceType() == Renderer::ResourceType::TEXTURE_2D)
			{
				Texture2D* texture2D = static_cast<Texture2D*>(*colorTexture);
				if (texture2D->getGenerateMipmaps())
				{
					d3d11DeviceContext.GenerateMips(texture2D->getD3D11ShaderResourceView());
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Framebuffer::setDebugName(const char* name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			{ // Assign a debug name to the Direct3D 11 render target view, do also add the index to the name
				const size_t nameLength = strlen(name) + 5;	// Direct3D 11 supports 8 render targets ("D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT", so: One digit + one [ + one ] + one space + terminating zero = 5 characters)
				char* nameWithIndex = new char[nameLength];
				ID3D11RenderTargetView** d3d11RenderTargetViewsEnd = mD3D11RenderTargetViews + mNumberOfColorTextures;
				for (ID3D11RenderTargetView** d3d11RenderTargetView = mD3D11RenderTargetViews; d3d11RenderTargetView < d3d11RenderTargetViewsEnd; ++d3d11RenderTargetView)
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
					sprintf_s(nameWithIndex, nameLength, "%s [%d]", name, static_cast<uint32_t>(d3d11RenderTargetView - mD3D11RenderTargetViews));
					(*d3d11RenderTargetView)->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					(*d3d11RenderTargetView)->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(nameLength), nameWithIndex);
				}
				delete [] nameWithIndex;
			}

			// Assign a debug name to the Direct3D 11 depth stencil view
			if (nullptr != mD3D11DepthStencilView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D11DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
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
} // Direct3D11Renderer
