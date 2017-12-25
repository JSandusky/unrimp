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
	inline SubMesh::SubMesh() :
		mMaterialResourceId(getUninitialized<MaterialResourceId>()),
		mStartIndexLocation(0),
		mNumberOfIndices(0)
	{
		// Nothing here
	}

	inline SubMesh::SubMesh(MaterialResourceId materialResourceId, uint32_t startIndexLocation, uint32_t numberOfIndices) :
		mMaterialResourceId(materialResourceId),
		mStartIndexLocation(startIndexLocation),
		mNumberOfIndices(numberOfIndices)
	{
		// Nothing here
	}

	inline SubMesh::~SubMesh()
	{
		// Nothing here
	}

	inline SubMesh::SubMesh(const SubMesh& subMesh) :
		mMaterialResourceId(subMesh.mMaterialResourceId),
		mStartIndexLocation(subMesh.mStartIndexLocation),
		mNumberOfIndices(subMesh.mNumberOfIndices)
	{
		// Nothing here
	}

	inline SubMesh& SubMesh::operator=(const SubMesh& subMesh)
	{
		mMaterialResourceId	= subMesh.mMaterialResourceId;
		mStartIndexLocation = subMesh.mStartIndexLocation;
		mNumberOfIndices	= subMesh.mNumberOfIndices;

		// Done
		return *this;
	}

	inline MaterialResourceId SubMesh::getMaterialResourceId() const
	{
		return mMaterialResourceId;
	}

	inline void SubMesh::setMaterialResourceId(MaterialResourceId materialResourceId)
	{
		mMaterialResourceId = materialResourceId;
	}

	inline uint32_t SubMesh::getStartIndexLocation() const
	{
		return mStartIndexLocation;
	}

	inline void SubMesh::setStartIndexLocation(uint32_t startIndexLocation)
	{
		mStartIndexLocation = startIndexLocation;
	}

	inline uint32_t SubMesh::getNumberOfIndices() const
	{
		return mNumberOfIndices;
	}

	inline void SubMesh::setNumberOfIndices(uint32_t numberOfIndices)
	{
		mNumberOfIndices = numberOfIndices;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
