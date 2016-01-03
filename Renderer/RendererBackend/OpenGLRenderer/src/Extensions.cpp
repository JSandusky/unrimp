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
		// Nothing to do in here
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
	bool Extensions::isWGL_EXT_swap_control()				const { return mWGL_EXT_swap_control;				}
	bool Extensions::isWGL_ARB_pixel_format()				const { return mWGL_ARB_pixel_format;				}
	bool Extensions::isWGL_ARB_render_texture()				const { return mWGL_ARB_render_texture;				}
	bool Extensions::isWGL_ARB_make_current_read()			const { return mWGL_ARB_make_current_read;			}
	bool Extensions::isWGL_ARB_multisample()				const { return mWGL_ARB_multisample;				}
	bool Extensions::isWGL_ATI_pixel_format_float()			const { return mWGL_ATI_pixel_format_float;			}
	bool Extensions::isWGL_NV_float_buffer()				const { return mWGL_NV_float_buffer;				}
	// GLX (Linux only)
	bool Extensions::isGLX_SGI_swap_control()				const { return mGLX_SGI_swap_control;				}
	// EXT
	bool Extensions::isGL_EXT_compiled_vertex_array()		const { return mGL_EXT_compiled_vertex_array;		}
	bool Extensions::isGL_EXT_draw_range_elements()			const { return mGL_EXT_draw_range_elements;			}
	bool Extensions::isGL_EXT_fog_coord()					const { return mGL_EXT_fog_coord;					}
	bool Extensions::isGL_EXT_secondary_color()				const { return mGL_EXT_secondary_color;				}
	bool Extensions::isGL_EXT_texture_compression_s3tc()	const { return mGL_EXT_texture_compression_s3tc;	}
	bool Extensions::isGL_EXT_texture_compression_latc()	const { return mGL_EXT_texture_compression_latc;	}
	bool Extensions::isGL_EXT_texture_lod_bias()			const { return mGL_EXT_texture_lod_bias;			}
	bool Extensions::isGL_EXT_texture_filter_anisotropic()	const { return mGL_EXT_texture_filter_anisotropic;	}
	bool Extensions::isGL_EXT_separate_specular_color()		const { return mGL_EXT_separate_specular_color;		}
	bool Extensions::isGL_EXT_texture_edge_clamp()			const { return mGL_EXT_texture_edge_clamp;			}
	bool Extensions::isGL_EXT_texture_array()				const { return mGL_EXT_texture_array;				}
	bool Extensions::isGL_EXT_texture_rectangle()			const { return mGL_EXT_texture_rectangle;			}
	bool Extensions::isGL_EXT_texture3D()					const { return mGL_EXT_texture3D;					}
	bool Extensions::isGL_EXT_texture_cube_map()			const { return mGL_EXT_texture_cube_map;			}
	bool Extensions::isGL_EXT_stencil_wrap()				const { return mGL_EXT_stencil_wrap;				}
	bool Extensions::isGL_EXT_stencil_two_side()			const { return mGL_EXT_stencil_two_side;			}
	bool Extensions::isGL_EXT_packed_depth_stencil()		const { return mGL_EXT_packed_depth_stencil;		}
	bool Extensions::isGL_EXT_depth_bounds_test()			const { return mGL_EXT_depth_bounds_test;			}
	bool Extensions::isGL_EXT_framebuffer_multisample()		const { return mGL_EXT_framebuffer_multisample;		}
	bool Extensions::isGL_EXT_framebuffer_blit()			const { return mGL_EXT_framebuffer_blit;			}
	bool Extensions::isGL_EXT_transform_feedback()			const { return mGL_EXT_transform_feedback;			}
	bool Extensions::isGL_EXT_direct_state_access()			const { return mGL_EXT_direct_state_access;			}
	// ARB
	bool Extensions::isGL_ARB_framebuffer_object()			const { return mGL_ARB_framebuffer_object;			}
	bool Extensions::isGL_ARB_texture_float()				const { return mGL_ARB_texture_float;				}
	bool Extensions::isGL_ARB_color_buffer_float()			const { return mGL_ARB_color_buffer_float;			}
	bool Extensions::isGL_ARB_multitexture()				const { return mGL_ARB_multitexture;				}
	bool Extensions::isGL_ARB_vertex_buffer_object()		const { return mGL_ARB_vertex_buffer_object;		}
	bool Extensions::isGL_ARB_texture_border_clamp()		const { return mGL_ARB_texture_border_clamp;		}
	bool Extensions::isGL_ARB_texture_mirrored_repeat()		const { return mGL_ARB_texture_mirrored_repeat;		}
	bool Extensions::isGL_ARB_texture_cube_map()			const { return mGL_ARB_texture_cube_map;			}
	bool Extensions::isGL_ARB_texture_env_combine()			const { return mGL_ARB_texture_env_combine;			}
	bool Extensions::isGL_ARB_texture_env_dot3()			const { return mGL_ARB_texture_env_dot3;			}
	bool Extensions::isGL_ARB_occlusion_query()				const { return mGL_ARB_occlusion_query;				}
	bool Extensions::isGL_ARB_texture_compression()			const { return mGL_ARB_texture_compression;			}
	bool Extensions::isGL_ARB_depth_texture()				const { return mGL_ARB_depth_texture;				}
	bool Extensions::isGL_ARB_point_sprite()				const { return mGL_ARB_point_sprite;				}
	bool Extensions::isGL_ARB_point_parameters()			const { return mGL_ARB_point_parameters;			}
	bool Extensions::isGL_ARB_shading_language_100()		const { return mGL_ARB_shading_language_100;		}
	bool Extensions::isGL_ARB_vertex_program()				const { return mGL_ARB_vertex_program;				}
	bool Extensions::isGL_ARB_fragment_program()			const { return mGL_ARB_fragment_program;			}
	bool Extensions::isGL_ARB_draw_buffers()				const { return mGL_ARB_draw_buffers;				}
	bool Extensions::isGL_ARB_shader_objects()				const { return mGL_ARB_shader_objects;				}
	bool Extensions::isGL_ARB_get_program_binary()			const { return mGL_ARB_get_program_binary;			}
	bool Extensions::isGL_ARB_texture_non_power_of_two()	const { return mGL_ARB_texture_non_power_of_two;	}
	bool Extensions::isGL_ARB_texture_rectangle()			const { return mGL_ARB_texture_rectangle;			}
	bool Extensions::isGL_ARB_multisample()					const { return mGL_ARB_multisample;					}
	bool Extensions::isGL_ARB_uniform_buffer_object()		const { return mGL_ARB_uniform_buffer_object;		}
	bool Extensions::isGL_ARB_texture_buffer_object()		const { return mGL_ARB_texture_buffer_object;		}
	bool Extensions::isGL_ARB_half_float_vertex()			const { return mGL_ARB_half_float_vertex;			}
	bool Extensions::isGL_ARB_vertex_shader()				const { return mGL_ARB_vertex_shader;				}
	bool Extensions::isGL_ARB_tessellation_shader()			const { return mGL_ARB_tessellation_shader;			}
	bool Extensions::isGL_ARB_geometry_shader4()			const { return mGL_ARB_geometry_shader4;			}
	bool Extensions::isGL_ARB_fragment_shader()				const { return mGL_ARB_fragment_shader;				}
	bool Extensions::isGL_ARB_depth_clamp()					const { return mGL_ARB_depth_clamp;					}
	bool Extensions::isGL_ARB_draw_instanced()				const { return mGL_ARB_draw_instanced;				}
	bool Extensions::isGL_ARB_instanced_arrays()			const { return mGL_ARB_instanced_arrays;			}
	bool Extensions::isGL_ARB_vertex_array_object()			const { return mGL_ARB_vertex_array_object;			}
	bool Extensions::isGL_ARB_sampler_objects()				const { return mGL_ARB_sampler_objects;				}
	bool Extensions::isGL_ARB_draw_elements_base_vertex()	const { return mGL_ARB_draw_elements_base_vertex;	}
	bool Extensions::isGL_ARB_debug_output()				const { return mGL_ARB_debug_output;				}
	// ATI
	bool Extensions::isGL_ATI_meminfo()						const { return mGL_ATI_meminfo;						}
	bool Extensions::isGL_ATI_separate_stencil()			const { return mGL_ATI_separate_stencil;			}
	bool Extensions::isGL_ATI_draw_buffers()				const { return mGL_ATI_draw_buffers;				}
	bool Extensions::isGL_ATI_texture_compression_3dc()		const { return mGL_ATI_texture_compression_3dc;		}
	// AMD
	bool Extensions::isGL_AMD_vertex_shader_tessellator()	const { return mGL_AMD_vertex_shader_tessellator;	}
	// NV
	bool Extensions::isGL_NV_texture_rectangle()			const { return mGL_NV_texture_rectangle;			}
	bool Extensions::isGL_NV_occlusion_query()				const { return mGL_NV_occlusion_query;				}
	bool Extensions::isGL_NVX_gpu_memory_info()				const { return mGL_NVX_gpu_memory_info;				}
	bool Extensions::isGL_NV_half_float()					const { return mGL_NV_half_float;					}
	bool Extensions::isGL_NV_texture_compression_vtc()		const { return mGL_NV_texture_compression_vtc;		}
	// SGIS
	bool Extensions::isGL_SGIS_generate_mipmap()			const { return mGL_SGIS_generate_mipmap;			}
	// HP
	bool Extensions::isGL_HP_occlusion_test()				const { return mGL_HP_occlusion_test;				}


	///////////////////////////////////////////////////////////
	// Misc
	///////////////////////////////////////////////////////////
	GLint Extensions::getGL_MAX_ELEMENTS_VERTICES_EXT() const	{ return mGL_MAX_ELEMENTS_VERTICES_EXT; }
	GLint Extensions::getGL_MAX_ELEMENTS_INDICES_EXT() const	{ return mGL_MAX_ELEMENTS_INDICES_EXT; }


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
				const int loopMax = 2;
			#elif APPLE
				// On Mac OS X, only "glGetString(GL_EXTENSIONS)" is required
				const int loopMax = 1;
			#elif LINUX
				// "glGetString()" & "glXQueryExtensionsString()" & "glXGetClientString()"
				const int loopMax = 3;
			#endif
			const char *extensions = nullptr;
			for (int i = 0; i < loopMax; ++i)
			{
				// Extension names should not have spaces
				const char *where = strchr(extension, ' ');
				if (nullptr != where || '\0' == *extension)
				{
					return false; // Extension not found
				}

				// Advanced extensions
				// TODO(sw) Move the query for advanced extensions (via platfrom specific methods) to the context?
				if (i > 0)
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
							if (2 == i)
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
				}
				if (nullptr == extensions)
				{
					return false; // Extension not found
				}

				// It takes a bit of care to be fool-proof about parsing the
				// OpenGL extensions string. Don't be fooled by substrings,
				// etc:
				const char *start = extensions;
				where = strstr(start, extension);
				while (nullptr != where)
				{
					const char *terminator = where + strlen(extension);
					if (where == start || ' ' == *(where - 1) || ' ' == *terminator || '\0' == *terminator)
					{
						// Extension found
						return true;
					}
					start = terminator;
					where = strstr(start, extension);
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
		mWGL_EXT_swap_control				= false;
		mWGL_ARB_pixel_format				= false;
		mWGL_ARB_render_texture				= false;
		mWGL_ARB_make_current_read			= false;
		mWGL_ARB_multisample				= false;
		mWGL_ATI_pixel_format_float			= false;
		mWGL_NV_float_buffer				= false;
		// GLX (Linux only)
		mGLX_SGI_swap_control				= false;
		// EXT
		mGL_EXT_compiled_vertex_array		= false;
		mGL_EXT_draw_range_elements			= false;
		mGL_EXT_fog_coord					= false;
		mGL_EXT_secondary_color				= false;
		mGL_EXT_texture_compression_s3tc	= false;
		mGL_EXT_texture_compression_latc	= false;
		mGL_EXT_texture_lod_bias			= false;
		mGL_EXT_texture_filter_anisotropic	= false;
		mGL_EXT_separate_specular_color		= false;
		mGL_EXT_texture_edge_clamp			= false;
		mGL_EXT_texture_array				= false;
		mGL_EXT_texture_rectangle			= false;
		mGL_EXT_texture3D					= false;
		mGL_EXT_texture_cube_map			= false;
		mGL_EXT_stencil_wrap				= false;
		mGL_EXT_stencil_two_side			= false;
		mGL_EXT_packed_depth_stencil		= false;
		mGL_EXT_depth_bounds_test			= false;
		mGL_EXT_framebuffer_multisample		= false;
		mGL_EXT_framebuffer_blit			= false;
		mGL_EXT_transform_feedback			= false;
		mGL_EXT_direct_state_access			= false;
		// ARB
		mGL_ARB_framebuffer_object			= false;
		mGL_ARB_texture_float				= false;
		mGL_ARB_color_buffer_float			= false;
		mGL_ARB_multitexture				= false;
		mGL_ARB_vertex_buffer_object		= false;
		mGL_ARB_texture_border_clamp		= false;
		mGL_ARB_texture_mirrored_repeat		= false;
		mGL_ARB_texture_cube_map			= false;
		mGL_ARB_texture_env_combine			= false;
		mGL_ARB_texture_env_dot3			= false;
		mGL_ARB_occlusion_query				= false;
		mGL_ARB_texture_compression			= false;
		mGL_ARB_depth_texture				= false;
		mGL_ARB_point_sprite				= false;
		mGL_ARB_point_parameters			= false;
		mGL_ARB_shading_language_100		= false;
		mGL_ARB_vertex_program				= false;
		mGL_ARB_fragment_program			= false;
		mGL_ARB_draw_buffers				= false;
		mGL_ARB_shader_objects				= false;
		mGL_ARB_get_program_binary			= false;
		mGL_ARB_texture_non_power_of_two	= false;
		mGL_ARB_texture_rectangle			= false;
		mGL_ARB_multisample					= false;
		mGL_ARB_uniform_buffer_object		= false;
		mGL_ARB_texture_buffer_object		= false;
		mGL_ARB_half_float_vertex			= false;
		mGL_ARB_vertex_shader				= false;
		mGL_ARB_tessellation_shader			= false;
		mGL_ARB_geometry_shader4			= false;
		mGL_ARB_fragment_shader				= false;
		mGL_ARB_depth_clamp					= false;
		mGL_ARB_draw_instanced				= false;
		mGL_ARB_instanced_arrays			= false;
		mGL_ARB_vertex_array_object			= false;
		mGL_ARB_sampler_objects				= false;
		mGL_ARB_draw_elements_base_vertex	= false;
		mGL_ARB_debug_output				= false;
		// ATI
		mGL_ATI_meminfo						= false;
		mGL_ATI_separate_stencil			= false;
		mGL_ATI_draw_buffers				= false;
		mGL_ATI_texture_compression_3dc		= false;
		// AMD
		mGL_AMD_vertex_shader_tessellator	= false;
		// NV
		mGL_NV_texture_rectangle			= false;
		mGL_NV_occlusion_query				= false;
		mGL_NVX_gpu_memory_info				= false;
		mGL_NV_half_float					= false;
		mGL_NV_texture_compression_vtc		= false;
		// SGIS
		mGL_SGIS_generate_mipmap			= false;
		// HP
		mGL_HP_occlusion_test				= false;
		// Misc
		mGL_MAX_ELEMENTS_VERTICES_EXT = mGL_MAX_ELEMENTS_INDICES_EXT = 0;
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


		//[-------------------------------------------------------]
		//[ EXT                                                   ]
		//[-------------------------------------------------------]
		// GL_EXT_compiled_vertex_array
		mGL_EXT_compiled_vertex_array = isSupported("GL_EXT_compiled_vertex_array");
		if (mGL_EXT_compiled_vertex_array)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glLockArraysEXT)
			IMPORT_FUNC(glUnlockArraysEXT)
			mGL_EXT_compiled_vertex_array = result;
		}

		// GL_EXT_draw_range_elements
		mGL_EXT_draw_range_elements = isSupported("GL_EXT_draw_range_elements");
		if (mGL_EXT_draw_range_elements)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDrawRangeElementsEXT)
			mGL_EXT_draw_range_elements = result;
		}
		if (mGL_EXT_draw_range_elements)
		{	
			glGetIntegerv(GL_MAX_ELEMENTS_VERTICES_EXT, &mGL_MAX_ELEMENTS_VERTICES_EXT);
			glGetIntegerv(GL_MAX_ELEMENTS_INDICES_EXT,	&mGL_MAX_ELEMENTS_INDICES_EXT);
		}
		else
		{
			mGL_MAX_ELEMENTS_VERTICES_EXT = mGL_MAX_ELEMENTS_INDICES_EXT = 0;
		}

		// GL_EXT_fog_coord
		mGL_EXT_fog_coord = isSupported("GL_EXT_fog_coord");
		if (mGL_EXT_fog_coord)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glFogCoordfEXT)
			IMPORT_FUNC(glFogCoordfvEXT)
			IMPORT_FUNC(glFogCoorddEXT)
			IMPORT_FUNC(glFogCoorddvEXT)
			IMPORT_FUNC(glFogCoordPointerEXT)
			mGL_EXT_fog_coord = result;
		}

		// GL_EXT_secondary_color
		mGL_EXT_secondary_color = isSupported("GL_EXT_secondary_color");
		if (mGL_EXT_secondary_color)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glSecondaryColor3bEXT)
			IMPORT_FUNC(glSecondaryColor3bvEXT)
			IMPORT_FUNC(glSecondaryColor3dEXT)
			IMPORT_FUNC(glSecondaryColor3dvEXT)
			IMPORT_FUNC(glSecondaryColor3fEXT)
			IMPORT_FUNC(glSecondaryColor3fvEXT)
			IMPORT_FUNC(glSecondaryColor3iEXT)
			IMPORT_FUNC(glSecondaryColor3ivEXT)
			IMPORT_FUNC(glSecondaryColor3sEXT)
			IMPORT_FUNC(glSecondaryColor3svEXT)
			IMPORT_FUNC(glSecondaryColor3ubEXT)
			IMPORT_FUNC(glSecondaryColor3ubvEXT)
			IMPORT_FUNC(glSecondaryColor3uiEXT)
			IMPORT_FUNC(glSecondaryColor3uivEXT)
			IMPORT_FUNC(glSecondaryColor3usEXT)
			IMPORT_FUNC(glSecondaryColor3usvEXT)
			IMPORT_FUNC(glSecondaryColorPointerEXT)
			mGL_EXT_secondary_color = result;
		}

		// GL_EXT_texture_compression_s3tc
		mGL_EXT_texture_compression_s3tc = isSupported("GL_EXT_texture_compression_s3tc");

		// GL_EXT_texture_compression_latc
		if (isSupported("GL_EXT_texture_compression_latc"))
		{
			mGL_EXT_texture_compression_latc = true;
		}
		else
		{
			mGL_EXT_texture_compression_latc = false;
		}

		// GL_EXT_texture_lod_bias
		mGL_EXT_texture_lod_bias = isSupported("GL_EXT_texture_lod_bias");

		// GL_EXT_texture_filter_anisotropic
		mGL_EXT_texture_filter_anisotropic = isSupported("GL_EXT_texture_filter_anisotropic");

		// GL_EXT_separate_specular_color
		mGL_EXT_separate_specular_color = isSupported("GL_EXT_separate_specular_color");

		// GL_EXT_texture_edge_clamp
		mGL_EXT_texture_edge_clamp = isSupported("GL_EXT_texture_edge_clamp");

		// GL_EXT_texture_array
		mGL_EXT_texture_array = isSupported("GL_EXT_texture_array");

		// GL_EXT_texture_rectangle
		mGL_EXT_texture_rectangle = isSupported("GL_EXT_texture_rectangle");

		// GL_EXT_texture3D
		mGL_EXT_texture3D = isSupported("GL_EXT_texture3D");
		if (mGL_EXT_texture3D)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glTexImage3DEXT)
			IMPORT_FUNC(glTexSubImage3DEXT)
			mGL_EXT_texture3D = result;
		}

		// GL_EXT_texture_cube_map
		mGL_EXT_texture_cube_map = isSupported("GL_EXT_texture_cube_map");

		// GL_EXT_stencil_wrap
		mGL_EXT_stencil_wrap = isSupported("GL_EXT_stencil_wrap");

		// GL_EXT_stencil_two_side
		mGL_EXT_stencil_two_side = isSupported("GL_EXT_stencil_two_side");
		if (mGL_EXT_stencil_two_side)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glActiveStencilFaceEXT)
			mGL_EXT_stencil_two_side = result;
		}

		// GL_EXT_packed_depth_stencil
		mGL_EXT_packed_depth_stencil = isSupported("GL_EXT_packed_depth_stencil");

		// GL_EXT_depth_bounds_test
		mGL_EXT_depth_bounds_test = isSupported("GL_EXT_depth_bounds_test");
		if (mGL_EXT_depth_bounds_test)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDepthBoundsEXT)
			mGL_EXT_depth_bounds_test = result;
		}

		// GL_EXT_framebuffer_multisample
		mGL_EXT_framebuffer_multisample = isSupported("GL_EXT_framebuffer_multisample");
		if (mGL_EXT_framebuffer_multisample)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glRenderbufferStorageMultisampleEXT)
			mGL_EXT_framebuffer_multisample = result;
		}

		// GL_EXT_framebuffer_blit
		mGL_EXT_framebuffer_blit = isSupported("GL_EXT_framebuffer_blit");
		if (mGL_EXT_framebuffer_blit)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glBlitFramebufferEXT)
			mGL_EXT_framebuffer_blit = result;
		}

		// GL_EXT_transform_feedback
		mGL_EXT_transform_feedback = isSupported("GL_EXT_transform_feedback");
		if (mGL_EXT_transform_feedback)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glBeginTransformFeedbackEXT)
			IMPORT_FUNC(glEndTransformFeedbackEXT)
			IMPORT_FUNC(glBindBufferRangeEXT)
			IMPORT_FUNC(glBindBufferOffsetEXT)
			IMPORT_FUNC(glBindBufferBaseEXT)
			IMPORT_FUNC(glTransformFeedbackVaryingsEXT)
			IMPORT_FUNC(glGetTransformFeedbackVaryingEXT)
			mGL_EXT_transform_feedback = result;
		}

		// GL_EXT_direct_state_access
		mGL_EXT_direct_state_access = isSupported("GL_EXT_direct_state_access");
		if (mGL_EXT_direct_state_access)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glNamedBufferDataEXT)
			IMPORT_FUNC(glNamedBufferSubDataEXT)
			IMPORT_FUNC(glProgramUniform1fEXT)
			IMPORT_FUNC(glProgramUniform2fvEXT)
			IMPORT_FUNC(glProgramUniform3fvEXT)
			IMPORT_FUNC(glProgramUniform4fvEXT)
			IMPORT_FUNC(glProgramUniformMatrix3fvEXT)
			IMPORT_FUNC(glProgramUniformMatrix4fvEXT)
			IMPORT_FUNC(glProgramUniform1iEXT)
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
			IMPORT_FUNC(glIsRenderbuffer)
			IMPORT_FUNC(glBindRenderbuffer)
			IMPORT_FUNC(glDeleteRenderbuffers)
			IMPORT_FUNC(glGenRenderbuffers)
			IMPORT_FUNC(glRenderbufferStorage)
			IMPORT_FUNC(glGetRenderbufferParameteriv)
			IMPORT_FUNC(glIsFramebuffer)
			IMPORT_FUNC(glBindFramebuffer)
			IMPORT_FUNC(glDeleteFramebuffers)
			IMPORT_FUNC(glGenFramebuffers)
			IMPORT_FUNC(glCheckFramebufferStatus)
			IMPORT_FUNC(glFramebufferTexture1D)
			IMPORT_FUNC(glFramebufferTexture2D)
			IMPORT_FUNC(glFramebufferTexture3D)
			IMPORT_FUNC(glFramebufferRenderbuffer)
			IMPORT_FUNC(glGetFramebufferAttachmentParameteriv)
			IMPORT_FUNC(glGenerateMipmap)
			mGL_ARB_framebuffer_object = result;
		}

		// GL_ARB_texture_float
		mGL_ARB_texture_float = isSupported("GL_ARB_texture_float");

		// GL_ARB_color_buffer_float
		mGL_ARB_color_buffer_float = isSupported("GL_ARB_color_buffer_float");
		if (mGL_ARB_color_buffer_float)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glClampColorARB)
			mGL_ARB_color_buffer_float = result;
		}

		// GL_ARB_multitexture
		mGL_ARB_multitexture = isSupported("GL_ARB_multitexture");
		if (mGL_ARB_multitexture)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glActiveTextureARB)
			IMPORT_FUNC(glClientActiveTextureARB)
			IMPORT_FUNC(glMultiTexCoord1dARB)
			IMPORT_FUNC(glMultiTexCoord1dvARB)
			IMPORT_FUNC(glMultiTexCoord1fARB)
			IMPORT_FUNC(glMultiTexCoord1fvARB)
			IMPORT_FUNC(glMultiTexCoord1iARB)
			IMPORT_FUNC(glMultiTexCoord1ivARB)
			IMPORT_FUNC(glMultiTexCoord1sARB)
			IMPORT_FUNC(glMultiTexCoord1svARB)
			IMPORT_FUNC(glMultiTexCoord2dARB)
			IMPORT_FUNC(glMultiTexCoord2dvARB)
			IMPORT_FUNC(glMultiTexCoord2fARB)
			IMPORT_FUNC(glMultiTexCoord2fvARB)
			IMPORT_FUNC(glMultiTexCoord2iARB)
			IMPORT_FUNC(glMultiTexCoord2ivARB)
			IMPORT_FUNC(glMultiTexCoord2sARB)
			IMPORT_FUNC(glMultiTexCoord2svARB)
			IMPORT_FUNC(glMultiTexCoord3dARB)
			IMPORT_FUNC(glMultiTexCoord3dvARB)
			IMPORT_FUNC(glMultiTexCoord3fARB)
			IMPORT_FUNC(glMultiTexCoord3fvARB)
			IMPORT_FUNC(glMultiTexCoord3iARB)
			IMPORT_FUNC(glMultiTexCoord3ivARB)
			IMPORT_FUNC(glMultiTexCoord3sARB)
			IMPORT_FUNC(glMultiTexCoord3svARB)
			IMPORT_FUNC(glMultiTexCoord4dARB)
			IMPORT_FUNC(glMultiTexCoord4dvARB)
			IMPORT_FUNC(glMultiTexCoord4fARB)
			IMPORT_FUNC(glMultiTexCoord4fvARB)
			IMPORT_FUNC(glMultiTexCoord4iARB)
			IMPORT_FUNC(glMultiTexCoord4ivARB)
			IMPORT_FUNC(glMultiTexCoord4sARB)
			IMPORT_FUNC(glMultiTexCoord4svARB)
			mGL_ARB_multitexture = result;
		}

		// GL_ARB_vertex_buffer_object
		mGL_ARB_vertex_buffer_object = isSupported("GL_ARB_vertex_buffer_object");
		if (mGL_ARB_vertex_buffer_object)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glBindBufferARB)
			IMPORT_FUNC(glDeleteBuffersARB)
			IMPORT_FUNC(glGenBuffersARB)
			IMPORT_FUNC(glBufferDataARB)
			IMPORT_FUNC(glBufferSubDataARB)
			IMPORT_FUNC(glGetBufferSubDataARB)
			IMPORT_FUNC(glMapBufferARB)
			IMPORT_FUNC(glUnmapBufferARB)
			IMPORT_FUNC(glGetBufferParameterivARB)
			IMPORT_FUNC(glGetBufferPointervARB)
			mGL_ARB_vertex_buffer_object = result;
		}

		// GL_ARB_texture_border_clamp
		mGL_ARB_texture_border_clamp = isSupported("GL_ARB_texture_border_clamp");

		// GL_ARB_texture_mirrored_repeat 
		mGL_ARB_texture_mirrored_repeat = isSupported("GL_ARB_texture_mirrored_repeat");

		// GL_ARB_texture_cube_map
		mGL_ARB_texture_cube_map = isSupported("GL_ARB_texture_cube_map");

		// GL_ARB_texture_env_combine
		mGL_ARB_texture_env_combine = isSupported("GL_ARB_texture_env_combine");

		// GL_ARB_texture_env_dot3
		mGL_ARB_texture_env_dot3 = isSupported("GL_ARB_texture_env_dot3");

		// GL_ARB_occlusion_query
		mGL_ARB_occlusion_query = isSupported("GL_ARB_occlusion_query");
		if (mGL_ARB_occlusion_query)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glGenQueriesARB)
			IMPORT_FUNC(glDeleteQueriesARB)
			IMPORT_FUNC(glIsQueryARB)
			IMPORT_FUNC(glBeginQueryARB)
			IMPORT_FUNC(glEndQueryARB)
			IMPORT_FUNC(glGetQueryivARB)
			IMPORT_FUNC(glGetQueryObjectivARB)
			IMPORT_FUNC(glGetQueryObjectuivARB)
			mGL_ARB_occlusion_query = result;
		}

		// GL_ARB_texture_compression
		mGL_ARB_texture_compression = isSupported("GL_ARB_texture_compression");
		if (mGL_ARB_texture_compression)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glCompressedTexImage3DARB)
			IMPORT_FUNC(glCompressedTexImage2DARB)
			IMPORT_FUNC(glCompressedTexImage1DARB)
			IMPORT_FUNC(glCompressedTexSubImage3DARB)
			IMPORT_FUNC(glCompressedTexSubImage2DARB)
			IMPORT_FUNC(glCompressedTexSubImage1DARB)
			IMPORT_FUNC(glGetCompressedTexImageARB)
			mGL_ARB_texture_compression = result;
		}

		// GL_ARB_depth_texture
		mGL_ARB_depth_texture = isSupported("GL_ARB_depth_texture");

		// GL_ARB_point_sprite
		mGL_ARB_point_sprite = isSupported("GL_ARB_point_sprite");

		// GL_ARB_point_parameters
		mGL_ARB_point_parameters = isSupported("GL_ARB_point_parameters");
		if (mGL_ARB_point_parameters)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glPointParameterfARB)
			IMPORT_FUNC(glPointParameterfvARB)
			mGL_ARB_point_parameters = result;
		}

		// GL_ARB_shading_language_100
		mGL_ARB_shading_language_100 = isSupported("GL_ARB_shading_language_100");

		// GL_ARB_vertex_program
		mGL_ARB_vertex_program = isSupported("GL_ARB_vertex_program");
		if (mGL_ARB_vertex_program)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glVertexAttribPointerARB)
			IMPORT_FUNC(glEnableVertexAttribArrayARB)
			IMPORT_FUNC(glDisableVertexAttribArrayARB)
			IMPORT_FUNC(glGetProgramivARB)
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
			IMPORT_FUNC(glValidateProgramARB)
			IMPORT_FUNC(glUniform1fARB)
			IMPORT_FUNC(glUniform2fARB)
			IMPORT_FUNC(glUniform3fARB)
			IMPORT_FUNC(glUniform4fARB)
			IMPORT_FUNC(glUniform1iARB)
			IMPORT_FUNC(glUniform2iARB)
			IMPORT_FUNC(glUniform3iARB)
			IMPORT_FUNC(glUniform4iARB)
			IMPORT_FUNC(glUniform1fvARB)
			IMPORT_FUNC(glUniform2fvARB)
			IMPORT_FUNC(glUniform3fvARB)
			IMPORT_FUNC(glUniform4fvARB)
			IMPORT_FUNC(glUniform1ivARB)
			IMPORT_FUNC(glUniform2ivARB)
			IMPORT_FUNC(glUniform3ivARB)
			IMPORT_FUNC(glUniform4ivARB)
			IMPORT_FUNC(glUniformMatrix2fvARB)
			IMPORT_FUNC(glUniformMatrix3fvARB)
			IMPORT_FUNC(glUniformMatrix4fvARB)
			IMPORT_FUNC(glGetObjectParameterfvARB)
			IMPORT_FUNC(glGetObjectParameterivARB)
			IMPORT_FUNC(glGetInfoLogARB)
			IMPORT_FUNC(glGetAttachedObjectsARB)
			IMPORT_FUNC(glGetUniformLocationARB)
			IMPORT_FUNC(glGetActiveUniformARB)
			IMPORT_FUNC(glGetUniformfvARB)
			IMPORT_FUNC(glGetUniformivARB)
			IMPORT_FUNC(glGetShaderSourceARB)
			mGL_ARB_shader_objects = result;
		}

		// GL_ARB_get_programinary
		mGL_ARB_get_program_binary = isSupported("GL_ARB_get_program_binary");
		if (mGL_ARB_get_program_binary)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glGetProgramBinary)
			IMPORT_FUNC(glProgramBinary)
			IMPORT_FUNC(glProgramParameteri)
			mGL_ARB_get_program_binary = result;
		}

		// GL_ARB_texture_non_power_of_two
		mGL_ARB_texture_non_power_of_two = isSupported("GL_ARB_texture_non_power_of_two");

		// GL_ARB_texture_rectangle
		mGL_ARB_texture_rectangle = isSupported("GL_ARB_texture_rectangle");

		// GL_ARB_multisample
		mGL_ARB_multisample = isSupported("GL_ARB_multisample");

		// GL_ARB_uniform_buffer_object
		mGL_ARB_uniform_buffer_object = isSupported("GL_ARB_uniform_buffer_object");
		if (mGL_ARB_uniform_buffer_object)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glGetUniformIndices)
			IMPORT_FUNC(glGetActiveUniformsiv)
			IMPORT_FUNC(glGetActiveUniformName)
			IMPORT_FUNC(glGetUniformBlockIndex)
			IMPORT_FUNC(glGetActiveUniformBlockiv)
			IMPORT_FUNC(glGetActiveUniformBlockName)
			IMPORT_FUNC(glUniformBlockBinding)
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

		// GL_ARB_half_float_vertex
		mGL_ARB_half_float_vertex = isSupported("GL_ARB_half_float_vertex");

		// GL_ARB_vertex_shader
		mGL_ARB_vertex_shader = isSupported("GL_ARB_vertex_shader");
		if (mGL_ARB_vertex_shader)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glBindAttribLocationARB)
			IMPORT_FUNC(glGetActiveAttribARB)
			IMPORT_FUNC(glGetAttribLocationARB)
			mGL_ARB_vertex_shader = result;
		}

		// GL_ARB_tessellation_shader
		mGL_ARB_tessellation_shader = isSupported("GL_ARB_tessellation_shader");
		if (mGL_ARB_tessellation_shader)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glPatchParameteri)
			IMPORT_FUNC(glPatchParameterfv)
			mGL_ARB_tessellation_shader = result;
		}

		// GL_ARB_fragment_shader
		mGL_ARB_fragment_shader = isSupported("GL_ARB_fragment_shader");

		// GL_ARB_geometry_shader4
		mGL_ARB_geometry_shader4 = isSupported("GL_ARB_geometry_shader4");
		if (mGL_ARB_geometry_shader4)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glProgramParameteriARB)
			mGL_ARB_geometry_shader4 = result;
		}

		// GL_ARB_depth_clamp
		mGL_ARB_depth_clamp = isSupported("GL_ARB_depth_clamp");

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
			IMPORT_FUNC(glDrawRangeElementsBaseVertex)
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


		//[-------------------------------------------------------]
		//[ ATI                                                   ]
		//[-------------------------------------------------------]
		// GL_ATI_meminfo
		mGL_ATI_meminfo = isSupported("GL_ATI_meminfo");

		// GL_ATI_separate_stencil
		mGL_ATI_separate_stencil = isSupported("GL_ATI_separate_stencil");
		if (mGL_ATI_separate_stencil)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glStencilOpSeparateATI)
			IMPORT_FUNC(glStencilFuncSeparateATI)
			mGL_ATI_separate_stencil = result;
		}

		// GL_ATI_draw_buffers
		mGL_ATI_draw_buffers = isSupported("GL_ATI_draw_buffers");
		if (mGL_ATI_draw_buffers)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glDrawBuffersATI)
			mGL_ATI_draw_buffers = result;
		}

		// GL_ATI_texture_compression_3dc
		mGL_ATI_texture_compression_3dc = isSupported("GL_ATI_texture_compression_3dc");


		//[-------------------------------------------------------]
		//[ AMD                                                   ]
		//[-------------------------------------------------------]
		// GL_AMD_vertex_shader_tessellator
		// -> The extension is listed as "GL_AMD_vertex_shader_tessellator" (see http://www.opengl.org/registry/specs/AMD/vertex_shader_tessellator.txt)
		// -> In "http://www.opengl.org/registry/api/glext.h" it's listed as "GL_AMD_vertex_shader_tesselator" (typo?)
		mGL_AMD_vertex_shader_tessellator = isSupported("GL_AMD_vertex_shader_tessellator");
		if (mGL_AMD_vertex_shader_tessellator)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glTessellationFactorAMD)
			IMPORT_FUNC(glTessellationModeAMD)
			mGL_AMD_vertex_shader_tessellator = result;
		}


		//[-------------------------------------------------------]
		//[ NV                                                    ]
		//[-------------------------------------------------------]
		// GL_NV_texture_rectangle
		mGL_NV_texture_rectangle = isSupported("GL_NV_texture_rectangle");

		// GL_NV_occlusion_query
		mGL_NV_occlusion_query = isSupported("GL_NV_occlusion_query");
		if (mGL_NV_occlusion_query)
		{
			// Load the entry points
			bool result = true;	// Success by default
			IMPORT_FUNC(glGenOcclusionQueriesNV)
			IMPORT_FUNC(glDeleteOcclusionQueriesNV)
			IMPORT_FUNC(glIsOcclusionQueryNV)
			IMPORT_FUNC(glBeginOcclusionQueryNV)
			IMPORT_FUNC(glEndOcclusionQueryNV)
			IMPORT_FUNC(glGetOcclusionQueryivNV)
			IMPORT_FUNC(glGetOcclusionQueryuivNV)
			mGL_NV_occlusion_query = result;
		}

		// GL_NVX_gpu_memory_info
		mGL_NVX_gpu_memory_info = isSupported("GL_NVX_gpu_memory_info");

		// GL_NV_half_float
		mGL_NV_half_float = isSupported("GL_NV_half_float");

		// GL_NV_texture_compression_vtc
		mGL_NV_texture_compression_vtc = isSupported("GL_NV_texture_compression_vtc");


		//[-------------------------------------------------------]
		//[ SGIS                                                  ]
		//[-------------------------------------------------------]
		// GL_SGIS_generate_mipmap
		mGL_SGIS_generate_mipmap = isSupported("GL_SGIS_generate_mipmap");


		//[-------------------------------------------------------]
		//[ HP                                                    ]
		//[-------------------------------------------------------]
		// GL_HP_occlusion_test
		mGL_HP_occlusion_test = isSupported("GL_HP_occlusion_test");


		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
