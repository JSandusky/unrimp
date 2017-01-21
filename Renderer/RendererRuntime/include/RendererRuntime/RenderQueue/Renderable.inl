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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline Renderable::~Renderable()
	{
		unsetMaterialResourceIdInternal();
	}

	inline uint64_t Renderable::getSortingKey() const
	{
		return mSortingKey;
	}

	inline RenderableManager& Renderable::getRenderableManager() const
	{
		return mRenderableManager;
	}

	inline Renderer::IVertexArrayPtr Renderable::getVertexArrayPtr() const
	{
		return mVertexArrayPtr;
	}

	inline void Renderable::setVertexArrayPtr(const Renderer::IVertexArrayPtr& vertexArrayPtr)
	{
		mVertexArrayPtr = vertexArrayPtr;
		calculateSortingKey();
	}

	inline Renderer::PrimitiveTopology Renderable::getPrimitiveTopology() const
	{
		return mPrimitiveTopology;
	}

	inline void Renderable::setPrimitiveTopology(Renderer::PrimitiveTopology primitiveTopology)
	{
		mPrimitiveTopology = primitiveTopology;
	}

	inline bool Renderable::getDrawIndexed() const
	{
		return mDrawIndexed;
	}

	inline void Renderable::setDrawIndexed(bool drawIndexed)
	{
		mDrawIndexed = drawIndexed;
	}

	inline uint32_t Renderable::getStartIndexLocation() const
	{
		return mStartIndexLocation;
	}

	inline void Renderable::setStartIndexLocation(uint32_t startIndexLocation)
	{
		mStartIndexLocation = startIndexLocation;
	}

	inline uint32_t Renderable::getNumberOfIndices() const
	{
		return mNumberOfIndices;
	}

	inline void Renderable::setNumberOfIndices(uint32_t numberOfIndices)
	{
		mNumberOfIndices = numberOfIndices;
	}

	inline MaterialResourceId Renderable::getMaterialResourceId() const
	{
		return mMaterialResourceId;
	}

	inline void Renderable::unsetMaterialResourceId()
	{
		unsetMaterialResourceIdInternal();
		calculateSortingKey();
	}

	inline uint8_t Renderable::getRenderQueueIndex() const
	{
		return mRenderQueueIndex;
	}

	inline bool Renderable::getCastShadows() const
	{
		return mCastShadows;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
