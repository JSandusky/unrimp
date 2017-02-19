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

	inline const uint8_t* SkeletonResource::getBoneHierarchy() const
	{
		return mBoneHierarchy;
	}

	inline const glm::mat4* SkeletonResource::getLocalBonePoses() const
	{
		return mLocalBonePoses;
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
		mBoneHierarchy(nullptr),
		mLocalBonePoses(nullptr),
		mGlobalBonePoses(nullptr)
	{
		// Nothing here
	}

	inline SkeletonResource::~SkeletonResource()
	{
		// Sanity checks
		assert(0 == mNumberOfBones);
		assert(nullptr == mBoneHierarchy);
		assert(nullptr == mLocalBonePoses);
		assert(nullptr == mGlobalBonePoses);
	}

	inline void SkeletonResource::clearSkeletonData()
	{
		mNumberOfBones = 0;
		delete [] mBoneHierarchy;
		mBoneHierarchy = nullptr;
		delete [] mLocalBonePoses;
		mLocalBonePoses = nullptr;
		delete [] mGlobalBonePoses;
		mGlobalBonePoses = nullptr;
	}

	inline void SkeletonResource::initializeElement(SkeletonResourceId skeletonResourceId)
	{
		// Sanity checks
		assert(0 == mNumberOfBones);
		assert(nullptr == mBoneHierarchy);
		assert(nullptr == mLocalBonePoses);
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
