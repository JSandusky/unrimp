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
#include "OpenGLRenderer/Shader/Separate/ProgramSeparate.h"
#include "OpenGLRenderer/Shader/Separate/VertexShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/GeometryShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/FragmentShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/TessellationControlShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/TessellationEvaluationShaderSeparate.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/RootSignature.h"
#include "OpenGLRenderer/OpenGLRenderer.h"

#include <Renderer/ILog.h>


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
		void bindUniformBlock(const Renderer::DescriptorRange& descriptorRange, uint32_t openGLProgram, uint32_t uniformBlockBindingIndex)
		{
			// Explicit binding points ("layout(binding = 0)" in GLSL shader) requires OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension,
			// for backward compatibility, ask for the uniform block index
			const GLuint uniformBlockIndex = OpenGLRenderer::glGetUniformBlockIndex(openGLProgram, descriptorRange.baseShaderRegisterName);
			if (GL_INVALID_INDEX != uniformBlockIndex)
			{
				// Associate the uniform block with the given binding point
				OpenGLRenderer::glUniformBlockBinding(openGLProgram, uniformBlockIndex, uniformBlockBindingIndex);
			}
		}

		void bindUniformLocation(const Renderer::DescriptorRange& descriptorRange, uint32_t openGLProgramPipeline, uint32_t openGLProgram)
		{
			const GLint uniformLocation = OpenGLRenderer::glGetUniformLocationARB(openGLProgram, descriptorRange.baseShaderRegisterName);
			if (uniformLocation >= 0)
			{
				// OpenGL/GLSL is not automatically assigning texture units to samplers, so, we have to take over this job
				// -> When using OpenGL or OpenGL ES 2 this is required
				// -> OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension supports explicit binding points ("layout(binding = 0)"
				//    in GLSL shader) , for backward compatibility we don't use it in here
				// -> When using Direct3D 9, 10, 11 or 12, the texture unit
				//    to use is usually defined directly within the shader by using the "register"-keyword
				// -> Use the "GL_ARB_direct_state_access" or "GL_EXT_direct_state_access" extension if possible to not change OpenGL states
				if (nullptr != OpenGLRenderer::glProgramUniform1i)
				{
					OpenGLRenderer::glProgramUniform1i(openGLProgram, uniformLocation, static_cast<GLint>(descriptorRange.baseShaderRegister));
				}
				else if (nullptr != OpenGLRenderer::glProgramUniform1iEXT)
				{
					OpenGLRenderer::glProgramUniform1iEXT(openGLProgram, uniformLocation, static_cast<GLint>(descriptorRange.baseShaderRegister));
				}
				else
				{
					// TODO(co) There's room for binding API call related optimization in here (will certainly be no huge overall efficiency gain)
					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Backup the currently used OpenGL program
						GLint openGLProgramBackup = 0;
						OpenGLRenderer::glGetProgramPipelineiv(openGLProgramPipeline, GL_ACTIVE_PROGRAM, &openGLProgramBackup);
						if (static_cast<uint32_t>(openGLProgramBackup) == openGLProgram)
						{
							// Set uniform, please note that for this our program must be the currently used one
							OpenGLRenderer::glUniform1iARB(uniformLocation, static_cast<GLint>(descriptorRange.baseShaderRegister));
						}
						else
						{
							// Set uniform, please note that for this our program must be the currently used one
							OpenGLRenderer::glActiveShaderProgram(openGLProgramPipeline, openGLProgram);
							OpenGLRenderer::glUniform1iARB(uniformLocation, static_cast<GLint>(descriptorRange.baseShaderRegister));

							// Be polite and restore the previous used OpenGL program
							OpenGLRenderer::glActiveShaderProgram(openGLProgramPipeline, static_cast<GLuint>(openGLProgramBackup));
						}
					#else
						OpenGLRenderer::glActiveShaderProgram(openGLProgramPipeline, openGLProgram);
						OpenGLRenderer::glUniform1iARB(uniformLocation, static_cast<GLint>(descriptorRange.baseShaderRegister));
					#endif
				}
			}
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
	ProgramSeparate::ProgramSeparate(OpenGLRenderer& openGLRenderer, const Renderer::IRootSignature& rootSignature, VertexShaderSeparate* vertexShaderSeparate, TessellationControlShaderSeparate* tessellationControlShaderSeparate, TessellationEvaluationShaderSeparate* tessellationEvaluationShaderSeparate, GeometryShaderSeparate* geometryShaderSeparate, FragmentShaderSeparate* fragmentShaderSeparate) :
		IProgram(openGLRenderer),
		mOpenGLProgramPipeline(0),
		mVertexShaderSeparate(vertexShaderSeparate),
		mTessellationControlShaderSeparate(tessellationControlShaderSeparate),
		mTessellationEvaluationShaderSeparate(tessellationEvaluationShaderSeparate),
		mGeometryShaderSeparate(geometryShaderSeparate),
		mFragmentShaderSeparate(fragmentShaderSeparate)
	{
		// Create the OpenGL program pipeline
		glGenProgramPipelines(1, &mOpenGLProgramPipeline);

		// If the "GL_ARB_direct_state_access" nor "GL_EXT_direct_state_access" extension is available, we need to change OpenGL states during resource creation (nasty thing)
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program pipeline
			GLint openGLProgramPipelineBackup = 0;
		#endif
		if (nullptr == glProgramUniform1i && nullptr == glProgramUniform1iEXT)
		{
			#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
				glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &openGLProgramPipelineBackup);
			#endif
			glBindProgramPipeline(mOpenGLProgramPipeline);
		}

		// Add references to the provided shaders
		#define USE_PROGRAM_STAGES(ShaderBit, ShaderSeparate) if (nullptr != ShaderSeparate) { ShaderSeparate->addReference(); glUseProgramStages(mOpenGLProgramPipeline, ShaderBit, ShaderSeparate->getOpenGLShaderProgram()); }
		USE_PROGRAM_STAGES(GL_VERTEX_SHADER_BIT,		  mVertexShaderSeparate)
		USE_PROGRAM_STAGES(GL_TESS_CONTROL_SHADER_BIT,	  mTessellationControlShaderSeparate)
		USE_PROGRAM_STAGES(GL_TESS_EVALUATION_SHADER_BIT, mTessellationEvaluationShaderSeparate)
		USE_PROGRAM_STAGES(GL_GEOMETRY_SHADER_BIT,		  mGeometryShaderSeparate)
		USE_PROGRAM_STAGES(GL_FRAGMENT_SHADER_BIT,		  mFragmentShaderSeparate)
		#undef USE_PROGRAM_STAGES

		// Validate program pipeline
		glValidateProgramPipeline(mOpenGLProgramPipeline);
		GLint validateStatus = 0;
		glGetProgramPipelineiv(mOpenGLProgramPipeline, GL_VALIDATE_STATUS, &validateStatus);
		if (GL_TRUE == validateStatus)
		{
			// We're not using "glBindFragDataLocation()", else the user would have to provide us with additional OpenGL-only specific information
			// -> Use modern GLSL:
			//    "layout(location = 0) out vec4 ColorOutput0;"
			//    "layout(location = 1) out vec4 ColorOutput1;"
			// -> Use legacy GLSL if necessary:
			//    "gl_FragData[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);"
			//    "gl_FragData[1] = vec4(0.0f, 0.0f, 1.0f, 0.0f);"

			// The actual locations assigned to uniform variables are not known until the program object is linked successfully
			// -> So we have to build a root signature parameter index -> uniform location mapping here
			const Renderer::RootSignature& rootSignatureData = static_cast<const RootSignature&>(rootSignature).getRootSignature();
			const uint32_t numberOfRootParameters = rootSignatureData.numberOfParameters;
			uint32_t uniformBlockBindingIndex = 0;
			for (uint32_t rootParameterIndex = 0; rootParameterIndex < numberOfRootParameters; ++rootParameterIndex)
			{
				const Renderer::RootParameter& rootParameter = rootSignatureData.parameters[rootParameterIndex];
				if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
				{
					assert(nullptr != reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges));
					const uint32_t numberOfDescriptorRanges = rootParameter.descriptorTable.numberOfDescriptorRanges;
					for (uint32_t descriptorRangeIndex = 0; descriptorRangeIndex < numberOfDescriptorRanges; ++descriptorRangeIndex)
					{
						const Renderer::DescriptorRange& descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges)[descriptorRangeIndex];

						// Ignore sampler range types in here (OpenGL handles samplers in a different way then Direct3D 10>=)
						if (Renderer::DescriptorRangeType::UBV == descriptorRange.rangeType)
						{
							#define BIND_UNIFORM_BLOCK(ShaderSeparate) if (nullptr != ShaderSeparate) ::detail::bindUniformBlock(descriptorRange, ShaderSeparate->getOpenGLShaderProgram(), uniformBlockBindingIndex);
							switch (descriptorRange.shaderVisibility)
							{
								case Renderer::ShaderVisibility::ALL:
									BIND_UNIFORM_BLOCK(mVertexShaderSeparate)
									BIND_UNIFORM_BLOCK(mTessellationControlShaderSeparate)
									BIND_UNIFORM_BLOCK(mTessellationEvaluationShaderSeparate)
									BIND_UNIFORM_BLOCK(mGeometryShaderSeparate)
									BIND_UNIFORM_BLOCK(mFragmentShaderSeparate)
									break;

								case Renderer::ShaderVisibility::VERTEX:
									BIND_UNIFORM_BLOCK(mVertexShaderSeparate)
									break;

								case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
									BIND_UNIFORM_BLOCK(mTessellationControlShaderSeparate)
									break;

								case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
									BIND_UNIFORM_BLOCK(mTessellationEvaluationShaderSeparate)
									break;

								case Renderer::ShaderVisibility::GEOMETRY:
									BIND_UNIFORM_BLOCK(mGeometryShaderSeparate)
									break;

								case Renderer::ShaderVisibility::FRAGMENT:
									BIND_UNIFORM_BLOCK(mFragmentShaderSeparate)
									break;
							}
							#undef BIND_UNIFORM_BLOCK
							++uniformBlockBindingIndex;
						}
						else if (Renderer::DescriptorRangeType::SAMPLER != descriptorRange.rangeType)
						{
							#define BIND_UNIFORM_LOCATION(ShaderSeparate) if (nullptr != ShaderSeparate) ::detail::bindUniformLocation(descriptorRange, mOpenGLProgramPipeline, ShaderSeparate->getOpenGLShaderProgram());
							switch (descriptorRange.shaderVisibility)
							{
								case Renderer::ShaderVisibility::ALL:
									BIND_UNIFORM_LOCATION(mVertexShaderSeparate)
									BIND_UNIFORM_LOCATION(mTessellationControlShaderSeparate)
									BIND_UNIFORM_LOCATION(mTessellationEvaluationShaderSeparate)
									BIND_UNIFORM_LOCATION(mGeometryShaderSeparate)
									BIND_UNIFORM_LOCATION(mFragmentShaderSeparate)
									break;

								case Renderer::ShaderVisibility::VERTEX:
									BIND_UNIFORM_LOCATION(mVertexShaderSeparate)
									break;

								case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
									BIND_UNIFORM_LOCATION(mTessellationControlShaderSeparate)
									break;

								case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
									BIND_UNIFORM_LOCATION(mTessellationEvaluationShaderSeparate)
									break;

								case Renderer::ShaderVisibility::GEOMETRY:
									BIND_UNIFORM_LOCATION(mGeometryShaderSeparate)
									break;

								case Renderer::ShaderVisibility::FRAGMENT:
									BIND_UNIFORM_LOCATION(mFragmentShaderSeparate)
									break;
							}
							#undef BIND_UNIFORM_LOCATION
						}
					}
				}
			}
		}
		else
		{
			// Error, program pipeline validation failed!

			// Get the length of the information (including a null termination)
			GLint informationLength = 0;
			glGetProgramPipelineiv(mOpenGLProgramPipeline, GL_INFO_LOG_LENGTH, &informationLength);
			if (informationLength > 1)
			{
				// Allocate memory for the information
				char* informationLog = new char[static_cast<uint32_t>(informationLength)];

				// Get the information
				glGetProgramPipelineInfoLog(mOpenGLProgramPipeline, informationLength, nullptr, informationLog);

				// Output the debug string
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, informationLog)

				// Cleanup information memory
				delete [] informationLog;
			}
		}

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous used OpenGL program pipeline
			if (nullptr == glProgramUniform1i && nullptr == glProgramUniform1iEXT)
			{
				glBindProgramPipeline(static_cast<GLuint>(openGLProgramPipelineBackup));
			}
		#endif
	}

	ProgramSeparate::~ProgramSeparate()
	{
		// Destroy the OpenGL program pipeline
		glDeleteProgramPipelines(1, &mOpenGLProgramPipeline);

		// Release the shader references
		if (nullptr != mVertexShaderSeparate)
		{
			mVertexShaderSeparate->releaseReference();
		}
		if (nullptr != mTessellationControlShaderSeparate)
		{
			mTessellationControlShaderSeparate->releaseReference();
		}
		if (nullptr != mTessellationEvaluationShaderSeparate)
		{
			mTessellationEvaluationShaderSeparate->releaseReference();
		}
		if (nullptr != mGeometryShaderSeparate)
		{
			mGeometryShaderSeparate->releaseReference();
		}
		if (nullptr != mFragmentShaderSeparate)
		{
			mFragmentShaderSeparate->releaseReference();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	handle ProgramSeparate::getUniformHandle(const char* uniformName)
	{
		GLint uniformLocation = -1;
		#define GET_UNIFORM_LOCATION(ShaderSeparate) if (uniformLocation < 0 && nullptr != ShaderSeparate) uniformLocation = glGetUniformLocationARB(ShaderSeparate->getOpenGLShaderProgram(), uniformName);
		GET_UNIFORM_LOCATION(mVertexShaderSeparate)
		GET_UNIFORM_LOCATION(mTessellationControlShaderSeparate)
		GET_UNIFORM_LOCATION(mTessellationEvaluationShaderSeparate)
		GET_UNIFORM_LOCATION(mGeometryShaderSeparate)
		GET_UNIFORM_LOCATION(mFragmentShaderSeparate)
		#undef GET_UNIFORM_LOCATION
		return static_cast<handle>(uniformLocation);
	}

	void ProgramSeparate::setUniform1i(handle uniformHandle, int value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program pipeline
			GLint openGLProgramPipelineBackup = 0;
			glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &openGLProgramPipelineBackup);

			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniform1iARB(static_cast<GLint>(uniformHandle), value);

			// Be polite and restore the previous used OpenGL program pipeline
			glBindProgramPipeline(static_cast<GLuint>(openGLProgramPipelineBackup));
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniform1iARB(static_cast<GLint>(uniformHandle), value);
		#endif
	}

	void ProgramSeparate::setUniform1f(handle uniformHandle, float value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program pipeline
			GLint openGLProgramPipelineBackup = 0;
			glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &openGLProgramPipelineBackup);

			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniform1fARB(static_cast<GLint>(uniformHandle), value);

			// Be polite and restore the previous used OpenGL program pipeline
			glBindProgramPipeline(static_cast<GLuint>(openGLProgramPipelineBackup));
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniform1fARB(static_cast<GLint>(uniformHandle), value);
		#endif
	}

	void ProgramSeparate::setUniform2fv(handle uniformHandle, const float* value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program pipeline
			GLint openGLProgramPipelineBackup = 0;
			glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &openGLProgramPipelineBackup);

			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniform2fvARB(static_cast<GLint>(uniformHandle), 1, value);

			// Be polite and restore the previous used OpenGL program pipeline
			glBindProgramPipeline(static_cast<GLuint>(openGLProgramPipelineBackup));
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniform2fvARB(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramSeparate::setUniform3fv(handle uniformHandle, const float* value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program pipeline
			GLint openGLProgramPipelineBackup = 0;
			glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &openGLProgramPipelineBackup);

			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniform3fvARB(static_cast<GLint>(uniformHandle), 1, value);

			// Be polite and restore the previous used OpenGL program pipeline
			glBindProgramPipeline(static_cast<GLuint>(openGLProgramPipelineBackup));
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniform3fvARB(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramSeparate::setUniform4fv(handle uniformHandle, const float* value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program pipeline
			GLint openGLProgramPipelineBackup = 0;
			glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &openGLProgramPipelineBackup);

			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniform4fvARB(static_cast<GLint>(uniformHandle), 1, value);

			// Be polite and restore the previous used OpenGL program pipeline
			glBindProgramPipeline(static_cast<GLuint>(openGLProgramPipelineBackup));
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniform4fvARB(static_cast<GLint>(uniformHandle), 1, value);
		#endif
	}

	void ProgramSeparate::setUniformMatrix3fv(handle uniformHandle, const float* value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program pipeline
			GLint openGLProgramPipelineBackup = 0;
			glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &openGLProgramPipelineBackup);

			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniformMatrix3fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);

			// Be polite and restore the previous used OpenGL program pipeline
			glBindProgramPipeline(static_cast<GLuint>(openGLProgramPipelineBackup));
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniformMatrix3fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		#endif
	}

	void ProgramSeparate::setUniformMatrix4fv(handle uniformHandle, const float* value)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL program pipeline
			GLint openGLProgramPipelineBackup = 0;
			glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &openGLProgramPipelineBackup);

			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniformMatrix4fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);

			// Be polite and restore the previous used OpenGL program pipeline
			glBindProgramPipeline(static_cast<GLuint>(openGLProgramPipelineBackup));
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glBindProgramPipeline(mOpenGLProgramPipeline);
			glActiveShaderProgram(mOpenGLProgramPipeline, mVertexShaderSeparate->getOpenGLShaderProgram());
			glUniformMatrix4fvARB(static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
