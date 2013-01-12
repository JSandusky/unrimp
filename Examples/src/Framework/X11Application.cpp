#include "X11Application.h"
#include "X11Window.h"

X11Application* X11Application::_self = 0;

X11Application::X11Application()
	: mDisplay(XOpenDisplay(0))
{
	_self = this;
}

X11Application::~X11Application()
{
	XCloseDisplay(mDisplay);
}

int X11Application::run()
{
	XEvent event;
	
	while (!mWindows.empty())  {
		XNextEvent(mDisplay, &event);
		HandleEvent(event);
	}
	return 0;
}

bool X11Application::handlePendingEvents()
{
	XEvent event;
	
	while (!mWindows.empty() && XPending(mDisplay)>0)  {
		XNextEvent(mDisplay, &event);
		HandleEvent(event);
	}
	
	mWindows.empty();
}

bool X11Application::HandleEvent(XEvent& event)
{
	std::map<WindowHandle, WindowEntry>::iterator it;
	
	it = mWindows.find(event.xany.window);
	
	if (it != mWindows.end())
		return it->second.x11Window->HandleEvent(event);
	
	return false;
}

void X11Application::AddWindowToEventLoop(X11Window& window)
{
	std::map<WindowHandle, WindowEntry>::iterator it;
	
	it = mWindows.find(window.winId());
	if (it != mWindows.end())
		return; // already added
		
	WindowEntry entry = {window.winId(), &window};
	mWindows.insert(std::pair<WindowHandle, WindowEntry>(window.winId(), entry));
}

void X11Application::RemoveWindowFromEventLoop(const X11Window& window)
{
	std::map<WindowHandle, WindowEntry>::iterator it;
	
	it = mWindows.find(window.winId());
	if (it != mWindows.end())
		mWindows.erase(it);
}


X11Application::X11Application(const X11Application& other)
{
}

X11Application& X11Application::operator=(const X11Application& other)
{
    return *this;
}

