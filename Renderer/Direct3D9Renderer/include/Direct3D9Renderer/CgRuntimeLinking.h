/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#ifndef __DIRECT3D9RENDERER_CGRUNTIMELINKING_H__
#define __DIRECT3D9RENDERER_CGRUNTIMELINKING_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Cg/cgD3D9.h>


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
	*    Cg runtime linking
	*/
	class CgRuntimeLinking
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		CgRuntimeLinking();

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~CgRuntimeLinking();

		/**
		*  @brief
		*    Return whether or not Cg is available
		*
		*  @return
		*    "true" if Cg is available, else "false"
		*/
		bool isCgAvaiable();


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
		*    Load the Cg entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadCgEntryPoints();

		/**
		*  @brief
		*    Load the Cg Direct3D 9 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadCgD3D9EntryPoints();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		void *mCgSharedLibrary;			///< Cg shared library, can be a null pointer
		void *mCgD3D9SharedLibrary;		///< Cg D3D9 shared library, can be a null pointer
		bool  mEntryPointsRegistered;	///< Entry points successfully registered?
		bool  mInitialized;				///< Already initialized?


	};


	//[-------------------------------------------------------]
	//[ Cg core functions                                     ]
	//[-------------------------------------------------------]
	#ifdef CG_DEFINERUNTIMELINKING
		#define FNDEF_CG(retType, funcName, args) retType (CGENTRY *funcPtr_##funcName) args
	#else
		#define FNDEF_CG(retType, funcName, args) extern retType (CGENTRY *funcPtr_##funcName) args
	#endif
	FNDEF_CG(CGcontext,		cgCreateContext,			(void));
	FNDEF_CG(void,			cgDestroyContext,			(CGcontext));
	FNDEF_CG(void,			cgSetErrorCallback,			(CGerrorCallbackFunc));
	FNDEF_CG(CGerror,		cgGetError,					(void));
	FNDEF_CG(const char *,	cgGetErrorString,			(CGerror));
	FNDEF_CG(void,			cgDestroyProgram,			(CGprogram));
	FNDEF_CG(CGprofile,		cgGetProfile,				(const char *));
	FNDEF_CG(CGbool,		cgGetProfileProperty,		(CGprofile, CGenum));
	FNDEF_CG(CGprogram,		cgCreateProgram,			(CGcontext, CGenum, const char *, CGprofile, const char *, const char **));
	FNDEF_CG(CGprogram,		cgCombinePrograms,			(int, const CGprogram *));
	FNDEF_CG(int,			cgGetNumProgramDomains,		(CGprogram));
	FNDEF_CG(CGprofile,		cgGetProgramDomainProfile,	(CGprogram, int));
	FNDEF_CG(CGparameter,	cgGetNamedParameter,		(CGprogram, const char *));
	FNDEF_CG(void,			cgSetParameter1f,			(CGparameter, float));
	FNDEF_CG(void,			cgSetParameter2fv,			(CGparameter, const float *));
	FNDEF_CG(void,			cgSetParameter3fv,			(CGparameter, const float *));
	FNDEF_CG(void,			cgSetParameter4fv,			(CGparameter, const float *));
	FNDEF_CG(void,			cgSetMatrixParameterfc,		(CGparameter, const float *));


	//[-------------------------------------------------------]
	//[ Cg Direct3D 9 functions                               ]
	//[-------------------------------------------------------]
	#ifdef CG_DEFINERUNTIMELINKING
		#define FNDEF_CGD3D9(retType, funcName, args) retType (CGD3D9ENTRY *funcPtr_##funcName) args
	#else
		#define FNDEF_CGD3D9(retType, funcName, args) extern retType (CGD3D9ENTRY *funcPtr_##funcName) args
	#endif
	FNDEF_CGD3D9(CGprofile,	cgD3D9GetLatestVertexProfile,	(void));
	FNDEF_CGD3D9(CGprofile,	cgD3D9GetLatestPixelProfile,	(void));
	FNDEF_CGD3D9(HRESULT,	cgD3D9SetDevice,				(IDirect3DDevice9 *));
	FNDEF_CGD3D9(HRESULT,	cgD3D9LoadProgram,				(CGprogram, CGbool, DWORD));
	FNDEF_CGD3D9(HRESULT,	cgD3D9BindProgram,				(CGprogram));


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif

	// Redirect cg* and cgD3D9* function calls to funcPtr_cg* and funcPtr_cgD3D9*

	// Cg - Core
	#define cgCreateContext				FNPTR(cgCreateContext)
	#define cgDestroyContext			FNPTR(cgDestroyContext)
	#define cgSetErrorCallback			FNPTR(cgSetErrorCallback)
	#define cgGetError					FNPTR(cgGetError)
	#define cgGetErrorString			FNPTR(cgGetErrorString)
	#define cgDestroyProgram			FNPTR(cgDestroyProgram)
	#define cgGetProfile				FNPTR(cgGetProfile)
	#define cgGetProfileProperty		FNPTR(cgGetProfileProperty)
	#define cgCreateProgram				FNPTR(cgCreateProgram)
	#define cgCombinePrograms			FNPTR(cgCombinePrograms)
	#define cgGetNumProgramDomains		FNPTR(cgGetNumProgramDomains)
	#define cgGetProgramDomainProfile	FNPTR(cgGetProgramDomainProfile)
	#define cgGetNamedParameter			FNPTR(cgGetNamedParameter)
	#define cgSetParameter1f			FNPTR(cgSetParameter1f)
	#define cgSetParameter2fv			FNPTR(cgSetParameter2fv)
	#define cgSetParameter3fv			FNPTR(cgSetParameter3fv)
	#define cgSetParameter4fv			FNPTR(cgSetParameter4fv)
	#define cgSetMatrixParameterfc		FNPTR(cgSetMatrixParameterfc)

	// Cg - Direct3D 9
	#define cgD3D9GetLatestVertexProfile	FNPTR(cgD3D9GetLatestVertexProfile)
	#define cgD3D9GetLatestPixelProfile		FNPTR(cgD3D9GetLatestPixelProfile)
	#define cgD3D9SetDevice					FNPTR(cgD3D9SetDevice)
	#define cgD3D9LoadProgram				FNPTR(cgD3D9LoadProgram)
	#define cgD3D9BindProgram				FNPTR(cgD3D9BindProgram)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D9RENDERER_CGRUNTIMELINKING_H__
