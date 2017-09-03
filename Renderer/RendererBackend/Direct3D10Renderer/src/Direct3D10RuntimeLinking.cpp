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
#define DIRECT3D10_DEFINERUNTIMELINKING

#include "Direct3D10Renderer/Direct3D10RuntimeLinking.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"

#include <Renderer/ILog.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D10RuntimeLinking::Direct3D10RuntimeLinking(Direct3D10Renderer& direct3D10Renderer) :
		mDirect3D10Renderer(direct3D10Renderer),
		mD3D10SharedLibrary(nullptr),
		mD3DX10SharedLibrary(nullptr),
		mD3DCompilerSharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing here
	}

	Direct3D10RuntimeLinking::~Direct3D10RuntimeLinking()
	{
		// Destroy the shared library instances
		if (nullptr != mD3D10SharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3D10SharedLibrary));
		}
		if (nullptr != mD3DX10SharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3DX10SharedLibrary));
		}
		if (nullptr != mD3DCompilerSharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3DCompilerSharedLibrary));
		}
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
				// Load the D3D10, D3DX10 and D3DCompiler entry points
				mEntryPointsRegistered = (loadD3D10EntryPoints() && loadD3DX10EntryPoints() && loadD3DCompilerEntryPoints());
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
		mD3D10SharedLibrary = ::LoadLibraryExA("d3d10.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (nullptr != mD3D10SharedLibrary)
		{
			mD3DX10SharedLibrary = ::LoadLibraryExA("d3dx10_43.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr != mD3DX10SharedLibrary)
			{
				mD3DCompilerSharedLibrary = ::LoadLibraryExA("D3DCompiler_47.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr == mD3DCompilerSharedLibrary)
				{
					RENDERER_LOG(mDirect3D10Renderer.getContext(), CRITICAL, "Failed to load in the shared Direct3D 10 library \"D3DCompiler_47.dll\"")
				}
			}
			else
			{
				RENDERER_LOG(mDirect3D10Renderer.getContext(), CRITICAL, "Failed to load in the shared Direct3D 10 library \"d3dx10_43.dll\"")
			}
		}
		else
		{
			RENDERER_LOG(mDirect3D10Renderer.getContext(), CRITICAL, "Failed to load in the Direct3D 10 shared library \"d3d10.dll\"")
		}

		// Done
		return (nullptr != mD3D10SharedLibrary && nullptr != mD3DX10SharedLibrary && nullptr != mD3DCompilerSharedLibrary);
	}

	bool Direct3D10RuntimeLinking::loadD3D10EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																							\
			if (result)																																											\
			{																																													\
				void* symbol = ::GetProcAddress(static_cast<HMODULE>(mD3D10SharedLibrary), #funcName);																							\
				if (nullptr != symbol)																																							\
				{																																												\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																															\
				}																																												\
				else																																											\
				{																																												\
					wchar_t moduleFilename[MAX_PATH];																																			\
					moduleFilename[0] = '\0';																																					\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3D10SharedLibrary), moduleFilename, MAX_PATH);																					\
					RENDERER_LOG(mDirect3D10Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 10 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																								\
				}																																												\
			}

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
		#define IMPORT_FUNC(funcName)																																							\
			if (result)																																											\
			{																																													\
				void* symbol = ::GetProcAddress(static_cast<HMODULE>(mD3DX10SharedLibrary), #funcName);																							\
				if (nullptr != symbol)																																							\
				{																																												\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																															\
				}																																												\
				else																																											\
				{																																												\
					wchar_t moduleFilename[MAX_PATH];																																			\
					moduleFilename[0] = '\0';																																					\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3DX10SharedLibrary), moduleFilename, MAX_PATH);																					\
					RENDERER_LOG(mDirect3D10Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 10 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																								\
				}																																												\
			}

		// Load the entry points
		IMPORT_FUNC(D3DX10FilterTexture);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	bool Direct3D10RuntimeLinking::loadD3DCompilerEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																							\
			if (result)																																											\
			{																																													\
				void* symbol = ::GetProcAddress(static_cast<HMODULE>(mD3DCompilerSharedLibrary), #funcName);																					\
				if (nullptr != symbol)																																							\
				{																																												\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																															\
				}																																												\
				else																																											\
				{																																												\
					wchar_t moduleFilename[MAX_PATH];																																			\
					moduleFilename[0] = '\0';																																					\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3DCompilerSharedLibrary), moduleFilename, MAX_PATH);																			\
					RENDERER_LOG(mDirect3D10Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 10 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																								\
				}																																												\
			}

		// Load the entry points
		IMPORT_FUNC(D3DCompile);
		IMPORT_FUNC(D3DCreateBlob);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
