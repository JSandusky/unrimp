/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline RenderableManager::~RenderableManager()
	{
		// Nothing here
	}

	inline const RenderableManager::Renderables& RenderableManager::getRenderables() const
	{
		return mRenderables;
	}

	inline RenderableManager::Renderables& RenderableManager::getRenderables()
	{
		return mRenderables;
	}

	inline const Transform& RenderableManager::getTransform() const
	{
		// We know that this pointer is always valid
		assert(nullptr != mTransform);
		return *mTransform;
	}

	inline bool RenderableManager::isVisible() const
	{
		return mVisible;
	}

	inline void RenderableManager::setVisible(bool visible)
	{
		mVisible = visible;
	}

	inline float RenderableManager::getCachedDistanceToCamera() const
	{
		return mCachedDistanceToCamera;
	}

	inline void RenderableManager::setCachedDistanceToCamera(float distanceToCamera)
	{
		mCachedDistanceToCamera = distanceToCamera;
	}

	inline uint8_t RenderableManager::getMinimumRenderQueueIndex() const
	{
		return mMinimumRenderQueueIndex;
	}

	inline uint8_t RenderableManager::getMaximumRenderQueueIndex() const
	{
		return mMaximumRenderQueueIndex;
	}

	inline bool RenderableManager::getCastShadows() const
	{
		return mCastShadows;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
