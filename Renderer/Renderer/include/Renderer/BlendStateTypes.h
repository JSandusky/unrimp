/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#ifndef __RENDERER_BLENDSTATE_TYPES_H__
#define __RENDERER_BLENDSTATE_TYPES_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/RendererTypes.h"	// For "Renderer::ColorWriteEnable"


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
	*    Blend
	*
	*  @note
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	// TODO(co) Renderer::Blend, document
	struct Blend
	{
		enum Enum
		{
			ZERO		     = 1,
			ONE			     = 2,
			SRC_COLOR	     = 3,
			INV_SRC_COLOR    = 4,
			SRC_ALPHA	     = 5,
			INV_SRC_ALPHA    = 6,
			DEST_ALPHA	     = 7,
			INV_DEST_ALPHA   = 8,
			DEST_COLOR	     = 9,
			INV_DEST_COLOR   = 10,
			SRC_ALPHA_SAT    = 11,
			BLEND_FACTOR     = 14,
			INV_BLEND_FACTOR = 15,
			SRC_1_COLOR	     = 16,
			INV_SRC_1_COLOR  = 17,
			SRC_1_ALPHA	     = 18,
			INV_SRC_1_ALPHA  = 19
		};
	};

	/**
	*  @brief
	*    Blend operation
	*
	*  @note
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	// TODO(co) Renderer::BlendOp, document
	struct BlendOp
	{
		enum Enum
		{
			ADD			 = 1,
			SUBTRACT	 = 2,
			REV_SUBTRACT = 3,
			MIN			 = 4,
			MAX			 = 5
		};
	};


	//[-------------------------------------------------------]
	//[ Types                                                 ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Render target blend description
	*
	*  @note
	*    - This render target blend description maps directly to Direct3D 10.1 & 11, do not change it
	*    - This also means that "int" is used over "bool" because in Direct3D it's defined this way
	*    - If you want to know how the default values were chosen, have a look into the "Renderer::IBlendState::getDefaultBlendState()"-implementation
	*/
	// TODO(co) Renderer::RasterizerState, documentation
	struct RenderTargetBlendDesc
	{
		int			  blendEnable;				///< Default: "false"
		Blend::Enum	  srcBlend;					///< Default: "Renderer::Blend::ONE"
		Blend::Enum	  destBlend;				///< Default: "Renderer::Blend::ZERO"
		BlendOp::Enum blendOp;					///< Default: "Renderer::BlendOp::ADD"
		Blend::Enum	  srcBlendAlpha;			///< Default: "Renderer::Blend::ONE"
		Blend::Enum	  destBlendAlpha;			///< Default: "Renderer::Blend::ZERO"
		BlendOp::Enum blendOpAlpha;				///< Default: "Renderer::BlendOp::ADD"
		uint8_t		  renderTargetWriteMask;	///< Combination of "Renderer::ColorWriteEnable"-flags. Default: "Renderer::ColorWriteEnable::ALL"
	};

	/**
	*  @brief
	*    Blend state
	*
	*  @note
	*    - This blend state maps directly to Direct3D 10.1 & 11, do not change it
	*    - This also means that "int" is used over "bool" because in Direct3D it's defined this way
	*    - If you want to know how the default values were chosen, have a look into the "Renderer::IBlendState::getDefaultBlendState()"-implementation
	*/
	// TODO(co) Renderer::BlendState, document
	struct BlendState
	{
		int					  alphaToCoverageEnable;	///< Default: "false"
		int					  independentBlendEnable;	///< Default: "false"
		RenderTargetBlendDesc renderTarget[8];			///< Default: See "Renderer::RenderTargetBlendDesc"
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_BLENDSTATE_TYPES_H__
