/*********************************************************\
 * Copyright (c) 2013-2013 Stephan Wezel
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
#include "CmdLineArgs.h"
#ifdef WIN32
	#include <windows.h>
#ifdef UNICODE
	#include "utf8/utf8.h" // to convert utf16 strings to utf8
#else
	#include <sstream>
	#include <algorithm>
	#include <iterator>
#endif
#endif


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
CmdLineArgs::CmdLineArgs()
{
#if WIN32
	#ifdef UNICODE
		int wargc;
		wchar_t **wargv = CommandLineToArgvW(GetCommandLineW(),&wargc);
		if (wargc > 0) {			
			// argv[0] is the path+name of the programm
			// -> ignore it
			m_args.reserve(wargc-1);
			std::vector<std::wstring> lines(wargv+1, wargv+wargc);
			for(std::vector<std::wstring>::iterator wit = lines.begin(); wit != lines.end(); ++wit)
			{
				std::string utf8line; 
				utf8::utf16to8((*wit).begin(), (*wit).end(), std::back_inserter(utf8line));
				m_args.push_back(utf8line);
			}
			
		}
		LocalFree(wargv);
	#else
		std::string cmdLine(GetCommandLineA());
		std::istringstream ss(cmdLine);
		std::istream_iterator<std::string> iss(ss);
		// the first token is the path+name of the programm
		// -> ignore it
		iss++;
		std::copy(iss,
			 std::istream_iterator<std::string>(),
			 std::back_inserter<std::vector<std::string> >(m_args));
	#endif
#endif
}

CmdLineArgs::CmdLineArgs(int argc, char **argv)
	: m_args(argv+1, argv+argc)
{
}
