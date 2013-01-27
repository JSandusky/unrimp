/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#ifndef __DIRECT3D9RENDERER_VERTEXARRAY_H__
#define __DIRECT3D9RENDERER_VERTEXARRAY_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/IVertexArray.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct IDirect3DDevice9;
struct IDirect3DVertexBuffer9;
struct IDirect3DVertexDeclaration9;
namespace Renderer
{
	struct VertexArrayAttribute;
}
namespace Direct3D9Renderer
{
	class IndexBuffer;
	class VertexBuffer;
	class Direct3D9Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 9 vertex array class
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
		*  @param[in] direct3D9Renderer
		*    Owner Direct3D 9 renderer instance
		*  @param[in] numberOfAttributes
		*    Number of attributes (position, color, texture coordinate, normal...), having zero attributes is valid
		*  @param[in] attributes
		*    At least nNumberOfAttributes instances of vertex array attributes, can be a null pointer in case there are zero attributes
		*  @param[in] indexBuffer
		*    Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		*/
		VertexArray(Direct3D9Renderer &direct3D9Renderer, unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexArray();

		/**
		*  @brief
		*    Return the Direct3D 9 vertex declaration instance
		*
		*  @return
		*    Direct3D 9 vertex declaration instance, can be a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline IDirect3DVertexDeclaration9 *getDirect3DVertexDeclaration9() const;

		/**
		*  @brief
		*    Enable the Direct3D 9 vertex declaration and stream source
		*/
		void enableDirect3DVertexDeclarationAndStreamSource() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IDirect3DDevice9			 *mDirect3DDevice9;				/**< The Direct3D 9 device instance (we keep a reference to it), null pointer on horrible error (so we don't check) */
		IndexBuffer					 *mIndexBuffer;					/**< Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer */
		IDirect3DVertexDeclaration9	 *mDirect3DVertexDeclaration9;	/**< Direct3D 9 vertex declaration instance, can be a null pointer */
		// Direct3D 9 input slots
		unsigned int				  mNumberOfSlots;				/**< Number of used Direct3D 9 input slots */
		IDirect3DVertexBuffer9		**mDirect3DVertexBuffer9;		/**< Direct3D 9 vertex buffers, if "mDirect3DVertexDeclaration9" is no null pointer this is no null pointer as well */
		unsigned int				 *mStrides;						/**< Strides in bytes, if "mDirect3DVertexBuffer9" is no null pointer this is no null pointer as well */
		unsigned int				 *mInstancesPerElement;			/**< Instances per element, if "mDirect3DVertexBuffer9" is no null pointer this is no null pointer as well */
		// For proper vertex buffer reference counter behaviour
		unsigned int				  mNumberOfVertexBuffers;		/**< Number of vertex buffers */
		VertexBuffer				**mVertexBuffers;				/**< Vertex buffers (we keep a reference to it) used by this vertex array, can be a null pointer */


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D9Renderer/VertexArray.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D9RENDERER_VERTEXARRAY_H__
