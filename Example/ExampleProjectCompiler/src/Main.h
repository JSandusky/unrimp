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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Platform independent program entry point              ]
//[-------------------------------------------------------]
int programEntryPoint();


//[-------------------------------------------------------]
//[ Platform dependent program entry point                ]
//[-------------------------------------------------------]
// Windows implementation
#ifdef WIN32

	// For memory leak detection
	#ifdef _DEBUG
		#define _CRTDBG_MAP_ALLOC
		#include <stdlib.h>
		#include <crtdbg.h>
	#endif

	#ifdef _CONSOLE
		#ifdef UNICODE
			int wmain(int argc, wchar_t **argv)
		#else
			int main(int argc, char **argv)
		#endif
			{
				// Call the platform independent program entry point
				const int result = programEntryPoint();

				// For memory leak detection
				#ifdef _DEBUG
					_CrtDumpMemoryLeaks();
				#endif

				// Done
				return result;
			}
	#else
		#include "WindowsHeader.h"
		#ifdef UNICODE
			int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
		#else
			int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
		#endif
			{
				// Call the platform independent program entry point
				const int result = programEntryPoint();

				// For memory leak detection
				#ifdef _DEBUG
					_CrtDumpMemoryLeaks();
				#endif

				// Done
				return result;
			}
	#endif

// Linux implementation
#elif LINUX
	int main(int, char **)
	{
		// Call the platform independent program entry point
		return programEntryPoint();
	}
#endif
