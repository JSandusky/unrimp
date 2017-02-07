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
//[ Includes                                              ]
//[-------------------------------------------------------]
#define OPENGL_DEFINERUNTIMELINKING

#include "OpenGLRenderer/OpenGLRuntimeLinking.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_PRINTF()"
#ifdef WIN32
	#include <Renderer/WindowsHeader.h>
#elif defined LINUX
	#include <Renderer/LinuxHeader.h>

	#include <dlfcn.h>
	#include <link.h>
	#include <iostream>
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Macros                                                ]
	//[-------------------------------------------------------]
	// Define a helper macro
	#ifdef WIN32
		#define IMPORT_FUNC(funcName)																																			\
			if (result)																																							\
			{																																									\
				void *symbol = ::GetProcAddress(static_cast<HMODULE>(mOpenGLSharedLibrary), #funcName);																			\
				if (nullptr == symbol)																																			\
				{																																								\
					symbol = wglGetProcAddress(#funcName);																														\
				}																																								\
				if (nullptr != symbol)																																			\
				{																																								\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																											\
				}																																								\
				else																																							\
				{																																								\
					wchar_t moduleFilename[MAX_PATH];																															\
					moduleFilename[0] = '\0';																																	\
					::GetModuleFileNameW(static_cast<HMODULE>(mOpenGLSharedLibrary), moduleFilename, MAX_PATH);																	\
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: Failed to locate the entry point \"%s\" within the OpenGL shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																				\
				}																																								\
			}
	#elif defined LINUX
		#define IMPORT_FUNC(funcName)																																			\
			if (result)																																							\
			{																																									\
				void *symbol = ::dlsym(mOpenGLSharedLibrary, #funcName);																										\
				if (nullptr != symbol)																																			\
				{																																								\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																											\
				}																																								\
				else																																							\
				{																																								\
					link_map *linkMap = nullptr;																																\
					const char* libraryName = "unknown";																														\
					if (dlinfo(mOpenGLSharedLibrary, RTLD_DI_LINKMAP, &linkMap))																								\
					{																																							\
						libraryName = linkMap->l_name;																															\
					}																																							\
					std::cout << "OpenGL error: Failed to locate the entry point \"" << #funcName << "\" within the OpenGL shared library \"" << libraryName << "\"\n";		/* TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead*/ \
					result = false;																																				\
				}																																								\
			}
	#else
		#error "Unsupported platform"
	#endif


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	OpenGLRuntimeLinking::OpenGLRuntimeLinking() :
		mOpenGLSharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing here
	}

	OpenGLRuntimeLinking::~OpenGLRuntimeLinking()
	{
		// Destroy the shared library instances
		#ifdef WIN32
			if (nullptr != mOpenGLSharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mOpenGLSharedLibrary));
			}
		#elif defined LINUX
			if (nullptr != mOpenGLSharedLibrary)
			{
				::dlclose(mOpenGLSharedLibrary);
			}
		#else
			#error "Unsupported platform"
		#endif
	}

	bool OpenGLRuntimeLinking::isOpenGLAvaiable()
	{
		// Already initialized?
		if (!mInitialized)
		{
			// We're now initialized
			mInitialized = true;

			// Load the shared libraries
			if (loadSharedLibraries())
			{
				// Load the OpenGL entry points
				mEntryPointsRegistered = loadOpenGLEntryPoints();
			}
		}

		// Entry points successfully registered?
		return mEntryPointsRegistered;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool OpenGLRuntimeLinking::loadSharedLibraries()
	{
		// Load the shared library
		#ifdef WIN32
			mOpenGLSharedLibrary = ::LoadLibraryExA("opengl32.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr == mOpenGLSharedLibrary)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Failed to load in the shared library \"opengl32.dll\"\n")
			}
		#elif defined LINUX
			mOpenGLSharedLibrary = ::dlopen("libGL.so", RTLD_NOW);
			if (nullptr == mOpenGLSharedLibrary)
			{
				std::cout << "OpenGL error: Failed to load in the shared library \"libGL.so\"\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
			}
		#else
			#error "Unsupported platform"
		#endif

		// Done
		return (nullptr != mOpenGLSharedLibrary);
	}

	bool OpenGLRuntimeLinking::loadOpenGLEntryPoints()
	{
		bool result = true;	// Success by default

		// Load the entry points
		IMPORT_FUNC(glGetString);
		IMPORT_FUNC(glGetIntegerv);
		IMPORT_FUNC(glBindTexture);
		IMPORT_FUNC(glClear);
		IMPORT_FUNC(glClearStencil);
		IMPORT_FUNC(glClearDepth);
		IMPORT_FUNC(glClearColor);
		IMPORT_FUNC(glDrawArrays);
		IMPORT_FUNC(glDrawElements);
		IMPORT_FUNC(glColor4f);
		IMPORT_FUNC(glEnable);
		IMPORT_FUNC(glDisable);
		IMPORT_FUNC(glBlendFunc);
		IMPORT_FUNC(glFrontFace);
		IMPORT_FUNC(glCullFace);
		IMPORT_FUNC(glPolygonMode);
		IMPORT_FUNC(glTexParameteri);
		IMPORT_FUNC(glGenTextures);
		IMPORT_FUNC(glDeleteTextures);
		IMPORT_FUNC(glTexImage1D);
		IMPORT_FUNC(glTexImage2D);
		IMPORT_FUNC(glPixelStorei);
		IMPORT_FUNC(glDepthMask);
		IMPORT_FUNC(glViewport);
		IMPORT_FUNC(glDepthRange);
		IMPORT_FUNC(glScissor);
		IMPORT_FUNC(glFlush);
		IMPORT_FUNC(glFinish);
		#ifdef WIN32
			IMPORT_FUNC(wglGetCurrentDC);
			IMPORT_FUNC(wglGetProcAddress);
			IMPORT_FUNC(wglCreateContext);
			IMPORT_FUNC(wglDeleteContext);
			IMPORT_FUNC(wglMakeCurrent);
		#elif LINUX
			IMPORT_FUNC(glXMakeCurrent);
			IMPORT_FUNC(glXGetProcAddress);
			IMPORT_FUNC(glXGetProcAddressARB);
			IMPORT_FUNC(glXChooseVisual);
			IMPORT_FUNC(glXCreateContext);
			IMPORT_FUNC(glXDestroyContext);
			IMPORT_FUNC(glXGetCurrentContext);
			IMPORT_FUNC(glXQueryExtensionsString);
			IMPORT_FUNC(glXChooseFBConfig);
			IMPORT_FUNC(glXSwapBuffers);
			IMPORT_FUNC(glXGetClientString);
		#endif

		// Done
		return result;
	}

	bool OpenGLRuntimeLinking::loadOpenGL3EntryPoints()
	{
		bool result = true;	// Success by default

		// Load the entry points
		IMPORT_FUNC(glGetStringi);

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
