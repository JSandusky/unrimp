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

#include "OpenGLRenderer/CgRuntimeLinking.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_PRINTF()"
#ifdef WIN32
	#include <Renderer/WindowsHeader.h>
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
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
		mCgGLSharedLibrary(nullptr),
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
			if (nullptr != mCgGLSharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mCgGLSharedLibrary));
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
				// Load the Cg and Cg OpenGL entry points
				mEntryPointsRegistered = (loadCgEntryPoints() && loadCgGLEntryPoints());
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
				mCgGLSharedLibrary = ::LoadLibraryExA("cgGL.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr == mCgGLSharedLibrary)
				{
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Failed to load in the shared library \"cgGL.dll\"\n")
				}
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Failed to load in the shared library \"cg.dll\"\n")
			}
		#else
			#error "Unsupported platform"
		#endif

		// Done
		return (nullptr != mCgSharedLibrary && nullptr != mCgGLSharedLibrary);
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
			#define IMPORT_FUNC(funcName)																																		\
				if (result)																																						\
				{																																								\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mCgSharedLibrary), #funcName);																			\
					if (nullptr != symbol)																																		\
					{																																							\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																										\
					}																																							\
					else																																						\
					{																																							\
						wchar_t moduleFilename[MAX_PATH];																														\
						moduleFilename[0] = '\0';																																\
						::GetModuleFileNameW(static_cast<HMODULE>(mCgSharedLibrary), moduleFilename, MAX_PATH);																	\
						RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: Failed to locate the entry point \"%s\" within the Cg shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																			\
					}																																							\
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
		IMPORT_FUNC(cgGetParameterBufferIndex);
		IMPORT_FUNC(cgGetParameterClass);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}

	/**
	*  @brief
	*    Load the Cg OpenGL entry points
	*/
	bool CgRuntimeLinking::loadCgGLEntryPoints()
	{
		bool result = true;	// Success by default

		// Define a helper macro
		#ifdef WIN32
			#define IMPORT_FUNC(funcName)																																		\
				if (result)																																						\
				{																																								\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mCgGLSharedLibrary), #funcName);																		\
					if (nullptr != symbol)																																		\
					{																																							\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																										\
					}																																							\
					else																																						\
					{																																							\
						wchar_t moduleFilename[MAX_PATH];																														\
						moduleFilename[0] = '\0';																																\
						::GetModuleFileNameW(static_cast<HMODULE>(mCgGLSharedLibrary), moduleFilename, MAX_PATH);																\
						RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: Failed to locate the entry point \"%s\" within the CgGL shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																			\
					}																																							\
				}
		#else
			#error "Unsupported platform"
		#endif

		// Load the entry points
		IMPORT_FUNC(cgGLIsProfileSupported);
		IMPORT_FUNC(cgGLGetLatestProfile);
		IMPORT_FUNC(cgGLBindProgram);
		IMPORT_FUNC(cgGLEnableProfile);
		IMPORT_FUNC(cgGLLoadProgram);
		IMPORT_FUNC(cgGLGetProgramID);
		IMPORT_FUNC(cgGLSetParameterPointer);
		IMPORT_FUNC(cgGLEnableClientState);
		IMPORT_FUNC(cgGLDisableClientState);
		IMPORT_FUNC(cgGLGetTextureEnum);

		// Undefine the helper macro
		#undef IMPORT_FUNC

		// Done
		return result;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
