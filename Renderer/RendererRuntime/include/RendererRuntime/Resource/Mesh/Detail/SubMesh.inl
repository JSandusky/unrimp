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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline SubMesh::SubMesh() :
		mMaterialResourceId(~0u),	// TODO(co) Set material resource ID to "uninitialized"
		mPrimitiveTopology(Renderer::PrimitiveTopology::UNKNOWN),
		mStartIndexLocation(0),
		mNumberOfIndices(0)
	{
		// Nothing here
	}

	inline SubMesh::~SubMesh()
	{
		// Nothing here
	}

	inline SubMesh::SubMesh(const SubMesh& subMesh) :
		mMaterialResourceId(subMesh.mMaterialResourceId),
		mPrimitiveTopology(subMesh.mPrimitiveTopology),
		mStartIndexLocation(subMesh.mStartIndexLocation),
		mNumberOfIndices(subMesh.mNumberOfIndices)
	{
		// Nothing here
	}

	inline SubMesh& SubMesh::operator=(const SubMesh& subMesh)
	{
		mMaterialResourceId	= subMesh.mMaterialResourceId;
		mPrimitiveTopology	= subMesh.mPrimitiveTopology;
		mStartIndexLocation = subMesh.mStartIndexLocation;
		mNumberOfIndices	= subMesh.mNumberOfIndices;

		// Done
		return *this;
	}

	inline MaterialResourceId SubMesh::getMaterialResourceId() const
	{
		return mMaterialResourceId;
	}

	inline Renderer::PrimitiveTopology SubMesh::getPrimitiveTopology() const
	{
		return mPrimitiveTopology;
	}

	inline uint32_t SubMesh::getStartIndexLocation() const
	{
		return mStartIndexLocation;
	}

	inline uint32_t SubMesh::getNumberOfIndices() const
	{
		return mNumberOfIndices;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
