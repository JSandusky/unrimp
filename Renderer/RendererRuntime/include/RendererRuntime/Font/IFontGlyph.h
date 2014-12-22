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
#ifndef __RENDERERRUNTIME_IFONTGLYPH_H__
#define __RENDERERRUNTIME_IFONTGLYPH_H__


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
	*    Abstract font glyph interface
	*/
	class IFontGlyph
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IFontGlyph();

		/**
		*  @brief
		*    Return the width and height (both in pixel) of the glyph
		*
		*  @param[out] width
		*    Receives the width (in pixel) of the glyph
		*  @param[out] height
		*    Receives the height (in pixel) of the glyph
		*/
		inline void getWidthAndHeight(int &width, int &height) const;

		/**
		*  @brief
		*    Return the distance (in pixel) from the current pen position to the glyph bitmap
		*
		*  @param[out] cornerX
		*    Receives the X distance (in pixel) from the current pen position to the glyph bitmap
		*  @param[out] cornerY
		*    Receives the Y distance (in pixel) from the current pen position to the glyph bitmap
		*/
		inline void getCorner(float &cornerX, float &cornerY) const;

		/**
		*  @brief
		*    Return the object space pen advance
		*
		*  @param[out] penAdvanceX
		*    Receives the object space pen X advance
		*  @param[out] penAdvanceY
		*    Receives the object space pen Y advance
		*/
		inline void getPenAdvance(float &penAdvanceX, float &penAdvanceY) const;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Default constructor
		*/
		inline IFontGlyph();

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit IFontGlyph(const IFontGlyph &source);

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
		inline IFontGlyph &operator =(const IFontGlyph &source);


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		int   mWidth;		///< The width (in pixel) of the glyph
		int   mHeight;		///< The height (in pixel) of the glyph
		float mCornerX;		///< The X distance (in pixel) from the current pen position to the glyph bitmap
		float mCornerY;		///< The Y distance (in pixel) from the current pen position to the glyph bitmap
		float mPenAdvanceX;	///< The object space pen X advance
		float mPenAdvanceY;	///< The object space pen Y advance


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Font/IFontGlyph.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERRUNTIME_IFONTGLYPH_H__
