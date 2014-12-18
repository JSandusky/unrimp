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
#include "OpenGLRenderer/VertexArrayNoVao.h"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/Program.h"
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
	VertexArrayNoVao::VertexArrayNoVao(Program &program, unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer) :
		VertexArray(static_cast<OpenGLRenderer&>(program.getRenderer()), indexBuffer, InternalResourceType::NO_VAO),
		mNumberOfAttributes(numberOfAttributes),
		mAttributes(numberOfAttributes ? new Renderer::VertexArrayAttribute[numberOfAttributes] : nullptr),
		mAttributeLocations(numberOfAttributes ? new int[numberOfAttributes] : nullptr),
		mIsGL_ARB_instanced_arrays(static_cast<OpenGLRenderer&>(program.getRenderer()).getContext().getExtensions().isGL_ARB_instanced_arrays())
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
				RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL warning: There's no active vertex attribute with the name \"%s\"\n", attribute->name)
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

	void VertexArrayNoVao::enableOpenGLVertexAttribArrays()
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL array buffer
			// -> Using "GL_EXT_direct_state_access" this would not help in here because "glVertexAttribPointerARB" is not specified there :/
			GLint openGLArrayBufferBackup = 0;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING_ARB, &openGLArrayBufferBackup);
		#endif

		// Loop through all attributes
		const int *attributeLocation = mAttributeLocations;
		const Renderer::VertexArrayAttribute *attributeEnd = mAttributes + mNumberOfAttributes;
		for (const Renderer::VertexArrayAttribute *attribute = mAttributes; attribute < attributeEnd; ++attribute, ++attributeLocation)
		{
			// Is the attribute location valid?
			if (*attributeLocation > -1)
			{
				// Set the OpenGL vertex attribute pointer
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, static_cast<VertexBuffer*>(attribute->vertexBuffer)->getOpenGLArrayBuffer());
				glVertexAttribPointerARB(static_cast<GLuint>(*attributeLocation), Mapping::getOpenGLSize(attribute->vertexArrayFormat), Mapping::getOpenGLType(attribute->vertexArrayFormat), GL_FALSE, static_cast<GLsizei>(attribute->stride), reinterpret_cast<GLvoid*>(attribute->offset));

				// Per-instance instead of per-vertex requires "GL_ARB_instanced_arrays"
				if (attribute->instancesPerElement > 0 && mIsGL_ARB_instanced_arrays)
				{
					glVertexAttribDivisorARB(static_cast<GLuint>(*attributeLocation), attribute->instancesPerElement);
				}

				// Enable OpenGL vertex attribute array
				glEnableVertexAttribArrayARB(static_cast<GLuint>(*attributeLocation));
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

	void VertexArrayNoVao::disableOpenGLVertexAttribArrays()
	{
		// No previous bound OpenGL element array buffer restore, there's not really a point in it

		// Loop through all attributes
		const int *attributeLocation = mAttributeLocations;
		const Renderer::VertexArrayAttribute *attributeEnd = mAttributes + mNumberOfAttributes;
		for (const Renderer::VertexArrayAttribute *attribute = mAttributes; attribute < attributeEnd; ++attribute, ++attributeLocation)
		{
			// Is the attribute location valid?
			if (*attributeLocation > -1)
			{
				// Disable OpenGL vertex attribute array
				glDisableVertexAttribArrayARB(static_cast<GLuint>(*attributeLocation));

				// Per-instance instead of per-vertex requires "GL_ARB_instanced_arrays"
				if (attribute->instancesPerElement > 0 && mIsGL_ARB_instanced_arrays)
				{
					glVertexAttribDivisorARB(static_cast<GLuint>(*attributeLocation), 0);
				}
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
