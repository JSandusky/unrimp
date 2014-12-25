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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __RENDERERRUNTIME_FONTTEXTURE_H__
#define __RENDERERRUNTIME_FONTTEXTURE_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Font/IFontTexture.h"
#include "RendererRuntime/PlatformTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ITexture2D;
}
namespace RendererRuntime
{
	class RendererRuntimeImpl;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Font texture class
	*/
	class FontTexture : public IFontTexture
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] rendererRuntimeImpl
		*    Renderer runtime implementation instance to use
		*  @param[in] filename
		*    The ASCII font filename, must be valid
		*
		*  @note
		*    - The font instance keeps a reference to the provided renderer runtime implementation instance
		*/
		FontTexture(RendererRuntimeImpl &rendererRuntimeImpl, const char *filename);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~FontTexture();

		/**
		*  @brief
		*    Return the size of the glyph texture atlas (in pixel)
		*
		*  @param[out] glyphTextureAtlasSizeX
		*    Receives the X component of the glyph texture atlas size
		*  @param[out] glyphTextureAtlasSizeY
		*    Receives the Y component of the glyph texture atlas size
		*/
		inline void getGlyphTextureAtlasSize(uint32_t &glyphTextureAtlasSizeX, uint32_t &glyphTextureAtlasSizeY) const;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IFont methods         ]
	//[-------------------------------------------------------]
	public:
		inline virtual bool isValid() const override;
		virtual float getTextWidth(const char *text) override;
		virtual void drawText(const char *text, const float *color, const float objectSpaceToClipSpace[16], float scaleX = 1.0f, float scaleY = 1.0f, float biasX = 0.0f, float biasY = 0.0f, uint32_t flags = 0) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Default constructor
		*/
		inline FontTexture();

		/**
		*  @brief
		*    Destroy the glyph texture atlas
		*/
		void destroyGlyphTextureAtlas();


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct FontGlyphTexture
		{
			// Font glyph
			int   width;		///< The width (in pixel) of the glyph
			int   height;		///< The height (in pixel) of the glyph
			float cornerX;		///< The X distance (in pixel) from the current pen position to the glyph bitmap
			float cornerY;		///< The Y distance (in pixel) from the current pen position to the glyph bitmap
			float penAdvanceX;	///< The object space pen X advance
			float penAdvanceY;	///< The object space pen Y advance
			// Texture font glyph specific
			float texCoordMinX;	///< X coordinate of the normalized minimum glyph texture coordinate inside the glyph texture atlas
			float texCoordMinY;	///< Y coordinate of the normalized minimum glyph texture coordinate inside the glyph texture atlas
			float texCoordMaxX;	///< X coordinate of the normalized maximum glyph texture coordinate inside the glyph texture atlas
			float texCoordMaxY;	///< Y coordinate of the normalized maximum glyph texture coordinate inside the glyph texture atlas
		};


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		RendererRuntimeImpl	 *mRendererRuntimeImpl;			///< The used renderer runtime implementation instance (we keep a reference to it), always valid
		uint32_t			  mFontFileSize;				///< Font file size in bytes
		uint8_t				 *mFontFileData;				///< Font file data, can be a null pointer
		uint32_t			  mGlyphTextureAtlasPadding;	///< Glyph texture atlas gab between glyphs in pixel
		uint32_t			  mGlyphTextureAtlasSizeX;		///< X component of the glyph texture atlas size
		uint32_t			  mGlyphTextureAtlasSizeY;		///< Y component of the glyph texture atlas size
		uint32_t			  mNumberOfFontGlyphs;			///< Number of currently active glyphs
		FontGlyphTexture	 *mFontGlyphs;					///< Array of currently active glyphs ("character code -> glyph instance"), can be a null pointer, but only if "mNumberOfFontGlyphs" is zero
		Renderer::ITexture2D *mTexture2D;					///< The glyph texture atlas (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Font/FontTextureFreeType.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERRUNTIME_FONTTEXTURE_H__
