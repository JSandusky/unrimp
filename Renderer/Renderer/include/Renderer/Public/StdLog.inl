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
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to '::size_t', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'std::codecvt_base': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::codecvt_base': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4774)	// warning C4774: 'sprintf_s' : format string expected in argument 3 is not a string literal
	#include <iostream>
	#include <string>
PRAGMA_WARNING_POP

#ifdef WIN32
	// Disable warnings in external headers, we can't fix them
	PRAGMA_WARNING_PUSH
		PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'const char' to 'utf8::uint8_t', signed/unsigned mismatch
		PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
		PRAGMA_WARNING_DISABLE_MSVC(5039)	// warning C5039: 'TpSetCallbackCleanupGroup': pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
		#include <utf8/utf8.h>	// To convert UTF-8 strings to UTF-16

		#include <windows.h>

		// Get rid of some nasty OS macros
		#undef max
	PRAGMA_WARNING_POP
#elif LINUX
	// Nothing here
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
	inline bool StdLog::print(Type type, const char* attachment, const char* file, uint32_t line, const char* format, ...)
	{
		bool requestDebugBreak = false;

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
			requestDebugBreak = printInternal(type, attachment, file, line, formattedText, textLength);
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
			requestDebugBreak = printInternal(type, attachment, file, line, formattedText, textLength);

			// Cleanup
			delete [] formattedText;
		}

		// Done
		return requestDebugBreak;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::StdLog methods            ]
	//[-------------------------------------------------------]
	inline bool StdLog::printInternal(Type type, const char*, const char* file, uint32_t line, const char* message, uint32_t)
	{
		std::lock_guard<std::mutex> mutexLock(mMutex);
		bool requestDebugBreak = false;

		// Construct the full UTF-8 message text
		#ifdef RENDERER_NO_DEBUG
			std::ignore = file;
			std::ignore = line;
			std::string fullMessage = std::string(typeToString(type)) + message;
		#else
			std::string fullMessage = "File \"" + std::string(file) + "\" | Line " + std::to_string(line) + " | " + std::string(typeToString(type)) + message;
		#endif
		if ('\n' != fullMessage.back())
		{
			fullMessage += '\n';
		}

		// Platform specific handling
		#ifdef WIN32
		{
			// Convert UTF-8 string to UTF-16
			std::wstring utf16Line;
			utf8::utf8to16(fullMessage.begin(), fullMessage.end(), std::back_inserter(utf16Line));

			// Write into standard output stream
			if (Type::CRITICAL == type)
			{
				std::wcerr << utf16Line.c_str();
			}
			else
			{
				std::wcout << utf16Line.c_str();
			}

			// On MS Windows, ensure the output can be seen inside the Visual Studio output window as well
			::OutputDebugStringW(utf16Line.c_str());
			if (Type::CRITICAL == type && ::IsDebuggerPresent())
			{
				requestDebugBreak = true;
			}
		}
		#elif LINUX
			// Write into standard output stream
			if (Type::CRITICAL == type)
			{
				std::cerr << fullMessage.c_str();
			}
			else
			{
				std::cout << fullMessage.c_str();
			}
		#else
			#error "Unsupported platform"
		#endif

		// Done
		return requestDebugBreak;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
