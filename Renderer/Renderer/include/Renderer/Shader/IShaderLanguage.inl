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
	inline IShaderLanguage::~IShaderLanguage()
	{
		// Nothing here
	}

	inline IRenderer& IShaderLanguage::getRenderer() const
	{
		return *mRenderer;
	}

	inline IShaderLanguage::OptimizationLevel IShaderLanguage::getOptimizationLevel() const
	{
		return mOptimizationLevel;
	}

	inline void IShaderLanguage::setOptimizationLevel(OptimizationLevel optimizationLevel)
	{
		mOptimizationLevel = optimizationLevel;
	}

	inline IProgram* IShaderLanguage::createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, IFragmentShader* fragmentShader)
	{
		return createProgram(rootSignature, vertexAttributes, vertexShader, nullptr, nullptr, nullptr, fragmentShader);
	}

	inline IProgram* IShaderLanguage::createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, IGeometryShader* geometryShader, IFragmentShader* fragmentShader)
	{
		return createProgram(rootSignature, vertexAttributes, vertexShader, nullptr, nullptr, geometryShader, fragmentShader);
	}

	inline IProgram* IShaderLanguage::createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, ITessellationControlShader* tessellationControlShader, ITessellationEvaluationShader* tessellationEvaluationShader, IFragmentShader* fragmentShader)
	{
		return createProgram(rootSignature, vertexAttributes, vertexShader, tessellationControlShader, tessellationEvaluationShader, nullptr, fragmentShader);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline IShaderLanguage::IShaderLanguage(IRenderer& renderer) :
		mRenderer(&renderer),
		mOptimizationLevel(OptimizationLevel::Ultra)
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
