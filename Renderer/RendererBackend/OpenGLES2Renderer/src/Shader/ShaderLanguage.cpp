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
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"	// Must be included before "OpenGLES2Renderer/Shader/ShaderLanguage.h"
#include "OpenGLES2Renderer/Shader/ShaderLanguage.h"
#include "OpenGLES2Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 2 functions


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t ShaderLanguage::loadShader(uint32_t shaderType, const char *shaderSource)
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

					// Ouput the debug string
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
	ShaderLanguage::~ShaderLanguage()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	ShaderLanguage::ShaderLanguage(OpenGLES2Renderer &openGLES2Renderer) :
		IShaderLanguage(openGLES2Renderer)
	{
		// Nothing to do in here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
