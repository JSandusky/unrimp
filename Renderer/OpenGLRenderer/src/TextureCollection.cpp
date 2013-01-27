/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#include "OpenGLRenderer/TextureCollection.h"
#include "OpenGLRenderer/OpenGLRenderer.h"

#include <Renderer/ITexture.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	TextureCollection::TextureCollection(OpenGLRenderer &openGLRenderer, unsigned int numberOfTextures, Renderer::ITexture **textures) :
		Renderer::ITextureCollection(openGLRenderer),
		mNumberOfTextures(numberOfTextures),
		mTextures((numberOfTextures > 0) ? new Renderer::ITexture*[numberOfTextures] : nullptr)
	{
		// Loop through all given textures
		Renderer::ITexture **currentTexture = mTextures;
		Renderer::ITexture **textureEnd		= mTextures + mNumberOfTextures;
		for (; currentTexture < textureEnd; ++currentTexture, ++textures)
		{
			// Get the current texture
			Renderer::ITexture *texture = *textures;

			// Add a texture reference
			texture->addReference();
			*currentTexture = texture;

			// Security check: Is the given resource owned by this renderer?
			#ifndef OPENGLRENDERER_NO_RENDERERMATCHCHECK
				if (&openGLRenderer != &texture->getRenderer())
				{
					// Output an error message and keep on going in order to keep a resonable behaviour even in case on an error
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: The given resource at index %d is owned by another renderer instance", currentTexture - mTextures)
				}
			#endif
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	TextureCollection::~TextureCollection()
	{
		// Release the allocated memory
		if (nullptr != mTextures)
		{
			// Loop through all given textures and release our references
			Renderer::ITexture **currentTexture = mTextures;
			Renderer::ITexture **textureEnd		= mTextures + mNumberOfTextures;
			for (; currentTexture < textureEnd; ++currentTexture)
			{
				(*currentTexture)->release();
			}

			// Release the allocated memory
			delete [] mTextures;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
