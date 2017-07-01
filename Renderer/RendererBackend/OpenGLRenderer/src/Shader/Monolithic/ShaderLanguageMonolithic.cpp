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
#include "OpenGLRenderer/Shader/Monolithic/ShaderLanguageMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/ProgramMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/ProgramMonolithicDsa.h"
#include "OpenGLRenderer/Shader/Monolithic/VertexShaderMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/GeometryShaderMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/FragmentShaderMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/TessellationControlShaderMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/TessellationEvaluationShaderMonolithic.h"
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
	const char* ShaderLanguageMonolithic::NAME = "GLSL";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t ShaderLanguageMonolithic::loadShaderFromSourcecode(uint32_t shaderType, const char* sourceCode)
	{
		// Create the shader object
		const GLuint openGLShader = glCreateShaderObjectARB(shaderType);

		// Load the shader source
		glShaderSourceARB(openGLShader, 1, &sourceCode, nullptr);

		// Compile the shader
		glCompileShaderARB(openGLShader);

		// Check the compile status
		GLint compiled = GL_FALSE;
		glGetObjectParameterivARB(openGLShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
		if (GL_TRUE == compiled)
		{
			// All went fine, return the shader
			return openGLShader;
		}
		else
		{
			// Error, failed to compile the shader!
			#ifdef RENDERER_OUTPUT_DEBUG
				// Get the length of the information
				GLint informationLength = 0;
				glGetObjectParameterivARB(openGLShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &informationLength);
				if (informationLength > 1)
				{
					// Allocate memory for the information
					GLchar* informationLog = new GLchar[static_cast<uint32_t>(informationLength)];

					// Get the information
					glGetInfoLogARB(openGLShader, informationLength, nullptr, informationLog);

					// Output the debug string
					RENDERER_OUTPUT_DEBUG_STRING(informationLog)

					// Cleanup information memory
					delete [] informationLog;
				}
			#endif

			// Destroy the shader
			// -> A value of 0 for shader will be silently ignored
			glDeleteObjectARB(openGLShader);

			// Error!
			return 0;
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageMonolithic::ShaderLanguageMonolithic(OpenGLRenderer& openGLRenderer) :
		IShaderLanguage(openGLRenderer)
	{
		// Nothing here
	}

	ShaderLanguageMonolithic::~ShaderLanguageMonolithic()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char* ShaderLanguageMonolithic::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader* ShaderLanguageMonolithic::createVertexShaderFromBytecode(const Renderer::VertexAttributes&, const Renderer::ShaderBytecode&)
	{
		// Error!
		assert(false && "Monolithic shaders have no shader bytecode, only a monolithic program bytecode");
		return nullptr;
	}

	Renderer::IVertexShader* ShaderLanguageMonolithic::createVertexShaderFromSourceCode(const Renderer::VertexAttributes&, const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode*)
	{
		// Check whether or not there's vertex shader support
		// -> Monolithic shaders have no shader bytecode, only a monolithic program bytecode
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_vertex_shader())
		{
			return new VertexShaderMonolithic(openGLRenderer, shaderSourceCode.sourceCode);
		}
		else
		{
			// Error! There's no vertex shader support!
			return nullptr;
		}
	}

	Renderer::ITessellationControlShader* ShaderLanguageMonolithic::createTessellationControlShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// Error!
		assert(false && "Monolithic shaders have no shader bytecode, only a monolithic program bytecode");
		return nullptr;
	}

	Renderer::ITessellationControlShader* ShaderLanguageMonolithic::createTessellationControlShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode*)
	{
		// Check whether or not there's tessellation support
		// -> Monolithic shaders have no shader bytecode, only a monolithic program bytecode
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_tessellation_shader())
		{
			return new TessellationControlShaderMonolithic(openGLRenderer, shaderSourceCode.sourceCode);
		}
		else
		{
			// Error! There's no tessellation support!
			return nullptr;
		}
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageMonolithic::createTessellationEvaluationShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// Error!
		assert(false && "Monolithic shaders have no shader bytecode, only a monolithic program bytecode");
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageMonolithic::createTessellationEvaluationShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode*)
	{
		// Check whether or not there's tessellation support
		// -> Monolithic shaders have no shader bytecode, only a monolithic program bytecode
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_tessellation_shader())
		{
			return new TessellationEvaluationShaderMonolithic(openGLRenderer, shaderSourceCode.sourceCode);
		}
		else
		{
			// Error! There's no tessellation support!
			return nullptr;
		}
	}

	Renderer::IGeometryShader* ShaderLanguageMonolithic::createGeometryShaderFromBytecode(const Renderer::ShaderBytecode&, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t)
	{
		// Error!
		assert(false && "Monolithic shaders have no shader bytecode, only a monolithic program bytecode");
		return nullptr;
	}

	Renderer::IGeometryShader* ShaderLanguageMonolithic::createGeometryShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, Renderer::ShaderBytecode*)
	{
		// Check whether or not there's geometry shader support
		// -> Monolithic shaders have no shader bytecode, only a monolithic program bytecode
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_geometry_shader4())
		{
			// In modern GLSL, "geometry shader input primitive topology" & "geometry shader output primitive topology" & "number of output vertices" can be directly set within GLSL by writing e.g.
			//   "layout(triangles) in;"
			//   "layout(triangle_strip, max_vertices = 3) out;"
			// -> To be able to support older GLSL versions, we have to provide this information also via OpenGL API functions
			return new GeometryShaderMonolithic(openGLRenderer, shaderSourceCode.sourceCode, gsInputPrimitiveTopology, gsOutputPrimitiveTopology, numberOfOutputVertices);
		}
		else
		{
			// Error! There's no geometry shader support!
			return nullptr;
		}
	}

	Renderer::IFragmentShader* ShaderLanguageMonolithic::createFragmentShaderFromBytecode(const Renderer::ShaderBytecode&)
	{
		// Error!
		assert(false && "Monolithic shaders have no shader bytecode, only a monolithic program bytecode");
		return nullptr;
	}

	Renderer::IFragmentShader* ShaderLanguageMonolithic::createFragmentShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode*)
	{
		// Check whether or not there's fragment shader support
		// -> Monolithic shaders have no shader bytecode, only a monolithic program bytecode
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		if (openGLRenderer.getExtensions().isGL_ARB_fragment_shader())
		{
			return new FragmentShaderMonolithic(openGLRenderer, shaderSourceCode.sourceCode);
		}
		else
		{
			// Error! There's no fragment shader support!
			return nullptr;
		}
	}

	Renderer::IProgram* ShaderLanguageMonolithic::createProgram(const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, Renderer::IVertexShader* vertexShader, Renderer::ITessellationControlShader* tessellationControlShader, Renderer::ITessellationEvaluationShader* tessellationEvaluationShader, Renderer::IGeometryShader* geometryShader, Renderer::IFragmentShader* fragmentShader)
	{
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());

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
			return new ProgramMonolithicDsa(openGLRenderer, rootSignature, vertexAttributes, static_cast<VertexShaderMonolithic*>(vertexShader), static_cast<TessellationControlShaderMonolithic*>(tessellationControlShader), static_cast<TessellationEvaluationShaderMonolithic*>(tessellationEvaluationShader), static_cast<GeometryShaderMonolithic*>(geometryShader), static_cast<FragmentShaderMonolithic*>(fragmentShader));
		}
		else
		{
			// Traditional bind version
			return new ProgramMonolithic(openGLRenderer, rootSignature, vertexAttributes, static_cast<VertexShaderMonolithic*>(vertexShader), static_cast<TessellationControlShaderMonolithic*>(tessellationControlShader), static_cast<TessellationEvaluationShaderMonolithic*>(tessellationEvaluationShader), static_cast<GeometryShaderMonolithic*>(geometryShader), static_cast<FragmentShaderMonolithic*>(fragmentShader));
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
