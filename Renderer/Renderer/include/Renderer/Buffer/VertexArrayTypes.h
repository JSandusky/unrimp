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
#include "Renderer/PlatformTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IVertexBuffer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Definitions                                           ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Vertex attribute format
	*/
	enum class VertexAttributeFormat
	{
		FLOAT_1			= 0,	///< Float 1 (one component per element, 32 bit floating point per component), supported by DirectX 9, DirectX 10, DirectX 11, OpenGL and OpenGL ES 2
		FLOAT_2			= 1,	///< Float 2 (two components per element, 32 bit floating point per component), supported by DirectX 9, DirectX 10, DirectX 11, OpenGL and OpenGL ES 2
		FLOAT_3			= 2,	///< Float 3 (three components per element, 32 bit floating point per component), supported by DirectX 9, DirectX 10, DirectX 11, OpenGL and OpenGL ES 2
		FLOAT_4			= 3,	///< Float 4 (four components per element, 32 bit floating point per component), supported by DirectX 9, DirectX 10, DirectX 11, OpenGL and OpenGL ES 2
		R8G8B8A8_UNORM	= 4,	///< Unsigned byte 4 (four components per element, 8 bit integer per component), will be passed in a normalized form into shaders, supported by DirectX 9, DirectX 10, DirectX 11, OpenGL and OpenGL ES 2
		R8G8B8A8_UINT	= 5,	///< Unsigned byte 4 (four components per element, 8 bit integer per component), supported by DirectX 9, DirectX 10, DirectX 11, OpenGL and OpenGL ES 2
		SHORT_2			= 6,	///< Short 2 (two components per element, 16 bit integer per component), supported by DirectX 9, DirectX 10, DirectX 11, OpenGL and OpenGL ES 2
		SHORT_4			= 7		///< Short 4 (four components per element, 16 bit integer per component), supported by DirectX 9, DirectX 10, DirectX 11, OpenGL and OpenGL ES 2
	};


	//[-------------------------------------------------------]
	//[ Types                                                 ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Vertex attribute ("Input element description" in Direct3D terminology)
	*
	*  @note
	*    - This piece of data is POD and can be serialized/deserialized as a whole (hence the byte alignment compiler setting)
	*/
	#pragma pack(push)
	#pragma pack(1)
		struct VertexAttribute
		{
			// Data destination
			VertexAttributeFormat vertexAttributeFormat;	///< Vertex attribute format
			char				  name[32];					///< Vertex attribute name
			char				  semanticName[32];			///< Vertex attribute semantic name
			uint32_t			  semanticIndex;			///< Vertex attribute semantic index
			// Data source
			uint32_t			  inputSlot;				///< Index of the vertex input slot to use (see "Renderer::VertexArrayVertexBuffer")
			uint32_t			  alignedByteOffset;		///< Offset (in bytes) from the start of the vertex to this certain attribute
			// Data source, instancing part
			uint32_t			  instancesPerElement;		/**< Number of instances to draw with the same data before advancing in the buffer by one element.
																 0 for no instancing meaning the data is per-vertex instead of per-instance, 1 for drawing one
																 instance with the same data, 2 for drawing two instances with the same data and so on.
																 Instanced arrays is a shader model 3 feature, only supported if "Renderer::Capabilities::instancedArrays" is true.
																 In order to support Direct3D 9, do not use this within the first attribute. */
		};
	#pragma pack(pop)

	/**
	*  @brief
	*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
	*
	*  @see
	*    - "Renderer::IVertexArray" class documentation
	*/
	struct VertexAttributes
	{
			  uint32_t		   numberOfAttributes;	///< Number of attributes (position, color, texture coordinate, normal...), having zero attributes is valid
		const VertexAttribute* attributes;			///< At least "numberOfAttributes" instances of vertex array attributes, can be a null pointer in case there are zero attributes, the data is internally copied and you have to free your memory if you no longer need it

		VertexAttributes()
		{
		}
		VertexAttributes(uint32_t _numberOfAttributes, const VertexAttribute*	_attributes) :
			numberOfAttributes(_numberOfAttributes),
			attributes(_attributes)
		{
		}
	};

	/**
	*  @brief
	*    Vertex array vertex buffer
	*
	*  @see
	*    - "Renderer::IVertexArray" class documentation
	*/
	struct VertexArrayVertexBuffer
	{
		IVertexBuffer* vertexBuffer;	///< Vertex buffer used at this vertex input slot (vertex array instances keep a reference to the vertex buffers used by the vertex array attributes, see "Renderer::IRenderer::createVertexArray()" for details)
		uint32_t	   strideInBytes;	///< Specifies the size in bytes of each vertex entry
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer