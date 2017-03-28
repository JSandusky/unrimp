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
#include "RendererRuntime/Resource/SkeletonAnimation/Loader/SkeletonAnimationResourceLoader.h"
#include "RendererRuntime/Resource/SkeletonAnimation/Loader/SkeletonAnimationFileFormat.h"
#include "RendererRuntime/Resource/SkeletonAnimation/SkeletonAnimationResource.h"
#include "RendererRuntime/Core/File/IFile.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId SkeletonAnimationResourceLoader::TYPE_ID("skeleton_animation");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void SkeletonAnimationResourceLoader::onDeserialization(IFile& file)
	{
		// Read in the skeleton animation header
		v1SkeletonAnimation::Header skeletonAnimationHeader;
		file.read(&skeletonAnimationHeader, sizeof(v1SkeletonAnimation::Header));
		assert(v1SkeletonAnimation::FORMAT_TYPE == skeletonAnimationHeader.formatType);
		assert(v1SkeletonAnimation::FORMAT_VERSION == skeletonAnimationHeader.formatVersion);
		mSkeletonAnimationResource->mNumberOfChannels = skeletonAnimationHeader.numberOfChannels;
		mSkeletonAnimationResource->mDurationInTicks  = skeletonAnimationHeader.durationInTicks;
		mSkeletonAnimationResource->mTicksPerSecond   = skeletonAnimationHeader.ticksPerSecond;

		// Read in the channel byte offsets
		mSkeletonAnimationResource->mChannelByteOffsets.resize(skeletonAnimationHeader.numberOfChannels);
		file.read(mSkeletonAnimationResource->mChannelByteOffsets.data(), sizeof(uint32_t) * mSkeletonAnimationResource->mChannelByteOffsets.size());

		// Read in the data of all bone channels in one big chunk
		mSkeletonAnimationResource->mChannelData.resize(skeletonAnimationHeader.numberOfChannelDataBytes);
		file.read(mSkeletonAnimationResource->mChannelData.data(), sizeof(uint8_t) * mSkeletonAnimationResource->mChannelData.size());

		// That's all folks. There are no more memory allocations to see here. Please go on.
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
