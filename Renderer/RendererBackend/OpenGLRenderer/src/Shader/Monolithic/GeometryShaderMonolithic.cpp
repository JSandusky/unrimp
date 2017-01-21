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
#include "OpenGLRenderer/Shader/Monolithic/GeometryShaderMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/ShaderLanguageMonolithic.h"
#include "OpenGLRenderer/Extensions.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	GeometryShaderMonolithic::GeometryShaderMonolithic(OpenGLRenderer &openGLRenderer, const uint8_t *, uint32_t, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices) :
		IGeometryShader(reinterpret_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShader(0),
		mOpenGLGsInputPrimitiveTopology(static_cast<int>(gsInputPrimitiveTopology)),	// The "Renderer::GsInputPrimitiveTopology" values directly map to OpenGL constants, do not change them
		mOpenGLGsOutputPrimitiveTopology(static_cast<int>(gsOutputPrimitiveTopology)),	// The "Renderer::GsOutputPrimitiveTopology" values directly map to OpenGL constants, do not change them
		mNumberOfOutputVertices(numberOfOutputVertices)
	{
		// Nothing here
	}

	GeometryShaderMonolithic::GeometryShaderMonolithic(OpenGLRenderer &openGLRenderer, const char *sourceCode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices) :
		IGeometryShader(reinterpret_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShader(ShaderLanguageMonolithic::loadShader(GL_GEOMETRY_SHADER_ARB, sourceCode)),
		mOpenGLGsInputPrimitiveTopology(static_cast<int>(gsInputPrimitiveTopology)),	// The "Renderer::GsInputPrimitiveTopology" values directly map to OpenGL constants, do not change them
		mOpenGLGsOutputPrimitiveTopology(static_cast<int>(gsOutputPrimitiveTopology)),	// The "Renderer::GsOutputPrimitiveTopology" values directly map to OpenGL constants, do not change them
		mNumberOfOutputVertices(numberOfOutputVertices)
	{
		// Nothing here
	}

	GeometryShaderMonolithic::~GeometryShaderMonolithic()
	{
		// Destroy the OpenGL shader
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteObjectARB(mOpenGLShader);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char *GeometryShaderMonolithic::getShaderLanguageName() const
	{
		return ShaderLanguageMonolithic::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
