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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D12Renderer/D3D12.h"	// It's safe to include this header in here because "Direct3D12RuntimeLinking.h" should not included by users


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 12 runtime linking
	*
	*  @todo
	*    - TODO(co) Looks like there's no "D3DX12", so we stick to "D3DX12" for now
	*/
	class Direct3D12RuntimeLinking
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		Direct3D12RuntimeLinking();

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Direct3D12RuntimeLinking();

		/**
		*  @brief
		*    Return whether or not Direct3D 12 is available
		*
		*  @return
		*    "true" if Direct3D 12 is available, else "false"
		*/
		bool isDirect3D12Avaiable();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
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
		*    Load the DXGI entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadDxgiEntryPoints();

		/**
		*  @brief
		*    Load the D3D12 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadD3D12EntryPoints();

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


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		void *mDxgiSharedLibrary;			///< DXGI shared library, can be a null pointer
		void *mD3D12SharedLibrary;			///< D3D12 shared library, can be a null pointer
		void *mD3DX11SharedLibrary;			///< D3DX11 shared library, can be a null pointer
		void *mD3DCompilerSharedLibrary;	///< D3DCompiler shared library, can be a null pointer
		bool  mEntryPointsRegistered;		///< Entry points successfully registered?
		bool  mInitialized;					///< Already initialized?


	};


	//[-------------------------------------------------------]
	//[ DXGI core functions                                   ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D12_DEFINERUNTIMELINKING
		#define FNDEF_DXGI(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_DXGI(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_DXGI(HRESULT,	CreateDXGIFactory1,	(REFIID riid, _COM_Outptr_ void **ppFactory));


	//[-------------------------------------------------------]
	//[ D3D12 core functions                                  ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D12_DEFINERUNTIMELINKING
		#define FNDEF_D3D12(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3D12(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3D12(HRESULT,	D3D12CreateDevice,				(_In_opt_ IUnknown* pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, _In_ REFIID riid, _COM_Outptr_opt_ void** ppDevice));
	FNDEF_D3D12(HRESULT,	D3D12SerializeRootSignature,	(_In_ const D3D12_ROOT_SIGNATURE_DESC* pRootSignature, _In_ D3D_ROOT_SIGNATURE_VERSION Version, _Out_ ID3DBlob** ppBlob, _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorBlob));
	#ifndef DIRECT3D12RENDERER_NO_DEBUG
		FNDEF_D3D12(HRESULT,	D3D12GetDebugInterface,	(_In_ REFIID riid, _COM_Outptr_opt_ void** ppvDebug));
	#endif


	//[-------------------------------------------------------]
	//[ D3DX11 functions                                      ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D12_DEFINERUNTIMELINKING
		#define FNDEF_D3DX11(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3DX11(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3DX11(HRESULT,	D3DX11CompileFromMemory,	(LPCSTR, SIZE_T, LPCSTR, CONST D3D10_SHADER_MACRO *, LPD3D10INCLUDE, LPCSTR, LPCSTR, UINT, UINT, ID3DX12ThreadPump *, ID3D10Blob **, ID3D10Blob **, HRESULT *));
	// FNDEF_D3DX11(HRESULT,	D3DX11FilterTexture,		(ID3D12DeviceContext *, ID3D12Resource *, UINT, UINT));	// TODO(co) Direct3D 12 update


	//[-------------------------------------------------------]
	//[ D3DCompiler functions                                 ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D12_DEFINERUNTIMELINKING
		#define FNDEF_D3DX11(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3DX11(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	typedef __interface ID3D10Blob *LPD3D10BLOB;	// "__interface" is no keyword of the ISO C++ standard, shouldn't be a problem because this in here is MS Windows only and it's also within the Direct3D headers we have to use
	typedef ID3D10Blob ID3DBlob;
	FNDEF_D3DX11(HRESULT,	D3DCreateBlob,				(SIZE_T Size, ID3DBlob** ppBlob));


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif

	// Redirect D3D12* and D3DX11* function calls to funcPtr_D3D12* and funcPtr_D3DX11*

	// DXGI
	#define CreateDXGIFactory1	FNPTR(CreateDXGIFactory1)

	// D3D12
	#define D3D12CreateDevice			FNPTR(D3D12CreateDevice)
	#define D3D12SerializeRootSignature	FNPTR(D3D12SerializeRootSignature)
	#ifndef DIRECT3D12RENDERER_NO_DEBUG
		#define D3D12GetDebugInterface	FNPTR(D3D12GetDebugInterface)
	#endif

	// D3DX11
	#define D3DX11CompileFromMemory	FNPTR(D3DX11CompileFromMemory)
	// #define D3DX11FilterTexture		FNPTR(D3DX11FilterTexture)	// TODO(co) Direct3D 12 update

	// D3DCompiler
	#define D3DCreateBlob	FNPTR(D3DCreateBlob)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
