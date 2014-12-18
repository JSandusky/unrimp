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
#include "RendererToolkit/FontGlyphTextureFreeType.h"
#include "RendererToolkit/FontTextureFreeType.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void FontGlyphTextureFreeType::initialize(FontTextureFreeType &fontTextureFreeType, unsigned long characterCode, unsigned int positionOffsetX, unsigned int positionOffsetY, unsigned char *glyphTextureAtlasData)
	{
		// Get the FreeType library face object (aka "The Font")
		FT_Face *ftFace = fontTextureFreeType.getFTFace();
		if (nullptr != ftFace)
		{
			// Load the Glyph for the character
			if (0 == FT_Load_Glyph(*ftFace, FT_Get_Char_Index(*ftFace, characterCode), FT_LOAD_DEFAULT))
			{
				// Move the glyph within the glyph slot of the face object into a glyph object
				FT_Glyph ftGlyph;
				if (0 == FT_Get_Glyph((*ftFace)->glyph, &ftGlyph))
				{
					// Convert the glyph to a bitmap
					FT_Glyph_To_Bitmap(&ftGlyph, ft_render_mode_normal, 0, 1);
					const FT_BitmapGlyph ftBitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(ftGlyph);

					// Get a reference to the glyph bitmap
					const FT_Bitmap &ftBitmap = ftBitmapGlyph->bitmap;

					// Get the size (in pixel) of the glyph
					mWidth  = ftBitmap.width;
					mHeight = ftBitmap.rows;

					{ // Copy the glyph bitmap into the glyph texture atlas
						// Get the X component of the glyph texture atlas size (in pixel)
						unsigned int numberOfBytesPerRow    = 0;
						unsigned int glyphTextureAtlasSizeY = 0;
						fontTextureFreeType.getGlyphTextureAtlasSize(numberOfBytesPerRow, glyphTextureAtlasSizeY);

						// Copy all rows (one byte per texel)
						unsigned char *currentGlyphTextureAtlasData = glyphTextureAtlasData + positionOffsetX + numberOfBytesPerRow * positionOffsetY;
						unsigned char *endGlyphTextureAtlasData		= currentGlyphTextureAtlasData + numberOfBytesPerRow * mHeight;
						unsigned char *currentFTBitmapBuffer		= ftBitmap.buffer;
						for (; currentGlyphTextureAtlasData < endGlyphTextureAtlasData; currentGlyphTextureAtlasData += numberOfBytesPerRow, currentFTBitmapBuffer += mWidth)
						{
							// Copy the current row part
							memcpy(currentGlyphTextureAtlasData, currentFTBitmapBuffer, static_cast<size_t>(mWidth));
						}
					}

					// Set the distance (in pixel) from the current pen position to the glyph bitmap
					mCornerX = static_cast<float>(ftBitmapGlyph->left);
					mCornerY = static_cast<float>(ftBitmapGlyph->top) - mHeight;

					// Set the pen advance, the FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with /64
					mPenAdvanceX = static_cast<float>((*ftFace)->glyph->advance.x) / 64.0f;
					mPenAdvanceY = static_cast<float>((*ftFace)->glyph->advance.y) / 64.0f;

					// Get the size of the glyph texture atlas
					unsigned int glyphTextureAtlasSizeX = 0;
					unsigned int glyphTextureAtlasSizeY = 0;
					fontTextureFreeType.getGlyphTextureAtlasSize(glyphTextureAtlasSizeX, glyphTextureAtlasSizeY);

					// Calculate the normalized minimum glyph texture coordinate inside the glyph texture atlas
					mTexCoordMinX = static_cast<float>(positionOffsetX) / static_cast<float>(glyphTextureAtlasSizeX);
					mTexCoordMinY = static_cast<float>(positionOffsetY) / static_cast<float>(glyphTextureAtlasSizeY);

					// Calculate the normalized maximum glyph texture coordinate inside the glyph texture atlas
					mTexCoordMaxX = static_cast<float>(positionOffsetX + mWidth)  / static_cast<float>(glyphTextureAtlasSizeX);
					mTexCoordMaxY = static_cast<float>(positionOffsetY + mHeight) / static_cast<float>(glyphTextureAtlasSizeY);

					// Release the created FreeType glyph
					FT_Done_Glyph(ftGlyph);
				}
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
