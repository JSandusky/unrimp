/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/ApplicationImplLinux.h"
#include "Framework/IApplication.h"


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
/**
*  @brief
*    Default constructor
*/
ApplicationImplLinux::ApplicationImplLinux(IApplication &application, const char *windowTitle) :
	mApplication(&application),
	mNativeWindowHandle(NULL_HANDLE),
	mDisplay(0)
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

/**
*  @brief
*    Destructor
*/
ApplicationImplLinux::~ApplicationImplLinux()
{
	// Nothing to do in here
	// mNativeWindowHandle is destroyed within onDeinitialization()
}


//[-------------------------------------------------------]
//[ Public virtual IApplicationImpl methods               ]
//[-------------------------------------------------------]
void ApplicationImplLinux::onInitialization()
{
	mDisplay = XOpenDisplay(0);
	
	// Atoms
	WM_DELETE_WINDOW	= XInternAtom(mDisplay, "WM_DELETE_WINDOW",	 True);
	UTF8_STRING			= XInternAtom(mDisplay, "UTF8_STRING",			 False);
	WM_NAME				= XInternAtom(mDisplay, "WM_NAME",				 False);
	_NET_WM_NAME		= XInternAtom(mDisplay, "_NET_WM_NAME",		 False);
	_NET_WM_VISIBLE_NAME = XInternAtom(mDisplay, "_NET_WM_VISIBLE_NAME", False);
	
	const unsigned int  nWidth  = 640;
	const unsigned int  nHeight = 480;
	const int           nScreen = DefaultScreen(mDisplay);
	Visual             *pVisual = DefaultVisual(mDisplay, nScreen);
	const int           nDepth  = DefaultDepth(mDisplay, nScreen);

	// Create the native OS window instance with a black background (else we will see trash if nothing has been drawn, yet)
	XSetWindowAttributes sXSetWindowAttributes;
	sXSetWindowAttributes.background_pixel = 0;
	sXSetWindowAttributes.event_mask = ExposureMask | StructureNotifyMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask | VisibilityChangeMask | KeyPressMask | MotionNotify;
	mNativeWindowHandle = XCreateWindow(mDisplay, XRootWindow(mDisplay, nScreen), 0, 0, nWidth, nHeight, 0, nDepth, InputOutput, pVisual, CWBackPixel | CWEventMask, &sXSetWindowAttributes);
	XSetWMProtocols(mDisplay, mNativeWindowHandle, &WM_DELETE_WINDOW, 1);

// 	// Set icon
// 	Atom wmIcon = XInternAtom(mDisplay, "_NET_WM_ICON", False);
// 	Atom wmCardinal = XInternAtom(mDisplay, "CARDINAL", False);
// 	XChangeProperty(mDisplay, mNativeWindowHandle, wmIcon, wmCardinal, 32,
// 					PropModeReplace, reinterpret_cast<const unsigned char*>(pl_icon), pl_icon_length);

	const int nNumOfElements = strlen(mWindowTitle);
	const unsigned char* windowTitle = reinterpret_cast<unsigned char*>(mWindowTitle);
	XChangeProperty(mDisplay, mNativeWindowHandle, WM_NAME,				 UTF8_STRING, 8, PropModeReplace, windowTitle, nNumOfElements);
	XChangeProperty(mDisplay, mNativeWindowHandle, _NET_WM_NAME,		 UTF8_STRING, 8, PropModeReplace, windowTitle, nNumOfElements);
	XChangeProperty(mDisplay, mNativeWindowHandle, _NET_WM_VISIBLE_NAME, UTF8_STRING, 8, PropModeReplace, windowTitle, nNumOfElements);
	
	XMapRaised(mDisplay, mNativeWindowHandle);
	
	XSync(mDisplay, False);
}

void ApplicationImplLinux::onDeinitialization()
{
	// Destroy the OS window instance, in case there's one
	if (NULL_HANDLE != mNativeWindowHandle)
	{
		// Destroy the OpenGL dummy window
		::XDestroyWindow(mDisplay, mNativeWindowHandle);
		mNativeWindowHandle = NULL_HANDLE;
	}
	
	XCloseDisplay(mDisplay);
	mDisplay = 0;
}

