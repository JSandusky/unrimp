#ifndef X11WINDOW_H
#define X11WINDOW_H

#include <X11/Xlib.h>

class X11Window
{

public:
    X11Window();
	virtual ~X11Window();
	virtual bool HandleEvent(XEvent &event);
	void setTitle(const char* title);
	void show();
	void getWindowSize(int &width, int &height) const;
	void refresh();
	
	Window winId() const { return mWindowId; }

private:
    X11Window(const X11Window& other);
    virtual X11Window& operator=(const X11Window& other);
	
private:
	bool mDestroyed;
	Window mWindowId;
	::Atom					 WM_DELETE_WINDOW;		/**< System atom for delete */
	::Atom					 UTF8_STRING;			/**< Atom for the type of a window title */
	::Atom 					 WM_NAME;				/**< Window title (old?) */
	::Atom 					 _NET_WM_NAME;			/**< Window title */
	::Atom 					 _NET_WM_VISIBLE_NAME;	/**< Window title (visible title, can be different) */
};

#endif // X11WINDOW_H
