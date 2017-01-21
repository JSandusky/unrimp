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
#define DIRECT3D11_DEFINERUNTIMELINKING

#include "Direct3D11Renderer/Direct3D11RuntimeLinking.h"

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
	Direct3D11RuntimeLinking::Direct3D11RuntimeLinking() :
		mD3D11SharedLibrary(nullptr),
		mD3DX11SharedLibrary(nullptr),
		mD3DCompilerSharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing here
	}

	Direct3D11RuntimeLinking::~Direct3D11RuntimeLinking()
	{
		// Destroy the shared library instances
		if (nullptr != mD3D11SharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3D11SharedLibrary));
		}
		if (nullptr != mD3DX11SharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3DX11SharedLibrary));
		}
		if (nullptr != mD3DCompilerSharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3DCompilerSharedLibrary));
		}
	}

	bool Direct3D11RuntimeLinking::isDirect3D11Avaiable()
	{
		// Already initialized?
		if (!mInitialized)
		{
			// We're now initialized
			mInitialized = true;

			// Load the shared libraries
			if (loadSharedLibraries())
			{
				// Load the D3D11, D3DX11 and D3DCompiler entry points
				mEntryPointsRegistered = (loadD3D11EntryPoints() && loadD3DX11EntryPoints() && loadD3DCompilerEntryPoints());
			}
		}

		// Entry points successfully registered?
		return mEntryPointsRegistered;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool Direct3D11RuntimeLinking::loadSharedLibraries()
	{
		// Load the shared library
		mD3D11SharedLibrary = ::LoadLibraryExA("d3d11.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (nullptr != mD3D11SharedLibrary)
		{
			mD3DX11SharedLibrary = ::LoadLibraryExA("d3dx11_43.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr != mD3DX11SharedLibrary)
			{
				mD3DCompilerSharedLibrary = ::LoadLibraryExA("D3DCompiler_47.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr == mD3DCompilerSharedLibrary)
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Failed to load in the shared library \"D3DCompiler_47.dll\"\n")
				}
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Failed to load in the shared library \"d3dx11_43.dll\"\n")
			}
		}
		else
		{
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Failed to load in the shared library \"d3d11.dll\"\n")
		}

		// Done
		return (nullptr != mD3D11SharedLibrary && nullptr != mD3DX11SharedLibrary && nullptr != mD3DCompilerSharedLibrary);
	}

	bool Direct3D11RuntimeLinking::loadD3D11EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																					\
			if (result)																																									\
			{																																											\
				void *symbol = ::GetProcAddress(static_cast<HMODULE>(mD3D11SharedLibrary), #funcName);																					\
				if (nullptr != symbol)																																					\
				{																																										\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																													\
				}																																										\
				else																																									\
				{																																										\
					wchar_t moduleFilename[MAX_PATH];																																	\
					moduleFilename[0] = '\0';																																			\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3D11SharedLibrary), moduleFilename, MAX_PATH);																			\
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 11 error: Failed to locate the entry point \"%s\" within the Direct3D 11 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																						\
				}																																										\
			}

		// Load the entry points
		IMPORT_FUNC(D3D11CreateDevice);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	bool Direct3D11RuntimeLinking::loadD3DX11EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																					\
			if (result)																																									\
			{																																											\
				void *symbol = ::GetProcAddress(static_cast<HMODULE>(mD3DX11SharedLibrary), #funcName);																					\
				if (nullptr != symbol)																																					\
				{																																										\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																													\
				}																																										\
				else																																									\
				{																																										\
					wchar_t moduleFilename[MAX_PATH];																																	\
					moduleFilename[0] = '\0';																																			\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3DX11SharedLibrary), moduleFilename, MAX_PATH);																			\
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 11 error: Failed to locate the entry point \"%s\" within the Direct3D 11 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																						\
				}																																										\
			}

		// Load the entry points
		IMPORT_FUNC(D3DX11CompileFromMemory);
		IMPORT_FUNC(D3DX11FilterTexture);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	bool Direct3D11RuntimeLinking::loadD3DCompilerEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																					\
			if (result)																																									\
			{																																											\
				void *symbol = ::GetProcAddress(static_cast<HMODULE>(mD3DCompilerSharedLibrary), #funcName);																			\
				if (nullptr != symbol)																																					\
				{																																										\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																													\
				}																																										\
				else																																									\
				{																																										\
					wchar_t moduleFilename[MAX_PATH];																																	\
					moduleFilename[0] = '\0';																																			\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3DCompilerSharedLibrary), moduleFilename, MAX_PATH);																	\
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 11 error: Failed to locate the entry point \"%s\" within the Direct3D 11 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																						\
				}																																										\
			}

		// Load the entry points
		IMPORT_FUNC(D3DCreateBlob);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
