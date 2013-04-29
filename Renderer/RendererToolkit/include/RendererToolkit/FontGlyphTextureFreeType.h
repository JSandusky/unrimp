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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __RENDERERTOOLKIT_FONTGLYPHTEXTUREFREETYPE_H__
#define __RENDERERTOOLKIT_FONTGLYPHTEXTUREFREETYPE_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/IFontGlyphTexture.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererToolkit
{
	class FontTextureFreeType;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    FreeType font glyph texture class
	*/
	class FontGlyphTextureFreeType : public IFontGlyphTexture
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		inline FontGlyphTextureFreeType();

		/**
		*  @brief
		*    Initialize the glyph
		*
		*  @param[in]  fontTextureFreeType
		*    Owner FreeType texture font
		*  @param[in]  characterCode
		*    Character code
		*  @param[in]  positionOffsetX
		*    X position offset (in texel) of the glyph inside the font texture atlas
		*  @param[in]  positionOffsetY
		*    Y position offset (in texel) of the glyph inside the font texture atlas
		*  @param[out] glyphTextureAtlasData
		*    Pointer to the glyph texture atlas data to fill, must be valid! (data is not destroyed or cached inside this method)
		*/
		void initialize(FontTextureFreeType &fontTextureFreeType, unsigned long characterCode, unsigned int positionOffsetX, unsigned int positionOffsetY, unsigned char *glyphTextureAtlasData);

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~FontGlyphTextureFreeType();

		/**
		*  @brief
		*    Return the normalized minimum glyph texture coordinate inside the glyph texture atlas
		*
		*  @param[out] texCoordMinX
		*    Receives the X coordinate of the normalized minimum glyph texture coordinate inside the glyph texture atlas
		*  @param[out] texCoordMinY
		*    Receives the Y coordinate of the normalized minimum glyph texture coordinate inside the glyph texture atlas
		*/
		inline void getTexCoordMin(float &texCoordMinX, float &texCoordMinY) const;

		/**
		*  @brief
		*    Return the normalized maximum glyph texture coordinate inside the glyph texture atlas
		*
		*  @param[out] texCoordMaxX
		*    Receives the X coordinate of the normalized maximum glyph texture coordinate inside the glyph texture atlas
		*  @param[out] texCoordMaxY
		*    Receives the Y coordinate of the normalized maximum glyph texture coordinate inside the glyph texture atlas
		*/
		inline void getTexCoordMax(float &texCoordMaxX, float &texCoordMaxY) const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		float mTexCoordMinX;	///< X coordinate of the normalized minimum glyph texture coordinate inside the glyph texture atlas
		float mTexCoordMinY;	///< Y coordinate of the normalized minimum glyph texture coordinate inside the glyph texture atlas
		float mTexCoordMaxX;	///< X coordinate of the normalized maximum glyph texture coordinate inside the glyph texture atlas
		float mTexCoordMaxY;	///< Y coordinate of the normalized maximum glyph texture coordinate inside the glyph texture atlas


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererToolkit/FontGlyphTextureFreeType.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERTOOLKIT_FONTGLYPHTEXTUREFREETYPE_H__
