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
}

#ifdef LINUX
	// Copied from Xlib.h
	struct _XDisplay;
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
			WIN32,
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
		*  @param[in] contextType
		*    The type of the context
		*  @param[in] log
		*    Log instance to use, the log instance must stay valid as long as the renderer instance exists
		*  @param[in] nativeWindowHandle
		*    Native window handle
		*  @param[in] useExternalContext
		*    Indicates if an external renderer context is used; in this case the renderer itself has nothing to do with the creation/managing of an renderer context
		*/
		inline Context(ContextType contextType, ILog& log, handle nativeWindowHandle = 0, bool useExternalContext = false);

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~Context();

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
		*    Return the log instance
		*
		*  @return
		*    The log instance
		*/
		inline ILog& getLog() const;

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
		*    Return an handle to the renderer api shared library
		*
		*  @return
		*    The handle to the renderer api shared library
		*/
		inline void* getRendererApiSharedLibrary() const;

		/**
		*  @brief
		*    Sets the handle for the renderer api shared library to use instead of let it load by the renderer instance
		*
		*  @param[in] rendererApiSharedLibrary
		*    An handle to the renderer api shared library. The renderer will use this handle instead of loading the renderer api shared library itself
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
		ContextType mContextType;
		ILog&  mLog;
		handle mNativeWindowHandle;
		bool   mUseExternalContext;
		void*  mRendererApiSharedLibrary;	///< An handle to the renderer api shared library (e.g. optained via dlopen and co


	};

// TODO(sw) Hide it via an define for non linux builds? This definition doesn't use any platform specific headers
#ifdef LINUX
	/**
	*  @brief
	*    X11 Version of the context class
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
		*  @param[in] display
		*    The X11 display connection
		*  @param[in] nativeWindowHandle
		*    Native window handle
		*  @param[in] useExternalContext
		*    Indicates if an external renderer context is used; in this case the renderer itself has nothing to do with the creation/managing of an renderer context
		*/
		inline X11Context(ILog& log, _XDisplay* display, handle nativeWindowHandle = 0, bool useExternalContext = false);

		/**
		*  @brief
		*    Return the type of the context
		*
		*  @return
		*    The context type
		*/
		inline _XDisplay* getDisplay() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		_XDisplay* mDisplay;


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
