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
#include "OpenGLES2Renderer/Shader/Program.h"
#include "OpenGLES2Renderer/IContext.h"
#include "OpenGLES2Renderer/IExtensions.h"
#include "OpenGLES2Renderer/IndexBuffer.h"
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"
#include "OpenGLES2Renderer/VertexArrayNoVao.h"
#include "OpenGLES2Renderer/VertexArrayVao.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Program::~Program()
	{
		// Destroy the OpenGL ES 2 program
		// -> A value of 0 for program will be silently ignored
		glDeleteProgram(mOpenGLES2Program);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	Program::Program(OpenGLES2Renderer &openGLES2Renderer) :
		IProgram(openGLES2Renderer),
		mOpenGLES2Program(0)
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	Renderer::IVertexArray *Program::createVertexArray(uint32_t numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, Renderer::IIndexBuffer *indexBuffer)
	{
		// Is "GL_OES_vertex_array_object" there?
		if (static_cast<OpenGLES2Renderer&>(getRenderer()).getContext().getExtensions().isGL_OES_vertex_array_object())
		{
			// Effective vertex array object (VAO)
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new VertexArrayVao(*this, numberOfAttributes, attributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
		}
		else
		{
			// Traditional version
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new VertexArrayNoVao(*this, numberOfAttributes, attributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
		}
	}

	int Program::getAttributeLocation(const char *attributeName)
	{
		return glGetAttribLocation(mOpenGLES2Program, attributeName);
	}

	uint32_t Program::getUniformBlockIndex(const char *, uint32_t defaultIndex)
	{
		// OpenGL ES 2 has no uniform buffer support
		return defaultIndex;
	}

	handle Program::getUniformHandle(const char *uniformName)
	{
		return static_cast<handle>(glGetUniformLocation(mOpenGLES2Program, uniformName));
	}

	uint32_t Program::setTextureUnit(handle uniformHandle, uint32_t unit)
	{
		// OpenGL ES 2/GLSL is not automatically assigning texture units to samplers, so, we have to take over this job
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently used OpenGL ES 2 program
			GLint openGLES2ProgramBackup = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &openGLES2ProgramBackup);
			if (openGLES2ProgramBackup == mOpenGLES2Program)
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUniform1i(static_cast<GLint>(uniformHandle), unit);
			}
			else
			{
				// Set uniform, please note that for this our program must be the currently used one
				glUseProgram(mOpenGLES2Program);
				glUniform1i(static_cast<GLint>(uniformHandle), unit);

				// Be polite and restore the previous used OpenGL ES 2 program
				glUseProgram(openGLES2ProgramBackup);
			}
		#else
			// Set uniform, please note that for this our program must be the currently used one
			glUseProgram(mOpenGLES2Program);
			glUniform1i(static_cast<GLint>(uniformHandle), static_cast<GLint>(unit));
		#endif
		return unit;
	}

	void Program::setUniform1i(handle uniformHandle, int value)
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

	void Program::setUniform1f(handle uniformHandle, float value)
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

	void Program::setUniform2fv(handle uniformHandle, const float *value)
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

	void Program::setUniform3fv(handle uniformHandle, const float *value)
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

	void Program::setUniform4fv(handle uniformHandle, const float *value)
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
	void Program::setUniformMatrix3fv(handle uniformHandle, const float *value)
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

	void Program::setUniformMatrix4fv(handle uniformHandle, const float *value)
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
