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
#include "Direct3D9Renderer/ShaderLanguageCg.h"
#include "Direct3D9Renderer/ProgramCg.h"
#include "Direct3D9Renderer/VertexShaderCg.h"
#include "Direct3D9Renderer/CgRuntimeLinking.h"
#include "Direct3D9Renderer/FragmentShaderCg.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"

#include <Cg/cgD3D9.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
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
	ShaderLanguageCg::ShaderLanguageCg(Direct3D9Renderer &direct3D9Renderer) :
		ShaderLanguage(direct3D9Renderer),
		mDirect3D9Renderer(&direct3D9Renderer),
		mCGcontext(cgCreateContext())
	{
		// Is there a valid Cg context?
		if (nullptr != mCGcontext)
		{
			// Set the Cg error callback function
			cgSetErrorCallback(cgErrorCallback);

			// Tell the Cg context about our Direct3D 9 device
			cgD3D9SetDevice(direct3D9Renderer.getDirect3DDevice9());
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	ShaderLanguageCg::~ShaderLanguageCg()
	{
		// Is there a valid Cg context?
		if (nullptr != mCGcontext)
		{
			// Take back our Direct3D 9 device from the Cg context
			cgD3D9SetDevice(nullptr);

			// Destroy the Cg context instance
			cgDestroyContext(mCGcontext);

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

	Renderer::IVertexShader *ShaderLanguageCg::createVertexShader(const char *sourceCode, const char *, const char *, const char *)
	{
		return (nullptr != mCGcontext) ? new VertexShaderCg(getDirect3D9Renderer(), *mCGcontext, sourceCode) : nullptr;
	}

	Renderer::ITessellationControlShader *ShaderLanguageCg::createTessellationControlShader(const char *, const char *, const char *, const char *)
	{
		// Error! Direct3D 9 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageCg::createTessellationEvaluationShader(const char *, const char *, const char *, const char *)
	{
		// Error! Direct3D 9 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::IGeometryShader *ShaderLanguageCg::createGeometryShader(const char *, Renderer::GsInputPrimitiveTopology::Enum, Renderer::GsOutputPrimitiveTopology::Enum, unsigned int, const char *, const char *, const char *)
	{
		// Error! Direct3D 9 has no geometry shader support.
		return nullptr;
	}

	Renderer::IFragmentShader *ShaderLanguageCg::createFragmentShader(const char *sourceCode, const char *, const char *, const char *)
	{
		return (nullptr != mCGcontext) ? new FragmentShaderCg(getDirect3D9Renderer(), *mCGcontext, sourceCode) : nullptr;
	}

	Renderer::IProgram *ShaderLanguageCg::createProgram(Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader)
	{
		// Is there a valid Cg context?
		if (nullptr != mCGcontext)
		{
			// A shader can be a null pointer, but if it's not the shader and program language must match!
			// -> Optimization: Comparing the shader language name by directly comparing the pointer address of
			//    the name is safe because we know that we always reference to one and the same name address
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			if (nullptr != vertexShader && vertexShader->getShaderLanguageName() != NAME)
			{
				// Error! Vertex shader language mismatch!
			}
			else if (nullptr != tessellationControlShader)
			{
				// Error! Direct3D 9 has no tessellation control shader support.
			}
			else if (nullptr != tessellationEvaluationShader)
			{
				// Error! Direct3D 9 has no tessellation evaluation shader support.
			}
			else if (nullptr != geometryShader)
			{
				// Error! Direct3D 9 has no geometry shader support.
			}
			else if (nullptr != fragmentShader && fragmentShader->getShaderLanguageName() != NAME)
			{
				// Error! Fragment shader language mismatch!
			}
			else
			{
				// Create the program instance
				return new ProgramCg(*mDirect3D9Renderer, static_cast<VertexShaderCg*>(vertexShader), static_cast<FragmentShaderCg*>(fragmentShader));
			}

			// Error! Shader language mismatch!
			// -> Ensure a correct reference counter behaviour, even in the situation of an error
			if (nullptr != vertexShader)
			{
				vertexShader->addReference();
				vertexShader->release();
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

	Renderer::IUniformBuffer *ShaderLanguageCg::createUniformBuffer(unsigned int, const void *, Renderer::BufferUsage::Enum)
	{
		// Error! Direct3D 9 has no uniform buffer support.
		return nullptr;
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
			RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 9 Cg error: %s\n", cgGetErrorString(lastCgError))
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
