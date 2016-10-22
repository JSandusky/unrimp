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
//[ Includes                                              ]
//[-------------------------------------------------------]
#define EXTENSIONS_DEFINE

#include "VulkanRenderer/Extensions.h"
#include "VulkanRenderer/VulkanRuntimeLinking.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_PRINTF()"
#ifdef LINUX
	#include <Renderer/LinuxHeader.h>
	#include "VulkanRenderer/Linux/ContextLinux.h"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Extensions::Extensions(IContext &context) :
		mContext(&context),
		mInitialized(false)
	{
		// Reset extensions
		resetExtensions();
	}

	Extensions::~Extensions()
	{
		// Nothing here
	}

	bool Extensions::isInitialized() const
	{
		return mInitialized;
	}

	///////////////////////////////////////////////////////////
	// Returns whether an extension is supported or not
	///////////////////////////////////////////////////////////
	// TODO(co) Implement me


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool Extensions::isSupported(const char *extension) const
	{
		// Check whether or not the given extension string pointer is valid
		if (nullptr != extension)
		{
			// Is the extension supported by the hardware?
			if (checkExtension(extension))
			{
				// Extension is supported!
				return true;
			}
		}

		// Extension isn't supported!
		return false;
	}

	bool Extensions::checkExtension(const char *extension) const
	{
		// Check whether or not the given extension string pointer is valid
		if (nullptr != extension)
		{
			// TODO(co) Implement me
		}

		// Extension not found
		return false;
	}

	void Extensions::resetExtensions()
	{
		mInitialized = false;

		// TODO(co) Implement me
	}

	bool Extensions::initializeUniversal()
	{
		// TODO(co) Implement me

		// Done
		return true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
