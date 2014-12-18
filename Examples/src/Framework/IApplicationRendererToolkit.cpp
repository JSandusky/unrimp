/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_TOOLKIT


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/IApplicationRendererToolkit.h"
#ifdef SHARED_LIBRARIES
	// Dynamically linked libraries
	#ifdef WIN32
		#include "Framework/WindowsHeader.h"

	#elif defined LINUX
		#include "Framework/LinuxHeader.h"

		#include <dlfcn.h>
	#else
		#error "Unsupported platform"
	#endif

	#include <stdio.h>
#endif

#include <string.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
IApplicationRendererToolkit::~IApplicationRendererToolkit()
{
	// Nothing to do in here
	// m_pRendererToolkit is destroyed within onDeinitialization()
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void IApplicationRendererToolkit::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Is there a valid renderer instance?
	Renderer::IRenderer *renderer = getRenderer();
	if (nullptr != renderer)
	{
		// Create the renderer toolkit instance
		mRendererToolkit = createRendererToolkitInstance(*renderer);
	}
}

void IApplicationRendererToolkit::onDeinitialization()
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


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
IApplicationRendererToolkit::IApplicationRendererToolkit(const char *rendererName) :
	IApplicationRenderer(rendererName),
	mRendererToolkitSharedLibrary(nullptr)
{
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
RendererToolkit::IRendererToolkit *IApplicationRendererToolkit::createRendererToolkitInstance(Renderer::IRenderer &renderer)
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
				void *symbol = ::GetProcAddress(static_cast<HMODULE>(mRendererToolkitSharedLibrary), "createRendererToolkitInstance");
				if (nullptr != symbol)
				{
					// "createRendererToolkitInstance()" signature
					typedef RendererToolkit::IRendererToolkit *(__cdecl *createRendererToolkitInstance)(Renderer::IRenderer &renderer);

					// Create the renderer toolkit instance
					return static_cast<createRendererToolkitInstance>(symbol)(renderer);
				}
				else
				{
					// Error!
					OUTPUT_DEBUG_PRINTF("Failed to locate the entry point \"createRendererToolkitInstance\" within the renderer toolkit shared library \"%s\"", RENDERER_TOOLKIT_FILENAME)
				}
			}
			else
			{
				OUTPUT_DEBUG_PRINTF("Failed to load in the shared library \"%s\"\n", RENDERER_TOOLKIT_FILENAME)
			}
		#elif defined LINUX
			// Load in the shared library
			#ifdef _DEBUG
				static const char RENDERER_TOOLKIT_FILENAME[] = "RendererToolkitD.so";
			#else
				static const char RENDERER_TOOLKIT_FILENAME[] = "RendererToolkit.so";
			#endif
			mRendererToolkitSharedLibrary = dlopen(RENDERER_TOOLKIT_FILENAME, RTLD_NOW);
			if (nullptr != mRendererToolkitSharedLibrary)
			{
				// Get the "createRendererToolkitInstance()" function pointer
				void *symbol = dlsym(mRendererToolkitSharedLibrary, "createRendererToolkitInstance");
				if (nullptr != symbol)
				{
					// "createRendererToolkitInstance()" signature
					typedef RendererToolkit::IRendererToolkit *(*createRendererToolkitInstance)(Renderer::IRenderer &renderer);

					// Create the renderer toolkit instance
					return reinterpret_cast<createRendererToolkitInstance>(symbol)(renderer);
				}
				else
				{
					// Error!
					OUTPUT_DEBUG_PRINTF("Failed to locate the entry point \"createRendererToolkitInstance\" within the renderer toolkit shared library \"%s\"", RENDERER_TOOLKIT_FILENAME)
				}
			}
			else
			{
				OUTPUT_DEBUG_PRINTF("Failed to load in the shared library \"%s\"\n", RENDERER_TOOLKIT_FILENAME)
			}
		#else
			#error "Unsupported platform"
		#endif

		// Error!
		return nullptr;
	#else
		// Statically linked libraries

		// "createRendererToolkitInstance()" signature
		extern RendererToolkit::IRendererToolkit *createRendererToolkitInstance(Renderer::IRenderer &renderer);

		// Create the renderer toolkit instance
		return createRendererToolkitInstance(renderer);
	#endif
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // RENDERER_NO_TOOLKIT
