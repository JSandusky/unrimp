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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Export.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class RenderableManager;
	class MaterialResourceManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t MaterialResourceId;	///< POD material resource identifier
	typedef uint32_t SkeletonResourceId;	///< POD skeleton resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Renderable
	*
	*  @note
	*    - Example: Abstract representation of a sub-mesh which is part of an mesh scene item
	*/
	class Renderable
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MaterialResource;	// Must be able to update cached material data


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT Renderable();
		RENDERERRUNTIME_API_EXPORT Renderable(RenderableManager& renderableManager, const Renderer::IVertexArrayPtr& vertexArrayPtr, bool drawIndexed, uint32_t startIndexLocation, uint32_t numberOfIndices, const MaterialResourceManager& materialResourceManager, MaterialResourceId materialResourceId, SkeletonResourceId skeletonResourceId, uint32_t instanceCount = 1);
		inline ~Renderable();

		//[-------------------------------------------------------]
		//[ Derived data                                          ]
		//[-------------------------------------------------------]
		inline uint64_t getSortingKey() const;

		//[-------------------------------------------------------]
		//[ Data                                                  ]
		//[-------------------------------------------------------]
		inline RenderableManager& getRenderableManager() const;
		inline Renderer::IVertexArrayPtr getVertexArrayPtr() const;
		inline void setVertexArrayPtr(const Renderer::IVertexArrayPtr& vertexArrayPtr);
		inline bool getDrawIndexed() const;
		inline void setDrawIndexed(bool drawIndexed);
		inline uint32_t getStartIndexLocation() const;
		inline void setStartIndexLocation(uint32_t startIndexLocation);
		inline uint32_t getNumberOfIndices() const;
		inline void setNumberOfIndices(uint32_t numberOfIndices);
		inline MaterialResourceId getMaterialResourceId() const;
		RENDERERRUNTIME_API_EXPORT void setMaterialResourceId(const MaterialResourceManager& materialResourceManager, MaterialResourceId materialResourceId);
		inline void unsetMaterialResourceId();
		inline SkeletonResourceId getSkeletonResourceId() const;
		inline void setSkeletonResourceId(SkeletonResourceId skeletonResourceId);
		inline uint32_t getInstanceCount() const;
		inline void setInstanceCount(uint32_t instanceCount);

		//[-------------------------------------------------------]
		//[ Cached material data                                  ]
		//[-------------------------------------------------------]
		inline uint8_t getRenderQueueIndex() const;
		inline bool getCastShadows() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		Renderable& operator=(const Renderable&) = delete;
		RENDERERRUNTIME_API_EXPORT void calculateSortingKey();
		RENDERERRUNTIME_API_EXPORT void unsetMaterialResourceIdInternal();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Derived data
		uint64_t						mSortingKey;			///< The sorting key is directly calculated after data change, no lazy evaluation since it's changed rarely but requested often (no branching)
		// Data
		RenderableManager&				mRenderableManager;
		Renderer::IVertexArrayPtr		mVertexArrayPtr;		///< Vertex array object (VAO), can be a null pointer
		uint32_t						mStartIndexLocation;
		uint32_t						mNumberOfIndices;
		MaterialResourceId				mMaterialResourceId;
		SkeletonResourceId				mSkeletonResourceId;
		uint32_t						mInstanceCount;
		bool							mDrawIndexed;			///< Placed at this location due to padding
		// Cached material data
		uint8_t							mRenderQueueIndex;
		bool							mCastShadows;
		// Internal data
		const MaterialResourceManager*	mMaterialResourceManager;
		int								mMaterialResourceAttachmentIndex;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/RenderQueue/Renderable.inl"
