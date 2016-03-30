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
#define VULKAN_DEFINERUNTIMELINKING

#include "VulkanRenderer/VulkanRuntimeLinking.h"

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
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VulkanRuntimeLinking::VulkanRuntimeLinking() :
		mVulkanSharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing to do in here
	}

	VulkanRuntimeLinking::~VulkanRuntimeLinking()
	{
		// Destroy the shared library instances
		#ifdef WIN32
			if (nullptr != mVulkanSharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mVulkanSharedLibrary));
			}
		#elif defined LINUX
			if (nullptr != mVulkanSharedLibrary)
			{
				::dlclose(mVulkanSharedLibrary);
			}
		#else
			#error "Unsupported platform"
		#endif
	}

	bool VulkanRuntimeLinking::isVulkanAvaiable()
	{
		// Already initialized?
		if (!mInitialized)
		{
			// We're now initialized
			mInitialized = true;

			// Load the shared libraries
			if (loadSharedLibraries())
			{
				// Load the Vulkan entry points
				mEntryPointsRegistered = loadVulkanEntryPoints();
			}
		}

		// Entry points successfully registered?
		return mEntryPointsRegistered;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool VulkanRuntimeLinking::loadSharedLibraries()
	{
		// Load the shared library
		#ifdef WIN32
			// TODO(co) Implement me
			// mVulkanSharedLibrary = ::LoadLibraryExA("TODO.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr == mVulkanSharedLibrary)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Vulkan error: Failed to load in the shared library \"TODO.dll\"\n")
			}
		#elif defined LINUX
			// TODO(co) Implement me
			// mVulkanSharedLibrary = ::dlopen("TODO.so", RTLD_NOW);
			if (nullptr == mVulkanSharedLibrary)
			{
				std::cout<<"Vulkan error: Failed to load in the shared library \"TODO.so\"\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
			}
		#else
			#error "Unsupported platform"
		#endif

		// Done
		return (nullptr != mVulkanSharedLibrary);
	}

	bool VulkanRuntimeLinking::loadVulkanEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																																			\
				if (result)																																							\
				{																																									\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mVulkanSharedLibrary), #funcName);																			\
					if (nullptr != symbol)																																			\
					{																																								\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																											\
					}																																								\
					else																																							\
					{																																								\
						wchar_t moduleFilename[MAX_PATH];																															\
						moduleFilename[0] = '\0';																																	\
						::GetModuleFileNameW(static_cast<HMODULE>(mVulkanSharedLibrary), moduleFilename, MAX_PATH);																	\
						RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: Failed to locate the entry point \"%s\" within the Vulkan shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																				\
					}																																								\
				}
		#elif defined LINUX
			#define IMPORT_FUNC(funcName)																																			\
				if (result)																																							\
				{																																									\
					void *symbol = ::dlsym(mVulkanSharedLibrary, #funcName);																										\
					if (nullptr != symbol)																																			\
					{																																								\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																											\
					}																																								\
					else																																							\
					{																																								\
						link_map *linkMap = nullptr;																																\
						const char* libraryName = "unknown";																														\
						if (dlinfo(mVulkanSharedLibrary, RTLD_DI_LINKMAP, &linkMap))																								\
						{																																							\
							libraryName = linkMap->l_name;																															\
						}																																							\
						std::cout << "Vulkan error: Failed to locate the entry point \"" << #funcName << "\" within the Vulkan shared library \"" << libraryName << "\"\n";		/* TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead*/ \
						result = false;																																				\
					}																																								\
				}
		#else
			#error "Unsupported platform"
		#endif

		// Load the entry points
		// TODO(co) Implement me
		// IMPORT_FUNC(glGetString);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
