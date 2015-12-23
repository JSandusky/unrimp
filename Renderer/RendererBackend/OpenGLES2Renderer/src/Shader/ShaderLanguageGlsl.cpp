/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"	// Must be included before "OpenGLES2Renderer/Shader/ShaderLanguageGlsl.h"
#include "OpenGLES2Renderer/Shader/ShaderLanguageGlsl.h"
#include "OpenGLES2Renderer/Shader/FragmentShaderGlsl.h"
#include "OpenGLES2Renderer/Shader/ProgramGlsl.h"
#include "OpenGLES2Renderer/Shader/VertexShaderGlsl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageGlsl::NAME = "GLSL";


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageGlsl::ShaderLanguageGlsl(OpenGLES2Renderer &openGLES2Renderer) :
		ShaderLanguage(openGLES2Renderer)
	{
		// Nothing to do in here
	}

	ShaderLanguageGlsl::~ShaderLanguageGlsl()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageGlsl::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader *ShaderLanguageGlsl::createVertexShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShaderGlsl(getOpenGLES2Renderer(), bytecode, numberOfBytes);
	}

	Renderer::IVertexShader *ShaderLanguageGlsl::createVertexShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShaderGlsl(getOpenGLES2Renderer(), sourceCode);
	}

	Renderer::ITessellationControlShader *ShaderLanguageGlsl::createTessellationControlShaderFromBytecode(const uint8_t *, uint32_t )
	{
		// Error! OpenGL ES 2 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationControlShader *ShaderLanguageGlsl::createTessellationControlShaderFromSourceCode(const char *, const char *, const char *, const char *)
	{
		// Error! OpenGL ES 2 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageGlsl::createTessellationEvaluationShaderFromBytecode(const uint8_t *, uint32_t )
	{
		// Error! OpenGL ES 2 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageGlsl::createTessellationEvaluationShaderFromSourceCode(const char *, const char *, const char *, const char *)
	{
		// Error! OpenGL ES 2 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::IGeometryShader *ShaderLanguageGlsl::createGeometryShaderFromBytecode(const uint8_t *, uint32_t, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t, const char *, const char *, const char *)
	{
		// Error! OpenGL ES 2 has no geometry shader support.
		return nullptr;
	}

	Renderer::IGeometryShader *ShaderLanguageGlsl::createGeometryShaderFromSourceCode(const char *, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t, const char *, const char *, const char *)
	{
		// Error! OpenGL ES 2 has no geometry shader support.
		return nullptr;
	}

	Renderer::IFragmentShader *ShaderLanguageGlsl::createFragmentShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShaderGlsl(getOpenGLES2Renderer(), bytecode, numberOfBytes);
	}

	Renderer::IFragmentShader *ShaderLanguageGlsl::createFragmentShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShaderGlsl(getOpenGLES2Renderer(), sourceCode);
	}

	Renderer::IProgram *ShaderLanguageGlsl::createProgram(const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader)
	{
		// A shader can be a null pointer, but if it's not the shader and program language must match!
		// -> Optimization: Comparing the shader language name by directly comparing the pointer address of
		//    the name is safe because we know that we always reference to one and the same name address
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		if (nullptr != vertexShader && vertexShader->getShaderLanguageName() != NAME)
		{
			// Error! Vertex shader language mismatch!
		}
		else if (nullptr != tessellationControlShader)
		{
			// Error! OpenGL ES 2 has no tessellation control shader support.
		}
		else if (nullptr != tessellationEvaluationShader)
		{
			// Error! OpenGL ES 2 has no tessellation evaluation shader support.
		}
		else if (nullptr != geometryShader)
		{
			// Error! OpenGL ES 2 has no geometry shader support.
		}
		else if (nullptr != fragmentShader && fragmentShader->getShaderLanguageName() != NAME)
		{
			// Error! Fragment shader language mismatch!
		}
		else
		{
			// Create the program
			return new ProgramGlsl(getOpenGLES2Renderer(), rootSignature, vertexAttributes, static_cast<VertexShaderGlsl*>(vertexShader), static_cast<FragmentShaderGlsl*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->release();
		}
		if (nullptr != fragmentShader)
		{
			fragmentShader->addReference();
			fragmentShader->release();
		}

		// Error!
		return nullptr;
	}

	Renderer::IUniformBuffer *ShaderLanguageGlsl::createUniformBuffer(uint32_t, const void *, Renderer::BufferUsage)
	{
		// Error! OpenGL ES 2 has no uniform buffer support.
		return nullptr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
