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
	#endif
	#include <GL/gl.h>
	#include <GL/glext.h>	// Requires definitions from "gl.h"
#endif
#include "Renderer/PlatformTypes.h"

// Get rid of some nasty OS macros
#undef max
#undef None	// Linux: Undefine "None", this name is used inside enums defined by Unrimp (which gets defined inside Xlib.h pulled in by glx.h)


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
		// EXT
		bool isGL_EXT_texture_lod_bias() const;
		bool isGL_EXT_texture_filter_anisotropic() const;
		bool isGL_EXT_texture_array() const;
		bool isGL_EXT_texture3D() const;
		bool isGL_EXT_direct_state_access() const;
		// ARB
		bool isGL_ARB_framebuffer_object() const;
		bool isGL_ARB_multitexture() const;
		bool isGL_ARB_vertex_buffer_object() const;
		bool isGL_ARB_texture_compression() const;
		bool isGL_ARB_vertex_program() const;
		bool isGL_ARB_fragment_program() const;
		bool isGL_ARB_draw_buffers() const;
		bool isGL_ARB_shader_objects() const;
		bool isGL_ARB_separate_shader_objects() const;
		bool isGL_ARB_get_program_binary() const;
		bool isGL_ARB_uniform_buffer_object() const;
		bool isGL_ARB_texture_buffer_object() const;
		bool isGL_ARB_draw_indirect() const;
		bool isGL_ARB_multi_draw_indirect() const;
		bool isGL_ARB_vertex_shader() const;
		bool isGL_ARB_tessellation_shader() const;
		bool isGL_ARB_geometry_shader4() const;
		bool isGL_ARB_fragment_shader() const;
		bool isGL_ARB_draw_instanced() const;
		bool isGL_ARB_instanced_arrays() const;
		bool isGL_ARB_vertex_array_object() const;
		bool isGL_ARB_sampler_objects() const;
		bool isGL_ARB_draw_elements_base_vertex() const;
		bool isGL_ARB_debug_output() const;
		bool isGL_ARB_direct_state_access() const;


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
		// EXT
		bool mGL_EXT_texture_lod_bias;
		bool mGL_EXT_texture_filter_anisotropic;
		bool mGL_EXT_texture_array;
		bool mGL_EXT_texture3D;
		bool mGL_EXT_direct_state_access;
		// ARB
		bool mGL_ARB_framebuffer_object;
		bool mGL_ARB_multitexture;
		bool mGL_ARB_vertex_buffer_object;
		bool mGL_ARB_texture_compression;
		bool mGL_ARB_vertex_program;
		bool mGL_ARB_fragment_program;
		bool mGL_ARB_draw_buffers;
		bool mGL_ARB_shader_objects;
		bool mGL_ARB_separate_shader_objects;
		bool mGL_ARB_get_program_binary;
		bool mGL_ARB_uniform_buffer_object;
		bool mGL_ARB_texture_buffer_object;
		bool mGL_ARB_draw_indirect;
		bool mGL_ARB_multi_draw_indirect;
		bool mGL_ARB_vertex_shader;
		bool mGL_ARB_tessellation_shader;
		bool mGL_ARB_geometry_shader4;
		bool mGL_ARB_fragment_shader;
		bool mGL_ARB_draw_instanced;
		bool mGL_ARB_instanced_arrays;
		bool mGL_ARB_vertex_array_object;
		bool mGL_ARB_sampler_objects;
		bool mGL_ARB_draw_elements_base_vertex;
		bool mGL_ARB_debug_output;
		bool mGL_ARB_direct_state_access;


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
	#endif


	//[-------------------------------------------------------]
	//[ EXT                                                   ]
	//[-------------------------------------------------------]
	// GL_EXT_texture3D
	FNDEF_EX(glTexImage3DEXT,	PFNGLTEXIMAGE3DEXTPROC);

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
	FNDEF_EX(glBindRenderbuffer,		PFNGLBINDRENDERBUFFERPROC);
	FNDEF_EX(glDeleteRenderbuffers,		PFNGLDELETERENDERBUFFERSPROC);
	FNDEF_EX(glGenRenderbuffers,		PFNGLGENRENDERBUFFERSPROC);
	FNDEF_EX(glRenderbufferStorage,		PFNGLRENDERBUFFERSTORAGEPROC);
	FNDEF_EX(glBindFramebuffer,			PFNGLBINDFRAMEBUFFERPROC);
	FNDEF_EX(glDeleteFramebuffers,		PFNGLDELETEFRAMEBUFFERSPROC);
	FNDEF_EX(glGenFramebuffers,			PFNGLGENFRAMEBUFFERSPROC);
	FNDEF_EX(glCheckFramebufferStatus,	PFNGLCHECKFRAMEBUFFERSTATUSPROC);
	FNDEF_EX(glFramebufferTexture2D,	PFNGLFRAMEBUFFERTEXTURE2DPROC);
	FNDEF_EX(glFramebufferRenderbuffer,	PFNGLFRAMEBUFFERRENDERBUFFERPROC);
	FNDEF_EX(glGenerateMipmap,			PFNGLGENERATEMIPMAPPROC);

	// GL_ARB_multitexture
	FNDEF_EX(glActiveTextureARB,	PFNGLACTIVETEXTUREARBPROC);

	// GL_ARB_vertex_buffer_object
	FNDEF_EX(glBindBufferARB,		PFNGLBINDBUFFERARBPROC);
	FNDEF_EX(glDeleteBuffersARB,	PFNGLDELETEBUFFERSARBPROC);
	FNDEF_EX(glGenBuffersARB,		PFNGLGENBUFFERSARBPROC);
	FNDEF_EX(glBufferDataARB,		PFNGLBUFFERDATAARBPROC);
	FNDEF_EX(glBufferSubDataARB,	PFNGLBUFFERSUBDATAARBPROC);
	FNDEF_EX(glMapBufferARB,		PFNGLMAPBUFFERARBPROC);
	FNDEF_EX(glUnmapBufferARB,		PFNGLUNMAPBUFFERARBPROC);

	// GL_ARB_texture_compression
	FNDEF_EX(glCompressedTexImage2DARB,	PFNGLCOMPRESSEDTEXIMAGE2DARBPROC);

	// GL_ARB_vertex_program
	FNDEF_EX(glVertexAttribPointerARB,		PFNGLVERTEXATTRIBPOINTERARBPROC);
	FNDEF_EX(glEnableVertexAttribArrayARB,	PFNGLENABLEVERTEXATTRIBARRAYARBPROC);
	FNDEF_EX(glDisableVertexAttribArrayARB,	PFNGLDISABLEVERTEXATTRIBARRAYARBPROC);

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
	FNDEF_EX(glUniform1fARB,			PFNGLUNIFORM1FARBPROC);
	FNDEF_EX(glUniform1iARB,			PFNGLUNIFORM1IARBPROC);
	FNDEF_EX(glUniform2iARB,			PFNGLUNIFORM2IARBPROC);
	FNDEF_EX(glUniform3iARB,			PFNGLUNIFORM3IARBPROC);
	FNDEF_EX(glUniform4iARB,			PFNGLUNIFORM4IARBPROC);
	FNDEF_EX(glUniform2fvARB,			PFNGLUNIFORM2FVARBPROC);
	FNDEF_EX(glUniform3fvARB,			PFNGLUNIFORM3FVARBPROC);
	FNDEF_EX(glUniform4fvARB,			PFNGLUNIFORM4FVARBPROC);
	FNDEF_EX(glUniformMatrix3fvARB,		PFNGLUNIFORMMATRIX3FVARBPROC);
	FNDEF_EX(glUniformMatrix4fvARB,		PFNGLUNIFORMMATRIX4FVARBPROC);
	FNDEF_EX(glGetObjectParameterivARB,	PFNGLGETOBJECTPARAMETERIVARBPROC);
	FNDEF_EX(glGetInfoLogARB,			PFNGLGETINFOLOGARBPROC);
	FNDEF_EX(glGetUniformLocationARB,	PFNGLGETUNIFORMLOCATIONARBPROC);

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
	FNDEF_EX(glProgramParameteri,	PFNGLPROGRAMPARAMETERIPROC);

	// GL_ARB_uniform_buffer_object
	FNDEF_EX(glGetUniformBlockIndex,	PFNGLGETUNIFORMBLOCKINDEXPROC);
	FNDEF_EX(glUniformBlockBinding,		PFNGLUNIFORMBLOCKBINDINGPROC);
	FNDEF_EX(glBindBufferBase,			PFNGLBINDBUFFERBASEPROC);

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

	// GL_ARB_tessellation_shader
	FNDEF_EX(glPatchParameteri,	PFNGLPATCHPARAMETERIPROC);

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
	FNDEF_EX(glDrawElementsInstancedBaseVertex,	PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC);

	// GL_ARB_debug_output
	FNDEF_EX(glDebugMessageCallbackARB,	PFNGLDEBUGMESSAGECALLBACKARBPROC);

	// GL_ARB_direct_state_access (opengl 4.5 core extension)
	FNDEF_EX(glNamedBufferData,					PFNGLNAMEDBUFFERDATAPROC);
	FNDEF_EX(glNamedBufferSubData,				PFNGLNAMEDBUFFERSUBDATAPROC);
	FNDEF_EX(glMapNamedBuffer,					PFNGLMAPNAMEDBUFFERPROC);
	FNDEF_EX(glUnmapNamedBuffer,				PFNGLUNMAPNAMEDBUFFERPROC);
	FNDEF_EX(glProgramUniform1f,				PFNGLPROGRAMUNIFORM1FPROC);
	FNDEF_EX(glProgramUniform2fv,				PFNGLPROGRAMUNIFORM2FVPROC);
	FNDEF_EX(glProgramUniform3fv,				PFNGLPROGRAMUNIFORM3FVPROC);
	FNDEF_EX(glProgramUniform4fv,				PFNGLPROGRAMUNIFORM4FVPROC);
	FNDEF_EX(glProgramUniformMatrix3fv,			PFNGLPROGRAMUNIFORMMATRIX3FVPROC);
	FNDEF_EX(glProgramUniformMatrix4fv,			PFNGLPROGRAMUNIFORMMATRIX4FVPROC);
	FNDEF_EX(glTextureParameteri,				PFNGLTEXTUREPARAMETERIPROC);
	FNDEF_EX(glGenerateTextureMipmap,			PFNGLGENERATETEXTUREMIPMAPPROC);
	FNDEF_EX(glEnableVertexArrayAttrib,			PFNGLENABLEVERTEXARRAYATTRIBPROC);
	FNDEF_EX(glCheckNamedFramebufferStatus,		PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC);
	FNDEF_EX(glNamedRenderbufferStorage,		PFNGLNAMEDRENDERBUFFERSTORAGEPROC);
	FNDEF_EX(glNamedFramebufferRenderbuffer,	PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC);
	FNDEF_EX(glNamedFramebufferTexture,			PFNGLNAMEDFRAMEBUFFERTEXTUREPROC);
	FNDEF_EX(glTextureBuffer,					PFNGLTEXTUREBUFFERPROC);
	FNDEF_EX(glBindTextureUnit,					PFNGLBINDTEXTUREUNITPROC);
	FNDEF_EX(glCompressedTextureSubImage2D,		PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC);
	FNDEF_EX(glTextureSubImage2D,				PFNGLTEXTURESUBIMAGE2DPROC);
	FNDEF_EX(glTextureSubImage3D,				PFNGLTEXTURESUBIMAGE3DPROC);
	FNDEF_EX(glVertexArrayAttribFormat,			PFNGLVERTEXARRAYATTRIBFORMATPROC);
	FNDEF_EX(glVertexArrayVertexBuffer,			PFNGLVERTEXARRAYVERTEXBUFFERPROC);
	FNDEF_EX(glVertexArrayBindingDivisor,		PFNGLVERTEXARRAYBINDINGDIVISORPROC);
	FNDEF_EX(glVertexArrayElementBuffer,		PFNGLVERTEXARRAYELEMENTBUFFERPROC);
	// Following methods are not direcly related to this extension but needed for implementation
	FNDEF_EX(glTextureStorage2D,				PFNGLTEXTURESTORAGE2DPROC);
	FNDEF_EX(glTextureStorage3D,				PFNGLTEXTURESTORAGE3DPROC);


	//[-------------------------------------------------------]
	//[ Undefine helper macro                                 ]
	//[-------------------------------------------------------]
	#undef FNDEF_EX


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
