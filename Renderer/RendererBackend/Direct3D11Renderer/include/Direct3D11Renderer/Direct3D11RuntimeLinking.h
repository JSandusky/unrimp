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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/D3D11.h"	// It's safe to include this header in here because "Direct3D11RuntimeLinking.h" should not included by users


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{
	class Direct3D11Renderer;
	struct AGSContext;	// AMD AGS
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 11 runtime linking
	*/
	class Direct3D11RuntimeLinking final
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D11Renderer
		*    Owner Direct3D 11 renderer instance
		*/
		explicit Direct3D11RuntimeLinking(Direct3D11Renderer& direct3D11Renderer);

		/**
		*  @brief
		*    Destructor
		*/
		~Direct3D11RuntimeLinking();

		/**
		*  @brief
		*    Return whether or not Direct3D 11 is available
		*
		*  @return
		*    "true" if Direct3D 11 is available, else "false"
		*/
		bool isDirect3D11Avaiable();

		/**
		*  @brief
		*    Return the AMD AGS instance
		*
		*  @return
		*    The AMD AGS instance, can be a null pointer, don't destroy the returned instance
		*/
		AGSContext* getAgsContext() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit Direct3D11RuntimeLinking(const Direct3D11RuntimeLinking& source) = delete;
		Direct3D11RuntimeLinking& operator =(const Direct3D11RuntimeLinking& source) = delete;

		/**
		*  @brief
		*    Load the shared libraries
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadSharedLibraries();

		/**
		*  @brief
		*    Load the D3D11 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadD3D11EntryPoints();

		/**
		*  @brief
		*    Load the D3DX11 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadD3DX11EntryPoints();

		/**
		*  @brief
		*    Load the D3DCompiler entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadD3DCompilerEntryPoints();

		/**
		*  @brief
		*    Load the AMD AGS entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadAmdAgsEntryPoints();

		/**
		*  @brief
		*    Load the NvAPI entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadNvAPIEntryPoints();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Direct3D11Renderer&	mDirect3D11Renderer;		///< Owner Direct3D 11 renderer instance
		void*				mD3D11SharedLibrary;		///< D3D11 shared library, can be a null pointer
		void*				mD3DX11SharedLibrary;		///< D3DX11 shared library, can be a null pointer
		void*				mD3DCompilerSharedLibrary;	///< D3DCompiler shared library, can be a null pointer
		void*				mAmdAgsSharedLibrary;		///< AMD AGS shared library, can be a null pointer
		AGSContext*			mAgsContext;				///< AMD AGS context, can be a null pointer
		void*				mNvAPISharedLibrary;		///< NvAPI shared library, can be a null pointer
		bool				mEntryPointsRegistered;		///< Entry points successfully registered?
		bool				mInitialized;				///< Already initialized?


	};


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	// Redirect D3D11* and D3DX11* function calls to funcPtr_D3D11* and funcPtr_D3DX11*
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif


	//[-------------------------------------------------------]
	//[ D3D11 core functions                                  ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D11_DEFINERUNTIMELINKING
		#define FNDEF_D3D11(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3D11(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3D11(HRESULT,	D3D11CreateDevice,	(__in_opt IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, __in_ecount_opt( FeatureLevels ) CONST D3D_FEATURE_LEVEL*, UINT, UINT, __out_opt ID3D11Device**, __out_opt D3D_FEATURE_LEVEL*, __out_opt ID3D11DeviceContext**));
	#define D3D11CreateDevice	FNPTR(D3D11CreateDevice)


	//[-------------------------------------------------------]
	//[ D3DX11 functions                                      ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D11_DEFINERUNTIMELINKING
		#define FNDEF_D3DX11(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3DX11(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3DX11(HRESULT,	D3DX11FilterTexture,	(ID3D11DeviceContext*, ID3D11Resource*, UINT, UINT));
	#define D3DX11FilterTexture	FNPTR(D3DX11FilterTexture)


	//[-------------------------------------------------------]
	//[ D3DCompiler functions                                 ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D11_DEFINERUNTIMELINKING
		#define FNDEF_D3DX11(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3DX11(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	typedef __interface ID3D10Blob *LPD3D10BLOB;	// "__interface" is no keyword of the ISO C++ standard, shouldn't be a problem because this in here is MS Windows only and it's also within the Direct3D headers we have to use
	typedef ID3D10Blob ID3DBlob;
	FNDEF_D3DX11(HRESULT,	D3DCompile,		(LPCVOID, SIZE_T, LPCSTR, CONST D3D_SHADER_MACRO*, ID3DInclude*, LPCSTR, LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**));
	FNDEF_D3DX11(HRESULT,	D3DCreateBlob,	(SIZE_T Size, ID3DBlob** ppBlob));
	#define D3DCompile		FNPTR(D3DCompile)
	#define D3DCreateBlob	FNPTR(D3DCreateBlob)


	//[-------------------------------------------------------]
	//[ AMD AGS functions                                     ]
	//[-------------------------------------------------------]
	// -> Using v5.1.1 - 2017-09-19
	// -> From https://github.com/GPUOpen-LibrariesAndSDKs/AGS_SDK and https://raw.githubusercontent.com/GPUOpen-LibrariesAndSDKs/Barycentrics12/master/ags_lib/inc/amd_ags.h
	#ifdef DIRECT3D11_DEFINERUNTIMELINKING
		#define FNDEF_AMD_AGS(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_AMD_AGS(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	struct AGSGPUInfo;
	struct AGSConfiguration;
	struct AGSDX11DeviceCreationParams
	{
		IDXGIAdapter*               pAdapter;
		D3D_DRIVER_TYPE             DriverType;
		HMODULE                     Software;
		UINT                        Flags;
		const D3D_FEATURE_LEVEL*    pFeatureLevels;
		UINT                        FeatureLevels;
		UINT                        SDKVersion;
		const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc;
	};
	struct AGSDX11ExtensionParams
	{
		unsigned int uavSlot;
		const WCHAR* pAppName;
		UINT         appVersion;
		const WCHAR* pEngineName;
		UINT         engineVersion;
	};
	struct AGSDX11ReturnedParams
	{
		ID3D11Device*        pDevice;
		D3D_FEATURE_LEVEL    FeatureLevel;
		ID3D11DeviceContext* pImmediateContext;
		IDXGISwapChain*      pSwapChain;
		unsigned int         extensionsSupported;
	};
	enum AGSReturnCode
	{
		AGS_SUCCESS,
		AGS_INVALID_ARGS,
		AGS_OUT_OF_MEMORY,
		AGS_ERROR_MISSING_DLL,
		AGS_ERROR_LEGACY_DRIVER,
		AGS_EXTENSION_NOT_SUPPORTED,
		AGS_ADL_FAILURE,
	};
	FNDEF_AMD_AGS(AGSReturnCode,	agsInit,													(AGSContext** context, const AGSConfiguration* config, AGSGPUInfo* gpuInfo));
	FNDEF_AMD_AGS(AGSReturnCode,	agsDeInit,													(AGSContext* context));
	FNDEF_AMD_AGS(AGSReturnCode,	agsDriverExtensionsDX11_CreateDevice,						(AGSContext* context, AGSDX11DeviceCreationParams* creationParams, AGSDX11ExtensionParams* extensionParams, AGSDX11ReturnedParams* returnedParams));
	FNDEF_AMD_AGS(AGSReturnCode,	agsDriverExtensionsDX11_DestroyDevice,						(AGSContext* context, ID3D11Device* device, unsigned int* references));
	FNDEF_AMD_AGS(AGSReturnCode,	agsDriverExtensionsDX11_MultiDrawInstancedIndirect,			(AGSContext* context, unsigned int drawCount, ID3D11Buffer* pBufferForArgs, unsigned int alignedByteOffsetForArgs, unsigned int byteStrideForArgs));
	FNDEF_AMD_AGS(AGSReturnCode,	agsDriverExtensionsDX11_MultiDrawIndexedInstancedIndirect,	(AGSContext* context, unsigned int drawCount, ID3D11Buffer* pBufferForArgs, unsigned int alignedByteOffsetForArgs, unsigned int byteStrideForArgs));
	#define agsInit														FNPTR(agsInit)
	#define agsDeInit													FNPTR(agsDeInit)
	#define agsDriverExtensionsDX11_CreateDevice						FNPTR(agsDriverExtensionsDX11_CreateDevice)
	#define agsDriverExtensionsDX11_DestroyDevice						FNPTR(agsDriverExtensionsDX11_DestroyDevice)
	#define agsDriverExtensionsDX11_MultiDrawInstancedIndirect			FNPTR(agsDriverExtensionsDX11_MultiDrawInstancedIndirect)
	#define agsDriverExtensionsDX11_MultiDrawIndexedInstancedIndirect	FNPTR(agsDriverExtensionsDX11_MultiDrawIndexedInstancedIndirect)


	//[-------------------------------------------------------]
	//[ NvAPI functions                                       ]
	//[-------------------------------------------------------]
	// From https://developer.nvidia.com/nvapi and http://developer.download.nvidia.com/NVAPI/PG-5116-001_v01_public.pdf
	#ifdef DIRECT3D11_DEFINERUNTIMELINKING
		#define FNDEF_NvAPI(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_NvAPI(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	typedef int NvAPI_Status;
	typedef unsigned int NvU32;
	FNDEF_NvAPI(NvAPI_Status,	NvAPI_Initialize,								());
	FNDEF_NvAPI(NvAPI_Status,	NvAPI_Unload,									());
	FNDEF_NvAPI(NvAPI_Status,	NvAPI_D3D11_MultiDrawInstancedIndirect,			(__in ID3D11DeviceContext* pDevContext11, __in NvU32 drawCount, __in ID3D11Buffer* pBuffer, __in NvU32 alignedByteOffsetForArgs, __in NvU32 alignedByteStrideForArgs));
	FNDEF_NvAPI(NvAPI_Status,	NvAPI_D3D11_MultiDrawIndexedInstancedIndirect,	(__in ID3D11DeviceContext* pDevContext11, __in NvU32 drawCount, __in ID3D11Buffer* pBuffer, __in NvU32 alignedByteOffsetForArgs, __in NvU32 alignedByteStrideForArgs));
	#define NvAPI_Initialize								FNPTR(NvAPI_Initialize)
	#define NvAPI_Unload									FNPTR(NvAPI_Unload)
	#define NvAPI_D3D11_MultiDrawInstancedIndirect			FNPTR(NvAPI_D3D11_MultiDrawInstancedIndirect)
	#define NvAPI_D3D11_MultiDrawIndexedInstancedIndirect	FNPTR(NvAPI_D3D11_MultiDrawIndexedInstancedIndirect)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
