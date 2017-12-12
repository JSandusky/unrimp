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
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"	// Must be included before "OpenGLES3Renderer/Shader/ShaderLanguageGlsl.h"
#include "OpenGLES3Renderer/Shader/ShaderLanguageGlsl.h"
#include "OpenGLES3Renderer/Shader/FragmentShaderGlsl.h"
#include "OpenGLES3Renderer/Shader/ProgramGlsl.h"
#include "OpenGLES3Renderer/Shader/VertexShaderGlsl.h"
#include "OpenGLES3Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 3 functions

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char* ShaderLanguageGlsl::NAME = "GLSLES";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t ShaderLanguageGlsl::loadShaderFromSourcecode(OpenGLES3Renderer& openGLES3Renderer, uint32_t shaderType, const char* sourceCode)
	{
		// Create the shader object
		const GLuint openGLES3Shader = glCreateShader(shaderType);

		// Load the shader source
		glShaderSource(openGLES3Shader, 1, &sourceCode, nullptr);

		// Compile the shader
		glCompileShader(openGLES3Shader);

		// Check the compile status
		GLint compiled = GL_FALSE;
		glGetShaderiv(openGLES3Shader, GL_COMPILE_STATUS, &compiled);
		if (GL_TRUE == compiled)
		{
			// All went fine, return the shader
			return openGLES3Shader;
		}
		else
		{
			// Error, failed to compile the shader!

			{ // Get the length of the information
				GLint informationLength = 0;
				glGetShaderiv(openGLES3Shader, GL_INFO_LOG_LENGTH, &informationLength);
				if (informationLength > 1)
				{
					// Allocate memory for the information
					const Renderer::Context& context = openGLES3Renderer.getContext();
					GLchar* informationLog = RENDERER_MALLOC_TYPED(context, GLchar, informationLength);

					// Get the information
					glGetShaderInfoLog(openGLES3Shader, informationLength, nullptr, informationLog);

					// Output the debug string
					if (openGLES3Renderer.getContext().getLog().print(Renderer::ILog::Type::CRITICAL, sourceCode, __FILE__, static_cast<uint32_t>(__LINE__), informationLog))
					{
						DEBUG_BREAK;
					}

					// Cleanup information memory
					RENDERER_FREE(context, informationLog);
				}
			}

			// Destroy the shader
			// -> A value of 0 for shader will be silently ignored
			glDeleteShader(openGLES3Shader);

			// Error!
			return 0;
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageGlsl::ShaderLanguageGlsl(OpenGLES3Renderer& openGLES3Renderer) :
		IShaderLanguage(openGLES3Renderer)
	{
		// Nothing here
	}

	ShaderLanguageGlsl::~ShaderLanguageGlsl()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char* ShaderLanguageGlsl::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader* ShaderLanguageGlsl::createVertexShaderFromBytecode(const Renderer::VertexAttributes&, const Renderer::ShaderBytecode&)
	{
		// Error!
		RENDERER_ASSERT(getRenderer().getContext(), false, "Monolithic shaders have no shader bytecode, only a monolithic program bytecode")
		return nullptr;
	}

	Renderer::IVertexShader* ShaderLanguageGlsl::createVertexShaderFromSourceCode(const Renderer::VertexAttributes&, const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode*)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		// -> Monolithic shaders have no shader bytecode, only a monolithic program bytecode
		return RENDERER_NEW(getRenderer().getContext(), VertexShaderGlsl)(static_cast<OpenGLES3Renderer&>(getRenderer()), shaderSourceCode.sourceCode);
	}

	Renderer::ITessellationControlShader* ShaderLanguageGlsl::createTessellationControlShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// Error!
		RENDERER_ASSERT(getRenderer().getContext(), false, "Monolithic shaders have no shader bytecode, only a monolithic program bytecode")
		return nullptr;
	}

	Renderer::ITessellationControlShader* ShaderLanguageGlsl::createTessellationControlShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::ShaderBytecode*)
	{
		// Error! OpenGL ES 3 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageGlsl::createTessellationEvaluationShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// Error!
		RENDERER_ASSERT(getRenderer().getContext(), false, "Monolithic shaders have no shader bytecode, only a monolithic program bytecode")
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageGlsl::createTessellationEvaluationShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::ShaderBytecode*)
	{
		// Error! OpenGL ES 3 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::IGeometryShader* ShaderLanguageGlsl::createGeometryShaderFromBytecode(const Renderer::ShaderBytecode&, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t)
	{
		// Error!
		RENDERER_ASSERT(getRenderer().getContext(), false, "Monolithic shaders have no shader bytecode, only a monolithic program bytecode")
		return nullptr;
	}

	Renderer::IGeometryShader* ShaderLanguageGlsl::createGeometryShaderFromSourceCode(const Renderer::ShaderSourceCode&, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t, Renderer::ShaderBytecode*)
	{
		// Error! OpenGL ES 3 has no geometry shader support.
		return nullptr;
	}

	Renderer::IFragmentShader* ShaderLanguageGlsl::createFragmentShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// Error!
		RENDERER_ASSERT(getRenderer().getContext(), false, "Monolithic shaders have no shader bytecode, only a monolithic program bytecode")
		return nullptr;
	}

	Renderer::IFragmentShader* ShaderLanguageGlsl::createFragmentShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode*)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		// -> Monolithic shaders have no shader bytecode, only a monolithic program bytecode
		return RENDERER_NEW(getRenderer().getContext(), FragmentShaderGlsl)(static_cast<OpenGLES3Renderer&>(getRenderer()), shaderSourceCode.sourceCode);
	}

	Renderer::IProgram* ShaderLanguageGlsl::createProgram(const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, Renderer::IVertexShader* vertexShader, Renderer::ITessellationControlShader* tessellationControlShader, Renderer::ITessellationEvaluationShader* tessellationEvaluationShader, Renderer::IGeometryShader* geometryShader, Renderer::IFragmentShader* fragmentShader)
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
			// Error! OpenGL ES 3 has no tessellation control shader support.
		}
		else if (nullptr != tessellationEvaluationShader)
		{
			// Error! OpenGL ES 3 has no tessellation evaluation shader support.
		}
		else if (nullptr != geometryShader)
		{
			// Error! OpenGL ES 3 has no geometry shader support.
		}
		else if (nullptr != fragmentShader && fragmentShader->getShaderLanguageName() != NAME)
		{
			// Error! Fragment shader language mismatch!
		}
		else
		{
			// Create the program
			return RENDERER_NEW(getRenderer().getContext(), ProgramGlsl)(static_cast<OpenGLES3Renderer&>(getRenderer()), rootSignature, vertexAttributes, static_cast<VertexShaderGlsl*>(vertexShader), static_cast<FragmentShaderGlsl*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->releaseReference();
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
} // OpenGLES3Renderer
