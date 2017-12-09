/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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

	/**
	*  @brief
	*    Debug break operation macro
	*/
	#define DEBUG_BREAK __debugbreak()

	/**
	*  @brief
	*    Platform specific "#pragma warning(push)" (MS Windows Visual Studio)
	*/
	#define PRAGMA_WARNING_PUSH __pragma(warning(push))

	/**
	*  @brief
	*    Platform specific "#pragma warning(pop)" (MS Windows Visual Studio)
	*/
	#define PRAGMA_WARNING_POP __pragma(warning(pop))

	/**
	*  @brief
	*    Platform specific "#pragma warning(disable: <x>)" (MS Windows Visual Studio)
	*/
	#define PRAGMA_WARNING_DISABLE_MSVC(id) __pragma(warning(disable: id))

	/**
	*  @brief
	*    Platform specific "#pragma clang diagnostic ignored <x>" (Clang)
	*/
	#define PRAGMA_WARNING_DISABLE_CLANG(id)

	/**
	*  @brief
	*    Platform specific "#pragma GCC diagnostic ignored <x>" (GCC)
	*/
	#define PRAGMA_WARNING_DISABLE_GCC(id)
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
	#define NOP asm ("nop");

	/**
	*  @brief
	*    Debug break operation macro
	*/
	#define DEBUG_BREAK __builtin_trap()

	#ifdef __clang__
		/**
		*  @brief
		*    Platform specific "#pragma clang diagnostic push" (Clang)
		*/
		#define PRAGMA_WARNING_PUSH _Pragma("clang diagnostic push")

		/**
		*  @brief
		*    Platform specific "#pragma clang diagnostic pop" (Clang)
		*/
		#define PRAGMA_WARNING_POP _Pragma("clang diagnostic pop")

		/**
		*  @brief
		*    Platform specific "#pragma warning(disable: <x>)" (MS Windows Visual Studio)
		*/
		#define PRAGMA_WARNING_DISABLE_MSVC(id)

		/**
		*  @brief
		*    Platform specific "#pragma GCC diagnostic ignored <x>" (GCC)
		*/
		#define PRAGMA_WARNING_DISABLE_GCC(id)

		/**
		*  @brief
		*    Platform specific "#pragma clang diagnostic ignored <x>" (Clang)
		*/
		// We need stringify because _Pragma expects an string literal
		#define PRAGMA_STRINGIFY(a) #a
		#define PRAGMA_WARNING_DISABLE_CLANG(id) _Pragma(PRAGMA_STRINGIFY(clang diagnostic ignored id) )
	#elif __GNUC__
		// gcc
		/**
		*  @brief
		*    Platform specific "#pragma GCC diagnostic push" (GCC)
		*/
		#define PRAGMA_WARNING_PUSH _Pragma("GCC diagnostic push")

		/**
		*  @brief
		*    Platform specific "#pragma warning(pop)" (GCC)
		*/
		#define PRAGMA_WARNING_POP _Pragma("GCC diagnostic pop")

		/**
		*  @brief
		*    Platform specific "#pragma warning(disable: <x>)" (MS Windows Visual Studio)
		*/
		#define PRAGMA_WARNING_DISABLE_MSVC(id)

		/**
		*  @brief
		*    Platform specific "#pragma GCC diagnostic ignored <x>" (GCC)
		*/
		// We need stringify because _Pragma expects an string literal
		#define PRAGMA_STRINGIFY(a) #a
		#define PRAGMA_WARNING_DISABLE_GCC(id) _Pragma(PRAGMA_STRINGIFY(GCC diagnostic ignored id) )

		/**
		*  @brief
		*    Platform specific "#pragma clang diagnostic ignored <x>" (Clang)
		*/
		#define PRAGMA_WARNING_DISABLE_CLANG(id)
	#else
		#error "Unsupported compiler"
	#endif	
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#ifdef WIN32
	#include <intrin.h>	// For "__nop()"
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
	#define GENERIC_API_EXPORT __attribute__ ((visibility("default")))

	// To export functions
	#define GENERIC_FUNCTION_EXPORT extern "C"  __attribute__ ((visibility("default")))
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
