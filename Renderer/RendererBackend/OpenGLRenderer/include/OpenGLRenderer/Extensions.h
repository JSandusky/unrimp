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
#ifdef WIN32
	// Disable warnings in external headers, we can't fix them
	__pragma(warning(push))
		__pragma(warning(disable: 4668))	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
		#include <GL/gl.h>
		#include <GL/glext.h>	// Requires definitions from "gl.h"
		#include <GL/wglext.h>	// Requires definitions from "gl.h"
	__pragma(warning(pop))
#else
	#ifdef LINUX
		#include <GL/glx.h>
		#include <GL/glxext.h>
		// Undefine None, this name is used inside enums defined by unrimp (Which gets defined inside Xlib.h pulled in by glx.h) 
		#ifdef None
			#undef None
		#endif
	#endif
	#include <GL/gl.h>
	#include <GL/glext.h>	// Requires definitions from "gl.h"
#endif
#include "Renderer/PlatformTypes.h"

// Get rid of some nasty OS macros
#undef max


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{
	class IContext;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Supported OpenGL graphic card extensions
	*
	*  @remarks
	*    You must check if the extension is supported by the current hardware before
	*    you use it. If the extension isn't available you should offer an alternative
	*    technique aka fallback.
	*
	*  @see
	*    - OpenGL extension registry at http://oss.sgi.com/projects/ogl-sample/registry/ for more information about
	*      the different extensions
	*/
	class Extensions
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] context
		*    Owner context
		*/
		explicit Extensions(IContext &context);

		/**
		*  @brief
		*    Destructor
		*
		*  @note
		*    - Not virtual by intent
		*/
		~Extensions();

		/**
		*  @brief
		*    Returns whether the extensions are initialized or not
		*
		*  @return
		*    "true" if the extension are initialized, else "false"
		*/
		bool isInitialized() const;

		///////////////////////////////////////////////////////////
		// Returns whether an extension is supported or not
		///////////////////////////////////////////////////////////
		// WGL (Windows only)
		bool isWGL_ARB_extensions_string() const;
		bool isWGL_EXT_swap_control() const;
		bool isWGL_ARB_pixel_format() const;
		bool isWGL_ARB_render_texture() const;
		bool isWGL_ARB_make_current_read() const;
		bool isWGL_ARB_multisample() const;			// "GLX_ARB_multisample" under Linux
		bool isWGL_ATI_pixel_format_float() const;	// "GLX_ATI_pixel_format_float" under Linux
		bool isWGL_NV_float_buffer() const;			// "GLX_NV_float_buffer" under Linux
		// GLX (Linux only)
		bool isGLX_SGI_swap_control() const;
		// EXT
		bool isGL_EXT_compiled_vertex_array() const;
		bool isGL_EXT_draw_range_elements() const;
		bool isGL_EXT_fog_coord() const;
		bool isGL_EXT_secondary_color() const;
		bool isGL_EXT_texture_compression_s3tc() const;
		bool isGL_EXT_texture_compression_latc() const;
		bool isGL_EXT_texture_lod_bias() const;
		bool isGL_EXT_texture_filter_anisotropic() const;
		bool isGL_EXT_separate_specular_color() const;
		bool isGL_EXT_texture_edge_clamp() const;
		bool isGL_EXT_texture_array() const;
		bool isGL_EXT_texture_rectangle() const;
		bool isGL_EXT_texture3D() const;
		bool isGL_EXT_texture_cube_map() const;
		bool isGL_EXT_stencil_wrap() const;
		bool isGL_EXT_stencil_two_side() const;
		bool isGL_EXT_packed_depth_stencil() const;
		bool isGL_EXT_depth_bounds_test() const;
		bool isGL_EXT_framebuffer_multisample() const;
		bool isGL_EXT_framebuffer_blit() const;
		bool isGL_EXT_transform_feedback() const;
		bool isGL_EXT_direct_state_access() const;
		// ARB
		bool isGL_ARB_framebuffer_object() const;
		bool isGL_ARB_texture_float() const;
		bool isGL_ARB_color_buffer_float() const;
		bool isGL_ARB_multitexture() const;
		bool isGL_ARB_vertex_buffer_object() const;
		bool isGL_ARB_texture_border_clamp() const;
		bool isGL_ARB_texture_mirrored_repeat() const;
		bool isGL_ARB_texture_cube_map() const;
		bool isGL_ARB_texture_env_combine() const;
		bool isGL_ARB_texture_env_dot3() const;
		bool isGL_ARB_occlusion_query() const;
		bool isGL_ARB_texture_compression() const;
		bool isGL_ARB_depth_texture() const;
		bool isGL_ARB_point_sprite() const;
		bool isGL_ARB_point_parameters() const;
		bool isGL_ARB_shading_language_100() const;
		bool isGL_ARB_vertex_program() const;
		bool isGL_ARB_fragment_program() const;
		bool isGL_ARB_draw_buffers() const;
		bool isGL_ARB_shader_objects() const;
		bool isGL_ARB_separate_shader_objects() const;
		bool isGL_ARB_get_program_binary() const;
		bool isGL_ARB_texture_non_power_of_two() const;
		bool isGL_ARB_texture_rectangle() const;
		bool isGL_ARB_multisample() const;
		bool isGL_ARB_uniform_buffer_object() const;
		bool isGL_ARB_texture_buffer_object() const;
		bool isGL_ARB_draw_indirect() const;
		bool isGL_ARB_multi_draw_indirect() const;
		bool isGL_ARB_half_float_vertex() const;
		bool isGL_ARB_vertex_shader() const;
		bool isGL_ARB_tessellation_shader() const;
		bool isGL_ARB_geometry_shader4() const;
		bool isGL_ARB_fragment_shader() const;
		bool isGL_ARB_depth_clamp() const;
		bool isGL_ARB_draw_instanced() const;
		bool isGL_ARB_instanced_arrays() const;
		bool isGL_ARB_vertex_array_object() const;
		bool isGL_ARB_sampler_objects() const;
		bool isGL_ARB_draw_elements_base_vertex() const;
		bool isGL_ARB_debug_output() const;
		// ATI
		bool isGL_ATI_meminfo() const;
		bool isGL_ATI_separate_stencil() const;
		bool isGL_ATI_draw_buffers() const;
		bool isGL_ATI_texture_compression_3dc() const;
		// AMD
		bool isGL_AMD_vertex_shader_tessellator() const;
		// NV
		bool isGL_NV_texture_rectangle() const;
		bool isGL_NV_occlusion_query() const;
		bool isGL_NVX_gpu_memory_info() const;
		bool isGL_NV_half_float() const;
		bool isGL_NV_texture_compression_vtc() const;
		// SGIS
		bool isGL_SGIS_generate_mipmap() const;
		// HP
		bool isGL_HP_occlusion_test() const;

		///////////////////////////////////////////////////////////
		// Misc
		///////////////////////////////////////////////////////////
		GLint getGL_MAX_ELEMENTS_VERTICES_EXT() const;
		GLint getGL_MAX_ELEMENTS_INDICES_EXT() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Checks whether an extension is supported by the given hardware or not
		*
		*  @param[in] extension
		*    ASCII name of the extension, if a null pointer, nothing happens
		*
		*  @return
		*    "true" if the extensions is supported, else "false"
		*/
		bool isSupported(const char *extension) const;

		/**
		*  @brief
		*    Checks whether an extension is supported by the given hardware or not
		*
		*  @param[in] extension
		*    ASCII name of the extension, if a null pointer, nothing happens
		*
		*  @return
		*    "true" if the extensions is supported, else "false"
		*/
		bool checkExtension(const char *extension) const;

		/**
		*  @brief
		*    Resets the extensions
		*/
		void resetExtensions();

		/**
		*  @brief
		*    Initialize the supported extensions
		*
		*  @param[in] useExtensions
		*    Use extensions?
		*
		*  @return
		*    "true" if all went fine, else "false"
		*
		*  @note
		*    - Platform dependent implementation
		*/
		bool initialize(bool useExtensions = true);

		/**
		*  @brief
		*    Initialize the supported universal extensions
		*
		*  @return
		*    "true" if all went fine, else "false"
		*
		*  @note
		*    - Platform independent implementation
		*/
		bool initializeUniversal();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IContext *mContext;		///< Owner context, always valid!
		bool	  mInitialized;	///< Are the extensions initialized?

		// Supported extensions
		// WGL (Windows only)
		bool mWGL_ARB_extensions_string;
		bool mWGL_EXT_swap_control;
		bool mWGL_ARB_pixel_format;
		bool mWGL_ARB_render_texture;
		bool mWGL_ARB_make_current_read;
		bool mWGL_ARB_multisample;
		bool mWGL_ATI_pixel_format_float;
		bool mWGL_NV_float_buffer;
		// GLX (Linux only)
		bool mGLX_SGI_swap_control;
		// EXT
		bool mGL_EXT_compiled_vertex_array;
		bool mGL_EXT_draw_range_elements;
		bool mGL_EXT_fog_coord;
		bool mGL_EXT_secondary_color;
		bool mGL_EXT_texture_compression_s3tc;
		bool mGL_EXT_texture_compression_latc;
		bool mGL_EXT_texture_lod_bias;
		bool mGL_EXT_texture_filter_anisotropic;
		bool mGL_EXT_separate_specular_color;
		bool mGL_EXT_texture_edge_clamp;
		bool mGL_EXT_texture_array;
		bool mGL_EXT_texture_rectangle;
		bool mGL_EXT_texture3D;
		bool mGL_EXT_texture_cube_map;
		bool mGL_EXT_stencil_wrap;
		bool mGL_EXT_stencil_two_side;
		bool mGL_EXT_packed_depth_stencil;
		bool mGL_EXT_depth_bounds_test;
		bool mGL_EXT_framebuffer_multisample;
		bool mGL_EXT_framebuffer_blit;
		bool mGL_EXT_transform_feedback;
		bool mGL_EXT_direct_state_access;
		// ARB
		bool mGL_ARB_framebuffer_object;
		bool mGL_ARB_texture_float;
		bool mGL_ARB_color_buffer_float;
		bool mGL_ARB_multitexture;
		bool mGL_ARB_vertex_buffer_object;
		bool mGL_ARB_texture_border_clamp;
		bool mGL_ARB_texture_mirrored_repeat;
		bool mGL_ARB_texture_cube_map;
		bool mGL_ARB_texture_env_combine;
		bool mGL_ARB_texture_env_dot3;
		bool mGL_ARB_occlusion_query;
		bool mGL_ARB_texture_compression;
		bool mGL_ARB_depth_texture;
		bool mGL_ARB_point_sprite;
		bool mGL_ARB_point_parameters;
		bool mGL_ARB_shading_language_100;
		bool mGL_ARB_vertex_program;
		bool mGL_ARB_fragment_program;
		bool mGL_ARB_draw_buffers;
		bool mGL_ARB_shader_objects;
		bool mGL_ARB_separate_shader_objects;
		bool mGL_ARB_get_program_binary;
		bool mGL_ARB_texture_non_power_of_two;
		bool mGL_ARB_texture_rectangle;
		bool mGL_ARB_multisample;
		bool mGL_ARB_uniform_buffer_object;
		bool mGL_ARB_texture_buffer_object;
		bool mGL_ARB_draw_indirect;
		bool mGL_ARB_multi_draw_indirect;
		bool mGL_ARB_half_float_vertex;
		bool mGL_ARB_vertex_shader;
		bool mGL_ARB_tessellation_shader;
		bool mGL_ARB_geometry_shader4;
		bool mGL_ARB_fragment_shader;
		bool mGL_ARB_depth_clamp;
		bool mGL_ARB_draw_instanced;
		bool mGL_ARB_instanced_arrays;
		bool mGL_ARB_vertex_array_object;
		bool mGL_ARB_sampler_objects;
		bool mGL_ARB_draw_elements_base_vertex;
		bool mGL_ARB_debug_output;
		// ATI
		bool mGL_ATI_meminfo;
		bool mGL_ATI_separate_stencil;
		bool mGL_ATI_draw_buffers;
		bool mGL_ATI_texture_compression_3dc;
		// AMD
		bool mGL_AMD_vertex_shader_tessellator;
		// NV
		bool mGL_NV_texture_rectangle;
		bool mGL_NV_occlusion_query;
		bool mGL_NVX_gpu_memory_info;
		bool mGL_NV_half_float;
		bool mGL_NV_texture_compression_vtc;
		// SGIS
		bool mGL_SGIS_generate_mipmap;
		// HP
		bool mGL_HP_occlusion_test;

		// Misc
		GLint mGL_MAX_ELEMENTS_VERTICES_EXT;
		GLint mGL_MAX_ELEMENTS_INDICES_EXT;


	};


	//[-------------------------------------------------------]
	//[ Define helper macro                                   ]
	//[-------------------------------------------------------]
	#ifdef EXTENSIONS_DEFINE
		#define FNDEF_EX(funcName, funcTypedef) funcTypedef funcName = nullptr
	#else
		#define FNDEF_EX(funcName, funcTypedef) extern funcTypedef funcName
	#endif


	//[-------------------------------------------------------]
	//[ WGL (Windows only) definitions                        ]
	//[-------------------------------------------------------]
	#ifdef WIN32
		// WGL_ARB_extensions_string
		FNDEF_EX(wglGetExtensionsStringARB,	PFNWGLGETEXTENSIONSSTRINGARBPROC);

		// WGL_EXT_swap_control
		FNDEF_EX(wglSwapIntervalEXT,	PFNWGLSWAPINTERVALEXTPROC);
		FNDEF_EX(wglGetSwapIntervalEXT,	PFNWGLGETSWAPINTERVALEXTPROC);

		// WGL_ARB_pixel_format
		FNDEF_EX(wglGetPixelFormatAttribivARB,	PFNWGLGETPIXELFORMATATTRIBIVARBPROC);
		FNDEF_EX(wglGetPixelFormatAttribfvARB,	PFNWGLGETPIXELFORMATATTRIBFVARBPROC);
		FNDEF_EX(wglChoosePixelFormatARB,		PFNWGLCHOOSEPIXELFORMATARBPROC);

		// WGL_ARB_render_texture
		FNDEF_EX(wglBindTexImageARB,	PFNWGLBINDTEXIMAGEARBPROC);
		FNDEF_EX(wglReleaseTexImageARB,	PFNWGLRELEASETEXIMAGEARBPROC);

		// WGL_ARB_make_current_read
		FNDEF_EX(wglMakeContextCurrentARB,	PFNWGLMAKECONTEXTCURRENTARBPROC);
		FNDEF_EX(wglGetCurrentReadDCARB,	PFNWGLGETCURRENTREADDCARBPROC);
	#endif


	//[-------------------------------------------------------]
	//[ GLX (Linux only)                                      ]
	//[-------------------------------------------------------]
	#if defined(LINUX) && !defined(APPLE)
		// GLX_SGI_swap_control
		FNDEF_EX(glXSwapIntervalSGI,	PFNGLXSWAPINTERVALSGIPROC);
	#endif


	//[-------------------------------------------------------]
	//[ EXT                                                   ]
	//[-------------------------------------------------------]
	// GL_EXT_compiled_vertex_array
	FNDEF_EX(glLockArraysEXT,	PFNGLLOCKARRAYSEXTPROC);
	FNDEF_EX(glUnlockArraysEXT,	PFNGLUNLOCKARRAYSEXTPROC);

	// GL_EXT_draw_range_elements
	FNDEF_EX(glDrawRangeElementsEXT,	PFNGLDRAWRANGEELEMENTSEXTPROC);

	// GL_EXT_fog_coord
	FNDEF_EX(glFogCoordfEXT,		PFNGLFOGCOORDFEXTPROC);
	FNDEF_EX(glFogCoordfvEXT,		PFNGLFOGCOORDFVEXTPROC);
	FNDEF_EX(glFogCoorddEXT,		PFNGLFOGCOORDDEXTPROC);
	FNDEF_EX(glFogCoorddvEXT,		PFNGLFOGCOORDDVEXTPROC);
	FNDEF_EX(glFogCoordPointerEXT,	PFNGLFOGCOORDPOINTEREXTPROC);

	// GL_EXT_secondary_color
	FNDEF_EX(glSecondaryColor3bEXT,			PFNGLSECONDARYCOLOR3BEXTPROC);
	FNDEF_EX(glSecondaryColor3bvEXT,		PFNGLSECONDARYCOLOR3BVEXTPROC);
	FNDEF_EX(glSecondaryColor3dEXT,			PFNGLSECONDARYCOLOR3DEXTPROC);
	FNDEF_EX(glSecondaryColor3dvEXT,		PFNGLSECONDARYCOLOR3DVEXTPROC);
	FNDEF_EX(glSecondaryColor3fEXT,			PFNGLSECONDARYCOLOR3FEXTPROC);
	FNDEF_EX(glSecondaryColor3fvEXT,		PFNGLSECONDARYCOLOR3FVEXTPROC);
	FNDEF_EX(glSecondaryColor3iEXT,			PFNGLSECONDARYCOLOR3IEXTPROC);
	FNDEF_EX(glSecondaryColor3ivEXT,		PFNGLSECONDARYCOLOR3IVEXTPROC);
	FNDEF_EX(glSecondaryColor3sEXT,			PFNGLSECONDARYCOLOR3SEXTPROC);
	FNDEF_EX(glSecondaryColor3svEXT,		PFNGLSECONDARYCOLOR3SVEXTPROC);
	FNDEF_EX(glSecondaryColor3ubEXT,		PFNGLSECONDARYCOLOR3UBEXTPROC);
	FNDEF_EX(glSecondaryColor3ubvEXT,		PFNGLSECONDARYCOLOR3UBVEXTPROC);
	FNDEF_EX(glSecondaryColor3uiEXT,		PFNGLSECONDARYCOLOR3UIEXTPROC);
	FNDEF_EX(glSecondaryColor3uivEXT,		PFNGLSECONDARYCOLOR3UIVEXTPROC);
	FNDEF_EX(glSecondaryColor3usEXT,		PFNGLSECONDARYCOLOR3USEXTPROC);
	FNDEF_EX(glSecondaryColor3usvEXT,		PFNGLSECONDARYCOLOR3USVEXTPROC);
	FNDEF_EX(glSecondaryColorPointerEXT,	PFNGLSECONDARYCOLORPOINTEREXTPROC);

	// GL_EXT_texture3D
	FNDEF_EX(glTexImage3DEXT,		PFNGLTEXIMAGE3DEXTPROC);
	FNDEF_EX(glTexSubImage3DEXT,	PFNGLTEXSUBIMAGE3DEXTPROC);

	// GL_EXT_stencil_two_side
	FNDEF_EX(glActiveStencilFaceEXT,	PFNGLACTIVESTENCILFACEEXTPROC);

	// GL_EXT_depth_bounds_test
	FNDEF_EX(glDepthBoundsEXT,	PFNGLDEPTHBOUNDSEXTPROC);

	// GL_EXT_framebuffer_multisample
	FNDEF_EX(glRenderbufferStorageMultisampleEXT,	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC);

	// GL_EXT_framebuffer_blit
	FNDEF_EX(glBlitFramebufferEXT,	PFNGLBLITFRAMEBUFFEREXTPROC);

	// GL_EXT_transform_feedback
	FNDEF_EX(glBeginTransformFeedbackEXT,		PFNGLBEGINTRANSFORMFEEDBACKEXTPROC);
	FNDEF_EX(glEndTransformFeedbackEXT,			PFNGLENDTRANSFORMFEEDBACKEXTPROC);
	FNDEF_EX(glBindBufferRangeEXT,				PFNGLBINDBUFFERRANGEEXTPROC);
	FNDEF_EX(glBindBufferOffsetEXT,				PFNGLBINDBUFFEROFFSETEXTPROC);
	FNDEF_EX(glBindBufferBaseEXT,				PFNGLBINDBUFFERBASEEXTPROC);
	FNDEF_EX(glTransformFeedbackVaryingsEXT,	PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC);
	FNDEF_EX(glGetTransformFeedbackVaryingEXT,	PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC);

	// GL_EXT_direct_state_access
	FNDEF_EX(glNamedBufferDataEXT,					PFNGLNAMEDBUFFERDATAEXTPROC);
	FNDEF_EX(glNamedBufferSubDataEXT,				PFNGLNAMEDBUFFERSUBDATAEXTPROC);
	FNDEF_EX(glMapNamedBufferEXT,					PFNGLMAPNAMEDBUFFEREXTPROC);
	FNDEF_EX(glUnmapNamedBufferEXT,					PFNGLUNMAPNAMEDBUFFEREXTPROC);
	FNDEF_EX(glProgramUniform1fEXT,					PFNGLPROGRAMUNIFORM1FEXTPROC);
	FNDEF_EX(glProgramUniform2fvEXT,				PFNGLPROGRAMUNIFORM2FVEXTPROC);
	FNDEF_EX(glProgramUniform3fvEXT,				PFNGLPROGRAMUNIFORM3FVEXTPROC);
	FNDEF_EX(glProgramUniform4fvEXT,				PFNGLPROGRAMUNIFORM4FVEXTPROC);
	FNDEF_EX(glProgramUniformMatrix3fvEXT,			PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC);
	FNDEF_EX(glProgramUniformMatrix4fvEXT,			PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC);
	FNDEF_EX(glProgramUniform1iEXT,					PFNGLPROGRAMUNIFORM1IEXTPROC);
	FNDEF_EX(glTextureImage2DEXT,					PFNGLTEXTUREIMAGE2DEXTPROC);
	FNDEF_EX(glTextureImage3DEXT,					PFNGLTEXTUREIMAGE3DEXTPROC);
	FNDEF_EX(glTextureParameteriEXT,				PFNGLTEXTUREPARAMETERIEXTPROC);
	FNDEF_EX(glGenerateTextureMipmapEXT,			PFNGLGENERATETEXTUREMIPMAPEXTPROC);
	FNDEF_EX(glCompressedTextureImage2DEXT,			PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC);
	FNDEF_EX(glVertexArrayVertexAttribOffsetEXT,	PFNGLVERTEXARRAYVERTEXATTRIBOFFSETEXTPROC);
	FNDEF_EX(glEnableVertexArrayAttribEXT,			PFNGLENABLEVERTEXARRAYATTRIBEXTPROC);
	FNDEF_EX(glBindMultiTextureEXT,					PFNGLBINDMULTITEXTUREEXTPROC);
	FNDEF_EX(glNamedFramebufferTexture2DEXT,		PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC);
	FNDEF_EX(glCheckNamedFramebufferStatusEXT,		PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC);
	FNDEF_EX(glNamedRenderbufferStorageEXT,			PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC);	
	FNDEF_EX(glNamedFramebufferRenderbufferEXT,		PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC);


	//[-------------------------------------------------------]
	//[ ARB                                                   ]
	//[-------------------------------------------------------]
	// GL_ARB_framebuffer_object
	FNDEF_EX(glIsRenderbuffer,						PFNGLISRENDERBUFFERPROC);
	FNDEF_EX(glBindRenderbuffer,					PFNGLBINDRENDERBUFFERPROC);
	FNDEF_EX(glDeleteRenderbuffers,					PFNGLDELETERENDERBUFFERSPROC);
	FNDEF_EX(glGenRenderbuffers,					PFNGLGENRENDERBUFFERSPROC);
	FNDEF_EX(glRenderbufferStorage,					PFNGLRENDERBUFFERSTORAGEPROC);
	FNDEF_EX(glGetRenderbufferParameteriv,			PFNGLGETRENDERBUFFERPARAMETERIVPROC);
	FNDEF_EX(glIsFramebuffer,						PFNGLISFRAMEBUFFERPROC);
	FNDEF_EX(glBindFramebuffer,						PFNGLBINDFRAMEBUFFERPROC);
	FNDEF_EX(glDeleteFramebuffers,					PFNGLDELETEFRAMEBUFFERSPROC);
	FNDEF_EX(glGenFramebuffers,						PFNGLGENFRAMEBUFFERSPROC);
	FNDEF_EX(glCheckFramebufferStatus,				PFNGLCHECKFRAMEBUFFERSTATUSPROC);
	FNDEF_EX(glFramebufferTexture1D,				PFNGLFRAMEBUFFERTEXTURE1DPROC);
	FNDEF_EX(glFramebufferTexture2D,				PFNGLFRAMEBUFFERTEXTURE2DPROC);
	FNDEF_EX(glFramebufferTexture3D,				PFNGLFRAMEBUFFERTEXTURE3DPROC);
	FNDEF_EX(glFramebufferRenderbuffer,				PFNGLFRAMEBUFFERRENDERBUFFERPROC);
	FNDEF_EX(glGetFramebufferAttachmentParameteriv,	PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC);
	FNDEF_EX(glGenerateMipmap,						PFNGLGENERATEMIPMAPPROC);

	// GL_ARB_color_buffer_float
	FNDEF_EX(glClampColorARB,	PFNGLCLAMPCOLORARBPROC);

	// GL_ARB_multitexture
	FNDEF_EX(glActiveTextureARB,			PFNGLACTIVETEXTUREARBPROC);
	FNDEF_EX(glClientActiveTextureARB,		PFNGLCLIENTACTIVETEXTUREARBPROC);
	FNDEF_EX(glMultiTexCoord1dARB,			PFNGLMULTITEXCOORD1DARBPROC);
	FNDEF_EX(glMultiTexCoord1dvARB,			PFNGLMULTITEXCOORD1DVARBPROC);
	FNDEF_EX(glMultiTexCoord1fARB,			PFNGLMULTITEXCOORD1FARBPROC);
	FNDEF_EX(glMultiTexCoord1fvARB,			PFNGLMULTITEXCOORD1FVARBPROC);
	FNDEF_EX(glMultiTexCoord1iARB,			PFNGLMULTITEXCOORD1IARBPROC);
	FNDEF_EX(glMultiTexCoord1ivARB,			PFNGLMULTITEXCOORD1IVARBPROC);
	FNDEF_EX(glMultiTexCoord1sARB,			PFNGLMULTITEXCOORD1SARBPROC);
	FNDEF_EX(glMultiTexCoord1svARB,			PFNGLMULTITEXCOORD1SVARBPROC);
	FNDEF_EX(glMultiTexCoord2dARB,			PFNGLMULTITEXCOORD2DARBPROC);
	FNDEF_EX(glMultiTexCoord2dvARB,			PFNGLMULTITEXCOORD2DVARBPROC);
	FNDEF_EX(glMultiTexCoord2fARB,			PFNGLMULTITEXCOORD2FARBPROC);
	FNDEF_EX(glMultiTexCoord2fvARB,			PFNGLMULTITEXCOORD2FVARBPROC);
	FNDEF_EX(glMultiTexCoord2iARB,			PFNGLMULTITEXCOORD2IARBPROC);
	FNDEF_EX(glMultiTexCoord2ivARB,			PFNGLMULTITEXCOORD2IVARBPROC);
	FNDEF_EX(glMultiTexCoord2sARB,			PFNGLMULTITEXCOORD2SARBPROC);
	FNDEF_EX(glMultiTexCoord2svARB,			PFNGLMULTITEXCOORD2SVARBPROC);
	FNDEF_EX(glMultiTexCoord3dARB,			PFNGLMULTITEXCOORD3DARBPROC);
	FNDEF_EX(glMultiTexCoord3dvARB,			PFNGLMULTITEXCOORD3DVARBPROC);
	FNDEF_EX(glMultiTexCoord3fARB,			PFNGLMULTITEXCOORD3FARBPROC);
	FNDEF_EX(glMultiTexCoord3fvARB,			PFNGLMULTITEXCOORD3FVARBPROC);
	FNDEF_EX(glMultiTexCoord3iARB,			PFNGLMULTITEXCOORD3IARBPROC);
	FNDEF_EX(glMultiTexCoord3ivARB,			PFNGLMULTITEXCOORD3IVARBPROC);
	FNDEF_EX(glMultiTexCoord3sARB,			PFNGLMULTITEXCOORD3SARBPROC);
	FNDEF_EX(glMultiTexCoord3svARB,			PFNGLMULTITEXCOORD3SVARBPROC);
	FNDEF_EX(glMultiTexCoord4dARB,			PFNGLMULTITEXCOORD4DARBPROC);
	FNDEF_EX(glMultiTexCoord4dvARB,			PFNGLMULTITEXCOORD4DVARBPROC);
	FNDEF_EX(glMultiTexCoord4fARB,			PFNGLMULTITEXCOORD4FARBPROC);
	FNDEF_EX(glMultiTexCoord4fvARB,			PFNGLMULTITEXCOORD4FVARBPROC);
	FNDEF_EX(glMultiTexCoord4iARB,			PFNGLMULTITEXCOORD4IARBPROC);
	FNDEF_EX(glMultiTexCoord4ivARB,			PFNGLMULTITEXCOORD4IVARBPROC);
	FNDEF_EX(glMultiTexCoord4sARB,			PFNGLMULTITEXCOORD4SARBPROC);
	FNDEF_EX(glMultiTexCoord4svARB,			PFNGLMULTITEXCOORD4SVARBPROC);

	// GL_ARB_vertex_buffer_object
	FNDEF_EX(glBindBufferARB,			PFNGLBINDBUFFERARBPROC);
	FNDEF_EX(glDeleteBuffersARB,		PFNGLDELETEBUFFERSARBPROC);
	FNDEF_EX(glGenBuffersARB,			PFNGLGENBUFFERSARBPROC);
	FNDEF_EX(glBufferDataARB,			PFNGLBUFFERDATAARBPROC);
	FNDEF_EX(glBufferSubDataARB,		PFNGLBUFFERSUBDATAARBPROC);
	FNDEF_EX(glGetBufferSubDataARB,		PFNGLGETBUFFERSUBDATAARBPROC);
	FNDEF_EX(glMapBufferARB,			PFNGLMAPBUFFERARBPROC);
	FNDEF_EX(glUnmapBufferARB,			PFNGLUNMAPBUFFERARBPROC);
	FNDEF_EX(glGetBufferParameterivARB,	PFNGLGETBUFFERPARAMETERIVARBPROC);
	FNDEF_EX(glGetBufferPointervARB,	PFNGLGETBUFFERPOINTERVARBPROC);

	// GL_ARB_occlusion_query
	FNDEF_EX(glGenQueriesARB,			PFNGLGENQUERIESARBPROC);
	FNDEF_EX(glDeleteQueriesARB,		PFNGLDELETEQUERIESARBPROC);
	FNDEF_EX(glIsQueryARB,				PFNGLISQUERYARBPROC);
	FNDEF_EX(glBeginQueryARB,			PFNGLBEGINQUERYARBPROC);
	FNDEF_EX(glEndQueryARB,				PFNGLENDQUERYARBPROC);
	FNDEF_EX(glGetQueryivARB,			PFNGLGETQUERYIVARBPROC);
	FNDEF_EX(glGetQueryObjectivARB,		PFNGLGETQUERYOBJECTIVARBPROC);
	FNDEF_EX(glGetQueryObjectuivARB,	PFNGLGETQUERYOBJECTUIVARBPROC);

	// GL_ARB_texture_compression
	FNDEF_EX(glCompressedTexImage3DARB,		PFNGLCOMPRESSEDTEXIMAGE3DARBPROC);
	FNDEF_EX(glCompressedTexImage2DARB,		PFNGLCOMPRESSEDTEXIMAGE2DARBPROC);
	FNDEF_EX(glCompressedTexImage1DARB,		PFNGLCOMPRESSEDTEXIMAGE1DARBPROC);
	FNDEF_EX(glCompressedTexSubImage3DARB,	PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC);
	FNDEF_EX(glCompressedTexSubImage2DARB,	PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC);
	FNDEF_EX(glCompressedTexSubImage1DARB,	PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC);
	FNDEF_EX(glGetCompressedTexImageARB,	PFNGLGETCOMPRESSEDTEXIMAGEARBPROC);

	// GL_ARB_point_parameters
	FNDEF_EX(glPointParameterfARB,	PFNGLPOINTPARAMETERFARBPROC);
	FNDEF_EX(glPointParameterfvARB,	PFNGLPOINTPARAMETERFVARBPROC);

	// GL_ARB_vertex_program
	FNDEF_EX(glVertexAttribPointerARB,		PFNGLVERTEXATTRIBPOINTERARBPROC);
	FNDEF_EX(glEnableVertexAttribArrayARB,	PFNGLENABLEVERTEXATTRIBARRAYARBPROC);
	FNDEF_EX(glDisableVertexAttribArrayARB,	PFNGLDISABLEVERTEXATTRIBARRAYARBPROC);
	FNDEF_EX(glGetProgramivARB,				PFNGLGETPROGRAMIVARBPROC);

	// GL_ARB_draw_buffers
	FNDEF_EX(glDrawBuffersARB,	PFNGLDRAWBUFFERSARBPROC);

	// GL_ARB_shader_objects
	FNDEF_EX(glDeleteObjectARB,			PFNGLDELETEOBJECTARBPROC);
	FNDEF_EX(glGetHandleARB,			PFNGLGETHANDLEARBPROC);
	FNDEF_EX(glDetachObjectARB,			PFNGLDETACHOBJECTARBPROC);
	FNDEF_EX(glCreateShaderObjectARB,	PFNGLCREATESHADEROBJECTARBPROC);
	FNDEF_EX(glShaderSourceARB,			PFNGLSHADERSOURCEARBPROC);
	FNDEF_EX(glCompileShaderARB,		PFNGLCOMPILESHADERARBPROC);
	FNDEF_EX(glCreateProgramObjectARB,	PFNGLCREATEPROGRAMOBJECTARBPROC);
	FNDEF_EX(glAttachObjectARB,			PFNGLATTACHOBJECTARBPROC);
	FNDEF_EX(glLinkProgramARB,			PFNGLLINKPROGRAMARBPROC);
	FNDEF_EX(glUseProgramObjectARB,		PFNGLUSEPROGRAMOBJECTARBPROC);
	FNDEF_EX(glValidateProgramARB,		PFNGLVALIDATEPROGRAMARBPROC);
	FNDEF_EX(glUniform1fARB,			PFNGLUNIFORM1FARBPROC);
	FNDEF_EX(glUniform2fARB,			PFNGLUNIFORM2FARBPROC);
	FNDEF_EX(glUniform3fARB,			PFNGLUNIFORM3FARBPROC);
	FNDEF_EX(glUniform4fARB,			PFNGLUNIFORM4FARBPROC);
	FNDEF_EX(glUniform1iARB,			PFNGLUNIFORM1IARBPROC);
	FNDEF_EX(glUniform2iARB,			PFNGLUNIFORM2IARBPROC);
	FNDEF_EX(glUniform3iARB,			PFNGLUNIFORM3IARBPROC);
	FNDEF_EX(glUniform4iARB,			PFNGLUNIFORM4IARBPROC);
	FNDEF_EX(glUniform1fvARB,			PFNGLUNIFORM1FVARBPROC);
	FNDEF_EX(glUniform2fvARB,			PFNGLUNIFORM2FVARBPROC);
	FNDEF_EX(glUniform3fvARB,			PFNGLUNIFORM3FVARBPROC);
	FNDEF_EX(glUniform4fvARB,			PFNGLUNIFORM4FVARBPROC);
	FNDEF_EX(glUniform1ivARB,			PFNGLUNIFORM1IVARBPROC);
	FNDEF_EX(glUniform2ivARB,			PFNGLUNIFORM2IVARBPROC);
	FNDEF_EX(glUniform3ivARB,			PFNGLUNIFORM3IVARBPROC);
	FNDEF_EX(glUniform4ivARB,			PFNGLUNIFORM4IVARBPROC);
	FNDEF_EX(glUniformMatrix2fvARB,		PFNGLUNIFORMMATRIX2FVARBPROC);
	FNDEF_EX(glUniformMatrix3fvARB,		PFNGLUNIFORMMATRIX3FVARBPROC);
	FNDEF_EX(glUniformMatrix4fvARB,		PFNGLUNIFORMMATRIX4FVARBPROC);
	FNDEF_EX(glGetObjectParameterfvARB,	PFNGLGETOBJECTPARAMETERFVARBPROC);
	FNDEF_EX(glGetObjectParameterivARB,	PFNGLGETOBJECTPARAMETERIVARBPROC);
	FNDEF_EX(glGetInfoLogARB,			PFNGLGETINFOLOGARBPROC);
	FNDEF_EX(glGetAttachedObjectsARB,	PFNGLGETATTACHEDOBJECTSARBPROC);
	FNDEF_EX(glGetUniformLocationARB,	PFNGLGETUNIFORMLOCATIONARBPROC);
	FNDEF_EX(glGetActiveUniformARB,		PFNGLGETACTIVEUNIFORMARBPROC);
	FNDEF_EX(glGetUniformfvARB,			PFNGLGETUNIFORMFVARBPROC);
	FNDEF_EX(glGetUniformivARB,			PFNGLGETUNIFORMIVARBPROC);
	FNDEF_EX(glGetShaderSourceARB,		PFNGLGETSHADERSOURCEARBPROC);

	// GL_ARB_separate_shader_objects
	FNDEF_EX(glCreateShaderProgramv,		PFNGLCREATESHADERPROGRAMVPROC);
	FNDEF_EX(glDeleteProgram,				PFNGLDELETEPROGRAMPROC);
	FNDEF_EX(glGenProgramPipelines,			PFNGLGENPROGRAMPIPELINESPROC);
	FNDEF_EX(glDeleteProgramPipelines,		PFNGLDELETEPROGRAMPIPELINESPROC);
	FNDEF_EX(glBindProgramPipeline,			PFNGLBINDPROGRAMPIPELINEPROC);
	FNDEF_EX(glUseProgramStages,			PFNGLUSEPROGRAMSTAGESPROC);
	FNDEF_EX(glValidateProgramPipeline,		PFNGLVALIDATEPROGRAMPIPELINEPROC);
	FNDEF_EX(glGetProgramPipelineiv,		PFNGLGETPROGRAMPIPELINEIVPROC);
	FNDEF_EX(glGetProgramPipelineInfoLog,	PFNGLGETPROGRAMPIPELINEINFOLOGPROC);
	FNDEF_EX(glActiveShaderProgram,			PFNGLACTIVESHADERPROGRAMPROC);

	// GL_ARB_get_program_binary
	FNDEF_EX(glGetProgramBinary,	PFNGLGETPROGRAMBINARYPROC);
	FNDEF_EX(glProgramBinary,		PFNGLPROGRAMBINARYPROC);
	FNDEF_EX(glProgramParameteri,	PFNGLPROGRAMPARAMETERIPROC);

	// GL_ARB_uniform_buffer_object
	FNDEF_EX(glGetUniformIndices,			PFNGLGETUNIFORMINDICESPROC);
	FNDEF_EX(glGetActiveUniformsiv,			PFNGLGETACTIVEUNIFORMSIVPROC);
	FNDEF_EX(glGetActiveUniformName,		PFNGLGETACTIVEUNIFORMNAMEPROC);
	FNDEF_EX(glGetUniformBlockIndex,		PFNGLGETUNIFORMBLOCKINDEXPROC);
	FNDEF_EX(glGetActiveUniformBlockiv,		PFNGLGETACTIVEUNIFORMBLOCKIVPROC);
	FNDEF_EX(glGetActiveUniformBlockName,	PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC);
	FNDEF_EX(glUniformBlockBinding,			PFNGLUNIFORMBLOCKBINDINGPROC);
	FNDEF_EX(glBindBufferBase,				PFNGLBINDBUFFERBASEPROC);

	// GL_ARB_texture_buffer_object
	FNDEF_EX(glTexBufferARB,	PFNGLTEXBUFFERARBPROC);

	// GL_ARB_draw_indirect
	FNDEF_EX(glDrawArraysIndirect,		PFNGLDRAWARRAYSINDIRECTPROC);
	FNDEF_EX(glDrawElementsIndirect,	PFNGLDRAWELEMENTSINDIRECTPROC);

	// GL_ARB_multi_draw_indirect
	FNDEF_EX(glMultiDrawArraysIndirect,		PFNGLMULTIDRAWARRAYSINDIRECTPROC);
	FNDEF_EX(glMultiDrawElementsIndirect,	PFNGLMULTIDRAWELEMENTSINDIRECTPROC);

	// GL_ARB_vertex_shader
	FNDEF_EX(glBindAttribLocationARB,	PFNGLBINDATTRIBLOCATIONARBPROC);
	FNDEF_EX(glGetActiveAttribARB,		PFNGLGETACTIVEATTRIBARBPROC);
	FNDEF_EX(glGetAttribLocationARB,	PFNGLGETATTRIBLOCATIONARBPROC);

	// GL_ARB_tessellation_shader
	FNDEF_EX(glPatchParameteri,		PFNGLPATCHPARAMETERIPROC);
	FNDEF_EX(glPatchParameterfv,	PFNGLPATCHPARAMETERFVPROC);

	// GL_ARB_geometry_shader4
	FNDEF_EX(glProgramParameteriARB,	PFNGLPROGRAMPARAMETERIARBPROC);

	// GL_ARB_draw_instanced
	FNDEF_EX(glDrawArraysInstancedARB,		PFNGLDRAWARRAYSINSTANCEDARBPROC);
	FNDEF_EX(glDrawElementsInstancedARB,	PFNGLDRAWELEMENTSINSTANCEDARBPROC);

	// GL_ARB_instanced_arrays
	FNDEF_EX(glVertexAttribDivisorARB,	PFNGLVERTEXATTRIBDIVISORARBPROC);

	// GL_ARB_vertex_array_object
	FNDEF_EX(glBindVertexArray,		PFNGLBINDVERTEXARRAYPROC);
	FNDEF_EX(glDeleteVertexArrays,	PFNGLDELETEVERTEXARRAYSPROC);
	FNDEF_EX(glGenVertexArrays,		PFNGLGENVERTEXARRAYSPROC);

	// GL_ARB_sampler_objects
	FNDEF_EX(glGenSamplers,			PFNGLGENSAMPLERSPROC);
	FNDEF_EX(glDeleteSamplers,		PFNGLDELETESAMPLERSPROC);
	FNDEF_EX(glBindSampler,			PFNGLBINDSAMPLERPROC);
	FNDEF_EX(glSamplerParameteri,	PFNGLSAMPLERPARAMETERIPROC);
	FNDEF_EX(glSamplerParameterf,	PFNGLSAMPLERPARAMETERFPROC);
	FNDEF_EX(glSamplerParameterfv,	PFNGLSAMPLERPARAMETERFVPROC);

	// GL_ARB_draw_elements_base_vertex
	FNDEF_EX(glDrawElementsBaseVertex,			PFNGLDRAWELEMENTSBASEVERTEXPROC);
	FNDEF_EX(glDrawRangeElementsBaseVertex,		PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC);
	FNDEF_EX(glDrawElementsInstancedBaseVertex,	PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC);

	// GL_ARB_debug_output
	FNDEF_EX(glDebugMessageCallbackARB,	PFNGLDEBUGMESSAGECALLBACKARBPROC);


	//[-------------------------------------------------------]
	//[ ATI                                                   ]
	//[-------------------------------------------------------]
	// GL_ATI_separate_stencil
	FNDEF_EX(glStencilOpSeparateATI,	PFNGLSTENCILOPSEPARATEATIPROC);
	FNDEF_EX(glStencilFuncSeparateATI,	PFNGLSTENCILFUNCSEPARATEATIPROC);

	// GL_ATI_draw_buffers
	FNDEF_EX(glDrawBuffersATI,	PFNGLDRAWBUFFERSATIPROC);


	//[-------------------------------------------------------]
	//[ AMD                                                   ]
	//[-------------------------------------------------------]
	// GL_AMD_vertex_shader_tessellator
	// -> The extension is listed as "GL_AMD_vertex_shader_tessellator" (see http://www.opengl.org/registry/specs/AMD/vertex_shader_tessellator.txt)
	// -> In "http://www.opengl.org/registry/api/glext.h" it's listed as "GL_AMD_vertex_shader_tesselator" (typo?)
	FNDEF_EX(glTessellationFactorAMD,	PFNGLTESSELLATIONFACTORAMDPROC);
	FNDEF_EX(glTessellationModeAMD,		PFNGLTESSELLATIONMODEAMDPROC);


	//[-------------------------------------------------------]
	//[ NV                                                    ]
	//[-------------------------------------------------------]
	// GL_NV_occlusion_query
	FNDEF_EX(glGenOcclusionQueriesNV,		PFNGLGENOCCLUSIONQUERIESNVPROC);
	FNDEF_EX(glDeleteOcclusionQueriesNV,	PFNGLDELETEOCCLUSIONQUERIESNVPROC);
	FNDEF_EX(glIsOcclusionQueryNV,			PFNGLISOCCLUSIONQUERYNVPROC);
	FNDEF_EX(glBeginOcclusionQueryNV,		PFNGLBEGINOCCLUSIONQUERYNVPROC);
	FNDEF_EX(glEndOcclusionQueryNV,			PFNGLENDOCCLUSIONQUERYNVPROC);
	FNDEF_EX(glGetOcclusionQueryivNV,		PFNGLGETOCCLUSIONQUERYIVNVPROC);
	FNDEF_EX(glGetOcclusionQueryuivNV,		PFNGLGETOCCLUSIONQUERYUIVNVPROC);


	//[-------------------------------------------------------]
	//[ Undefine helper macro                                 ]
	//[-------------------------------------------------------]
	#undef FNDEF_EX


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
