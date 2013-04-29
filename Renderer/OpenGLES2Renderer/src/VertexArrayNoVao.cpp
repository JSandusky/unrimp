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
#include "OpenGLES2Renderer/VertexArrayNoVao.h"
#include "OpenGLES2Renderer/Mapping.h"
#include "OpenGLES2Renderer/Program.h"
#include "OpenGLES2Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 2 functions
#include "OpenGLES2Renderer/IndexBuffer.h"
#include "OpenGLES2Renderer/VertexBuffer.h"

#include <GLES2/gl2.h>

#include <memory.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArrayNoVao::VertexArrayNoVao(Program &program, unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer) :
		VertexArray(program.getRenderer(), indexBuffer),
		mNumberOfAttributes(numberOfAttributes),
		mAttributes(numberOfAttributes ? new Renderer::VertexArrayAttribute[numberOfAttributes] : nullptr),
		mAttributeLocations(numberOfAttributes ? new int[numberOfAttributes] : nullptr)
	{
		// Copy over the data
		if (nullptr != mAttributes)
		{
			memcpy(mAttributes, attributes, sizeof(Renderer::VertexArrayAttribute) * mNumberOfAttributes);
		}

		// Get the attribute locations
		int *attributeLocation = mAttributeLocations;
		const Renderer::VertexArrayAttribute *attributeEnd = attributes + numberOfAttributes;
		for (const Renderer::VertexArrayAttribute *attribute = attributes; attribute < attributeEnd; ++attribute, ++attributeLocation)
		{
			// Get the attribute location
			*attributeLocation = program.getAttributeLocation(attribute->name);
			if (*attributeLocation < 0)
			{
				RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 2 warning: There's no active vertex attribute with the name \"%s\"\n", attribute->name)
			}

			// Add a reference to the used vertex buffer
			attribute->vertexBuffer->addReference();
		}
	}

	VertexArrayNoVao::~VertexArrayNoVao()
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

		// Destroy the vertex array attribute locations
		if (nullptr != mAttributeLocations)
		{
			delete [] mAttributeLocations;
		}
	}

	void VertexArrayNoVao::enableOpenGLES2VertexAttribArrays()
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES 2 array buffer
			GLint openGLES2ArrayBufferBackup = 0;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &openGLES2ArrayBufferBackup);
		#endif

		// Loop through all attributes
		const int *attributeLocation = mAttributeLocations;
		const Renderer::VertexArrayAttribute *attributeEnd = mAttributes + mNumberOfAttributes;
		for (const Renderer::VertexArrayAttribute *attribute = mAttributes; attribute < attributeEnd; ++attribute, ++attributeLocation)
		{
			// Is the attribute location valid?
			if (*attributeLocation > -1)
			{
				// Set the OpenGL ES 2 vertex attribute pointer
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				glBindBuffer(GL_ARRAY_BUFFER, static_cast<VertexBuffer*>(attribute->vertexBuffer)->getOpenGLES2ArrayBuffer());
				glVertexAttribPointer(static_cast<GLuint>(*attributeLocation), Mapping::getOpenGLES2Size(attribute->vertexArrayFormat), Mapping::getOpenGLES2Type(attribute->vertexArrayFormat), GL_FALSE, static_cast<GLsizei>(attribute->stride), reinterpret_cast<GLvoid*>(attribute->offset));

				// Enable OpenGL ES 2 vertex attribute array
				glEnableVertexAttribArray(static_cast<GLuint>(*attributeLocation));
			}
		}

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 2 array buffer
			glBindBuffer(GL_ARRAY_BUFFER, openGLES2ArrayBufferBackup);
		#endif

		// Get the used index buffer
		// -> In case of no index buffer we don't bind buffer 0, there's not really a point in it
		const IndexBuffer *indexBuffer = getIndexBuffer();
		if (nullptr != indexBuffer)
		{
			// Bind OpenGL ES 2 element array buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getOpenGLES2ElementArrayBuffer());
		}
	}

	void VertexArrayNoVao::disableOpenGLES2VertexAttribArrays()
	{
		// No previous bound OpenGL element array buffer restore, there's not really a point in it

		// Loop through all attributes
		const int *attributeLocation    = mAttributeLocations;
		const int *attributeLocationEnd = mAttributeLocations + mNumberOfAttributes;
		for (; attributeLocation < attributeLocationEnd; ++attributeLocation)
		{
			// Is the attribute location valid?
			if (*attributeLocation > -1)
			{
				// Disable OpenGL ES 2 vertex attribute array
				glDisableVertexAttribArray(static_cast<GLuint>(*attributeLocation));
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
