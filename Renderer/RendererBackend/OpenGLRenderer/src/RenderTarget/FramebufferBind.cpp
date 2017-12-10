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
#include "OpenGLRenderer/RenderTarget/FramebufferBind.h"
#include "OpenGLRenderer/Texture/Texture2DArray.h"
#include "OpenGLRenderer/Texture/Texture2D.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	FramebufferBind::FramebufferBind(Renderer::IRenderPass& renderPass, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment) :
		Framebuffer(renderPass, colorFramebufferAttachments, depthStencilFramebufferAttachment)
	{
		// Texture reference handling is done within the base class "Framebuffer"
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(renderPass.getRenderer());

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL framebuffer
			GLint openGLFramebufferBackup = 0;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &openGLFramebufferBackup);
		#endif

		// Create the OpenGL framebuffer
		glGenFramebuffers(1, &mOpenGLFramebuffer);

		// Bind this OpenGL framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, mOpenGLFramebuffer);

		// Loop through all framebuffer color attachments
		const Renderer::FramebufferAttachment* colorFramebufferAttachment    = colorFramebufferAttachments;
		const Renderer::FramebufferAttachment* colorFramebufferAttachmentEnd = colorFramebufferAttachments + mNumberOfColorTextures;
		for (GLenum openGLAttachment = GL_COLOR_ATTACHMENT0; colorFramebufferAttachment < colorFramebufferAttachmentEnd; ++colorFramebufferAttachment, ++openGLAttachment)
		{
			Renderer::ITexture* texture = colorFramebufferAttachment->texture;

			// Security check: Is the given resource owned by this renderer?
			#ifndef OPENGLRENDERER_NO_RENDERERMATCHCHECK
				if (&openGLRenderer != &texture->getRenderer())
				{
					// Output an error message and keep on going in order to keep a reasonable behaviour even in case on an error
					RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: The given color texture at index %d is owned by another renderer instance", colorFramebufferAttachment - colorFramebufferAttachments)

					// Continue, there's no point in trying to do any error handling in here
					continue;
				}
			#endif

			// Evaluate the color texture type
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Set the OpenGL framebuffer color attachment
					const Texture2D* texture2D = static_cast<const Texture2D*>(texture);
					glFramebufferTexture2D(GL_FRAMEBUFFER, openGLAttachment, static_cast<GLenum>((texture2D->getNumberOfMultisamples() > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D), texture2D->getOpenGLTexture(), static_cast<GLint>(colorFramebufferAttachment->mipmapIndex));
					if (!mMultisampleRenderTarget && texture2D->getNumberOfMultisamples() > 1)
					{
						mMultisampleRenderTarget = true;
					}
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Set the OpenGL framebuffer color attachment
					const Texture2DArray* texture2DArray = static_cast<const Texture2DArray*>(texture);
					glFramebufferTextureLayer(GL_FRAMEBUFFER, openGLAttachment, texture2DArray->getOpenGLTexture(), static_cast<GLint>(colorFramebufferAttachment->mipmapIndex), static_cast<GLint>(colorFramebufferAttachment->layerIndex));
					if (!mMultisampleRenderTarget && texture2DArray->getNumberOfMultisamples() > 1)
					{
						mMultisampleRenderTarget = true;
					}
					break;
				}

				case Renderer::ResourceType::ROOT_SIGNATURE:
				case Renderer::ResourceType::RESOURCE_GROUP:
				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::RENDER_PASS:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::INDIRECT_BUFFER:
				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_3D:
				case Renderer::ResourceType::TEXTURE_CUBE:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				default:
					RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "The type of the given color texture at index %ld is not supported by the OpenGL renderer backend", colorFramebufferAttachment - colorFramebufferAttachments)
					break;
			}
		}

		// Depth stencil texture
		if (nullptr != mDepthStencilTexture)
		{
			// Security check: Is the given resource owned by this renderer?
			#ifndef OPENGLRENDERER_NO_RENDERERMATCHCHECK
				if (&openGLRenderer != &mDepthStencilTexture->getRenderer())
				{
					// Output an error message and keep on going in order to keep a reasonable behaviour even in case on an error
					RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: The given depth stencil texture is owned by another renderer instance")
				}
			#endif

			// Evaluate the depth stencil texture type
			switch (mDepthStencilTexture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Sanity check
					RENDERER_ASSERT(renderPass.getRenderer().getContext(), 0 == depthStencilFramebufferAttachment->layerIndex, "Invalid OpenGL depth stencil framebuffer attachment layer index")

					// Bind the depth stencil texture to framebuffer
					const Texture2D* texture2D = static_cast<const Texture2D*>(mDepthStencilTexture);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, static_cast<GLenum>((texture2D->getNumberOfMultisamples() > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D), texture2D->getOpenGLTexture(), static_cast<GLint>(depthStencilFramebufferAttachment->mipmapIndex));
					if (!mMultisampleRenderTarget && texture2D->getNumberOfMultisamples() > 1)
					{
						mMultisampleRenderTarget = true;
					}
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Bind the depth stencil texture to framebuffer
					const Texture2DArray* texture2DArray = static_cast<const Texture2DArray*>(mDepthStencilTexture);
					glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture2DArray->getOpenGLTexture(), static_cast<GLint>(depthStencilFramebufferAttachment->mipmapIndex), static_cast<GLint>(depthStencilFramebufferAttachment->layerIndex));
					if (!mMultisampleRenderTarget && texture2DArray->getNumberOfMultisamples() > 1)
					{
						mMultisampleRenderTarget = true;
					}
					break;
				}

				case Renderer::ResourceType::ROOT_SIGNATURE:
				case Renderer::ResourceType::RESOURCE_GROUP:
				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::RENDER_PASS:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::INDIRECT_BUFFER:
				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_3D:
				case Renderer::ResourceType::TEXTURE_CUBE:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				default:
					RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: The type of the given depth stencil texture is not supported by the OpenGL renderer backend")
					break;
			}
		}

		// Check the status of the OpenGL framebuffer
		const GLenum openGLStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (openGLStatus)
		{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: Not all framebuffer attachment points are framebuffer attachment complete (\"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\")")
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: No images are attached to the framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\")")
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: Incomplete draw buffer framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\")")
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: Incomplete read buffer framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\")")
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: Incomplete multisample framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\")")
				break;

			case GL_FRAMEBUFFER_UNDEFINED:
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: Undefined framebuffer (\"GL_FRAMEBUFFER_UNDEFINED\")")
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED:
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: The combination of internal formats of the attached images violates an implementation-dependent set of restrictions (\"GL_FRAMEBUFFER_UNSUPPORTED\")")
				break;

			// From "GL_EXT_framebuffer_object" (should no longer matter, should)
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: Not all attached images have the same width and height (\"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\")")
				break;

			// From "GL_EXT_framebuffer_object" (should no longer matter, should)
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, "OpenGL error: Incomplete formats framebuffer object (\"GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\")")
				break;

			default:
			case GL_FRAMEBUFFER_COMPLETE:
				// Nothing here
				break;
		}

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
	//[ Public virtual OpenGLRenderer::Framebuffer methods    ]
	//[-------------------------------------------------------]
	void FramebufferBind::generateMipmaps() const
	{
		// Sanity check
		RENDERER_ASSERT(getRenderer().getContext(), mGenerateMipmaps, "OpenGL framebuffer mipmap generation is disabled")

		// TODO(co) Complete, currently only 2D textures are supported
		Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
		for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture)
		{
			// Valid entry?
			if ((*colorTexture)->getResourceType() == Renderer::ResourceType::TEXTURE_2D)
			{
				Texture2D* texture2D = static_cast<Texture2D*>(*colorTexture);
				if (texture2D->getGenerateMipmaps())
				{
					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Backup the currently bound OpenGL texture
						// TODO(co) It's possible to avoid calling this multiple times
						GLint openGLTextureBackup = 0;
						glGetIntegerv(GL_TEXTURE_BINDING_2D, &openGLTextureBackup);
					#endif

					// Generate mipmaps
					glActiveTextureARB(GL_TEXTURE0_ARB);
					glBindTexture(GL_TEXTURE_2D, texture2D->getOpenGLTexture());
					glGenerateMipmap(GL_TEXTURE_2D);

					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Be polite and restore the previous bound OpenGL texture
						glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(openGLTextureBackup));
					#endif
				}
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
