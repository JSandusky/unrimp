/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
	inline PipelineStateBuilder::PipelineStateBuilder()
	{
		rootSignature						= nullptr;
		program								= nullptr;
		vertexAttributes.numberOfAttributes	= 0;
		vertexAttributes.attributes			= nullptr;
		primitiveTopologyType				= PrimitiveTopologyType::TRIANGLE;
		rasterizerState						= RasterizerStateBuilder::getDefaultRasterizerState();
		depthStencilState					= DepthStencilStateBuilder::getDefaultDepthStencilState();
		blendState							= BlendStateBuilder::getDefaultBlendState();
		numberOfRenderTargets				= 1;
		renderTargetViewFormats[0]			= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[1]			= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[2]			= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[3]			= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[4]			= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[5]			= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[6]			= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[7]			= TextureFormat::R8G8B8A8;
		depthStencilViewFormat				= TextureFormat::D32_FLOAT;
	}

	inline PipelineStateBuilder::PipelineStateBuilder(IRootSignature* _rootSignature, IProgram* _program, const VertexAttributes& _vertexAttributes)
	{
		rootSignature				= _rootSignature;
		program						= _program;
		vertexAttributes			= _vertexAttributes;
		primitiveTopologyType		= PrimitiveTopologyType::TRIANGLE;
		rasterizerState				= RasterizerStateBuilder::getDefaultRasterizerState();
		depthStencilState			= DepthStencilStateBuilder::getDefaultDepthStencilState();
		blendState					= BlendStateBuilder::getDefaultBlendState();
		numberOfRenderTargets		= 1;
		renderTargetViewFormats[0]	= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[1]	= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[2]	= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[3]	= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[4]	= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[5]	= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[6]	= TextureFormat::R8G8B8A8;
		renderTargetViewFormats[7]	= TextureFormat::R8G8B8A8;
		depthStencilViewFormat		= TextureFormat::D32_FLOAT;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
