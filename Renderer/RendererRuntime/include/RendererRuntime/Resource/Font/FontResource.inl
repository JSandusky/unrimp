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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline uint32_t FontResource::getSize() const
	{
		return mSize;
	}

	inline uint32_t FontResource::getResolution() const
	{
		return mResolution;
	}

	inline uint32_t FontResource::getSizeInPixels() const
	{
		return static_cast<uint32_t>(mSize / 72.0f * mResolution);
	}

	inline float FontResource::getAscender() const
	{
		return mAscender;
	}

	inline float FontResource::getDescender() const
	{
		return mDescender;
	}

	inline float FontResource::getHeight() const
	{
		return mHeight;
	}

	inline uint32_t FontResource::getHeightInPixels() const
	{
		return static_cast<uint32_t>(getHeight() / 72.0f * mResolution);
	}

	inline void FontResource::getGlyphTextureAtlasSize(uint32_t &glyphTextureAtlasSizeX, uint32_t &glyphTextureAtlasSizeY) const
	{
		glyphTextureAtlasSizeX = mGlyphTextureAtlasSizeX;
		glyphTextureAtlasSizeY = mGlyphTextureAtlasSizeY;
	}

	inline bool FontResource::isValid() const
	{
		return (0 != mNumberOfFontGlyphs);
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline FontResource::FontResource() :
		IResource(getUninitialized<FontResourceId>()),
		mRendererRuntimeImpl(nullptr),
		mSize(0),
		mResolution(0),
		mAscender(0.0f),
		mDescender(0.0f),
		mHeight(0.0f),
		mGlyphTextureAtlasPadding(3),
		mGlyphTextureAtlasSizeX(0),
		mGlyphTextureAtlasSizeY(0),
		mNumberOfFontGlyphs(0),
		mFontGlyphs(nullptr),
		mTexture2D(nullptr)
	{
		// Nothing here
	}

	inline FontResource::FontResource(ResourceId resourceId) :
		IResource(resourceId),
		mRendererRuntimeImpl(nullptr),
		mSize(0),
		mResolution(0),
		mAscender(0.0f),
		mDescender(0.0f),
		mHeight(0.0f),
		mGlyphTextureAtlasPadding(3),
		mGlyphTextureAtlasSizeX(0),
		mGlyphTextureAtlasSizeY(0),
		mNumberOfFontGlyphs(0),
		mFontGlyphs(nullptr),
		mTexture2D(nullptr)
	{
		// Nothing here
	}

	inline FontResource::~FontResource()
	{
		// Destroy the glyph texture atlas
		destroyGlyphTextureAtlas();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
