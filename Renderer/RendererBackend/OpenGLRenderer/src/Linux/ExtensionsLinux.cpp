/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"

#include <Renderer/LinuxHeader.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool Extensions::initialize(bool useExtensions)
	{
		// Disable the following warning, we can't do anything to resolve this warning
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
		//[ GLX (Linux only) definitions                          ]
		//[-------------------------------------------------------]

		// WGL_EXT_swap_control
		if (isSupported("GLX_SGI_swap_control"))
		{
			glXSwapIntervalSGI = reinterpret_cast<PFNGLXSWAPINTERVALSGIPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXSwapIntervalSGI")));
			if (nullptr == glXSwapIntervalSGI)
			{
				mGLX_SGI_swap_control = false;
			}
			else
			{
				mGLX_SGI_swap_control = true;
			}
		}
		else
		{
			mGLX_SGI_swap_control = false;
		}

		// WGL_ARB_multisample
		mWGL_ARB_multisample = isSupported("GLX_ARB_multisample");

		// WGL_ATI_pixel_format_float
		mWGL_ATI_pixel_format_float = isSupported("GLX_ATI_pixel_format_float");

		// WGL_NV_float_buffer
		mWGL_NV_float_buffer = isSupported("GLX_NV_float_buffer");


		// Initialize the supported universal extensions
		return initializeUniversal();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
