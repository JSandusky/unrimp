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
#ifndef __RENDERERRUNTIME_FONTTEXTUREFREETYPE_H__
#define __RENDERERRUNTIME_FONTTEXTUREFREETYPE_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Font/IFontTexture.h"
#include "RendererRuntime/PlatformTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
typedef struct FT_FaceRec_ *FT_Face;
namespace Renderer
{
	class ITexture2D;
}
namespace RendererRuntime
{
	class RendererRuntimeImpl;
	class FontGlyphTextureFreeType;
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
	*    FreeType font texture class
	*/
	class FontTextureFreeType : public IFontTexture
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
		FontTextureFreeType(RendererRuntimeImpl &rendererRuntimeImpl, const char *filename);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~FontTextureFreeType();

		/**
		*  @brief
		*    Return the FreeType library face object (aka "The Font")
		*
		*  @return
		*    The FreeType library face object, a null pointer on error
		*/
		inline FT_Face *getFTFace() const;

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
		virtual bool setSize(uint32_t size = 12, uint32_t resolution = 96) override;
		inline virtual bool isValid() const override;
		virtual float getAscender() const override;
		virtual float getDescender() const override;
		virtual float getHeight() const override;
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
		inline FontTextureFreeType();

		/**
		*  @brief
		*    Calculate the glyph texture atlas size using the current glyph settings and hardware capabilities
		*
		*  @param[out] glyphTextureAtlasSizeX
		*    Receives the X component of the glyph texture atlas size
		*  @param[out] glyphTextureAtlasSizeY
		*    Receives the Y component of the glyph texture atlas size
		*/
		void calculateGlyphTextureAtlasSize(uint32_t &glyphTextureAtlasSizeX, uint32_t &glyphTextureAtlasSizeY);

		/**
		*  @brief
		*    Create the glyph texture atlas
		*/
		void createGlyphTextureAtlas();

		/**
		*  @brief
		*    Return the number of bytes the glyph texture atlas allocates
		*
		*  @param[in] includeMipmaps
		*    Include the memory required for the mipmaps?
		*
		*  @return
		*    The number of bytes the glyph texture atlas allocates
		*/
		uint32_t getGlyphTextureAtlasNumberOfBytes(bool includeMipmaps) const;

		/**
		*  @brief
		*    Destroy the glyph texture atlas
		*/
		void destroyGlyphTextureAtlas();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		RendererRuntimeImpl		 *mRendererRuntimeImpl;			///< The used renderer runtime implementation instance (we keep a reference to it), always valid
		uint32_t				  mFontFileSize;				///< Font file size in bytes
		uint8_t					 *mFontFileData;				///< Font file data, can be a null pointer
		FT_Face					 *mFTFace;						///< FreeType library face (aka "The Font"), a null pointer on error
		uint32_t				  mGlyphTextureAtlasPadding;	///< Glyph texture atlas gab between glyphs in pixel
		uint32_t				  mGlyphTextureAtlasSizeX;		///< X component of the glyph texture atlas size
		uint32_t				  mGlyphTextureAtlasSizeY;		///< Y component of the glyph texture atlas size
		uint32_t				  mNumberOfFontGlyphs;			///< Number of currently active glyphs
		FontGlyphTextureFreeType *mFontGlyphs;					///< Array of currently active glyphs ("character code -> glyph instance"), can be a null pointer, but only if "mNumberOfFontGlyphs" is zero
		Renderer::ITexture2D	 *mTexture2D;					///< The glyph texture atlas (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Font/FontTextureFreeType.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERRUNTIME_FONTTEXTUREFREETYPE_H__
