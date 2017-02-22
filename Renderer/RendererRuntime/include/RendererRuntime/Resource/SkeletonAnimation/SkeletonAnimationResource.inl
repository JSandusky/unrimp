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
	inline uint8_t SkeletonAnimationResource::getNumberOfChannels() const
	{
		return mNumberOfChannels;
	}

	inline float SkeletonAnimationResource::getDurationInTicks() const
	{
		return mDurationInTicks;
	}

	inline float SkeletonAnimationResource::getTicksPerSecond() const
	{
		return mTicksPerSecond;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline SkeletonAnimationResource::SkeletonAnimationResource() :
		mNumberOfChannels(0),
		mDurationInTicks(0.0f),
		mTicksPerSecond(0.0f)
	{
		// Nothing here
	}

	inline SkeletonAnimationResource::~SkeletonAnimationResource()
	{
		// Sanity checks
		assert(0 == mNumberOfChannels);
		assert(0.0f == mDurationInTicks);
		assert(0.0f == mTicksPerSecond);
	}

	inline void SkeletonAnimationResource::clearSkeletonAnimationData()
	{
		mNumberOfChannels = 0;
		mDurationInTicks  = 0.0f;
		mTicksPerSecond   = 0.0f;
	}

	inline void SkeletonAnimationResource::initializeElement(SkeletonAnimationResourceId skeletonAnimationResourceId)
	{
		// Sanity checks
		assert(0 == mNumberOfChannels);
		assert(0.0f == mDurationInTicks);
		assert(0.0f == mTicksPerSecond);

		// Call base implementation
		IResource::initializeElement(skeletonAnimationResourceId);
	}

	inline void SkeletonAnimationResource::deinitializeElement()
	{
		// Reset everything
		clearSkeletonAnimationData();

		// Call base implementation
		IResource::deinitializeElement();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
