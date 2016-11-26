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
#include "RendererRuntime/Export.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t MaterialResourceId;	///< POD material resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Renderable
	*/
	class Renderable
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline Renderable();
		inline Renderable(const Renderer::IVertexArrayPtr& vertexArrayPtr, Renderer::PrimitiveTopology primitiveTopology, uint32_t startIndexLocation, uint32_t numberOfIndices, MaterialResourceId materialResourceId);
		inline ~Renderable();
		inline uint64_t getSortingKey() const;
		inline Renderer::IVertexArrayPtr getVertexArrayPtr() const;
		inline void setVertexArrayPtr(const Renderer::IVertexArrayPtr& vertexArrayPtr);
		inline Renderer::PrimitiveTopology getPrimitiveTopology() const;
		inline Renderer::PrimitiveTopology setPrimitiveTopology(Renderer::PrimitiveTopology primitiveTopology);
		inline uint32_t getStartIndexLocation() const;
		inline void setStartIndexLocation(uint32_t startIndexLocation);
		inline uint32_t getNumberOfIndices() const;
		inline void setNumberOfIndices(uint32_t numberOfIndices);
		inline MaterialResourceId getMaterialResourceId() const;
		inline void setMaterialResourceId(MaterialResourceId materialResourceId);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		RENDERERRUNTIME_API_EXPORT void calculateKey();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Derived data
		uint64_t					mSortingKey;			///< The sorting key is directly calculated after data change, no lazy evaluation since it's changed rarely but requested often (no branching)
		// Data
		Renderer::IVertexArrayPtr	mVertexArrayPtr;		///< Vertex array object (VAO), can be a null pointer
		Renderer::PrimitiveTopology	mPrimitiveTopology;
		uint32_t					mStartIndexLocation;
		uint32_t					mNumberOfIndices;
		MaterialResourceId			mMaterialResourceId;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/RenderQueue/Renderable.inl"
