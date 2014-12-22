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
#ifndef __RENDERER_RUNTIME_PLATFORM_TYPES_H__
#define __RENDERER_RUNTIME_PLATFORM_TYPES_H__


// Contains only stuff which is not inside the public "Renderer/Public/Renderer.h"-header


//[-------------------------------------------------------]
//[ Data types                                            ]
//[-------------------------------------------------------]
#ifdef WIN32
	// To export classes, methods and variables
	// -> Do not add this within the public "RendererRuntime/RendererRuntime.h"-header, it's for the internal implementation only
	#define GENERIC_API_EXPORT	extern "C" __declspec(dllexport)
#elif LINUX
	// To export classes, methods and variables
	// -> Do not add this within the public "RendererRuntime/RendererRuntime.h"-header, it's for the internal implementation only
	#if defined(HAVE_VISIBILITY_ATTR)
		#define GENERIC_API_EXPORT	extern "C" __attribute__ ((visibility("default")))
	#else
		#define GENERIC_API_EXPORT	extern "C"
	#endif
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Debug                                                 ]
//[-------------------------------------------------------]
// Debug macros
#ifdef RENDERER_NO_DEBUG
	// Debugging stuff is not supported
	// -> Do not add this within the public "RendererRuntime/RendererRuntime.h"-header, it's for the internal implementation only
	#define RENDERER_OUTPUT_DEBUG_STRING(outputString)
	#define RENDERER_OUTPUT_DEBUG_PRINTF(outputString, ...)
#else
	// OUTPUT_DEBUG_* macros
	// -> Do not add this within the public "RendererRuntime/RendererRuntime.h"-header, it's for the internal implementation only
	#ifdef _DEBUG
		#ifdef WIN32
			#ifndef RENDERER_OUTPUT_DEBUG
				#define RENDERER_OUTPUT_DEBUG
				#include "RendererRuntime/WindowsHeader.h"
				#include <strsafe.h>	// For "StringCbVPrintf()"
				#define RENDERER_OUTPUT_DEBUG_STRING(outputString) OutputDebugString(TEXT(outputString));
				inline void outputDebugPrintf(LPCTSTR outputString, ...)
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
				#define RENDERER_OUTPUT_DEBUG_PRINTF(outputString, ...) outputDebugPrintf(outputString, __VA_ARGS__);
			#endif
		#elif LINUX
			// Debugging stuff is not supported
			#define RENDERER_OUTPUT_DEBUG_STRING(outputString)
			#define RENDERER_OUTPUT_DEBUG_PRINTF(outputString, ...)
		#else
			#error "Unsupported platform"
		#endif
	#else
		#define RENDERER_OUTPUT_DEBUG_STRING(outputString)
		#define RENDERER_OUTPUT_DEBUG_PRINTF(outputString, ...)
	#endif
#endif


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_RUNTIME_PLATFORM_TYPES_H__
