/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "Direct3D9Renderer/RenderTarget/Framebuffer.h"
#include "Direct3D9Renderer/RenderTarget/RenderPass.h"
#include "Direct3D9Renderer/Texture/Texture2D.h"
#include "Direct3D9Renderer/d3d9.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/IRenderer.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
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
		mDirect3D9ColorSurfaces(nullptr),
		mDirect3D9DepthStencilSurface(nullptr)
	{
		// The Direct3D 9 documentation says the following about the framebuffer width and height when using multiple render targets
		//   "" (!)
		// So, in here I use the smallest width and height as the size of the framebuffer.

		// Add a reference to the used color textures
		Direct3D9Renderer& direct3D9Renderer = static_cast<Direct3D9Renderer&>(renderPass.getRenderer());
		if (mNumberOfColorTextures > 0)
		{
			const Renderer::Context& context = direct3D9Renderer.getContext();
			mColorTextures = RENDERER_MALLOC_TYPED(context, Renderer::ITexture*, mNumberOfColorTextures);
			mDirect3D9ColorSurfaces = RENDERER_MALLOC_TYPED(context, IDirect3DSurface9*, mNumberOfColorTextures);

			// Loop through all color textures
			IDirect3DSurface9** direct3D9ColorSurface = mDirect3D9ColorSurfaces;
			Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture, ++colorFramebufferAttachments, ++direct3D9ColorSurface)
			{
				// Sanity check
				RENDERER_ASSERT(renderPass.getRenderer().getContext(), nullptr != colorFramebufferAttachments->texture, "Invalid Direct3D 9 color framebuffer attachment texture")

				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*colorTexture = colorFramebufferAttachments->texture;
				(*colorTexture)->addReference();

				// Evaluate the color texture type
				switch ((*colorTexture)->getResourceType())
				{
					case Renderer::ResourceType::TEXTURE_2D:
					{
						// Sanity check
						RENDERER_ASSERT(renderPass.getRenderer().getContext(), 0 == colorFramebufferAttachments->layerIndex, "Invalid Direct3D 9 color framebuffer attachment layer index")

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

						// Get the Direct3D 9 surface
						texture2D->getDirect3DTexture9()->GetSurfaceLevel(colorFramebufferAttachments->mipmapIndex, direct3D9ColorSurface);
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
					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
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
						RENDERER_LOG(direct3D9Renderer.getContext(), CRITICAL, "The type of the given color texture at index %d is not supported by the Direct3D 9 renderer backend", colorTexture - mColorTextures)
						*direct3D9ColorSurface = nullptr;
						break;
				}
			}
		}

		// Add a reference to the used depth stencil texture
		if (nullptr != depthStencilFramebufferAttachment)
		{
			mDepthStencilTexture = depthStencilFramebufferAttachment->texture;
			RENDERER_ASSERT(renderPass.getRenderer().getContext(), nullptr != mDepthStencilTexture, "Invalid Direct3D 9 depth stencil framebuffer attachment texture")
			mDepthStencilTexture->addReference();

			// Evaluate the depth stencil texture type
			switch (mDepthStencilTexture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Sanity check
					RENDERER_ASSERT(renderPass.getRenderer().getContext(), 0 == depthStencilFramebufferAttachment->layerIndex, "Invalid Direct3D 9 depth stencil framebuffer attachment layer index")

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

					// Get the Direct3D 9 surface
					texture2D->getDirect3DTexture9()->GetSurfaceLevel(depthStencilFramebufferAttachment->mipmapIndex, &mDirect3D9DepthStencilSurface);
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
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
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
					RENDERER_LOG(direct3D9Renderer.getContext(), CRITICAL, "The type of the given depth stencil texture is not supported by the Direct3D 9 renderer backend")
					break;
			}
		}

		// Validate the framebuffer width and height
		if (0 == mWidth || UINT_MAX == mWidth)
		{
			RENDERER_ASSERT(renderPass.getRenderer().getContext(), false, "Invalid Direct3D 9 framebuffer width")
			mWidth = 1;
		}
		if (0 == mHeight || UINT_MAX == mHeight)
		{
			RENDERER_ASSERT(renderPass.getRenderer().getContext(), false, "Invalid Direct3D 9 framebuffer height")
			mHeight = 1;
		}
	}

	Framebuffer::~Framebuffer()
	{
		// Release the reference to the used color textures
		const Renderer::Context& context = getRenderer().getContext();
		if (nullptr != mDirect3D9ColorSurfaces)
		{
			// Release references
			IDirect3DSurface9** direct3D9ColorSurfacesEnd = mDirect3D9ColorSurfaces + mNumberOfColorTextures;
			for (IDirect3DSurface9** direct3D9ColorSurface = mDirect3D9ColorSurfaces; direct3D9ColorSurface < direct3D9ColorSurfacesEnd; ++direct3D9ColorSurface)
			{
				(*direct3D9ColorSurface)->Release();
			}

			// Cleanup
			RENDERER_FREE(context, mDirect3D9ColorSurfaces);
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
			RENDERER_FREE(context, mColorTextures);
		}

		// Release the reference to the used depth stencil texture
		if (nullptr != mDirect3D9DepthStencilSurface)
		{
			// Release reference
			mDirect3D9DepthStencilSurface->Release();
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
	void Framebuffer::setDebugName(const char*)
	{
		// In here we could assign the given debug name to all surfaces assigned to the
		// framebuffer, but this might end up within a naming chaos due to overwriting
		// possible already set names... don't do this...
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
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void Framebuffer::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), Framebuffer, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
