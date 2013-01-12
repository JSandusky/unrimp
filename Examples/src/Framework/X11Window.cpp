#include "X11Window.h"
#include "X11Application.h"
#include <cstring>

X11Window::X11Window()
	: mDestroyed(false)
{
	Display *display = X11Application::instance()->getDisplay();
	
	WM_DELETE_WINDOW	= XInternAtom(display, "WM_DELETE_WINDOW",	 True);
	UTF8_STRING			= XInternAtom(display, "UTF8_STRING",			 False);
	WM_NAME				= XInternAtom(display, "WM_NAME",				 False);
	_NET_WM_NAME		= XInternAtom(display, "_NET_WM_NAME",		 False);
	_NET_WM_VISIBLE_NAME = XInternAtom(display, "_NET_WM_VISIBLE_NAME", False);
	
	const unsigned int  nWidth  = 640;
	const unsigned int  nHeight = 480;
	const int           nScreen = DefaultScreen(display);
	Visual             *pVisual = DefaultVisual(display, nScreen);
	const int           nDepth  = DefaultDepth(display, nScreen);

	// Create the native OS window instance with a black background (else we will see trash if nothing has been drawn, yet)
	XSetWindowAttributes sXSetWindowAttributes;
	sXSetWindowAttributes.background_pixel = 0;
	sXSetWindowAttributes.event_mask = ExposureMask | StructureNotifyMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask | VisibilityChangeMask | KeyPressMask | MotionNotify;
	mWindowId = XCreateWindow(display, XRootWindow(display, nScreen), 0, 0, nWidth, nHeight, 0, nDepth, InputOutput, pVisual, CWBackPixel | CWEventMask, &sXSetWindowAttributes);
	XSetWMProtocols(display, mWindowId, &WM_DELETE_WINDOW, 1);
	
	X11Application::instance()->AddWindowToEventLoop(*this);
}


X11Window::~X11Window()
{
	if (mWindowId)
		XDestroyWindow(X11Application::instance()->getDisplay(), mWindowId);
}

void X11Window::setTitle(const char* title)
{
	Display *mDisplay = X11Application::instance()->getDisplay();
	const int nNumOfElements = std::strlen(title);
	const unsigned char* windowTitle = reinterpret_cast<const unsigned char*>(title);
	XChangeProperty(mDisplay, mWindowId, WM_NAME,				UTF8_STRING, 8, PropModeReplace, windowTitle, nNumOfElements);
	XChangeProperty(mDisplay, mWindowId, _NET_WM_NAME,		 	UTF8_STRING, 8, PropModeReplace, windowTitle, nNumOfElements);
	XChangeProperty(mDisplay, mWindowId, _NET_WM_VISIBLE_NAME,	UTF8_STRING, 8, PropModeReplace, windowTitle, nNumOfElements);
}

void X11Window::show()
{
	XMapWindow(X11Application::instance()->getDisplay(), mWindowId);
}

void X11Window::getWindowSize(int &width, int &height) const
{
	// Is there a valid OS window?
	if (mWindowId)
	{
		// Get X window geometry information
		::Window nRootWindow = 0;
		int nPositionX = 0, nPositionY = 0;
		unsigned int nWidth = 0, nHeight = 0, nBorder = 0, nDepth = 0;
		XGetGeometry(X11Application::instance()->getDisplay(), mWindowId, &nRootWindow, &nPositionX, &nPositionY, &nWidth, &nHeight, &nBorder, &nDepth);
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

void X11Window::refresh()
{
	// Is there a valid OS window?
	if (mWindowId && !mDestroyed)
	{
		// Send expose event
		XEvent sEvent;
		sEvent.type			 = Expose;
		sEvent.xany.window	 = mWindowId;
		sEvent.xexpose.count = 0;
		XSendEvent(X11Application::instance()->getDisplay(), mWindowId, False, 0, &sEvent);

		// Do it!
		XSync(X11Application::instance()->getDisplay(), False);
	}
}


bool X11Window::HandleEvent(XEvent& event)
{
	
	// Process message
	switch (event.type) {
		case DestroyNotify:
			mDestroyed = true;
			// Mark window destroyed
			X11Application::instance()->RemoveWindowFromEventLoop(*this);
			mWindowId = 0;
			break;

		// Window configuration changed
		case ConfigureNotify:
			//mX11Application->onResize();
			break;

		case ClientMessage:
			// When the "WM_DELETE_WINDOW" client message is send, no "DestroyNotify"-message is generated because the
			// application itself should destroy/close the window to which the "WM_DELETE_WINDOW" client message was send to.
			// In this case, we will leave the event loop after this message was processed and no other messages are in the queue.
			// -> No "DestroyNotify"-message can be received
			if (event.xclient.data.l[0] == WM_DELETE_WINDOW)
			{
				XDestroyWindow(event.xany.display, mWindowId);
				mDestroyed = true;
			}
			break;
	}
	return false;
}


X11Window::X11Window(const X11Window& other)
{

}

X11Window& X11Window::operator=(const X11Window& other)
{
    return *this;
}

