/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include <Renderer/IVertexArray.h>


//[-------------------------------------------------------]
//[ Forward declaration                                   ]
//[-------------------------------------------------------]
// TODO(co) Direct3D 12 update
typedef __interface ID3D10Blob *LPD3D10BLOB;	// "__interface" is no keyword of the ISO C++ standard, shouldn't be a problem because this in here is MS Windows only and it's also within the Direct3D headers we have to use
typedef ID3D10Blob ID3DBlob;
/*
struct ID3D12Buffer;
struct ID3D12InputLayout;
struct ID3D12DeviceContext;
*/
namespace Renderer
{
	struct VertexArrayAttribute;
}
namespace Direct3D12Renderer
{
	class IndexBuffer;
	class VertexBuffer;
	class Direct3D12Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 12 vertex array class
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
		*  @param[in] direct3D12Renderer
		*    Owner Direct3D 12 renderer instance
		*  @param[in] d3dBlob
		*    The loaded and compiled vertex shader
		*  @param[in] numberOfAttributes
		*    Number of attributes (position, color, texture coordinate, normal...), having zero attributes is valid
		*  @param[in] attributes
		*    At least nNumberOfAttributes instances of vertex array attributes, can be a null pointer in case there are zero attributes
		*  @param[in] indexBuffer
		*    Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		*/
		VertexArray(Direct3D12Renderer &direct3D12Renderer, ID3DBlob &d3dBlob, uint32_t numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexArray();

		/**
		*  @brief
		*    Return the Direct3D 12 input layout
		*
		*  @return
		*    Direct3D 12 input layout instance, can be a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		// TODO(co) Direct3D 12 update
		// inline ID3D12InputLayout *getD3D12InputLayout() const;

		/**
		*  @brief
		*    Set the Direct3D 12 vertex declaration and stream source
		*/
		void setDirect3DIASetInputLayoutAndStreamSource() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// TODO(co) Direct3D 12 update
		//ID3D12DeviceContext	 *mD3D12DeviceContext;		///< The Direct3D 12 device context instance (we keep a reference to it), null pointer on horrible error (so we don't check)
		IndexBuffer			 *mIndexBuffer;				///< Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		// TODO(co) Direct3D 12 update
		//ID3D12InputLayout	 *mD3D12InputLayout;		///< Direct3D 12 input layout, can be a null pointer
		// Direct3D 12 input slots
		uint32_t			  mNumberOfSlots;			///< Number of used Direct3D 12 input slots (type "UINT" not used in here in order to keep the header slim)
		// TODO(co) Direct3D 12 update
		//ID3D12Buffer		**mD3D12Buffers;			///< Direct3D 12 vertex buffers, if "mD3D12InputLayout" is no null pointer this is no null pointer as well
		uint32_t			 *mStrides;					///< Strides in bytes, if "mD3D12Buffers" is no null pointer this is no null pointer as well (type "UINT" not used in here in order to keep the header slim)
		uint32_t			 *mOffsets;					///< Offsets in bytes, if "mD3D12Buffers" is no null pointer this is no null pointer as well (type "UINT" not used in here in order to keep the header slim)
		// For proper vertex buffer reference counter behaviour
		uint32_t			  mNumberOfVertexBuffers;	///< Number of vertex buffers
		VertexBuffer		**mVertexBuffers;			///< Vertex buffers (we keep a reference to it) used by this vertex array, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D12Renderer/VertexArray.inl"
