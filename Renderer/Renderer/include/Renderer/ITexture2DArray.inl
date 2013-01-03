/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_STATISTICS
	#include "Renderer/IRenderer.h"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Destructor
	*/
	inline ITexture2DArray::~ITexture2DArray()
	{
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			--getRenderer().getStatistics().currentNumberOfTexture2DArrays;
		#endif
	}

	/**
	*  @brief
	*    Return the width of the texture
	*/
	inline unsigned int ITexture2DArray::getWidth() const
	{
		return mWidth;
	}

	/**
	*  @brief
	*    Return the height of the texture
	*/
	inline unsigned int ITexture2DArray::getHeight() const
	{
		return mHeight;
	}

	/**
	*  @brief
	*    Return the number of slices
	*/
	inline unsigned int ITexture2DArray::getNumberOfSlices() const
	{
		return mNumberOfSlices;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	inline ITexture2DArray::ITexture2DArray(IRenderer &renderer, unsigned int width, unsigned int height, unsigned int numberOfSlices) :
		ITexture(ResourceType::TEXTURE_2D_ARRAY, renderer),
		mWidth(width),
		mHeight(height),
		mNumberOfSlices(numberOfSlices)
	{
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			++getRenderer().getStatistics().numberOfCreatedTexture2DArrays;
			++getRenderer().getStatistics().currentNumberOfTexture2DArrays;
		#endif
	}

	/**
	*  @brief
	*    Copy constructor
	*/
	inline ITexture2DArray::ITexture2DArray(const ITexture2DArray &source) :
		ITexture(source),
		mWidth(source.getWidth()),
		mHeight(source.getHeight()),
		mNumberOfSlices(source.getNumberOfSlices())
	{
		// Not supported
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			++getRenderer().getStatistics().numberOfCreatedTexture2DArrays;
			++getRenderer().getStatistics().currentNumberOfTexture2DArrays;
		#endif
	}

	/**
	*  @brief
	*    Copy operator
	*/
	inline ITexture2DArray &ITexture2DArray::operator =(const ITexture2DArray &)
	{
		// Not supported
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
