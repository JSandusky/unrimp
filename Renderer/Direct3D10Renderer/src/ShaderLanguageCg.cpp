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
#include "Direct3D10Renderer/ShaderLanguageCg.h"
#include "Direct3D10Renderer/ProgramCg.h"
#include "Direct3D10Renderer/UniformBuffer.h"
#include "Direct3D10Renderer/VertexShaderCg.h"
#include "Direct3D10Renderer/CgRuntimeLinking.h"
#include "Direct3D10Renderer/GeometryShaderCg.h"
#include "Direct3D10Renderer/FragmentShaderCg.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"

__pragma(warning(push))
	__pragma(warning(disable: 4668))	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <Cg/cgD3D10.h>
__pragma(warning(pop))


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char *ShaderLanguageCg::NAME = "Cg";


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageCg::ShaderLanguageCg(Direct3D10Renderer &direct3D10Renderer) :
		ShaderLanguage(direct3D10Renderer),
		mCgContext(cgCreateContext())
	{
		// Is there a valid Cg context?
		if (nullptr != mCgContext)
		{
			// Set the Cg error callback function
			cgSetErrorCallback(cgErrorCallback);

			// Tell the Cg context about our Direct3D 10 device
			cgD3D10SetDevice(mCgContext, direct3D10Renderer.getD3D10Device());
		}
	}

	ShaderLanguageCg::~ShaderLanguageCg()
	{
		// Is there a valid Cg context?
		if (nullptr != mCgContext)
		{
			// Take back our Direct3D 10 device from the Cg context
			cgD3D10SetDevice(mCgContext, nullptr);

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

	Renderer::IVertexShader *ShaderLanguageCg::createVertexShader(const char *sourceCode, const char *, const char *, const char *)
	{
		return (nullptr != mCgContext) ? new VertexShaderCg(getDirect3D10Renderer(), *mCgContext, sourceCode) : nullptr;
	}

	Renderer::ITessellationControlShader *ShaderLanguageCg::createTessellationControlShader(const char *, const char *, const char *, const char *)
	{
		// Error! Direct3D 10 has no tessellation control shader support.
		return nullptr;
	}

	Renderer::ITessellationEvaluationShader *ShaderLanguageCg::createTessellationEvaluationShader(const char *, const char *, const char *, const char *)
	{
		// Error! Direct3D 10 has no tessellation evaluation shader support.
		return nullptr;
	}

	Renderer::IGeometryShader *ShaderLanguageCg::createGeometryShader(const char *sourceCode, Renderer::GsInputPrimitiveTopology::Enum, Renderer::GsOutputPrimitiveTopology::Enum, unsigned int, const char *, const char *, const char *)
	{
		return (nullptr != mCgContext) ? new GeometryShaderCg(getDirect3D10Renderer(), *mCgContext, sourceCode) : nullptr;
	}

	Renderer::IFragmentShader *ShaderLanguageCg::createFragmentShader(const char *sourceCode, const char *, const char *, const char *)
	{
		return (nullptr != mCgContext) ? new FragmentShaderCg(getDirect3D10Renderer(), *mCgContext, sourceCode) : nullptr;
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
			else if (nullptr != tessellationControlShader)
			{
				// Error! Direct3D 10 has no tessellation control shader support.
			}
			else if (nullptr != tessellationEvaluationShader)
			{
				// Error! Direct3D 10 has no tessellation evaluation shader support.
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
				return new ProgramCg(getDirect3D10Renderer(), static_cast<VertexShaderCg*>(vertexShader), static_cast<GeometryShaderCg*>(geometryShader), static_cast<FragmentShaderCg*>(fragmentShader));
			}

			// Error! Shader language mismatch!
			// -> Ensure a correct reference counter behaviour, even in the situation of an error
			if (nullptr != vertexShader)
			{
				vertexShader->addReference();
				vertexShader->release();
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
		// There's no point in creating a special "UniformBufferCg"-version (tried it at first)
		// -> When creating a Cg buffer we have to use "cgD3D11CreateBuffer()" instead of "cgCreateBuffer()"
		// -> The Cg-documentation states: (http://http.developer.nvidia.com/Cg/cgSetProgramBuffer.html)
		//    "When the next program bind operation occurs, each buffer index which is set to a valid buffer handle is bound (along with the program) for use by the 3D API"
		//    -> This behaviour is not handy, asking Cg for the Direct3D 11 buffer and using it directly is not really working as well
		//    -> Save us all the trouble and just use Direct3D 11 uniform buffers directly, issues resolved
		return new UniformBuffer(getDirect3D10Renderer(), numberOfBytes, data, bufferUsage);
	}


	//[-------------------------------------------------------]
	//[ Private static Cg callback functions                  ]
	//[-------------------------------------------------------]
	void ShaderLanguageCg::cgErrorCallback()
	{
		const CGerror lastCgError = cgGetError();
		if (CG_NO_ERROR != lastCgError)
		{
			// Output error message
			RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 10 Cg error: %s\n", cgGetErrorString(lastCgError))
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
