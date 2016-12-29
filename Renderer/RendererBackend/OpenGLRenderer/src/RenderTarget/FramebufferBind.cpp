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
#include "OpenGLRenderer/RenderTarget/FramebufferBind.h"
#include "OpenGLRenderer/Texture/Texture2D.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	FramebufferBind::FramebufferBind(OpenGLRenderer &openGLRenderer, uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture) :
		Framebuffer(openGLRenderer, numberOfColorTextures, colorTextures, depthStencilTexture)
	{
		// Texture reference handling is done within the base class "Framebuffer"

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL framebuffer
			GLint openGLFramebufferBackup = 0;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &openGLFramebufferBackup);
		#endif

		// Bind this OpenGL framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, mOpenGLFramebuffer);

		// Loop through all framebuffer color attachments
		Renderer::ITexture **colorTexture    = colorTextures;
		Renderer::ITexture **colorTextureEnd = colorTextures + numberOfColorTextures;
		for (GLenum openGLAttachment = GL_COLOR_ATTACHMENT0; colorTexture < colorTextureEnd; ++colorTexture, ++openGLAttachment)
		{
			// Security check: Is the given resource owned by this renderer?
			#ifndef OPENGLRENDERER_NO_RENDERERMATCHCHECK
				if (&openGLRenderer != &(*colorTexture)->getRenderer())
				{
					// Output an error message and keep on going in order to keep a reasonable behaviour even in case on an error
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: The given color texture at index %d is owned by another renderer instance", colorTexture - colorTextures)

					// Continue, there's no point in trying to do any error handling in here
					continue;
				}
			#endif

			// Evaluate the color texture type
			switch ((*colorTexture)->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Set the OpenGL framebuffer color attachment
					const Texture2D* texture2D = static_cast<const Texture2D*>(*colorTexture);
					glFramebufferTexture2D(GL_FRAMEBUFFER, openGLAttachment, static_cast<GLenum>((texture2D->getNumberOfMultisamples() > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D), texture2D->getOpenGLTexture(), 0);
					if (!mMultisampleRenderTarget && texture2D->getNumberOfMultisamples() > 1)
					{
						mMultisampleRenderTarget = true;
					}
					break;
				}

				case Renderer::ResourceType::ROOT_SIGNATURE:
				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::INDIRECT_BUFFER:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				default:
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: The type of the given color texture at index %ld is not supported", colorTexture - colorTextures)
					break;
			}
		}

		// Depth stencil texture
		if (nullptr != depthStencilTexture)
		{
			// Security check: Is the given resource owned by this renderer?
			#ifndef OPENGLRENDERER_NO_RENDERERMATCHCHECK
				if (&openGLRenderer != &depthStencilTexture->getRenderer())
				{
					// Output an error message and keep on going in order to keep a reasonable behaviour even in case on an error
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: The given depth stencil texture is owned by another renderer instance")
				}
			#endif

			// Bind the depth stencil texture to framebuffer
			const Texture2D* texture2D = static_cast<const Texture2D*>(depthStencilTexture);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, static_cast<GLenum>((texture2D->getNumberOfMultisamples() > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D), texture2D->getOpenGLTexture(), 0);
			if (!mMultisampleRenderTarget && texture2D->getNumberOfMultisamples() > 1)
			{
				mMultisampleRenderTarget = true;
			}
		}

		#ifdef RENDERER_OUTPUT_DEBUG
			// Check the status of the OpenGL framebuffer
			const GLenum openGLStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			switch (openGLStatus)
			{
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Not all framebuffer attachment points are framebuffer attachment complete (\"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\")")
					break;

				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: No images are attached to the framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\")")
					break;

				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Incomplete draw buffer framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\")")
					break;

				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Incomplete read buffer framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\")")
					break;

				case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Incomplete multisample framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\")")
					break;

				case GL_FRAMEBUFFER_UNDEFINED:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Undefined framebuffer (\"GL_FRAMEBUFFER_UNDEFINED\")")
					break;

				case GL_FRAMEBUFFER_UNSUPPORTED:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: The combination of internal formats of the attached images violates an implementation-dependent set of restrictions (\"GL_FRAMEBUFFER_UNSUPPORTED\")")
					break;

				// From "GL_EXT_framebuffer_object" (should no longer matter, should)
				case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Not all attached images have the same width and height (\"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\")")
					break;

				// From "GL_EXT_framebuffer_object" (should no longer matter, should)
				case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Incomplete formats framebuffer object (\"GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\")")
					break;

				default:
				case GL_FRAMEBUFFER_COMPLETE:
					// Nothing here
					break;
			}
		#endif

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(openGLFramebufferBackup));
		#endif
	}

	FramebufferBind::~FramebufferBind()
	{
		// Texture reference handling is done within the base class "Framebuffer"
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
