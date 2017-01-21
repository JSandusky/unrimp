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
#include "OpenGLRenderer/Shader/Separate/ShaderLanguageSeparate.h"
#include "OpenGLRenderer/Shader/Separate/ProgramSeparate.h"
#include "OpenGLRenderer/Shader/Separate/ProgramSeparateDsa.h"
#include "OpenGLRenderer/Shader/Separate/VertexShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/GeometryShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/FragmentShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/TessellationControlShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/TessellationEvaluationShaderSeparate.h"
#include "OpenGLRenderer/IContext.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRenderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageSeparate::NAME = "GLSL";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t ShaderLanguageSeparate::loadShader(uint32_t shaderType, const char *sourceCode)
	{
		// Create the shader program
		const GLuint openGLProgram = glCreateShaderProgramv(shaderType, 1, &sourceCode);

		// Check the link status
		GLint linked = GL_FALSE;
		glGetObjectParameterivARB(openGLProgram, GL_LINK_STATUS, &linked);
		if (GL_TRUE == linked)
		{
			// All went fine, return the program
			return openGLProgram;
		}
		else
		{
			// Error, failed to compile the shader!
			#ifdef RENDERER_OUTPUT_DEBUG
			{
				// Get the length of the information
				GLint informationLength = 0;
				glGetObjectParameterivARB(openGLProgram, GL_OBJECT_INFO_LOG_LENGTH_ARB, &informationLength);
				if (informationLength > 1)
				{
					// Allocate memory for the information
					GLchar *informationLog = new GLchar[static_cast<uint32_t>(informationLength)];

					// Get the information
					glGetInfoLogARB(openGLProgram, informationLength, nullptr, informationLog);

					// Output the debug string
					RENDERER_OUTPUT_DEBUG_STRING(informationLog)

					// Cleanup information memory
					delete [] informationLog;
				}
			}
			#endif

			// Destroy the program
			// -> A value of 0 for shader will be silently ignored
			glDeleteProgram(openGLProgram);

			// Error!
			return 0;
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageSeparate::ShaderLanguageSeparate(OpenGLRenderer &openGLRenderer) :
		IShaderLanguage(openGLRenderer)
	{
		// Nothing here
	}

	ShaderLanguageSeparate::~ShaderLanguageSeparate()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageSeparate::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader *ShaderLanguageSeparate::createVertexShaderFromBytecode(const Renderer::VertexAttributes& vertexAttributes, const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		// Check whether or not there's vertex shader support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_vertex_shader())
		{
			return new VertexShaderSeparate(openGLRenderer, vertexAttributes, bytecode, numberOfBytes);
		}
		else
		{
			// Error! There's no vertex shader support!
			return nullptr;
		}
	}

	Renderer::IVertexShader *ShaderLanguageSeparate::createVertexShaderFromSourceCode(const Renderer::VertexAttributes& vertexAttributes, const char *sourceCode, const char *, const char *, const char *)
	{
		// Check whether or not there's vertex shader support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_vertex_shader())
		{
			return new VertexShaderSeparate(openGLRenderer, vertexAttributes, sourceCode);
		}
		else
		{
			// Error! There's no vertex shader support!
			return nullptr;
		}
	}

	Renderer::ITessellationControlShader *ShaderLanguageSeparate::createTessellationControlShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		// Check whether or not there's tessellation support
		OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_tessellation_shader())
		{
			return new TessellationControlShaderSeparate(openGLRenderer, bytecode, numberOfBytes);
		}
		else
		{
			// Error! There's no tessellation support!
			return nullptr;
		}
	}

	Renderer::ITessellationControlShader *ShaderLanguageSeparate::createTessellationControlShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		// Check whether or not there's tessellation support
		OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_tessellation_shader())
		{
			return new TessellationControlShaderSeparate(openGLRenderer, sourceCode);
		}
		else
		{
			// Error! There's no tessellation support!
			return nullptr;
		}
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageSeparate::createTessellationEvaluationShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		// Check whether or not there's tessellation support
		OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_tessellation_shader())
		{
			return new TessellationEvaluationShaderSeparate(openGLRenderer, bytecode, numberOfBytes);
		}
		else
		{
			// Error! There's no tessellation support!
			return nullptr;
		}
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageSeparate::createTessellationEvaluationShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		// Check whether or not there's tessellation support
		OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_tessellation_shader())
		{
			return new TessellationEvaluationShaderSeparate(openGLRenderer, sourceCode);
		}
		else
		{
			// Error! There's no tessellation support!
			return nullptr;
		}
	}

	Renderer::IGeometryShader *ShaderLanguageSeparate::createGeometryShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *, const char *, const char *)
	{
		// Check whether or not there's geometry shader support
		OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_geometry_shader4())
		{
			// In modern GLSL, "geometry shader input primitive topology" & "geometry shader output primitive topology" & "number of output vertices" can be directly set within GLSL by writing e.g.
			//   "layout(triangles) in;"
			//   "layout(triangle_strip, max_vertices = 3) out;"
			// -> To be able to support older GLSL versions, we have to provide this information also via OpenGL API functions
			return new GeometryShaderSeparate(openGLRenderer, bytecode, numberOfBytes, gsInputPrimitiveTopology, gsOutputPrimitiveTopology, numberOfOutputVertices);
		}
		else
		{
			// Error! There's no geometry shader support!
			return nullptr;
		}
	}

	Renderer::IGeometryShader *ShaderLanguageSeparate::createGeometryShaderFromSourceCode(const char *sourceCode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *, const char *, const char *)
	{
		// Check whether or not there's geometry shader support
		OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_geometry_shader4())
		{
			// In modern GLSL, "geometry shader input primitive topology" & "geometry shader output primitive topology" & "number of output vertices" can be directly set within GLSL by writing e.g.
			//   "layout(triangles) in;"
			//   "layout(triangle_strip, max_vertices = 3) out;"
			// -> To be able to support older GLSL versions, we have to provide this information also via OpenGL API functions
			return new GeometryShaderSeparate(openGLRenderer, sourceCode, gsInputPrimitiveTopology, gsOutputPrimitiveTopology, numberOfOutputVertices);
		}
		else
		{
			// Error! There's no geometry shader support!
			return nullptr;
		}
	}

	Renderer::IFragmentShader *ShaderLanguageSeparate::createFragmentShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes)
	{
		// Check whether or not there's fragment shader support
		OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_fragment_shader())
		{
			return new FragmentShaderSeparate(openGLRenderer, bytecode, numberOfBytes);
		}
		else
		{
			// Error! There's no fragment shader support!
			return nullptr;
		}
	}

	Renderer::IFragmentShader *ShaderLanguageSeparate::createFragmentShaderFromSourceCode(const char *sourceCode, const char *, const char *, const char *)
	{
		// Check whether or not there's fragment shader support
		OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_fragment_shader())
		{
			return new FragmentShaderSeparate(openGLRenderer, sourceCode);
		}
		else
		{
			// Error! There's no fragment shader support!
			return nullptr;
		}
	}

	Renderer::IProgram *ShaderLanguageSeparate::createProgram(const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes&, Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader)
	{
		OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());

		// A shader can be a null pointer, but if it's not the shader and program language must match!
		// -> Optimization: Comparing the shader language name by directly comparing the pointer address of
		//    the name is safe because we know that we always reference to one and the same name address
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		if (nullptr != vertexShader && vertexShader->getShaderLanguageName() != NAME)
		{
			// Error! Vertex shader language mismatch!
		}
		else if (nullptr != tessellationControlShader && tessellationControlShader->getShaderLanguageName() != NAME)
		{
			// Error! Tessellation control shader language mismatch!
		}
		else if (nullptr != tessellationEvaluationShader && tessellationEvaluationShader->getShaderLanguageName() != NAME)
		{
			// Error! Tessellation evaluation shader language mismatch!
		}
		else if (nullptr != geometryShader && geometryShader->getShaderLanguageName() != NAME)
		{
			// Error! Geometry shader language mismatch!
		}
		else if (nullptr != fragmentShader && fragmentShader->getShaderLanguageName() != NAME)
		{
			// Error! Fragment shader language mismatch!
		}

		// Is "GL_EXT_direct_state_access" there?
		else if (openGLRenderer.getExtensions().isGL_EXT_direct_state_access() || openGLRenderer.getExtensions().isGL_ARB_direct_state_access())
		{
			// Effective direct state access (DSA)
			return new ProgramSeparateDsa(openGLRenderer, rootSignature, static_cast<VertexShaderSeparate*>(vertexShader), static_cast<TessellationControlShaderSeparate*>(tessellationControlShader), static_cast<TessellationEvaluationShaderSeparate*>(tessellationEvaluationShader), static_cast<GeometryShaderSeparate*>(geometryShader), static_cast<FragmentShaderSeparate*>(fragmentShader));
		}
		else
		{
			// Traditional bind version
			return new ProgramSeparate(openGLRenderer, rootSignature, static_cast<VertexShaderSeparate*>(vertexShader), static_cast<TessellationControlShaderSeparate*>(tessellationControlShader), static_cast<TessellationEvaluationShaderSeparate*>(tessellationEvaluationShader), static_cast<GeometryShaderSeparate*>(geometryShader), static_cast<FragmentShaderSeparate*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->releaseReference();
		}
		if (nullptr != tessellationControlShader)
		{
			tessellationControlShader->addReference();
			tessellationControlShader->releaseReference();
		}
		if (nullptr != tessellationEvaluationShader)
		{
			tessellationEvaluationShader->addReference();
			tessellationEvaluationShader->releaseReference();
		}
		if (nullptr != geometryShader)
		{
			geometryShader->addReference();
			geometryShader->releaseReference();
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
} // OpenGLRenderer
