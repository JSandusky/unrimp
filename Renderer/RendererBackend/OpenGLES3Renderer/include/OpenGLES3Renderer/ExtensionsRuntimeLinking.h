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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLES3Renderer/IOpenGLES3Context.h"
#include "OpenGLES3Renderer/IExtensions.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{
	class OpenGLES3Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL ES 3 extensions runtime linking
	*/
	class ExtensionsRuntimeLinking : public IExtensions
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLES3Renderer
		*    Owner OpenGL ES 3 renderer instance
		*/
		explicit ExtensionsRuntimeLinking(OpenGLES3Renderer& openGLES3Renderer);

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
	//[ Public virtual OpenGLES3Renderer::IExtensions methods ]
	//[-------------------------------------------------------]
	public:
		///////////////////////////////////////////////////////////
		// Returns whether an extension is supported or not
		///////////////////////////////////////////////////////////
		// EXT
		inline virtual bool isGL_EXT_texture_compression_s3tc() const override;
		inline virtual bool isGL_EXT_texture_compression_dxt1() const override;
		inline virtual bool isGL_EXT_texture_compression_latc() const override;
		inline virtual bool isGL_EXT_texture_buffer() const override;
		inline virtual bool isGL_EXT_draw_elements_base_vertex() const override;
		inline virtual bool isGL_EXT_base_instance() const override;
		// AMD
		inline virtual bool isGL_AMD_compressed_3DC_texture() const override;
		// NV
		inline virtual bool isGL_NV_fbo_color_attachments() const override;
		// OES
		inline virtual bool isGL_OES_element_index_uint() const override;
		inline virtual bool isGL_OES_packed_depth_stencil() const override;
		inline virtual bool isGL_OES_depth24() const override;
		inline virtual bool isGL_OES_depth32() const override;
		// KHR
		inline virtual bool isGL_KHR_debug() const override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit ExtensionsRuntimeLinking(const ExtensionsRuntimeLinking& source) = delete;
		ExtensionsRuntimeLinking& operator =(const ExtensionsRuntimeLinking& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		OpenGLES3Renderer& mOpenGLES3Renderer;	///< Owner OpenGL ES 3 renderer instance
		// EXT
		bool mGL_EXT_texture_compression_s3tc;
		bool mGL_EXT_texture_compression_dxt1;
		bool mGL_EXT_texture_compression_latc;
		bool mGL_EXT_texture_buffer;
		bool mGL_EXT_draw_elements_base_vertex;
		bool mGL_EXT_base_instance;
		// AMD
		bool mGL_AMD_compressed_3DC_texture;
		// NV
		bool mGL_NV_fbo_color_attachments;
		// OES
		bool mGL_OES_element_index_uint;
		bool mGL_OES_packed_depth_stencil;
		bool mGL_OES_depth24;
		bool mGL_OES_depth32;
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

	// GL_EXT_texture_buffer
	FNDEF_EX(void,	glTexBufferEXT,	(GLenum target, GLenum internalformat, GLuint buffer));
	#define glTexBufferEXT	FNPTR(glTexBufferEXT)

	// GL_EXT_draw_elements_base_vertex
	FNDEF_EX(void,	glDrawElementsBaseVertexEXT,			(GLenum mode, GLsizei count, GLenum type, const void* indices, GLint basevertex));
	FNDEF_EX(void,	glDrawElementsInstancedBaseVertexEXT,	(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLint basevertex));
	#define glDrawElementsBaseVertexEXT				FNPTR(glDrawElementsBaseVertexEXT)
	#define glDrawElementsInstancedBaseVertexEXT	FNPTR(glDrawElementsInstancedBaseVertexEXT)

	// GL_EXT_base_instance
	FNDEF_EX(void,	glDrawArraysInstancedBaseInstanceEXT,				(GLenum mode, int first, GLsizei count, GLsizei instancecount, GLuint baseinstance));
	FNDEF_EX(void,	glDrawElementsInstancedBaseInstanceEXT,				(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLuint baseinstance));
	FNDEF_EX(void,	glDrawElementsInstancedBaseVertexBaseInstanceEXT,	(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance));
	#define glDrawArraysInstancedBaseInstanceEXT				FNPTR(glDrawArraysInstancedBaseInstanceEXT)
	#define glDrawElementsInstancedBaseInstanceEXT				FNPTR(glDrawElementsInstancedBaseInstanceEXT)
	#define glDrawElementsInstancedBaseVertexBaseInstanceEXT	FNPTR(glDrawElementsInstancedBaseVertexBaseInstanceEXT)


	//[-------------------------------------------------------]
	//[ AMD definitions                                       ]
	//[-------------------------------------------------------]
	// GL_AMD_compressed_3DC_texture
	#define GL_3DC_X_AMD	0x87F9
	#define GL_3DC_XY_AMD	0x87FA


	//[-------------------------------------------------------]
	//[ NV definitions                                        ]
	//[-------------------------------------------------------]
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


	//[-------------------------------------------------------]
	//[ OES definitions                                       ]
	//[-------------------------------------------------------]
	// GL_OES_element_index_uint
	#define GL_UNSIGNED_INT	0x1405

	// GL_OES_packed_depth_stencil
	#define GL_DEPTH_STENCIL_OES		0x84F9
	#define GL_UNSIGNED_INT_24_8_OES	0x84FA
	#define GL_DEPTH24_STENCIL8_OES		0x88F0

	// GL_OES_depth24
	#define GL_DEPTH_COMPONENT24_OES	0x81A6

	// GL_OES_depth32
	#define GL_DEPTH_COMPONENT32_OES	0x81A7


	//[-------------------------------------------------------]
	//[ KHR definitions                                       ]
	//[-------------------------------------------------------]
	// GL_KHR_debug
	FNDEF_EX(void,	glDebugMessageCallbackKHR,	(GLDEBUGPROCKHR callback, const void* userParam));
	#define glDebugMessageCallbackKHR	FNPTR(glDebugMessageCallbackKHR)


	//[-------------------------------------------------------]
	//[ Undefine helper macro                                 ]
	//[-------------------------------------------------------]
	#undef FNDEF_EX


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLES3Renderer/ExtensionsRuntimeLinking.inl"
