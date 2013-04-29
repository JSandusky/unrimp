/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#define DIRECT3D10_DEFINERUNTIMELINKING

#include "Direct3D10Renderer/Direct3D10RuntimeLinking.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_PRINTF()"
#ifdef WIN32
	#include <Renderer/WindowsHeader.h>
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D10RuntimeLinking::Direct3D10RuntimeLinking() :
		mD3D10SharedLibrary(nullptr),
		mD3DX10SharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing to do in here
	}

	Direct3D10RuntimeLinking::~Direct3D10RuntimeLinking()
	{
		// Destroy the shared library instances
		#ifdef WIN32
			if (nullptr != mD3D10SharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mD3D10SharedLibrary));
			}
			if (nullptr != mD3DX10SharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mD3DX10SharedLibrary));
			}
		#else
			#error "Unsupported platform"
		#endif
	}

	bool Direct3D10RuntimeLinking::isDirect3D10Avaiable()
	{
		// Already initialized?
		if (!mInitialized)
		{
			// We're now initialized
			mInitialized = true;

			// Load the shared libraries
			if (loadSharedLibraries())
			{
				// Load the D3D10 and D3DX10 entry points
				mEntryPointsRegistered = (loadD3D10EntryPoints() && loadD3DX10EntryPoints());
			}
		}

		// Entry points successfully registered?
		return mEntryPointsRegistered;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool Direct3D10RuntimeLinking::loadSharedLibraries()
	{
		// Load the shared library
		#ifdef WIN32
			mD3D10SharedLibrary = ::LoadLibraryExA("d3d10.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr != mD3D10SharedLibrary)
			{
				mD3DX10SharedLibrary = ::LoadLibraryExA("d3dx10_43.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr == mD3DX10SharedLibrary)
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Failed to load in the shared library \"d3dx10_43.dll\"\n")
				}
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Failed to load in the shared library \"d3d10.dll\"\n")
			}
		#else
			#error "Unsupported platform"
		#endif

		// Done
		return (nullptr != mD3D10SharedLibrary && nullptr != mD3DX10SharedLibrary);
	}

	bool Direct3D10RuntimeLinking::loadD3D10EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																																					\
				if (result)																																									\
				{																																											\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mD3D10SharedLibrary), #funcName);																					\
					if (nullptr != symbol)																																					\
					{																																										\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																													\
					}																																										\
					else																																									\
					{																																										\
						wchar_t moduleFilename[MAX_PATH];																																	\
						moduleFilename[0] = '\0';																																			\
						::GetModuleFileNameW(static_cast<HMODULE>(mD3D10SharedLibrary), moduleFilename, MAX_PATH);																			\
						RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 10 error: Failed to locate the entry point \"%s\" within the Direct3D 10 shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																						\
					}																																										\
				}
		#else
			#error "Unsupported platform"
		#endif

		// Load the entry points
		IMPORT_FUNC(D3D10CreateDevice);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	bool Direct3D10RuntimeLinking::loadD3DX10EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																																					\
				if (result)																																									\
				{																																											\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mD3DX10SharedLibrary), #funcName);																					\
					if (nullptr != symbol)																																					\
					{																																										\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																													\
					}																																										\
					else																																									\
					{																																										\
						wchar_t moduleFilename[MAX_PATH];																																	\
						moduleFilename[0] = '\0';																																			\
						::GetModuleFileNameW(static_cast<HMODULE>(mD3DX10SharedLibrary), moduleFilename, MAX_PATH);																			\
						RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 10 error: Failed to locate the entry point \"%s\" within the Direct3D 10 shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																						\
					}																																										\
				}
		#else
			#error "Unsupported platform"
		#endif

		// Load the entry points
		IMPORT_FUNC(D3DX10CompileFromMemory);
		IMPORT_FUNC(D3DX10FilterTexture);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
