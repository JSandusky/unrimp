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
#define EXTENSIONS_DEFINERUNTIMELINKING

#include "OpenGLES2Renderer/ExtensionsRuntimeLinking.h"
#ifdef LINUX
	#include <string.h>
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ExtensionsRuntimeLinking::ExtensionsRuntimeLinking() :
		// EXT
		mGL_EXT_texture_compression_s3tc(false),
		mGL_EXT_texture_compression_dxt1(false),
		mGL_EXT_texture_compression_latc(false),
		mGL_EXT_texture_filter_anisotropic(false),
		mGL_EXT_texture_array(false),
		mGL_EXT_Cg_shader(false),
		// AMD
		mGL_AMD_compressed_3DC_texture(false),
		// NV
		mGL_NV_get_tex_image(false),
		mGL_NV_fbo_color_attachments(false),
		mGL_NV_draw_buffers(false),
		mGL_NV_read_buffer(false),
		// OES
		mGL_OES_mapbuffer(false),
		mGL_OES_element_index_uint(false),
		mGL_OES_texture_3D(false),
		mGL_OES_packed_depth_stencil(false),
		mGL_OES_depth24(false),
		mGL_OES_depth32(false),
		mGL_OES_vertex_half_float(false),
		mGL_OES_vertex_array_object(false),
		// ANGLE
		mGL_ANGLE_framebuffer_blit(false),
		mGL_ANGLE_framebuffer_multisample(false)
	{
		// Nothing to do in here
	}

	ExtensionsRuntimeLinking::~ExtensionsRuntimeLinking()
	{
		// Nothing to do in here
	}

	void ExtensionsRuntimeLinking::initialize()
	{
		// Define a helper macro
		#define IMPORT_FUNC(funcName)																														\
			if (result)																																		\
			{																																				\
				void *symbol = eglGetProcAddress(#funcName);																								\
				if (nullptr != symbol)																														\
				{																																			\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																						\
				}																																			\
				else																																		\
				{																																			\
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 2 error: Failed to locate the entry point \"%s\" within the GLES shared library", #funcName)	\
					result = false;																															\
				}																																			\
			}

		// Get the extensions string
		const char *extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

		//[-------------------------------------------------------]
		//[ EXT                                                   ]
		//[-------------------------------------------------------]
		mGL_EXT_texture_compression_s3tc   = (nullptr != strstr(extensions, "GL_EXT_texture_compression_s3tc"));
		mGL_EXT_texture_compression_dxt1   = (nullptr != strstr(extensions, "GL_EXT_texture_compression_dxt1"));
		mGL_EXT_texture_compression_latc   = (nullptr != strstr(extensions, "GL_EXT_texture_compression_latc"));
		mGL_EXT_texture_filter_anisotropic = (nullptr != strstr(extensions, "GL_EXT_texture_filter_anisotropic"));
		mGL_EXT_texture_array			   = (nullptr != strstr(extensions, "GL_EXT_texture_array"));
		if (mGL_EXT_texture_array)
		{
			// A funny thing: Tegra 2 has support for the extension "GL_EXT_texture_array", but has no support
			// for the "GL_OES_texture_3D"-extension. At least it's not listed in the extension string.
			// "GL_EXT_texture_array" is reusing functions defined by "GL_OES_texture_3D"... and therefore we're
			// now getting the function pointers of "GL_OES_texture_3D" to be able to use "GL_EXT_texture_array".
			// Works even when "GL_OES_texture_3D" is not listed. Nice.

			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glTexImage3DOES)
			IMPORT_FUNC(glTexSubImage3DOES)
			IMPORT_FUNC(glCopyTexSubImage3DOES)
			IMPORT_FUNC(glCompressedTexImage3DOES)
			IMPORT_FUNC(glCompressedTexSubImage3DOES)
			IMPORT_FUNC(glFramebufferTexture3DOES)
			mGL_EXT_texture_array = result;
		}
		mGL_EXT_Cg_shader = (nullptr != strstr(extensions, "GL_EXT_Cg_shader"));

		//[-------------------------------------------------------]
		//[ AMD                                                   ]
		//[-------------------------------------------------------]
		mGL_AMD_compressed_3DC_texture = (nullptr != strstr(extensions, "GL_AMD_compressed_3DC_texture"));

		//[-------------------------------------------------------]
		//[ NV                                                    ]
		//[-------------------------------------------------------]
		mGL_NV_get_tex_image = (nullptr != strstr(extensions, "GL_NV_get_tex_image"));
		if (mGL_NV_get_tex_image)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glGetTexImageNV)
			IMPORT_FUNC(glGetCompressedTexImageNV)
			IMPORT_FUNC(glGetTexLevelParameterfvNV)
			IMPORT_FUNC(glGetTexLevelParameterivNV)
			mGL_NV_get_tex_image = result;
		}
		mGL_NV_fbo_color_attachments = (nullptr != strstr(extensions, "GL_NV_fbo_color_attachments"));
		mGL_NV_draw_buffers = (nullptr != strstr(extensions, "GL_NV_draw_buffers"));
		if (mGL_NV_draw_buffers)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDrawBuffersNV)
			mGL_NV_draw_buffers = result;
		}
		mGL_NV_read_buffer = (nullptr != strstr(extensions, "GL_NV_read_buffer"));
		if (mGL_NV_read_buffer)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glReadBufferNV)
			mGL_NV_read_buffer = result;
		}

		//[-------------------------------------------------------]
		//[ OES                                                   ]
		//[-------------------------------------------------------]
		mGL_OES_mapbuffer = (nullptr != strstr(extensions, "GL_OES_mapbuffer"));
		if (mGL_OES_mapbuffer)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glGetBufferPointervOES)
			IMPORT_FUNC(glMapBufferOES)
			IMPORT_FUNC(glUnmapBufferOES)
			mGL_OES_mapbuffer = result;
		}
		mGL_OES_element_index_uint = (nullptr != strstr(extensions, "GL_OES_element_index_uint"));
		mGL_OES_texture_3D		   = (nullptr != strstr(extensions, "GL_OES_texture_3D"));
		if (mGL_OES_texture_3D && !mGL_EXT_texture_array)
		{	// See "GL_EXT_texture_array"-comment above
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glTexImage3DOES)
			IMPORT_FUNC(glTexSubImage3DOES)
			IMPORT_FUNC(glCopyTexSubImage3DOES)
			IMPORT_FUNC(glCompressedTexImage3DOES)
			IMPORT_FUNC(glCompressedTexSubImage3DOES)
			IMPORT_FUNC(glFramebufferTexture3DOES)
			mGL_OES_texture_3D = result;
		}
		mGL_OES_packed_depth_stencil = (nullptr != strstr(extensions, "GL_OES_packed_depth_stencil"));
		mGL_OES_depth24				 = (nullptr != strstr(extensions, "GL_OES_depth24"));
		mGL_OES_depth32				 = (nullptr != strstr(extensions, "GL_OES_depth32"));
		mGL_OES_vertex_half_float	 = (nullptr != strstr(extensions, "GL_OES_vertex_half_float"));
		mGL_OES_vertex_array_object  = (nullptr != strstr(extensions, "GL_OES_vertex_array_object"));
		if (mGL_OES_vertex_array_object)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glBindVertexArrayOES)
			IMPORT_FUNC(glDeleteVertexArraysOES)
			IMPORT_FUNC(glGenVertexArraysOES)
			mGL_OES_vertex_array_object = result;
		}

		//[-------------------------------------------------------]
		//[ ANGLE                                                 ]
		//[-------------------------------------------------------]
		mGL_ANGLE_framebuffer_blit = (nullptr != strstr(extensions, "GL_ANGLE_framebuffer_blit"));
		if (mGL_ANGLE_framebuffer_blit)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glBlitFramebufferEXT)
			mGL_ANGLE_framebuffer_blit = result;
		}
		mGL_ANGLE_framebuffer_multisample = (nullptr != strstr(extensions, "GL_ANGLE_framebuffer_multisample"));
		if (mGL_ANGLE_framebuffer_multisample)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glRenderbufferStorageMultisampleANGLE)
			mGL_ANGLE_framebuffer_multisample = result;
		}

		// Undefine the helper macro
		#undef IMPORT_FUNC
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
