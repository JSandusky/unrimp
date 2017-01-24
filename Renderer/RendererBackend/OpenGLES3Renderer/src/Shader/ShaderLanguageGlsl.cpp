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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageGlsl::NAME = "GLSLES";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t ShaderLanguageGlsl::loadShader(uint32_t shaderType, const char *shaderSource)
	{
		// Create the shader object
		const GLuint openGLES2Shader = glCreateShader(shaderType);

		// Load the shader source
		glShaderSource(openGLES2Shader, 1, &shaderSource, nullptr);

		// Compile the shader
		glCompileShader(openGLES2Shader);

		// Check the compile status
		GLint compiled = GL_FALSE;
		glGetShaderiv(openGLES2Shader, GL_COMPILE_STATUS, &compiled);
		if (GL_TRUE == compiled)
		{
			// All went fine, return the shader
			return openGLES2Shader;
		}
		else
		{
			// Error, failed to compile the shader!
			#ifdef RENDERER_OUTPUT_DEBUG
				// Get the length of the information
				GLint informationLength = 0;
				glGetShaderiv(openGLES2Shader, GL_INFO_LOG_LENGTH, &informationLength);
				if (informationLength > 1)
				{
					// Allocate memory for the information
					GLchar *informationLog = new GLchar[static_cast<uint32_t>(informationLength)];

					// Get the information
					glGetShaderInfoLog(openGLES2Shader, informationLength, nullptr, informationLog);

					// Output the debug string
					RENDERER_OUTPUT_DEBUG_STRING(informationLog)

					// Cleanup information memory
					delete [] informationLog;
				}
			#endif

			// Destroy the shader
			// -> A value of 0 for shader will be silently ignored
			glDeleteShader(openGLES2Shader);

			// Error!
			return 0;
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageGlsl::ShaderLanguageGlsl(OpenGLES3Renderer &openGLES3Renderer) :
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
	const char *ShaderLanguageGlsl::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader *ShaderLanguageGlsl::createVertexShaderFromBytecode(const Renderer::VertexAttributes&, const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShaderGlsl(static_cast<OpenGLES3Renderer&>(getRenderer()), bytecode, numberOfBytes);
	}

	Renderer::IVertexShader *ShaderLanguageGlsl::createVertexShaderFromSourceCode(const Renderer::VertexAttributes&, const char *sourceCode, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::vertexShader", we know there's vertex shader support
		return new VertexShaderGlsl(static_cast<OpenGLES3Renderer&>(getRenderer()), sourceCode);
	}

	Renderer::ITessellationControlShader *ShaderLanguageGlsl::createTessellationControlShaderFromBytecode(const uint8_t *, uint32_t )
	{
		// Error! OpenGL ES 3 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationControlShader *ShaderLanguageGlsl::createTessellationControlShaderFromSourceCode(const char *, const char *, const char *, const char *)
	{
		// Error! OpenGL ES 3 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageGlsl::createTessellationEvaluationShaderFromBytecode(const uint8_t *, uint32_t )
	{
		// Error! OpenGL ES 3 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageGlsl::createTessellationEvaluationShaderFromSourceCode(const char *, const char *, const char *, const char *)
	{
		// Error! OpenGL ES 3 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::IGeometryShader *ShaderLanguageGlsl::createGeometryShaderFromBytecode(const uint8_t *, uint32_t, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t, const char *, const char *, const char *)
	{
		// Error! OpenGL ES 3 has no geometry shader support.
		return nullptr;
	}

	Renderer::IGeometryShader *ShaderLanguageGlsl::createGeometryShaderFromSourceCode(const char *, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t, const char *, const char *, const char *)
	{
		// Error! OpenGL ES 3 has no geometry shader support.
		return nullptr;
	}

	Renderer::IFragmentShader *ShaderLanguageGlsl::createFragmentShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShaderGlsl(static_cast<OpenGLES3Renderer&>(getRenderer()), bytecode, numberOfBytes);
	}

	Renderer::IFragmentShader *ShaderLanguageGlsl::createFragmentShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		// There's no need to check for "Renderer::Capabilities::fragmentShader", we know there's fragment shader support
		return new FragmentShaderGlsl(static_cast<OpenGLES3Renderer&>(getRenderer()), sourceCode);
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
			return new ProgramGlsl(static_cast<OpenGLES3Renderer&>(getRenderer()), rootSignature, vertexAttributes, static_cast<VertexShaderGlsl*>(vertexShader), static_cast<FragmentShaderGlsl*>(fragmentShader));
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
