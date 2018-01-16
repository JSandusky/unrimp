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
#include "PrecompiledHeader.h"
#include "Framework/ApplicationImplWindows.h"
#include "Framework/IApplication.h"
#include "Framework/IApplicationRendererRuntime.h"

#include <RendererRuntime/IRendererRuntime.h>
#include <RendererRuntime/DebugGui/Detail/DebugGuiManagerWindows.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
ApplicationImplWindows::ApplicationImplWindows(IApplication& application, const char* windowTitle) :
	IApplicationImpl(application),
	mApplication(&application),
	mNativeWindowHandle(NULL_HANDLE),
	mFirstUpdate(true)
{
	// Copy the given window title
	if (nullptr != windowTitle)
	{
		strncpy(mWindowTitle, windowTitle, 64);
	}
	else
	{
		mWindowTitle[0] = '\0';
	}
}

ApplicationImplWindows::~ApplicationImplWindows()
{
	// Nothing here
	// mNativeWindowHandle is destroyed within onDeinitialization()
}


//[-------------------------------------------------------]
//[ Public virtual IApplicationImpl methods               ]
//[-------------------------------------------------------]
void ApplicationImplWindows::onInitialization()
{
	{ // Setup and register the window class for the OpenGL dummy window
		WNDCLASS windowDummyClass;
		windowDummyClass.hInstance		= ::GetModuleHandle(nullptr);
		windowDummyClass.lpszClassName	= TEXT("ApplicationImplWindows");
		windowDummyClass.lpfnWndProc	= wndProc;
		windowDummyClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowDummyClass.hIcon			= nullptr;
		windowDummyClass.hCursor		= ::LoadCursor(nullptr, IDC_ARROW);
		windowDummyClass.lpszMenuName	= nullptr;
		windowDummyClass.cbClsExtra		= 0;
		windowDummyClass.cbWndExtra		= 0;
		windowDummyClass.hbrBackground	= nullptr;
		::RegisterClass(&windowDummyClass);
	}

	// Create the OS window instance
	mNativeWindowHandle = ::CreateWindowA("ApplicationImplWindows", mWindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, nullptr, nullptr, ::GetModuleHandle(nullptr), this);
}

void ApplicationImplWindows::onDeinitialization()
{
	// Destroy the OS window instance, in case there's one
	if (NULL_HANDLE != mNativeWindowHandle)
	{
		// Destroy the OpenGL dummy window
		::DestroyWindow(mNativeWindowHandle);
		mNativeWindowHandle = NULL_HANDLE;
	}

	// Unregister the window class for the OpenGL dummy window
	::UnregisterClass(TEXT("ApplicationImplWindows"), ::GetModuleHandle(nullptr));
}

bool ApplicationImplWindows::processMessages()
{
	// The window is made visible before the first processing of operation system messages, this way the concrete example has the
	// opportunity to e.g. restore the window position and size from a previous season without having a visible jumping window
	if (mFirstUpdate)
	{
		if (NULL_HANDLE != mNativeWindowHandle)
		{
			// Show the created OS window
			::ShowWindow(mNativeWindowHandle, SW_SHOWDEFAULT);
			::UpdateWindow(mNativeWindowHandle);
		}
		mFirstUpdate = false;
	}

	// By default, do not shut down the application
	bool quit = false;

	// Look if messages are waiting (non-blocking)
	MSG msg;
	while (::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
	{
		// Get the waiting message
		::GetMessage(&msg, nullptr, 0, 0);
		if (WM_QUIT == msg.message)
		{
			// Shut down the application
			quit = true;
		}

		// Process message
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	// Done, tell the caller whether or not to shut down the application
	return quit;
}

void ApplicationImplWindows::getWindowSize(int& width, int& height) const
{
	// Is there a valid OS window?
	if (NULL_HANDLE != mNativeWindowHandle)
	{
		RECT rect;
		::GetClientRect(mNativeWindowHandle, &rect);
		width  = rect.right  - rect.left;
		height = rect.bottom - rect.top;
	}
	else
	{
		// There's no valid OS window, return known values
		width  = 0;
		height = 0;
	}
}

handle ApplicationImplWindows::getNativeWindowHandle() const
{
	return reinterpret_cast<handle>(mNativeWindowHandle);
}

void ApplicationImplWindows::redraw()
{
	// Is there a valid OS window?
	if (NULL_HANDLE != mNativeWindowHandle)
	{
		// Redraw window
		::RedrawWindow(mNativeWindowHandle, nullptr, nullptr, RDW_INVALIDATE);
	}
}


//[-------------------------------------------------------]
//[ Private static MS Windows callback function           ]
//[-------------------------------------------------------]
LRESULT CALLBACK ApplicationImplWindows::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Get pointer to application implementation
	ApplicationImplWindows* applicationImplWindows = nullptr;
	if (WM_CREATE == message)
	{
		applicationImplWindows = static_cast<ApplicationImplWindows*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
	}
	else if (NULL_HANDLE != hWnd)
	{
		applicationImplWindows = reinterpret_cast<ApplicationImplWindows*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	// Call the MS Windows callback of the debug GUI
	if (nullptr != applicationImplWindows)
	{
		// TODO(co) Evil cast ahead. Maybe simplify the example application framework? After all, it's just an example framework for Unrimp and nothing too generic.
		const IApplicationRendererRuntime* applicationRendererRuntime = dynamic_cast<IApplicationRendererRuntime*>(&applicationImplWindows->getApplication());
		if (nullptr != applicationRendererRuntime)
		{
			const RendererRuntime::IRendererRuntime* rendererRuntime = applicationRendererRuntime->getRendererRuntime();
			if (nullptr != rendererRuntime)
			{
				static_cast<RendererRuntime::DebugGuiManagerWindows&>(rendererRuntime->getDebugGuiManager()).wndProc(hWnd, message, wParam, lParam);
			}
		}
	}

	// Evaluate message
	switch (message)
	{
		// Initialize window
		case WM_CREATE:
			// Set window pointer and handle (SetWindowLongPtr is the 64bit equivalent to SetWindowLong)
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(applicationImplWindows));
			return 0;

		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;

		case WM_SIZE:
			// Inform the owner application instance
			applicationImplWindows->mApplication->onResize();
			return 0;

		case WM_SYSKEYDOWN:
			// Toggle fullscreen right now? (Alt-Return)
			if (VK_RETURN == wParam && (lParam & (1 << 29)))	// Bit 29 = the ALT-key
			{
				// Inform the owner application instance
				applicationImplWindows->mApplication->onToggleFullscreenState();
			}
			return 0;

		case WM_KEYDOWN:
			// Application shutdown = "escape"-key = for all examples
			if (27 == wParam)
			{
				applicationImplWindows->mApplication->exit();
			}
			return 0;

		// Window paint request
		case WM_PAINT:
		{
			// Begin paint
			PAINTSTRUCT paintStruct;
			::BeginPaint(hWnd, &paintStruct);

			// Redraw, but only if the draw area isn't null
			if (!::IsRectEmpty(&paintStruct.rcPaint))
			{
				// Inform the owner application instance
				applicationImplWindows->mApplication->onDrawRequest();
			}

			// End paint
			::EndPaint(hWnd, &paintStruct);
			return 0;
		}

		// Let the OS handle this message
		default:
			return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
}
