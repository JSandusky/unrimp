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
#include "CmdLineArgs.h"
#ifdef WIN32
	#include <windows.h>
	#include "utf8/utf8.h" // to convert utf16 strings to utf8
#endif


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
CmdLineArgs::CmdLineArgs()
{
#if WIN32
	int wargc;
	wchar_t **wargv = CommandLineToArgvW(GetCommandLineW(),&wargc);
	if (wargc > 0) {
		m_args.reserve(wargc);
		utf8::utf16to8(wargv, wargv+wargc, back_inserter(m_args));
	}
	LocalFree(wargv);
#endif
}

CmdLineArgs::CmdLineArgs(int argc, char **argv)
	: m_args(argv+1, argv+argc)
{
}
