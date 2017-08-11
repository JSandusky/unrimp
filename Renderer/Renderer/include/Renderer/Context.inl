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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline Context::Context(ContextType contextType, ILog& log, handle nativeWindowHandle, bool useExternalContext) :
		mContextType(contextType),
		mLog(log),
		mNativeWindowHandle(nativeWindowHandle),
		mUseExternalContext(useExternalContext),
		mRendererApiSharedLibrary(nullptr)
	{
		// Nothing here
	}

	inline Context::~Context()
	{
		// Nothing here
	}

	inline Context::ContextType Context::getType() const
	{
		return mContextType;
	}

	inline ILog& Context::getLog() const
	{
		return mLog;
	}

	inline handle Context::getNativeWindowHandle() const
	{
		return mNativeWindowHandle;
	}

	inline bool Context::isUsingExternalContext() const
	{
		return mUseExternalContext;
	}

	inline void* Context::getRendererApiSharedLibrary() const
	{
		return mRendererApiSharedLibrary;
	}

	inline void Context::setRendererApiSharedLibrary(void* rendererApiSharedLibrary)
	{
		mRendererApiSharedLibrary = rendererApiSharedLibrary;
	}

#ifdef LINUX
	inline X11Context::X11Context(ILog& log, _XDisplay* display, handle nativeWindowHandle, bool useExternalContext) :
		Context(Context::ContextType::X11, log, nativeWindowHandle, useExternalContext),
		mDisplay(display)
	{
		// Nothing here
	}

	inline _XDisplay* X11Context::getDisplay() const
	{
		return mDisplay;
	}


	inline WaylandContext::WaylandContext(ILog& log, wl_display* display, wl_surface* surface, bool useExternalContext) :
		Context(Context::ContextType::WAYLAND, log, 1, useExternalContext), // Under wayland the surface (aka window) handle is not an integer, but the renderer implementation expects an integer as window handle so we give here an value != 0 so that a swapchain is created
		mDisplay(display),
		mSurface(surface)
	{
		// Nothing here
	}

	inline wl_display* WaylandContext::getDisplay() const
	{
		return mDisplay;
	}

	inline wl_surface* WaylandContext::getSurface() const
	{
		return mSurface;
	}
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
