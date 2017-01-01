/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "RendererRuntime/Core/StringId.h"
#include "RendererRuntime/Core/NonCopyable.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderTarget;
	class CommandBuffer;
}
namespace RendererRuntime
{
	class Renderable;
	class IRendererRuntime;
	class RenderableManager;
	class CompositorContextData;
	class IndirectBufferManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId MaterialTechniqueId;	///< Material technique identifier, internally just a POD "uint32_t", result of hashing the material technique name


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Render queue
	*
	*  @remarks
	*    Using layered rendering (aka bucketized rendering) concept described in
	*    - "realtimecollisiondetection.net – the blog" - "Order your graphics draw calls around!" - http://realtimecollisiondetection.net/blog/?p=86
	*    - "Molecular Musings" - "Stateless, layered, multi-threaded rendering – Part 1" - https://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/
	*
	*    The sole purpose of the render queue is to fill sorted commands into a given command buffer.
	*/
	class RenderQueue : protected NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] rendererRuntime
		*    Indirect buffer manager to use, must stay valid as long as the indirect buffer manager instance exists
		*  @param[in] minimumRenderQueueIndex
		*    Minimum render queue index (inclusive)
		*  @param[in] maximumRenderQueueIndex
		*    Maximum render queue index (inclusive)
		*  @param[in] transparentPass
		*    "true" if this render queue is used for a transparent render pass, else "false" for opaque render pass (influences the renderables sorting)
		*  @param[in] doSort
		*    Sort renderables?
		*/
		RenderQueue(IndirectBufferManager& indirectBufferManager, uint8_t minimumRenderQueueIndex, uint8_t maximumRenderQueueIndex, bool transparentPass, bool doSort);

		inline ~RenderQueue();
		inline uint8_t getMinimumRenderQueueIndex() const;
		inline uint8_t getMaximumRenderQueueIndex() const;
		void clear();
		void addRenderablesFromRenderableManager(const RenderableManager& renderableManager, bool castShadows = false);
		void fillCommandBuffer(const Renderer::IRenderTarget& renderTarget, MaterialTechniqueId materialTechniqueId, const CompositorContextData& compositorContextData, Renderer::CommandBuffer& commandBuffer);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		RenderQueue(const RenderQueue&) = delete;
		RenderQueue& operator=(const RenderQueue&) = delete;


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct QueuedRenderable
		{
			const Renderable* renderable;	///< Always valid, don't destroy the instance
			uint64_t		  sortingKey;	///< Key used for sorting

			inline QueuedRenderable() :
				renderable(nullptr),
				sortingKey(0)
			{}
			inline QueuedRenderable(const Renderable& _renderable, uint64_t _sortingKey) :
				renderable(&_renderable),
				sortingKey(_sortingKey)
			{}
			inline bool operator < (const QueuedRenderable& queuedRenderable) const
			{
				return (sortingKey < queuedRenderable.sortingKey);
			}
		};
		typedef std::vector<QueuedRenderable> QueuedRenderables;

		struct Queue
		{
			QueuedRenderables queuedRenderables;
			bool			  sorted;

			Queue() :
				sorted(false)
			{}
		};
		typedef std::vector<Queue> Queues;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		const IRendererRuntime&	mRendererRuntime;			///< Renderer runtime instance, we don't own the instance so don't delete it
		IndirectBufferManager&	mIndirectBufferManager;		///< Indirect buffer manager instance, we don't own the instance so don't delete it
		Queues					mQueues;
		uint8_t					mMinimumRenderQueueIndex;	///< Inclusive
		uint8_t					mMaximumRenderQueueIndex;	///< Inclusive
		bool					mTransparentPass;
		bool					mDoSort;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/RenderQueue/RenderQueue.inl"
