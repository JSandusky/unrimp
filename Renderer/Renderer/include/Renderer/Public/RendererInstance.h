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
#include <Renderer/Public/Renderer.h>

#ifdef SHARED_LIBRARIES
	// Dynamically linked libraries
	#ifdef WIN32
		#include "Renderer/WindowsHeader.h"

	#elif defined LINUX
		#include <dlfcn.h>
		#include <iostream>	// For "std::cerr"
	#else
		#error "Unsupported platform"
	#endif

	#include <stdio.h>
#endif

#include <string.h>

// Statically linked libraries create renderer instance signatures
// This is needed to do here because the methods in the libaries are also defined in global namespace
#ifndef SHARED_LIBRARIES
	// Null
	#ifndef RENDERER_NO_NULL
		// "createNullRendererInstance()" signature
		extern Renderer::IRenderer *createNullRendererInstance(Renderer::handle);
	#endif

	// OpenGL
	#ifndef RENDERER_NO_OPENGL
		// "createOpenGLRendererInstance()" signature
		extern Renderer::IRenderer *createOpenGLRendererInstance(Renderer::handle);
	#endif

	// OpenGLES3
	#ifndef RENDERER_NO_OPENGLES3
		// "createOpenGLES3RendererInstance()" signature
		extern Renderer::IRenderer *createOpenGLES3RendererInstance(Renderer::handle);
	#endif

	// Direct3D 9
	#ifndef RENDERER_NO_DIRECT3D9
		// "createDirect3D9RendererInstance()" signature
		extern Renderer::IRenderer *createDirect3D9RendererInstance(Renderer::handle);
	#endif

	// Direct3D 10
	#ifndef RENDERER_NO_DIRECT3D10
		// "createDirect3D10RendererInstance()" signature
		extern Renderer::IRenderer *createDirect3D10RendererInstance(Renderer::handle);
	#endif

	// Direct3D 11
	#ifndef RENDERER_NO_DIRECT3D11
		// "createDirect3D11RendererInstance()" signature
		extern Renderer::IRenderer *createDirect3D11RendererInstance(Renderer::handle);
	#endif
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Renderer instance using runtime linking
	*/
	class RendererInstance
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] rendererName
		*    Case sensitive ASCII name of the renderer to instance, must be valid.
		*    Example renderer names: "Null", "OpenGL", "OpenGLES3", "Direct3D9", "Direct3D10", "Direct3D11", "Direct3D12", "Vulkan"
		*  @param[in] rendererName
		*    Native window handle TODO(co) A renderer instance fixed connected to a native window handle? This can't be right. If it's about OpenGL context sharing we surely can find a better solution.
		*/
		explicit RendererInstance(const char* rendererName, handle nativeWindowHandle)
		{
			// In order to keep it simple in this test project the supported renderer backends are
			// fixed typed in. For a real system a dynamic plugin system would be a good idea.
			#ifdef SHARED_LIBRARIES
				// Dynamically linked libraries
				#ifdef WIN32
					// Load in the dll
					char rendererFilename[128];
					#ifdef _DEBUG
						sprintf(rendererFilename, "%sRendererD.dll", rendererName);
					#else
						sprintf(rendererFilename, "%sRenderer.dll", rendererName);
					#endif
					mRendererSharedLibrary = ::LoadLibraryExA(rendererFilename, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
					if (nullptr != mRendererSharedLibrary)
					{
						// Get the "CreateRendererInstance()" function pointer
						char functionName[128];
						sprintf(functionName, "create%sRendererInstance", rendererName);
						void *symbol = ::GetProcAddress(static_cast<HMODULE>(mRendererSharedLibrary), functionName);
						if (nullptr != symbol)
						{
							// "createRendererInstance()" signature
							typedef Renderer::IRenderer *(__cdecl *createRendererInstance)(Renderer::handle);

							// Create the renderer instance
							mRenderer = (static_cast<createRendererInstance>(symbol))(nativeWindowHandle);
						}
						else
						{
							// Error!
							OUTPUT_DEBUG_PRINTF("Failed to locate the entry point \"%s\" within the renderer shared library \"%s\"", functionName, rendererFilename)
						}
					}
					else
					{
						OUTPUT_DEBUG_PRINTF("Failed to load in the shared library \"%s\"\n", rendererFilename)
					}
				#elif defined LINUX
					// Load in the shared library
					char rendererFilename[128];
					#ifdef _DEBUG
						sprintf(rendererFilename, "%sRendererD.so", rendererName);
					#else
						sprintf(rendererFilename, "lib%sRenderer.so", rendererName);
					#endif
					mRendererSharedLibrary = ::dlopen(rendererFilename, RTLD_NOW);
					if (nullptr != mRendererSharedLibrary)
					{
						// Get the "CreateRendererInstance()" function pointer
						char functionName[128];
						sprintf(functionName, "create%sRendererInstance", rendererName);
						void *symbol = ::dlsym(mRendererSharedLibrary, functionName);
						if (nullptr != symbol)
						{
							// "createRendererInstance()" signature
							typedef Renderer::IRenderer *(*createRendererInstance)(Renderer::handle);

							// Create the renderer instance
							mRenderer = (reinterpret_cast<createRendererInstance>(symbol))(nativeWindowHandle);
						}
						else
						{
							// Error!
							std::cerr << "Failed to locate the entry point \"" << functionName << "\" within the renderer shared library \"" << rendererFilename << "\"\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead... as seen below, why the additional output?
							OUTPUT_DEBUG_PRINTF("Failed to locate the entry point \"%s\" within the renderer shared library \"%s\"", functionName, rendererFilename)
						}
					}
					else
					{
						std::cerr << "Failed to load in the shared library \"" << rendererFilename << "\"\nReason:" << dlerror() << "\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead... as seen below, why the additional output?
						OUTPUT_DEBUG_PRINTF("Failed to load in the shared library \"%s\"\n", rendererFilename)
					}
				#else
					#error "Unsupported platform"
				#endif
			#else
				// Statically linked libraries

				// Null
				#ifndef RENDERER_NO_NULL
					if (0 == strcmp(rendererName, "Null"))
					{
						// Create the renderer instance
						mRenderer = createNullRendererInstance(nativeWindowHandle);
					}
				#endif

				// OpenGL
				#ifndef RENDERER_NO_OPENGL
					if (0 == strcmp(rendererName, "OpenGL"))
					{
						// Create the renderer instance
						mRenderer = createOpenGLRendererInstance(nativeWindowHandle);
					}
				#endif

				// OpenGLES3
				#ifndef RENDERER_NO_OPENGLES3
					if (0 == strcmp(rendererName, "OpenGLES3"))
					{
						// Create the renderer instance
						mRenderer = createOpenGLES3RendererInstance(nativeWindowHandle);
					}
				#endif

				// Direct3D 9
				#ifndef RENDERER_NO_DIRECT3D9
					if (0 == strcmp(rendererName, "Direct3D9"))
					{
						// Create the renderer instance
						mRenderer = createDirect3D9RendererInstance(nativeWindowHandle);
					}
				#endif

				// Direct3D 10
				#ifndef RENDERER_NO_DIRECT3D10
					if (0 == strcmp(rendererName, "Direct3D10"))
					{
						// Create the renderer instance
						mRenderer = createDirect3D10RendererInstance(nativeWindowHandle);
					}
				#endif

				// Direct3D 11
				#ifndef RENDERER_NO_DIRECT3D11
					if (0 == strcmp(rendererName, "Direct3D11"))
					{
						// Create the renderer instance
						mRenderer = createDirect3D11RendererInstance(nativeWindowHandle);
					}
				#endif
			#endif
		}

		/**
		*  @brief
		*    Destructor
		*/
		~RendererInstance()
		{
			// Delete the renderer instance
			mRenderer = nullptr;

			// Destroy the shared library instance
			#ifdef SHARED_LIBRARIES
				#ifdef WIN32
					if (nullptr != mRendererSharedLibrary)
					{
						::FreeLibrary(static_cast<HMODULE>(mRendererSharedLibrary));
						mRendererSharedLibrary = nullptr;
					}
				#elif defined LINUX
					if (nullptr != mRendererSharedLibrary)
					{
						::dlclose(mRendererSharedLibrary);
						mRendererSharedLibrary = nullptr;
					}
				#else
					#error "Unsupported platform"
				#endif
			#endif
		}

		/**
		*  @brief
		*    Return the renderer instance
		*
		*  @remarks
		*    The renderer instance, can be a null pointer
		*/
		inline Renderer::IRenderer *getRenderer() const
		{
			return mRenderer;
		}


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		void				   *mRendererSharedLibrary;	///< Renderer shared library, can be a null pointer
		Renderer::IRendererPtr  mRenderer;				///< Renderer instance, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
