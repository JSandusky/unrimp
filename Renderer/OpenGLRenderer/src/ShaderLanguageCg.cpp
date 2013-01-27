/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#include "OpenGLRenderer/ShaderLanguageCg.h"
#include "OpenGLRenderer/IContext.h"
#include "OpenGLRenderer/ProgramCg.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/VertexShaderCg.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/CgRuntimeLinking.h"
#include "OpenGLRenderer/GeometryShaderCg.h"
#include "OpenGLRenderer/FragmentShaderCg.h"
#include "OpenGLRenderer/UniformBufferGlslDsa.h"
#include "OpenGLRenderer/UniformBufferGlslBind.h"
#include "OpenGLRenderer/TessellationControlShaderCg.h"
#include "OpenGLRenderer/TessellationEvaluationShaderCg.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_STRING()"

#include <Cg/cgGL.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageCg::NAME = "Cg";


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	ShaderLanguageCg::ShaderLanguageCg(OpenGLRenderer &openGLRenderer) :
		ShaderLanguage(openGLRenderer),
		mCgContext(cgCreateContext())
	{
		// Is there a valid Cg context?
		if (nullptr != mCgContext)
		{
			// Set the Cg error callback function
			cgSetErrorCallback(cgErrorCallback);
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	ShaderLanguageCg::~ShaderLanguageCg()
	{
		// Is there a valid Cg context?
		if (nullptr != mCgContext)
		{
			// Destroy the Cg context instance
			cgDestroyContext(mCgContext);

			// Set no Cg error callback function
			cgSetErrorCallback(nullptr);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageCg::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader *ShaderLanguageCg::createVertexShader(const char *sourceCode, const char *profile, const char *, const char *)
	{
		return (nullptr != mCgContext) ? new VertexShaderCg(getOpenGLRenderer(), *mCgContext, sourceCode, profile) : nullptr;
	}

	Renderer::ITessellationControlShader *ShaderLanguageCg::createTessellationControlShader(const char *sourceCode, const char *profile, const char *, const char *)
	{
		return (nullptr != mCgContext) ? new TessellationControlShaderCg(getOpenGLRenderer(), *mCgContext, sourceCode, profile) : nullptr;
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageCg::createTessellationEvaluationShader(const char *sourceCode, const char *profile, const char *, const char *)
	{
		return (nullptr != mCgContext) ? new TessellationEvaluationShaderCg(getOpenGLRenderer(), *mCgContext, sourceCode, profile) : nullptr;
	}

	Renderer::IGeometryShader *ShaderLanguageCg::createGeometryShader(const char *sourceCode, Renderer::GsInputPrimitiveTopology::Enum, Renderer::GsOutputPrimitiveTopology::Enum, unsigned int, const char *profile, const char *, const char *)
	{
		return (nullptr != mCgContext) ? new GeometryShaderCg(getOpenGLRenderer(), *mCgContext, sourceCode, profile) : nullptr;
	}

	Renderer::IFragmentShader *ShaderLanguageCg::createFragmentShader(const char *sourceCode, const char *profile, const char *, const char *)
	{
		return (nullptr != mCgContext) ? new FragmentShaderCg(getOpenGLRenderer(), *mCgContext, sourceCode, profile) : nullptr;
	}

	Renderer::IProgram *ShaderLanguageCg::createProgram(Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader)
	{
		// Is there a valid Cg context?
		if (nullptr != mCgContext)
		{
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
			else
			{
				// Create the program instance
				return new ProgramCg(getOpenGLRenderer(), static_cast<VertexShaderCg*>(vertexShader), static_cast<TessellationControlShaderCg*>(tessellationControlShader), static_cast<TessellationEvaluationShaderCg*>(tessellationEvaluationShader), static_cast<GeometryShaderCg*>(geometryShader), static_cast<FragmentShaderCg*>(fragmentShader));
			}

			// Error! Shader language mismatch!
			// -> Ensure a correct reference counter behaviour, even in the situation of an error
			if (nullptr != vertexShader)
			{
				vertexShader->addReference();
				vertexShader->release();
			}
			if (nullptr != tessellationControlShader)
			{
				tessellationControlShader->addReference();
				tessellationControlShader->release();
			}
			if (nullptr != tessellationEvaluationShader)
			{
				tessellationEvaluationShader->addReference();
				tessellationEvaluationShader->release();
			}
			if (nullptr != geometryShader)
			{
				geometryShader->addReference();
				geometryShader->release();
			}
			if (nullptr != fragmentShader)
			{
				fragmentShader->addReference();
				fragmentShader->release();
			}
		}

		// Error!
		return nullptr;
	}

	Renderer::IUniformBuffer *ShaderLanguageCg::createUniformBuffer(unsigned int numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		// TODO(co) Review Cg uniform buffer

		// "GL_ARB_uniform_buffer_object" required
		OpenGLRenderer &openGLRenderer = getOpenGLRenderer();
		if (openGLRenderer.getContext().getExtensions().isGL_ARB_uniform_buffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (openGLRenderer.getContext().getExtensions().isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new UniformBufferGlslDsa(openGLRenderer, numberOfBytes, data, bufferUsage);
			}
			else
			{
				// Traditional bind version
				return new UniformBufferGlslBind(openGLRenderer, numberOfBytes, data, bufferUsage);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}


	//[-------------------------------------------------------]
	//[ Private static Cg callback functions                  ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Internal Cg error callback function
	*/
	void ShaderLanguageCg::cgErrorCallback()
	{
		const CGerror lastCgError = cgGetError();
		if (CG_NO_ERROR != lastCgError)
		{
			// Output error message
			RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL Cg error: %s\n", cgGetErrorString(lastCgError))
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
