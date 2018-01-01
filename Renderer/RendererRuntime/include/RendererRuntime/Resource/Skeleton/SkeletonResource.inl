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

	inline const uint8_t* SkeletonResource::getBoneParentIndices() const
	{
		return mBoneParentIndices;
	}

	inline const uint32_t* SkeletonResource::getBoneIds() const
	{
		return mBoneIds;
	}

	inline glm::mat4* SkeletonResource::getLocalBoneMatrices()
	{
		return mLocalBoneMatrices;
	}

	inline const glm::mat4* SkeletonResource::getLocalBoneMatrices() const
	{
		return mLocalBoneMatrices;
	}

	inline const glm::mat4* SkeletonResource::getBoneOffsetMatrices() const
	{
		return mBoneOffsetMatrices;
	}

	inline const glm::mat4* SkeletonResource::getGlobalBoneMatrices() const
	{
		return mGlobalBoneMatrices;
	}

	inline const glm::mat3x4* SkeletonResource::getBoneSpaceMatrices() const
	{
		return mBoneSpaceMatrices;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline SkeletonResource::SkeletonResource() :
		mNumberOfBones(0),
		mBoneParentIndices(nullptr),
		mBoneIds(nullptr),
		mLocalBoneMatrices(nullptr),
		mBoneOffsetMatrices(nullptr),
		mGlobalBoneMatrices(nullptr),
		mBoneSpaceMatrices(nullptr)
	{
		// Nothing here
	}

	inline SkeletonResource::~SkeletonResource()
	{
		// Sanity checks
		assert(0 == mNumberOfBones);
		assert(nullptr == mBoneParentIndices);
		assert(nullptr == mBoneIds);
		assert(nullptr == mLocalBoneMatrices);
		assert(nullptr == mBoneOffsetMatrices);
		assert(nullptr == mGlobalBoneMatrices);
		assert(nullptr == mBoneSpaceMatrices);
	}

	inline void SkeletonResource::clearSkeletonData()
	{
		mNumberOfBones = 0;
		delete [] mBoneParentIndices;
		mBoneParentIndices = nullptr;
		// delete [] mBoneIds;				// The complete skeleton data is sequential in memory, so, deleting "mBoneParentIndices" is does it all
		mBoneIds = nullptr;
		// delete [] mLocalBoneMatrices;	// The complete skeleton data is sequential in memory, so, deleting "mBoneParentIndices" is does it all
		mLocalBoneMatrices = nullptr;
		// delete [] mBoneOffsetMatrices;	// The complete skeleton data is sequential in memory, so, deleting "mBoneParentIndices" is does it all
		mBoneOffsetMatrices = nullptr;
		// delete [] mGlobalBoneMatrices;	// The complete skeleton data is sequential in memory, so, deleting "mBoneParentIndices" is does it all
		mGlobalBoneMatrices = nullptr;
		// delete [] mBoneSpaceMatrices;	// The complete skeleton data is sequential in memory, so, deleting "mBoneParentIndices" is does it all
		mBoneSpaceMatrices = nullptr;
	}

	inline void SkeletonResource::initializeElement(SkeletonResourceId skeletonResourceId)
	{
		// Sanity checks
		assert(0 == mNumberOfBones);
		assert(nullptr == mBoneParentIndices);
		assert(nullptr == mBoneIds);
		assert(nullptr == mLocalBoneMatrices);
		assert(nullptr == mBoneOffsetMatrices);
		assert(nullptr == mGlobalBoneMatrices);
		assert(nullptr == mBoneSpaceMatrices);

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
