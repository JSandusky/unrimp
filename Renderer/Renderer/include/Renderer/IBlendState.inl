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
//[ Includes                                              ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_STATISTICS
	#include "Renderer/IRenderer.h"
#endif
#include "Renderer/BlendStateTypes.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	inline const BlendState &IBlendState::getDefaultBlendState()
	{
		// As default values, the one of Direct3D 11 and Direct 10 were chosen in order to make it easier for those renderer implementations
		// (choosing OpenGL default values would bring no benefit due to the design of the OpenGL API)

		// Direct3D 11 "D3D11_BLEND_DESC structure"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476087%28v=vs.85%29.aspx

		// Direct3D 10 "D3D10_BLEND_DESC structure"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb204893%28v=vs.85%29.aspx

		// Direct3D 9 "D3DRENDERSTATETYPE enumeration"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb172599%28v=vs.85%29.aspx

		// OpenGL & OpenGL ES 2: The official specifications (unlike Direct3D, OpenGL versions are more compatible to each other)

		// Return default values
		// TODO(co) Finish default state comments
		static const Renderer::BlendState BLEND_STATE =
		{																											//	Direct3D 11	Direct3D 10	Direct3D 9			OpenGL
			false,										// alphaToCoverageEnable (int)																"false"			"false"
			false,										// independentBlendEnable (int)																"false"			"false"
			
			{ // renderTarget[8]
				// renderTarget[0]
				{ 
					false,								// blendEnable (int)																		"false"			"false"
					Renderer::Blend::ONE,				// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Renderer::Blend::ONE,				// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					Renderer::ColorWriteEnable::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnable"-flags		"ALL"			"ALL"
				},
				// renderTarget[1]
				{
					false,								// blendEnable (int)																		"false"			"false"
					Renderer::Blend::ONE,				// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Renderer::Blend::ONE,				// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					Renderer::ColorWriteEnable::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnable"-flags		"ALL"			"ALL"
				},
				// renderTarget[2]
				{
					false,								// blendEnable (int)																		"false"			"false"
					Renderer::Blend::ONE,				// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Renderer::Blend::ONE,				// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					Renderer::ColorWriteEnable::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnable"-flags		"ALL"			"ALL"
				},
				// renderTarget[3]
				{
					false,								// blendEnable (int)																		"false"			"false"
					Renderer::Blend::ONE,				// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Renderer::Blend::ONE,				// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					Renderer::ColorWriteEnable::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnable"-flags		"ALL"			"ALL"
				},
				// renderTarget[4]
				{
					false,								// blendEnable (int)																		"false"			"false"
					Renderer::Blend::ONE,				// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Renderer::Blend::ONE,				// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					Renderer::ColorWriteEnable::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnable"-flags		"ALL"			"ALL"
				},
				// renderTarget[5]
				{
					false,								// blendEnable (int)																		"false"			"false"
					Renderer::Blend::ONE,				// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Renderer::Blend::ONE,				// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					Renderer::ColorWriteEnable::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnable"-flags		"ALL"			"ALL"
				},
				// renderTarget[6]
				{
					false,								// blendEnable (int)																		"false"			"false"
					Renderer::Blend::ONE,				// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Renderer::Blend::ONE,				// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					Renderer::ColorWriteEnable::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnable"-flags		"ALL"			"ALL"
				},
				// renderTarget[7]
				{
					false,								// blendEnable (int)																		"false"			"false"
					Renderer::Blend::ONE,				// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Renderer::Blend::ONE,				// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Renderer::Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					Renderer::BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					Renderer::ColorWriteEnable::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnable"-flags		"ALL"			"ALL"
				},
			},
		};
		return BLEND_STATE;
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IBlendState::~IBlendState()
	{
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			--getRenderer().getStatistics().currentNumberOfBlendStates;
		#endif
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline IBlendState::IBlendState(IRenderer &renderer) :
		IState(ResourceType::BLEND_STATE, renderer)
	{
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			++getRenderer().getStatistics().numberOfCreatedBlendStates;
			++getRenderer().getStatistics().currentNumberOfBlendStates;
		#endif
	}

	inline IBlendState::IBlendState(const IBlendState &source) :
		IState(source)
	{
		// Not supported
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			++getRenderer().getStatistics().numberOfCreatedBlendStates;
			++getRenderer().getStatistics().currentNumberOfBlendStates;
		#endif
	}

	inline IBlendState &IBlendState::operator =(const IBlendState &)
	{
		// Not supported
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
