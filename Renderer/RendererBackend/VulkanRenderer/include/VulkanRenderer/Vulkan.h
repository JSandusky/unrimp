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
#include <Renderer/PlatformTypes.h>

#if defined(_WIN32)
	#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__ANDROID__)
	#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(LINUX)
	// TODO(sw) Make this optional which platform we support? For now we support xlib and Wayland.
	#define VK_USE_PLATFORM_XLIB_KHR
	#define VK_USE_PLATFORM_WAYLAND_KHR
#endif
#define VK_NO_PROTOTYPES

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// Warning	C4668	'<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <vulkan/vulkan.h>
	#undef max	// Get rid of nasty OS macro
	#undef None // Get rid of nasty OS macro
PRAGMA_WARNING_POP
