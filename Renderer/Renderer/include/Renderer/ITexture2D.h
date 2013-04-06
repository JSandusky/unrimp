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
#ifndef __RENDERER_ITEXTURE2D_H__
#define __RENDERER_ITEXTURE2D_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/ITexture.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract 2D texture interface
	*/
	class ITexture2D : public ITexture
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~ITexture2D();

		/**
		*  @brief
		*    Return the width of the texture
		*
		*  @return
		*    The width of the texture
		*/
		inline unsigned int getWidth() const;

		/**
		*  @brief
		*    Return the height of the texture
		*
		*  @return
		*    The height of the texture
		*/
		inline unsigned int getHeight() const;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*  @param[in] width
		*    The width of the texture
		*  @param[in] height
		*    The height of the texture
		*/
		inline ITexture2D(IRenderer &renderer, unsigned int width, unsigned int height);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit ITexture2D(const ITexture2D &source);

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
		inline ITexture2D &operator =(const ITexture2D &source);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		unsigned int mWidth;	///< The width of the texture
		unsigned int mHeight;	///< The height of the texture


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<ITexture2D> ITexture2DPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/ITexture2D.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_ITEXTURE2D_H__
