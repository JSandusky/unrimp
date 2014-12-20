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
#ifndef __RENDERERTOOLKIT_IFONT_H__
#define __RENDERERTOOLKIT_IFONT_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/SmartRefCount.h>


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
	*    Abstract font interface
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
	class IFont : public Renderer::RefCount<IFont>
	{


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
		*    Destructor
		*/
		inline virtual ~IFont();

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
		*    Return the font (line) height in pixels
		*
		*  @return
		*    The font height in pixels
		*
		*  @see
		*    - "getSizeInPixels()"
		*/
		inline uint32_t getHeightInPixels() const;


	//[-------------------------------------------------------]
	//[ Public virtual IFont methods                          ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Set the font size
		*
		*  @param[in] size
		*    Nominal font size in points, for example 12 (72 points per inch)
		*  @param[in] resolution
		*    The horizontal and vertical resolution in DPI, for example 96
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		virtual bool setSize(uint32_t size = 12, uint32_t resolution = 96) = 0;

		/**
		*  @brief
		*    Return whether or not this font is valid
		*
		*  @return
		*    "true" if the font is valid, else "false"
		*/
		virtual bool isValid() const = 0;

		/**
		*  @brief
		*    Return the ascender in font units
		*
		*  @return
		*    The ascender in font units
		*/
		virtual float getAscender() const = 0;

		/**
		*  @brief
		*    Return the descender in font units
		*
		*  @return
		*    The descender in font units (negative value)
		*/
		virtual float getDescender() const = 0;

		/**
		*  @brief
		*    Return the height in font units
		*
		*  @return
		*    The height in font units
		*/
		virtual float getHeight() const = 0;

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
		virtual float getTextWidth(const char *text) = 0;

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
		*    Draw flags, see "RendererToolkit::IFont::EDrawFlags"
		*/
		virtual void drawText(const char *text, const float *color, const float objectSpaceToClipSpace[16], float scaleX = 1.0f, float scaleY = 1.0f, float biasX = 0.0f, float biasY = 0.0f, uint32_t flags = 0) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Default constructor
		*/
		inline IFont();

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit IFont(const IFont &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline IFont &operator =(const IFont &source);


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		uint32_t mSize;			///< Font size in points, for example 12 (72 points per inch)
		uint32_t mResolution;	///< The horizontal and vertical resolution in DPI, for example 96


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef Renderer::SmartRefCount<IFont> IFontPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererToolkit/Font/IFont.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERTOOLKIT_IFONT_H__
