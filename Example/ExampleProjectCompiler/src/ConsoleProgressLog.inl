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
#include <iostream>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
inline ConsoleProgressLog::ConsoleProgressLog()
{
	// Nothing here
}

inline ConsoleProgressLog::~ConsoleProgressLog()
{
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual RendererToolkit::IProgressLog methods  ]
//[-------------------------------------------------------]
inline void ConsoleProgressLog::print(const char* format, ...)
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
		printInternal(formattedText, textLength, false);
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
		printInternal(formattedText, textLength, false);

		// Cleanup
		delete [] formattedText;
	}
}

inline void ConsoleProgressLog::printLine(const char* format, ...)
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
		printInternal(formattedText, textLength, true);
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
		printInternal(formattedText, textLength, true);

		// Cleanup
		delete [] formattedText;
	}
}

inline void ConsoleProgressLog::printProgress(uint32_t current, uint32_t max)
{
	std::lock_guard<std::mutex> mutexLock(mMutex);
	std::cout<<"\rProgress: "<<current<<'/'<<max;
	std::cout.flush();
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
inline void ConsoleProgressLog::printInternal(const char* message, uint32_t numberOfCharacters, bool addNewLine)
{
	std::lock_guard<std::mutex> mutexLock(mMutex);
	if (addNewLine)
	{
		std::cout<<message<<'\n';
	}
	else
	{
		std::cout<<message;
	}

}
