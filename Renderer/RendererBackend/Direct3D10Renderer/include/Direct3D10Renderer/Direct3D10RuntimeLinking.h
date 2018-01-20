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
#include "Direct3D10Renderer/D3D10.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{
	class Direct3D10Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 10 runtime linking
	*/
	class Direct3D10RuntimeLinking final
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D10Renderer
		*    Owner Direct3D 10 renderer instance
		*/
		explicit Direct3D10RuntimeLinking(Direct3D10Renderer& direct3D10Renderer);

		/**
		*  @brief
		*    Destructor
		*/
		~Direct3D10RuntimeLinking();

		/**
		*  @brief
		*    Return whether or not Direct3D 10 is available
		*
		*  @return
		*    "true" if Direct3D 10 is available, else "false"
		*/
		bool isDirect3D10Avaiable();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit Direct3D10RuntimeLinking(const Direct3D10RuntimeLinking& source) = delete;
		Direct3D10RuntimeLinking& operator =(const Direct3D10RuntimeLinking& source) = delete;

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
		*    Load the D3D10 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadD3D10EntryPoints();

		/**
		*  @brief
		*    Load the D3DX10 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadD3DX10EntryPoints();

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
		Direct3D10Renderer&	mDirect3D10Renderer;		///< Owner Direct3D 10 renderer instance
		void*				mD3D10SharedLibrary;		///< D3D10 shared library, can be a null pointer
		void*				mD3DX10SharedLibrary;		///< D3DX10 shared library, can be a null pointer
		void*				mD3DCompilerSharedLibrary;	///< D3DCompiler shared library, can be a null pointer
		bool				mEntryPointsRegistered;		///< Entry points successfully registered?
		bool				mInitialized;				///< Already initialized?


	};


	//[-------------------------------------------------------]
	//[ D3D10 core functions                                  ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D10_DEFINERUNTIMELINKING
		#define FNDEF_D3D10(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3D10(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3D10(HRESULT,	D3D10CreateDevice,	(IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, ID3D10Device**));


	//[-------------------------------------------------------]
	//[ D3DX10 functions                                      ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D10_DEFINERUNTIMELINKING
		#define FNDEF_D3DX10(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3DX10(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3DX10(HRESULT,	D3DX10FilterTexture,	(ID3D10Resource*, UINT, UINT));


	//[-------------------------------------------------------]
	//[ D3DCompiler functions                                 ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D10_DEFINERUNTIMELINKING
		#define FNDEF_D3DX10(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3DX10(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	typedef __interface ID3D10Blob *LPD3D10BLOB;	// "__interface" is no keyword of the ISO C++ standard, shouldn't be a problem because this in here is MS Windows only and it's also within the Direct3D headers we have to use
	typedef ID3D10Blob ID3DBlob;
	FNDEF_D3DX10(HRESULT,	D3DCompile,		(LPCVOID, SIZE_T, LPCSTR, CONST D3D_SHADER_MACRO*, ID3DInclude*, LPCSTR, LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**));
	FNDEF_D3DX10(HRESULT,	D3DCreateBlob,	(SIZE_T Size, ID3DBlob** ppBlob));


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif

	// Redirect D3D10* and D3DX10* function calls to funcPtr_D3D10* and funcPtr_D3DX10*

	// D3D10
	#define D3D10CreateDevice	FNPTR(D3D10CreateDevice)
	#define D3D10CreateBlob		FNPTR(D3D10CreateBlob)

	// D3DX10
	#define D3DX10FilterTexture		FNPTR(D3DX10FilterTexture)

	// D3DCompiler
	#define D3DCompile		FNPTR(D3DCompile)
	#define D3DCreateBlob	FNPTR(D3DCreateBlob)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
