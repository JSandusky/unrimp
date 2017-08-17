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
#include "OpenGLES3Renderer/RenderTarget/Framebuffer.h"
#include "OpenGLES3Renderer/Texture/Texture2DArray.h"
#include "OpenGLES3Renderer/Texture/Texture2D.h"
#include "OpenGLES3Renderer/IOpenGLES3Context.h"	// We need to include this header, else the linker won't find our defined OpenGL ES 3 functions
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"

#include <Renderer/ILog.h>

#include <climits> // For UINT_MAX


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Framebuffer::Framebuffer(OpenGLES3Renderer& openGLES3Renderer, uint32_t numberOfColorFramebufferAttachments, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment) :
		IFramebuffer(openGLES3Renderer),
		mOpenGLES3Framebuffer(0),
		mDepthRenderbuffer(0),
		mNumberOfColorTextures(numberOfColorFramebufferAttachments),
		mColorTextures(nullptr),	// Set below
		mDepthStencilTexture(nullptr),
		mWidth(1),
		mHeight(1),
		mGenerateMipmaps(false)
	{
		// Unlike the "GL_ARB_framebuffer_object"-extension of OpenGL, in OpenGL ES 3 all
		// textures attached to the framebuffer must have the same width and height

		// Create the OpenGL ES 3 framebuffer
		glGenFramebuffers(1, &mOpenGLES3Framebuffer);

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES 3 framebuffer
			GLint openGLES3FramebufferBackup = 0;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &openGLES3FramebufferBackup);
		#endif

		// Bind this OpenGL ES 3 framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, mOpenGLES3Framebuffer);

		// Are there any color textures? (usually there are, so we just keep the "glBindFramebuffer()" above without trying to make this method implementation more complex)
		if (mNumberOfColorTextures > 0)
		{
			mColorTextures = new Renderer::ITexture*[mNumberOfColorTextures];

			// Loop through all framebuffer color attachments
			// -> "GL_COLOR_ATTACHMENT0" and "GL_COLOR_ATTACHMENT0_NV" have the same value
			Renderer::ITexture** colorTexture = mColorTextures;
			const Renderer::FramebufferAttachment* colorFramebufferAttachment	 = colorFramebufferAttachments;
			const Renderer::FramebufferAttachment* colorFramebufferAttachmentEnd = colorFramebufferAttachments + numberOfColorFramebufferAttachments;
			for (GLenum openGLES3Attachment = GL_COLOR_ATTACHMENT0; colorFramebufferAttachment < colorFramebufferAttachmentEnd; ++colorFramebufferAttachment, ++openGLES3Attachment, ++colorTexture)
			{
				// Sanity check
				assert(nullptr != colorFramebufferAttachments->texture);

				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*colorTexture = colorFramebufferAttachment->texture;
				(*colorTexture)->addReference();

				// Security check: Is the given resource owned by this renderer?
				#ifndef OPENGLES3RENDERER_NO_RENDERERMATCHCHECK
					if (&openGLES3Renderer != &(*colorTexture)->getRenderer())
					{
						// Output an error message and keep on going in order to keep a reasonable behaviour even in case on an error
						RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "OpenGL ES 3 error: The given color texture at index %d is owned by another renderer instance", colorTexture - mColorTextures)

						// Continue, there's no point in trying to do any error handling in here
						continue;
					}
				#endif

				// Evaluate the color texture type
				switch ((*colorTexture)->getResourceType())
				{
					case Renderer::ResourceType::TEXTURE_2D:
					{
						// Sanity check
						assert(0 == colorFramebufferAttachment->layerIndex);

						// Set the OpenGL ES 3 framebuffer color attachment
						Texture2D* texture2D = static_cast<Texture2D*>(*colorTexture);
						glFramebufferTexture2D(GL_FRAMEBUFFER, openGLES3Attachment, GL_TEXTURE_2D, texture2D->getOpenGLES3Texture(), static_cast<GLint>(colorFramebufferAttachment->mipmapIndex));

						// If this is the primary render target, get the framebuffer width and height
						if (GL_COLOR_ATTACHMENT0 == openGLES3Attachment)
						{
							mWidth  = texture2D->getWidth();
							mHeight = texture2D->getHeight();
						}

						// Generate mipmaps?
						if (texture2D->getGenerateMipmaps())
						{
							mGenerateMipmaps = true;
						}
						break;
					}

					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					{
						// Set the OpenGL ES 3 framebuffer color attachment
						Texture2DArray* texture2DArray = static_cast<Texture2DArray*>(*colorTexture);
						glFramebufferTextureLayer(GL_FRAMEBUFFER, openGLES3Attachment, texture2DArray->getOpenGLES3Texture(), static_cast<GLint>(colorFramebufferAttachment->mipmapIndex), static_cast<GLint>(colorFramebufferAttachment->layerIndex));

						// If this is the primary render target, get the framebuffer width and height
						if (GL_COLOR_ATTACHMENT0 == openGLES3Attachment)
						{
							mWidth  = texture2DArray->getWidth();
							mHeight = texture2DArray->getHeight();
						}
						break;
					}

					case Renderer::ResourceType::ROOT_SIGNATURE:
					case Renderer::ResourceType::RESOURCE_GROUP:
					case Renderer::ResourceType::PROGRAM:
					case Renderer::ResourceType::VERTEX_ARRAY:
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
						RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "The type of the given color texture at index %ld is not supported by the OpenGL ES 3 renderer backend", colorTexture - mColorTextures)
						break;
				}
			}
		}

		// Add a reference to the used depth stencil texture
		if (nullptr != depthStencilFramebufferAttachment)
		{
			mDepthStencilTexture = depthStencilFramebufferAttachment->texture;
			assert(nullptr != mDepthStencilTexture);
			mDepthStencilTexture->addReference();

			// Evaluate the color texture type
			switch (mDepthStencilTexture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Sanity check
					assert(0 == depthStencilFramebufferAttachment->layerIndex);

					// Bind the depth stencil texture to framebuffer
					const Texture2D* texture2D = static_cast<const Texture2D*>(mDepthStencilTexture);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture2D->getOpenGLES3Texture(), static_cast<GLint>(depthStencilFramebufferAttachment->mipmapIndex));

					// Generate mipmaps?
					if (texture2D->getGenerateMipmaps())
					{
						mGenerateMipmaps = true;
					}
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Bind the depth stencil texture to framebuffer
					const Texture2DArray* texture2DArray = static_cast<const Texture2DArray*>(mDepthStencilTexture);
					glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture2DArray->getOpenGLES3Texture(), static_cast<GLint>(depthStencilFramebufferAttachment->mipmapIndex), static_cast<GLint>(depthStencilFramebufferAttachment->layerIndex));
					break;
				}

				case Renderer::ResourceType::ROOT_SIGNATURE:
				case Renderer::ResourceType::RESOURCE_GROUP:
				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
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
					RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "The type of the given depth stencil texture is not supported by the OpenGL ES 3 renderer backend")
					break;
			}
		}

		// Check the status of the OpenGL ES 3 framebuffer
		const GLenum openGLES3Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (openGLES3Status)
		{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "OpenGL ES 3 error: Not all framebuffer attachment points are framebuffer attachment complete (\"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\")")
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "OpenGL ES 3 error: No images are attached to the framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\")")
				break;

		// Not supported by OpenGL ES 3
		//	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		//		RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "OpenGL ES 3 error: Incomplete draw buffer framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\")")
		//		break;

		// Not supported by OpenGL ES 3
		//	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		//		RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "OpenGL ES 3 error: Incomplete read buffer framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\")")
		//		break;

			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "OpenGL ES 3 error: Incomplete multisample framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\")")
				break;

			case GL_FRAMEBUFFER_UNDEFINED:
				RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "OpenGL ES 3 error: Undefined framebuffer (\"GL_FRAMEBUFFER_UNDEFINED\")")
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED:
				RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "OpenGL ES 3 error: The combination of internal formats of the attached images violates an implementation-dependent set of restrictions (\"GL_FRAMEBUFFER_UNSUPPORTED\")")
				break;

			// Not supported by OpenGL ES 3
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "OpenGL ES 3 error: Not all attached images have the same width and height (\"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\")")
				break;

			// Not supported by OpenGL ES 3
			// OpenGL: From "GL_EXT_framebuffer_object" (should no longer matter, should)
		//	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		//		RENDERER_LOG(openGLES3Renderer.getContext(), CRITICAL, "OpenGL ES 3 error: Incomplete formats framebuffer object (\"GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\")")
		//		break;

			default:
			case GL_FRAMEBUFFER_COMPLETE:
				// Nothing here
				break;
		}

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 3 framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(openGLES3FramebufferBackup));
		#endif

		// Validate the framebuffer width and height
		if (0 == mWidth || UINT_MAX == mWidth)
		{
			assert(false);
			mWidth = 1;
		}
		if (0 == mHeight || UINT_MAX == mHeight)
		{
			assert(false);
			mHeight = 1;
		}
	}

	Framebuffer::~Framebuffer()
	{
		// Destroy the OpenGL ES 3 framebuffer
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteFramebuffers(1, &mOpenGLES3Framebuffer);
		glDeleteRenderbuffers(1, &mDepthRenderbuffer);

		// Release the reference to the used color textures
		if (nullptr != mColorTextures)
		{
			// Release references
			Renderer::ITexture** colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture** colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture)
			{
				(*colorTexture)->releaseReference();
			}

			// Cleanup
			delete [] mColorTextures;
		}

		// Release the reference to the used depth stencil texture
		if (nullptr != mDepthStencilTexture)
		{
			// Release reference
			mDepthStencilTexture->releaseReference();
		}
	}

	void Framebuffer::generateMipmaps() const
	{
		// Sanity check
		assert(mGenerateMipmaps);

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
					#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
						// Backup the currently bound OpenGL ES 3 texture
						// TODO(co) It's possible to avoid calling this multiple times
						GLint openGLES3TextureBackup = 0;
						glGetIntegerv(GL_TEXTURE_BINDING_2D, &openGLES3TextureBackup);
					#endif

					// Generate mipmaps
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texture2D->getOpenGLES3Texture());
					glGenerateMipmap(GL_TEXTURE_2D);

					#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
						// Be polite and restore the previous bound OpenGL ES 3 texture
						glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(openGLES3TextureBackup));
					#endif
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void Framebuffer::getWidthAndHeight(uint32_t& width, uint32_t& height) const
	{
		// No fancy implementation in here, just copy over the internal information
		width  = mWidth;
		height = mHeight;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
