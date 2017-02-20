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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline uint8_t SkeletonResource::getNumberOfBones() const
	{
		return mNumberOfBones;
	}

	inline const uint8_t* SkeletonResource::getBoneParents() const
	{
		return mBoneParents;
	}

	inline const uint32_t* SkeletonResource::getBoneIds() const
	{
		return mBoneIds;
	}

	inline const glm::mat4* SkeletonResource::getLocalBonePoses() const
	{
		return mLocalBonePoses;
	}

	inline const glm::mat4* SkeletonResource::getBoneOffsetMatrix() const
	{
		return mBoneOffsetMatrix;
	}

	inline const glm::mat4* SkeletonResource::getGlobalBonePoses() const
	{
		return mGlobalBonePoses;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline SkeletonResource::SkeletonResource() :
		mNumberOfBones(0),
		mBoneParents(nullptr),
		mBoneIds(nullptr),
		mLocalBonePoses(nullptr),
		mBoneOffsetMatrix(nullptr),
		mGlobalBonePoses(nullptr)
	{
		// Nothing here
	}

	inline SkeletonResource::~SkeletonResource()
	{
		// Sanity checks
		assert(0 == mNumberOfBones);
		assert(nullptr == mBoneParents);
		assert(nullptr == mBoneIds);
		assert(nullptr == mLocalBonePoses);
		assert(nullptr == mBoneOffsetMatrix);
		assert(nullptr == mGlobalBonePoses);
	}

	inline void SkeletonResource::clearSkeletonData()
	{
		mNumberOfBones = 0;
		delete [] mBoneParents;
		mBoneParents = nullptr;
		// delete [] mBoneIds;			// The complete skeleton data is sequential in memory, so, deleting "mBoneParents" is does it all
		mBoneIds = nullptr;
		// delete [] mLocalBonePoses;	// The complete skeleton data is sequential in memory, so, deleting "mBoneParents" is does it all
		mLocalBonePoses = nullptr;
		// delete [] mBoneOffsetMatrix;	// The complete skeleton data is sequential in memory, so, deleting "mBoneParents" is does it all
		mBoneOffsetMatrix = nullptr;
		// delete [] mGlobalBonePoses;	// The complete skeleton data is sequential in memory, so, deleting "mBoneParents" is does it all
		mGlobalBonePoses = nullptr;
	}

	inline void SkeletonResource::initializeElement(SkeletonResourceId skeletonResourceId)
	{
		// Sanity checks
		assert(0 == mNumberOfBones);
		assert(nullptr == mBoneParents);
		assert(nullptr == mBoneIds);
		assert(nullptr == mLocalBonePoses);
		assert(nullptr == mBoneOffsetMatrix);
		assert(nullptr == mGlobalBonePoses);

		// Call base implementation
		IResource::initializeElement(skeletonResourceId);
	}

	inline void SkeletonResource::deinitializeElement()
	{
		// Reset everything
		clearSkeletonData();

		// Call base implementation
		IResource::deinitializeElement();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
