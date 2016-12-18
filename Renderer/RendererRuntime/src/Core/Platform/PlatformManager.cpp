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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Core/Platform/PlatformManager.h"
#ifdef WIN32
	#include "RendererRuntime/Core/Platform/WindowsHeader.h"
#elif LINUX
	#include <sys/prctl.h>
#endif

#include <inttypes.h>	// For uint32_t, uint64_t etc.


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
		// "prctl()" and "pthread_setname_np()" support only up to 16 characters (including the terminating zero), so this is our limiting factor
		assert((strlen(shortName) + 1) < 16);	// +1 for the terminating zero
		assert(strlen(descriptiveName) >= strlen(shortName));

		// Platform specific part
		#ifdef WIN32
			// Ignore short name
			::detail::setThreadName(::GetCurrentThreadId(), descriptiveName);
		#elif LINUX
			// Ignore descriptive name
			#warning "Linux: RendererRuntime::PlatformManager::setCurrentThreadName() is untested"	// TODO(co) Not tested
			::prctl(PR_SET_NAME, shortName, 0, 0, 0);
		#elif __APPLE__
			// Ignore descriptive name
			#warning "Mac OS X: RendererRuntime::PlatformManager::setCurrentThreadName() is untested"	// TODO(co) Not tested
			::pthread_setname_np(shortName);
		#else
			#error "Unsupported platform"
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
