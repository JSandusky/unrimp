/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#ifndef __IAPPLICATION_H__
#define __IAPPLICATION_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/PlatformTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
class IApplicationImpl;


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
	*    Return the window size
	*
	*  @param[out] width
	*    Receives the window width
	*  @param[out] height
	*    Receives the window height
	*/
	inline void getWindowSize(int &width, int &height) const;

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
	*    Called in case a key is now pressed
	*
	*  @param[in] key
	*    Pressed key
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onKeyDown(unsigned int key);

	/**
	*  @brief
	*    Called in case a key was releaed
	*
	*  @param[in] key
	*    Released key
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onKeyUp(unsigned int key);

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
	explicit IApplication(const char *windowTitle);

	/**
	*  @brief
	*    Copy constructor
	*
	*  @param[in] source
	*    Source to copy from
	*/
	explicit IApplication(const IApplication &source);

	/**
	*  @brief
	*    Copy operator
	*
	*  @param[in] source
	*    Source to copy from
	*
	*  @return
	*    Reference to this instance
	*/
	IApplication &operator =(const IApplication &source);


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	IApplicationImpl *mApplicationImpl;	///< Application implementation instance, always valid


};


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "IApplication.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __IAPPLICATION_H__
