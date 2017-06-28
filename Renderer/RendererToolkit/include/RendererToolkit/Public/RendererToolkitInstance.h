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
#include <RendererToolkit/Public/RendererToolkit.h>

#ifdef SHARED_LIBRARIES
	// Dynamically linked libraries
	#ifdef WIN32
		#include "RendererToolkit/WindowsHeader.h"
	#elif defined LINUX
		#include <dlfcn.h>
	#else
		#error "Unsupported platform"
	#endif

	#include <stdio.h>
#endif

#include <string.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IFileManager;
}
namespace RendererToolkit
{
	class IRendererToolkit;
}


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
#ifndef SHARED_LIBRARIES
	// Statically linked library create renderer runtime instance signatures
	// This is needed to do here because the methods in the library are also defined in global namespace

	// "createRendererToolkitInstance()" signature
	extern RendererToolkit::IRendererToolkit* createRendererToolkitInstance(RendererRuntime::IFileManager& fileManager);
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Renderer toolkit instance using runtime linking
	*/
	class RendererToolkitInstance
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] fileManager
		*    The file manager instance to use
		*/
		RendererToolkitInstance(RendererRuntime::IFileManager& fileManager)
		{
			#ifdef SHARED_LIBRARIES
				// Dynamically linked libraries
				#ifdef WIN32
					// Load in the dll
					#ifdef _DEBUG
						static const char RENDERER_TOOLKIT_FILENAME[] = "RendererToolkitD.dll";
					#else
						static const char RENDERER_TOOLKIT_FILENAME[] = "RendererToolkit.dll";
					#endif
					mRendererToolkitSharedLibrary = ::LoadLibraryExA(RENDERER_TOOLKIT_FILENAME, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
					if (nullptr != mRendererToolkitSharedLibrary)
					{
						// Get the "createRendererToolkitInstance()" function pointer
						void* symbol = ::GetProcAddress(static_cast<HMODULE>(mRendererToolkitSharedLibrary), "createRendererToolkitInstance");
						if (nullptr != symbol)
						{
							// "createRendererToolkitInstance()" signature
							typedef RendererToolkit::IRendererToolkit* (__cdecl *createRendererToolkitInstance)(RendererRuntime::IFileManager& fileManager);

							// Create the renderer toolkit instance
							mRendererToolkit = static_cast<createRendererToolkitInstance>(symbol)(fileManager);
						}
						else
						{
							// Error!
							// TODO(co) Error handling
						//	OUTPUT_DEBUG_PRINTF("Failed to locate the entry point \"createRendererToolkitInstance\" within the renderer toolkit shared library \"%s\"", RENDERER_TOOLKIT_FILENAME)
						}
					}
					else
					{
						// TODO(co) Error handling
					//	OUTPUT_DEBUG_PRINTF("Failed to load in the shared library \"%s\"\n", RENDERER_TOOLKIT_FILENAME)
					}
				#elif defined LINUX
					// Load in the shared library
					#ifdef _DEBUG
						static const char RENDERER_TOOLKIT_FILENAME[] = "libRendererToolkitD.so";
					#else
						static const char RENDERER_TOOLKIT_FILENAME[] = "libRendererToolkit.so";
					#endif
					mRendererToolkitSharedLibrary = dlopen(RENDERER_TOOLKIT_FILENAME, RTLD_NOW);
					if (nullptr != mRendererToolkitSharedLibrary)
					{
						// Get the "createRendererToolkitInstance()" function pointer
						void* symbol = dlsym(mRendererToolkitSharedLibrary, "createRendererToolkitInstance");
						if (nullptr != symbol)
						{
							// "createRendererToolkitInstance()" signature
							typedef RendererToolkit::IRendererToolkit* (*createRendererToolkitInstance)(RendererRuntime::IFileManager& fileManager);

							// Create the renderer toolkit instance
							mRendererToolkit = reinterpret_cast<createRendererToolkitInstance>(symbol)(fileManager);
						}
						else
						{
							// Error!
							// TODO(co) Error handling
						//	OUTPUT_DEBUG_PRINTF("Failed to locate the entry point \"createRendererToolkitInstance\" within the renderer toolkit shared library \"%s\"", RENDERER_TOOLKIT_FILENAME)
						}
					}
					else
					{
						// TODO(co) Error handling
						// OUTPUT_DEBUG_PRINTF("Failed to load in the shared library \"%s\"\n", RENDERER_TOOLKIT_FILENAME)
					}
				#else
					#error "Unsupported platform"
				#endif
			#else
				// Statically linked libraries

				// Create the renderer toolkit instance
				mRendererToolkit = createRendererToolkitInstance(fileManager);
			#endif
		}

		/**
		*  @brief
		*    Destructor
		*/
		~RendererToolkitInstance()
		{
			// Delete the renderer toolkit instance
			mRendererToolkit = nullptr;

			// Destroy the shared library instance
			#ifdef SHARED_LIBRARIES
				#ifdef WIN32
					if (nullptr != mRendererToolkitSharedLibrary)
					{
						::FreeLibrary(static_cast<HMODULE>(mRendererToolkitSharedLibrary));
						mRendererToolkitSharedLibrary = nullptr;
					}
				#elif defined LINUX
					if (nullptr != mRendererToolkitSharedLibrary)
					{
						::dlclose(mRendererToolkitSharedLibrary);
						mRendererToolkitSharedLibrary = nullptr;
					}
				#else
					#error "Unsupported platform"
				#endif
			#endif
		}

		/**
		*  @brief
		*    Return the renderer toolkit instance
		*
		*  @remarks
		*    The renderer toolkit instance, can be a null pointer
		*/
		inline RendererToolkit::IRendererToolkit* getRendererToolkit() const
		{
			return mRendererToolkit;
		}


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		void*								 mRendererToolkitSharedLibrary;	///< Renderer toolkit shared library, can be a null pointer
		RendererToolkit::IRendererToolkitPtr mRendererToolkit;				///< Renderer toolkit instance, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
