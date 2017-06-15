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
#include "OpenGLRenderer/Shader/Monolithic/ProgramMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/VertexShaderMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/GeometryShaderMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/FragmentShaderMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/TessellationControlShaderMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/TessellationEvaluationShaderMonolithic.h"
#include "OpenGLRenderer/IContext.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/RootSignature.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"

#include <Renderer/Buffer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramMonolithic::ProgramMonolithic(OpenGLRenderer &openGLRenderer, const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, VertexShaderMonolithic *vertexShaderMonolithic, TessellationControlShaderMonolithic *tessellationControlShaderMonolithic, TessellationEvaluationShaderMonolithic *tessellationEvaluationShaderMonolithic, GeometryShaderMonolithic *geometryShaderMonolithic, FragmentShaderMonolithic *fragmentShaderMonolithic) :
		IProgram(openGLRenderer),
		mOpenGLProgram(glCreateProgramObjectARB()),
		mDrawIdUniformLocation(-1)
	{
		// Attach the shaders to the program
		// -> We don't need to keep a reference to the shader, to add and release at once to ensure a nice behaviour
		if (nullptr != vertexShaderMonolithic)
		{
			vertexShaderMonolithic->addReference();
			glAttachObjectARB(mOpenGLProgram, vertexShaderMonolithic->getOpenGLShader());
			vertexShaderMonolithic->releaseReference();
		}
		if (nullptr != tessellationControlShaderMonolithic)
		{
			tessellationControlShaderMonolithic->addReference();
			glAttachObjectARB(mOpenGLProgram, tessellationControlShaderMonolithic->getOpenGLShader());
			tessellationControlShaderMonolithic->releaseReference();
		}
		if (nullptr != tessellationEvaluationShaderMonolithic)
		{
			tessellationEvaluationShaderMonolithic->addReference();
			glAttachObjectARB(mOpenGLProgram, tessellationEvaluationShaderMonolithic->getOpenGLShader());
			tessellationEvaluationShaderMonolithic->releaseReference();
		}
		if (nullptr != geometryShaderMonolithic)
		{
			// Add a reference to the shader
			geometryShaderMonolithic->addReference();

			// Attach the monolithic shader to the monolithic program
			glAttachObjectARB(mOpenGLProgram, geometryShaderMonolithic->getOpenGLShader());

			// In modern GLSL, "geometry shader input primitive topology" & "geometry shader output primitive topology" & "number of output vertices" can be directly set within GLSL by writing e.g.
			//   "layout(triangles) in;"
			//   "layout(triangle_strip, max_vertices = 3) out;"
			// -> To be able to support older GLSL versions, we have to provide this information also via OpenGL API functions

			// Set the OpenGL geometry shader input primitive topology
			glProgramParameteriARB(mOpenGLProgram, GL_GEOMETRY_INPUT_TYPE_ARB, geometryShaderMonolithic->getOpenGLGsInputPrimitiveTopology());

			// Set the OpenGL geometry shader output primitive topology
			glProgramParameteriARB(mOpenGLProgram, GL_GEOMETRY_OUTPUT_TYPE_ARB, geometryShaderMonolithic->getOpenGLGsOutputPrimitiveTopology());

			// Set the number of output vertices
			glProgramParameteriARB(mOpenGLProgram, GL_GEOMETRY_VERTICES_OUT_ARB, static_cast<GLint>(geometryShaderMonolithic->getNumberOfOutputVertices()));

			// Release the shader
			geometryShaderMonolithic->releaseReference();
		}
		if (nullptr != fragmentShaderMonolithic)
		{
			fragmentShaderMonolithic->addReference();
			glAttachObjectARB(mOpenGLProgram, fragmentShaderMonolithic->getOpenGLShader());
			fragmentShaderMonolithic->releaseReference();
		}

		{ // Define the vertex array attribute binding locations ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 & 12 terminology)
			const uint32_t numberOfVertexAttributes = vertexAttributes.numberOfAttributes;
			for (uint32_t vertexAttribute = 0; vertexAttribute < numberOfVertexAttributes; ++vertexAttribute)
			{
				glBindAttribLocationARB(mOpenGLProgram, vertexAttribute, vertexAttributes.attributes[vertexAttribute].name);
			}
		}

		// Link the program
		glLinkProgramARB(mOpenGLProgram);

		// Check the link status
		GLint linked = GL_FALSE;
		glGetObjectParameterivARB(mOpenGLProgram, GL_OBJECT_LINK_STATUS_ARB, &linked);
		if (GL_TRUE == linked)
		{
			// We're not using "glBindFragDataLocation()", else the user would have to provide us with additional OpenGL-only specific information
			// -> Use modern GLSL:
			//    "layout(location = 0) out vec4 ColorOutput0;"
			//    "layout(location = 1) out vec4 ColorOutput1;"
			// -> Use legacy GLSL if necessary:
			//    "gl_FragData[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);"
			//    "gl_FragData[1] = vec4(0.0f, 0.0f, 1.0f, 0.0f);"

			// Get draw ID uniform location
			if (!openGLRenderer.getExtensions().isGL_ARB_base_instance())
			{
				mDrawIdUniformLocation = glGetUniformLocationARB(mOpenGLProgram, "drawIdUniform");
			}

			// The actual locations assigned to uniform variables are not known until the program object is linked successfully
			// -> So we have to build a root signature parameter index -> uniform location mapping here
			const Renderer::RootSignature& rootSignatureData = static_cast<const RootSignature&>(rootSignature).getRootSignature();
			const uint32_t numberOfParameters = rootSignatureData.numberOfParameters;
			for (uint32_t parameterIndex = 0; parameterIndex < numberOfParameters; ++parameterIndex)
			{
				const Renderer::RootParameter& rootParameter = rootSignatureData.parameters[parameterIndex];
				if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
				{
					// TODO(co) For now, we only support a single descriptor range
					if (1 != rootParameter.descriptorTable.numberOfDescriptorRanges)
					{
						RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Only a single descriptor range is supported")
					}
					else
					{
						const Renderer::DescriptorRange* descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges);

						// Ignore sampler range types in here (OpenGL handles samplers in a different way then Direct3D 10>=)
						if (Renderer::DescriptorRangeType::UBV == descriptorRange->rangeType)
						{
							// Explicit binding points ("layout(binding = 0)" in GLSL shader) requires OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension,
							// for backward compatibility, ask for the uniform block index
							const GLuint uniformBlockIndex = glGetUniformBlockIndex(mOpenGLProgram, descriptorRange->baseShaderRegisterName);
							if (GL_INVALID_INDEX != uniformBlockIndex)
							{
								// Associate the uniform block with the given binding point
								glUniformBlockBinding(mOpenGLProgram, uniformBlockIndex, parameterIndex);
							}
						}
						else if (Renderer::DescriptorRangeType::SAMPLER != descriptorRange->rangeType)
						{
							const GLint uniformLocation = glGetUniformLocationARB(mOpenGLProgram, descriptorRange->baseShaderRegisterName);
							if (uniformLocation >= 0)
							{
								// OpenGL/GLSL is not automatically assigning texture units to samplers, so, we have to take over this job
								// -> When using OpenGL or OpenGL ES 2 this is required
								// -> OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension supports explicit binding points ("layout(binding = 0)"
								//    in GLSL shader) , for backward compatibility we don't use it in here
								// -> When using Direct3D 9, 10, 11 or 12, the texture unit
								//    to use is usually defined directly within the shader by using the "register"-keyword
								// -> Use the "GL_ARB_direct_state_access" or "GL_EXT_direct_state_access" extension if possible to not change OpenGL states
								if (nullptr != glProgramUniform1i)
								{
									glProgramUniform1i(mOpenGLProgram, uniformLocation, static_cast<GLint>(descriptorRange->baseShaderRegister));
								}
								else if (nullptr != glProgramUniform1iEXT)
								{
									glProgramUniform1iEXT(mOpenGLProgram, uniformLocation, static_cast<GLint>(descriptorRange->baseShaderRegister));
								}
								else
								{
									// TODO(co) There's room for binding API call related optimization in here (will certainly be no huge overall efficiency gain)
									#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
										// Backup the currently used OpenGL program
										GLint openGLProgramBackup = 0;
										glGetIntegerv(GL_CURRENT_PROGRAM, &openGLProgramBackup);
										if (static_cast<GLuint>(openGLProgramBackup) == mOpenGLProgram)
										{
											// Set uniform, please note that for this our program must be the currently used one
											glUniform1iARB(uniformLocation, static_cast<GLint>(descriptorRange->baseShaderRegister));
										}
										else
										{
											// Set uniform, please note that for this our program must be the currently used one
											glUseProgramObjectARB(mOpenGLProgram);
											glUniform1iARB(uniformLocation, static_cast<GLint>(descriptorRange->baseShaderRegister));

											// Be polite and restore the previous used OpenGL program
											glUseProgramObjectARB(static_cast<GLhandleARB>(openGLProgramBackup));
										}
									#else
										glUseProgramObjectARB(mOpenGLProgram);
										glUniform1iARB(uniformLocation, static_cast<GLint>(descriptorRange->baseShaderRegister));
									#endif
								}
							}
						}
					}
				}
			}
		}
		else
		{
			// Error, program link failed!
			#ifdef RENDERER_OUTPUT_DEBUG
				// Get the length of the information (including a null termination)
				GLint informationLength = 0;
				glGetObjectParameterivARB(mOpenGLProgram, GL_OBJECT_INFO_LOG_LENGTH_ARB, &informationLength);
				if (informationLength > 1)
				{
					// Allocate memory for the information
					char *informationLog = new char[static_cast<uint32_t>(informationLength)];

					// Get the information
					glGetInfoLogARB(mOpenGLProgram, informationLength, nullptr, informationLog);

					// Output the debug string
					RENDERER_OUTPUT_DEBUG_STRING(informationLog)

					// Cleanup information memory
					delete [] informationLog;
				}
			#endif
		}
	}

	ProgramMonolithic::~ProgramMonolithic()
	{
		// Destroy the OpenGL program
		// -> A value of 0 for program will be silently ignored
		glDeleteObjectARB(mOpenGLProgram);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	handle ProgramMonolithic::getUniformHandle(const char *uniformName)
	{
		return static_cast<handle>(glGetUniformLocationARB(mOpenGLProgram, uniformName));
	}

	void ProgramMonolithic::setUniform1i(handle uniformHandle, int value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLhandleARB openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform1iARB(static_cast<GLint>(uniformHandle), value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform1iARB(static_cast<GLint>(uniformHandle), value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform1iARB(static_cast<GLint>(uniformHandle), value);
		#endif
	}

	void ProgramMonolithic::setUniform1f(handle uniformHandle, float value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLhandleARB openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform1fARB(static_cast<GLint>(uniformHandle), value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform1fARB(static_cast<GLint>(uniformHandle), value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform1fARB(static_cast<GLint>(uniformHandle), value);
		#endif
	}

	void ProgramMonolithic::setUniform2fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLhandleARB openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform2fvARB(static_cast<GLint>(uniformHandle), 1, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform2fvARB(static_cast<GLint>(uniformHandle), 1, value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform2fvARB(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramMonolithic::setUniform3fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLhandleARB openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform3fvARB(static_cast<GLint>(uniformHandle), 1, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform3fvARB(static_cast<GLint>(uniformHandle), 1, value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform3fvARB(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramMonolithic::setUniform4fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLhandleARB openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform4fvARB(static_cast<GLint>(uniformHandle), 1, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniform4fvARB(static_cast<GLint>(uniformHandle), 1, value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniform4fvARB(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramMonolithic::setUniformMatrix3fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLhandleARB openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniformMatrix3fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniformMatrix3fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniformMatrix3fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		#endif
	}

	void ProgramMonolithic::setUniformMatrix4fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program
			const GLhandleARB openGLProgramBackup = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
			if (openGLProgramBackup == mOpenGLProgram)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniformMatrix4fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgramObjectARB(mOpenGLProgram);
				glUniformMatrix4fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);

				// Be polite and restore the previous used OpenGL program
				glUseProgramObjectARB(openGLProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgramObjectARB(mOpenGLProgram);
			glUniformMatrix4fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
