/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "Direct3D11Renderer/Direct3D11Renderer.h"

#include <Renderer/ILog.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D11RuntimeLinking::Direct3D11RuntimeLinking(Direct3D11Renderer& direct3D11Renderer) :
		mDirect3D11Renderer(direct3D11Renderer),
		mD3D11SharedLibrary(nullptr),
		mD3DX11SharedLibrary(nullptr),
		mD3DCompilerSharedLibrary(nullptr),
		mAmdAgsSharedLibrary(nullptr),
		mAgsContext(nullptr),
		mNvAPISharedLibrary(nullptr),
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
		if (nullptr != mAmdAgsSharedLibrary)
		{
			if (nullptr != agsDeInit && AGS_SUCCESS != agsDeInit(mAgsContext))
			{
				RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Direct3D 11: Failed to unload AMG AGS")
			}
			::FreeLibrary(static_cast<HMODULE>(mAmdAgsSharedLibrary));
		}
		if (nullptr != mNvAPISharedLibrary)
		{
			if (nullptr != NvAPI_Unload && 0 != NvAPI_Unload())
			{
				RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Direct3D 11: Failed to unload NvAPI")
			}
			::FreeLibrary(static_cast<HMODULE>(mNvAPISharedLibrary));
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

	AGSContext* Direct3D11RuntimeLinking::getAgsContext() const
	{
		return mAgsContext;
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
					RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Failed to load in the shared Direct3D 11 library \"D3DCompiler_47.dll\"")
				}

				// TODO(co) Need to ensure this only runs on AMD GPUs and also makes no problems when changing the GPU inside a system
				// Optional vendor specific part: AMD AGS
				mAmdAgsSharedLibrary = ::LoadLibraryExA("amd_ags.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr != mAmdAgsSharedLibrary && !loadAmdAgsEntryPoints())
				{
					RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Direct3D 11: Failed to load AMD AGS function entry points")
					::FreeLibrary(static_cast<HMODULE>(mAmdAgsSharedLibrary));
					mAmdAgsSharedLibrary									  = nullptr;
					agsInit													  = nullptr;
					agsDeInit												  = nullptr;
					agsDriverExtensionsDX11_CreateDevice					  = nullptr;
					agsDriverExtensionsDX11_DestroyDevice					  = nullptr;
					agsDriverExtensionsDX11_MultiDrawInstancedIndirect		  = nullptr;
					agsDriverExtensionsDX11_MultiDrawIndexedInstancedIndirect = nullptr;
				}

				// Optional vendor specific part: NvAPI
				mNvAPISharedLibrary = ::LoadLibraryExA("nvapi.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr != mNvAPISharedLibrary && !loadNvAPIEntryPoints())
				{
					RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Direct3D 11: Failed to load NvAPI function entry points")
					::FreeLibrary(static_cast<HMODULE>(mNvAPISharedLibrary));
					mNvAPISharedLibrary							  = nullptr;
					NvAPI_Initialize							  = nullptr;
					NvAPI_Unload								  = nullptr;
					NvAPI_D3D11_MultiDrawInstancedIndirect		  = nullptr;
					NvAPI_D3D11_MultiDrawIndexedInstancedIndirect = nullptr;
				}
			}
			else
			{
				RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Failed to load in the shared Direct3D 11 library \"d3dx11_43.dll\"")
			}
		}
		else
		{
			RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Failed to load in the shared Direct3D 11 library \"d3d11.dll\"")
		}

		// Done
		return (nullptr != mD3D11SharedLibrary && nullptr != mD3DX11SharedLibrary && nullptr != mD3DCompilerSharedLibrary);
	}

	bool Direct3D11RuntimeLinking::loadD3D11EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																							\
			if (result)																																											\
			{																																													\
				void* symbol = ::GetProcAddress(static_cast<HMODULE>(mD3D11SharedLibrary), #funcName);																							\
				if (nullptr != symbol)																																							\
				{																																												\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																															\
				}																																												\
				else																																											\
				{																																												\
					wchar_t moduleFilename[MAX_PATH];																																			\
					moduleFilename[0] = '\0';																																					\
					::GetModuleFileNameW(static_cast<HMODULE>(mD3D11SharedLibrary), moduleFilename, MAX_PATH);																					\
					RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 11 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																								\
				}																																												\
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
					RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 11 shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																								\
				}																																												\
			}

		// Load the entry points
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
					RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the Direct3D 11 shared library \"%s\"", #funcName, moduleFilename)	\
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

	bool Direct3D11RuntimeLinking::loadAmdAgsEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																						\
			if (result)																																										\
			{																																												\
				void* symbol = ::GetProcAddress(static_cast<HMODULE>(mAmdAgsSharedLibrary), #funcName);																						\
				if (nullptr != symbol)																																						\
				{																																											\
					*(reinterpret_cast<void**>(&(funcName))) = symbol;																														\
				}																																											\
				else																																										\
				{																																											\
					wchar_t moduleFilename[MAX_PATH];																																		\
					moduleFilename[0] = '\0';																																				\
					::GetModuleFileNameW(static_cast<HMODULE>(mAmdAgsSharedLibrary), moduleFilename, MAX_PATH);																				\
					RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the AMD AGS shared library \"%s\"", #funcName, moduleFilename)	\
					result = false;																																							\
				}																																											\
			}

		// Load the entry points
		IMPORT_FUNC(agsInit);
		IMPORT_FUNC(agsDeInit);
		IMPORT_FUNC(agsDriverExtensionsDX11_CreateDevice);
		IMPORT_FUNC(agsDriverExtensionsDX11_DestroyDevice);
		IMPORT_FUNC(agsDriverExtensionsDX11_MultiDrawInstancedIndirect);
		IMPORT_FUNC(agsDriverExtensionsDX11_MultiDrawIndexedInstancedIndirect);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Initialize AMD AGS (e.g. for multi-indirect-draw support)
		if (nullptr != agsInit && AGS_SUCCESS != agsInit(&mAgsContext, nullptr, nullptr))
		{
			RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Direct3D 11: Failed to initialize AMD AGS")
			result = false;
		}

		// Done
		return result;
	}

	bool Direct3D11RuntimeLinking::loadNvAPIEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#define IMPORT_FUNC(funcName)																																					\
			void* symbol = ::GetProcAddress(static_cast<HMODULE>(mNvAPISharedLibrary), #funcName);																						\
			if (nullptr != symbol)																																						\
			{																																											\
				*(reinterpret_cast<void**>(&(funcName))) = symbol;																														\
			}																																											\
			else																																										\
			{																																											\
				wchar_t moduleFilename[MAX_PATH];																																		\
				moduleFilename[0] = '\0';																																				\
				::GetModuleFileNameW(static_cast<HMODULE>(mNvAPISharedLibrary), moduleFilename, MAX_PATH);																				\
				RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Failed to locate the entry point \"%s\" within the NvAPI shared library \"%s\"", #funcName, moduleFilename)	\
				result = false;																																							\
			}
		FNDEF_NvAPI(void*,	nvapi_QueryInterface,	(unsigned int offset));
		#define nvapi_QueryInterface	FNPTR(nvapi_QueryInterface)
		#define IMPORT_NVAPI_FUNC(funcName, ID) \
			*(reinterpret_cast<void**>(&(funcName))) = nvapi_QueryInterface(ID); \
			if (nullptr == funcName) \
			{ \
				result = false; \
			}

		// Load the entry points
		nvapi_QueryInterface = nullptr;
		IMPORT_FUNC(nvapi_QueryInterface);

		// Query function pointers
		if (nullptr != nvapi_QueryInterface)
		{
			IMPORT_NVAPI_FUNC(NvAPI_Initialize, 0x150E828UL)
			IMPORT_NVAPI_FUNC(NvAPI_Unload, 0xD22BDD7EUL)
			IMPORT_NVAPI_FUNC(NvAPI_D3D11_MultiDrawInstancedIndirect, 0xD4E26BBF)
			IMPORT_NVAPI_FUNC(NvAPI_D3D11_MultiDrawIndexedInstancedIndirect, 0x59E890F9)
		}

		// Undefine the helper macro
		#undef IMPORT_NVAPI_FUNC
		#undef NvAPI_QueryInterface
		#undef IMPORT_FUNC

		// Initialize NvAPI (e.g. for multi-indirect-draw support)
		if (result && nullptr != NvAPI_Initialize && 0 != NvAPI_Initialize())
		{
			RENDERER_LOG(mDirect3D11Renderer.getContext(), CRITICAL, "Direct3D 11: Failed to initialize NvAPI")
			result = false;
		}

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
