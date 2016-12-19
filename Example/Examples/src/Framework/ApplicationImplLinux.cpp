/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "Framework/ApplicationImplLinux.h"
#include "Framework/IApplication.h"
#include "Framework/X11Application.h"


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class ApplicationWindow : public X11Window
{
public:
	ApplicationWindow(IApplication &application) :
		mApplication(application)
	{
		// Nothing here
	}

	virtual bool HandleEvent(XEvent &event)
	{
		X11Window::HandleEvent(event);

		if (isDestroyed())
		{
			return true;
		}

		switch (event.type)
		{
			case Expose:
				// There could be more then one expose event currently in the event loop.
				// To avoid too many redraw calls, call onDrawRequest only when the current processed expose event is the last one.
				if (!event.xexpose.count)
				{
					mApplication.onDrawRequest();
				}
				break;

			// Window configuration changed
			case ConfigureNotify:
				mApplication.onResize();
				break;

			case KeyPress:
			{
				const uint32_t key = XLookupKeysym(&event.xkey, 0);
				mApplication.onKeyDown(key);
				break;
			}

			case KeyRelease:
			{
				const uint32_t key = XLookupKeysym(&event.xkey, 0);
				mApplication.onKeyUp(key);
				break;
			}
		}
		return false;
	}
private:
	IApplication &mApplication;
};


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
ApplicationImplLinux::ApplicationImplLinux(IApplication &application, const char *windowTitle) :
	IApplicationImpl(application),
	mApplication(&application),
	mX11EventLoop(nullptr),
	mMainWindow(nullptr)
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

ApplicationImplLinux::~ApplicationImplLinux()
{
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual IApplicationImpl methods               ]
//[-------------------------------------------------------]
void ApplicationImplLinux::onInitialization()
{
	mX11EventLoop = new X11Application();
	mMainWindow = new ApplicationWindow(*mApplication);

	mMainWindow->setTitle(mWindowTitle);
	mMainWindow->show();

	XSync(X11Application::instance()->getDisplay(), False);
}

void ApplicationImplLinux::onDeinitialization()
{
	// Destroy the OS window instance, in case there's one
	if (nullptr != mMainWindow)
	{
		// Destroy the OpenGL dummy window
		delete mMainWindow;
		mMainWindow = nullptr;
	}

	if (nullptr != mX11EventLoop)
	{
		delete mX11EventLoop;
		mX11EventLoop = nullptr;
	}
}

bool ApplicationImplLinux::processMessages()
{
	return mX11EventLoop->handlePendingEvents();
}

void ApplicationImplLinux::getWindowSize(int &width, int &height) const
{
	// Is there a valid OS window?
	if (nullptr != mMainWindow)
	{
		mMainWindow->getWindowSize(width, height);
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
	return mMainWindow->winId();
}

void ApplicationImplLinux::redraw()
{
	// Is there a valid OS window?
	if (nullptr != mMainWindow)
	{
		mMainWindow->refresh();
	}
}
