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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public virtual OpenGLES2Renderer::IExtensions methods ]
	//[-------------------------------------------------------]
	// EXT
	inline bool ExtensionsRuntimeLinking::isGL_EXT_texture_compression_s3tc() const
	{
		return mGL_EXT_texture_compression_s3tc;
	}

	inline bool ExtensionsRuntimeLinking::isGL_EXT_texture_compression_dxt1() const
	{
		return mGL_EXT_texture_compression_dxt1;
	}

	inline bool ExtensionsRuntimeLinking::isGL_EXT_texture_compression_latc() const
	{
		return mGL_EXT_texture_compression_latc;
	}

	inline bool ExtensionsRuntimeLinking::isGL_EXT_texture_filter_anisotropic() const
	{
		return mGL_EXT_texture_filter_anisotropic;
	}

	inline bool ExtensionsRuntimeLinking::isGL_EXT_texture_array() const
	{
		return mGL_EXT_texture_array;
	}

	inline bool ExtensionsRuntimeLinking::isGL_EXT_texture_buffer() const
	{
		return mGL_EXT_texture_buffer;
	}

	inline bool ExtensionsRuntimeLinking::isGL_EXT_draw_elements_base_vertex() const
	{
		return mGL_EXT_draw_elements_base_vertex;
	}

	// AMD
	inline bool ExtensionsRuntimeLinking::isGL_AMD_compressed_3DC_texture() const
	{
		return mGL_AMD_compressed_3DC_texture;
	}

	// NV
	inline bool ExtensionsRuntimeLinking::isGL_NV_get_tex_image() const
	{
		return mGL_NV_get_tex_image;
	}

	inline bool ExtensionsRuntimeLinking::isGL_NV_fbo_color_attachments() const
	{
		return mGL_NV_fbo_color_attachments;
	}

	inline bool ExtensionsRuntimeLinking::isGL_NV_draw_buffers() const
	{
		return mGL_NV_draw_buffers;
	}

	inline bool ExtensionsRuntimeLinking::isGL_NV_read_buffer() const
	{
		return mGL_NV_read_buffer;
	}

	// OES
	inline bool ExtensionsRuntimeLinking::isGL_OES_mapbuffer() const
	{
		return mGL_OES_mapbuffer;
	}

	inline bool ExtensionsRuntimeLinking::isGL_OES_element_index_uint() const
	{
		return mGL_OES_element_index_uint;
	}

	inline bool ExtensionsRuntimeLinking::isGL_OES_texture_3D() const
	{
		return mGL_OES_texture_3D;
	}

	inline bool ExtensionsRuntimeLinking::isGL_OES_packed_depth_stencil() const
	{
		return mGL_OES_packed_depth_stencil;
	}

	inline bool ExtensionsRuntimeLinking::isGL_OES_depth24() const
	{
		return mGL_OES_depth24;
	}

	inline bool ExtensionsRuntimeLinking::isGL_OES_depth32() const
	{
		return mGL_OES_depth32;
	}

	inline bool ExtensionsRuntimeLinking::isGL_OES_vertex_half_float() const
	{
		return mGL_OES_vertex_half_float;
	}

	inline bool ExtensionsRuntimeLinking::isGL_OES_vertex_array_object() const
	{
		return mGL_OES_vertex_array_object;
	}

	// ANGLE
	inline bool ExtensionsRuntimeLinking::isGL_ANGLE_framebuffer_blit() const
	{
		return mGL_ANGLE_framebuffer_blit;
	}

	inline bool ExtensionsRuntimeLinking::isGL_ANGLE_framebuffer_multisample() const
	{
		return mGL_ANGLE_framebuffer_multisample;
	}

	// KHR
	inline bool ExtensionsRuntimeLinking::isGL_KHR_debug() const
	{
		return mGL_KHR_debug;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
