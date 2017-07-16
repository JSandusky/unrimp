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
#include <Renderer/Buffer/IVertexArray.h>


//[-------------------------------------------------------]
//[ Forward declaration                                   ]
//[-------------------------------------------------------]
struct ID3D11Buffer;
struct ID3D11DeviceContext;
namespace Renderer
{
	struct VertexAttributes;
	struct VertexArrayVertexBuffer;
}
namespace Direct3D11Renderer
{
	class IndexBuffer;
	class VertexBuffer;
	class Direct3D11Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 11 vertex array class
	*/
	class VertexArray : public Renderer::IVertexArray
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D11Renderer
		*    Owner Direct3D 11 renderer instance
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] numberOfVertexBuffers
		*    Number of vertex buffers, having zero vertex buffers is valid
		*  @param[in] vertexBuffers
		*    At least numberOfVertexBuffers instances of vertex array vertex buffers, can be a null pointer in case there are zero vertex buffers, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] indexBuffer
		*    Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		*/
		VertexArray(Direct3D11Renderer& direct3D11Renderer, const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, IndexBuffer* indexBuffer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexArray();

		/**
		*  @brief
		*    Set the Direct3D 11 vertex declaration and stream source
		*/
		void setDirect3DIASetInputLayoutAndStreamSource() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit VertexArray(const VertexArray& source) = delete;
		VertexArray& operator =(const VertexArray& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ID3D11DeviceContext*  mD3D11DeviceContext;	///< The Direct3D 11 device context instance (we keep a reference to it), null pointer on horrible error (so we don't check)
		IndexBuffer*		  mIndexBuffer;			///< Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		// Direct3D 11 input slots
		uint32_t			  mNumberOfSlots;		///< Number of used Direct3D 11 input slots (type "UINT" not used in here in order to keep the header slim)
		ID3D11Buffer**		  mD3D11Buffers;		///< Direct3D 11 vertex buffers, if "mD3D11InputLayout" is no null pointer this is no null pointer as well
		uint32_t*			  mStrides;				///< Strides in bytes, if "mD3D11Buffers" is no null pointer this is no null pointer as well (type "UINT" not used in here in order to keep the header slim)
		uint32_t*			  mOffsets;				///< Offsets in bytes, if "mD3D11Buffers" is no null pointer this is no null pointer as well (type "UINT" not used in here in order to keep the header slim)
		// For proper vertex buffer reference counter behaviour
		VertexBuffer**		  mVertexBuffers;		///< Vertex buffers (we keep a reference to it) used by this vertex array, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
