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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Return the used renderer instance
	*/
	inline Renderer::IRenderer &FreeTypeContext::getRenderer() const
	{
		return *mRenderer;
	}

	/**
	*  @brief
	*    Return the FreeType library object
	*
	*  @return
	*    The FreeType library object, a null pointer on error (in case of an terrible error), do not delete the instance!
	*/
	inline FT_Library *FreeTypeContext::getFTLibrary() const
	{
		return mFTLibrary;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Default constructor
	*/
	inline FreeTypeContext::FreeTypeContext() :
		mRenderer(nullptr),
		mFTLibrary(nullptr)
	{
		// Nothing do to in here
	}

	/**
	*  @brief
	*    Copy constructor
	*/
	inline FreeTypeContext::FreeTypeContext(const FreeTypeContext &) :
		mRenderer(nullptr),
		mFTLibrary(nullptr)
	{
		// Not supported
	}

	/**
	*  @brief
	*    Copy operator
	*/
	inline FreeTypeContext &FreeTypeContext::operator =(const FreeTypeContext &)
	{
		// Not supported
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
