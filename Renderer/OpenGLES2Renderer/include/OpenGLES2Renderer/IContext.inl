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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Return the handle of a native OS window which is valid as long as the renderer instance exists
	*/
	inline handle IContext::getNativeWindowHandle() const
	{
		return mNativeWindowHandle;
	}

	/**
	*  @brief
	*    Return the used EGL display
	*/
	inline EGLDisplay IContext::getEGLDisplay() const
	{
		return mDisplay;
	}

	/**
	*  @brief
	*    Return the used EGL config
	*/
	inline EGLConfig IContext::getEGLConfig() const
	{
		return mConfig;
	}

	/**
	*  @brief
	*    Return the used EGL context
	*/
	inline EGLContext IContext::getEGLContext() const
	{
		return mContext;
	}

	/**
	*  @brief
	*    Return the used EGL dummy surface
	*/
	inline EGLSurface IContext::getEGLDummySurface() const
	{
		return mDummySurface;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
