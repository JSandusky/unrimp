/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#ifndef __DIRECT3D10RENDERER_CGRUNTIMELINKING_H__
#define __DIRECT3D10RENDERER_CGRUNTIMELINKING_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
__pragma(warning(push))
	__pragma(warning(disable: 4668))	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <Cg/cgD3D10.h>
__pragma(warning(pop))


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
		*    Load the Cg Direct3D 10 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadCgD3D10EntryPoints();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		void *mCgSharedLibrary;			///< Cg shared library, can be a null pointer
		void *mCgD3D10SharedLibrary;	///< Cg D3D10 shared library, can be a null pointer
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
	FNDEF_CG(int,			cgGetParameterBufferIndex,	(CGparameter));


	//[-------------------------------------------------------]
	//[ Cg Direct3D 10 functions                               ]
	//[-------------------------------------------------------]
	#ifdef CG_DEFINERUNTIMELINKING
		#define FNDEF_CGD3D10(retType, funcName, args) retType (CGD3D10ENTRY *funcPtr_##funcName) args
	#else
		#define FNDEF_CGD3D10(retType, funcName, args) extern retType (CGD3D10ENTRY *funcPtr_##funcName) args
	#endif
	FNDEF_CGD3D10(CGprofile,	cgD3D10GetLatestVertexProfile,		(void));
	FNDEF_CGD3D10(CGprofile,	cgD3D10GetLatestGeometryProfile,	(void));
	FNDEF_CGD3D10(CGprofile,	cgD3D10GetLatestPixelProfile,		(void));
	FNDEF_CGD3D10(HRESULT,		cgD3D10SetDevice,					(CGcontext, ID3D10Device *));
	FNDEF_CGD3D10(HRESULT,		cgD3D10LoadProgram,					(CGprogram, UINT));
	FNDEF_CGD3D10(HRESULT,		cgD3D10BindProgram,					(CGprogram));
	FNDEF_CGD3D10(ID3D10Blob *,	cgD3D10GetCompiledProgram,			(CGprogram));


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif

	// Redirect cg* and cgD3D10* function calls to funcPtr_cg* and funcPtr_cgD3D10*

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
	#define cgGetParameterBufferIndex	FNPTR(cgGetParameterBufferIndex)

	// Cg - Direct3D 10
	#define cgD3D10GetLatestVertexProfile	FNPTR(cgD3D10GetLatestVertexProfile)
	#define cgD3D10GetLatestGeometryProfile	FNPTR(cgD3D10GetLatestGeometryProfile)
	#define cgD3D10GetLatestPixelProfile	FNPTR(cgD3D10GetLatestPixelProfile)
	#define cgD3D10SetDevice				FNPTR(cgD3D10SetDevice)
	#define cgD3D10LoadProgram				FNPTR(cgD3D10LoadProgram)
	#define cgD3D10BindProgram				FNPTR(cgD3D10BindProgram)
	#define cgD3D10GetCompiledProgram		FNPTR(cgD3D10GetCompiledProgram)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D10RENDERER_CGRUNTIMELINKING_H__
