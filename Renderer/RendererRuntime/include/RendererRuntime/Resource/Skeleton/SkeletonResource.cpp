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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Skeleton/SkeletonResource.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	uint32_t SkeletonResource::getBoneIndexByBoneId(uint32_t boneId) const
	{
		// TODO(co) Maybe it makes sense to store the bone IDs in some order to speed up the following
		for (uint8_t boneIndex = 0; boneIndex < mNumberOfBones; ++boneIndex)
		{
			if (mBoneIds[boneIndex] == boneId)
			{
				return boneIndex;
			}
		}
		return getUninitialized<uint32_t>();
	}

	void SkeletonResource::localToGlobalPose()
	{
		// The root has no parent
		mGlobalBonePoses[0] = mLocalBonePoses[0];

		// Due to cache friendly depth-first rolled up bone hierarchy, the global parent bone pose is already up-to-date
		for (uint8_t i = 1; i < mNumberOfBones; ++i)
		{
			const uint8_t parentBone = mBoneParents[i];
			mGlobalBonePoses[i] = mGlobalBonePoses[parentBone] * mLocalBonePoses[i];
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
