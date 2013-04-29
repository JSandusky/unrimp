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
#include "OpenGLRenderer/VertexArrayCg.h"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/ProgramCg.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/IndexBuffer.h"
#include "OpenGLRenderer/VertexBuffer.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/CgRuntimeLinking.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"

#include <memory.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArrayCg::VertexArrayCg(ProgramCg &programCg, unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer) :
		VertexArray(static_cast<OpenGLRenderer&>(programCg.getRenderer()), indexBuffer, InternalResourceType::CG),
		mProgramCg(&programCg),
		mNumberOfAttributes(numberOfAttributes),
		mAttributes(numberOfAttributes ? new Renderer::VertexArrayAttribute[numberOfAttributes] : nullptr),
		mCgParameters(numberOfAttributes ? new CGparameter[numberOfAttributes] : nullptr)
	{
		// Acquire our Cg program reference
		mProgramCg->addReference();

		// Copy over the data
		if (nullptr != mAttributes)
		{
			memcpy(mAttributes, attributes, sizeof(Renderer::VertexArrayAttribute) * mNumberOfAttributes);
		}

		// Get the Cg program instance
		CGprogram cgProgram = programCg.getCgProgram();

		// Get the Cg parameters
		CGparameter *cgParameter = mCgParameters;
		const Renderer::VertexArrayAttribute *attributeEnd = attributes + numberOfAttributes;
		for (const Renderer::VertexArrayAttribute *attribute = attributes; attribute < attributeEnd; ++attribute, ++cgParameter)
		{
			// Get the Cg parameter
			*cgParameter = cgGetNamedParameter(cgProgram, attribute->name);
			if (nullptr == cgParameter)
			{
				RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL Cg warning: There's no active vertex attribute with the name \"%s\"\n", attribute->name)
			}

			// Add a reference to the used vertex buffer
			attribute->vertexBuffer->addReference();
		}
	}

	VertexArrayCg::~VertexArrayCg()
	{
		// Destroy the vertex array attributes
		if (nullptr != mAttributes)
		{
			// Release the reference to the used vertex buffers
			const Renderer::VertexArrayAttribute *attributeEnd = mAttributes + mNumberOfAttributes;
			for (const Renderer::VertexArrayAttribute *attribute = mAttributes; attribute < attributeEnd; ++attribute)
			{
				attribute->vertexBuffer->release();
			}

			// Cleanup
			delete [] mAttributes;
		}

		// Destroy the Cg parameters
		if (nullptr != mCgParameters)
		{
			delete [] mCgParameters;
		}

		// Release our Cg program reference
		mProgramCg->release();
	}

	void VertexArrayCg::enableOpenGLVertexAttribArrays()
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL array buffer
			// -> Using "GL_EXT_direct_state_access" this would not help in here because "glVertexAttribPointerARB" is not specified there :/
			GLint openGLArrayBufferBackup = 0;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING_ARB, &openGLArrayBufferBackup);
		#endif

		// Loop through all Cg parameters
		const CGparameter *cgParameter = mCgParameters;
		const Renderer::VertexArrayAttribute *attributeEnd = mAttributes + mNumberOfAttributes;
		for (const Renderer::VertexArrayAttribute *attribute = mAttributes; attribute < attributeEnd; ++attribute, ++cgParameter)
		{
			// Is the Cg parameter valid?
			if (nullptr != cgParameter)
			{
				// Set the OpenGL vertex attribute pointer
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, static_cast<VertexBuffer*>(attribute->vertexBuffer)->getOpenGLArrayBuffer());
				cgGLSetParameterPointer(*cgParameter, Mapping::getOpenGLSize(attribute->vertexArrayFormat), Mapping::getOpenGLType(attribute->vertexArrayFormat), static_cast<GLsizei>(attribute->stride), reinterpret_cast<GLvoid*>(attribute->offset));

				// TODO(co) How to set "glVertexAttribDivisorARB()" for Cg?

				// Enable OpenGL Cg vertex attribute array
				cgGLEnableClientState(*cgParameter);
			}
		}

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL array buffer
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, openGLArrayBufferBackup);
		#endif

		// Get the used index buffer
		// -> In case of no index buffer we don't bind buffer 0, there's not really a point in it
		const IndexBuffer *indexBuffer = getIndexBuffer();
		if (nullptr != indexBuffer)
		{
			// Bind OpenGL element array buffer
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer->getOpenGLElementArrayBuffer());
		}
	}

	void VertexArrayCg::disableOpenGLVertexAttribArrays()
	{
		// No previous bound OpenGL element array buffer restore, there's not really a point in it

		// Loop through all Cg parameters
		const CGparameter *cgParameter    = mCgParameters;
		const CGparameter *cgParameterEnd = mCgParameters + mNumberOfAttributes;
		for (; cgParameter < cgParameterEnd; ++cgParameter)
		{
			// Is the Cg parameter valid?
			if (nullptr != cgParameter)
			{
				// Disable OpenGL Cg vertex attribute array
				cgGLDisableClientState(*cgParameter);
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
