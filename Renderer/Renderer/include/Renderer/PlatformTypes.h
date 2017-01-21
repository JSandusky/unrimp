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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <inttypes.h>	// For uint32_t, uint64_t etc.
#include <cassert>


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
	#if X64_ARCHITECTURE
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
	// TODO(co) Do we really want to include the string stuff in here?
	#include <strings.h>
	#define stricmp(a, b)	strcasecmp(a, b)
#endif


//[-------------------------------------------------------]
//[ Debug                                                 ]
//[-------------------------------------------------------]
// Debug macros
#ifdef RENDERER_NO_DEBUG
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

	// OUTPUT_DEBUG_* macros
	// -> Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
	// -> Debugging stuff is not supported
	#define RENDERER_OUTPUT_DEBUG_STRING(outputString)
	#define RENDERER_OUTPUT_DEBUG_PRINTF(outputString, ...)
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
		assert(strlen(name) < 256); \
		char detailedName[256 + numberOfDecorationCharacters] = decoration; \
		if (name[0] != '\0') \
		{ \
			strcat(detailedName, ": "); \
			strncat(detailedName, name, 256); \
		}

	// OUTPUT_DEBUG_* macros
	// -> Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
	#ifdef _DEBUG
		#ifdef WIN32
			#ifndef RENDERER_OUTPUT_DEBUG
				#define RENDERER_OUTPUT_DEBUG
				#include <Renderer/WindowsHeader.h>
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
			#include <iostream>
			#define RENDERER_OUTPUT_DEBUG_STRING(outputString) std::cerr << outputString << '\n';
			#define RENDERER_OUTPUT_DEBUG_PRINTF(outputString, ...) fprintf(stderr, outputString, __VA_ARGS__);
		#else
			#error "Unsupported platform"
		#endif
	#else
		#define RENDERER_OUTPUT_DEBUG_STRING(outputString)
		#define RENDERER_OUTPUT_DEBUG_PRINTF(outputString, ...)
	#endif
#endif
