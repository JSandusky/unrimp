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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Resource/IResource.h"


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
	template <class ELEMENT_TYPE, typename ID_TYPE> class PackedElementManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t FontResourceId;	///< POD font resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Font resource
	*
	*  @remarks
	*
	*    Height       Ascender    Internal leading
	*      |            |         |
	*      |   |    |   |             |
	*      |   |    |   |             |
	*      |   |----|   |             |
	*      |   |    |   |             |
	*    0 |___|____|___|_____________|___ Baseline
	*      |0               |         |
	*      |                |         |
	*                   Descender    Size
	*
	*    Term definitions:
	*    - Height = Ascender + Descender
	*    - Size   = Height - Internal leading
	*
	*  @note
	*    - For italic/bold fonts, you have to specify the corresponding font variation file of the font file
	*/
	class FontResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class FontResourceLoader;
		friend class FontResourceManager;
		friend class PackedElementManager<FontResource, FontResourceId>;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		/**
		*  brief
		*    Draw flags
		*/
		enum EDrawFlags
		{
			CENTER_TEXT    = 1<<0,	///< Center the text
			UNDERLINE_TEXT = 1<<1,	///< Underline the text
			CROSSOUT_TEXT  = 1<<2,	///< Cross out the text
			MIPMAPPING     = 1<<3	///< Use mipmapping (may blur the font in an ugly way)
		};


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the nominal font size in points (72 points per inch)
		*
		*  @return
		*    The nominal font size in points, for example 12
		*/
		inline uint32_t getSize() const;

		/**
		*  @brief
		*    Return the horizontal and vertical resolution in DPI
		*
		*  @return
		*    The horizontal and vertical resolution in DPI, for example 96
		*/
		inline uint32_t getResolution() const;

		/**
		*  @brief
		*    Return the font size in pixels
		*
		*  @return
		*    The font size in pixels
		*
		*  @remarks
		*    Example: Arial 12pt at 96 dpi:
		*
		*                          Font size in points             12
		*    Font size in pixels = ------------------- × 96 dpi = ---- × 96 = 16 pixels
		*                          72 points per inch              72
		*/
		inline uint32_t getSizeInPixels() const;

		/**
		*  @brief
		*    Return the ascender in font units
		*
		*  @return
		*    The ascender in font units
		*/
		inline float getAscender() const;

		/**
		*  @brief
		*    Return the descender in font units
		*
		*  @return
		*    The descender in font units (negative value)
		*/
		inline float getDescender() const;

		/**
		*  @brief
		*    Return the height in font units
		*
		*  @return
		*    The height in font units
		*/
		inline float getHeight() const;

		/**
		*  @brief
		*    Return the font (line) height in pixels
		*
		*  @return
		*    The font height in pixels
		*
		*  @see
		*    - "getSizeInPixels()"
		*/
		inline uint32_t getHeightInPixels() const;

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

		/**
		*  @brief
		*    Return whether or not this font is valid
		*
		*  @return
		*    "true" if the font is valid, else "false"
		*/
		inline bool isValid() const;

		/**
		*  @brief
		*    Return the width of a text (in font units) drawn with this font
		*
		*  @param[in] text
		*    ASCII text to get the width of, if null pointer or empty string zero will be returned
		*
		*  @return
		*    The width of a text (in font units) drawn with this font
		*/
		RENDERERRUNTIME_API_EXPORT float getTextWidth(const char *text);

		/**
		*  @brief
		*    Draw a text
		*
		*  @param[in] text
		*    ASCII text to draw, if null pointer or empty string nothing happens
		*  @param[in] color
		*    RGBA text color, if null pointer nothing happens, array must have at least four elements
		*  @param[in] objectSpaceToClipSpace
		*    4x4 object space to clip space matrix
		*  @param[in] scaleX
		*    X component of the font scale, there are situations were it's more comfortable to have a special scale variable as just "objectSpaceToClipSpace" with all build in
		*  @param[in] scaleY
		*    Y component of the font scale, there are situations were it's more comfortable to have a special scale variable as just "objectSpaceToClipSpace" with all build in
		*  @param[in] biasX
		*    X component of the font bias (position offset), there are situations were it's more comfortable to have a special bias variable as just "objectSpaceToClipSpace" with all build in
		*  @param[in] biasY
		*    Y component of the font bias (position offset), there are situations were it's more comfortable to have a special bias variable as just "objectSpaceToClipSpace" with all build in
		*  @param[in] flags
		*    Draw flags, see "RendererRuntime::FontResource::EDrawFlags"
		*/
		RENDERERRUNTIME_API_EXPORT void drawText(const char *text, const float *color, const float objectSpaceToClipSpace[16], float scaleX = 1.0f, float scaleY = 1.0f, float biasX = 0.0f, float biasY = 0.0f, uint32_t flags = 0);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Default constructor
		*/
		inline FontResource();

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] textureResourceId
		*    Texture resource ID
		*/
		inline explicit FontResource(FontResourceId fontResourceId);

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~FontResource();

		FontResource(const FontResource&) = delete;
		FontResource& operator=(const FontResource&) = delete;

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
		uint32_t			  mSize;						///< Font size in points, for example 12 (72 points per inch)
		uint32_t			  mResolution;					///< The horizontal and vertical resolution in DPI, for example 96
		float				  mAscender;
		float				  mDescender;
		float				  mHeight;
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
#include "RendererRuntime/Resource/Font/FontResource.inl"
