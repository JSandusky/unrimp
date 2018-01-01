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
#include "OpenGLRenderer/Shader/Separate/GeometryShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/ShaderLanguageSeparate.h"
#include "OpenGLRenderer/Extensions.h"

#include <Renderer/IRenderer.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	GeometryShaderSeparate::GeometryShaderSeparate(OpenGLRenderer& openGLRenderer, const Renderer::ShaderBytecode& shaderBytecode, Renderer::GsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology, uint32_t) :
		IGeometryShader(reinterpret_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShaderProgram(ShaderLanguageSeparate::loadShaderProgramFromBytecode(openGLRenderer, GL_GEOMETRY_SHADER_ARB, shaderBytecode))
	{
		// Nothing here
	}

	GeometryShaderSeparate::GeometryShaderSeparate(OpenGLRenderer& openGLRenderer, const char* sourceCode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, Renderer::ShaderBytecode* shaderBytecode) :
		IGeometryShader(reinterpret_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShaderProgram(ShaderLanguageSeparate::loadShaderProgramFromSourceCode(openGLRenderer, GL_GEOMETRY_SHADER_ARB, sourceCode))
	{
		// In modern GLSL, "geometry shader input primitive topology" & "geometry shader output primitive topology" & "number of output vertices" can be directly set within GLSL by writing e.g.
		//   "layout(triangles) in;"
		//   "layout(triangle_strip, max_vertices = 3) out;"
		// -> To be able to support older GLSL versions, we have to provide this information also via OpenGL API functions
		glProgramParameteriARB(mOpenGLShaderProgram, GL_GEOMETRY_INPUT_TYPE_ARB, static_cast<int>(gsInputPrimitiveTopology));	// The "Renderer::GsInputPrimitiveTopology" values directly map to OpenGL constants, do not change them
		glProgramParameteriARB(mOpenGLShaderProgram, GL_GEOMETRY_OUTPUT_TYPE_ARB, static_cast<int>(gsOutputPrimitiveTopology));	// The "Renderer::GsOutputPrimitiveTopology" values directly map to OpenGL constants, do not change them
		glProgramParameteriARB(mOpenGLShaderProgram, GL_GEOMETRY_VERTICES_OUT_ARB, static_cast<GLint>(numberOfOutputVertices));

		// Return shader bytecode, if requested do to so
		if (nullptr != shaderBytecode)
		{
			ShaderLanguageSeparate::shaderSourceCodeToShaderBytecode(openGLRenderer, GL_GEOMETRY_SHADER_ARB, sourceCode, *shaderBytecode);
		}
	}

	GeometryShaderSeparate::~GeometryShaderSeparate()
	{
		// Destroy the OpenGL shader program
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteProgram(mOpenGLShaderProgram);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* GeometryShaderSeparate::getShaderLanguageName() const
	{
		return ShaderLanguageSeparate::NAME;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void GeometryShaderSeparate::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), GeometryShaderSeparate, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
