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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __OPENGLES2RENDERER_EXTENSIONS_H__
#define __OPENGLES2RENDERER_EXTENSIONS_H__


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract OpenGL ES 2 extensions base interface
	*
	*  @note
	*    - Extensions are only optional, so do always take into account that an extension may not be available
	*/
	class IExtensions
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~IExtensions();


	//[-------------------------------------------------------]
	//[ Public virtual OpenGLES2Renderer::IExtensions methods ]
	//[-------------------------------------------------------]
	public:
		///////////////////////////////////////////////////////////
		// Returns whether an extension is supported or not
		///////////////////////////////////////////////////////////
		// EXT
		virtual bool isGL_EXT_texture_compression_s3tc() const = 0;
		virtual bool isGL_EXT_texture_compression_dxt1() const = 0;
		virtual bool isGL_EXT_texture_compression_latc() const = 0;
		virtual bool isGL_EXT_texture_filter_anisotropic() const = 0;
		virtual bool isGL_EXT_texture_array() const = 0;
		// AMD
		virtual bool isGL_AMD_compressed_3DC_texture() const = 0;
		// NV
		virtual bool isGL_NV_get_tex_image() const = 0;
		virtual bool isGL_NV_fbo_color_attachments() const = 0;
		virtual bool isGL_NV_draw_buffers() const = 0;
		virtual bool isGL_NV_read_buffer() const = 0;
		// OES
		virtual bool isGL_OES_mapbuffer() const = 0;
		virtual bool isGL_OES_element_index_uint() const = 0;
		virtual bool isGL_OES_texture_3D() const = 0;
		virtual bool isGL_OES_packed_depth_stencil() const = 0;
		virtual bool isGL_OES_depth24() const = 0;
		virtual bool isGL_OES_depth32() const = 0;
		virtual bool isGL_OES_vertex_half_float() const = 0;
		virtual bool isGL_OES_vertex_array_object() const = 0;
		// ANGLE
		virtual bool isGL_ANGLE_framebuffer_blit() const = 0;
		virtual bool isGL_ANGLE_framebuffer_multisample() const = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Default constructor
		*/
		IExtensions();

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		explicit IExtensions(const IExtensions &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		IExtensions &operator =(const IExtensions &source);


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer


//[-------------------------------------------------------]
//[ Implementation includes                               ]
//[-------------------------------------------------------]
#include "OpenGLES2Renderer/ExtensionsRuntimeLinking.h"	// Required in here because we define the OpenGL ES 2 extension functions for dynamic runtime linking


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLES2RENDERER_EXTENSIONS_H__
