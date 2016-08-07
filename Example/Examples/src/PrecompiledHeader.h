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


// Do never ever include this optional precompiled header within another public header!
// The precompiled header is only used to speed up compilation, it's no replacement for
// a decent header design. Be clever, don't use precompiled headers like a sledgehammer
// or you increase instead of decrease the compile time faster as you can say "bad idea".


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
// We can't use pragma once here since precompiled headers get a special treatment on Mac OS X that is incompatible with #pragma once
#ifndef _RENDERER_RUNTIME_PRECOMPILED_HEADER_H_
#define _RENDERER_RUNTIME_PRECOMPILED_HEADER_H_


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
// Renderer (stable)
#include <Renderer/Public/Renderer.h>

// GLM
// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#include <glm/glm.hpp>
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/matrix_transform.hpp>
#pragma warning(pop)

// C++ standard headers
#include <math.h>
#include <string.h>
#include <float.h>
#include <stdlib.h>
#include <limits.h>


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // _RENDERER_RUNTIME_PRECOMPILED_HEADER_H_
