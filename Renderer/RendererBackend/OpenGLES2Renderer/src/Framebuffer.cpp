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
#include "OpenGLES2Renderer/Framebuffer.h"
#include "OpenGLES2Renderer/IContext.h"	// We need to include this header, else the linker won't find our defined OpenGL ES 2 functions
#include "OpenGLES2Renderer/Texture2D.h"
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Framebuffer::Framebuffer(OpenGLES2Renderer &openGLES2Renderer, uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture) :
		IFramebuffer(openGLES2Renderer),
		mOpenGLES2Framebuffer(0),
		mNumberOfColorTextures(numberOfColorTextures),
		mColorTextures(nullptr),	// Set below
		mDepthStencilTexture(depthStencilTexture),
		mWidth(0),
		mHeight(0)
	{
		// Unlike the "GL_ARB_framebuffer_object"-extension of OpenGL, in OpenGL ES 2 all
		// textures attached to the framebuffer must have the same width and height

		// Create the OpenGL ES 2 framebuffer
		glGenFramebuffers(1, &mOpenGLES2Framebuffer);

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES 2 framebuffer
			GLint openGLES2FramebufferBackup = 0;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &openGLES2FramebufferBackup);
		#endif

		// Bind this OpenGL ES 2 framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, mOpenGLES2Framebuffer);

		// Are there any color textures? (usually there are, so we just keep the "glBindFramebuffer()" above without trying to make this method implementation more complex)
		if (mNumberOfColorTextures > 0)
		{
			mColorTextures = new Renderer::ITexture*[mNumberOfColorTextures];

			// Loop through all framebuffer color attachments
			// -> "GL_COLOR_ATTACHMENT0" and "GL_COLOR_ATTACHMENT0_NV" have the same value
			Renderer::ITexture **colorTextureToSetup = mColorTextures;
			Renderer::ITexture **colorTexture		 = colorTextures;
			Renderer::ITexture **colorTextureEnd     = colorTextures + numberOfColorTextures;
			for (GLenum openGLES2Attachment = GL_COLOR_ATTACHMENT0; colorTexture < colorTextureEnd; ++colorTexture, ++openGLES2Attachment, ++colorTextureToSetup)
			{
				// Valid entry?
				if (nullptr != *colorTextures)
				{
					// TODO(co) Add security check: Is the given resource one of the currently used renderer?
					*colorTextureToSetup = *colorTextures;
					(*colorTextureToSetup)->addReference();

					// Security check: Is the given resource owned by this renderer?
					#ifndef OPENGLES2RENDERER_NO_RENDERERMATCHCHECK
						if (&openGLES2Renderer != &(*colorTexture)->getRenderer())
						{
							// Output an error message and keep on going in order to keep a resonable behaviour even in case on an error
							RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 2 error: The given color texture at index %d is owned by another renderer instance", colorTexture - colorTextures)

							// Continue, there's no point in trying to do any error handling in here
							continue;
						}
					#endif

					// Evaluate the color texture type
					switch ((*colorTexture)->getResourceType())
					{
						case Renderer::ResourceType::TEXTURE_2D:
						{
							Texture2D *texture2D = static_cast<Texture2D*>(*colorTexture);

							// Set the OpenGL ES 2 framebuffer color attachment
							glFramebufferTexture2D(GL_FRAMEBUFFER, openGLES2Attachment, GL_TEXTURE_2D, texture2D->getOpenGLES2Texture(), 0);

							// If this is the primary render target, get the framebuffer width and height
							if (GL_COLOR_ATTACHMENT0 == openGLES2Attachment)
							{
								mWidth  = texture2D->getWidth();
								mHeight = texture2D->getHeight();
							}
							break;
						}

						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
						case Renderer::ResourceType::SWAP_CHAIN:
						case Renderer::ResourceType::FRAMEBUFFER:
						case Renderer::ResourceType::INDEX_BUFFER:
						case Renderer::ResourceType::VERTEX_BUFFER:
						case Renderer::ResourceType::UNIFORM_BUFFER:
						case Renderer::ResourceType::TEXTURE_BUFFER:
						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
						case Renderer::ResourceType::RASTERIZER_STATE:
						case Renderer::ResourceType::DEPTH_STENCIL_STATE:
						case Renderer::ResourceType::BLEND_STATE:
						case Renderer::ResourceType::SAMPLER_STATE:
						case Renderer::ResourceType::VERTEX_SHADER:
						case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
						case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
						case Renderer::ResourceType::GEOMETRY_SHADER:
						case Renderer::ResourceType::FRAGMENT_SHADER:
						case Renderer::ResourceType::TEXTURE_COLLECTION:
						case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
						default:
							RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 2 error: The type of the given color texture at index %d is not supported", colorTexture - colorTextures)
							break;
					}
				}
				else
				{
					*colorTexture = nullptr;
				}
			}
		}

		// Add a reference to the used depth stencil texture
		if (nullptr != mDepthStencilTexture)
		{
			mDepthStencilTexture->addReference();

			// TODO(co) Depth texture support
		}

		#ifdef RENDERER_OUTPUT_DEBUG
			// Check the status of the OpenGL ES 2 framebuffer
			const GLenum openGLES2Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			switch (openGLES2Status)
			{
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Not all framebuffer attachment points are framebuffer attachment complete (\"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\")")
					break;

				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: No images are attached to the framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\")")
					break;

			// Not supported by OpenGL ES 2
			//	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			//		RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Incomplete draw buffer framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\")")
			//		break;

			// Not supported by OpenGL ES 2
			//	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			//		RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Incomplete read buffer framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\")")
			//		break;

			// Not supported by OpenGL ES 2
			//	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			//		RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Incomplete multisample framebuffer (\"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\")")
			//		break;

			// Not supported by OpenGL ES 2
			//	case GL_FRAMEBUFFER_UNDEFINED:
			//		RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Undefined framebuffer (\"GL_FRAMEBUFFER_UNDEFINED\")")
			//		break;

				case GL_FRAMEBUFFER_UNSUPPORTED:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: The combination of internal formats of the attached images violates an implementation-dependent set of restrictions (\"GL_FRAMEBUFFER_UNSUPPORTED\")")
					break;

				// Not supported by OpenGL ES 2
				case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Not all attached images have the same width and height (\"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\")")
					break;

				// Not supported by OpenGL ES 2
				// OpenGL: From "GL_EXT_framebuffer_object" (should no longer matter, should)
			//	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			//		RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Incomplete formats framebuffer object (\"GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\")")
			//		break;

				default:
				case GL_FRAMEBUFFER_COMPLETE:
					// Nothing to do in here
					break;
			}
		#endif

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 2 framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, openGLES2FramebufferBackup);
		#endif
	}

	Framebuffer::~Framebuffer()
	{
		// Destroy the OpenGL ES 2 framebuffer
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteFramebuffers(1, &mOpenGLES2Framebuffer);

		// Release the reference to the used color textures
		if (nullptr != mColorTextures)
		{
			// Release references
			Renderer::ITexture **colorTexturesEnd = mColorTextures + mNumberOfColorTextures;
			for (Renderer::ITexture **colorTexture = mColorTextures; colorTexture < colorTexturesEnd; ++colorTexture)
			{
				// Valid entry?
				if (nullptr != *colorTexture)
				{
					(*colorTexture)->release();
				}
			}

			// Cleanup
			delete [] mColorTextures;
		}

		// Release the reference to the used depth stencil texture
		if (nullptr != mDepthStencilTexture)
		{
			// Release reference
			mDepthStencilTexture->release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void Framebuffer::getWidthAndHeight(uint32_t &width, uint32_t &height) const
	{
		// No fancy implementation in here, just copy over the internal information
		width  = mWidth;
		height = mHeight;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
