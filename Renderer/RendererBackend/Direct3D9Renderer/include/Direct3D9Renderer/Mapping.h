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
#include <Renderer/RendererTypes.h>
#include <Renderer/Buffer/VertexArrayTypes.h>
#include <Renderer/Buffer/BufferTypes.h>
#include <Renderer/Buffer/IndexBufferTypes.h>
#include <Renderer/Texture/TextureTypes.h>
#include <Renderer/State/SamplerStateTypes.h>


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
	*    Direct3D 9 mapping
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
		*    "Renderer::FilterMode" to Direct3D 9 magnification filter mode
		*
		*  @param[in] filterMode
		*    "Renderer::FilterMode" to map
		*
		*  @return
		*    Direct3D 9 magnification filter mode
		*/
		static uint32_t getDirect3D9MagFilterMode(Renderer::FilterMode filterMode);

		/**
		*  @brief
		*    "Renderer::FilterMode" to Direct3D 9 minification filter mode
		*
		*  @param[in] filterMode
		*    "Renderer::FilterMode" to map
		*
		*  @return
		*    Direct3D 9 minification filter mode
		*/
		static uint32_t getDirect3D9MinFilterMode(Renderer::FilterMode filterMode);

		/**
		*  @brief
		*    "Renderer::FilterMode" to Direct3D 9 mipmapping filter mode
		*
		*  @param[in] filterMode
		*    "Renderer::FilterMode" to map
		*
		*  @return
		*    Direct3D 9 mipmapping filter mode
		*/
		static uint32_t getDirect3D9MipFilterMode(Renderer::FilterMode filterMode);

		//[-------------------------------------------------------]
		//[ Renderer::TextureAddressMode                          ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::TextureAddressMode" to Direct3D 9 texture address mode
		*
		*  @param[in] textureAddressMode
		*    "Renderer::TextureAddressMode" to map
		*
		*  @return
		*    Direct3D 9 texture address mode
		*/
		static uint32_t getDirect3D9TextureAddressMode(Renderer::TextureAddressMode textureAddressMode);

		//[-------------------------------------------------------]
		//[ Renderer::ComparisonFunc                              ]
		//[-------------------------------------------------------]
		// TODO(co)
		/**
		*  @brief
		*    "Renderer::ComparisonFunc" to Direct3D 9 comparison function
		*
		*  @param[in] comparisonFunc
		*    "Renderer::ComparisonFunc" to map
		*
		*  @return
		*    Direct3D 9 comparison function
		*/
	//	static GLint getDirect3D9ComparisonFunc(Renderer::ComparisonFunc comparisonFunc);

		//[-------------------------------------------------------]
		//[ Renderer::VertexAttributeFormat and semantic          ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::VertexAttributeFormat" to Direct3D 9 type
		*
		*  @param[in] vertexAttributeFormat
		*    "Renderer::VertexAttributeFormat" to map
		*
		*  @return
		*    Direct3D 9 type (type "BYTE" not used in here in order to keep the header slim)
		*/
		static uint8_t getDirect3D9Type(Renderer::VertexAttributeFormat vertexAttributeFormat);

		/**
		*  @brief
		*    "Semantic as string" to Direct3D 9 semantic
		*
		*  @param[in] semanticName
		*    Semantic name as string, must be a valid pointer
		*
		*  @return
		*    Direct3D 9 semantic, "D3DDECLUSAGE_POSITION" as fallback if no match was found (type "BYTE" not used in here in order to keep the header slim)
		*/
		static uint32_t getDirect3D9Semantic(const char *semanticName);

		//[-------------------------------------------------------]
		//[ Renderer::BufferUsage                                 ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::BufferUsage" to Direct3D 9 usage
		*
		*  @param[in] bufferUsage
		*    "Renderer::BufferUsage" to map
		*
		*  @return
		*    Direct3D 9 usage
		*/
		static uint32_t getDirect3D9Usage(Renderer::BufferUsage bufferUsage);

		//[-------------------------------------------------------]
		//[ Renderer::IndexBufferFormat                           ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::IndexBufferFormat" to Direct3D 9 format
		*
		*  @param[in] indexBufferFormat
		*    "Renderer::IndexBufferFormat" to map
		*
		*  @return
		*    Direct3D 9 format (type "D3DFORMAT" not used in here in order to keep the header slim)
		*/
		static uint32_t getDirect3D9Format(Renderer::IndexBufferFormat::Enum indexBufferFormat);

		//[-------------------------------------------------------]
		//[ Renderer::TextureFormat                               ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::TextureFormat" to Direct3D 9 format
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to map
		*
		*  @return
		*    Direct3D 9 format (type "D3DFORMAT" not used in here in order to keep the header slim)
		*/
		static uint32_t getDirect3D9Format(Renderer::TextureFormat::Enum textureFormat);


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
