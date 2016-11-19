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
namespace Renderer
{



	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IndirectBuffer::IndirectBuffer(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation) :
		IIndirectBuffer(*static_cast<IRenderer*>(nullptr)),	// Evil but does the trick in this special use case
		mDrawInstancedArguments(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation)
	{
		// Nothing here
	}

	inline IndirectBuffer::~IndirectBuffer()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual IIndirectBuffer methods                ]
	//[-------------------------------------------------------]
	inline const uint8_t* IndirectBuffer::getEmulationData() const
	{
		return reinterpret_cast<const uint8_t*>(&mDrawInstancedArguments);
	}

	inline void IndirectBuffer::copyDataFrom(uint32_t, const void*)
	{
		// Not required for this implementation
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
