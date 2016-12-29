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


// Do never ever include this optional precompiled header within another public header.
// The precompiled header is only used to speed up compilation, it's no replacement for
// a decent header design. Be clever, don't use precompiled headers like a sledgehammer
// or you increase instead of decrease the compile time faster as you can say "bad idea".


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
// We can't use pragma once here since precompiled headers get a special treatment on Mac OS X that is incompatible with #pragma once
#ifndef _RENDERER_RUNTIME_PRECOMPILED_HEADER_H_
#define _RENDERER_RUNTIME_PRECOMPILED_HEADER_H_


// This here should recognize when you try to include multiple precompiled headers.
// Use this block without modification of "_RENDERER_RUNTIME_PRECOMPILED_HEADER_INCLUDED_"
// in all precompiled headers of your project.
#ifdef _RENDERER_RUNTIME_PRECOMPILED_HEADER_INCLUDED_
	#error Multiple precompiled headers included. The reason could be inclusion of a precompiled header in a public header file.
#else
	#define _RENDERER_RUNTIME_PRECOMPILED_HEADER_INCLUDED_
#endif


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
// Unrimp headers which are considered to be stable and fundamental
#include "RendererRuntime/Core/Platform/PlatformTypes.h"

// GLM
// -> Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4201)	// warning C4201: nonstandard extension used: nameless struct/union
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/glm.hpp>
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/quaternion.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtx/matrix_decompose.hpp>
	#include <glm/gtx/quaternion.hpp>
PRAGMA_WARNING_POP

// C++ standard headers
#include <inttypes.h>
#include <cassert>
#include <limits>
#include <cstring>
#include <queue>
#include <deque>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include <type_traits>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <functional>
#include <chrono>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4355)	// warning C4355: 'this': used in base member initializer list
	PRAGMA_WARNING_DISABLE_MSVC(4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'Concurrency::details::_ExceptionHolder': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'Concurrency::details::_ExceptionHolder': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(5026)	// warning C5026: 'Concurrency::details::_RunAllParam<Concurrency::details::_Unit_type>': move constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'Concurrency::details::_RunAllParam<Concurrency::details::_Unit_type>': move assignment operator was implicitly defined as deleted
	#include <future>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // _RENDERER_RUNTIME_PRECOMPILED_HEADER_H_
