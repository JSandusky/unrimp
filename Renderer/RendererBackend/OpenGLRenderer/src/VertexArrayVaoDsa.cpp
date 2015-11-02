/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "OpenGLRenderer/VertexArrayVaoDsa.h"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/IContext.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/IndexBuffer.h"
#include "OpenGLRenderer/VertexBuffer.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
	#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#endif
#include "OpenGLRenderer/Shader/Program.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArrayVaoDsa::VertexArrayVaoDsa(Program &program, uint32_t numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, IndexBuffer *indexBuffer) :
		VertexArrayVao(static_cast<OpenGLRenderer&>(program.getRenderer()), numberOfVertexBuffers, vertexBuffers, indexBuffer)
	{
		// Vertex buffer reference handling is done within the base class "VertexArrayVao"

		// Loop through all attributes
		const Renderer::VertexArrayAttribute *attributeEnd = attributes + numberOfAttributes;
		for (const Renderer::VertexArrayAttribute *attribute = attributes; attribute < attributeEnd; ++attribute)
		{
			// Get the attribute location
			const int attributeLocation = program.getAttributeLocation(attribute->name);
			if (attributeLocation > -1)
			{
				// Set the OpenGL vertex attribute pointer
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				const Renderer::VertexArrayVertexBuffer& vertexArrayVertexBuffer = vertexBuffers[attribute->inputSlot];
				glVertexArrayVertexAttribOffsetEXT(mOpenGLVertexArray, static_cast<VertexBuffer*>(vertexArrayVertexBuffer.vertexBuffer)->getOpenGLArrayBuffer(), static_cast<GLuint>(attributeLocation), Mapping::getOpenGLSize(attribute->vertexArrayFormat), Mapping::getOpenGLType(attribute->vertexArrayFormat), GL_FALSE, static_cast<GLsizei>(vertexArrayVertexBuffer.strideInBytes), static_cast<GLintptr>(attribute->alignedByteOffset));

				// Per-instance instead of per-vertex requires "GL_ARB_instanced_arrays"
				if (attribute->instancesPerElement > 0 && static_cast<OpenGLRenderer&>(program.getRenderer()).getContext().getExtensions().isGL_ARB_instanced_arrays())
				{
					// Sadly, DSA has no support for "GL_ARB_instanced_arrays", so, we have to use the bind way
					// -> Keep the bind-horror as local as possible

					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Backup the currently bound OpenGL vertex array
						GLint openGLVertexArrayBackup = 0;
						glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &openGLVertexArrayBackup);
					#endif

					// Bind this OpenGL vertex array
					glBindVertexArray(mOpenGLVertexArray);

					// Set divisor
					glVertexAttribDivisorARB(static_cast<GLuint>(attributeLocation), attribute->instancesPerElement);

				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL vertex array
					glBindVertexArray(openGLVertexArrayBackup);
				#endif
				}

				// Enable OpenGL vertex attribute array
				glEnableVertexArrayAttribEXT(mOpenGLVertexArray, static_cast<GLuint>(attributeLocation));
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL warning: There's no active vertex attribute with the name \"%s\"\n", attribute->name)
			}
		}

		// Check the used index buffer
		// -> In case of no index buffer we don't bind buffer 0, there's not really a point in it
		if (nullptr != indexBuffer)
		{
			// Sadly, DSA has no support for element array buffer, so, we have to use the bind way
			// -> Keep the bind-horror as local as possible

			#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
				// Backup the currently bound OpenGL vertex array
				GLint openGLVertexArrayBackup = 0;
				glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &openGLVertexArrayBackup);

				// Backup the currently bound OpenGL element array buffer
				GLint openGLElementArrayBufferBackup = 0;
				glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB, &openGLElementArrayBufferBackup);
			#endif

			// Bind this OpenGL vertex array
			glBindVertexArray(mOpenGLVertexArray);

			// Bind OpenGL element array buffer
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer->getOpenGLElementArrayBuffer());

			#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous bound OpenGL vertex array
				glBindVertexArray(openGLVertexArrayBackup);

				// Be polite and restore the previous bound OpenGL element array buffer
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, openGLElementArrayBufferBackup);
			#endif
		}
	}

	VertexArrayVaoDsa::~VertexArrayVaoDsa()
	{
		// Vertex buffer reference handling is done within the base class "VertexArrayVao"
		// Nothing to do in here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
