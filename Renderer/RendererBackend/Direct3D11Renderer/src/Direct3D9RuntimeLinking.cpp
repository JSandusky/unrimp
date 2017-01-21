/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#define DIRECT3D9_DEFINERUNTIMELINKING

#include "Direct3D11Renderer/Direct3D9RuntimeLinking.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_PRINTF()"
#include <Renderer/WindowsHeader.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D9RuntimeLinking::Direct3D9RuntimeLinking() :
		mD3D9SharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing here
	}

	Direct3D9RuntimeLinking::~Direct3D9RuntimeLinking()
	{
		// Destroy the shared library instance
		if (nullptr != mD3D9SharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3D9SharedLibrary));
		}
	}

	bool Direct3D9RuntimeLinking::isDirect3D9Avaiable()
	{
		// Already initialized?
		if (!mInitialized)
		{
			// We're now initialized
			mInitialized = true;

			// Load the shared library
			if (loadSharedLibrary())
			{
				// Load the D3D9 entry points
				mEntryPointsRegistered = loadD3D9EntryPoints();
			}
		}

		// Entry points successfully registered?
		return mEntryPointsRegistered;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool Direct3D9RuntimeLinking::loadSharedLibrary()
	{
		// Load the shared library
		mD3D9SharedLibrary = ::LoadLibraryExA("d3d9.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (nullptr == mD3D9SharedLibrary)
		{
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Failed to load in the shared library \"d3d9.dll\"\n")
		}

		// Done
		return (nullptr != mD3D9SharedLibrary);
	}

	bool Direct3D9RuntimeLinking::loadD3D9EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																					\
			if (result)																																									\
			{																																											\
				void *symbol = ::GetProcAddress(static_cast<HMODULE>(mD3D9SharedLibrary), #funcName);																					\
				if (nullptr != symbol)																																					\
				{																																										\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																													\
				}																																										\
				else																																									\
				{																																										\
					wchar_t moduleFilename[MAX_PATH];																																	\
					moduleFilename[0] = '\0';																																			\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3D9SharedLibrary), moduleFilename, MAX_PATH);																			\
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 11 error: Failed to locate the entry point \"%s\" within the Direct3D 9 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																						\
				}																																										\
			}

		// Load the entry points
		IMPORT_FUNC(D3DPERF_GetStatus);
		IMPORT_FUNC(D3DPERF_SetOptions);
		IMPORT_FUNC(D3DPERF_SetMarker);
		IMPORT_FUNC(D3DPERF_BeginEvent);
		IMPORT_FUNC(D3DPERF_EndEvent);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
