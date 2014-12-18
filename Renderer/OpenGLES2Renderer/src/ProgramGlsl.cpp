/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "OpenGLES2Renderer/ProgramGlsl.h"
#include "OpenGLES2Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 2 functions
#include "OpenGLES2Renderer/VertexShaderGlsl.h"
#include "OpenGLES2Renderer/FragmentShaderGlsl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramGlsl::ProgramGlsl(OpenGLES2Renderer &openGLES2Renderer, VertexShaderGlsl *vertexShaderGlsl, FragmentShaderGlsl *fragmentShaderGlsl) :
		Program(openGLES2Renderer)
	{
		// Create the OpenGL ES 2 program
		mOpenGLES2Program = glCreateProgram();

		// Attach the shaders to the program
		// -> We don't need to keep a reference to the shader, to add and release at once to ensure a nice behaviour
		if (nullptr != vertexShaderGlsl)
		{
			vertexShaderGlsl->addReference();
			glAttachShader(mOpenGLES2Program, vertexShaderGlsl->getOpenGLES2Shader());
			vertexShaderGlsl->release();
		}
		if (nullptr != fragmentShaderGlsl)
		{
			fragmentShaderGlsl->addReference();
			glAttachShader(mOpenGLES2Program, fragmentShaderGlsl->getOpenGLES2Shader());
			fragmentShaderGlsl->release();
		}

		// Link the program
		glLinkProgram(mOpenGLES2Program);

		// Check the link status
		GLint linked = GL_FALSE;
		glGetProgramiv(mOpenGLES2Program, GL_LINK_STATUS, &linked);
		if (GL_TRUE != linked)
		{
			// Error, program link failed!
			#ifdef RENDERER_OUTPUT_DEBUG
				// Get the length of the information (including a null termination)
				GLint informationLength = 0;
				glGetProgramiv(mOpenGLES2Program, GL_INFO_LOG_LENGTH, &informationLength);
				if (informationLength > 1)
				{
					// Allocate memory for the information
					char *informationLog = new char[static_cast<unsigned int>(informationLength)];

					// Get the information
					glGetProgramInfoLog(mOpenGLES2Program, informationLength, nullptr, informationLog);

					// Ouput the debug string
					RENDERER_OUTPUT_DEBUG_STRING(informationLog)

					// Cleanup information memory
					delete [] informationLog;
				}
			#endif
		}
	}

	ProgramGlsl::~ProgramGlsl()
	{
		// Nothing to do in here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
