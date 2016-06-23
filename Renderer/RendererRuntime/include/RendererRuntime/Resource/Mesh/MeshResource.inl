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
	inline uint32_t MeshResource::getNumberOfVertices() const
	{
		return mNumberOfVertices;
	}

	inline void MeshResource::setNumberOfVertices(uint32_t numberOfVertices)
	{
		mNumberOfVertices = numberOfVertices;
	}

	inline uint32_t MeshResource::getNumberOfIndices() const
	{
		return mNumberOfIndices;
	}

	inline void MeshResource::setNumberOfIndices(uint32_t numberOfIndices)
	{
		mNumberOfIndices = numberOfIndices;
	}

	inline Renderer::IVertexArrayPtr MeshResource::getVertexArrayPtr() const
	{
		return mVertexArray;
	}

	inline void MeshResource::setVertexArray(Renderer::IVertexArray* vertexArray)
	{
		mVertexArray = vertexArray;
	}

	inline const SubMeshes& MeshResource::getSubMeshes() const
	{
		return mSubMeshes;
	}

	inline SubMeshes& MeshResource::getSubMeshes()
	{
		return mSubMeshes;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline MeshResource::MeshResource() :
		IResource(getUninitialized<MeshResourceId>()),
		mNumberOfVertices(0),
		mNumberOfIndices(0)
	{
		// Nothing here
	}

	inline MeshResource::MeshResource(MeshResourceId meshResourceId) :
		IResource(meshResourceId),
		mNumberOfVertices(0),
		mNumberOfIndices(0)
	{
		// Nothing here
	}

	inline MeshResource::~MeshResource()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
