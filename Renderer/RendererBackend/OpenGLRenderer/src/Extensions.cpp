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
//[ Includes                                              ]
//[-------------------------------------------------------]
#define EXTENSIONS_DEFINE

#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_PRINTF()"
#ifdef LINUX
	#include <Renderer/LinuxHeader.h>
	#include "OpenGLRenderer/Linux/ContextLinux.h"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Extensions::Extensions(IContext &context) :
		mContext(&context),
		mInitialized(false)
	{
		// Reset extensions
		resetExtensions();
	}

	Extensions::~Extensions()
	{
		// Nothing here
	}

	bool Extensions::isInitialized() const
	{
		return mInitialized;
	}

	///////////////////////////////////////////////////////////
	// Returns whether an extension is supported or not
	///////////////////////////////////////////////////////////
	// WGL (Windows only)
	bool Extensions::isWGL_ARB_extensions_string()			const { return mWGL_ARB_extensions_string;			}
	// EXT
	bool Extensions::isGL_EXT_texture_lod_bias()			const { return mGL_EXT_texture_lod_bias;			}
	bool Extensions::isGL_EXT_texture_filter_anisotropic()	const { return mGL_EXT_texture_filter_anisotropic;	}
	bool Extensions::isGL_EXT_texture_array()				const { return mGL_EXT_texture_array;				}
	bool Extensions::isGL_EXT_texture3D()					const { return mGL_EXT_texture3D;					}
	bool Extensions::isGL_EXT_direct_state_access()			const { return mGL_EXT_direct_state_access;			}
	// ARB
	bool Extensions::isGL_ARB_framebuffer_object()			const { return mGL_ARB_framebuffer_object;			}
	bool Extensions::isGL_ARB_multitexture()				const { return mGL_ARB_multitexture;				}
	bool Extensions::isGL_ARB_vertex_buffer_object()		const { return mGL_ARB_vertex_buffer_object;		}
	bool Extensions::isGL_ARB_texture_compression()			const { return mGL_ARB_texture_compression;			}
	bool Extensions::isGL_ARB_vertex_program()				const { return mGL_ARB_vertex_program;				}
	bool Extensions::isGL_ARB_fragment_program()			const { return mGL_ARB_fragment_program;			}
	bool Extensions::isGL_ARB_draw_buffers()				const { return mGL_ARB_draw_buffers;				}
	bool Extensions::isGL_ARB_shader_objects()				const { return mGL_ARB_shader_objects;				}
	bool Extensions::isGL_ARB_separate_shader_objects()		const { return mGL_ARB_separate_shader_objects;		}
	bool Extensions::isGL_ARB_get_program_binary()			const { return mGL_ARB_get_program_binary;			}
	bool Extensions::isGL_ARB_uniform_buffer_object()		const { return mGL_ARB_uniform_buffer_object;		}
	bool Extensions::isGL_ARB_texture_buffer_object()		const { return mGL_ARB_texture_buffer_object;		}
	bool Extensions::isGL_ARB_draw_indirect()				const { return mGL_ARB_draw_indirect;				}
	bool Extensions::isGL_ARB_multi_draw_indirect()			const { return mGL_ARB_multi_draw_indirect;			}
	bool Extensions::isGL_ARB_vertex_shader()				const { return mGL_ARB_vertex_shader;				}
	bool Extensions::isGL_ARB_tessellation_shader()			const { return mGL_ARB_tessellation_shader;			}
	bool Extensions::isGL_ARB_geometry_shader4()			const { return mGL_ARB_geometry_shader4;			}
	bool Extensions::isGL_ARB_fragment_shader()				const { return mGL_ARB_fragment_shader;				}
	bool Extensions::isGL_ARB_draw_instanced()				const { return mGL_ARB_draw_instanced;				}
	bool Extensions::isGL_ARB_instanced_arrays()			const { return mGL_ARB_instanced_arrays;			}
	bool Extensions::isGL_ARB_vertex_array_object()			const { return mGL_ARB_vertex_array_object;			}
	bool Extensions::isGL_ARB_sampler_objects()				const { return mGL_ARB_sampler_objects;				}
	bool Extensions::isGL_ARB_draw_elements_base_vertex()	const { return mGL_ARB_draw_elements_base_vertex;	}
	bool Extensions::isGL_ARB_debug_output()				const { return mGL_ARB_debug_output;				}
	bool Extensions::isGL_ARB_direct_state_access()			const { return mGL_ARB_direct_state_access;			}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool Extensions::isSupported(const char *extension) const
	{
		// Check whether or not the given extension string pointer is valid
		if (nullptr != extension)
		{
			// Is the extension supported by the hardware?
			if (checkExtension(extension))
			{
				// Extension is supported!
				return true;
			}
		}

		// Extension isn't supported!
		return false;
	}

	bool Extensions::checkExtension(const char *extension) const
	{
		// Check whether or not the given extension string pointer is valid
		if (nullptr != extension)
		{
			// Under Windows all available extensions can be received via one additional function
			// but under Linux there are two additional functions for this
			#ifdef WIN32
				// "glGetString()" & "wglGetExtensionsStringARB()"
				const int numberOfLoops = 2;
			#elif APPLE
				// On Mac OS X, only "glGetString(GL_EXTENSIONS)" is required
				const int numberOfLoops = 1;
			#elif LINUX
				// "glGetString()" & "glXQueryExtensionsString()" & "glXGetClientString()"
				const int numberOfLoops = 3;
			#endif
			const char *extensions = nullptr;
			for (int loopIndex = 0; loopIndex < numberOfLoops; ++loopIndex)
			{
				// Extension names should not have spaces
				const char *where = strchr(extension, ' ');
				if (nullptr != where || '\0' == *extension)
				{
					return false; // Extension not found
				}

				// Advanced extensions
				// TODO(sw) Move the query for advanced extensions (via platform specific methods) to the context?
				if (loopIndex > 0)
				{
					#ifdef WIN32
						// WGL extensions
						if (!mWGL_ARB_extensions_string)
						{
							// Extension not found
							return false;
						}
						extensions = static_cast<const char*>(wglGetExtensionsStringARB(wglGetCurrentDC()));
					#elif APPLE
						// On Mac OS X, only "glGetString(GL_EXTENSIONS)" is required
					#elif LINUX
						// Get the X server display connection
						Display *display = static_cast<ContextLinux&>(*mContext).getDisplay();
						if (nullptr != display)
						{
							if (2 == loopIndex)
							{
								extensions = static_cast<const char*>(glXQueryExtensionsString(display, XDefaultScreen(display)));
							}
							else
							{
								extensions = static_cast<const char*>(glXGetClientString(display, GLX_EXTENSIONS));
							}
						}
					#endif

				// Normal extensions
				}
				else
				{
					extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
					if (nullptr == extensions)
					{
						// "glGetString(GL_EXTENSIONS)" is not available in core profiles, we have to use "glGetStringi()" 
						int numberOfExtensions = 0;
						glGetIntegerv(GL_NUM_EXTENSIONS, &numberOfExtensions);
						for (GLuint extensionIndex = 0; extensionIndex < static_cast<GLuint>(numberOfExtensions); ++extensionIndex)
						{
							if (0 == strcmp(extension, reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, extensionIndex))))
							{
								// Extension found
								return true;
							}
						}
					}
				}
				if (nullptr != extensions)
				{
					// It takes a bit of care to be fool-proof about parsing the
					// OpenGL extensions string. Don't be fooled by substrings,
					// etc:
					const char *start = extensions;
					where = strstr(start, extension);
					while (nullptr != where)
					{
						const char *terminator = where + strlen(extension);
						if ((where == start || ' ' == *(where - 1)) && (' ' == *terminator || '\0' == *terminator))
						{
							// Extension found
							return true;
						}
						start = terminator;
						where = strstr(start, extension);
					}
				}
			}
		}

		// Extension not found
		return false;
	}

	void Extensions::resetExtensions()
	{
		mInitialized = false;

		// Extensions
		// WGL (Windows only)
		mWGL_ARB_extensions_string			= false;
		// EXT
		mGL_EXT_texture_lod_bias			= false;
		mGL_EXT_texture_filter_anisotropic	= false;
		mGL_EXT_texture_array				= false;
		mGL_EXT_texture3D					= false;
		mGL_EXT_direct_state_access			= false;
		// ARB
		mGL_ARB_framebuffer_object			= false;
		mGL_ARB_multitexture				= false;
		mGL_ARB_vertex_buffer_object		= false;
		mGL_ARB_texture_compression			= false;
		mGL_ARB_vertex_program				= false;
		mGL_ARB_fragment_program			= false;
		mGL_ARB_draw_buffers				= false;
		mGL_ARB_shader_objects				= false;
		mGL_ARB_separate_shader_objects		= false;
		mGL_ARB_get_program_binary			= false;
		mGL_ARB_uniform_buffer_object		= false;
		mGL_ARB_texture_buffer_object		= false;
		mGL_ARB_draw_indirect				= false;
		mGL_ARB_multi_draw_indirect			= false;
		mGL_ARB_vertex_shader				= false;
		mGL_ARB_tessellation_shader			= false;
		mGL_ARB_geometry_shader4			= false;
		mGL_ARB_fragment_shader				= false;
		mGL_ARB_draw_instanced				= false;
		mGL_ARB_instanced_arrays			= false;
		mGL_ARB_vertex_array_object			= false;
		mGL_ARB_sampler_objects				= false;
		mGL_ARB_draw_elements_base_vertex	= false;
		mGL_ARB_debug_output				= false;
		mGL_ARB_direct_state_access			= false;
	}

	bool Extensions::initializeUniversal()
	{
		// Define a platform dependent helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																													\
				if (result)																																	\
				{																																			\
					void *symbol = wglGetProcAddress(#funcName);																							\
					if (nullptr != symbol)																													\
					{																																		\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																					\
					}																																		\
					else																																	\
					{																																		\
						RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: Failed to locate the entry point \"%s\" within the OpenGL shared library", #funcName)	\
						result = false;																														\
					}																																		\
				}
		#elif APPLE
			// For OpenGL extension handling, Apple provides several documents like
			// - "Technical Note TN2080 Understanding and Detecting OpenGL Functionality" (http://developer.apple.com/library/mac/#technotes/tn2080/_index.html)
			// - "Cross-Development Programming Guide" (http://www.filibeto.org/unix/macos/lib/dev/documentation/DeveloperTools/Conceptual/cross_development/cross_development.pdf)
			// -> All referencing to "QA1188: GetProcAdress and OpenGL Entry Points" (http://developer.apple.com/qa/qa2001/qa1188.html).
			//    Sadly, it appears that this site no longer exists.
			// -> It appears that for Mac OS X v10.6 >, the "dlopen"-way is recommended.
			#define IMPORT_FUNC(funcName)																													\
				if (result)																																	\
				{																																			\
					void *symbol = m_pOpenGLSharedLibrary ? dlsym(mOpenGLSharedLibrary, #funcName) : nullptr;												\
					if (nullptr != symbol)																													\
					{																																		\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																					\
					}																																		\
					else																																	\
					{																																		\
						RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: Failed to locate the entry point \"%s\" within the OpenGL shared library", #funcName)	\
						result = false;																														\
					}																																		\
				}
		#elif LINUX
			typedef void (*GLfunction)();
			#define IMPORT_FUNC(funcName)																													\
				if (result)																																	\
				{																																			\
					GLfunction symbol = glXGetProcAddressARB(reinterpret_cast<const GLubyte*>(#funcName));													\
					if (nullptr != symbol)																													\
					{																																		\
						*(reinterpret_cast<GLfunction*>(&(funcName))) = symbol;																				\
					}																																		\
					else																																	\
					{																																		\
						RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: Failed to locate the entry point \"%s\" within the OpenGL shared library", #funcName)	\
						result = false;																														\
					}																																		\
				}
		#endif

		// One thing about OpenGL versions and extensions: In case we're using a certain OpenGL core profile, graphics driver implementations might
		// decide to not list OpenGL extensions which are a part of this OpenGL core profile. Such a behavior was first noted using Linux Mesa 3D.
		// When not taking this into account, horrible things will happen.
		GLint profile = 0;
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
		const bool isCoreProfile = (profile & GL_CONTEXT_CORE_PROFILE_BIT);


		//[-------------------------------------------------------]
		//[ EXT                                                   ]
		//[-------------------------------------------------------]

		// GL_EXT_texture_lod_bias
		mGL_EXT_texture_lod_bias = isCoreProfile ? true : isSupported("GL_EXT_texture_lod_bias");	// Is core feature since OpenGL 1.4

		// GL_EXT_texture_filter_anisotropic
		mGL_EXT_texture_filter_anisotropic = isSupported("GL_EXT_texture_filter_anisotropic");

		// GL_EXT_texture_array
		mGL_EXT_texture_array = isSupported("GL_EXT_texture_array");

		// GL_EXT_texture3D
		mGL_EXT_texture3D = isCoreProfile ? true : isSupported("GL_EXT_texture3D");	// Is core feature since OpenGL 1.2
		if (mGL_EXT_texture3D)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glTexImage3DEXT)
			mGL_EXT_texture3D = result;
		}

		// GL_EXT_direct_state_access
		mGL_EXT_direct_state_access = isSupported("GL_EXT_direct_state_access");
		if (mGL_EXT_direct_state_access)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glNamedBufferDataEXT)
			IMPORT_FUNC(glNamedBufferSubDataEXT)
			IMPORT_FUNC(glMapNamedBufferEXT)
			IMPORT_FUNC(glUnmapNamedBufferEXT)
			IMPORT_FUNC(glProgramUniform1fEXT)
			IMPORT_FUNC(glProgramUniform2fvEXT)
			IMPORT_FUNC(glProgramUniform3fvEXT)
			IMPORT_FUNC(glProgramUniform4fvEXT)
			IMPORT_FUNC(glProgramUniformMatrix3fvEXT)
			IMPORT_FUNC(glProgramUniformMatrix4fvEXT)
			IMPORT_FUNC(glTextureImage2DEXT)
			IMPORT_FUNC(glTextureImage3DEXT)
			IMPORT_FUNC(glTextureParameteriEXT)
			IMPORT_FUNC(glGenerateTextureMipmapEXT)
			IMPORT_FUNC(glCompressedTextureImage2DEXT)
			IMPORT_FUNC(glVertexArrayVertexAttribOffsetEXT)
			IMPORT_FUNC(glEnableVertexArrayAttribEXT)
			IMPORT_FUNC(glBindMultiTextureEXT)
			IMPORT_FUNC(glNamedFramebufferTexture2DEXT)
			IMPORT_FUNC(glCheckNamedFramebufferStatusEXT)
			IMPORT_FUNC(glNamedRenderbufferStorageEXT);
			IMPORT_FUNC(glNamedFramebufferRenderbufferEXT);
			mGL_EXT_direct_state_access = result;
		}


		//[-------------------------------------------------------]
		//[ ARB                                                   ]
		//[-------------------------------------------------------]
		// GL_ARB_framebuffer_object
		mGL_ARB_framebuffer_object = isSupported("GL_ARB_framebuffer_object");
		if (mGL_ARB_framebuffer_object)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glBindRenderbuffer)
			IMPORT_FUNC(glDeleteRenderbuffers)
			IMPORT_FUNC(glGenRenderbuffers)
			IMPORT_FUNC(glRenderbufferStorage)
			IMPORT_FUNC(glBindFramebuffer)
			IMPORT_FUNC(glDeleteFramebuffers)
			IMPORT_FUNC(glGenFramebuffers)
			IMPORT_FUNC(glCheckFramebufferStatus)
			IMPORT_FUNC(glFramebufferTexture2D)
			IMPORT_FUNC(glFramebufferRenderbuffer)
			IMPORT_FUNC(glGenerateMipmap)
			mGL_ARB_framebuffer_object = result;
		}

		// GL_ARB_multitexture
		mGL_ARB_multitexture = isCoreProfile ? true : isSupported("GL_ARB_multitexture");	// Is core feature since OpenGL 1.3
		if (mGL_ARB_multitexture)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glActiveTextureARB)
			mGL_ARB_multitexture = result;
		}

		// GL_ARB_vertex_buffer_object
		mGL_ARB_vertex_buffer_object = isCoreProfile ? true : isSupported("GL_ARB_vertex_buffer_object");	// Is core feature since OpenGL 1.5
		if (mGL_ARB_vertex_buffer_object)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glBindBufferARB)
			IMPORT_FUNC(glDeleteBuffersARB)
			IMPORT_FUNC(glGenBuffersARB)
			IMPORT_FUNC(glBufferDataARB)
			IMPORT_FUNC(glBufferSubDataARB)
			IMPORT_FUNC(glMapBufferARB)
			IMPORT_FUNC(glUnmapBufferARB)
			mGL_ARB_vertex_buffer_object = result;
		}

		// GL_ARB_texture_compression
		mGL_ARB_texture_compression = isCoreProfile ? true : isSupported("GL_ARB_texture_compression");	// Is core since OpenGL 1.3
		if (mGL_ARB_texture_compression)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glCompressedTexImage2DARB)
			mGL_ARB_texture_compression = result;
		}

		// GL_ARB_vertex_program
		mGL_ARB_vertex_program = isCoreProfile ? true : isSupported("GL_ARB_vertex_program");
		if (mGL_ARB_vertex_program)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glVertexAttribPointerARB)
			IMPORT_FUNC(glEnableVertexAttribArrayARB)
			IMPORT_FUNC(glDisableVertexAttribArrayARB)
			mGL_ARB_vertex_program = result;
		}

		// GL_ARB_fragment_program (we do not need any of the functions this extension provides)
		mGL_ARB_fragment_program = isSupported("GL_ARB_fragment_program");

		// GL_ARB_draw_buffers
		mGL_ARB_draw_buffers = isSupported("GL_ARB_draw_buffers");
		if (mGL_ARB_draw_buffers)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDrawBuffersARB)
			mGL_ARB_draw_buffers = result;
		}

		// GL_ARB_shader_objects
		mGL_ARB_shader_objects = isSupported("GL_ARB_shader_objects");
		if (mGL_ARB_shader_objects)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDeleteObjectARB)
			IMPORT_FUNC(glGetHandleARB)
			IMPORT_FUNC(glDetachObjectARB)
			IMPORT_FUNC(glCreateShaderObjectARB)
			IMPORT_FUNC(glShaderSourceARB)
			IMPORT_FUNC(glCompileShaderARB)
			IMPORT_FUNC(glCreateProgramObjectARB)
			IMPORT_FUNC(glAttachObjectARB)
			IMPORT_FUNC(glLinkProgramARB)
			IMPORT_FUNC(glUseProgramObjectARB)
			IMPORT_FUNC(glUniform1fARB)
			IMPORT_FUNC(glUniform1iARB)
			IMPORT_FUNC(glUniform2iARB)
			IMPORT_FUNC(glUniform3iARB)
			IMPORT_FUNC(glUniform4iARB)
			IMPORT_FUNC(glUniform2fvARB)
			IMPORT_FUNC(glUniform3fvARB)
			IMPORT_FUNC(glUniform4fvARB)
			IMPORT_FUNC(glUniformMatrix3fvARB)
			IMPORT_FUNC(glUniformMatrix4fvARB)
			IMPORT_FUNC(glGetObjectParameterivARB)
			IMPORT_FUNC(glGetInfoLogARB)
			IMPORT_FUNC(glGetUniformLocationARB)
			mGL_ARB_shader_objects = result;
		}

		// GL_ARB_separate_shader_objects
		mGL_ARB_separate_shader_objects = isSupported("GL_ARB_separate_shader_objects");
		if (mGL_ARB_separate_shader_objects)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glCreateShaderProgramv)
			IMPORT_FUNC(glDeleteProgram)
			IMPORT_FUNC(glGenProgramPipelines)
			IMPORT_FUNC(glDeleteProgramPipelines)
			IMPORT_FUNC(glBindProgramPipeline)
			IMPORT_FUNC(glUseProgramStages)
			IMPORT_FUNC(glValidateProgramPipeline)
			IMPORT_FUNC(glGetProgramPipelineiv)
			IMPORT_FUNC(glGetProgramPipelineInfoLog)
			IMPORT_FUNC(glActiveShaderProgram)
			mGL_ARB_separate_shader_objects = result;
		}

		// GL_ARB_get_programinary
		mGL_ARB_get_program_binary = isSupported("GL_ARB_get_program_binary");
		if (mGL_ARB_get_program_binary)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glProgramParameteri)
			mGL_ARB_get_program_binary = result;
		}

		// GL_ARB_uniform_buffer_object
		mGL_ARB_uniform_buffer_object = isSupported("GL_ARB_uniform_buffer_object");
		if (mGL_ARB_uniform_buffer_object)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glGetUniformBlockIndex)
			IMPORT_FUNC(glUniformBlockBinding)
			IMPORT_FUNC(glBindBufferBase)
			mGL_ARB_uniform_buffer_object = result;
		}

		// GL_ARB_texture_buffer_object
		mGL_ARB_texture_buffer_object = isSupported("GL_ARB_texture_buffer_object");
		if (mGL_ARB_texture_buffer_object)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glTexBufferARB)
			mGL_ARB_texture_buffer_object = result;
		}

		// GL_ARB_draw_indirect
		mGL_ARB_draw_indirect = isSupported("GL_ARB_draw_indirect");
		if (mGL_ARB_draw_indirect)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDrawArraysIndirect)
			IMPORT_FUNC(glDrawElementsIndirect)
			mGL_ARB_draw_indirect = result;
		}

		// GL_ARB_multi_draw_indirect
		mGL_ARB_multi_draw_indirect = isSupported("GL_ARB_multi_draw_indirect");
		if (mGL_ARB_multi_draw_indirect)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glMultiDrawArraysIndirect)
			IMPORT_FUNC(glMultiDrawElementsIndirect)
			mGL_ARB_multi_draw_indirect = result;
		}

		// GL_ARB_vertex_shader
		mGL_ARB_vertex_shader = isSupported("GL_ARB_vertex_shader");
		if (mGL_ARB_vertex_shader)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glBindAttribLocationARB)
			mGL_ARB_vertex_shader = result;
		}

		// GL_ARB_tessellation_shader
		mGL_ARB_tessellation_shader = isSupported("GL_ARB_tessellation_shader");
		if (mGL_ARB_tessellation_shader)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glPatchParameteri)
			mGL_ARB_tessellation_shader = result;
		}

		// GL_ARB_fragment_shader
		mGL_ARB_fragment_shader = isSupported("GL_ARB_fragment_shader");

		// GL_ARB_geometry_shader4
		// TODO(sw) This extension was promoted to core feature but heavily modified source: https://www.khronos.org/opengl/wiki/History_of_OpenGL#OpenGL_3.2_.282009.29
		// TODO(sw) But this extension doesn't show up with mesa 3D either with an old OpenGL context (max OpenGL 3.3) or with an profile context (with OpenGL 4.3)
		mGL_ARB_geometry_shader4 = isSupported("GL_ARB_geometry_shader4");
		if (mGL_ARB_geometry_shader4)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glProgramParameteriARB)
			mGL_ARB_geometry_shader4 = result;
		}

		// GL_ARB_draw_instanced
		mGL_ARB_draw_instanced = isSupported("GL_ARB_draw_instanced");
		if (mGL_ARB_draw_instanced)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDrawArraysInstancedARB)
			IMPORT_FUNC(glDrawElementsInstancedARB)
			mGL_ARB_draw_instanced = result;
		}

		// GL_ARB_instanced_arrays
		mGL_ARB_instanced_arrays = isSupported("GL_ARB_instanced_arrays");
		if (mGL_ARB_instanced_arrays)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glVertexAttribDivisorARB)
			mGL_ARB_instanced_arrays = result;
		}

		// GL_ARB_vertex_array_object
		mGL_ARB_vertex_array_object = isSupported("GL_ARB_vertex_array_object");
		if (mGL_ARB_vertex_array_object)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glBindVertexArray)
			IMPORT_FUNC(glDeleteVertexArrays)
			IMPORT_FUNC(glGenVertexArrays)
			mGL_ARB_vertex_array_object = result;
		}

		// GL_ARB_sampler_objects
		mGL_ARB_sampler_objects = isSupported("GL_ARB_sampler_objects");
		if (mGL_ARB_sampler_objects)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glGenSamplers)
			IMPORT_FUNC(glDeleteSamplers)
			IMPORT_FUNC(glBindSampler)
			IMPORT_FUNC(glSamplerParameteri)
			IMPORT_FUNC(glSamplerParameterf)
			IMPORT_FUNC(glSamplerParameterfv)
			mGL_ARB_sampler_objects = result;
		}

		// GL_ARB_draw_elements_base_vertex
		mGL_ARB_draw_elements_base_vertex = isSupported("GL_ARB_draw_elements_base_vertex");
		if (mGL_ARB_draw_elements_base_vertex)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDrawElementsBaseVertex)
			IMPORT_FUNC(glDrawElementsInstancedBaseVertex)
			mGL_ARB_draw_elements_base_vertex = result;
		}

		// GL_ARB_debug_output
		mGL_ARB_debug_output = isSupported("GL_ARB_debug_output");
		if (mGL_ARB_debug_output)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDebugMessageCallbackARB)
			mGL_ARB_debug_output = result;
		}

		// GL_ARB_direct_state_access
		mGL_ARB_direct_state_access = isSupported("GL_ARB_direct_state_access");
		if (mGL_ARB_direct_state_access)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glNamedBufferData)
			IMPORT_FUNC(glNamedBufferSubData)
			IMPORT_FUNC(glMapNamedBuffer)
			IMPORT_FUNC(glUnmapNamedBuffer)
			IMPORT_FUNC(glProgramUniform1f)
			IMPORT_FUNC(glProgramUniform2fv)
			IMPORT_FUNC(glProgramUniform3fv)
			IMPORT_FUNC(glProgramUniform4fv)
			IMPORT_FUNC(glProgramUniformMatrix3fv)
			IMPORT_FUNC(glProgramUniformMatrix4fv)
			IMPORT_FUNC(glTextureParameteri)
			IMPORT_FUNC(glGenerateTextureMipmap)
			IMPORT_FUNC(glEnableVertexArrayAttrib)
			IMPORT_FUNC(glCheckNamedFramebufferStatus)
			IMPORT_FUNC(glNamedRenderbufferStorage)
			IMPORT_FUNC(glNamedFramebufferRenderbuffer)
			IMPORT_FUNC(glNamedFramebufferTexture)
			IMPORT_FUNC(glTextureBuffer)
			IMPORT_FUNC(glBindTextureUnit)
			IMPORT_FUNC(glCompressedTextureSubImage2D)
			IMPORT_FUNC(glTextureSubImage2D)
			IMPORT_FUNC(glTextureSubImage3D)
			IMPORT_FUNC(glVertexArrayAttribFormat)
			IMPORT_FUNC(glVertexArrayVertexBuffer)
			IMPORT_FUNC(glVertexArrayBindingDivisor)
			IMPORT_FUNC(glVertexArrayElementBuffer)
			IMPORT_FUNC(glTextureStorage2D)
			IMPORT_FUNC(glTextureStorage3D)
			mGL_ARB_direct_state_access = result;
		}

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
