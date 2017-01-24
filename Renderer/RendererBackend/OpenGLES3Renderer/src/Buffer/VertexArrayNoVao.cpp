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
#include "OpenGLES3Renderer/Buffer/VertexArrayNoVao.h"
#include "OpenGLES3Renderer/Buffer/IndexBuffer.h"
#include "OpenGLES3Renderer/Buffer/VertexBuffer.h"
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 3 functions
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"

#include <GLES3/gl3.h>

#include <memory.h>
#include <cstring>	// For "memcpy()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArrayNoVao::VertexArrayNoVao(OpenGLES3Renderer &openGLES3Renderer, const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, IndexBuffer *indexBuffer) :
		VertexArray(openGLES3Renderer, indexBuffer),
		mNumberOfAttributes(vertexAttributes.numberOfAttributes),
		mAttributes(mNumberOfAttributes ? new Renderer::VertexAttribute[mNumberOfAttributes] : nullptr),
		mNumberOfVertexBuffers(numberOfVertexBuffers),
		mVertexBuffers(numberOfVertexBuffers ? new Renderer::VertexArrayVertexBuffer[numberOfVertexBuffers] : nullptr)
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
				vertexBuffer->vertexBuffer->releaseReference();
			}

			// Cleanup
			delete [] mVertexBuffers;
		}
	}

	void VertexArrayNoVao::enableOpenGLES3VertexAttribArrays()
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES 3 array buffer
			GLint openGLES2ArrayBufferBackup = 0;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &openGLES2ArrayBufferBackup);
		#endif

		// Loop through all attributes
		// -> We're using "glBindAttribLocation()" when linking the program so we have known attribute locations (the vertex array can't know about the program)
		GLuint attributeLocation = 0;
		const Renderer::VertexAttribute *attributeEnd = mAttributes + mNumberOfAttributes;
		for (const Renderer::VertexAttribute *attribute = mAttributes; attribute < attributeEnd; ++attribute, ++attributeLocation)
		{
			// Set the OpenGL ES 3 vertex attribute pointer
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			const Renderer::VertexArrayVertexBuffer& vertexArrayVertexBuffer = mVertexBuffers[attribute->inputSlot];
			glBindBuffer(GL_ARRAY_BUFFER, static_cast<VertexBuffer*>(vertexArrayVertexBuffer.vertexBuffer)->getOpenGLES3ArrayBuffer());
			glVertexAttribPointer(attributeLocation,
								  Mapping::getOpenGLES3Size(attribute->vertexAttributeFormat),
								  Mapping::getOpenGLES3Type(attribute->vertexAttributeFormat),
								  static_cast<GLboolean>(Mapping::isOpenGLES3VertexAttributeFormatNormalized(attribute->vertexAttributeFormat)),
								  static_cast<GLsizei>(vertexArrayVertexBuffer.strideInBytes),
								  reinterpret_cast<void*>(static_cast<uintptr_t>(attribute->alignedByteOffset)));

			// Enable OpenGL ES 3 vertex attribute array
			glEnableVertexAttribArray(attributeLocation);
		}

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 3 array buffer
			glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(openGLES2ArrayBufferBackup));
		#endif

		// Get the used index buffer
		// -> In case of no index buffer we don't bind buffer 0, there's not really a point in it
		const IndexBuffer *indexBuffer = getIndexBuffer();
		if (nullptr != indexBuffer)
		{
			// Bind OpenGL ES 3 element array buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getOpenGLES3ElementArrayBuffer());
		}
	}

	void VertexArrayNoVao::disableOpenGLES3VertexAttribArrays()
	{
		// No previous bound OpenGL element array buffer restore, there's not really a point in it

		// Loop through all attributes
		// -> We're using "glBindAttribLocation()" when linking the program so we have known attribute locations (the vertex array can't know about the program)
		for (GLuint attributeLocation = 0; attributeLocation < mNumberOfAttributes; ++attributeLocation)
		{
			// Disable OpenGL ES 3 vertex attribute array
			glDisableVertexAttribArray(attributeLocation);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
