/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace NullRenderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguage::NAME = "Null";


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguage::ShaderLanguage(NullRenderer &nullRenderer) :
		IShaderLanguage(reinterpret_cast<Renderer::IRenderer&>(nullRenderer))
	{
		// Nothing to do in here
	}

	ShaderLanguage::~ShaderLanguage()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char *ShaderLanguage::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader *ShaderLanguage::createVertexShaderFromBytecode(const Renderer::VertexAttributes&, const uint8_t *, uint32_t)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShader(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IVertexShader *ShaderLanguage::createVertexShaderFromSourceCode(const Renderer::VertexAttributes&, const char *, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShader(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::ITessellationControlShader *ShaderLanguage::createTessellationControlShaderFromBytecode(const uint8_t *, uint32_t)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfPatchVertices", we know there's tessellation control shader support
		return new TessellationControlShader(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::ITessellationControlShader *ShaderLanguage::createTessellationControlShaderFromSourceCode(const char *, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfPatchVertices", we know there's tessellation control shader support
		return new TessellationControlShader(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguage::createTessellationEvaluationShaderFromBytecode(const uint8_t *, uint32_t)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfPatchVertices", we know there's tessellation evaluation shader support
		return new TessellationEvaluationShader(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguage::createTessellationEvaluationShaderFromSourceCode(const char *, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfPatchVertices", we know there's tessellation evaluation shader support
		return new TessellationEvaluationShader(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IGeometryShader *ShaderLanguage::createGeometryShaderFromBytecode(const uint8_t *, uint32_t, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfGsOutputVertices", we know there's geometry shader support
		return new GeometryShader(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IGeometryShader *ShaderLanguage::createGeometryShaderFromSourceCode(const char *, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::maximumNumberOfGsOutputVertices", we know there's geometry shader support
		return new GeometryShader(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IFragmentShader *ShaderLanguage::createFragmentShaderFromBytecode(const uint8_t *, uint32_t)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShader(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IFragmentShader *ShaderLanguage::createFragmentShaderFromSourceCode(const char *, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShader(reinterpret_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IProgram *ShaderLanguage::createProgram(const Renderer::IRootSignature&, const Renderer::VertexAttributes&, Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader)
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
			return new Program(reinterpret_cast<NullRenderer&>(getRenderer()), static_cast<VertexShader*>(vertexShader), static_cast<TessellationControlShader*>(tessellationControlShader), static_cast<TessellationEvaluationShader*>(tessellationEvaluationShader), static_cast<GeometryShader*>(geometryShader), static_cast<FragmentShader*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->release();
		}
		if (nullptr != tessellationControlShader)
		{
			tessellationControlShader->addReference();
			tessellationControlShader->release();
		}
		if (nullptr != tessellationEvaluationShader)
		{
			tessellationEvaluationShader->addReference();
			tessellationEvaluationShader->release();
		}
		if (nullptr != geometryShader)
		{
			geometryShader->addReference();
			geometryShader->release();
		}
		if (nullptr != fragmentShader)
		{
			fragmentShader->addReference();
			fragmentShader->release();
		}

		// Error!
		return nullptr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // NullRenderer
