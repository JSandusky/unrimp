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
#include "RendererRuntime/RenderQueue/IndirectBufferManager.h"
#include "RendererRuntime/Core/SwizzleVectorElementRemove.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndirectBufferManager::IndirectBufferManager(const IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mBufferManager(rendererRuntime.getBufferManager())
	{
		// Nothing here
	}

	IndirectBufferManager::~IndirectBufferManager()
	{
		// At this point in time, no indirect buffers should be in use anymore
		assert(mUsedIndirectBuffers.empty());

		// Destroy all indirect buffers
		for (IndirectBuffer& indirectBuffer : mFreeIndirectBuffers)
		{
			indirectBuffer.indirectBuffer->releaseReference();
		}
		for (IndirectBuffer& indirectBuffer : mUsedIndirectBuffers)
		{
			indirectBuffer.indirectBuffer->releaseReference();
		}
	}

	Renderer::IIndirectBuffer* IndirectBufferManager::getIndirectBuffer(uint32_t maximumNumberOfDrawCalls)
	{
		const uint32_t maximumNumberOfBytes = sizeof(Renderer::DrawIndexedInstancedArguments) * maximumNumberOfDrawCalls;

		// Find the smallest free indirect buffer which still fulfills the requirement
		IndirectBuffers::iterator iterator					  = mFreeIndirectBuffers.begin();
		IndirectBuffers::iterator iteratorEnd				  = mFreeIndirectBuffers.end();
		uint32_t				  smallestBufferNumberOfBytes = std::numeric_limits<uint32_t>::max();
		IndirectBuffers::iterator smallestBuffer			  = iteratorEnd;
		while (iterator != iteratorEnd)
		{
			const uint32_t bufferNumberOfBytes = iterator->numberOfBytes;
			if (maximumNumberOfBytes <= bufferNumberOfBytes && smallestBufferNumberOfBytes > bufferNumberOfBytes)
			{
				// New best hit found
				smallestBuffer				= iterator;
				smallestBufferNumberOfBytes	= bufferNumberOfBytes;
			}
			++iterator;
		}

		// Create new indirect buffer, if required
		Renderer::IIndirectBuffer* indirectBuffer = nullptr;
		if (smallestBuffer == iteratorEnd)
		{
			indirectBuffer = mBufferManager.createIndirectBuffer(maximumNumberOfBytes, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
			mUsedIndirectBuffers.emplace_back(indirectBuffer, maximumNumberOfBytes);
			RENDERER_SET_RESOURCE_DEBUG_NAME(indirectBuffer, "Indirect buffer manager")
		}
		else
		{
			indirectBuffer = smallestBuffer->indirectBuffer;
			mUsedIndirectBuffers.push_back(*smallestBuffer);
			::detail::swizzleVectorElementRemove(mFreeIndirectBuffers, smallestBuffer);
		}

		// Done
		return indirectBuffer;
	}

	void IndirectBufferManager::onPostCommandBufferExecution()
	{
		// Free all used indirect buffers
		mFreeIndirectBuffers.insert(mFreeIndirectBuffers.end(), mUsedIndirectBuffers.begin(), mUsedIndirectBuffers.end());
		mUsedIndirectBuffers.clear();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
