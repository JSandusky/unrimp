/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "RendererRuntime/Resource/Font/FontResourceSerializer.h"
#include "RendererRuntime/Resource/Font/FontImpl.h"
#include "RendererRuntime/RendererRuntimeImpl.h"

#include <stdio.h>
#include <string.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	IFont* FontResourceSerializer::loadFont(const char* filename)
	{
		FontImpl* fontImpl = new FontImpl(mRendererRuntimeImpl);

		// Open the file
		// TODO(co) At the moment "fopen()" etc. are used directly
		FILE *file = fopen(filename, "rb");
		if (nullptr != file)
		{
			// Read in the font header
			struct FontHeader
			{
				uint32_t formatType;
				uint16_t formatVersion;
				uint32_t size;
				uint32_t resolution;
				float	 ascender;
				float	 descender;
				float	 height;
				uint32_t numberOfFontGlyphs;
				uint32_t glyphTextureAtlasSizeX;
				uint32_t glyphTextureAtlasSizeY;
			};
			FontHeader fontHeader;
			fread(&fontHeader, sizeof(FontHeader), 1, file);
			fontImpl->mSize						= fontHeader.size;
			fontImpl->mResolution				= fontHeader.resolution;
			fontImpl->mAscender					= fontHeader.ascender;
			fontImpl->mDescender				= fontHeader.descender;
			fontImpl->mHeight					= fontHeader.height;
			fontImpl->mGlyphTextureAtlasSizeX	= fontHeader.glyphTextureAtlasSizeX;
			fontImpl->mGlyphTextureAtlasSizeY	= fontHeader.glyphTextureAtlasSizeY;
			fontImpl->mNumberOfFontGlyphs		= fontHeader.numberOfFontGlyphs;

			// Read in the font glyphs
			fontImpl->mFontGlyphs = new FontImpl::FontGlyphTexture[fontImpl->mNumberOfFontGlyphs];
			fread(fontImpl->mFontGlyphs, sizeof(FontImpl::FontGlyphTexture), fontImpl->mNumberOfFontGlyphs, file);

			{ // Read in the font data
				// Allocate memory for the glyph texture atlas and read in the data
				const uint32_t totalNumberOfBytes = fontHeader.glyphTextureAtlasSizeX * fontHeader.glyphTextureAtlasSizeY; // Alpha, one byte
				uint8_t* glyphTextureAtlasData = new uint8_t[totalNumberOfBytes];
				fread(glyphTextureAtlasData, sizeof(uint8_t), totalNumberOfBytes, file);

				{ // Renderer related part
					// Begin debug event
					RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&mRendererRuntimeImpl.getRenderer())

					// Create the glyph texture atlas and add our internal reference
					fontImpl->mTexture2D = mRendererRuntimeImpl.getRenderer().createTexture2D(fontImpl->mGlyphTextureAtlasSizeX, fontImpl->mGlyphTextureAtlasSizeY, Renderer::TextureFormat::A8, glyphTextureAtlasData, Renderer::TextureFlag::GENERATE_MIPMAPS);
					if (nullptr != fontImpl->mTexture2D)
					{
						RENDERER_SET_RESOURCE_DEBUG_NAME(fontImpl->mTexture2D, filename)
						fontImpl->mTexture2D->addReference();
					}

					// End debug event
					RENDERER_END_DEBUG_EVENT(&mRendererRuntimeImpl.getRenderer())
				}

				// Free allocated memory
				delete [] glyphTextureAtlasData;
			}

			// Close the file
			fclose(file);
		}
		else
		{
			// TODO(co) Error handling
		}

		return fontImpl;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
