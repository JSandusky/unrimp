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
#include "RendererRuntime/Export.h"

#include <string>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4251)	// warning C4251: 'std::_String_alloc<std::_String_base_types<_Elem,_Alloc>>::_Mypair': class 'std::_Compressed_pair<std::_Wrap_alloc<std::allocator<char>>,std::_String_val<std::_Simple_types<char>>,true>' needs to have dll-interface to be used by clients of class 'std::_String_alloc<std::_String_base_types<_Elem,_Alloc>>'


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    "std::string" as easy-to-handle class
	*
	*  @remarks
	*    Using "std::string" directly has several drawbacks such as
	*    - No simple forward declaration of "std::string" possible
	*    - In each compilation module ("cpp") an own "std::string" template instance is created,
	*      the linker later on then tries to merge those instances which is rather inefficient
	*    - Due to "wall" compiler warnings, we need to disable warnings when including external headers
	*   which are a problem for large scale projects. Additionally, there are handy default instances.
	*/
	class RENDERERRUNTIME_API_EXPORT String : public std::string
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const String EMPTY;	///< Empty string


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		String();


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_POP
