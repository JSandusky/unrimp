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
#include "OpenGLRenderer/Shader/Separate/VertexShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/ShaderLanguageSeparate.h"
#include "OpenGLRenderer/Shader/Monolithic/ShaderLanguageMonolithic.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#include "OpenGLRenderer/Extensions.h"

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
		// Basing on the implementation from https://www.opengl.org/registry/specs/ARB/separate_shader_objects.txt
		GLuint CreateOpenGLShaderProgram(const Renderer::VertexAttributes& vertexAttributes, GLenum type, const char* sourceCode)
		{
			// Create the OpenGL shader
			const GLuint openGLShader = OpenGLRenderer::ShaderLanguageMonolithic::loadShader(type, sourceCode);
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
						#ifdef RENDERER_OUTPUT_DEBUG
							// Get the length of the information (including a null termination)
							GLint informationLength = 0;
							OpenGLRenderer::glGetObjectParameterivARB(openGLProgram, GL_OBJECT_INFO_LOG_LENGTH_ARB, &informationLength);
							if (informationLength > 1)
							{
								// Allocate memory for the information
								char *informationLog = new char[static_cast<uint32_t>(informationLength)];

								// Get the information
								OpenGLRenderer::glGetInfoLogARB(openGLProgram, informationLength, nullptr, informationLog);

								// Output the debug string
								RENDERER_OUTPUT_DEBUG_STRING(informationLog)

								// Cleanup information memory
								delete [] informationLog;
							}
						#endif
					}
				}
			}

			// Error!
			return 0;
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
	VertexShaderSeparate::VertexShaderSeparate(OpenGLRenderer &openGLRenderer, const Renderer::VertexAttributes&, const Renderer::ShaderBytecode&) :
		IVertexShader(reinterpret_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShaderProgram(0)
	{
		// TODO(co) Implement me
	}

	VertexShaderSeparate::VertexShaderSeparate(OpenGLRenderer &openGLRenderer, const Renderer::VertexAttributes& vertexAttributes, const char *sourceCode, Renderer::ShaderBytecode*) :
		IVertexShader(reinterpret_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShaderProgram(::detail::CreateOpenGLShaderProgram(vertexAttributes, GL_VERTEX_SHADER_ARB, sourceCode))
	{
		// TODO(co) Return shader bytecode, if requested do to so
		// Nothing here
	}

	VertexShaderSeparate::~VertexShaderSeparate()
	{
		// Destroy the OpenGL shader program
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteProgram(mOpenGLShaderProgram);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char *VertexShaderSeparate::getShaderLanguageName() const
	{
		return ShaderLanguageSeparate::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
