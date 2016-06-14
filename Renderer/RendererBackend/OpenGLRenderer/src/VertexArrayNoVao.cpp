/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "OpenGLRenderer/VertexArrayNoVao.h"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/IContext.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/IndexBuffer.h"
#include "OpenGLRenderer/VertexBuffer.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
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
	VertexArrayNoVao::VertexArrayNoVao(OpenGLRenderer &openGLRenderer, const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, IndexBuffer *indexBuffer) :
		VertexArray(openGLRenderer, indexBuffer, InternalResourceType::NO_VAO),
		mNumberOfAttributes(vertexAttributes.numberOfAttributes),
		mAttributes(mNumberOfAttributes ? new Renderer::VertexAttribute[mNumberOfAttributes] : nullptr),
		mNumberOfVertexBuffers(numberOfVertexBuffers),
		mVertexBuffers(numberOfVertexBuffers ? new Renderer::VertexArrayVertexBuffer[numberOfVertexBuffers] : nullptr),
		mIsGL_ARB_instanced_arrays(openGLRenderer.getExtensions().isGL_ARB_instanced_arrays())
	{
		// Copy over the data
		if (nullptr != mAttributes)
		{
			memcpy(mAttributes, vertexAttributes.attributes, sizeof(Renderer::VertexAttribute) * mNumberOfAttributes);
		}
		if (nullptr != mVertexBuffers)
		{
			memcpy(mVertexBuffers, vertexBuffers, sizeof(Renderer::VertexArrayVertexBuffer) * mNumberOfVertexBuffers);
		}

		{ // Add a reference to the used vertex buffers
			const Renderer::VertexArrayVertexBuffer *vertexBufferEnd = mVertexBuffers + mNumberOfVertexBuffers;
			for (const Renderer::VertexArrayVertexBuffer *vertexBuffer = mVertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer)
			{
				vertexBuffer->vertexBuffer->addReference();
			}
		}
	}

	VertexArrayNoVao::~VertexArrayNoVao()
	{
		// Destroy the vertex array attributes
		if (nullptr != mAttributes)
		{
			// Cleanup
			delete [] mAttributes;
		}

		// Destroy the vertex array vertex buffers
		if (nullptr != mVertexBuffers)
		{
			// Release the reference to the used vertex buffers
			const Renderer::VertexArrayVertexBuffer *vertexBufferEnd = mVertexBuffers + mNumberOfVertexBuffers;
			for (const Renderer::VertexArrayVertexBuffer *vertexBuffer = mVertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer)
			{
				vertexBuffer->vertexBuffer->release();
			}

			// Cleanup
			delete [] mVertexBuffers;
		}
	}

	void VertexArrayNoVao::enableOpenGLVertexAttribArrays()
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL array buffer
			// -> Using "GL_EXT_direct_state_access" this would not help in here because "glVertexAttribPointerARB" is not specified there :/
			GLint openGLArrayBufferBackup = 0;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING_ARB, &openGLArrayBufferBackup);
		#endif

		// Loop through all attributes
		// -> We're using "glBindAttribLocationARB()" when linking the program so we have known attribute locations (the vertex array can't know about the program)
		GLuint attributeLocation = 0;
		const Renderer::VertexAttribute *attributeEnd = mAttributes + mNumberOfAttributes;
		for (const Renderer::VertexAttribute *attribute = mAttributes; attribute < attributeEnd; ++attribute, ++attributeLocation)
		{
			// Set the OpenGL vertex attribute pointer
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			const Renderer::VertexArrayVertexBuffer& vertexArrayVertexBuffer = mVertexBuffers[attribute->inputSlot];
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, static_cast<VertexBuffer*>(vertexArrayVertexBuffer.vertexBuffer)->getOpenGLArrayBuffer());
			glVertexAttribPointerARB(attributeLocation,
									 Mapping::getOpenGLSize(attribute->vertexAttributeFormat),
									 Mapping::getOpenGLType(attribute->vertexAttributeFormat),
									 static_cast<GLboolean>(Mapping::isOpenGLVertexAttributeFormatNormalized(attribute->vertexAttributeFormat)),
									 static_cast<GLsizei>(vertexArrayVertexBuffer.strideInBytes),
									 reinterpret_cast<GLvoid*>(attribute->alignedByteOffset));

			// Per-instance instead of per-vertex requires "GL_ARB_instanced_arrays"
			if (attribute->instancesPerElement > 0 && mIsGL_ARB_instanced_arrays)
			{
				glVertexAttribDivisorARB(attributeLocation, attribute->instancesPerElement);
			}

			// Enable OpenGL vertex attribute array
			glEnableVertexAttribArrayARB(attributeLocation);
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

	void VertexArrayNoVao::disableOpenGLVertexAttribArrays()
	{
		// No previous bound OpenGL element array buffer restore, there's not really a point in it

		// Loop through all attributes
		// -> We're using "glBindAttribLocationARB()" when linking the program so we have known attribute locations (the vertex array can't know about the program)
		GLuint attributeLocation = 0;
		const Renderer::VertexAttribute *attributeEnd = mAttributes + mNumberOfAttributes;
		for (const Renderer::VertexAttribute *attribute = mAttributes; attribute < attributeEnd; ++attribute, ++attributeLocation)
		{
			// Disable OpenGL vertex attribute array
			glDisableVertexAttribArrayARB(attributeLocation);

			// Per-instance instead of per-vertex requires "GL_ARB_instanced_arrays"
			if (attribute->instancesPerElement > 0 && mIsGL_ARB_instanced_arrays)
			{
				glVertexAttribDivisorARB(attributeLocation, 0);
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
