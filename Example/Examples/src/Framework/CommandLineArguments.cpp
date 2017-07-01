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
#include "PrecompiledHeader.h"
#include "CommandLineArguments.h"
#ifdef WIN32
	#include "WindowsHeader.h"
	#ifdef UNICODE
		#include "utf8/utf8.h"	// To convert utf16 strings to utf8
	#else
		#include <sstream>
		#include <iterator>
		#include <algorithm>
	#endif
#endif


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
CommandLineArguments::CommandLineArguments()
{
#if WIN32
	#ifdef UNICODE
		int wargc = 0;
		wchar_t** wargv = ::CommandLineToArgvW(GetCommandLineW(), &wargc);
		if (wargc > 0)
		{
			// argv[0] is the path+name of the program
			// -> Ignore it
			mArguments.reserve(wargc - 1);
			std::vector<std::wstring> lines(wargv + 1, wargv + wargc);
			for (std::vector<std::wstring>::iterator iterator = lines.begin(); iterator != lines.end(); ++iterator)
			{
				std::string utf8line;
				utf8::utf16to8((*iterator).begin(), (*iterator).end(), std::back_inserter(utf8line));
				mArguments.push_back(utf8line);
			}
			
		}
		LocalFree(wargv);
	#else
		std::string cmdLine(::GetCommandLineA());
		std::istringstream ss(cmdLine);
		std::istream_iterator<std::string> iss(ss);

		// The first token is the path+name of the program
		// -> Ignore it
		++iss;
		std::copy(iss,
			 std::istream_iterator<std::string>(),
			 std::back_inserter<std::vector<std::string>>(mArguments));
	#endif
#endif
}
