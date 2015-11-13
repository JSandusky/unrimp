/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4201)	// warning C4201: nonstandard extension used: nameless struct/union
	#include <glm/glm.hpp>
#pragma warning(pop)
#pragma warning(push)
	#pragma warning(disable: 4608)	// warning C4608: 'glm::tvec3<float,0>::<unnamed-tag>::s' has already been initialized by another union member in the initializer list, 'glm::tvec3<float,0>::<unnamed-tag>::b'


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
	*    "glm::vec3" as easy-to-handle class
	*
	*  @remarks
	*    Using "glm::vec3" directly has several drawbacks such as
	*    - No simple forward declaration of "glm::vec3" possible
	*    - In each compilation module ("cpp") an own "glm::vec3" template instance is created,
	*      the linker later on then tries to merge those instances which is rather inefficient
	*    - Due to "wall" compiler warnings, we need to disable warnings when including external headers
	*   which are a problem for large scale projects. Additionally, there are handy default instances.
	*/
	class RENDERERRUNTIME_API_EXPORT Vector3 : public glm::vec3
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const Vector3 ZERO;		///< Zero vector
		static const Vector3 UNIT_X;	///< Unit vector x-axis
		static const Vector3 UNIT_Y;	///< Unit vector y-axis
		static const Vector3 UNIT_Z;	///< Unit vector z-axis
		static const Vector3 UNIT_XYZ;	///< Sum of unit vectors


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		Vector3();
		inline Vector3(float x, float y, float z);


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


// Disable warnings in external headers, we can't fix them
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/Vector3.inl"
