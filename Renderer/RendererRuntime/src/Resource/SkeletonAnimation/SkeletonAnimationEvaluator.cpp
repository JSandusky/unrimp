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
#include "RendererRuntime/Resource/SkeletonAnimation/SkeletonAnimationEvaluator.h"
#include "RendererRuntime/Resource/SkeletonAnimation/SkeletonAnimationResourceManager.h"
#include "RendererRuntime/Core/Math/Math.h"

#include <glm/gtc/matrix_transform.hpp>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void SkeletonAnimationEvaluator::evaluate(float timeInSeconds)
	{
		const SkeletonAnimationResource& skeletonAnimationResource = static_cast<const SkeletonAnimationResource&>(mSkeletonAnimationResourceManager.getResourceByResourceId(mSkeletonAnimationResourceId));
		const uint8_t numberOfChannels = skeletonAnimationResource.getNumberOfChannels();
		const float durationInTicks = skeletonAnimationResource.getDurationInTicks();
		const SkeletonAnimationResource::ChannelByteOffsets& channelByteOffsets = skeletonAnimationResource.getChannelByteOffsets();
		const SkeletonAnimationResource::ChannelData& channelData = skeletonAnimationResource.getChannelData();
		if (mTransformMatrices.empty())
		{
			// Allocate memory
			mBoneIds.resize(numberOfChannels);
			mTransformMatrices.resize(numberOfChannels);
			mLastPositions.resize(numberOfChannels, std::make_tuple(0, 0, 0));

			// Backup bone IDs
			for (uint8_t i = 0; i < numberOfChannels; ++i)
			{
				mBoneIds[i] = reinterpret_cast<const SkeletonAnimationResource::ChannelHeader&>(*(channelData.data() + channelByteOffsets[i])).boneId;
			}
		}

		// Extract ticks per second; assume default value if not given
		const float ticksPerSecond = (0.0f != skeletonAnimationResource.getTicksPerSecond()) ? skeletonAnimationResource.getTicksPerSecond() : 25.0f;

		// Every following time calculation happens in ticks
		float timeInTicks = timeInSeconds * ticksPerSecond;

		// Map the time into the duration of the animation
		timeInTicks = (durationInTicks > 0.0f) ? fmod(timeInTicks, durationInTicks) : 0.0f;

		// Calculate the transformations for each animation channel
		for (uint8_t i = 0; i < numberOfChannels; ++i)
		{
			const uint8_t* currentChannelData = channelData.data() + channelByteOffsets[i];

			// Get channel header
			const SkeletonAnimationResource::ChannelHeader& channelHeader = reinterpret_cast<const SkeletonAnimationResource::ChannelHeader&>(*currentChannelData);
			currentChannelData += sizeof(SkeletonAnimationResource::ChannelHeader);

			// Sanity checks
			assert(channelHeader.numberOfPositionKeys > 0);
			assert(channelHeader.numberOfRotationKeys > 0);
			assert(channelHeader.numberOfScaleKeys > 0);

			// Get channel keys
			const SkeletonAnimationResource::Vector3Key* positionKeys = reinterpret_cast<const SkeletonAnimationResource::Vector3Key*>(currentChannelData);
			currentChannelData += sizeof(SkeletonAnimationResource::Vector3Key) * channelHeader.numberOfPositionKeys;
			const SkeletonAnimationResource::QuaternionKey* rotationKeys = reinterpret_cast<const SkeletonAnimationResource::QuaternionKey*>(currentChannelData);
			currentChannelData += sizeof(SkeletonAnimationResource::QuaternionKey) * channelHeader.numberOfRotationKeys;
			const SkeletonAnimationResource::Vector3Key* scaleKeys = reinterpret_cast<const SkeletonAnimationResource::Vector3Key*>(currentChannelData);

			// Position
			glm::vec3 presentPosition;
			{
				// Look for present frame number. Search from last position if time is after the last time, else from beginning
				// Should be much quicker than always looking from start for the average use case.
				uint32_t frame = (timeInTicks >= mLastTimeInTicks) ? std::get<0>(mLastPositions[i]) : 0;
				while (frame < channelHeader.numberOfPositionKeys - 1)
				{
					if (timeInTicks < positionKeys[frame + 1].timeInTicks)
					{
						break;
					}
					++frame;
				}

				// Interpolate between this frame's value and next frame's value
				const uint32_t nextFrame = (frame + 1) % channelHeader.numberOfPositionKeys;
				const SkeletonAnimationResource::Vector3Key& key = positionKeys[frame];
				const SkeletonAnimationResource::Vector3Key& nextKey = positionKeys[nextFrame];
				float timeDifference = nextKey.timeInTicks - key.timeInTicks;
				if (timeDifference < 0.0f)
				{
					timeDifference += durationInTicks;
				}
				if (timeDifference > 0.0f)
				{
					const float factor = float((timeInTicks - key.timeInTicks) / timeDifference);
					presentPosition = key.value + (nextKey.value - key.value) * factor;	// TODO(co) Use "glm::mix()"
				}
				else
				{
					presentPosition = key.value;
				}
				std::get<0>(mLastPositions[i]) = frame;
			}

			// Rotation
			glm::quat presentRotation;
			{
				uint32_t frame = (timeInTicks >= mLastTimeInTicks) ? std::get<1>(mLastPositions[i]) : 0;
				while (frame < channelHeader.numberOfRotationKeys - 1)
				{
					if (timeInTicks < rotationKeys[frame + 1].timeInTicks)
					{
						break;
					}
					++frame;
				}

				// Interpolate between this frame's value and next frame's value
				const uint32_t nextFrame = (frame + 1) % channelHeader.numberOfRotationKeys;
				const SkeletonAnimationResource::QuaternionKey& key = rotationKeys[frame];
				const SkeletonAnimationResource::QuaternionKey& nextKey = rotationKeys[nextFrame];
				float timeDifference = nextKey.timeInTicks - key.timeInTicks;
				if (timeDifference < 0.0f)
				{
					timeDifference += durationInTicks;
				}
				if (timeDifference > 0.0f)
				{
					const float factor = float((timeInTicks - key.timeInTicks) / timeDifference);
					presentRotation = glm::slerp(key.value, nextKey.value, factor);
				}
				else
				{
					presentRotation = key.value;
				}
				std::get<1>(mLastPositions[i]) = frame;
			}

			// Scale
			glm::vec3 presentScale(1.0f, 1.0f, 1.0f);
			{
				uint32_t frame = (timeInTicks >= mLastTimeInTicks) ? std::get<2>(mLastPositions[i]) : 0;
				while (frame < channelHeader.numberOfScaleKeys - 1)
				{
					if (timeInTicks < scaleKeys[frame + 1].timeInTicks)
					{
						break;
					}
					++frame;
				}

				// TODO(co) Interpolation maybe? This time maybe even logarithmic, not linear.
				presentScale = scaleKeys[frame].value;
				std::get<2>(mLastPositions[i]) = frame;
			}

			// Build a transformation matrix from it
			// TODO(co) Review temporary matrix instances on the C-runtime stack
			mTransformMatrices[i] = glm::translate(Math::IDENTITY_MATRIX, presentPosition) * glm::toMat4(presentRotation) * glm::scale(Math::IDENTITY_MATRIX, presentScale);
		}

		mLastTimeInTicks = timeInTicks;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
