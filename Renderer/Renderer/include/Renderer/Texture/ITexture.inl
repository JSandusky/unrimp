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
//[ Include                                               ]
//[-------------------------------------------------------]
#include "Renderer/PlatformTypes.h"

// This is for the internal implementation only, other projects will use the public "Renderer/Public/Renderer.h"-header,
// so it's acceptable to make an include in here
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'return': conversion from 'int' to 'std::char_traits<wchar_t>::int_type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4987)	// warning C4987: nonstandard extension used: 'throw (...)'
	#include <cmath>
	#include <algorithm>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	inline uint32_t ITexture::getNumberOfMipmaps(uint32_t width)
	{
		// Don't write "return static_cast<uint32_t>(1 + std::floor(std::log2(width)));"
		// -> Android GNU STL has no "std::log2()", poor but no disaster in here because we can use another solution
		// -> log2(x) = log(x) / log(2)
		// -> log(2) = 0.69314718055994529
		return static_cast<uint32_t>(1 + std::floor(std::log(width) / 0.69314718055994529));
	}

	inline uint32_t ITexture::getNumberOfMipmaps(uint32_t width, uint32_t height)
	{
		return getNumberOfMipmaps(std::max(width, height));
	}

	inline uint32_t ITexture::getNumberOfMipmaps(uint32_t width, uint32_t height, uint32_t depth)
	{
		return getNumberOfMipmaps(width, std::max(height, depth));
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline ITexture::~ITexture()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline ITexture::ITexture(ResourceType resourceType, IRenderer& renderer) :
		IResource(resourceType, renderer)
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
