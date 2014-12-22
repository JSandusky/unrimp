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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <inttypes.h>	// For uint32_t, uint64_t etc.


//[-------------------------------------------------------]
//[ C++11 definitions                                     ]
//[-------------------------------------------------------]
#ifdef WIN32
	// Microsoft Visual Studio 2010: Have a look at http://blogs.msdn.com/b/vcblog/archive/2010/04/06/c-0x-core-language-features-in-vc10-the-table.aspx see which C++11 features are supported

	// "nullptr"-definition
	#if defined(__INTEL_COMPILER) || !defined(_MSC_VER) || _MSC_VER < 1600	// The Intel C++ compiler has no support for nullptr, 1600 = Microsoft Visual Studio 2010
		/**
		*  @brief
		*    nullptr definition for compilers don't supporting this C++11 feature
		*
		*  @note
		*    - The implementation comes from the "A name for the null pointer: nullptr"-document (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2431.pdf)
		*/
		const class
		{
			public:
				template<class T> operator T*() const  { return 0; }
				template<class C, class T> operator T C::*() const  { return 0; }
			private:
				void operator&() const;
		} nullptr = {};
	#endif
#elif LINUX
	// GCC: Have a look at http://gcc.gnu.org/projects/cxx0x.html see which C++11 features are supported

	// "nullptr"-definition
	#if (__GNUC__ == 4 && __GNUC_MINOR__ == 5 && __GNUC_PATCHLEVEL__ < 3)
		// There's a bug in GCC 4.5.x ... 4.5.2 causing compiler errors when using the nullptr-class solution from below:
		//   "Bug 45383 - [4.5 Regression] Implicit conversion to pointer does no longer automatically generate operator== and operator!=."
		//   (http://gcc.gnu.org/bugzilla/show_bug.cgi?id=45383)
		// ... looked a few minutes for a solution, without success... (just defining it as 0 is no solution and will introduce other compiler errors!)
		#error "Due to a bug in GCC 4.5.x ... 4.5.2 this GCC version is not supported. Please use a newer or older GCC version instead."
	#elif ((__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6)) && !defined(__clang__))
		/**
		*  @brief
		*    nullptr definition for compilers don't supporting this C++11 feature
		*
		*  @note
		*    - The implementation comes from the "A name for the null pointer: nullptr"-document (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2431.pdf)
		*/
		const class
		{
			public:
				template<class T> operator T*() const  { return 0; }
				template<class C, class T> operator T C::*() const  { return 0; }
			private:
				void operator&() const;
		} nullptr = {};
	#endif

	// "override"-definition, see e.g. http://www2.research.att.com/~bs/C++0xFAQ.html#override (C++11 keyword is introduced in GCC 4.7 or clang 2.9)
	#if ((__GNUC__ < 4 || __GNUC_MINOR__ < 7) && !defined(__clang__))
		#define override
	#endif
#else
	#error "Unsupported platform"
#endif


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
