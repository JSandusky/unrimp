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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __RENDERER_RUNTIME_PLATFORM_TYPES_H__
#define __RENDERER_RUNTIME_PLATFORM_TYPES_H__


// Contains only stuff which is not inside the public "Renderer/Public/Renderer.h"-header


//[-------------------------------------------------------]
//[ C++ compiler keywords                                 ]
//[-------------------------------------------------------]
#ifdef WIN32
	/**
	*  @brief
	*    Force the compiler to inline something
	*
	*  @note
	*    - Do only use this when you really have to, usually it's best to let the compiler decide by using the standard keyword "inline"
	*/
	#define FORCEINLINE __forceinline

	/**
	*  @brief
	*    No operation macro ("_asm nop"/"__nop()")
	*/
	#define NOP __nop()
#elif LINUX
	/**
	*  @brief
	*    Force the compiler to inline something
	*
	*  @note
	*    - Do only use this when you really have to, usually it's best to let the compiler decide by using the standard keyword "inline"
	*/
	#define FORCEINLINE __attribute__((always_inline))

	/**
	*  @brief
	*    No operation macro ("_asm nop"/__nop())
	*/
	#define NOP __nop()	// TODO(co) This will probably not work, when it's time for Unix ports, review this
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Project independent generic export                    ]
//[-------------------------------------------------------]
#ifdef WIN32
	// To export classes, methods and variables
	#define GENERIC_API_EXPORT	__declspec(dllexport)

	// To export functions
	#define GENERIC_FUNCTION_EXPORT	extern "C" __declspec(dllexport)
#elif LINUX
	// To export classes, methods and variables
	#if defined(HAVE_VISIBILITY_ATTR)
		#define GENERIC_API_EXPORT __attribute__ ((visibility("default")))
	#else
		#define GENERIC_API_EXPORT
	#endif

	// To export functions
	#define GENERIC_FUNCTION_EXPORT	__attribute__ ((visibility("default")))
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Project independent generic import                    ]
//[-------------------------------------------------------]
#ifdef WIN32
	// To import classes, methods and variables
	#define GENERIC_API_IMPORT	__declspec(dllimport)
#elif LINUX
	// To import classes, methods and variables
	#define GENERIC_API_IMPORT
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Error                                                 ]
//[-------------------------------------------------------]
// OUTPUT_ERROR_* macros
// -> Do not add this within the public "RendererRuntime/RendererRuntime.h"-header, it's for the internal implementation only
#ifdef WIN32
	#ifndef RENDERERRUNTIME_OUTPUT_ERROR
		#define RENDERERRUNTIME_OUTPUT_ERROR
		#include "RendererRuntime/WindowsHeader.h"
		#include <strsafe.h>	// For "StringCbVPrintf()"
		#define RENDERERRUNTIME_OUTPUT_ERROR_STRING(outputString) OutputDebugString(TEXT(outputString));
		inline void rendererRuntimeOutputErrorPrintf(LPCTSTR outputString, ...)
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
		#define RENDERERRUNTIME_OUTPUT_ERROR_PRINTF(outputString, ...) rendererRuntimeOutputErrorPrintf(outputString, __VA_ARGS__);
	#endif
#elif LINUX
	// TODO(co) Error stuff is not supported for now
	#define RENDERERRUNTIME_OUTPUT_ERROR_STRING(outputString)
	#define RENDERERRUNTIME_OUTPUT_ERROR_PRINTF(outputString, ...)
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Debug                                                 ]
//[-------------------------------------------------------]
// OUTPUT_DEBUG_* macros
// -> Do not add this within the public "RendererRuntime/RendererRuntime.h"-header, it's for the internal implementation only
#ifdef RENDERER_NO_DEBUG
	// Debugging stuff is not supported
	#define RENDERERRUNTIME_OUTPUT_DEBUG_STRING(outputString)
	#define RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF(outputString, ...)
#else
	#ifdef _DEBUG
		#define RENDERERRUNTIME_OUTPUT_DEBUG_STRING(outputString)	   RENDERERRUNTIME_OUTPUT_ERROR_STRING(outputString)
		#define RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF(outputString, ...) RENDERERRUNTIME_OUTPUT_ERROR_PRINTF(outputString, __VA_ARGS__)
	#else
		#define RENDERERRUNTIME_OUTPUT_DEBUG_STRING(outputString)
		#define RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF(outputString, ...)
	#endif
#endif


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_RUNTIME_PLATFORM_TYPES_H__
