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
	enum class ResourceType
	{
		ROOT_SIGNATURE				   = 0,		///< Root signature
		PROGRAM						   = 1,		///< Program, "Renderer::IShader"-related
		VERTEX_ARRAY				   = 2,		///< Vertex array object (VAO, input-assembler (IA) stage), "Renderer::IBuffer"-related
		// IRenderTarget
		SWAP_CHAIN					   = 3,		///< Swap chain
		FRAMEBUFFER					   = 4,		///< Framebuffer object (FBO)
		// IBuffer
		INDEX_BUFFER				   = 5,		///< Index buffer object (IBO, input-assembler (IA) stage)
		VERTEX_BUFFER				   = 6,		///< Vertex buffer object (VBO, input-assembler (IA) stage)
		UNIFORM_BUFFER				   = 7,		///< Uniform buffer object (UBO, "constant buffer" in Direct3D terminology)
		TEXTURE_BUFFER				   = 8,		///< Texture buffer object (TBO)
		INDIRECT_BUFFER				   = 9,		///< Indirect buffer object
		// ITexture
		TEXTURE_1D					   = 10,	///< Texture 1D
		TEXTURE_2D					   = 11,	///< Texture 2D
		TEXTURE_2D_ARRAY			   = 12,	///< Texture 2D array
		TEXTURE_3D					   = 13,	///< Texture 3D
		TEXTURE_CUBE				   = 14,	///< Texture cube
		// IState
		PIPELINE_STATE				   = 15,	///< Pipeline state (PSO)
		SAMPLER_STATE				   = 16,	///< Sampler state
		// IShader
		VERTEX_SHADER				   = 17,	///< Vertex shader (VS)
		TESSELLATION_CONTROL_SHADER	   = 18,	///< Tessellation control shader (TCS, "hull shader" in Direct3D terminology)
		TESSELLATION_EVALUATION_SHADER = 19,	///< Tessellation evaluation shader (TES, "domain shader" in Direct3D terminology)
		GEOMETRY_SHADER				   = 20,	///< Geometry shader (GS)
		FRAGMENT_SHADER				   = 21		///< Fragment shader (FS, "pixel shader" in Direct3D terminology)
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
