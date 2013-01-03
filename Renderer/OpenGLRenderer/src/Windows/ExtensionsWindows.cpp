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
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Initialize the supported extensions
	*/
	bool Extensions::initialize(bool useExtensions)
	{
		// Disable the following warning, we can't do anything to resolve this warning
		__pragma(warning(push))
		__pragma(warning(disable: 4191))	// warning C4191: 'reinterpret_cast' : unsafe conversion from 'PROC' to '<x>'

		// Should the extensions be used?
		if (useExtensions)
		{
			mInitialized = true;
		}
		else
		{
			resetExtensions();
			mInitialized = true;

			// Done
			return true;
		}


		//[-------------------------------------------------------]
		//[ WGL (Windows only) definitions                        ]
		//[-------------------------------------------------------]
		// WGL_ARB_extensions_string
		wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB"));
		mWGL_ARB_extensions_string = (nullptr != wglGetExtensionsStringARB);

		// WGL_EXT_swap_control
		if (isSupported("WGL_EXT_swap_control"))
		{
			wglSwapIntervalEXT	  = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>	  (wglGetProcAddress("wglSwapIntervalEXT"));
			wglGetSwapIntervalEXT = reinterpret_cast<PFNWGLGETSWAPINTERVALEXTPROC>(wglGetProcAddress("wglGetSwapIntervalEXT"));
			if (nullptr == wglSwapIntervalEXT || nullptr == wglGetSwapIntervalEXT)
			{
				mWGL_EXT_swap_control = false;
			}
			else
			{
				mWGL_EXT_swap_control = true;
			}
		}
		else
		{
			mWGL_EXT_swap_control = false;
		}

		// WGL_ARB_pixel_format
		if (isSupported("WGL_ARB_pixel_format"))
		{
			wglGetPixelFormatAttribivARB = reinterpret_cast<PFNWGLGETPIXELFORMATATTRIBIVARBPROC>(wglGetProcAddress("wglGetPixelFormatAttribivARB"));
			wglGetPixelFormatAttribfvARB = reinterpret_cast<PFNWGLGETPIXELFORMATATTRIBFVARBPROC>(wglGetProcAddress("wglGetPixelFormatAttribfvARB"));
			wglChoosePixelFormatARB		 = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>		(wglGetProcAddress("wglChoosePixelFormatARB"));
			if (nullptr == wglGetPixelFormatAttribivARB || nullptr == wglGetPixelFormatAttribfvARB || nullptr == wglChoosePixelFormatARB)
			{
				mWGL_ARB_pixel_format = false;
			}
			else
			{
				mWGL_ARB_pixel_format = true;
			}
		}
		else
		{
			mWGL_ARB_pixel_format = false;
		}

		// WGL_ARB_render_texture
		if (isSupported("WGL_ARB_render_texture"))
		{
			wglBindTexImageARB	   = reinterpret_cast<PFNWGLBINDTEXIMAGEARBPROC>	(wglGetProcAddress("wglBindTexImageARB"));
			wglReleaseTexImageARB  = reinterpret_cast<PFNWGLRELEASETEXIMAGEARBPROC> (wglGetProcAddress("wglReleaseTexImageARB"));
			if (nullptr == wglBindTexImageARB || nullptr == wglReleaseTexImageARB)
			{
				mWGL_ARB_render_texture = false;
			}
			else
			{
				mWGL_ARB_render_texture = true;
			}
		}
		else
		{
			mWGL_ARB_render_texture = false;
		}

		// WGL_ARB_make_current_read
		if (isSupported("WGL_ARB_make_current_read"))
		{
			wglMakeContextCurrentARB = reinterpret_cast<PFNWGLMAKECONTEXTCURRENTARBPROC>(wglGetProcAddress("wglMakeContextCurrentARB"));
			wglGetCurrentReadDCARB   = reinterpret_cast<PFNWGLGETCURRENTREADDCARBPROC>  (wglGetProcAddress("wglGetCurrentReadDCARB"));
			if (nullptr == wglMakeContextCurrentARB || nullptr == wglGetCurrentReadDCARB)
			{
				mWGL_ARB_make_current_read = false;
			}
			else
			{
				mWGL_ARB_make_current_read = true;
			}
		} else {
			mWGL_ARB_make_current_read = false;
		}

		// WGL_ARB_multisample
		mWGL_ARB_multisample = isSupported("WGL_ARB_multisample");

		// WGL_ATI_pixel_format_float
		mWGL_ATI_pixel_format_float = isSupported("WGL_ATI_pixel_format_float");

		// WGL_NV_float_buffer
		mWGL_NV_float_buffer = isSupported("WGL_NV_float_buffer");


		// Initialize the supported universal extensions
		return initializeUniversal();

		// Restore the previous warning configuration
		__pragma(warning(pop))
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
