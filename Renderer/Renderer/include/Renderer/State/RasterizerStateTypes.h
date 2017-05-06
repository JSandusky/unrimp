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
	*    Fill mode
	*
	*  @note
	*    - These constants directly map to Direct3D 10 & 11 & 12 constants, do not change them
	*
	*  @see
	*    - "D3D12_FILL_MODE"-documentation for details
	*/
	enum class FillMode
	{
		WIREFRAME = 2,	///< Wireframe
		SOLID     = 3	///< Solid
	};

	/**
	*  @brief
	*    Cull mode
	*
	*  @note
	*    - These constants directly map to Direct3D 10 & 11 & 12 constants, do not change them
	*
	*  @see
	*    - "D3D12_CULL_MODE"-documentation for details
	*/
	enum class CullMode
	{
		NONE  = 1,	///< No culling
		FRONT = 2,	///< Do not draw triangles that are front-facing
		BACK  = 3	///< Do not draw triangles that are back-facing
	};

	/**
	*  @brief
	*    Conservative rasterization mode
	*
	*  @note
	*    - These constants directly map to Direct3D 12 constants, do not change them
	*
	*  @see
	*    - "D3D12_CONSERVATIVE_RASTERIZATION_MODE"-documentation for details
	*/
	enum class ConservativeRasterizationMode
	{
		OFF	= 0,	///< Conservative rasterization is off
		ON	= 1		///< Conservative rasterization is on
	};


	//[-------------------------------------------------------]
	//[ Types                                                 ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Rasterizer state
	*
	*  @note
	*    - This rasterizer state maps directly to Direct3D 10 & 11 & 12, do not change it
	*    - This also means that "int" is used over "bool" because in Direct3D it's defined this way
	*    - If you want to know how the default values were chosen, have a look into the "Renderer::RasterizerStateBuilder::getDefaultRasterizerState()"-implementation
	*    - Lookout! In Direct3D 12 the scissor test can't be deactivated and hence one always needs to set a valid scissor rectangle.
	*      Use the convenience "Renderer::Command::SetViewportAndScissorRectangle"-command if possible to not walk into this Direct3D 12 trap.
	*
	*  @see
	*    - "D3D12_RASTERIZER_DESC"-documentation for details
	*/
	struct RasterizerState
	{
		FillMode						fillMode;						///< Default: "Renderer::FillMode::SOLID"
		CullMode						cullMode;						///< Default: "Renderer::CullMode::BACK"
		int								frontCounterClockwise;			///< Select counter-clockwise polygons as front-facing? Boolean value. Default: "false"
		int								depthBias;						///< Default: "0"
		float							depthBiasClamp;					///< Default: "0.0f"
		float							slopeScaledDepthBias;			///< Default: "0.0f"
		int								depthClipEnable;				///< Boolean value. Default: "true"
		int								multisampleEnable;				///< Boolean value. Default: "false"
		int								antialiasedLineEnable;			///< Boolean value. Default: "false"
		unsigned int					forcedSampleCount;				///< Default: "0"
		ConservativeRasterizationMode	conservativeRasterizationMode;	///< Boolean value. >= Direct3D 12 only. Default: "false"
		int								scissorEnable;					///< Boolean value. Not available in Direct3D 12 (scissor testing is always enabled). Default: "false"
	};
	struct RasterizerStateBuilder
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the default rasterizer state
		*
		*  @return
		*    The default rasterizer state, see "Renderer::RasterizerState" for the default values
		*/
		static inline const RasterizerState& getDefaultRasterizerState();


	};



//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/State/RasterizerStateTypes.inl"
