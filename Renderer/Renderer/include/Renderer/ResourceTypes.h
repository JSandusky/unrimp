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
#ifndef __RENDERER_RESOURCE_TYPES_H__
#define __RENDERER_RESOURCE_TYPES_H__


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
	*    Resource type
	*/
	struct ResourceType
	{
		enum Enum
		{
			PROGRAM						   = 0,		/**< Program */
			VERTEX_ARRAY				   = 1,		/**< Vertex array object (VAO, input-assembler (IA) stage) */
			// IRenderTarget
			SWAP_CHAIN					   = 2,		/**< Swap chain */
			FRAMEBUFFER					   = 3,		/**< Framebuffer object (FBO) */
			// IBuffer
			INDEX_BUFFER				   = 4,		/**< Index buffer object (IBO, input-assembler (IA) stage) */
			VERTEX_BUFFER				   = 5,		/**< Vertex buffer object (VBO, input-assembler (IA) stage) */
			UNIFORM_BUFFER				   = 6,		/**< Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) */
			// ITexture
			TEXTURE_BUFFER				   = 7,		/**< Texture buffer object (TBO) */
			TEXTURE_2D					   = 8,		/**< Texture 2D */
			TEXTURE_2D_ARRAY			   = 9,		/**< Texture 2D array */
			// IState
			RASTERIZER_STATE			   = 10,	/**< Rasterizer state (rasterizer stage (RS)) */
			DEPTH_STENCIL_STATE			   = 11,	/**< Depth stencil state (output-merger (OM) stage) */
			BLEND_STATE					   = 12,	/**< Blend state (output-merger (OM) stage) */
			SAMPLER_STATE				   = 13,	/**< Sampler state */
			// IShader
			VERTEX_SHADER				   = 14,	/**< Vertex shader (VS) */
			TESSELLATION_CONTROL_SHADER	   = 15,	/**< Tessellation control shader (TCS, "hull shader" in Direct3D terminology) */
			TESSELLATION_EVALUATION_SHADER = 16,	/**< Tessellation evaluation shader (TES, "domain shader" in Direct3D terminology) */
			GEOMETRY_SHADER				   = 17,	/**< Geometry shader (GS) */
			FRAGMENT_SHADER				   = 18,	/**< Fragment shader (FS, "pixel shader" in Direct3D terminology) */
			// ICollection
			TEXTURE_COLLECTION			   = 19,	/**< Texture collection */
			SAMPLER_STATE_COLLECTION	   = 20		/**< Sampler state collection */
		};
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_RESOURCE_TYPES_H__
