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
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <inttypes.h>	// For uint32_t, uint64_t etc.
PRAGMA_WARNING_POP
#include <string.h>		// For "strcat()" and "strncat()", we include it here because inside this header macros are defined which uses those methods


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

	// To export classes, methods and variables
	// -> Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
	#define GENERIC_API_EXPORT	extern "C" __declspec(dllexport)
#elif LINUX
	#ifdef X64_ARCHITECTURE
		typedef uint64_t handle;
	#else
		typedef uint32_t handle;
	#endif
	#ifndef NULL_HANDLE
		#define NULL_HANDLE 0
	#endif

	// To export classes, methods and variables
	// -> Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
	#if defined(HAVE_VISIBILITY_ATTR)
		#define GENERIC_API_EXPORT	extern "C" __attribute__ ((visibility("default")))
	#else
		#define GENERIC_API_EXPORT	extern "C"
	#endif
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
#ifdef LINUX
	#define stricmp(a, b)	strcasecmp(a, b)
#endif


//[-------------------------------------------------------]
//[ Debug                                                 ]
//[-------------------------------------------------------]
// Debug macros
#ifdef RENDERER_DEBUG
	/**
	*  @brief
	*    Set a debug marker
	*
	*  @param[in] renderer
	*    Pointer to the renderer instance to use, can be a null pointer
	*  @param[in] name
	*    ASCII name of the debug marker
	*
	*  @note
	*    - Only for renderer backend internal usage, don't expose it inside the public renderer header
	*/
	#define RENDERER_SET_DEBUG_MARKER(renderer, name) if (nullptr != renderer) { (renderer)->setDebugMarker(name); }

	/**
	*  @brief
	*    Set a debug marker by using the current function name ("__FUNCTION__") as marker name
	*
	*  @param[in] renderer
	*    Pointer to the renderer instance to use, can be a null pointer
	*
	*  @note
	*    - Only for renderer backend internal usage, don't expose it inside the public renderer header
	*/
	#define RENDERER_SET_DEBUG_MARKER_FUNCTION(renderer) if (nullptr != renderer) { (renderer)->setDebugMarker(__FUNCTION__); }

	/**
	*  @brief
	*    Begin debug event
	*
	*  @param[in] renderer
	*    Pointer to the renderer instance to use, can be a null pointer
	*  @param[in] name
	*    ASCII name of the debug event
	*
	*  @note
	*    - Only for renderer backend internal usage, don't expose it inside the public renderer header
	*/
	#define RENDERER_BEGIN_DEBUG_EVENT(renderer, name) if (nullptr != renderer) { (renderer)->beginDebugEvent(name); }

	/**
	*  @brief
	*    Begin debug event by using the current function name ("__FUNCTION__") as event name
	*
	*  @param[in] renderer
	*    Pointer to the renderer instance to use, can be a null pointer
	*
	*  @note
	*    - Only for renderer backend internal usage, don't expose it inside the public renderer header
	*/
	#define RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer) if (nullptr != renderer) { (renderer)->beginDebugEvent(__FUNCTION__); }

	/**
	*  @brief
	*    End the last started debug event
	*
	*  @param[in] renderer
	*    Pointer to the renderer instance to use, can be a null pointer
	*
	*  @note
	*    - Only for renderer backend internal usage, don't expose it inside the public renderer header
	*/
	#define RENDERER_END_DEBUG_EVENT(renderer) if (nullptr != renderer) { (renderer)->endDebugEvent(); }

	/**
	*  @brief
	*    Assign a name to a given resource for debugging purposes
	*
	*  @param[in] resource
	*    Resource to assign the debug name to, can be a null pointer
	*  @param[in] name
	*    ASCII name for debugging purposes, must be valid (there's no internal null pointer test)
	*/
	#define RENDERER_SET_RESOURCE_DEBUG_NAME(resource, name) if (nullptr != resource) { (resource)->setDebugName(name); }

	/**
	*  @brief
	*    Decorate the debug name to make it easier to see the semantic of the resource
	*
	*  @param[in] name
	*    Debug name provided from the outside
	*  @param[in] decoration
	*    Decoration to append in front (e.g. "IBO", will result in appended "IBO: " in front if the provided name isn't empty)
	*  @param[in] numberOfDecorationCharacters
	*    Number of decoration characters
	*
	*  @note
	*    - The result is in local string variable "detailedName"
	*    - Traditional C-string on the runtime stack used for efficiency reasons (just for debugging, but must still be some kind of usable)
	*    - Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
	*/
	#define RENDERER_DECORATED_DEBUG_NAME(name, detailedName, decoration, numberOfDecorationCharacters) \
		RENDERER_ASSERT(getRenderer().getContext(), strlen(name) < 256, "Name is not allowed to exceed 255 characters") \
		char detailedName[256 + numberOfDecorationCharacters] = decoration; \
		if (name[0] != '\0') \
		{ \
			strcat(detailedName, ": "); \
			strncat(detailedName, name, 256); \
		}
#else
	/**
	*  @brief
	*    Set a debug marker
	*
	*  @param[in] renderer
	*    Pointer to the renderer instance to use, can be a null pointer
	*  @param[in] name
	*    ASCII name of the debug marker
	*
	*  @note
	*    - Only for renderer backend internal usage, don't expose it inside the public renderer header
	*/
	#define RENDERER_SET_DEBUG_MARKER(renderer, name)

	/**
	*  @brief
	*    Set a debug marker by using the current function name ("__FUNCTION__") as marker name
	*
	*  @param[in] renderer
	*    Pointer to the renderer instance to use, can be a null pointer
	*
	*  @note
	*    - Only for renderer backend internal usage, don't expose it inside the public renderer header
	*/
	#define RENDERER_SET_DEBUG_MARKER_FUNCTION(renderer)

	/**
	*  @brief
	*    Begin debug event
	*
	*  @param[in] renderer
	*    Pointer to the renderer instance to use, can be a null pointer
	*  @param[in] name
	*    ASCII name of the debug event
	*
	*  @note
	*    - Only for renderer backend internal usage, don't expose it inside the public renderer header
	*/
	#define RENDERER_BEGIN_DEBUG_EVENT(renderer, name)

	/**
	*  @brief
	*    Begin debug event by using the current function name ("__FUNCTION__") as event name
	*
	*  @param[in] renderer
	*    Pointer to the renderer instance to use, can be a null pointer
	*
	*  @note
	*    - Only for renderer backend internal usage, don't expose it inside the public renderer header
	*/
	#define RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

	/**
	*  @brief
	*    End the last started debug event
	*
	*  @param[in] renderer
	*    Pointer to the renderer instance to use, can be a null pointer
	*
	*  @note
	*    - Only for renderer backend internal usage, don't expose it inside the public renderer header
	*/
	#define RENDERER_END_DEBUG_EVENT(renderer)

	/**
	*  @brief
	*    Assign a name to a given resource for debugging purposes
	*
	*  @param[in] resource
	*    Resource to assign the debug name to, can be a null pointer
	*  @param[in] name
	*    ASCII name for debugging purposes, must be valid (there's no internal null pointer test)
	*/
	#define RENDERER_SET_RESOURCE_DEBUG_NAME(resource, name)
#endif
