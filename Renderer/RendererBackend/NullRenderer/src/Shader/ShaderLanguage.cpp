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
#include "NullRenderer/Shader/ShaderLanguage.h"
#include "NullRenderer/Shader/Program.h"
#include "NullRenderer/Shader/VertexShader.h"
#include "NullRenderer/Shader/GeometryShader.h"
#include "NullRenderer/Shader/FragmentShader.h"
#include "NullRenderer/Shader/TessellationControlShader.h"
#include "NullRenderer/Shader/TessellationEvaluationShader.h"

#include <Renderer/IRenderer.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace NullRenderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char* ShaderLanguage::NAME = "Null";


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguage::ShaderLanguage(NullRenderer& nullRenderer) :
		IShaderLanguage(reinterpret_cast<Renderer::IRenderer&>(nullRenderer))
	{
		// Nothing here
	}

	ShaderLanguage::~ShaderLanguage()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char* ShaderLanguage::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader* ShaderLanguage::createVertexShaderFromBytecode(const Renderer::VertexAttributes&, const Renderer::ShaderBytecode&)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return RENDERER_NEW(getRenderer().getContext(), VertexShader)(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IVertexShader* ShaderLanguage::createVertexShaderFromSourceCode(const Renderer::VertexAttributes&, const Renderer::ShaderSourceCode&, Renderer::ShaderBytecode*)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return RENDERER_NEW(getRenderer().getContext(), VertexShader)(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::ITessellationControlShader* ShaderLanguage::createTessellationControlShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfPatchVertices", we know there's tessellation control shader support
		return RENDERER_NEW(getRenderer().getContext(), TessellationControlShader)(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::ITessellationControlShader* ShaderLanguage::createTessellationControlShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::ShaderBytecode*)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfPatchVertices", we know there's tessellation control shader support
		return RENDERER_NEW(getRenderer().getContext(), TessellationControlShader)(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguage::createTessellationEvaluationShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfPatchVertices", we know there's tessellation evaluation shader support
		return RENDERER_NEW(getRenderer().getContext(), TessellationEvaluationShader)(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguage::createTessellationEvaluationShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::ShaderBytecode*)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfPatchVertices", we know there's tessellation evaluation shader support
		return RENDERER_NEW(getRenderer().getContext(), TessellationEvaluationShader)(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IGeometryShader* ShaderLanguage::createGeometryShaderFromBytecode(const Renderer::ShaderBytecode&, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfGsOutputVertices", we know there's geometry shader support
		return RENDERER_NEW(getRenderer().getContext(), GeometryShader)(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IGeometryShader* ShaderLanguage::createGeometryShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t, Renderer::ShaderBytecode*)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfGsOutputVertices", we know there's geometry shader support
		return RENDERER_NEW(getRenderer().getContext(), GeometryShader)(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IFragmentShader* ShaderLanguage::createFragmentShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return RENDERER_NEW(getRenderer().getContext(), FragmentShader)(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IFragmentShader* ShaderLanguage::createFragmentShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::ShaderBytecode*)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return RENDERER_NEW(getRenderer().getContext(), FragmentShader)(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IProgram* ShaderLanguage::createProgram(const Renderer::IRootSignature&, const Renderer::VertexAttributes&, Renderer::IVertexShader* vertexShader, Renderer::ITessellationControlShader* tessellationControlShader, Renderer::ITessellationEvaluationShader* tessellationEvaluationShader, Renderer::IGeometryShader* geometryShader, Renderer::IFragmentShader* fragmentShader)
	{
		// A shader can be a null pointer, but if it's not the shader and program language must match!
		// -> Optimization: Comparing the shader language name by directly comparing the pointer address of
		//    the name is safe because we know that we always reference to one and the same name address
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		if (nullptr != vertexShader && vertexShader->getShaderLanguageName() != NAME)
		{
			// Error! Vertex shader language mismatch!
		}
		else if (nullptr != tessellationControlShader && tessellationControlShader->getShaderLanguageName() != NAME)
		{
			// Error! Tessellation control shader language mismatch!
		}
		else if (nullptr != tessellationEvaluationShader && tessellationEvaluationShader->getShaderLanguageName() != NAME)
		{
			// Error! Tessellation evaluation shader language mismatch!
		}
		else if (nullptr != geometryShader && geometryShader->getShaderLanguageName() != NAME)
		{
			// Error! Geometry shader language mismatch!
		}
		else if (nullptr != fragmentShader && fragmentShader->getShaderLanguageName() != NAME)
		{
			// Error! Fragment shader language mismatch!
		}
		else
		{
			return RENDERER_NEW(getRenderer().getContext(), Program)(reinterpret_cast<NullRenderer&>(getRenderer()), static_cast<VertexShader*>(vertexShader), static_cast<TessellationControlShader*>(tessellationControlShader), static_cast<TessellationEvaluationShader*>(tessellationEvaluationShader), static_cast<GeometryShader*>(geometryShader), static_cast<FragmentShader*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->releaseReference();
		}
		if (nullptr != tessellationControlShader)
		{
			tessellationControlShader->addReference();
			tessellationControlShader->releaseReference();
		}
		if (nullptr != tessellationEvaluationShader)
		{
			tessellationEvaluationShader->addReference();
			tessellationEvaluationShader->releaseReference();
		}
		if (nullptr != geometryShader)
		{
			geometryShader->addReference();
			geometryShader->releaseReference();
		}
		if (nullptr != fragmentShader)
		{
			fragmentShader->addReference();
			fragmentShader->releaseReference();
		}

		// Error!
		return nullptr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // NullRenderer
