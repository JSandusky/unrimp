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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Core/Platform/PlatformManager.h"
#ifdef WIN32
	#include "RendererRuntime/Core/Platform/WindowsHeader.h"

	// Disable warnings in external headers, we can't fix them
	PRAGMA_WARNING_PUSH
		PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'const char' to 'utf8::uint8_t', signed/unsigned mismatch
		#include <utf8/utf8.h>	// To convert UTF-8 strings to UTF-16
	PRAGMA_WARNING_POP
#elif LINUX
	#include <sys/prctl.h>
#endif

#include <tuple>
#include <cassert>
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <inttypes.h>	// For uint32_t, uint64_t etc.
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Windows                                               ]
		//[-------------------------------------------------------]
		#ifdef WIN32
			const DWORD MS_VC_EXCEPTION = 0x406D1388;

			#pragma pack(push, 8)
				typedef struct tagTHREADNAME_INFO
				{
					DWORD  dwType;		///< Must be 0x1000
					LPCSTR szName;		///< Pointer to name (in user address space)
					DWORD  dwThreadID;	///< Thread ID (-1 = caller thread)
					DWORD  dwFlags;		///< Reserved for future use, must be zero
				} THREADNAME_INFO;
			#pragma pack(pop)

			void setThreadName(uint32_t dwThreadID, const char* name)
			{
				THREADNAME_INFO info;
				info.dwType		= 0x1000;
				info.szName		= name;
				info.dwThreadID = dwThreadID;
				info.dwFlags	= 0;
				__try
				{
					::RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					// Nothing here
				}
			}
		#elif LINUX
			// Nothing special here
		#else
			#error "Unsupported platform"
		#endif


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	void PlatformManager::setCurrentThreadName(const char* shortName, const char* descriptiveName)
	{
		// "pthread_setname_np()" support only up to 16 characters (including the terminating zero), so this is our limiting factor
		assert((strlen(shortName) + 1) <= 16);	// +1 for the terminating zero
		assert(strlen(descriptiveName) >= strlen(shortName));

		// Platform specific part
		#ifdef WIN32
			std::ignore = shortName;
			::detail::setThreadName(::GetCurrentThreadId(), descriptiveName);
		#elif LINUX
			std::ignore = descriptiveName;
			::pthread_setname_np(pthread_self(), shortName);
		#elif __APPLE__
			std::ignore = descriptiveName;
			#warning "Mac OS X: RendererRuntime::PlatformManager::setCurrentThreadName() is untested"	// TODO(co) Not tested
			::pthread_setname_np(shortName);
		#else
			#error "Unsupported platform"
		#endif
	}

	bool PlatformManager::execute(const char* command, const char* parameters, AbsoluteDirectoryName workingDirectory)
	{
		// Sanity checks
		assert(nullptr != command);
		assert(strlen(command) != 0);
		assert(nullptr != parameters);
		assert(nullptr != workingDirectory);

		// Platform specific part
		#ifdef WIN32
			// Get UTF-16 command string
			std::string stdString = command;
			std::wstring utf16Command;
			utf8::utf8to16(stdString.begin(), stdString.end(), std::back_inserter(utf16Command));

			// Get UTF-16 parameters string
			stdString = parameters;
			std::wstring utf16Parameters;
			utf8::utf8to16(stdString.begin(), stdString.end(), std::back_inserter(utf16Parameters));

			// Get UTF-16 working directory string
			stdString = workingDirectory;
			std::wstring utf16WorkingDirectory;
			utf8::utf8to16(stdString.begin(), stdString.end(), std::back_inserter(utf16WorkingDirectory));

			// Execute command
			const HINSTANCE result = ::ShellExecuteW(nullptr, L"open", utf16Command.c_str(), utf16Parameters.c_str(), utf16WorkingDirectory.c_str(), SW_SHOWDEFAULT);

			// Has the execution been successful?
			return (result > HINSTANCE(32));
		#elif LINUX
			std::ignore = command;
			std::ignore = parameters;
			std::ignore = workingDirectory;
			#warning "RendererRuntime::PlatformManager::execute() isn't implemented"
		#else
			#error "Unsupported platform"
		#endif
	}

	bool PlatformManager::openUrlExternal(const char* url)
	{
		// Sanity checks
		assert(nullptr != url);
		assert(strlen(url) != 0);

		// Platform specific part
		#ifdef WIN32
			// Execute command
			return execute("explorer", url);
		#elif LINUX
			std::ignore = url;
			#warning "RendererRuntime::PlatformManager::openUrlExternal() isn't implemented"
		#else
			#error "Unsupported platform"
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
