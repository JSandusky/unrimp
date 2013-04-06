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
//[ Includes                                              ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_STATISTICS
	#include "Renderer/IRenderer.h"
#endif
#include "Renderer/RasterizerStateTypes.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	inline const RasterizerState &IRasterizerState::getDefaultRasterizerState()
	{
		// As default values, the one of Direct3D 11 and Direct 10 were chosen in order to make it easier for those renderer implementations
		// (choosing OpenGL default values would bring no benefit due to the design of the OpenGL API)

		// Direct3D 11 "D3D11_RASTERIZER_DESC structure"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476198%28v=vs.85%29.aspx

		// Direct3D 10 "D3D10_RASTERIZER_DESC structure"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb172408(v=vs.85).aspx

		// Direct3D 9 "D3DRENDERSTATETYPE enumeration"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb172599%28v=vs.85%29.aspx

		// OpenGL & OpenGL ES 2: The official specifications (unlike Direct3D, OpenGL versions are more compatible to each other)

		// Return default values
		static const Renderer::RasterizerState RASTERIZER_STATE =
		{																		//	Direct3D 11	Direct3D 10	Direct3D 9		OpenGL
			Renderer::FillMode::SOLID,	// fillMode (Renderer::FillMode::Enum)	"SOLID"			"SOLID"		"SOLID"			"SOLID"
			Renderer::CullMode::BACK,	// cullMode (Renderer::CullMode::Enum)	"BACK"			"Back"		"BACK" (CCW)	"BACK"
			false,						// frontCounterClockwise (int)			"false"			"false"		"false" (CCW)	"true"
			0,							// depthBias (int)						"0"				"0"			"0"				TODO(co)
			0.0f,						// depthBiasClamp (float)				"0.0f"			"0.0f"		<unsupported>	TODO(co)
			0.0f,						// slopeScaledDepthBias (float)			"0.0f"			"0.0f"		"0.0f"			TODO(co)
			true,						// depthClipEnable (int)				"true"			"true"		<unsupported>	TODO(co)
			false,						// scissorEnable (int)					"false"			"false"		"false"			"false"
			false,						// multisampleEnable (int)				"false"			"false"		"true"			"true"
			false						// antialiasedLineEnable (int)			"false"			"false"		"false"			"false"
		};
		return RASTERIZER_STATE;
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IRasterizerState::~IRasterizerState()
	{
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			--getRenderer().getStatistics().currentNumberOfRasterizerStates;
		#endif
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline IRasterizerState::IRasterizerState(IRenderer &renderer) :
		IState(ResourceType::RASTERIZER_STATE, renderer)
	{
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			++getRenderer().getStatistics().numberOfCreatedRasterizerStates;
			++getRenderer().getStatistics().currentNumberOfRasterizerStates;
		#endif
	}

	inline IRasterizerState::IRasterizerState(const IRasterizerState &source) :
		IState(source)
	{
		// Not supported
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			++getRenderer().getStatistics().numberOfCreatedRasterizerStates;
			++getRenderer().getStatistics().currentNumberOfRasterizerStates;
		#endif
	}

	inline IRasterizerState &IRasterizerState::operator =(const IRasterizerState &)
	{
		// Not supported
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
