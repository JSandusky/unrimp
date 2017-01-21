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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLES2Renderer/Shader/ProgramGlsl.h"
#include "OpenGLES2Renderer/Shader/VertexShaderGlsl.h"
#include "OpenGLES2Renderer/Shader/FragmentShaderGlsl.h"
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"
#include "OpenGLES2Renderer/RootSignature.h"
#include "OpenGLES2Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 2 functions

#include <Renderer/Buffer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramGlsl::ProgramGlsl(OpenGLES2Renderer &openGLES2Renderer, const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, VertexShaderGlsl *vertexShaderGlsl, FragmentShaderGlsl *fragmentShaderGlsl) :
		IProgram(openGLES2Renderer),
		mNumberOfRootSignatureParameters(0),
		mRootSignatureParameterIndexToUniformLocation(nullptr),
		mOpenGLES2Program(0)
	{
		// Create the OpenGL ES 2 program
		mOpenGLES2Program = glCreateProgram();

		{ // Define the vertex array attribute binding locations ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
			const uint32_t numberOfVertexAttributes = vertexAttributes.numberOfAttributes;
			for (uint32_t vertexAttribute = 0; vertexAttribute < numberOfVertexAttributes; ++vertexAttribute)
			{
				glBindAttribLocation(mOpenGLES2Program, vertexAttribute, vertexAttributes.attributes[vertexAttribute].name);
			}
		}

		// Attach the shaders to the program
		// -> We don't need to keep a reference to the shader, to add and release at once to ensure a nice behaviour
		if (nullptr != vertexShaderGlsl)
		{
			vertexShaderGlsl->addReference();
			glAttachShader(mOpenGLES2Program, vertexShaderGlsl->getOpenGLES2Shader());
			vertexShaderGlsl->releaseReference();
		}
		if (nullptr != fragmentShaderGlsl)
		{
			fragmentShaderGlsl->addReference();
			glAttachShader(mOpenGLES2Program, fragmentShaderGlsl->getOpenGLES2Shader());
			fragmentShaderGlsl->releaseReference();
		}

		// Link the program
		glLinkProgram(mOpenGLES2Program);

		// Check the link status
		GLint linked = GL_FALSE;
		glGetProgramiv(mOpenGLES2Program, GL_LINK_STATUS, &linked);
		if (GL_TRUE == linked)
		{
			// The actual locations assigned to uniform variables are not known until the program object is linked successfully
			// -> So we have to build a root signature parameter index -> uniform location mapping here
			const Renderer::RootSignature& rootSignatureData = static_cast<const RootSignature&>(rootSignature).getRootSignature();
			const uint32_t numberOfParameters = rootSignatureData.numberOfParameters;
			if (numberOfParameters > 0)
			{
				mRootSignatureParameterIndexToUniformLocation = new int32_t[numberOfParameters];
				memset(mRootSignatureParameterIndexToUniformLocation, -1, sizeof(int32_t) * numberOfParameters);
				for (uint32_t parameterIndex = 0; parameterIndex < numberOfParameters; ++parameterIndex)
				{
					const Renderer::RootParameter& rootParameter = rootSignatureData.parameters[parameterIndex];
					if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
					{
						// TODO(co) For now, we only support a single descriptor range
						if (1 != rootParameter.descriptorTable.numberOfDescriptorRanges)
						{
							RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Only a single descriptor range is supported")
						}
						else
						{
							const Renderer::DescriptorRange* descriptorRange = rootParameter.descriptorTable.descriptorRanges;

							// Ignore sampler range types in here (OpenGL handles samplers in a different way then Direct3D 10>=)
							if (Renderer::DescriptorRangeType::SAMPLER != descriptorRange->rangeType)
							{
								const GLint uniformLocation = glGetUniformLocation(mOpenGLES2Program, descriptorRange->baseShaderRegisterName);
								if (uniformLocation >= 0)
								{
									mRootSignatureParameterIndexToUniformLocation[parameterIndex] = uniformLocation;

									// OpenGL ES 2/GLSL is not automatically assigning texture units to samplers, so, we have to take over this job
									// -> When using OpenGL or OpenGL ES 2 this is required
									// -> OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension supports explicit binding points ("layout(binding = 0)"
									//    in GLSL shader) , for backward compatibility we don't use it in here
									// -> When using Direct3D 9, Direct3D 10 or Direct3D 11, the texture unit
									//    to use is usually defined directly within the shader by using the "register"-keyword
									// TODO(co) There's room for binding API call related optimization in here (will certainly be no huge overall efficiency gain)
									#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
										// Backup the currently used OpenGL ES 2 program
										GLint openGLES2ProgramBackup = 0;
										glGetIntegerv(GL_CURRENT_PROGRAM, &openGLES2ProgramBackup);
										if (openGLES2ProgramBackup == mOpenGLES2Program)
										{
											// Set uniform, please note that for this our program must be the currently used one
											glUniform1i(uniformLocation, static_cast<GLint>(descriptorRange->baseShaderRegister));
										}
										else
										{
											// Set uniform, please note that for this our program must be the currently used one
											glUseProgram(mOpenGLES2Program);
											glUniform1i(uniformLocation, static_cast<GLint>(descriptorRange->baseShaderRegister));

											// Be polite and restore the previous used OpenGL ES 2 program
											glUseProgram(openGLES2ProgramBackup);
										}
									#else
										// Set uniform, please note that for this our program must be the currently used one
										glUseProgram(mOpenGLES2Program);
										glUniform1i(uniformLocation, static_cast<GLint>(descriptorRange->baseShaderRegister));
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
				glGetProgramiv(mOpenGLES2Program, GL_INFO_LOG_LENGTH, &informationLength);
				if (informationLength > 1)
				{
					// Allocate memory for the information
					char *informationLog = new char[static_cast<uint32_t>(informationLength)];

					// Get the information
					glGetProgramInfoLog(mOpenGLES2Program, informationLength, nullptr, informationLog);

					// Output the debug string
					RENDERER_OUTPUT_DEBUG_STRING(informationLog)

					// Cleanup information memory
					delete [] informationLog;
				}
			#endif
		}
	}

	ProgramGlsl::~ProgramGlsl()
	{
		// Destroy the OpenGL ES 2 program
		// -> A value of 0 for program will be silently ignored
		glDeleteProgram(mOpenGLES2Program);

		// Destroy root signature parameter index to OpenGL ES 2 uniform location mapping, if required
		if (nullptr != mRootSignatureParameterIndexToUniformLocation)
		{
			delete [] mRootSignatureParameterIndexToUniformLocation;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	handle ProgramGlsl::getUniformHandle(const char *uniformName)
	{
		return static_cast<handle>(glGetUniformLocation(mOpenGLES2Program, uniformName));
	}

	void ProgramGlsl::setUniform1i(handle uniformHandle, int value)
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL ES 2 program
			GLint openGLES2ProgramBackup = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &openGLES2ProgramBackup);
			if (openGLES2ProgramBackup == mOpenGLES2Program)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform1i(static_cast<GLint>(uniformHandle), value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgram(mOpenGLES2Program);
				glUniform1i(static_cast<GLint>(uniformHandle), value);

				// Be polite and restore the previous used OpenGL ES 2 program
				glUseProgram(openGLES2ProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgram(mOpenGLES2Program);
			glUniform1i(static_cast<GLint>(uniformHandle), value);
		#endif
	}

	void ProgramGlsl::setUniform1f(handle uniformHandle, float value)
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL ES 2 program
			GLint openGLES2ProgramBackup = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &openGLES2ProgramBackup);
			if (openGLES2ProgramBackup == mOpenGLES2Program)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform1f(static_cast<GLint>(uniformHandle), value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgram(mOpenGLES2Program);
				glUniform1f(static_cast<GLint>(uniformHandle), value);

				// Be polite and restore the previous used OpenGL ES 2 program
				glUseProgram(openGLES2ProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgram(mOpenGLES2Program);
			glUniform1f(static_cast<GLint>(uniformHandle), value);
		#endif
	}

	void ProgramGlsl::setUniform2fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL ES 2 program
			GLint openGLES2ProgramBackup = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &openGLES2ProgramBackup);
			if (openGLES2ProgramBackup == mOpenGLES2Program)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform2fv(static_cast<GLint>(uniformHandle), 1, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgram(mOpenGLES2Program);
				glUniform2fv(static_cast<GLint>(uniformHandle), 1, value);

				// Be polite and restore the previous used OpenGL ES 2 program
				glUseProgram(openGLES2ProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgram(mOpenGLES2Program);
			glUniform2fv(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramGlsl::setUniform3fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL ES 2 program
			GLint openGLES2ProgramBackup = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &openGLES2ProgramBackup);
			if (openGLES2ProgramBackup == mOpenGLES2Program)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform3fv(static_cast<GLint>(uniformHandle), 1, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgram(mOpenGLES2Program);
				glUniform3fv(static_cast<GLint>(uniformHandle), 1, value);

				// Be polite and restore the previous used OpenGL ES 2 program
				glUseProgram(openGLES2ProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgram(mOpenGLES2Program);
			glUniform3fv(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramGlsl::setUniform4fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL ES 2 program
			GLint openGLES2ProgramBackup = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &openGLES2ProgramBackup);
			if (openGLES2ProgramBackup == mOpenGLES2Program)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform4fv(static_cast<GLint>(uniformHandle), 1, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgram(mOpenGLES2Program);
				glUniform4fv(static_cast<GLint>(uniformHandle), 1, value);

				// Be polite and restore the previous used OpenGL ES 2 program
				glUseProgram(openGLES2ProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgram(mOpenGLES2Program);
			glUniform4fv(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramGlsl::setUniformMatrix3fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL ES 2 program
			GLint openGLES2ProgramBackup = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &openGLES2ProgramBackup);
			if (openGLES2ProgramBackup == mOpenGLES2Program)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniformMatrix3fv(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgram(mOpenGLES2Program);
				glUniformMatrix3fv(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);

				// Be polite and restore the previous used OpenGL ES 2 program
				glUseProgram(openGLES2ProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgram(mOpenGLES2Program);
			glUniformMatrix3fv(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		#endif
	}

	void ProgramGlsl::setUniformMatrix4fv(handle uniformHandle, const float *value)
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL ES 2 program
			GLint openGLES2ProgramBackup = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &openGLES2ProgramBackup);
			if (openGLES2ProgramBackup == mOpenGLES2Program)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniformMatrix4fv(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgram(mOpenGLES2Program);
				glUniformMatrix4fv(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);

				// Be polite and restore the previous used OpenGL ES 2 program
				glUseProgram(openGLES2ProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgram(mOpenGLES2Program);
			glUniformMatrix4fv(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
