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
//[ Includes                                              ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_STATISTICS
	#include "Renderer/IRenderer.h"
#endif
#include "Renderer/State/SamplerStateTypes.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	inline const SamplerState& ISamplerState::getDefaultSamplerState()
	{
		// As default values, the one of Direct3D 11 and Direct 10 were chosen in order to make it easier for those renderer implementations
		// (choosing OpenGL default values would bring no benefit due to the design of the OpenGL API)

		// Direct3D 11 "D3D11_SAMPLER_DESC structure"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476207%28v=vs.85%29.aspx

		// The Direct3D 10 documentation is buggy: (online and within the "Microsoft DirectX SDK (June 2010)"-SDK, checked it)
		//   - "D3D10_SAMPLER_DESC structure"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb172415%28v=vs.85%29.aspx
		//     -> Says "Default filter is Min_Mag_Mip_Point"
		//   - "ID3D10Device::VSSetSamplers method"-documentation at MSDN: msdn.microsoft.com/en-us/library/windows/desktop/bb173627(v=vs.85).aspx
		//     -> Says "Default filter is Min_Mag_Mip_Linear"
		//   -> When testing the behaviour, it "looks like" Min_Mag_Mip_Linear is used

		// Direct3D 9 "D3DSAMPLERSTATETYPE enumeration"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb172602%28v=vs.85%29.aspx

		// OpenGL & OpenGL ES 2: The official specifications (unlike Direct3D, OpenGL versions are more compatible to each other)

		// Return default values
		static const SamplerState SAMPLER_STATE =
		{																					//	Direct3D 11					Direct3D 10						Direct3D 9				OpenGL
			FilterMode::MIN_MAG_MIP_LINEAR,	// filter (Renderer::FilterMode)				"MIN_MAG_MIP_LINEAR"			"MIN_MAG_MIP_LINEAR"			"MIN_MAG_MIP_POINT"		"MIN_POINT_MAG_MIP_LINEAR"
			TextureAddressMode::CLAMP,		// addressU (Renderer::TextureAddressMode)		"CLAMP"							"CLAMP"							"WRAP"					"WRAP"
			TextureAddressMode::CLAMP,		// addressV (Renderer::TextureAddressMode)		"CLAMP"							"CLAMP"							"WRAP"					"WRAP"
			TextureAddressMode::CLAMP,		// addressW (Renderer::TextureAddressMode)		"CLAMP"							"CLAMP"							"WRAP"					"WRAP"
			0.0f,							// mipLODBias (float)							"0.0f"							"0.0f"							"0.0f"					"0.0f"
			16,								// maxAnisotropy (uint32_t)						"16"							"16"							"1"						"1"
			ComparisonFunc::NEVER,			// comparisonFunc (Renderer::ComparisonFunc)	"NEVER"							"NEVER"							<unsupported>			"LESS_EQUAL"
			{
				0.0f,						// borderColor[0] (float)						"0.0f"							"0.0f"							"0.0f"					"0.0f"
				0.0f,						// borderColor[1] (float)						"0.0f"							"0.0f"							"0.0f"					"0.0f"
				0.0f,						// borderColor[2] (float)						"0.0f"							"0.0f"							"0.0f"					"0.0f"
				0.0f						// borderColor[3] (float)						"0.0f"							"0.0f"							"0.0f"					"0.0f"
			},
			-3.402823466e+38f,				// minLOD (float) - Default: -FLT_MAX			"-3.402823466e+38F (-FLT_MAX)"	"-3.402823466e+38F (-FLT_MAX)"	<unsupported>			"-1000.0f"
			3.402823466e+38f				// maxLOD (float) - Default: FLT_MAX			"3.402823466e+38F (FLT_MAX)"	"3.402823466e+38F (FLT_MAX)"	"0.0f"					"1000.0f"
		};
		return SAMPLER_STATE;
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline ISamplerState::~ISamplerState()
	{
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			--getRenderer().getStatistics().currentNumberOfSamplerStates;
		#endif
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline ISamplerState::ISamplerState(IRenderer& renderer) :
		IState(ResourceType::SAMPLER_STATE, renderer)
	{
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			++getRenderer().getStatistics().numberOfCreatedSamplerStates;
			++getRenderer().getStatistics().currentNumberOfSamplerStates;
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
