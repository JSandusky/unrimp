/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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


// Contains only stuff which is not inside the public "Renderer/Public/Renderer.h"-header


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <RendererRuntime/Core/Platform/PlatformTypes.h>


//[-------------------------------------------------------]
//[ Error                                                 ]
//[-------------------------------------------------------]
// OUTPUT_ERROR_* macros
// -> Do not add this within the public "RendererToolkit/RendererToolkit.h"-header, it's for the internal implementation only
#ifdef WIN32
	#ifndef RENDERERTOOLKIT_OUTPUT_ERROR
		#define RENDERERTOOLKIT_OUTPUT_ERROR
		#include "RendererToolkit/WindowsHeader.h"
		#include <strsafe.h>	// For "StringCbVPrintf()"
		#define RENDERERTOOLKIT_OUTPUT_ERROR_STRING(outputString) OutputDebugString(TEXT(outputString));
		inline void rendererToolkitOutputErrorPrintf(LPCTSTR outputString, ...)
		{
			va_list argptr;
			va_start(argptr, outputString);
			TCHAR buffer[2000];
			const HRESULT hr = StringCbVPrintf(buffer, sizeof(buffer), outputString, argptr);
			if (STRSAFE_E_INSUFFICIENT_BUFFER == hr || S_OK == hr)
			{
				OutputDebugString(buffer);
			}
			else
			{
				OutputDebugString(TEXT("\"StringCbVPrintf()\" error"));
			}
		}
		#define RENDERERTOOLKIT_OUTPUT_ERROR_PRINTF(outputString, ...) rendererToolkitOutputErrorPrintf(outputString, __VA_ARGS__);
	#endif
#elif LINUX
	// TODO(co) Error stuff is not supported for now
	#define RENDERERTOOLKIT_OUTPUT_ERROR_STRING(outputString)
	#define RENDERERTOOLKIT_OUTPUT_ERROR_PRINTF(outputString, ...)
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Debug                                                 ]
//[-------------------------------------------------------]
// OUTPUT_DEBUG_* macros
// -> Do not add this within the public "RendererToolkit/RendererToolkit.h"-header, it's for the internal implementation only
#ifdef RENDERER_NO_DEBUG
	// Debugging stuff is not supported
	#define RENDERERTOOLKIT_OUTPUT_DEBUG_STRING(outputString)
	#define RENDERERTOOLKIT_OUTPUT_DEBUG_PRINTF(outputString, ...)
#else
	#ifdef _DEBUG
		#define RENDERERTOOLKIT_OUTPUT_DEBUG_STRING(outputString)	   RENDERERTOOLKIT_OUTPUT_ERROR_STRING(outputString)
		#define RENDERERTOOLKIT_OUTPUT_DEBUG_PRINTF(outputString, ...) RENDERERTOOLKIT_OUTPUT_ERROR_PRINTF(outputString, __VA_ARGS__)
	#else
		#define RENDERERTOOLKIT_OUTPUT_DEBUG_STRING(outputString)
		#define RENDERERTOOLKIT_OUTPUT_DEBUG_PRINTF(outputString, ...)
	#endif
#endif
