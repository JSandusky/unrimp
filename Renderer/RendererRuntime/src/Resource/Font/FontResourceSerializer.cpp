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

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#include <fstream>
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	IFont* FontResourceSerializer::loadFont(std::istream& istream)
	{
		FontImpl* fontImpl = new FontImpl(mRendererRuntimeImpl);

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
		istream.read(reinterpret_cast<char*>(&fontHeader), sizeof(FontHeader));
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
		istream.read(reinterpret_cast<char*>(fontImpl->mFontGlyphs), sizeof(FontImpl::FontGlyphTexture) * fontImpl->mNumberOfFontGlyphs);

		{ // Read in the font data
			// Allocate memory for the glyph texture atlas and read in the data
			const uint32_t totalNumberOfBytes = fontHeader.glyphTextureAtlasSizeX * fontHeader.glyphTextureAtlasSizeY; // Alpha, one byte
			uint8_t* glyphTextureAtlasData = new uint8_t[totalNumberOfBytes];
			istream.read(reinterpret_cast<char*>(glyphTextureAtlasData), totalNumberOfBytes);

			{ // Renderer related part
				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&mRendererRuntimeImpl.getRenderer())

				// Create the glyph texture atlas and add our internal reference
				fontImpl->mTexture2D = mRendererRuntimeImpl.getRenderer().createTexture2D(fontImpl->mGlyphTextureAtlasSizeX, fontImpl->mGlyphTextureAtlasSizeY, Renderer::TextureFormat::A8, glyphTextureAtlasData, Renderer::TextureFlag::GENERATE_MIPMAPS);
				if (nullptr != fontImpl->mTexture2D)
				{
					// TODO(co) Optionally also pass filename?
				//	RENDERER_SET_RESOURCE_DEBUG_NAME(fontImpl->mTexture2D, filename)
					fontImpl->mTexture2D->addReference();
				}

				// End debug event
				RENDERER_END_DEBUG_EVENT(&mRendererRuntimeImpl.getRenderer())
			}

			// Free allocated memory
			delete [] glyphTextureAtlasData;
		}

		return fontImpl;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
