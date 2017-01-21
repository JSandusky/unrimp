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
#include "Direct3D10Renderer/D3D10.h"


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
		*    Default constructor
		*/
		Direct3D9RuntimeLinking();

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Direct3D9RuntimeLinking();

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
		/**
		*  @brief
		*    Load the shared library
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadSharedLibrary();

		/**
		*  @brief
		*    Load the D3D9 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadD3D9EntryPoints();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		void *mD3D9SharedLibrary;		///< D3D9 shared library, can be a null pointer
		bool  mEntryPointsRegistered;	///< Entry points successfully registered?
		bool  mInitialized;				///< Already initialized?


	};


	//[-------------------------------------------------------]
	//[ D3D9 core functions                                   ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D9_DEFINERUNTIMELINKING
		#define FNDEF_D3D9(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3D9(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	// Debug
	FNDEF_D3D9(DWORD,	D3DPERF_GetStatus,	(void));
	FNDEF_D3D9(void,	D3DPERF_SetOptions,	(DWORD));
	FNDEF_D3D9(void,	D3DPERF_SetMarker,	(D3DCOLOR, LPCWSTR));
	FNDEF_D3D9(int,		D3DPERF_BeginEvent,	(D3DCOLOR, LPCWSTR));
	FNDEF_D3D9(int,		D3DPERF_EndEvent,	(void));


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif

	// Redirect D3D9* function calls to funcPtr_D3D9*

	// Debug
	#define D3DPERF_GetStatus	FNPTR(D3DPERF_GetStatus)
	#define D3DPERF_SetOptions	FNPTR(D3DPERF_SetOptions)
	#define D3DPERF_SetMarker	FNPTR(D3DPERF_SetMarker)
	#define D3DPERF_BeginEvent	FNPTR(D3DPERF_BeginEvent)
	#define D3DPERF_EndEvent	FNPTR(D3DPERF_EndEvent)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
