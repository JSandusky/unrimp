#ifndef APPLICATION_H
#define APPLICATION_H

#include <X11/Xlib.h>
#include <map>

class X11Window;
typedef Window WindowHandle;

class X11Application
{
	friend class X11Window;

public:
	static X11Application* instance() {return _self;}

public:
    X11Application();
	~X11Application();
	int run();
	bool handlePendingEvents();
	
	Display *getDisplay() { return mDisplay; }

private:
	bool HandleEvent(XEvent& event);
	
	void AddWindowToEventLoop(X11Window &window);
	void RemoveWindowFromEventLoop(const X11Window &window);
	
    X11Application(const X11Application& other);
    virtual X11Application& operator=(const X11Application& other);
	
private:
	static X11Application	*_self;
	
private:
	struct WindowEntry
	{
		WindowHandle window;
		X11Window *x11Window;
	};
	
private:
	Display *mDisplay;
	std::map<WindowHandle, WindowEntry> mWindows;
};

#endif // APPLICATION_H
