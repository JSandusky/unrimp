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
#define OPENVR_DEFINERUNTIMELINKING

#include "RendererRuntime/Vr/Detail/OpenVRRuntimeLinking.h"

#include <RendererRuntime/Core/Platform/PlatformTypes.h>	// For "RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF()"
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
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Macros                                                ]
	//[-------------------------------------------------------]
	// Define a helper macro
	#ifdef WIN32
		#define IMPORT_FUNC(funcName)																																				\
			if (result)																																								\
			{																																										\
				void *symbol = ::GetProcAddress(static_cast<HMODULE>(mOpenVRSharedLibrary), #funcName);																				\
				if (nullptr != symbol)																																				\
				{																																									\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																												\
				}																																									\
				else																																								\
				{																																									\
					wchar_t moduleFilename[MAX_PATH];																																\
					moduleFilename[0] = '\0';																																		\
					::GetModuleFileNameW(static_cast<HMODULE>(mOpenVRSharedLibrary), moduleFilename, MAX_PATH);																		\
					RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF("OpenVR error: Failed to locate the entry point \"%s\" within the OpenVR shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																					\
				}																																									\
			}
	#elif defined LINUX
		#define IMPORT_FUNC(funcName)																																				\
			if (result)																																								\
			{																																										\
				void *symbol = ::dlsym(mOpenVRSharedLibrary, #funcName);																											\
				if (nullptr != symbol)																																				\
				{																																									\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																												\
				}																																									\
				else																																								\
				{																																									\
					link_map *linkMap = nullptr;																																	\
					const char* libraryName = "unknown";																															\
					if (dlinfo(mOpenVRSharedLibrary, RTLD_DI_LINKMAP, &linkMap))																									\
					{																																								\
						libraryName = linkMap->l_name;																																\
					}																																								\
					std::cout << "OpenVR error: Failed to locate the entry point \"" << #funcName << "\" within the OpenVR shared library \"" << libraryName << "\"\n";		/* TODO(co) Use "RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF" instead*/ \
					result = false;																																					\
				}																																									\
			}
	#else
		#error "Unsupported platform"
	#endif


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	OpenVRRuntimeLinking::OpenVRRuntimeLinking() :
		mOpenVRSharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing here
	}

	OpenVRRuntimeLinking::~OpenVRRuntimeLinking()
	{
		// Destroy the shared library instances
		#ifdef WIN32
			if (nullptr != mOpenVRSharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mOpenVRSharedLibrary));
			}
		#elif defined LINUX
			if (nullptr != mOpenVRSharedLibrary)
			{
				::dlclose(mOpenVRSharedLibrary);
			}
		#else
			#error "Unsupported platform"
		#endif
	}

	bool OpenVRRuntimeLinking::isOpenVRAvaiable()
	{
		// Already initialized?
		if (!mInitialized)
		{
			// We're now initialized
			mInitialized = true;

			// Load the shared libraries
			if (loadSharedLibraries())
			{
				// Load the OpenVR entry points
				mEntryPointsRegistered = loadOpenVREntryPoints();
			}
		}

		// Entry points successfully registered?
		return mEntryPointsRegistered;
	}

	bool OpenVRRuntimeLinking::loadSharedLibraries()
	{
		// Load the shared library
		#ifdef WIN32
			mOpenVRSharedLibrary = ::LoadLibraryExA("openvr_api.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr == mOpenVRSharedLibrary)
			{
				RENDERERRUNTIME_OUTPUT_DEBUG_STRING("OpenVR error: Failed to load in the shared library \"openvr_api.dll\"\n")
			}
		#elif defined LINUX
			mOpenVRSharedLibrary = ::dlopen("libopenvr_api.so", RTLD_NOW);
			if (nullptr == mOpenVRSharedLibrary)
			{
				std::cout<<"OpenVR error: Failed to load in the shared library \"libopenvr_api.so\"\n";	// TODO(co) Use "RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF" instead
			}
		#else
			#error "Unsupported platform"
		#endif

		// Done
		return (nullptr != mOpenVRSharedLibrary);
	}

	bool OpenVRRuntimeLinking::loadOpenVREntryPoints()
	{
		bool result = true;	// Success by default

		// Load the entry points
		using namespace vr;
		IMPORT_FUNC(VR_IsHmdPresent);
		IMPORT_FUNC(VR_IsRuntimeInstalled);
		IMPORT_FUNC(VR_RuntimePath);
		IMPORT_FUNC(VR_GetVRInitErrorAsSymbol);
		IMPORT_FUNC(VR_GetVRInitErrorAsEnglishDescription);
		IMPORT_FUNC(VR_GetGenericInterface);
		IMPORT_FUNC(VR_IsInterfaceVersionValid);
		IMPORT_FUNC(VR_GetInitToken);
		IMPORT_FUNC(VR_InitInternal);
		IMPORT_FUNC(VR_ShutdownInternal);

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
