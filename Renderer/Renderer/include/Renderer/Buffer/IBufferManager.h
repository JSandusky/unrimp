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
#include "Renderer/SmartRefCount.h"
#include "Renderer/Buffer/BufferTypes.h"
#include "Renderer/Buffer/IndexBufferTypes.h"
#include "Renderer/Texture/TextureTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderer;
	class IVertexArray;
	class IIndexBuffer;
	class IVertexBuffer;
	class IUniformBuffer;
	class ITextureBuffer;
	class IIndirectBuffer;
	struct VertexAttributes;
	struct VertexArrayVertexBuffer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract buffer manager interface
	*
	*  @remarks
	*    The buffer manager is responsible for managing fine granular instances of
	*    - Vertex buffer object ("Renderer::IVertexBuffer")
	*    - Index buffer object ("Renderer::IIndexBuffer")
	*    - Vertex array object ("Renderer::IVertexArray")
	*    - Uniform buffer object ("Renderer::IUniformBuffer")
	*    - Texture buffer object ("Renderer::ITextureBuffer")
	*    - Indirect buffer object ("Renderer::IIndirectBuffer")
	*
	*    Implementations are free to implement a naive 1:1 mapping of a resource to an renderer API resource.
	*    For AZDO ("Almost Zero Driver Overhead") implementations might allocate a few big renderer API resources
	*    and manage the granular instances internally. Modern renderer APIs like Vulkan or DirectX 12 are build
	*    around the concept that the user just allocates a few big memory heaps and then managing them by their own.
	*/
	class IBufferManager : public RefCount<IBufferManager>
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IBufferManager();

		/**
		*  @brief
		*    Return the owner renderer instance
		*
		*  @return
		*    The owner renderer instance, do not release the returned instance unless you added an own reference to it
		*/
		inline IRenderer& getRenderer() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IBufferManager methods       ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Resource creation                                     ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Create a vertex buffer object (VBO, "array buffer" in OpenGL terminology) instance
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the vertex buffer, must be valid
		*  @param[in] data
		*    Vertex buffer data, can be a null pointer (empty buffer), the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*
		*  @return
		*    The created VBO instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual IVertexBuffer* createVertexBuffer(uint32_t numberOfBytes, const void* data = nullptr, BufferUsage bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;

		/**
		*  @brief
		*    Create an index buffer object (IBO, "element array buffer" in OpenGL terminology) instance
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the index buffer, must be valid
		*  @param[in] indexBufferFormat
		*    Index buffer data format
		*  @param[in] data
		*    Index buffer data, can be a null pointer (empty buffer), the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*
		*  @return
		*    The created IBO instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual IIndexBuffer* createIndexBuffer(uint32_t numberOfBytes, IndexBufferFormat::Enum indexBufferFormat, const void* data = nullptr, BufferUsage bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;

		/**
		*  @brief
		*    Create a vertex array instance
		*
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] numberOfVertexBuffers
		*    Number of vertex buffers, having zero vertex buffers is valid
		*  @param[in] vertexBuffers
		*    At least numberOfVertexBuffers instances of vertex array vertex buffers, can be a null pointer in case there are zero vertex buffers, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] indexBuffer
		*    Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		*
		*  @return
		*    The created vertex array instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The created vertex array instance keeps a reference to the vertex buffers used by the vertex array attributes
		*    - It's valid that a vertex array implementation is adding a reference and releasing it again at once
		*      (this means that in the case of not having any more references, a vertex buffer might get destroyed when calling this method)
		*/
		virtual IVertexArray* createVertexArray(const VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const VertexArrayVertexBuffer* vertexBuffers, IIndexBuffer* indexBuffer = nullptr) = 0;

		/**
		*  @brief
		*    Create an uniform buffer object (UBO, "constant buffer" in Direct3D terminology) instance
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the uniform buffer, must be valid
		*  @param[in] data
		*    Uniform buffer data, can be a null pointer (empty buffer), the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*
		*  @return
		*    The created UBO instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumUniformBufferSize" is >0
		*/
		virtual IUniformBuffer* createUniformBuffer(uint32_t numberOfBytes, const void* data = nullptr, Renderer::BufferUsage bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) = 0;

		/**
		*  @brief
		*    Create an texture buffer object (TBO) instance
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the texture buffer, must be valid
		*  @param[in] textureFormat
		*    Texture buffer data format
		*  @param[in] data
		*    Texture buffer data, can be a null pointer (empty buffer), the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*
		*  @return
		*    The created TBO instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumTextureBufferSize" is not 0
		*/
		virtual ITextureBuffer* createTextureBuffer(uint32_t numberOfBytes, TextureFormat::Enum textureFormat, const void* data = nullptr, BufferUsage bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;

		/**
		*  @brief
		*    Create an indirect buffer object instance
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the indirect buffer, must be valid
		*  @param[in] data
		*    Indirect buffer data, can be a null pointer (empty buffer), the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*
		*  @return
		*    The created UBO instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumIndirectBufferSize" is >0
		*/
		virtual IIndirectBuffer* createIndirectBuffer(uint32_t numberOfBytes, const void* data = nullptr, Renderer::BufferUsage bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline explicit IBufferManager(IRenderer& renderer);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit IBufferManager(const IBufferManager& source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline IBufferManager& operator =(const IBufferManager& source);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRenderer& mRenderer;	///< The owner renderer instance


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<IBufferManager> IBufferManagerPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Buffer/IBufferManager.inl"
