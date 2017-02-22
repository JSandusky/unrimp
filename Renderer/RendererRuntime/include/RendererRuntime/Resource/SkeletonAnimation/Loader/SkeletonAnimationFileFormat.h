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
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4201)	// warning C4201: nonstandard extension used: nameless struct/union
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	#include <glm/gtc/quaternion.hpp>
PRAGMA_WARNING_POP

#include <inttypes.h>	// For uint32_t, uint64_t etc.


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	// -> Skeleton animation file format content:
	//    - Skeleton animation header
	//    - n bone channels
	//      - 1..n position keys
	//      - 1..n rotation keys
	//      - 1..n scale keys
	namespace v1SkeletonAnimation
	{


		//[-------------------------------------------------------]
		//[ Definitions                                           ]
		//[-------------------------------------------------------]
		static const uint32_t FORMAT_TYPE	 = StringId("SkeletonAnimation");
		static const uint32_t FORMAT_VERSION = 1;

		#pragma pack(push)
		#pragma pack(1)
			struct Header
			{
				uint32_t formatType;
				uint16_t formatVersion;
				uint8_t  numberOfChannels;	///< The number of bone animation channels; each channel affects a single node
				float	 durationInTicks;	///< Duration of the animation in ticks
				float	 ticksPerSecond;	///< Ticks per second; 0 if not specified in the imported file
			};
			// TODO(co) We also need to store the skeleton hierarchy so we can perform a runtime matching and retargeting if required
			// TODO(co) We might also want to add a frame-key table to speed up pose calculation during runtime
			struct BoneChannelHeader
			{
				uint32_t boneId;				///< Bone ID ("RendererRuntime::StringId" on bone name)
				uint32_t numberOfPositionKeys;	///< Number of position keys, must be at least one
				uint32_t numberOfRotationKeys;	///< Number of rotation keys, must be at least one
				uint32_t numberOfScaleKeys;		///< Number of scale keys, must be at least one
			};
			struct Vector3Key
			{
				float	  time;		///< The time of this key
				glm::vec3 value;	///< The value of this key
			};
			struct QuaternionKey
			{
				float	  time;		///< The time of this key
				glm::quat value;	///< The value of this key
			};
		#pragma pack(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
	} // v1SkeletonAnimation
} // RendererRuntime
