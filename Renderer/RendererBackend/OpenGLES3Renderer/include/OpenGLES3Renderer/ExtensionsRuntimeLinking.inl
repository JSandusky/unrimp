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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public virtual OpenGLES3Renderer::IExtensions methods ]
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

	inline bool ExtensionsRuntimeLinking::isGL_EXT_texture_buffer() const
	{
		return mGL_EXT_texture_buffer;
	}

	inline bool ExtensionsRuntimeLinking::isGL_EXT_draw_elements_base_vertex() const
	{
		return mGL_EXT_draw_elements_base_vertex;
	}

	inline bool ExtensionsRuntimeLinking::isGL_EXT_base_instance() const
	{
		return mGL_EXT_base_instance;
	}

	inline bool ExtensionsRuntimeLinking::isGL_EXT_clip_control() const
	{
		return mGL_EXT_clip_control;
	}

	// AMD
	inline bool ExtensionsRuntimeLinking::isGL_AMD_compressed_3DC_texture() const
	{
		return mGL_AMD_compressed_3DC_texture;
	}

	// NV
	inline bool ExtensionsRuntimeLinking::isGL_NV_fbo_color_attachments() const
	{
		return mGL_NV_fbo_color_attachments;
	}

	// OES
	inline bool ExtensionsRuntimeLinking::isGL_OES_element_index_uint() const
	{
		return mGL_OES_element_index_uint;
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

	// KHR
	inline bool ExtensionsRuntimeLinking::isGL_KHR_debug() const
	{
		return mGL_KHR_debug;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
