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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Shader/Separate/VertexShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/ShaderLanguageSeparate.h"
#include "OpenGLRenderer/Shader/Monolithic/ShaderLanguageMonolithic.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/Extensions.h"

#include <Renderer/ILog.h>
#include <Renderer/IAllocator.h>
#include <Renderer/Buffer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void printOpenGLShaderProgramInformationIntoLog(OpenGLRenderer::OpenGLRenderer& openGLRenderer, GLuint openGLObject)
		{
			// Get the length of the information (including a null termination)
			GLint informationLength = 0;
			OpenGLRenderer::glGetObjectParameterivARB(openGLObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &informationLength);
			if (informationLength > 1)
			{
				// Allocate memory for the information
				const Renderer::Context& context = openGLRenderer.getContext();
				char* informationLog = RENDERER_MALLOC_TYPED(context, char, informationLength);

				// Get the information
				OpenGLRenderer::glGetInfoLogARB(openGLObject, informationLength, nullptr, informationLog);

				// Output the debug string
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, informationLog)

				// Cleanup information memory
				RENDERER_FREE(context, informationLog);
			}
		}

		// Basing on the implementation from https://www.opengl.org/registry/specs/ARB/separate_shader_objects.txt
		GLuint createShaderProgramObject(OpenGLRenderer::OpenGLRenderer& openGLRenderer, GLuint openGLShader, const Renderer::VertexAttributes& vertexAttributes)
		{
			if (openGLShader > 0)
			{
				// Create the OpenGL program
				const GLuint openGLProgram = OpenGLRenderer::glCreateProgramObjectARB();
				if (openGLProgram > 0)
				{
					OpenGLRenderer::glProgramParameteri(openGLProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);

					// Attach the shader to the program
					OpenGLRenderer::glAttachObjectARB(openGLProgram, openGLShader);

					// Define the vertex array attribute binding locations ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 & 12 terminology)
					// -> Crucial code that glCreateShaderProgram doesn't do
					{
						const uint32_t numberOfVertexAttributes = vertexAttributes.numberOfAttributes;
						for (uint32_t vertexAttribute = 0; vertexAttribute < numberOfVertexAttributes; ++vertexAttribute)
						{
							OpenGLRenderer::glBindAttribLocationARB(openGLProgram, vertexAttribute, vertexAttributes.attributes[vertexAttribute].name);
						}
					}

					// Link the program
					OpenGLRenderer::glLinkProgramARB(openGLProgram);

					// Detach the shader from the program
					OpenGLRenderer::glDetachObjectARB(openGLProgram, openGLShader);
				}

				// Destroy the OpenGL shader
				OpenGLRenderer::glDeleteObjectARB(openGLShader);

				// Check the link status
				if (openGLProgram > 0)
				{
					GLint linked = GL_FALSE;
					OpenGLRenderer::glGetObjectParameterivARB(openGLProgram, GL_OBJECT_LINK_STATUS_ARB, &linked);
					if (GL_TRUE == linked)
					{
						// Done
						return openGLProgram;
					}
					else
					{
						// Error, program link failed!
						printOpenGLShaderProgramInformationIntoLog(openGLRenderer, openGLProgram);
					}
				}
			}

			// Error!
			return 0;
		}

		GLuint loadShaderProgramFromBytecode(OpenGLRenderer::OpenGLRenderer& openGLRenderer, const Renderer::VertexAttributes& vertexAttributes, GLenum shaderType, const Renderer::ShaderBytecode& shaderBytecode)
		{
			// Create and load the shader object
			const GLuint openGLShader = OpenGLRenderer::ShaderLanguageSeparate::loadShaderFromBytecode(openGLRenderer, shaderType, shaderBytecode);

			// Specialize the shader
			// -> Before this shader the isn't compiled, after this shader is supposed to be compiled
			OpenGLRenderer::glSpecializeShaderARB(openGLShader, "main", 0, nullptr, nullptr);

			// Check the compile status
			GLint compiled = GL_FALSE;
			OpenGLRenderer::glGetObjectParameterivARB(openGLShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
			if (GL_TRUE == compiled)
			{
				// All went fine, create and return the program
				return createShaderProgramObject(openGLRenderer, openGLShader, vertexAttributes);
			}
			else
			{
				// Error, failed to compile the shader!
				printOpenGLShaderProgramInformationIntoLog(openGLRenderer, openGLShader);

				// Destroy the OpenGL shader
				// -> A value of 0 for shader will be silently ignored
				OpenGLRenderer::glDeleteObjectARB(openGLShader);

				// Error!
				return 0;
			}
		}

		GLuint loadShaderProgramFromSourcecode(OpenGLRenderer::OpenGLRenderer& openGLRenderer, const Renderer::VertexAttributes& vertexAttributes, GLenum type, const char* sourceCode)
		{
			return createShaderProgramObject(openGLRenderer, OpenGLRenderer::ShaderLanguageMonolithic::loadShaderFromSourcecode(openGLRenderer, type, sourceCode), vertexAttributes);
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexShaderSeparate::VertexShaderSeparate(OpenGLRenderer& openGLRenderer, const Renderer::VertexAttributes& vertexAttributes, const Renderer::ShaderBytecode& shaderBytecode) :
		IVertexShader(static_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShaderProgram(::detail::loadShaderProgramFromBytecode(openGLRenderer, vertexAttributes, GL_VERTEX_SHADER_ARB, shaderBytecode)),
		mDrawIdUniformLocation(openGLRenderer.getExtensions().isGL_ARB_base_instance() ? -1 : glGetUniformLocationARB(mOpenGLShaderProgram, "drawIdUniform"))
	{
		// Nothing here
	}

	VertexShaderSeparate::VertexShaderSeparate(OpenGLRenderer& openGLRenderer, const Renderer::VertexAttributes& vertexAttributes, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode) :
		IVertexShader(static_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShaderProgram(::detail::loadShaderProgramFromSourcecode(openGLRenderer, vertexAttributes, GL_VERTEX_SHADER_ARB, sourceCode)),
		mDrawIdUniformLocation(openGLRenderer.getExtensions().isGL_ARB_base_instance() ? -1 : glGetUniformLocationARB(mOpenGLShaderProgram, "drawIdUniform"))
	{
		// Return shader bytecode, if requested do to so
		if (nullptr != shaderBytecode)
		{
			ShaderLanguageSeparate::shaderSourceCodeToShaderBytecode(openGLRenderer, GL_VERTEX_SHADER_ARB, sourceCode, *shaderBytecode);
		}
	}

	VertexShaderSeparate::~VertexShaderSeparate()
	{
		// Destroy the OpenGL shader program
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteProgram(mOpenGLShaderProgram);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void VertexShaderSeparate::setDebugName(const char* name)
		{
			// Valid OpenGL shader program and "GL_KHR_debug"-extension available?
			if (0 != mOpenGLShaderProgram && static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_KHR_debug())
			{
				glObjectLabel(GL_PROGRAM, mOpenGLShaderProgram, -1, name);
			}
		}
	#else
		void VertexShaderSeparate::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* VertexShaderSeparate::getShaderLanguageName() const
	{
		return ShaderLanguageSeparate::NAME;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void VertexShaderSeparate::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), VertexShaderSeparate, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
