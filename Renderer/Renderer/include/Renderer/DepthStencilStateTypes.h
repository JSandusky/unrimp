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
#ifndef __RENDERER_DEPTHSTENCILSTATE_TYPES_H__
#define __RENDERER_DEPTHSTENCILSTATE_TYPES_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/RendererTypes.h"	// For "Renderer::ComparisonFunc"


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
	*    Depth write mask
	*
	*  @note
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	// TODO(co) Renderer::DepthStencilState, document
	struct DepthWriteMask
	{
		enum Enum
		{
			ZERO = 0,
			ALL  = 1
		};
	};

	/**
	*  @brief
	*    Stencil operation
	*
	*  @note
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	// TODO(co) Renderer::DepthStencilState, document
	struct StencilOp
	{
		enum Enum
		{
			KEEP	 = 1,
			ZERO	 = 2,
			REPLACE	 = 3,
			INCR_SAT = 4,
			DERC_SAT = 5,
			INVERT	 = 6,
			INCREASE = 7,
			DECREASE = 8
		};
	};


	//[-------------------------------------------------------]
	//[ Types                                                 ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Depth stencil operation description
	*
	*  @note
	*    - This depth stencil operation description maps directly to Direct3D 10 & 11, do not change it
	*    - If you want to know how the default values were chosen, have a look into the "Renderer::IDepthStencilState::getDefaultDepthStencilState()"-implementation
	*/
	// TODO(co) Renderer::DepthStencilState, document
	struct DepthStencilOpDesc
	{
		StencilOp::Enum		 stencilFailOp;			///< Default: "Renderer::StencilOp::KEEP"
		StencilOp::Enum		 stencilDepthFailOp;	///< Default: "Renderer::StencilOp::KEEP"
		StencilOp::Enum		 stencilPassOp;			///< Default: "Renderer::StencilOp::KEEP"
		ComparisonFunc::Enum stencilFunc;			///< Default: "Renderer::ComparisonFunc::ALWAYS"
	};

	/**
	*  @brief
	*    Depth stencil state
	*
	*  @note
	*    - This depth stencil state maps directly to Direct3D 10 & 11, do not change it
	*    - This also means that "int" is used over "bool" because in Direct3D it's defined this way
	*    - If you want to know how the default values were chosen, have a look into the "Renderer::IDepthStencilState::getDefaultDepthStencilState()"-implementation
	*/
	// TODO(co) Renderer::DepthStencilState, document
	struct DepthStencilState
	{
		int					 depthEnable;		///< Default: "true"
		DepthWriteMask::Enum depthWriteMask;	///< Default: "Renderer::DepthWriteMask::ALL"
		ComparisonFunc::Enum depthFunc;			///< Default: "Renderer::ComparisonFunc::LESS"
		int					 stencilEnable;		///< Default: "false"
		unsigned char		 stencilReadMask;	///< Default: "0xff"
		unsigned char		 stencilWriteMask;	///< Default: "0xff"
		DepthStencilOpDesc	 frontFace;			///< Default: See "Renderer::DepthStencilOpDesc"
		DepthStencilOpDesc	 backFace;			///< Default: See "Renderer::DepthStencilOpDesc"
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_DEPTHSTENCILSTATE_TYPES_H__
