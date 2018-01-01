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
#include "Direct3D9Renderer/d3d9.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{
	class Direct3D9Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 9 runtime linking
	*/
	class Direct3D9RuntimeLinking
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D9Renderer
		*    Owner Direct3D 9 renderer instance
		*/
		explicit Direct3D9RuntimeLinking(Direct3D9Renderer& direct3D9Renderer);

		/**
		*  @brief
		*    Destructor
		*/
		~Direct3D9RuntimeLinking();

		/**
		*  @brief
		*    Return whether or not Direct3D 9 is available
		*
		*  @return
		*    "true" if Direct3D 9 is available, else "false"
		*/
		bool isDirect3D9Avaiable();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit Direct3D9RuntimeLinking(const Direct3D9RuntimeLinking& source) = delete;
		Direct3D9RuntimeLinking& operator =(const Direct3D9RuntimeLinking& source) = delete;

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
		*    Load the D3D9 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadD3D9EntryPoints();

		/**
		*  @brief
		*    Load the D3DX9 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadD3DX9EntryPoints();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Direct3D9Renderer& mDirect3D9Renderer;
		void*			   mD3D9SharedLibrary;		///< D3D9 shared library, can be a null pointer
		void*			   mD3DX9SharedLibrary;		///< D3DX9 shared library, can be a null pointer
		bool			   mEntryPointsRegistered;	///< Entry points successfully registered?
		bool			   mInitialized;			///< Already initialized?


	};


	//[-------------------------------------------------------]
	//[ D3D9 core functions                                   ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D9_DEFINERUNTIMELINKING
		#define FNDEF_D3D9(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3D9(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3D9(IDirect3D9*,	Direct3DCreate9,	(UINT));
	// Debug
	FNDEF_D3D9(DWORD,		D3DPERF_GetStatus,	(void));
	FNDEF_D3D9(void,		D3DPERF_SetOptions,	(DWORD));
	#ifdef RENDERER_DEBUG
		FNDEF_D3D9(void,	D3DPERF_SetMarker,	(D3DCOLOR, LPCWSTR));
		FNDEF_D3D9(int,		D3DPERF_BeginEvent,	(D3DCOLOR, LPCWSTR));
		FNDEF_D3D9(int,		D3DPERF_EndEvent,	(void));
	#endif


	//[-------------------------------------------------------]
	//[ D3DX9 functions                                       ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D9_DEFINERUNTIMELINKING
		#define FNDEF_D3DX9(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3DX9(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3DX9(HRESULT,	D3DXLoadSurfaceFromMemory,	(LPDIRECT3DSURFACE9, CONST PALETTEENTRY*, CONST RECT*, LPCVOID, D3DFORMAT, UINT, CONST PALETTEENTRY*, CONST RECT*, DWORD, D3DCOLOR));
	FNDEF_D3DX9(HRESULT,	D3DXCompileShader,			(LPCSTR, UINT, CONST D3DXMACRO*, LPD3DXINCLUDE, LPCSTR, LPCSTR, DWORD, LPD3DXBUFFER*, LPD3DXBUFFER*, LPD3DXCONSTANTTABLE*));
	FNDEF_D3DX9(HRESULT,	D3DXGetShaderConstantTable,	(const DWORD*, LPD3DXCONSTANTTABLE*));


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif

	// Redirect D3D9* and D3DX9* function calls to funcPtr_D3D9* and funcPtr_D3DX9*

	// D3D9
	#define Direct3DCreate9		FNPTR(Direct3DCreate9)
	// Debug
	#define D3DPERF_GetStatus		FNPTR(D3DPERF_GetStatus)
	#define D3DPERF_SetOptions		FNPTR(D3DPERF_SetOptions)
	#ifdef RENDERER_DEBUG
		#define D3DPERF_SetMarker	FNPTR(D3DPERF_SetMarker)
		#define D3DPERF_BeginEvent	FNPTR(D3DPERF_BeginEvent)
		#define D3DPERF_EndEvent	FNPTR(D3DPERF_EndEvent)
	#endif

	// D3DX9
	#define D3DXLoadSurfaceFromMemory	FNPTR(D3DXLoadSurfaceFromMemory)
	#define D3DXCompileShader			FNPTR(D3DXCompileShader)
	#define D3DXGetShaderConstantTable	FNPTR(D3DXGetShaderConstantTable)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
