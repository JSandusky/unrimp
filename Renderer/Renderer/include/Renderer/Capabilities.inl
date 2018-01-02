/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline Capabilities::Capabilities() :
		deviceName{},
		preferredSwapChainColorTextureFormat(TextureFormat::Enum::UNKNOWN),
		preferredSwapChainDepthStencilTextureFormat(TextureFormat::Enum::UNKNOWN),
		maximumNumberOfViewports(0),
		maximumNumberOfSimultaneousRenderTargets(0),
		maximumTextureDimension(0),
		maximumNumberOf2DTextureArraySlices(0),
		maximumUniformBufferSize(0),
		maximumTextureBufferSize(0),
		maximumIndirectBufferSize(0),
		maximumNumberOfMultisamples(1),
		maximumAnisotropy(1),
		upperLeftOrigin(true),
		zeroToOneClipZ(true),
		individualUniforms(false),
		instancedArrays(false),
		drawInstanced(false),
		baseVertex(false),
		nativeMultiThreading(false),
		shaderBytecode(false),
		vertexShader(false),
		maximumNumberOfPatchVertices(0),
		maximumNumberOfGsOutputVertices(0),
		fragmentShader(false)
	{
		// Nothing here
	}

	inline Capabilities::~Capabilities()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
