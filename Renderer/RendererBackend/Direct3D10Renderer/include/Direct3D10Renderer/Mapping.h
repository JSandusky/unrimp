/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#ifndef __DIRECT3D10RENDERER_MAPPING_H__
#define __DIRECT3D10RENDERER_MAPPING_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/BufferTypes.h>
#include <Renderer/TextureTypes.h>
#include <Renderer/VertexArrayTypes.h>
#include <Renderer/IndexBufferTypes.h>


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
	*    Direct3D 10 mapping
	*/
	class Mapping
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Renderer::VertexArrayFormat and semantic              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::VertexArrayFormat" to Direct3D 10 format
		*
		*  @param[in] vertexArrayFormat
		*    "Renderer::VertexArrayFormat" to map
		*
		*  @return
		*    Direct3D 10 format (type "DXGI_FORMAT" not used in here in order to keep the header slim)
		*/
		static uint32_t getDirect3D10Format(Renderer::VertexArrayFormat::Enum vertexArrayFormat);

		//[-------------------------------------------------------]
		//[ Renderer::BufferUsage                                 ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::BufferUsage" to Direct3D 10 usage and CPU access flags
		*
		*  @param[in]  bufferUsage
		*    "Renderer::BufferUsage" to map
		*  @param[out] cpuAccessFlags
		*    Receives the CPU access flags
		*
		*  @return
		*    Direct3D 10 usage (type "D3D10_USAGE" not used in here in order to keep the header slim)
		*/
		static uint32_t getDirect3D10UsageAndCPUAccessFlags(Renderer::BufferUsage::Enum bufferUsage, uint32_t &cpuAccessFlags);

		//[-------------------------------------------------------]
		//[ Renderer::IndexBufferFormat                           ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::IndexBufferFormat" to Direct3D 10 format
		*
		*  @param[in] indexBufferFormat
		*    "Renderer::IndexBufferFormat" to map
		*
		*  @return
		*    Direct3D 10 format (type "DXGI_FORMAT" not used in here in order to keep the header slim)
		*/
		static uint32_t getDirect3D10Format(Renderer::IndexBufferFormat::Enum indexBufferFormat);

		//[-------------------------------------------------------]
		//[ Renderer::TextureFormat                               ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::TextureFormat" to Direct3D 10 size (in bytes)
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to map
		*
		*  @return
		*    Direct3D 10 size (in bytes)
		*/
		static uint32_t getDirect3D10Size(Renderer::TextureFormat::Enum textureFormat);

		/**
		*  @brief
		*    "Renderer::TextureFormat" to Direct3D 10 format
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to map
		*
		*  @return
		*    Direct3D 10 format (type "DXGI_FORMAT" not used in here in order to keep the header slim)
		*/
		static uint32_t getDirect3D10Format(Renderer::TextureFormat::Enum textureFormat);


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D10RENDERER_MAPPING_H__
