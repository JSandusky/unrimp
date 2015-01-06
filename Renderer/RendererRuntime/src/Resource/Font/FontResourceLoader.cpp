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
#include "RendererRuntime/Resource/Font/FontResourceLoader.h"
#include "RendererRuntime/Resource/Font/FontResource.h"
#include "RendererRuntime/Backend/RendererRuntimeImpl.h"

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void FontResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream ifstream(mAsset.assetFilename, std::ios::binary);

			// Read in the font header
			#pragma pack(push)
			#pragma pack(1)
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
			#pragma pack(pop)
			FontHeader fontHeader;
			ifstream.read(reinterpret_cast<char*>(&fontHeader), sizeof(FontHeader));
			mFontResource->mSize					= fontHeader.size;
			mFontResource->mResolution				= fontHeader.resolution;
			mFontResource->mAscender				= fontHeader.ascender;
			mFontResource->mDescender				= fontHeader.descender;
			mFontResource->mHeight					= fontHeader.height;
			mFontResource->mGlyphTextureAtlasSizeX	= fontHeader.glyphTextureAtlasSizeX;
			mFontResource->mGlyphTextureAtlasSizeY	= fontHeader.glyphTextureAtlasSizeY;
			mFontResource->mNumberOfFontGlyphs		= fontHeader.numberOfFontGlyphs;

			// Read in the font glyphs
			mFontResource->mFontGlyphs = new FontResource::FontGlyphTexture[mFontResource->mNumberOfFontGlyphs];
			ifstream.read(reinterpret_cast<char*>(mFontResource->mFontGlyphs), sizeof(FontResource::FontGlyphTexture) * mFontResource->mNumberOfFontGlyphs);

			// Allocate memory for the glyph texture atlas and read in the data
			const uint32_t totalNumberOfBytes = fontHeader.glyphTextureAtlasSizeX * fontHeader.glyphTextureAtlasSizeY; // Alpha, one byte
			if (mNumberOfGlyphTextureAtlasDataBytes < totalNumberOfBytes)
			{
				mNumberOfGlyphTextureAtlasDataBytes = totalNumberOfBytes;
				delete [] mGlyphTextureAtlasData;
				mGlyphTextureAtlasData = new uint8_t[mNumberOfGlyphTextureAtlasDataBytes];
			}

			// Read in the font data
			ifstream.read(reinterpret_cast<char*>(mGlyphTextureAtlasData), totalNumberOfBytes);
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load font asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void FontResourceLoader::onProcessing()
	{
		// Nothing here
	}

	void FontResourceLoader::onRendererBackendDispatch()
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&mRendererRuntimeImpl.getRenderer())

		// Create the glyph texture atlas and add our internal reference
		mFontResource->mTexture2D = mRendererRuntimeImpl.getRenderer().createTexture2D(mFontResource->mGlyphTextureAtlasSizeX, mFontResource->mGlyphTextureAtlasSizeY, Renderer::TextureFormat::A8, mGlyphTextureAtlasData, Renderer::TextureFlag::GENERATE_MIPMAPS);
		if (nullptr != mFontResource->mTexture2D)
		{
			RENDERER_SET_RESOURCE_DEBUG_NAME(mFontResource->mTexture2D, mAsset.assetFilename)	// TODO(co) Do also append asset ID
			mFontResource->mTexture2D->addReference();
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(&mRendererRuntimeImpl.getRenderer())
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
