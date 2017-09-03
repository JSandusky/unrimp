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
#define DIRECT3D12_DEFINERUNTIMELINKING

#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"

#include <Renderer/ILog.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D12RuntimeLinking::Direct3D12RuntimeLinking(Direct3D12Renderer& direct3D12Renderer) :
		mDirect3D12Renderer(direct3D12Renderer),
		mDxgiSharedLibrary(nullptr),
		mD3D12SharedLibrary(nullptr),
		mD3DX11SharedLibrary(nullptr),
		mD3DCompilerSharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing here
	}

	Direct3D12RuntimeLinking::~Direct3D12RuntimeLinking()
	{
		// Destroy the shared library instances
		if (nullptr != mDxgiSharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mDxgiSharedLibrary));
		}
		if (nullptr != mD3D12SharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3D12SharedLibrary));
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
				// Load the DXGI, D3D12, D3DX11 and D3DCompiler entry points
				mEntryPointsRegistered = (loadDxgiEntryPoints() && loadD3D12EntryPoints() && loadD3DX11EntryPoints() && loadD3DCompilerEntryPoints());
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
		// Load the shared library
		mDxgiSharedLibrary = ::LoadLibraryExA("dxgi.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (nullptr != mDxgiSharedLibrary)
		{
			mD3D12SharedLibrary = ::LoadLibraryExA("d3d12.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr != mD3D12SharedLibrary)
			{
				mD3DX11SharedLibrary = ::LoadLibraryExA("d3dx11_43.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr != mD3DX11SharedLibrary)
				{
					mD3DCompilerSharedLibrary = ::LoadLibraryExA("D3DCompiler_47.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
					if (nullptr == mD3DCompilerSharedLibrary)
					{
						RENDERER_LOG(mDirect3D12Renderer.getContext(), CRITICAL, "Failed to load in the shared Direct3D 12 library \"D3DCompiler_47.dll\"")
					}
				}
				else
				{
					RENDERER_LOG(mDirect3D12Renderer.getContext(), CRITICAL, "Failed to load in the shared Direct3D 12 library \"d3dx11_43.dll\"")
				}
			}
			else
			{
				RENDERER_LOG(mDirect3D12Renderer.getContext(), CRITICAL, "Failed to load in the shared Direct3D 12 library \"d3d12.dll\"")
			}
		}
		else
		{
			RENDERER_LOG(mDirect3D12Renderer.getContext(), CRITICAL, "Failed to load in the shared Direct3D 12 library \"dxgi.dll\"")
		}

		// Done
		return (nullptr != mDxgiSharedLibrary && nullptr != mD3D12SharedLibrary && nullptr != mD3DX11SharedLibrary && nullptr != mD3DCompilerSharedLibrary);
	}

	bool Direct3D12RuntimeLinking::loadDxgiEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																									\
			if (result)																																													\
			{																																															\
				void* symbol = ::GetProcAddress(static_cast<HMODULE>(mDxgiSharedLibrary), #funcName);																									\
				if (nullptr != symbol)																																									\
				{																																														\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																																	\
				}																																														\
				else																																													\
				{																																														\
					wchar_t moduleFilename[MAX_PATH];																																					\
					moduleFilename[0] = '\0';																																							\
					::GetModuleFileNameW(static_cast<HMODULE>(mDxgiSharedLibrary), moduleFilename, MAX_PATH);																							\
					RENDERER_LOG(mDirect3D12Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 12 DXGI shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																										\
				}																																														\
			}

		// Load the entry points
		IMPORT_FUNC(CreateDXGIFactory1);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	bool Direct3D12RuntimeLinking::loadD3D12EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																							\
			if (result)																																											\
			{																																													\
				void* symbol = ::GetProcAddress(static_cast<HMODULE>(mD3D12SharedLibrary), #funcName);																							\
				if (nullptr != symbol)																																							\
				{																																												\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																															\
				}																																												\
				else																																											\
				{																																												\
					wchar_t moduleFilename[MAX_PATH];																																			\
					moduleFilename[0] = '\0';																																					\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3D12SharedLibrary), moduleFilename, MAX_PATH);																					\
					RENDERER_LOG(mDirect3D12Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 12 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																								\
				}																																												\
			}

		// Load the entry points
		IMPORT_FUNC(D3D12CreateDevice);
		IMPORT_FUNC(D3D12SerializeRootSignature);
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			IMPORT_FUNC(D3D12GetDebugInterface);
		#endif

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	bool Direct3D12RuntimeLinking::loadD3DX11EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																							\
			if (result)																																											\
			{																																													\
				void* symbol = ::GetProcAddress(static_cast<HMODULE>(mD3DX11SharedLibrary), #funcName);																							\
				if (nullptr != symbol)																																							\
				{																																												\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																															\
				}																																												\
				else																																											\
				{																																												\
					wchar_t moduleFilename[MAX_PATH];																																			\
					moduleFilename[0] = '\0';																																					\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3DX11SharedLibrary), moduleFilename, MAX_PATH);																					\
					RENDERER_LOG(mDirect3D12Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 12 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																								\
				}																																												\
			}

		// Load the entry points
		// IMPORT_FUNC(D3DX11FilterTexture);	// TODO(co) Direct3D 12 update

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	bool Direct3D12RuntimeLinking::loadD3DCompilerEntryPoints()
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
					RENDERER_LOG(mDirect3D12Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 12 shared library \"%s\"", #funcName, moduleFilename)	\
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
} // Direct3D12Renderer
