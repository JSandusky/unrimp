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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	inline const BlendState& BlendStateBuilder::getDefaultBlendState()
	{
		// As default values, the one of Direct3D 11 and Direct 10 were chosen in order to make it easier for those renderer implementations
		// (choosing OpenGL default values would bring no benefit due to the design of the OpenGL API)
		// - Direct3D 11 "D3D11_BLEND_DESC structure"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476087%28v=vs.85%29.aspx
		// - Direct3D 10 "D3D10_BLEND_DESC structure"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb204893%28v=vs.85%29.aspx
		// - Direct3D 9 "D3DRENDERSTATETYPE enumeration"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb172599%28v=vs.85%29.aspx
		// - OpenGL & OpenGL ES 2: The official specifications (unlike Direct3D, OpenGL versions are more compatible to each other)

		// Return default values
		// TODO(co) Finish default state comments
		static const BlendState BLEND_STATE =
		{																																//	Direct3D 11	Direct3D 10	Direct3D 9			OpenGL
			false,								// alphaToCoverageEnable (int)																"false"			"false"
			false,								// independentBlendEnable (int)																"false"			"false"
			
			{ // renderTarget[8]
				// renderTarget[0]
				{ 
					false,						// blendEnable (int)																		"false"			"false"
					Blend::ONE,					// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Blend::ONE,					// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					ColorWriteEnableFlag::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnableFlag"-flags	"ALL"			"ALL"
				},
				// renderTarget[1]
				{
					false,						// blendEnable (int)																		"false"			"false"
					Blend::ONE,					// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Blend::ONE,					// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					ColorWriteEnableFlag::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnableFlag"-flags	"ALL"			"ALL"
				},
				// renderTarget[2]
				{
					false,						// blendEnable (int)																		"false"			"false"
					Blend::ONE,					// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Blend::ONE,					// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					ColorWriteEnableFlag::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnableFlag"-flags	"ALL"			"ALL"
				},
				// renderTarget[3]
				{
					false,						// blendEnable (int)																		"false"			"false"
					Blend::ONE,					// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Blend::ONE,					// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					ColorWriteEnableFlag::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnableFlag"-flags	"ALL"			"ALL"
				},
				// renderTarget[4]
				{
					false,						// blendEnable (int)																		"false"			"false"
					Blend::ONE,					// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Blend::ONE,					// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					ColorWriteEnableFlag::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnableFlag"-flags	"ALL"			"ALL"
				},
				// renderTarget[5]
				{
					false,						// blendEnable (int)																		"false"			"false"
					Blend::ONE,					// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Blend::ONE,					// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					ColorWriteEnableFlag::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnableFlag"-flags	"ALL"			"ALL"
				},
				// renderTarget[6]
				{
					false,						// blendEnable (int)																		"false"			"false"
					Blend::ONE,					// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Blend::ONE,					// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					ColorWriteEnableFlag::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnableFlag"-flags	"ALL"			"ALL"
				},
				// renderTarget[7]
				{
					false,						// blendEnable (int)																		"false"			"false"
					Blend::ONE,					// srcBlend (Renderer::Blend)																"ONE"			"ONE"
					Blend::ZERO,				// destBlend (Renderer::Blend)																"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOp (Renderer::BlendOp)																"ADD"			"ADD"
					Blend::ONE,					// srcBlendAlpha (Renderer::Blend)															"ONE"			"ONE"
					Blend::ZERO,				// destBlendAlpha (Renderer::Blend)															"ZERO"			"ZERO"
					BlendOp::ADD,				// blendOpAlpha (Renderer::BlendOp)															"ADD"			"ADD"
					ColorWriteEnableFlag::ALL,	// renderTargetWriteMask (uint8_t), combination of "Renderer::ColorWriteEnableFlag"-flags	"ALL"			"ALL"
				},
			},
		};
		return BLEND_STATE;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