bool ApplicationImplLinux::processMessages()
{
	// By default, do not shut down the application
	bool quit = false;

	// Look if messages are waiting (non-blocking)
	while (XPending(mDisplay) > 0) {
		// Get the waiting message
		XEvent sXEvent;
		XNextEvent(mDisplay, &sXEvent);

		// Process message
		switch (sXEvent.type) {
			case Expose:
				// There could be more then one Expose event currently in the event loop.
				// To avoid too many redraw calls, call OnDraw only when the current processed Expose event is the last one.
				if (!sXEvent.xexpose.count)
					mApplication->onDrawRequest();
				break;

			case DestroyNotify:
				// Mark window destroyed
				mNativeWindowHandle = 0;
				quit = true;
				break;

			// Window configuration changed
			case ConfigureNotify:
				mApplication->onResize();
				break;

			case ClientMessage:
				// When the "WM_DELETE_WINDOW" client message is send, no "DestroyNotify"-message is generated because the
				// application itself should destroy/close the window to which the "WM_DELETE_WINDOW" client message was send to.
				// In this case, we will leave the event loop after this message was processed and no other messages are in the queue.
				// -> No "DestroyNotify"-message can be received
				if (sXEvent.xclient.data.l[0] == WM_DELETE_WINDOW)
					quit = true;
				break;

			case KeyPress:
// 				// Is it allowed to toggle the fullscreen mode using hotkeys? (Alt-Return or AltGr-Return)
// 				if (m_pFrontendOS->GetToggleFullscreenMode()) {
// 					// It's allowed, toggle fullscreen right now?
// 					const unsigned int nKey = XLookupKeysym(&sXEvent.xkey, 0);
// 					if (nKey == XK_Return && ((sXEvent.xkey.state & Mod1Mask) || (sXEvent.xkey.state & Mod2Mask))) {
// 						// Toggle fullscreen mode
// 						m_pFrontendOS->SetFullscreen(!m_pFrontendOS->IsFullscreen());
// 					}
// 				}
			{
				const unsigned int nKey = XLookupKeysym(&sXEvent.xkey, 0);
				mApplication->onKeyDown(nKey);
			}
				break;
			case KeyRelease:
			{
				const unsigned int nKey = XLookupKeysym(&sXEvent.xkey, 0);
				mApplication->onKeyUp(nKey);
			}
				break;
		}
	}

	// Done, tell the caller whether or not to shut down the application
	return quit;
}

void ApplicationImplLinux::getWindowSize(int &width, int &height) const
{
	// Is there a valid OS window?
	if (NULL_HANDLE != mNativeWindowHandle)
	{
		// Get X window geometry information
		::Window nRootWindow = 0;
		int nPositionX = 0, nPositionY = 0;
		unsigned int nWidth = 0, nHeight = 0, nBorder = 0, nDepth = 0;
		XGetGeometry(mDisplay, mNativeWindowHandle, &nRootWindow, &nPositionX, &nPositionY, &nWidth, &nHeight, &nBorder, &nDepth);
		width = nWidth;
		height = nHeight;
	}
	else
	{
		// There's no valid OS window, return known values
		width  = 0;
		height = 0;
	}
}

handle ApplicationImplLinux::getNativeWindowHandle() const
{
	return mNativeWindowHandle;
}

void ApplicationImplLinux::redraw()
{
	// Is there a valid OS window?
	if (NULL_HANDLE != mNativeWindowHandle)
	{
		// Send expose event
		XEvent sEvent;
		sEvent.type			 = Expose;
		sEvent.xany.window	 = mNativeWindowHandle;
		sEvent.xexpose.count = 0;
		XSendEvent(mDisplay, mNativeWindowHandle, False, 0, &sEvent);

		// Do it!
		XSync(mDisplay, False);
	}
}