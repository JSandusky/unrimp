/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include <Renderer/Buffer/BufferTypes.h>
#include <Renderer/Buffer/IIndexBuffer.h>
#include <Renderer/Buffer/IndexBufferTypes.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3D10Buffer;
namespace Direct3D10Renderer
{
	class Direct3D10Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 10 index buffer object (IBO, "element array buffer" in OpenGL terminology) class
	*/
	class IndexBuffer : public Renderer::IIndexBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D10Renderer
		*    Owner Direct3D 10 renderer instance
		*  @param[in] numberOfBytes
		*    Number of bytes within the index buffer, must be valid
		*  @param[in] indexBufferFormat
		*    Index buffer data format
		*  @param[in] data
		*    Index buffer data, can be a null pointer (empty buffer)
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*/
		IndexBuffer(Direct3D10Renderer& direct3D10Renderer, uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data = nullptr, Renderer::BufferUsage bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~IndexBuffer() override;

		/**
		*  @brief
		*    Return the Direct3D index buffer instance
		*
		*  @return
		*    The Direct3D index buffer instance, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D10Buffer* getD3D10Buffer() const;

		/**
		*  @brief
		*    Return the DXGI index buffer data format
		*
		*  @return
		*    The DXGI index buffer data format (type "DXGI_FORMAT" not used in here in order to keep the header slim)
		*/
		inline uint32_t getDXGIFormat() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char* name) override;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit IndexBuffer(const IndexBuffer& source) = delete;
		IndexBuffer& operator =(const IndexBuffer& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ID3D10Buffer* mD3D10Buffer;	///< Direct3D index buffer instance, can be a null pointer
		uint32_t	  mDXGIFormat;	///< DXGI index buffer data format (type "DXGI_FORMAT" not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D10Renderer/Buffer/IndexBuffer.inl"
