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
#include "Direct3D10Renderer/RenderTarget/Framebuffer.h"
#include "Direct3D10Renderer/Texture/Texture2D.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"

#include <stdio.h>	// For "sprintf_s()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Framebuffer::Framebuffer(Direct3D10Renderer &direct3D10Renderer, uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture) :
		IFramebuffer(direct3D10Renderer),
		mNumberOfColorTextures(numberOfColorTextures),
		mColorTextures(nullptr),	// Set below
		mDepthStencilTexture(depthStencilTexture),
		mWidth(UINT_MAX),
		mHeight(UINT_MAX),
		mD3D10RenderTargetViews(nullptr),
		mD3D10DepthStencilView(nullptr)
	{
		// The Direct3D 10 "ID3D10Device::OMSetRenderTargets method"-documentation at MSDN http://msdn.microsoft.com/en-us/library/windows/desktop/bb173597%28v=vs.85%29.aspx
		// says the following about the framebuffer width and height when using multiple render targets
		//   "All render targets must have the same size in all dimensions (width and height, and depth for 3D or array size for *Array types)"
		// So, in here I use the smallest width and height as the size of the framebuffer and let Direct3D 10 handle the rest regarding errors.

		// Add a reference to the used color textures
		if (mNumberOfColorTextures > 0)
		{
			mColorTextures = new Renderer::ITexture*[mNumberOfColorTextures];
			mD3D10RenderTargetViews = new ID3D10RenderTargetView*[mNumberOfColorTextures];

			// Loop through all color textures
			ID3D10RenderTargetView **d3d10RenderTargetView = mD3D10RenderTargetViews;
			Renderer::ITexture **colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture **colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture, ++colorTextures, ++d3d10RenderTargetView)
			{
				// Valid entry?
				if (nullptr != *colorTextures)
				{
					// TODO(co) Add security check: Is the given resource one of the currently used renderer?
					*colorTexture = *colorTextures;
					(*colorTexture)->addReference();

					// Evaluate the color texture type
					switch ((*colorTexture)->getResourceType())
					{
						case Renderer::ResourceType::TEXTURE_2D:
						{
							// Update the framebuffer width and height if required
							Texture2D *texture2D = static_cast<Texture2D*>(*colorTexture);
							if (mWidth > texture2D->getWidth())
							{
								mWidth = texture2D->getWidth();
							}
							if (mHeight > texture2D->getHeight())
							{
								mHeight = texture2D->getHeight();
							}

							// Get the Direct3D 10 resource
							ID3D10Resource *d3d10Resource = nullptr;
							texture2D->getD3D10ShaderResourceView()->GetResource(&d3d10Resource);

							// Get the DXGI format of the 2D texture
							D3D10_TEXTURE2D_DESC d3d10Texture2DDesc;
							static_cast<ID3D10Texture2D*>(d3d10Resource)->GetDesc(&d3d10Texture2DDesc);

							// Create the Direct3D 10 render target view instance
							D3D10_RENDER_TARGET_VIEW_DESC d3d10RenderTargetViewDesc;
							d3d10RenderTargetViewDesc.Format			 = d3d10Texture2DDesc.Format;
							d3d10RenderTargetViewDesc.ViewDimension		 = D3D10_RTV_DIMENSION_TEXTURE2D;
							d3d10RenderTargetViewDesc.Texture2D.MipSlice = 0;
							direct3D10Renderer.getD3D10Device()->CreateRenderTargetView(d3d10Resource, &d3d10RenderTargetViewDesc, d3d10RenderTargetView);

							// Release our Direct3D 10 resource reference
							d3d10Resource->Release();
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
						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
						case Renderer::ResourceType::PIPELINE_STATE:
						case Renderer::ResourceType::SAMPLER_STATE:
						case Renderer::ResourceType::VERTEX_SHADER:
						case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
						case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
						case Renderer::ResourceType::GEOMETRY_SHADER:
						case Renderer::ResourceType::FRAGMENT_SHADER:
						default:
							RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 10 error: The type of the given color texture at index %d is not supported", colorTexture - colorTextures)
							*d3d10RenderTargetView = nullptr;
							break;
					}
				}
				else
				{
					*colorTexture = nullptr;
					*d3d10RenderTargetView = nullptr;
				}
			}
		}

		// Add a reference to the used depth stencil texture
		if (nullptr != mDepthStencilTexture)
		{
			mDepthStencilTexture->addReference();

			// Evaluate the depth stencil texture type
			switch (mDepthStencilTexture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Update the framebuffer width and height if required
					Texture2D *texture2D = static_cast<Texture2D*>(mDepthStencilTexture);
					if (mWidth > texture2D->getWidth())
					{
						mWidth = texture2D->getWidth();
					}
					if (mHeight > texture2D->getHeight())
					{
						mHeight = texture2D->getHeight();
					}

					// Get the Direct3D 10 resource
					ID3D10Resource *d3d10Resource = texture2D->getD3D10Texture2D();

					// Get the DXGI format of the 2D texture
					D3D10_TEXTURE2D_DESC d3d10Texture2DDesc;
					static_cast<ID3D10Texture2D*>(d3d10Resource)->GetDesc(&d3d10Texture2DDesc);

					// Create the Direct3D 10 render target view instance
					D3D10_DEPTH_STENCIL_VIEW_DESC d3d10DepthStencilViewDesc;
					d3d10DepthStencilViewDesc.Format			 = d3d10Texture2DDesc.Format;
					d3d10DepthStencilViewDesc.ViewDimension		 = D3D10_DSV_DIMENSION_TEXTURE2D;
					d3d10DepthStencilViewDesc.Texture2D.MipSlice = 0;
					direct3D10Renderer.getD3D10Device()->CreateDepthStencilView(d3d10Resource, &d3d10DepthStencilViewDesc, &mD3D10DepthStencilView);
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
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				default:
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: The type of the given depth stencil texture is not supported")
					break;
			}
		}

		// Validate the framebuffer width and height
		if (UINT_MAX == mWidth)
		{
			mWidth = 0;
		}
		if (UINT_MAX == mHeight)
		{
			mHeight = 0;
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			setDebugName("FBO");
		#endif
	}

	Framebuffer::~Framebuffer()
	{
		// Release the reference to the used color textures
		if (nullptr != mD3D10RenderTargetViews)
		{
			// Release references
			ID3D10RenderTargetView **d3d10RenderTargetViewsEnd = mD3D10RenderTargetViews + mNumberOfColorTextures;
			for (ID3D10RenderTargetView **d3d10RenderTargetView = mD3D10RenderTargetViews; d3d10RenderTargetView < d3d10RenderTargetViewsEnd; ++d3d10RenderTargetView)
			{
				// Valid entry?
				if (nullptr != *d3d10RenderTargetView)
				{
					(*d3d10RenderTargetView)->Release();
				}
			}

			// Cleanup
			delete [] mD3D10RenderTargetViews;
		}
		if (nullptr != mColorTextures)
		{
			// Release references
			Renderer::ITexture **colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture **colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture)
			{
				// Valid entry?
				if (nullptr != *colorTexture)
				{
					(*colorTexture)->release();
				}
			}

			// Cleanup
			delete [] mColorTextures;
		}

		// Release the reference to the used depth stencil texture
		if (nullptr != mD3D10DepthStencilView)
		{
			// Release reference
			mD3D10DepthStencilView->Release();
		}
		if (nullptr != mDepthStencilTexture)
		{
			// Release reference
			mDepthStencilTexture->release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Framebuffer::setDebugName(const char *name)
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			{ // Assign a debug name to the Direct3D 10 render target view, do also add the index to the name
				const size_t nameLength = strlen(name) + 5;	// Direct3D 10 supports 8 render targets ("D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT", so: One digit + one [ + one ] + one space + terminating zero = 5 characters)
				char *nameWithIndex = new char[nameLength];
				ID3D10RenderTargetView **d3d10RenderTargetViewsEnd = mD3D10RenderTargetViews + mNumberOfColorTextures;
				for (ID3D10RenderTargetView **d3d10RenderTargetView = mD3D10RenderTargetViews; d3d10RenderTargetView < d3d10RenderTargetViewsEnd; ++d3d10RenderTargetView)
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
					sprintf_s(nameWithIndex, nameLength, "%s [%d]", name, d3d10RenderTargetView - mD3D10RenderTargetViews);
					(*d3d10RenderTargetView)->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					(*d3d10RenderTargetView)->SetPrivateData(WKPDID_D3DDebugObjectName, nameLength, nameWithIndex);
				}
				delete [] nameWithIndex;
			}

			// Assign a debug name to the Direct3D 10 depth stencil view
			if (nullptr != mD3D10DepthStencilView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D10DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void Framebuffer::getWidthAndHeight(uint32_t &width, uint32_t &height) const
	{
		// No fancy implementation in here, just copy over the internal information
		width  = mWidth;
		height = mHeight;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
