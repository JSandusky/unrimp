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
#include <RendererRuntime/Public/RendererRuntime.h>

#ifdef SHARED_LIBRARIES
	// Dynamically linked libraries
	#ifdef WIN32
		#include "RendererRuntime/Core/Platform/WindowsHeader.h"

	#elif defined LINUX
		#include <dlfcn.h>
	#else
		#error "Unsupported platform"
	#endif

	#include <stdio.h>
#endif

#include <string.h>


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
#ifndef SHARED_LIBRARIES
	// Statically linked library create renderer runtime instance signatures
	// This is needed to do here because the methods in the library are also defined in global namespace

	// "createRendererRuntimeInstance()" signature
	extern RendererRuntime::IRendererRuntime *createRendererRuntimeInstance(Renderer::IRenderer &renderer, RendererRuntime::IFileManager& fileManager);
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Renderer runtime instance using runtime linking
	*/
	class RendererRuntimeInstance
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Renderer instance to use
		*/
		RendererRuntimeInstance(Renderer::IRenderer& renderer, RendererRuntime::IFileManager& fileManager)
		{
			#ifdef SHARED_LIBRARIES
				// Dynamically linked libraries
				#ifdef WIN32
					// Load in the dll
					#ifdef _DEBUG
						static const char RENDERER_RUNTIME_FILENAME[] = "RendererRuntimeD.dll";
					#else
						static const char RENDERER_RUNTIME_FILENAME[] = "RendererRuntime.dll";
					#endif
					mRendererRuntimeSharedLibrary = ::LoadLibraryExA(RENDERER_RUNTIME_FILENAME, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
					if (nullptr != mRendererRuntimeSharedLibrary)
					{
						// Get the "createRendererRuntimeInstance()" function pointer
						void *symbol = ::GetProcAddress(static_cast<HMODULE>(mRendererRuntimeSharedLibrary), "createRendererRuntimeInstance");
						if (nullptr != symbol)
						{
							// "createRendererRuntimeInstance()" signature
							typedef RendererRuntime::IRendererRuntime *(__cdecl *createRendererRuntimeInstance)(Renderer::IRenderer &renderer, RendererRuntime::IFileManager& fileManager);

							// Create the renderer runtime instance
							mRendererRuntime = static_cast<createRendererRuntimeInstance>(symbol)(renderer, fileManager);
						}
						else
						{
							// Error!
							// TODO(co) Error handling
						//	OUTPUT_DEBUG_PRINTF("Failed to locate the entry point \"createRendererRuntimeInstance\" within the renderer runtime shared library \"%s\"", RENDERER_RUNTIME_FILENAME)
						}
					}
					else
					{
						// TODO(co) Error handling
					//	OUTPUT_DEBUG_PRINTF("Failed to load in the shared library \"%s\"\n", RENDERER_RUNTIME_FILENAME)
					}
				#elif defined LINUX
					// Load in the shared library
					#ifdef _DEBUG
						static const char RENDERER_RUNTIME_FILENAME[] = "libRendererRuntimeD.so";
					#else
						static const char RENDERER_RUNTIME_FILENAME[] = "libRendererRuntime.so";
					#endif
					mRendererRuntimeSharedLibrary = dlopen(RENDERER_RUNTIME_FILENAME, RTLD_NOW);
					if (nullptr != mRendererRuntimeSharedLibrary)
					{
						// Get the "createRendererRuntimeInstance()" function pointer
						void *symbol = dlsym(mRendererRuntimeSharedLibrary, "createRendererRuntimeInstance");
						if (nullptr != symbol)
						{
							// "createRendererRuntimeInstance()" signature
							typedef RendererRuntime::IRendererRuntime *(*createRendererRuntimeInstance)(Renderer::IRenderer &renderer, RendererRuntime::IFileManager& fileManager);

							// Create the renderer runtime instance
							mRendererRuntime = reinterpret_cast<createRendererRuntimeInstance>(symbol)(renderer, fileManager);
						}
						else
						{
							// Error!
							// TODO(co) Error handling
						//	OUTPUT_DEBUG_PRINTF("Failed to locate the entry point \"createRendererRuntimeInstance\" within the renderer runtime shared library \"%s\"", RENDERER_RUNTIME_FILENAME)
						}
					}
					else
					{
						// TODO(co) Error handling
						// OUTPUT_DEBUG_PRINTF("Failed to load in the shared library \"%s\"\n", RENDERER_RUNTIME_FILENAME)
					}
				#else
					#error "Unsupported platform"
				#endif
			#else
				// Statically linked libraries

				// Create the renderer runtime instance
				mRendererRuntime = createRendererRuntimeInstance(renderer, fileManager);
			#endif
		}

		/**
		*  @brief
		*    Destructor
		*/
		~RendererRuntimeInstance()
		{
			// Delete the renderer runtime instance
			mRendererRuntime = nullptr;

			// Destroy the shared library instance
			#ifdef SHARED_LIBRARIES
				#ifdef WIN32
					if (nullptr != mRendererRuntimeSharedLibrary)
					{
						::FreeLibrary(static_cast<HMODULE>(mRendererRuntimeSharedLibrary));
						mRendererRuntimeSharedLibrary = nullptr;
					}
				#elif defined LINUX
					if (nullptr != mRendererRuntimeSharedLibrary)
					{
						::dlclose(mRendererRuntimeSharedLibrary);
						mRendererRuntimeSharedLibrary = nullptr;
					}
				#else
					#error "Unsupported platform"
				#endif
			#endif
		}

		/**
		*  @brief
		*    Return the renderer runtime instance
		*
		*  @remarks
		*    The renderer runtime instance, can be a null pointer
		*/
		inline RendererRuntime::IRendererRuntime *getRendererRuntime() const
		{
			return mRendererRuntime;
		}


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		void								 *mRendererRuntimeSharedLibrary;	///< Renderer runtime shared library, can be a null pointer
		RendererRuntime::IRendererRuntimePtr  mRendererRuntime;					///< Renderer runtime instance, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
