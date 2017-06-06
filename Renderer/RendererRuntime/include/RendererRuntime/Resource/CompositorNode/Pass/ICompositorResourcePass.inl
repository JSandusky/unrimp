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
#include "RendererRuntime/Core/GetUninitialized.h"

#include <tuple>	// For "std::ignore"
#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline const CompositorTarget& ICompositorResourcePass::getCompositorTarget() const
	{
		return mCompositorTarget;
	}

	inline float ICompositorResourcePass::getMinimumDepth() const
	{
		return mMinimumDepth;
	}

	inline float ICompositorResourcePass::getMaximumDepth() const
	{
		return mMaximumDepth;
	}

	inline bool ICompositorResourcePass::getSkipFirstExecution() const
	{
		return mSkipFirstExecution;
	}

	inline uint32_t ICompositorResourcePass::getNumberOfExecutions() const
	{
		return mNumberOfExecutions;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::ICompositorResourcePass methods ]
	//[-------------------------------------------------------]
	inline void ICompositorResourcePass::deserialize(uint32_t numberOfBytes, const uint8_t* data)
	{
		// Keep this in sync with "RendererRuntime::v1CompositorNode::Pass"
		// -> Don't include "RendererRuntime/Resource/CompositorNode/Loader/CompositorNodeFileFormat.h" here to keep the header complexity low (compile times matter)
		#pragma pack(push)
		#pragma pack(1)
			struct PassData
			{
				float	 minimumDepth		= 0.0f;
				float	 maximumDepth		= 1.0f;
				uint32_t numberOfExecutions	= RendererRuntime::getUninitialized<uint32_t>();
				bool	 skipFirstExecution	= false;
			};
		#pragma pack(pop)

		// Sanity check
		assert(sizeof(PassData) == numberOfBytes);
		std::ignore = numberOfBytes;

		// Read data
		const PassData* pass = reinterpret_cast<const PassData*>(data);
		mMinimumDepth		= pass->minimumDepth;
		mMaximumDepth		= pass->maximumDepth;
		mNumberOfExecutions = pass->numberOfExecutions;
		mSkipFirstExecution = pass->skipFirstExecution;

		// Sanity checks
		assert(mNumberOfExecutions > 0);
		assert(!mSkipFirstExecution || mNumberOfExecutions > 1);
	}

	inline bool ICompositorResourcePass::getRenderQueueIndexRange(uint8_t&, uint8_t&) const
	{
		// This compositor resource pass has no render queue range defined
		return false;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline ICompositorResourcePass::ICompositorResourcePass(const CompositorTarget& compositorTarget) :
		mCompositorTarget(compositorTarget),
		mMinimumDepth(0.0f),
		mMaximumDepth(1.0f),
		mSkipFirstExecution(false),
		mNumberOfExecutions(RendererRuntime::getUninitialized<uint32_t>())
	{
		// Nothing here
	}

	inline ICompositorResourcePass::~ICompositorResourcePass()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
