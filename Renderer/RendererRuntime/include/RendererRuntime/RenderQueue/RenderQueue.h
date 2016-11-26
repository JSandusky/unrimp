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
#include "RendererRuntime/Core/NonCopyable.h"

#include <vector>

#include <inttypes.h>	// For uint32_t, uint64_t etc.


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class Renderable;
	class IRendererRuntime;
	class RenderableManager;
	class IndirectBufferManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


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
	*/
	class RenderQueue : protected NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		explicit RenderQueue(const IRendererRuntime& rendererRuntime);
		~RenderQueue();
		inline void clear();
		void addRenderablesFromRenderableManager(uint32_t threadIndex, const RenderableManager& renderableManager);
		void draw();


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
			const Renderable*		 renderable;		///< Always valid, don't destroy the instance
			const RenderableManager* renderableManager;	///< Always valid, don't destroy the instance
			uint64_t				 sortingKey;		///< Key used for sorting

			inline QueuedRenderable() :
				renderable(nullptr),
				renderableManager(nullptr),
				sortingKey(0)
			{};
			inline QueuedRenderable(const Renderable& _renderable, const RenderableManager& _renderableManager, uint64_t _sortingKey) :
				renderable(&_renderable),
				renderableManager(&_renderableManager),
				sortingKey(_sortingKey)
			{};
			inline bool operator < (const QueuedRenderable& queuedRenderable) const
			{
				return (sortingKey < queuedRenderable.sortingKey);
			}
		};
		typedef std::vector<QueuedRenderable> QueuedRenderables;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		const IRendererRuntime&	mRendererRuntime;
		IndirectBufferManager&  mIndirectBufferManager;
		QueuedRenderables		mQueuedRenderables;
		bool					mDoSort;
		bool					mSorted;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/RenderQueue/RenderQueue.inl"
