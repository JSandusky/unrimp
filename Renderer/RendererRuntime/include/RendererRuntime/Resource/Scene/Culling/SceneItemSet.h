/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4100)	// warning C4100: 'address': unreferenced formal parameter
	PRAGMA_WARNING_DISABLE_MSVC(4324)	// warning C4324: 'xsimd::hadd::<unnamed-tag>': structure was padded due to alignment specifier
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'uint32_t' to 'int32_t', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4505)	// warning C4505: 'xsimd::detail::__ieee754_rem_pio2': unreferenced local function has been removed
	#include <xsimd/xsimd.hpp>
PRAGMA_WARNING_POP

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class ISceneItem;
}


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
	*    Scene item set
	*
	*  @note
	*    - Basing on "The Implementation of Frustum Culling in Stingray" - http://bitsquid.blogspot.de/2016/10/the-implementation-of-frustum-culling.html
	*/
	struct SceneItemSet
	{


		//[-------------------------------------------------------]
		//[ Public definitions                                    ]
		//[-------------------------------------------------------]
		typedef std::vector<float, xsimd::aligned_allocator<float, XSIMD_DEFAULT_ALIGNMENT>>			 FloatVector;
		typedef std::vector<uint32_t, xsimd::aligned_allocator<uint32_t, XSIMD_DEFAULT_ALIGNMENT>>		 IntegerVector;
		typedef std::vector<ISceneItem*, xsimd::aligned_allocator<ISceneItem*, XSIMD_DEFAULT_ALIGNMENT>> SceneItemVector;	// TODO(co) No raw pointers here (no smart pointers either, handles please)


		//[-------------------------------------------------------]
		//[ Public data                                           ]
		//[-------------------------------------------------------]
		// Minimum object space bounding box corner position
		FloatVector minimumX;
		FloatVector minimumY;
		FloatVector minimumZ;

		// Maximum object space bounding box corner position
		FloatVector maximumX;
		FloatVector maximumY;
		FloatVector maximumZ;

		// Object space to world space matrix
		FloatVector worldXX;
		FloatVector worldXY;
		FloatVector worldXZ;
		FloatVector worldXW;
		FloatVector worldYX;
		FloatVector worldYY;
		FloatVector worldYZ;
		FloatVector worldYW;
		FloatVector worldZX;
		FloatVector worldZY;
		FloatVector worldZZ;
		FloatVector worldZW;
		FloatVector worldWX;
		FloatVector worldWY;
		FloatVector worldWZ;
		FloatVector worldWW;

		// World space center position of bounding sphere (the bounding sphere isn't always at the object center, so we need to store this beside the transform position)
		FloatVector spherePositionX;
		FloatVector spherePositionY;
		FloatVector spherePositionZ;

		// Negative world space radius of bounding sphere, the bounding sphere radius is enclosing the bounding box (don't use the inner bounding box radius)
		FloatVector negativeRadius;

		// Flag to indicate if an object is culled or not
		IntegerVector visibilityFlag;

		// The type and ID of an object
		SceneItemVector sceneItemVector;

		uint32_t numberOfSceneItems = 0;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
