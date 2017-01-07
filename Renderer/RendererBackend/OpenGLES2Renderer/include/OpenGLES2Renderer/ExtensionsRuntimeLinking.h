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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLES2Renderer/IContext.h"
#include "OpenGLES2Renderer/IExtensions.h"


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
	*    OpenGL ES 2 extensions runtime linking
	*/
	class ExtensionsRuntimeLinking : public IExtensions
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		ExtensionsRuntimeLinking();

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ExtensionsRuntimeLinking();

		/**
		*  @brief
		*    Initialize the supported extensions
		*
		*  @note
		*    - Do only call this method if the EGL functions initialization was successful ("glGetString()" is used)
		*      and there's an active render context
		*/
		void initialize();


	//[-------------------------------------------------------]
	//[ Public virtual OpenGLES2Renderer::IExtensions methods ]
	//[-------------------------------------------------------]
	public:
		///////////////////////////////////////////////////////////
		// Returns whether an extension is supported or not
		///////////////////////////////////////////////////////////
		// EXT
		inline virtual bool isGL_EXT_texture_compression_s3tc() const override;
		inline virtual bool isGL_EXT_texture_compression_dxt1() const override;
		inline virtual bool isGL_EXT_texture_compression_latc() const override;
		inline virtual bool isGL_EXT_texture_filter_anisotropic() const override;
		inline virtual bool isGL_EXT_texture_array() const override;
		inline virtual bool isGL_EXT_texture_buffer() const override;
		inline virtual bool isGL_EXT_draw_elements_base_vertex() const override;
		// AMD
		inline virtual bool isGL_AMD_compressed_3DC_texture() const override;
		// NV
		inline virtual bool isGL_NV_get_tex_image() const override;
		inline virtual bool isGL_NV_fbo_color_attachments() const override;
		inline virtual bool isGL_NV_draw_buffers() const override;
		inline virtual bool isGL_NV_read_buffer() const override;
		// OES
		inline virtual bool isGL_OES_mapbuffer() const override;
		inline virtual bool isGL_OES_element_index_uint() const override;
		inline virtual bool isGL_OES_texture_3D() const override;
		inline virtual bool isGL_OES_packed_depth_stencil() const override;
		inline virtual bool isGL_OES_depth24() const override;
		inline virtual bool isGL_OES_depth32() const override;
		inline virtual bool isGL_OES_vertex_half_float() const override;
		inline virtual bool isGL_OES_vertex_array_object() const override;
		// ANGLE
		inline virtual bool isGL_ANGLE_framebuffer_blit() const override;
		inline virtual bool isGL_ANGLE_framebuffer_multisample() const override;
		// KHR
		inline virtual bool isGL_KHR_debug() const override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// EXT
		bool mGL_EXT_texture_compression_s3tc;
		bool mGL_EXT_texture_compression_dxt1;
		bool mGL_EXT_texture_compression_latc;
		bool mGL_EXT_texture_filter_anisotropic;
		bool mGL_EXT_texture_array;
		bool mGL_EXT_texture_buffer;
		bool mGL_EXT_draw_elements_base_vertex;
		// AMD
		bool mGL_AMD_compressed_3DC_texture;
		// NV
		bool mGL_NV_get_tex_image;
		bool mGL_NV_fbo_color_attachments;
		bool mGL_NV_draw_buffers;
		bool mGL_NV_read_buffer;
		// OES
		bool mGL_OES_mapbuffer;
		bool mGL_OES_element_index_uint;
		bool mGL_OES_texture_3D;
		bool mGL_OES_packed_depth_stencil;
		bool mGL_OES_depth24;
		bool mGL_OES_depth32;
		bool mGL_OES_vertex_half_float;
		bool mGL_OES_vertex_array_object;
		// ANGLE
		bool mGL_ANGLE_framebuffer_blit;
		bool mGL_ANGLE_framebuffer_multisample;
		// KHR
		bool mGL_KHR_debug;


	};


	//[-------------------------------------------------------]
	//[ Define helper macro                                   ]
	//[-------------------------------------------------------]
	#ifdef EXTENSIONS_DEFINERUNTIMELINKING
		#define FNDEF_EX(retType, funcName, args) retType (GL_APIENTRY *funcPtr_##funcName) args = nullptr
	#else
		#define FNDEF_EX(retType, funcName, args) extern retType (GL_APIENTRY *funcPtr_##funcName) args
	#endif
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif


	//[-------------------------------------------------------]
	//[ EXT definitions                                       ]
	//[-------------------------------------------------------]
	// GL_EXT_texture_compression_s3tc
	#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT		0x83F0
	#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT	0x83F1
	#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT	0x83F2
	#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT	0x83F3

	// GL_EXT_texture_compression_dxt1
	// #define GL_COMPRESSED_RGB_S3TC_DXT1_EXT	0x83F0	// Already defined for GL_EXT_texture_compression_s3tc

	// GL_EXT_texture_compression_latc
	#define GL_COMPRESSED_LUMINANCE_LATC1_EXT				0x8C70
	#define GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT		0x8C71
	#define GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT			0x8C72
	#define GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT	0x8C73

	// GL_EXT_texture_filter_anisotropic
	#define GL_TEXTURE_MAX_ANISOTROPY_EXT		0x84FE
	#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT	0x84FF

	// GL_EXT_texture_array
	#define GL_TEXTURE_1D_ARRAY_EXT						0x8C18
	#define GL_TEXTURE_2D_ARRAY_EXT						0x8C1A
	#define GL_PROXY_TEXTURE_2D_ARRAY_EXT				0x8C1B
	#define GL_PROXY_TEXTURE_1D_ARRAY_EXT				0x8C19
	#define GL_TEXTURE_BINDING_1D_ARRAY_EXT				0x8C1C
	#define GL_TEXTURE_BINDING_2D_ARRAY_EXT				0x8C1D
	#define GL_MAX_ARRAY_TEXTURE_LAYERS_EXT				0x88FF
	#define GL_COMPARE_REF_DEPTH_TO_TEXTURE_EXT			0x884E
	#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT	0x8CD4
	#define GL_SAMPLER_1D_ARRAY_EXT						0x8DC0
	#define GL_SAMPLER_2D_ARRAY_EXT						0x8DC1
	#define GL_SAMPLER_1D_ARRAY_SHADOW_EXT				0x8DC3
	#define GL_SAMPLER_2D_ARRAY_SHADOW_EXT				0x8DC4

	// GL_EXT_texture_buffer
	FNDEF_EX(void,	glTexBufferEXT,	(GLenum target, GLenum internalformat, GLuint buffer));
	#define glTexBufferEXT	FNPTR(glTexBufferEXT)

	// GL_EXT_draw_elements_base_vertex
	FNDEF_EX(void,	glDrawElementsBaseVertexEXT,			(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex));
	FNDEF_EX(void,	glDrawElementsInstancedBaseVertexEXT,	(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex));
	#define glDrawElementsBaseVertexEXT				FNPTR(glDrawElementsBaseVertexEXT)
	#define glDrawElementsInstancedBaseVertexEXT	FNPTR(glDrawElementsInstancedBaseVertexEXT)


	//[-------------------------------------------------------]
	//[ AMD definitions                                       ]
	//[-------------------------------------------------------]
	// GL_AMD_compressed_3DC_texture
	#define GL_3DC_X_AMD	0x87F9
	#define GL_3DC_XY_AMD	0x87FA


	//[-------------------------------------------------------]
	//[ NV definitions                                        ]
	//[-------------------------------------------------------]
	// GL_NV_get_tex_image
	#define GL_TEXTURE_WIDTH_NV					0x1000
	#define GL_TEXTURE_HEIGHT_NV				0x1001
	#define GL_TEXTURE_INTERNAL_FORMAT_NV		0x1003
	#define GL_TEXTURE_COMPONENTS_NV			GL_TEXTURE_INTERNAL_FORMAT_NV
	#define GL_TEXTURE_BORDER_NV				0x1005
	#define GL_TEXTURE_RED_SIZE_NV				0x805C
	#define GL_TEXTURE_GREEN_SIZE_NV			0x805D
	#define GL_TEXTURE_BLUE_SIZE_NV				0x805E
	#define GL_TEXTURE_ALPHA_SIZE_NV			0x805F
	#define GL_TEXTURE_LUMINANCE_SIZE_NV		0x8060
	#define GL_TEXTURE_INTENSITY_SIZE_NV		0x8061
	#define GL_TEXTURE_DEPTH_NV					0x8071
	#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_NV	0x86A0
	#define GL_TEXTURE_COMPRESSED_NV			0x86A1
	#define GL_TEXTURE_DEPTH_SIZE_NV			0x884A
	FNDEF_EX(void,	glGetTexImageNV,			(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *img));
	FNDEF_EX(void,	glGetCompressedTexImageNV,	(GLenum target, GLint level, GLvoid *img));
	FNDEF_EX(void,	glGetTexLevelParameterfvNV,	(GLenum target, GLint level, GLenum pname, GLfloat *params));
	FNDEF_EX(void,	glGetTexLevelParameterivNV,	(GLenum target, GLint level, GLenum pname, GLint *params));
	#define glGetTexImageNV				FNPTR(glGetTexImageNV)
	#define glGetCompressedTexImageNV	FNPTR(glGetCompressedTexImageNV)
	#define glGetTexLevelParameterfvNV	FNPTR(glGetTexLevelParameterfvNV)
	#define glGetTexLevelParameterivNV	FNPTR(glGetTexLevelParameterivNV)

	// GL_NV_fbo_color_attachments
	#define GL_COLOR_ATTACHMENT0_NV		0x8CE0	// Same value as GL_COLOR_ATTACHMENT0
	#define GL_COLOR_ATTACHMENT1_NV		0x8CE1
	#define GL_COLOR_ATTACHMENT2_NV		0x8CE2
	#define GL_COLOR_ATTACHMENT3_NV		0x8CE3
	#define GL_COLOR_ATTACHMENT4_NV		0x8CE4
	#define GL_COLOR_ATTACHMENT5_NV		0x8CE5
	#define GL_COLOR_ATTACHMENT6_NV		0x8CE6
	#define GL_COLOR_ATTACHMENT7_NV		0x8CE7
	#define GL_COLOR_ATTACHMENT8_NV		0x8CE8
	#define GL_COLOR_ATTACHMENT9_NV		0x8CE9
	#define GL_COLOR_ATTACHMENT10_NV	0x8CEA
	#define GL_COLOR_ATTACHMENT11_NV	0x8CEB
	#define GL_COLOR_ATTACHMENT12_NV	0x8CEC
	#define GL_COLOR_ATTACHMENT13_NV	0x8CED
	#define GL_COLOR_ATTACHMENT14_NV	0x8CEE
	#define GL_COLOR_ATTACHMENT15_NV	0x8CEF

	// GL_NV_draw_buffers
	FNDEF_EX(void,	glDrawBuffersNV,	(GLsizei n, const GLenum *bufs));
	#define glDrawBuffersNV	FNPTR(glDrawBuffersNV)

	// GL_NV_read_buffer
	#define GL_READ_BUFFER_NV	0x0C02
	FNDEF_EX(void,	glReadBufferNV,	(GLenum src));
	#define glReadBufferNV	FNPTR(glReadBufferNV)


	//[-------------------------------------------------------]
	//[ OES definitions                                       ]
	//[-------------------------------------------------------]
	// GL_OES_mapbuffer
	#define GL_WRITE_ONLY_OES			0x88B9
	#define GL_BUFFER_ACCESS_OES		0x88BB
	#define GL_BUFFER_MAPPED_OES		0x88BC
	#define GL_BUFFER_MAP_POINTER_OES	0x88BD
	FNDEF_EX(void,		glGetBufferPointervOES,	(GLenum target, GLenum pname, void **params));
	FNDEF_EX(void *,	glMapBufferOES,			(GLenum target, GLenum access));
	FNDEF_EX(GLboolean,	glUnmapBufferOES,		(GLenum target));
	#define glGetBufferPointervOES	FNPTR(glGetBufferPointervOES)
	#define glMapBufferOES			FNPTR(glMapBufferOES)
	#define glUnmapBufferOES		FNPTR(glUnmapBufferOES)

	// GL_OES_element_index_uint
	#define GL_UNSIGNED_INT	0x1405

	// GL_OES_texture_3D
	#define GL_TEXTURE_3D_OES			0x806F
	#define GL_TEXTURE_WRAP_R_OES		0x8072
	#define GL_MAX_3D_TEXTURE_SIZE_OES	0x8073
	#define GL_TEXTURE_BINDING_3D_OES	0x806A
	FNDEF_EX(void,	glTexImage3DOES,				(GLenum target, int level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, int border, GLenum format, GLenum type, const void *pixels));
	FNDEF_EX(void,	glTexSubImage3DOES,				(GLenum target, int level, int xoffset, int yoffset, int zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels));
	FNDEF_EX(void,	glCopyTexSubImage3DOES,			(GLenum target, int level, int xoffset, int yoffset, int zoffset, int x, int y, GLsizei width, GLsizei height));
	FNDEF_EX(void,	glCompressedTexImage3DOES,		(GLenum target, int level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, int border, GLsizei imageSize, const void *data));
	FNDEF_EX(void,	glCompressedTexSubImage3DOES,	(GLenum target, int level, int xoffset, int yoffset, int zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data));
	FNDEF_EX(void,	glFramebufferTexture3DOES,		(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, int level, int zoffset));
	#define glTexImage3DOES					FNPTR(glTexImage3DOES)
	#define glTexSubImage3DOES				FNPTR(glTexSubImage3DOES)
	#define glCopyTexSubImage3DOES			FNPTR(glCopyTexSubImage3DOES)
	#define glCompressedTexImage3DOES		FNPTR(glCompressedTexImage3DOES)
	#define glCompressedTexSubImage3DOES	FNPTR(glCompressedTexSubImage3DOES)
	#define glFramebufferTexture3DOES		FNPTR(glFramebufferTexture3DOES)

	// GL_OES_packed_depth_stencil
	#define GL_DEPTH_STENCIL_OES		0x84F9
	#define GL_UNSIGNED_INT_24_8_OES	0x84FA
	#define GL_DEPTH24_STENCIL8_OES		0x88F0

	// GL_OES_depth24
	#define GL_DEPTH_COMPONENT24_OES	0x81A6

	// GL_OES_depth32
	#define GL_DEPTH_COMPONENT32_OES	0x81A7

	// GL_OES_vertex_array_object
	FNDEF_EX(void,	glBindVertexArrayOES,		(GLuint array));
	FNDEF_EX(void,	glDeleteVertexArraysOES,	(GLsizei n, const GLuint *arrays));
	FNDEF_EX(void,	glGenVertexArraysOES,		(GLsizei n, GLuint *arrays));
	#define glBindVertexArrayOES	FNPTR(glBindVertexArrayOES)
	#define glDeleteVertexArraysOES	FNPTR(glDeleteVertexArraysOES)
	#define glGenVertexArraysOES	FNPTR(glGenVertexArraysOES)


	//[-------------------------------------------------------]
	//[ ANGLE definitions                                     ]
	//[-------------------------------------------------------]
	// GL_ANGLE_framebuffer_blit
	#define GL_READ_FRAMEBUFFER_EXT			0x8CA8
	#define GL_DRAW_FRAMEBUFFER_EXT			0x8CA9
	#define GL_DRAW_FRAMEBUFFER_BINDING_EXT	0x8CA6
	#define GL_READ_FRAMEBUFFER_BINDING_EXT	0x8CAA
	FNDEF_EX(void,	glBlitFramebufferEXT,	(int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, GLbitfield mask, GLenum filter));
	#define glBlitFramebufferEXT	FNPTR(glBlitFramebufferEXT)

	// GL_ANGLE_framebuffer_multisample
	#define GL_RENDERBUFFER_SAMPLES_ANGLE				0x8CAB
	#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_ANGLE	0x8D56
	#define GL_MAX_SAMPLES_ANGLE						0x8D57
	FNDEF_EX(void,	glRenderbufferStorageMultisampleANGLE,	(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height));
	#define glRenderbufferStorageMultisampleANGLE	FNPTR(glRenderbufferStorageMultisampleANGLE)


	//[-------------------------------------------------------]
	//[ KHR definitions                                       ]
	//[-------------------------------------------------------]
	// GL_KHR_debug
	FNDEF_EX(void,	glDebugMessageCallbackKHR,	(GLDEBUGPROCKHR callback, const void *userParam));
	#define glDebugMessageCallbackKHR	FNPTR(glDebugMessageCallbackKHR)


	//[-------------------------------------------------------]
	//[ Undefine helper macro                                 ]
	//[-------------------------------------------------------]
	#undef FNDEF_EX


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLES2Renderer/ExtensionsRuntimeLinking.inl"
