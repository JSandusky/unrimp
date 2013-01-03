/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#define CG_DEFINERUNTIMELINKING

#include "Direct3D9Renderer/CgRuntimeLinking.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_PRINTF()"
#ifdef WIN32
	#include <Renderer/WindowsHeader.h>
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Default constructor
	*/
	CgRuntimeLinking::CgRuntimeLinking() :
		mCgSharedLibrary(nullptr),
		mCgD3D9SharedLibrary(nullptr),
		mEntryPointsRegistered(false),
		mInitialized(false)
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Destructor
	*/
	CgRuntimeLinking::~CgRuntimeLinking()
	{
		// Destroy the shared library instances
		#ifdef WIN32
			if (nullptr != mCgD3D9SharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mCgD3D9SharedLibrary));
			}
			if (nullptr != mCgSharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mCgSharedLibrary));
			}
		#else
			#error "Unsupported platform"
		#endif
	}

	/**
	*  @brief
	*    Return whether or not Cg is available
	*/
	bool CgRuntimeLinking::isCgAvaiable()
	{
		// Already initialized?
		if (!mInitialized)
		{
			// We're now initialized
			mInitialized = true;

			// Load the shared libraries
			if (loadSharedLibraries())
			{
				// Load the Cg and Cg Direct3D 9 entry points
				mEntryPointsRegistered = (loadCgEntryPoints() && loadCgD3D9EntryPoints());
			}
		}

		// Entry points successfully registered?
		return mEntryPointsRegistered;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Load the shared libraries
	*/
	bool CgRuntimeLinking::loadSharedLibraries()
	{
		// Load the shared library
		#ifdef WIN32
			mCgSharedLibrary = ::LoadLibraryExA("cg.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (nullptr != mCgSharedLibrary)
			{
				mCgD3D9SharedLibrary = ::LoadLibraryExA("cgD3D9.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr == mCgD3D9SharedLibrary)
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 error: Failed to load in the shared library \"cgD3D9.dll\"\n")
				}
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 error: Failed to load in the shared library \"cg.dll\"\n")
			}
		#else
			#error "Unsupported platform"
		#endif

		// Done
		return (nullptr != mCgSharedLibrary && nullptr != mCgD3D9SharedLibrary);
	}

	/**
	*  @brief
	*    Load the Cg entry points
	*/
	bool CgRuntimeLinking::loadCgEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																																			\
				if (result)																																							\
				{																																									\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mCgSharedLibrary), #funcName);																				\
					if (nullptr != symbol)																																			\
					{																																								\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																											\
					}																																								\
					else																																							\
					{																																								\
						wchar_t moduleFilename[MAX_PATH];																															\
						moduleFilename[0] = '\0';																																	\
						::GetModuleFileNameW(static_cast<HMODULE>(mCgSharedLibrary), moduleFilename, MAX_PATH);																		\
						RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 9 error: Failed to locate the entry point \"%s\" within the Cg shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																				\
					}																																								\
				}
		#else
			#error "Unsupported platform"
		#endif

		// Load the entry points
		IMPORT_FUNC(cgCreateContext);
		IMPORT_FUNC(cgDestroyContext);
		IMPORT_FUNC(cgSetErrorCallback);
		IMPORT_FUNC(cgGetError);
		IMPORT_FUNC(cgGetErrorString);
		IMPORT_FUNC(cgDestroyProgram);
		IMPORT_FUNC(cgGetProfile);
		IMPORT_FUNC(cgGetProfileProperty);
		IMPORT_FUNC(cgCreateProgram);
		IMPORT_FUNC(cgCombinePrograms);
		IMPORT_FUNC(cgGetNumProgramDomains);
		IMPORT_FUNC(cgGetProgramDomainProfile);
		IMPORT_FUNC(cgGetNamedParameter);
		IMPORT_FUNC(cgSetParameter1f);
		IMPORT_FUNC(cgSetParameter2fv);
		IMPORT_FUNC(cgSetParameter3fv);
		IMPORT_FUNC(cgSetParameter4fv);
		IMPORT_FUNC(cgSetMatrixParameterfc);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	/**
	*  @brief
	*    Load the Cg Direct3D 9 entry points
	*/
	bool CgRuntimeLinking::loadCgD3D9EntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																																				\
				if (result)																																								\
				{																																										\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mCgD3D9SharedLibrary), #funcName);																				\
					if (nullptr != symbol)																																				\
					{																																									\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																												\
					}																																									\
					else																																								\
					{																																									\
						wchar_t moduleFilename[MAX_PATH];																																\
						moduleFilename[0] = '\0';																																		\
						::GetModuleFileNameW(static_cast<HMODULE>(mCgD3D9SharedLibrary), moduleFilename, MAX_PATH);																		\
						RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 9 error: Failed to locate the entry point \"%s\" within the CgD3D9 shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																					\
					}																																									\
				}
		#else
			#error "Unsupported platform"
		#endif

		// Load the entry points
		IMPORT_FUNC(cgD3D9GetLatestVertexProfile);
		IMPORT_FUNC(cgD3D9GetLatestPixelProfile);
		IMPORT_FUNC(cgD3D9SetDevice);
		IMPORT_FUNC(cgD3D9LoadProgram);
		IMPORT_FUNC(cgD3D9BindProgram);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
