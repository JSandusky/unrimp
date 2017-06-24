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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/D3D11.h"	// It's safe to include this header in here because "Direct3D11RuntimeLinking.h" should not included by users


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
	class Direct3D11RuntimeLinking
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		Direct3D11RuntimeLinking();

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Direct3D11RuntimeLinking();

		/**
		*  @brief
		*    Return whether or not Direct3D 11 is available
		*
		*  @return
		*    "true" if Direct3D 11 is available, else "false"
		*/
		bool isDirect3D11Avaiable();


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


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		void* mD3D11SharedLibrary;			///< D3D11 shared library, can be a null pointer
		void* mD3DX11SharedLibrary;			///< D3DX11 shared library, can be a null pointer
		void* mD3DCompilerSharedLibrary;	///< D3DCompiler shared library, can be a null pointer
		bool  mEntryPointsRegistered;		///< Entry points successfully registered?
		bool  mInitialized;					///< Already initialized?


	};


	//[-------------------------------------------------------]
	//[ D3D11 core functions                                  ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D11_DEFINERUNTIMELINKING
		#define FNDEF_D3D11(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3D11(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3D11(HRESULT,	D3D11CreateDevice,	(__in_opt IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, __in_ecount_opt( FeatureLevels ) CONST D3D_FEATURE_LEVEL*, UINT, UINT, __out_opt ID3D11Device**, __out_opt D3D_FEATURE_LEVEL*, __out_opt ID3D11DeviceContext**));


	//[-------------------------------------------------------]
	//[ D3DX11 functions                                      ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D11_DEFINERUNTIMELINKING
		#define FNDEF_D3DX11(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3DX11(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3DX11(HRESULT,	D3DX11CompileFromMemory,	(LPCSTR, SIZE_T, LPCSTR, CONST D3D10_SHADER_MACRO*, LPD3D10INCLUDE, LPCSTR, LPCSTR, UINT, UINT, ID3DX11ThreadPump*, ID3D10Blob**, ID3D10Blob**, HRESULT*));
	FNDEF_D3DX11(HRESULT,	D3DX11FilterTexture,		(ID3D11DeviceContext*, ID3D11Resource*, UINT, UINT));


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
	FNDEF_D3DX11(HRESULT,	D3DCreateBlob,				(SIZE_T Size, ID3DBlob** ppBlob));


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif

	// Redirect D3D11* and D3DX11* function calls to funcPtr_D3D11* and funcPtr_D3DX11*

	// D3D11
	#define D3D11CreateDevice	FNPTR(D3D11CreateDevice)

	// D3DX11
	#define D3DX11CompileFromMemory	FNPTR(D3DX11CompileFromMemory)
	#define D3DX11FilterTexture		FNPTR(D3DX11FilterTexture)

	// D3DCompiler
	#define D3DCreateBlob	FNPTR(D3DCreateBlob)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
