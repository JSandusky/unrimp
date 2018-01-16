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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/PlatformTypes.h"
#include "Framework/IApplicationImpl.h"


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Abstract application interface
*/
class IApplication
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Destructor
	*/
	virtual ~IApplication();

	/**
	*  @brief
	*    Run the application
	*
	*  @return
	*    Program return code, 0 to indicate that no error has occurred
	*/
	int run();

	/**
	*  @brief
	*    Ask the application politely to shut down as soon as possible
	*/
	inline void exit();

	/**
	*  @brief
	*    Return the window size
	*
	*  @param[out] width
	*    Receives the window width
	*  @param[out] height
	*    Receives the window height
	*/
	inline void getWindowSize(int& width, int& height) const;

	/**
	*  @brief
	*    Return the OS dependent window handle
	*
	*  @remarks
	*    The OS dependent window handle, can be a null handle
	*/
	inline handle getNativeWindowHandle() const;

	/**
	*  @brief
	*    Redraw request
	*/
	inline void redraw();

	#ifdef LINUX
		/**
		*  @brief
		*    Return the X11 display connection object
		*
		*  @note
		*    - The X11 display connection object, can be a null pointer
		*/
		inline Display* getX11Display() const;
	#endif


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Called on application initialization
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onInitialization();

	/**
	*  @brief
	*    Called on application de-initialization
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onDeinitialization();

	/**
	*  @brief
	*    Called in case the window size changed
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onResize();

	/**
	*  @brief
	*    Called in case the fullscreen state should be toggled
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onToggleFullscreenState();

	/**
	*  @brief
	*    Update the logic
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onUpdate();

	/**
	*  @brief
	*    Draw request method
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onDrawRequest();

	/**
	*  @brief
	*    Draw method
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onDraw();


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
protected:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] windowTitle
	*    ASCII window title, can be a null pointer
	*/
	explicit IApplication(const char* windowTitle);

	explicit IApplication(const IApplication& source) = delete;
	IApplication& operator =(const IApplication& source) = delete;


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	IApplicationImpl* mApplicationImpl;	///< Application implementation instance, always valid
	bool			  mExit;			///< If "true", the application has been asked politely to shut down as soon as possible, else "false"


};


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "IApplication.inl"
