/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <inttypes.h>	// For uint32_t, uint64_t etc.


//[-------------------------------------------------------]
//[ Data types                                            ]
//[-------------------------------------------------------]
#ifdef WIN32
	#ifdef X64_ARCHITECTURE
		typedef unsigned __int64 handle;	// Replacement for nasty Microsoft Windows stuff leading to header chaos
	#else
		typedef unsigned __int32 handle;	// Replacement for nasty Microsoft Windows stuff leading to header chaos
	#endif
	#ifndef NULL_HANDLE
		#define NULL_HANDLE 0
	#endif

	// For debugging
	#ifndef OUTPUT_DEBUG
		#ifdef _DEBUG
			#include "Framework/WindowsHeader.h"
			#include <strsafe.h>	// For "StringCbVPrintf()"
			#define OUTPUT_DEBUG
			#define OUTPUT_DEBUG_STRING(outputString) OutputDebugString(TEXT(outputString));
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
			#define OUTPUT_DEBUG_PRINTF(outputString, ...) outputDebugPrintf(outputString, __VA_ARGS__);
		#else
			#define OUTPUT_DEBUG_STRING(outputString)
			#define OUTPUT_DEBUG_PRINTF(outputString, ...)
		#endif
	#endif
#elif LINUX
	#if X64_ARCHITECTURE
		typedef uint64_t handle;
	#else
		typedef uint32_t handle;
	#endif
	#ifndef NULL_HANDLE
		#define NULL_HANDLE 0
	#endif

	// Debugging stuff is not supported
	#define OUTPUT_DEBUG_STRING(outputString)
	#define OUTPUT_DEBUG_PRINTF(outputString, ...)
#else
	#error "Unsupported platform"
#endif
