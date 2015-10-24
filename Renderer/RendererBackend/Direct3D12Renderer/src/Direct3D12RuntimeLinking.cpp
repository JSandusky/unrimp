/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#define DIRECT3D12_DEFINERUNTIMELINKING

#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_PRINTF()"
#ifdef WIN32
	#include <Renderer/WindowsHeader.h>
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D12RuntimeLinking::Direct3D12RuntimeLinking() :
		mD3D12SharedLibrary(nullptr),
		mD3DX12SharedLibrary(nullptr),
		mD3DCompilerSharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing to do in here
	}

	Direct3D12RuntimeLinking::~Direct3D12RuntimeLinking()
	{
		// Destroy the shared library instances
		#ifdef WIN32
			if (nullptr != mD3D12SharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mD3D12SharedLibrary));
			}
			if (nullptr != mD3DX12SharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mD3DX12SharedLibrary));
			}
			if (nullptr != mD3DCompilerSharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mD3DCompilerSharedLibrary));
			}
		
		#else
			#error "Unsupported platform"
		#endif
	}

	bool Direct3D12RuntimeLinking::isDirect3D12Avaiable()
	{
		// Already initialized?
		if (!mInitialized)
		{
			// We're now initialized
			mInitialized = true;

			// Load the shared libraries
			if (loadSharedLibraries())
			{
				// Load the D3D12, D3DX12 and D3DCompiler entry points
				mEntryPointsRegistered = (loadD3D12EntryPoints() && loadD3DX12EntryPoints() && loadD3DCompilerEntryPoints());
			}
		}

		// Entry points successfully registered?
		return mEntryPointsRegistered;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	bool Direct3D12RuntimeLinking::loadSharedLibraries()
	{
		// TODO(co) Direct3D 12 update
		// Load the shared library
		#ifdef WIN32
			mD3D12SharedLibrary = ::LoadLibraryExA("d3d12.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr != mD3D12SharedLibrary)
			{
				mD3DX12SharedLibrary = ::LoadLibraryExA("d3dx12_43.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr != mD3DX12SharedLibrary)
				{
					mD3DCompilerSharedLibrary = ::LoadLibraryExA("D3DCompiler_47.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
					if (nullptr == mD3DCompilerSharedLibrary)
					{
						RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to load in the shared library \"D3DCompiler_47.dll\"\n")
					}
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to load in the shared library \"d3dx12_43.dll\"\n")
				}
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to load in the shared library \"d3d12.dll\"\n")
			}
		#else
			#error "Unsupported platform"
		#endif

		// Done
		return (nullptr != mD3D12SharedLibrary && nullptr != mD3DX12SharedLibrary && nullptr != mD3DCompilerSharedLibrary);
	}

	bool Direct3D12RuntimeLinking::loadD3D12EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																																					\
				if (result)																																									\
				{																																											\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mD3D12SharedLibrary), #funcName);																					\
					if (nullptr != symbol)																																					\
					{																																										\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																													\
					}																																										\
					else																																									\
					{																																										\
						wchar_t moduleFilename[MAX_PATH];																																	\
						moduleFilename[0] = '\0';																																			\
						::GetModuleFileNameW(static_cast<HMODULE>(mD3D12SharedLibrary), moduleFilename, MAX_PATH);																			\
						RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 12 error: Failed to locate the entry point \"%s\" within the Direct3D 12 shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																						\
					}																																										\
				}
		#else
			#error "Unsupported platform"
		#endif

		// Load the entry points
		IMPORT_FUNC(D3D12CreateDevice);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	bool Direct3D12RuntimeLinking::loadD3DX12EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																																					\
				if (result)																																									\
				{																																											\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mD3DX12SharedLibrary), #funcName);																					\
					if (nullptr != symbol)																																					\
					{																																										\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																													\
					}																																										\
					else																																									\
					{																																										\
						wchar_t moduleFilename[MAX_PATH];																																	\
						moduleFilename[0] = '\0';																																			\
						::GetModuleFileNameW(static_cast<HMODULE>(mD3DX12SharedLibrary), moduleFilename, MAX_PATH);																			\
						RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 12 error: Failed to locate the entry point \"%s\" within the Direct3D 12 shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																						\
					}																																										\
				}
		#else
			#error "Unsupported platform"
		#endif

		// Load the entry points
		IMPORT_FUNC(D3DX12CompileFromMemory);
		IMPORT_FUNC(D3DX12FilterTexture);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	bool Direct3D12RuntimeLinking::loadD3DCompilerEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
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
						RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 12 error: Failed to locate the entry point \"%s\" within the Direct3D 12 shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																						\
					}																																										\
				}
		#else
			#error "Unsupported platform"
		#endif

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
} // Direct3D12Renderer
