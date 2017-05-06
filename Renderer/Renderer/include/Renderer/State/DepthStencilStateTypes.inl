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
	inline const DepthStencilState& DepthStencilStateBuilder::getDefaultDepthStencilState()
	{
		// As default values, the one of Direct3D 11 and Direct 10 were chosen in order to make it easier for those renderer implementations
		// (choosing OpenGL default values would bring no benefit due to the design of the OpenGL API)
		// - Direct3D 11 "D3D11_DEPTH_STENCIL_DESC structure"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476110%28v=vs.85%29.aspx
		// - Direct3D 10 "D3D10_DEPTH_STENCIL_DESC structure"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb205036%28v=vs.85%29.aspx
		// - Direct3D 9 "D3DRENDERSTATETYPE enumeration"-documentation at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb172599%28v=vs.85%29.aspx
		// - OpenGL & OpenGL ES 2: The official specifications (unlike Direct3D, OpenGL versions are more compatible to each other)

		// Return default values
		static const DepthStencilState DEPTH_STENCIL_STATE =
		{																				//	Direct3D 11		Direct3D 10	Direct3D 9				OpenGL
			true,						// depthEnable (int)							"true"			"true"		"true"					TODO(co)
			DepthWriteMask::ALL,		// depthWriteMask (Renderer::DepthWriteMask)	"ALL"			"ALL"		"ALL"					TODO(co)
			ComparisonFunc::LESS,		// depthFunc (Renderer::ComparisonFunc)			"LESS"			"LESS"		"LESS_EQUAL"			TODO(co)
			false,						// stencilEnable (int)							"false"			"false"		"false"					TODO(co)
			0xff,						// stencilReadMask (uint8_t)					"0xff"			"0xff"		"0xffffffff"			TODO(co)
			0xff,						// stencilWriteMask (uint8_t)					"0xff"			"0xff"		"0xffffffff"			TODO(co)
			{ // sFrontFace (Renderer::DepthStencilOpDesc)
				StencilOp::KEEP,		// stencilFailOp (Renderer::StencilOp			"KEEP"			"KEEP"		"KEEP" (both sides)		TODO(co)
				StencilOp::KEEP,		// stencilDepthFailOp (Renderer::StencilOp)		"KEEP"			"KEEP"		"KEEP" (both sides)		TODO(co)
				StencilOp::KEEP,		// stencilPassOp (Renderer::StencilOp)			"KEEP"			"KEEP"		"KEEP" (both sides)		TODO(co)
				ComparisonFunc::ALWAYS	// stencilFunc (Renderer::ComparisonFunc)		"ALWAYS"		"ALWAYS"	"ALWAYS" (both sides)
			},
			{ // sBackFace (Renderer::DepthStencilOpDesc)
				StencilOp::KEEP,		// stencilFailOp (Renderer::StencilOp)			"KEEP"			"KEEP"		"KEEP" (both sides)		TODO(co)
				StencilOp::KEEP,		// stencilDepthFailOp (Renderer::StencilOp)		"KEEP"			"KEEP"		"KEEP" (both sides)		TODO(co)
				StencilOp::KEEP,		// stencilPassOp (Renderer::StencilOp)			"KEEP"			"KEEP"		"KEEP" (both sides)		TODO(co)
				ComparisonFunc::ALWAYS	// stencilFunc (Renderer::ComparisonFunc)		"ALWAYS"		"ALWAYS"	"ALWAYS" (both sides)	TODO(co)
			}
		};
		return DEPTH_STENCIL_STATE;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
