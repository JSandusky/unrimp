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
#define DIRECT3D9_DEFINERUNTIMELINKING

#include "Direct3D9Renderer/Direct3D9RuntimeLinking.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"

#include <Renderer/ILog.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D9RuntimeLinking::Direct3D9RuntimeLinking(Direct3D9Renderer& direct3D9Renderer) :
		mDirect3D9Renderer(direct3D9Renderer),
		mD3D9SharedLibrary(nullptr),
		mD3DX9SharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing here
	}

	Direct3D9RuntimeLinking::~Direct3D9RuntimeLinking()
	{
		// Destroy the shared library instances
		if (nullptr != mD3D9SharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3D9SharedLibrary));
		}
		if (nullptr != mD3DX9SharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3DX9SharedLibrary));
		}
	}

	bool Direct3D9RuntimeLinking::isDirect3D9Avaiable()
	{
		// Already initialized?
		if (!mInitialized)
		{
			// We're now initialized
			mInitialized = true;

			// Load the shared libraries
			if (loadSharedLibraries())
			{
				// Load the D3D9 and D3DX9 entry points
				mEntryPointsRegistered = (loadD3D9EntryPoints() && loadD3DX9EntryPoints());
			}
		}

		// Entry points successfully registered?
		return mEntryPointsRegistered;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool Direct3D9RuntimeLinking::loadSharedLibraries()
	{
		// Load the shared library
		mD3D9SharedLibrary = ::LoadLibraryExA("d3d9.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (nullptr != mD3D9SharedLibrary)
		{
			mD3DX9SharedLibrary = ::LoadLibraryExA("d3dx9_43.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr == mD3DX9SharedLibrary)
			{
				RENDERER_LOG(mDirect3D9Renderer.getContext(), CRITICAL, "Failed to load in the Direct3D 9 shared library \"d3dx9_43.dll\"")
			}
		}
		else
		{
			RENDERER_LOG(mDirect3D9Renderer.getContext(), CRITICAL, "Failed to load in the Direct3D 9 shared library \"d3d9.dll\"")
		}

		// Done
		return (nullptr != mD3D9SharedLibrary && nullptr != mD3DX9SharedLibrary);
	}

	bool Direct3D9RuntimeLinking::loadD3D9EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																							\
			if (result)																																											\
			{																																													\
				void* symbol = ::GetProcAddress(static_cast<HMODULE>(mD3D9SharedLibrary), #funcName);																							\
				if (nullptr != symbol)																																							\
				{																																												\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																															\
				}																																												\
				else																																											\
				{																																												\
					wchar_t moduleFilename[MAX_PATH];																																			\
					moduleFilename[0] = '\0';																																					\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3D9SharedLibrary), moduleFilename, MAX_PATH);																					\
					RENDERER_LOG(mDirect3D9Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 9 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																								\
				}																																												\
			}

		// Load the entry points
		IMPORT_FUNC(Direct3DCreate9);
		IMPORT_FUNC(D3DPERF_GetStatus);
		IMPORT_FUNC(D3DPERF_SetOptions);
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			IMPORT_FUNC(D3DPERF_SetMarker);
			IMPORT_FUNC(D3DPERF_BeginEvent);
			IMPORT_FUNC(D3DPERF_EndEvent);
		#endif

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	bool Direct3D9RuntimeLinking::loadD3DX9EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																							\
			if (result)																																											\
			{																																													\
				void* symbol = ::GetProcAddress(static_cast<HMODULE>(mD3DX9SharedLibrary), #funcName);																							\
				if (nullptr != symbol)																																							\
				{																																												\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																															\
				}																																												\
				else																																											\
				{																																												\
					wchar_t moduleFilename[MAX_PATH];																																			\
					moduleFilename[0] = '\0';																																					\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3DX9SharedLibrary), moduleFilename, MAX_PATH);																					\
					RENDERER_LOG(mDirect3D9Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 9 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																								\
				}																																												\
			}

		// Load the entry points
		IMPORT_FUNC(D3DXLoadSurfaceFromMemory);
		IMPORT_FUNC(D3DXCompileShader);
		IMPORT_FUNC(D3DXGetShaderConstantTable);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
