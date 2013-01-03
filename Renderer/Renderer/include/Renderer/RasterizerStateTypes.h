/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __RENDERER_RASTERIZERSTATE_TYPES_H__
#define __RENDERER_RASTERIZERSTATE_TYPES_H__


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
	*    Fill mode
	*
	*  @note
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	struct FillMode
	{
		enum Enum
		{
			WIREFRAME = 2,	/**< Wireframe */
			SOLID     = 3	/**< Solid */
		};
	};

	/**
	*  @brief
	*    Cull mode
	*
	*  @note
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	struct CullMode
	{
		enum Enum
		{
			NONE  = 1,	/**< No culling */
			FRONT = 2,	/**< Do not draw triangles that are front-facing */
			BACK  = 3	/**< Do not draw triangles that are back-facing */
		};
	};


	//[-------------------------------------------------------]
	//[ Types                                                 ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Rasterizer state
	*
	*  @note
	*    - This rasterizer state maps directly to Direct3D 10 & 11, do not change it
	*    - This also means that "int" is used over "bool" because in Direct3D it's defined this way
	*    - If you want to know how the default values were chosen, have a look into the "Renderer::IRasterizerState::getDefaultRasterizerState()"-implementation
	*/
	// TODO(co) Renderer::RasterizerState, documentation
	struct RasterizerState
	{
		FillMode::Enum fillMode;				/**< Default: "Renderer::FillMode::SOLID" */
		CullMode::Enum cullMode;				/**< Default: "Renderer::CullMode::BACK" */
		int			   frontCounterClockwise;	/**< Select counter-clockwise polygons as front-facing? Default: "false" */
		int			   depthBias;				/**< Default: "0" */
		float		   depthBiasClamp;			/**< Default: "0.0f" */
		float		   slopeScaledDepthBias;	/**< Default: "0.0f" */
		int			   depthClipEnable;			/**< Default: "true" */
		int			   scissorEnable;			/**< Default: "false" */
		int			   multisampleEnable;		/**< Default: "false" */
		int			   antialiasedLineEnable;	/**< Default: "false" */
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_RASTERIZERSTATE_TYPES_H__
