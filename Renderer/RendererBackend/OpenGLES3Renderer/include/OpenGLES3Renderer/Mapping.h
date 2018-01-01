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
#include <Renderer/RendererTypes.h>
#include <Renderer/Buffer/BufferTypes.h>
#include <Renderer/Buffer/VertexArrayTypes.h>
#include <Renderer/Buffer/IndexBufferTypes.h>
#include <Renderer/Texture/TextureTypes.h>
#include <Renderer/State/BlendStateTypes.h>
#include <Renderer/State/SamplerStateTypes.h>
#include <Renderer/State/PipelineStateTypes.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class Context;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL ES 3 mapping
	*/
	class Mapping
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Renderer::FilterMode                                  ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::FilterMode" to OpenGL ES 3 magnification filter mode
		*
		*  @param[in] context
		*    Renderer context to use
		*  @param[in] filterMode
		*    "Renderer::FilterMode" to map
		*
		*  @return
		*    OpenGL ES 3 magnification filter mode (type "GLint" not used in here in order to keep the header slim)
		*/
		static int getOpenGLES3MagFilterMode(const Renderer::Context& context, Renderer::FilterMode filterMode);

		/**
		*  @brief
		*    "Renderer::FilterMode" to OpenGL ES 3 minification filter mode
		*
		*  @param[in] context
		*    Renderer context to use
		*  @param[in] filterMode
		*    "Renderer::FilterMode" to map
		*  @param[in] hasMipmaps
		*    Are mipmaps available?
		*
		*  @return
		*    OpenGL ES 3 minification filter mode (type "GLint" not used in here in order to keep the header slim)
		*/
		static int getOpenGLES3MinFilterMode(const Renderer::Context& context, Renderer::FilterMode filterMode, bool hasMipmaps);

		/**
		*  @brief
		*    "Renderer::FilterMode" to OpenGL ES 3 compare mode
		*
		*  @param[in] filterMode
		*    "Renderer::FilterMode" to map
		*
		*  @return
		*    OpenGL ES 3 compare mode (type "GLint" not used in here in order to keep the header slim)
		*/
		static int getOpenGLES3CompareMode(Renderer::FilterMode filterMode);

		//[-------------------------------------------------------]
		//[ Renderer::TextureAddressMode                          ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::TextureAddressMode" to OpenGL ES 3 texture address mode
		*
		*  @param[in] textureAddressMode
		*    "Renderer::TextureAddressMode" to map
		*
		*  @return
		*    OpenGL ES 3 texture address mode (type "GLint" not used in here in order to keep the header slim)
		*/
		static int getOpenGLES3TextureAddressMode(Renderer::TextureAddressMode textureAddressMode);

		//[-------------------------------------------------------]
		//[ Renderer::ComparisonFunc                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::ComparisonFunc" to OpenGL ES 3 comparison function
		*
		*  @param[in] comparisonFunc
		*    "Renderer::ComparisonFunc" to map
		*
		*  @return
		*    OpenGL ES 3 comparison function (type "GLint" not used in here in order to keep the header slim)
		*/
		static int getOpenGLES3ComparisonFunc(Renderer::ComparisonFunc comparisonFunc);

		//[-------------------------------------------------------]
		//[ Renderer::VertexAttributeFormat                       ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::VertexAttributeFormat" to OpenGL ES 3 size (number of elements)
		*
		*  @param[in] vertexAttributeFormat
		*    "Renderer::VertexAttributeFormat" to map
		*
		*  @return
		*    OpenGL ES 3 size (number of elements, type GLint not used in here in order to keep the header slim)
		*/
		static int getOpenGLES3Size(Renderer::VertexAttributeFormat vertexAttributeFormat);

		/**
		*  @brief
		*    "Renderer::VertexAttributeFormat" to OpenGL ES 3 type
		*
		*  @param[in] vertexAttributeFormat
		*    "Renderer::VertexAttributeFormat" to map
		*
		*  @return
		*    OpenGL ES 3 type (type "GLenum" not used in here in order to keep the header slim)
		*/
		static uint32_t getOpenGLES3Type(Renderer::VertexAttributeFormat vertexAttributeFormat);

		/**
		*  @brief
		*    Return whether or not "Renderer::VertexAttributeFormat" is a normalized format
		*
		*  @param[in] vertexAttributeFormat
		*    "Renderer::VertexAttributeFormat" to check
		*
		*  @return
		*    "1" if the format is normalized, else "0" (type "GLboolean" not used in here in order to keep the header slim)
		*/
		static uint32_t isOpenGLES3VertexAttributeFormatNormalized(Renderer::VertexAttributeFormat vertexAttributeFormat);

		/**
		*  @brief
		*    Return whether or not "Renderer::VertexAttributeFormat" is an integer format
		*
		*  @param[in] vertexAttributeFormat
		*    "Renderer::VertexAttributeFormat" to check
		*
		*  @return
		*    "1" if the format is integer, else "0" (type "GLboolean" not used in here in order to keep the header slim)
		*/
		static uint32_t isOpenGLES3VertexAttributeFormatInteger(Renderer::VertexAttributeFormat vertexAttributeFormat);

		//[-------------------------------------------------------]
		//[ Renderer::BufferUsage                                 ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::BufferUsage" to OpenGL ES 3 usage
		*
		*  @param[in] bufferUsage
		*    "Renderer::BufferUsage" to map
		*
		*  @return
		*    OpenGL ES 3 usage (type "GLenum" not used in here in order to keep the header slim)
		*/
		static uint32_t getOpenGLES3Type(Renderer::BufferUsage bufferUsage);

		//[-------------------------------------------------------]
		//[ Renderer::IndexBufferFormat                           ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::IndexBufferFormat" to OpenGL ES 3 type
		*
		*  @param[in] indexBufferFormat
		*    "Renderer::IndexBufferFormat" to map
		*
		*  @return
		*    OpenGL ES 3 type (type "GLenum" not used in here in order to keep the header slim)
		*/
		static uint32_t getOpenGLES3Type(Renderer::IndexBufferFormat::Enum indexBufferFormat);

		//[-------------------------------------------------------]
		//[ Renderer::TextureFormat                               ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::TextureFormat" to OpenGL ES 3 internal format
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to map
		*
		*  @return
		*    OpenGL ES 3 internal format (type "GLint" not used in here in order to keep the header slim)
		*/
		static uint32_t getOpenGLES3InternalFormat(Renderer::TextureFormat::Enum textureFormat);

		/**
		*  @brief
		*    "Renderer::TextureFormat" to OpenGL ES 3 format
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to map
		*
		*  @return
		*    OpenGL ES 3 format (type "GLint" not used in here in order to keep the header slim)
		*/
		static uint32_t getOpenGLES3Format(Renderer::TextureFormat::Enum textureFormat);

		/**
		*  @brief
		*    "Renderer::TextureFormat" to OpenGL ES 3 type
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to map
		*
		*  @return
		*    OpenGL ES 3 type (type "GLenum" not used in here in order to keep the header slim)
		*/
		static uint32_t getOpenGLES3Type(Renderer::TextureFormat::Enum textureFormat);

		//[-------------------------------------------------------]
		//[ Renderer::PrimitiveTopology                           ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::PrimitiveTopology" to OpenGL ES 3 type
		*
		*  @param[in] primitive
		*    "Renderer::PrimitiveTopology" to map
		*
		*  @return
		*    OpenGL ES 3 type (type "GLenum" not used in here in order to keep the header slim)
		*/
		static uint32_t getOpenGLES3Type(Renderer::PrimitiveTopology primitive);

		//[-------------------------------------------------------]
		//[ Renderer::MapType                                     ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::MapType" to OpenGL ES 3 access bit field for "glMapBufferRange()"
		*
		*  @param[in] mapType
		*    "Renderer::MapType" to map
		*
		*  @return
		*    OpenGL ES 3 type (type "GLbitfield" not used in here in order to keep the header slim)
		*/
		static uint32_t getOpenGLES3MapRangeType(Renderer::MapType mapType);

		//[-------------------------------------------------------]
		//[ Renderer::MapType                                     ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::Blend" to OpenGL ES 3 type
		*
		*  @param[in] blend
		*    "Renderer::Blend" to map
		*
		*  @return
		*    OpenGL ES 3 type (type "GLenum" not used in here in order to keep the header slim)
		*/
		static uint32_t getOpenGLES3BlendType(Renderer::Blend blend);


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
