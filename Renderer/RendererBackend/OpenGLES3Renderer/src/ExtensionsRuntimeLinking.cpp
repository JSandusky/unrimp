/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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

#include "OpenGLES3Renderer/ExtensionsRuntimeLinking.h"
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"

#include <Renderer/ILog.h>

#ifdef LINUX
	#include <string.h>
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ExtensionsRuntimeLinking::ExtensionsRuntimeLinking(OpenGLES3Renderer& openGLES3Renderer) :
		mOpenGLES3Renderer(openGLES3Renderer),
		// EXT
		mGL_EXT_texture_compression_s3tc(false),
		mGL_EXT_texture_compression_dxt1(false),
		mGL_EXT_texture_compression_latc(false),
		mGL_EXT_texture_buffer(false),
		mGL_EXT_draw_elements_base_vertex(false),
		mGL_EXT_base_instance(false),
		mGL_EXT_clip_control(false),
		// AMD
		mGL_AMD_compressed_3DC_texture(false),
		// NV
		mGL_NV_fbo_color_attachments(false),
		// OES
		mGL_OES_element_index_uint(false),
		mGL_OES_packed_depth_stencil(false),
		mGL_OES_depth24(false),
		mGL_OES_depth32(false),
		// KHR
		mGL_KHR_debug(false)
	{
		// Nothing here
	}

	ExtensionsRuntimeLinking::~ExtensionsRuntimeLinking()
	{
		// Nothing here
	}

	void ExtensionsRuntimeLinking::initialize()
	{
		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																	\
			if (result)																																					\
			{																																							\
				void* symbol = eglGetProcAddress(#funcName);																											\
				if (nullptr != symbol)																																	\
				{																																						\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																									\
				}																																						\
				else																																					\
				{																																						\
					RENDERER_LOG(mOpenGLES3Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the OpenGL ES 3 shared library", #funcName)	\
					result = false;																																		\
				}																																						\
			}

		// Get the extensions string
		const char* extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

		//[-------------------------------------------------------]
		//[ EXT                                                   ]
		//[-------------------------------------------------------]
		// TODO(co) Review whether or not those extensions are already inside the OpenGL ES 3 core
		mGL_EXT_texture_compression_s3tc = (nullptr != strstr(extensions, "GL_EXT_texture_compression_s3tc"));
		mGL_EXT_texture_compression_dxt1 = (nullptr != strstr(extensions, "GL_EXT_texture_compression_dxt1"));
		mGL_EXT_texture_compression_latc = (nullptr != strstr(extensions, "GL_EXT_texture_compression_latc"));

		// TODO(sw) Core in opengles 3.2
		//mGL_EXT_texture_buffer = (nullptr != strstr(extensions, "GL_EXT_texture_buffer"));
		// TODO(sw) Disabled for now. With mesa 17.1.3 the OpenGLES driver supports version 3.1 + texture buffer. But currently the shader of the Example project supports only the emulation path
		mGL_EXT_texture_buffer = false;
		if (mGL_EXT_texture_buffer)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glTexBufferEXT)
			mGL_EXT_texture_buffer = result;
		}

		// TODO(sw) Core in opengles 3.2
		mGL_EXT_draw_elements_base_vertex = (nullptr != strstr(extensions, "GL_EXT_draw_elements_base_vertex"));
		if (mGL_EXT_draw_elements_base_vertex)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDrawElementsBaseVertexEXT)
			IMPORT_FUNC(glDrawElementsInstancedBaseVertexEXT)
			mGL_EXT_draw_elements_base_vertex = result;
		}

		mGL_EXT_base_instance = (nullptr != strstr(extensions, "GL_EXT_base_instance"));
		if (mGL_EXT_base_instance)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDrawArraysInstancedBaseInstanceEXT)
			IMPORT_FUNC(glDrawElementsInstancedBaseInstanceEXT)
			IMPORT_FUNC(glDrawElementsInstancedBaseVertexBaseInstanceEXT)
			mGL_EXT_base_instance = result;
		}

		mGL_EXT_clip_control = (nullptr != strstr(extensions, "GL_EXT_clip_control"));
		if (mGL_EXT_clip_control)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glClipControlEXT)
			mGL_EXT_clip_control = result;
		}


		//[-------------------------------------------------------]
		//[ AMD                                                   ]
		//[-------------------------------------------------------]
		mGL_AMD_compressed_3DC_texture = (nullptr != strstr(extensions, "GL_AMD_compressed_3DC_texture"));

		//[-------------------------------------------------------]
		//[ NV                                                    ]
		//[-------------------------------------------------------]
		mGL_NV_fbo_color_attachments = (nullptr != strstr(extensions, "GL_NV_fbo_color_attachments"));

		//[-------------------------------------------------------]
		//[ OES                                                   ]
		//[-------------------------------------------------------]
		mGL_OES_element_index_uint	 = (nullptr != strstr(extensions, "GL_OES_element_index_uint"));
		mGL_OES_packed_depth_stencil = (nullptr != strstr(extensions, "GL_OES_packed_depth_stencil"));
		mGL_OES_depth24				 = (nullptr != strstr(extensions, "GL_OES_depth24"));
		mGL_OES_depth32				 = (nullptr != strstr(extensions, "GL_OES_depth32"));

		//[-------------------------------------------------------]
		//[ KHR                                                   ]
		//[-------------------------------------------------------]
		mGL_KHR_debug = (nullptr != strstr(extensions, "GL_KHR_debug"));
		if (mGL_KHR_debug)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDebugMessageCallbackKHR)
			IMPORT_FUNC(glDebugMessageControlKHR)
			IMPORT_FUNC(glDebugMessageInsertKHR)
			IMPORT_FUNC(glPushDebugGroupKHR)
			IMPORT_FUNC(glPopDebugGroupKHR)
			IMPORT_FUNC(glObjectLabelKHR)
			mGL_KHR_debug = result;
		}

		// Undefine the helper macro
		#undef IMPORT_FUNC
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
