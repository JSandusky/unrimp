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
#include "Renderer/ILog.h"
#include "Renderer/Context.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline Statistics::Statistics() :
		currentNumberOfRootSignatures(0),
		numberOfCreatedRootSignatures(0),
		currentNumberOfPrograms(0),
		numberOfCreatedPrograms(0),
		currentNumberOfVertexArrays(0),
		numberOfCreatedVertexArrays(0),
		// IRenderTarget
		currentNumberOfSwapChains(0),
		numberOfCreatedSwapChains(0),
		currentNumberOfFramebuffers(0),
		numberOfCreatedFramebuffers(0),
		// IBuffer
		currentNumberOfIndexBuffers(0),
		numberOfCreatedIndexBuffers(0),
		currentNumberOfVertexBuffers(0),
		numberOfCreatedVertexBuffers(0),
		currentNumberOfUniformBuffers(0),
		numberOfCreatedUniformBuffers(0),
		currentNumberOfTextureBuffers(0),
		numberOfCreatedTextureBuffers(0),
		currentNumberOfIndirectBuffers(0),
		numberOfCreatedIndirectBuffers(0),
		// ITexture
		currentNumberOfTexture1Ds(0),
		numberOfCreatedTexture1Ds(0),
		currentNumberOfTexture2Ds(0),
		numberOfCreatedTexture2Ds(0),
		currentNumberOfTexture2DArrays(0),
		numberOfCreatedTexture2DArrays(0),
		currentNumberOfTexture3Ds(0),
		numberOfCreatedTexture3Ds(0),
		currentNumberOfTextureCubes(0),
		numberOfCreatedTextureCubes(0),
		// IState
		currentNumberOfPipelineStates(0),
		numberOfCreatedPipelineStates(0),
		currentNumberOfSamplerStates(0),
		numberOfCreatedSamplerStates(0),
		// IShader
		currentNumberOfVertexShaders(0),
		numberOfCreatedVertexShaders(0),
		currentNumberOfTessellationControlShaders(0),
		numberOfCreatedTessellationControlShaders(0),
		currentNumberOfTessellationEvaluationShaders(0),
		numberOfCreatedTessellationEvaluationShaders(0),
		currentNumberOfGeometryShaders(0),
		numberOfCreatedGeometryShaders(0),
		currentNumberOfFragmentShaders(0),
		numberOfCreatedFragmentShaders(0)
	{
		// Nothing here
	}

	inline Statistics::~Statistics()
	{
		// Nothing here
	}

	inline uint32_t Statistics::getNumberOfCurrentResources() const
	{
		// Calculate the current number of resource instances
		return	currentNumberOfRootSignatures +
				currentNumberOfPrograms +
				currentNumberOfVertexArrays +
				// IRenderTarget
				currentNumberOfSwapChains +
				currentNumberOfFramebuffers +
				// IBuffer
				currentNumberOfIndexBuffers +
				currentNumberOfVertexBuffers +
				currentNumberOfUniformBuffers +
				currentNumberOfTextureBuffers +
				currentNumberOfIndirectBuffers +
				// ITexture
				currentNumberOfTexture1Ds +
				currentNumberOfTexture2Ds +
				currentNumberOfTexture2DArrays +
				currentNumberOfTexture3Ds +
				currentNumberOfTextureCubes +
				// IState
				currentNumberOfPipelineStates +
				currentNumberOfSamplerStates +
				// IShader
				currentNumberOfVertexShaders +
				currentNumberOfTessellationControlShaders +
				currentNumberOfTessellationEvaluationShaders +
				currentNumberOfGeometryShaders +
				currentNumberOfFragmentShaders;
	}

	inline void Statistics::debugOutputCurrentResouces(const Context& context) const
	{
		// Start
		RENDERER_LOG(context, INFORMATION, "** Number of current renderer resource instances **")

		// Misc
		RENDERER_LOG(context, INFORMATION, "Root signatures: %d", currentNumberOfRootSignatures.load())
		RENDERER_LOG(context, INFORMATION, "Programs: %d", currentNumberOfPrograms.load())
		RENDERER_LOG(context, INFORMATION, "Vertex arrays: %d", currentNumberOfVertexArrays.load())

		// IRenderTarget
		RENDERER_LOG(context, INFORMATION, "Swap chains: %d", currentNumberOfSwapChains.load())
		RENDERER_LOG(context, INFORMATION, "Framebuffers: %d", currentNumberOfFramebuffers.load())

		// IBuffer
		RENDERER_LOG(context, INFORMATION, "Index buffers: %d", currentNumberOfIndexBuffers.load())
		RENDERER_LOG(context, INFORMATION, "Vertex buffers: %d", currentNumberOfVertexBuffers.load())
		RENDERER_LOG(context, INFORMATION, "Uniform buffers: %d", currentNumberOfUniformBuffers.load())
		RENDERER_LOG(context, INFORMATION, "Texture buffers: %d", currentNumberOfTextureBuffers.load())
		RENDERER_LOG(context, INFORMATION, "Indirect buffers: %d", currentNumberOfIndirectBuffers.load())

		// ITexture
		RENDERER_LOG(context, INFORMATION, "1D textures: %d", currentNumberOfTexture1Ds.load())
		RENDERER_LOG(context, INFORMATION, "2D textures: %d", currentNumberOfTexture2Ds.load())
		RENDERER_LOG(context, INFORMATION, "2D texture arrays: %d", currentNumberOfTexture2DArrays.load())
		RENDERER_LOG(context, INFORMATION, "3D textures: %d", currentNumberOfTexture3Ds.load())
		RENDERER_LOG(context, INFORMATION, "Cube textures: %d", currentNumberOfTextureCubes.load())

		// IState
		RENDERER_LOG(context, INFORMATION, "Pipeline states: %d", currentNumberOfPipelineStates.load())
		RENDERER_LOG(context, INFORMATION, "Sampler states: %d", currentNumberOfSamplerStates.load())

		// IShader
		RENDERER_LOG(context, INFORMATION, "Vertex shaders: %d", currentNumberOfVertexShaders.load())
		RENDERER_LOG(context, INFORMATION, "Tessellation control shaders: %d", currentNumberOfTessellationControlShaders.load())
		RENDERER_LOG(context, INFORMATION, "Tessellation evaluation shaders: %d", currentNumberOfTessellationEvaluationShaders.load())
		RENDERER_LOG(context, INFORMATION, "Geometry shaders: %d", currentNumberOfGeometryShaders.load())
		RENDERER_LOG(context, INFORMATION, "Fragment shaders: %d", currentNumberOfFragmentShaders.load())

		// End
		RENDERER_LOG(context, INFORMATION, "***************************************************")
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
