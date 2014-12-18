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
#include "Direct3D9Renderer/ProgramCg.h"
#include "Direct3D9Renderer/IndexBuffer.h"
#include "Direct3D9Renderer/VertexArray.h"
#include "Direct3D9Renderer/VertexShaderCg.h"
#include "Direct3D9Renderer/FragmentShaderCg.h"
#include "Direct3D9Renderer/CgRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramCg::ProgramCg(Direct3D9Renderer &direct3D9Renderer, VertexShaderCg *vertexShaderCg, FragmentShaderCg *fragmentShaderCg) :
		Program(direct3D9Renderer, InternalResourceType::CG),
		mDirect3D9Renderer(&direct3D9Renderer),
		mVertexShaderCg(vertexShaderCg),
		mFragmentShaderCg(fragmentShaderCg)
	{
		// Add references to the provided shaders
		if (nullptr != mVertexShaderCg)
		{
			mVertexShaderCg->addReference();
		}
		if (nullptr != mFragmentShaderCg)
		{
			mFragmentShaderCg->addReference();
		}
	}

	ProgramCg::~ProgramCg()
	{
		// Release the shader references
		if (nullptr != mVertexShaderCg)
		{
			mVertexShaderCg->release();
		}
		if (nullptr != mFragmentShaderCg)
		{
			mFragmentShaderCg->release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	Renderer::IVertexArray *ProgramCg::createVertexArray(unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, Renderer::IIndexBuffer *indexBuffer)
	{
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		return new VertexArray(*mDirect3D9Renderer, numberOfAttributes, attributes, static_cast<IndexBuffer*>(indexBuffer));
	}

	int ProgramCg::getAttributeLocation(const char *)
	{
		// TODO(co) Implement me
		return -1;
	}

	unsigned int ProgramCg::getUniformBlockIndex(const char *, unsigned int defaultIndex)
	{
		// Direct3D 9 has no uniform buffer support
		return defaultIndex;
	}

	handle ProgramCg::getUniformHandle(const char *uniformName)
	{
		CGparameter cgParameter = (nullptr != mVertexShaderCg) ? cgGetNamedParameter(mVertexShaderCg->getCgProgram(), uniformName) : nullptr;
		if (nullptr == cgParameter)
		{
			if (nullptr != mFragmentShaderCg)
			{
				cgParameter = cgGetNamedParameter(mFragmentShaderCg->getCgProgram(), uniformName);
			}
		}
		return reinterpret_cast<handle>(cgParameter);
	}

	void ProgramCg::setUniform1f(handle uniformHandle, float value)
	{
		cgSetParameter1f(reinterpret_cast<CGparameter>(uniformHandle), value);
	}

	void ProgramCg::setUniform2fv(handle uniformHandle, const float *value)
	{
		cgSetParameter2fv(reinterpret_cast<CGparameter>(uniformHandle), value);
	}

	void ProgramCg::setUniform3fv(handle uniformHandle, const float *value)
	{
		cgSetParameter3fv(reinterpret_cast<CGparameter>(uniformHandle), value);
	}

	void ProgramCg::setUniform4fv(handle uniformHandle, const float *value)
	{
		cgSetParameter4fv(reinterpret_cast<CGparameter>(uniformHandle), value);
	}

	void ProgramCg::setUniformMatrix3fv(handle uniformHandle, const float *value)
	{
		cgSetMatrixParameterfc(reinterpret_cast<CGparameter>(uniformHandle), value);
	}

	void ProgramCg::setUniformMatrix4fv(handle uniformHandle, const float *value)
	{
		cgSetMatrixParameterfc(reinterpret_cast<CGparameter>(uniformHandle), value);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
