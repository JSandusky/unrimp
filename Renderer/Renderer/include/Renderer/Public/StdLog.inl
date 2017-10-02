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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <cstdarg>

#ifdef WIN32
	// Disable warnings in external headers, we can't fix them
	PRAGMA_WARNING_PUSH
		PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
		#include <windows.h>
	PRAGMA_WARNING_POP
#elif LINUX
	#include <iostream>
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline StdLog::StdLog()
	{
		// Nothing here
	}

	inline StdLog::~StdLog()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ILog methods                 ]
	//[-------------------------------------------------------]
	inline void StdLog::print(Type type, const char* format, ...)
	{
		// Get the required buffer length, does not include the terminating zero character
		va_list vaList;
		va_start(vaList, format);
		const uint32_t textLength = static_cast<uint32_t>(vsnprintf(nullptr, 0, format, vaList));
		va_end(vaList);
		if (256 > textLength)
		{
			// Fast path: C-runtime stack

			// Construct the formatted text
			char formattedText[256];	// 255 +1 for the terminating zero character
			va_start(vaList, format);
			vsnprintf(formattedText, 256, format, vaList);
			va_end(vaList);

			// Internal processing
			printInternal(type, formattedText, textLength);
		}
		else
		{
			// Slow path: Heap
			// -> No reused scratch buffer in order to reduce memory allocation/deallocations in here to not make things more complex and to reduce the mutex locked scope

			// Construct the formatted text
			char* formattedText = new char[textLength + 1];	// 1+ for the terminating zero character
			va_start(vaList, format);
			vsnprintf(formattedText, textLength + 1, format, vaList);
			va_end(vaList);

			// Internal processing
			printInternal(type, formattedText, textLength);

			// Cleanup
			delete [] formattedText;
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline const char* StdLog::typeToString(Type type) const
	{
		switch (type)
		{
			case Type::TRACE:
				return "Trace: ";

			case Type::DEBUG:
				return "Debug: ";

			case Type::INFORMATION:
				return "Information: ";

			case Type::WARNING:
				return "Warning: ";

			case Type::PERFORMANCE_WARNING:
				return "Performance warning: ";

			case Type::COMPATIBILITY_WARNING:
				return "Compatibility warning: ";

			case Type::CRITICAL:
				return "Critical: ";

			default:
				return "Unknown: ";
		}
	}

	inline void StdLog::printInternal(Type type, const char* message, uint32_t numberOfCharacters)
	{
		std::lock_guard<std::mutex> mutexLock(mMutex);

		// Platform specific handling
		#ifdef WIN32
			std::ignore = numberOfCharacters;
			::OutputDebugString(typeToString(type));
			::OutputDebugString(message);
			::OutputDebugString("\n");
			if (Type::CRITICAL == type && ::IsDebuggerPresent())
			{
				::__debugbreak();
			}
		#elif LINUX
			std::ignore = numberOfCharacters;
			if (Type::CRITICAL == type)
			{
				std::cerr << typeToString(type) << message << '\n';
			}
			else
			{
				std::cout << typeToString(type) << message << '\n';
			}
		#else
			#error "Unsupported platform"
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
