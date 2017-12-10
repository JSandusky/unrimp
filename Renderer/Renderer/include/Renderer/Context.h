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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/PlatformTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ILog;
	class IAssert;
	class IAllocator;
}
#ifdef LINUX
	// Copied from "Xlib.h"
	struct _XDisplay;

	// Copied from "wayland-client.h"
	struct wl_display;
	struct wl_surface;
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Context class encapsulating all embedding related wirings
	*/
	class Context
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		enum class ContextType
		{
			WINDOWS,
			X11,
			WAYLAND
		};


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] log
		*    Log instance to use, the log instance must stay valid as long as the renderer instance exists
		*  @param[in] assert
		*    Assert instance to use, the assert instance must stay valid as long as the renderer instance exists
		*  @param[in] allocator
		*    Allocator instance to use, the allocator instance must stay valid as long as the renderer instance exists
		*  @param[in] nativeWindowHandle
		*    Native window handle
		*  @param[in] useExternalContext
		*    Indicates if an external renderer context is used; in this case the renderer itself has nothing to do with the creation/managing of an renderer context
		*  @param[in] contextType
		*    The type of the context
		*/
		inline Context(ILog& log, IAssert& assert, IAllocator& allocator, handle nativeWindowHandle = 0, bool useExternalContext = false, ContextType contextType = Context::ContextType::WINDOWS);

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~Context();

		/**
		*  @brief
		*    Return the log instance
		*
		*  @return
		*    The log instance
		*/
		inline ILog& getLog() const;

		/**
		*  @brief
		*    Return the assert instance
		*
		*  @return
		*    The assert instance
		*/
		inline IAssert& getAssert() const;

		/**
		*  @brief
		*    Return the allocator instance
		*
		*  @return
		*    The allocator instance
		*/
		inline IAllocator& getAllocator() const;

		/**
		*  @brief
		*    Return the native window handle
		*
		*  @return
		*    The native window handle
		*/
		inline handle getNativeWindowHandle() const;

		/**
		*  @brief
		*    Return whether or not an external context is used
		*
		*  @return
		*    "true" if an external context is used, else "false"
		*/
		inline bool isUsingExternalContext() const;

		/**
		*  @brief
		*    Return the type of the context
		*
		*  @return
		*    The context type
		*/
		inline ContextType getType() const;

		/**
		*  @brief
		*    Return a handle to the renderer API shared library
		*
		*  @return
		*    The handle to the renderer API shared library
		*/
		inline void* getRendererApiSharedLibrary() const;

		/**
		*  @brief
		*    Set the handle for the renderer API shared library to use instead of let it load by the renderer instance
		*
		*  @param[in] rendererApiSharedLibrary
		*    A handle to the renderer API shared library; the renderer will use this handle instead of loading the renderer API shared library itself
		*/
		inline void setRendererApiSharedLibrary(void* rendererApiSharedLibrary);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ILog&		mLog;
		IAssert&	mAssert;
		IAllocator&	mAllocator;
		handle		mNativeWindowHandle;
		bool		mUseExternalContext;
		ContextType	mContextType;
		void*		mRendererApiSharedLibrary;	///< A handle to the renderer API shared library (e.g. obtained via "dlopen()" and co)


	};

// TODO(sw) Hide it via an define for non Linux builds? This definition doesn't use any platform specific headers
#ifdef LINUX
	/**
	*  @brief
	*    X11 version of the context class
	*/
	class X11Context : public Context
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] log
		*    Log instance to use, the log instance must stay valid as long as the renderer instance exists
		*  @param[in] assert
		*    Assert instance to use, the assert instance must stay valid as long as the renderer instance exists
		*  @param[in] allocator
		*    Allocator instance to use, the allocator instance must stay valid as long as the renderer instance exists
		*  @param[in] display
		*    The X11 display connection
		*  @param[in] nativeWindowHandle
		*    Native window handle
		*  @param[in] useExternalContext
		*    Indicates if an external renderer context is used; in this case the renderer itself has nothing to do with the creation/managing of an renderer context
		*/
		inline X11Context(ILog& log, IAssert& assert, IAllocator& allocator, _XDisplay* display, handle nativeWindowHandle = 0, bool useExternalContext = false);

		/**
		*  @brief
		*    Return the x11 display connection
		*
		*  @return
		*    The x11 display connection
		*/
		inline _XDisplay* getDisplay() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		_XDisplay* mDisplay;


	};

	/**
	*  @brief
	*    Wayland version of the context class
	*/
	class WaylandContext : public Context
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] log
		*    Log instance to use, the log instance must stay valid as long as the renderer instance exists
		*  @param[in] assert
		*    Assert instance to use, the assert instance must stay valid as long as the renderer instance exists
		*  @param[in] allocator
		*    Allocator instance to use, the allocator instance must stay valid as long as the renderer instance exists
		*  @param[in] display
		*    The Wayland display connection
		*  @param[in] surface
		*    The Wayland surface
		*  @param[in] useExternalContext
		*    Indicates if an external renderer context is used; in this case the renderer itself has nothing to do with the creation/managing of an renderer context
		*/
		inline WaylandContext(ILog& log, IAssert& assert, IAllocator& allocator, wl_display* display, wl_surface* surface = 0, bool useExternalContext = false);

		/**
		*  @brief
		*    Return the Wayland display connection
		*
		*  @return
		*    The Wayland display connection
		*/
		inline wl_display* getDisplay() const;

		/**
		*  @brief
		*    Return the Wayland surface
		*
		*  @return
		*    The Wayland surface
		*/
		inline wl_surface* getSurface() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		wl_display* mDisplay;
		wl_surface* mSurface;


	};
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Context.inl"
