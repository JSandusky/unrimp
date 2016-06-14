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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/PlatformTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
class IApplication;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Abstract application implementation interface
*/
class IApplicationImpl
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Destructor
	*/
	virtual ~IApplicationImpl();

	/**
	*  @brief
	*    Return the owner application
	*
	*  @return
	*    The owner application
	*/
	IApplication& getApplication() const;


//[-------------------------------------------------------]
//[ Public virtual IApplicationImpl methods               ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Called on application initialization
	*/
	virtual void onInitialization() = 0;

	/**
	*  @brief
	*    Called on application de-initialization
	*/
	virtual void onDeinitialization() = 0;

	/**
	*  @brief
	*    Process OS messages (non-blocking)
	*
	*  @return
	*    "true" in case the application should shut down, else "false"
	*/
	virtual bool processMessages() = 0;

	/**
	*  @brief
	*    Return the window size
	*
	*  @param[out] width
	*    Receives the window width
	*  @param[out] height
	*    Receives the window height
	*/
	virtual void getWindowSize(int &width, int &height) const = 0;

	/**
	*  @brief
	*    Return the OS dependent window handle
	*
	*  @remarks
	*    The OS dependent window handle, can be a null handle
	*/
	virtual handle getNativeWindowHandle() const = 0;

	/**
	*  @brief
	*    Redraw request
	*/
	virtual void redraw() = 0;


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
protected:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] application
	*    Owner application
	*/
	explicit IApplicationImpl(IApplication& application);

	/**
	*  @brief
	*    Copy constructor
	*
	*  @param[in] source
	*    Source to copy from
	*/
	explicit IApplicationImpl(const IApplicationImpl &source);

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
	IApplicationImpl &operator =(const IApplicationImpl &source);


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	IApplication& mApplication;


};
