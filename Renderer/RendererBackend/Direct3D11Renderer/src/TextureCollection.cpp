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
#include "Direct3D11Renderer/TextureCollection.h"
#include "Direct3D11Renderer/Texture2D.h"
#include "Direct3D11Renderer/TextureBuffer.h"
#include "Direct3D11Renderer/Texture2DArray.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureCollection::TextureCollection(Direct3D11Renderer &direct3D11Renderer, uint32_t numberOfTextures, Renderer::ITexture **textures) :
		Renderer::ITextureCollection(direct3D11Renderer),
		mNumberOfD3D11ShaderResourceViews(numberOfTextures),
		mD3D11ShaderResourceViews((numberOfTextures > 0) ? new ID3D11ShaderResourceView*[numberOfTextures] : nullptr),
		mTextures((numberOfTextures > 0) ? new Renderer::ITexture*[numberOfTextures] : nullptr)
	{
		// Loop through all given textures and gather the Direct3D 11 shader resource views
		ID3D11ShaderResourceView **currentD3D11ShaderResourceView = mD3D11ShaderResourceViews;
		Renderer::ITexture		 **currentTexture				  = mTextures;
		Renderer::ITexture		 **textureEnd					  = mTextures + mNumberOfD3D11ShaderResourceViews;
		for (; currentTexture < textureEnd; ++currentD3D11ShaderResourceView, ++currentTexture, ++textures)
		{
			// Get the current texture
			Renderer::ITexture *texture = *textures;

			// Add a texture reference
			texture->addReference();
			*currentTexture = texture;

			// Security check: Is the given resource owned by this renderer?
			#ifndef DIRECT3D11RENDERER_NO_RENDERERMATCHCHECK
				if (&direct3D11Renderer != &texture->getRenderer())
				{
					// Output an error message and keep on going in order to keep a reasonable behaviour even in case on an error
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 11 error: The given resource at index %d is owned by another renderer instance", currentTexture - mTextures)
					*currentD3D11ShaderResourceView = nullptr;
					continue;
				}
			#endif

			// Evaluate the texture in order to get the Direct3D 11 shader resource view
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
					*currentD3D11ShaderResourceView = static_cast<TextureBuffer*>(texture)->getD3D11ShaderResourceView();
					break;

				case Renderer::ResourceType::TEXTURE_2D:
					*currentD3D11ShaderResourceView = static_cast<Texture2D*>(texture)->getD3D11ShaderResourceView();
					break;

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					*currentD3D11ShaderResourceView = static_cast<Texture2DArray*>(texture)->getD3D11ShaderResourceView();
					break;

				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::RASTERIZER_STATE:
				case Renderer::ResourceType::DEPTH_STENCIL_STATE:
				case Renderer::ResourceType::BLEND_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				case Renderer::ResourceType::TEXTURE_COLLECTION:
				case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
				default:
					// Not handled in here
					break;
			}
		}
	}

	TextureCollection::~TextureCollection()
	{
		// Release the allocated memory
		if (nullptr != mD3D11ShaderResourceViews)
		{
			delete [] mD3D11ShaderResourceViews;
		}
		if (nullptr != mTextures)
		{
			// Loop through all given textures and release our references
			Renderer::ITexture **currentTexture	= mTextures;
			Renderer::ITexture **textureEnd		= mTextures + mNumberOfD3D11ShaderResourceViews;
			for (; currentTexture < textureEnd; ++currentTexture)
			{
				(*currentTexture)->release();
			}

			// Release the allocated memory
			delete [] mTextures;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void TextureCollection::setDebugName(const char *)
	{
		// In here we could assign the given debug name to all textures assigned to the
		// texture collection, but this might end up within a naming chaos due to overwriting
		// possible already set names... don't do this...
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
