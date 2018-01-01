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
//[ Public definitions                                    ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Keyboard key definitions, but only the ones we need inside the renderer examples application
	*
	*  @note
	*    - In real world applications, use a decent input library (which would be overkill for the simple renderer examples application)
	*/
	enum KeyboardKeys
	{
		LEFT_SHIFT_KEY		= 16,
		LEFT_STRG_KEY		= 17,
		ESCAPE_KEY			= 27,
		SPACE_KEY			= 32,
		PAGE_UP_KEY			= 33,
		PAGE_DOWN_KEY		= 34,
		ARROW_LEFT_KEY		= 37,
		ARROW_UP_KEY		= 38,
		ARROW_RIGHT_KEY		= 39,
		ARROW_DOWN_KEY		= 40,
		A_KEY				= 65,
		D_KEY				= 68,
		Q_KEY				= 81,
		S_KEY				= 83,
		W_KEY				= 87,
		NUMPAD_PLUS_KEY		= 107,
		NUMPAD_MINUS_KEY	= 109,
		OEM_PLUS_KEY		= 187,	///< +*~ key
		OEM_MINUS_KEY		= 189
	};


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
	*    Called in case a key is now pressed
	*
	*  @param[in] key
	*    Pressed key
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onKeyDown(uint32_t key);

	/**
	*  @brief
	*    Called in case a key was released
	*
	*  @param[in] key
	*    Released key
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onKeyUp(uint32_t key);

	/**
	*  @brief
	*    Called in case a mouse button is now pressed
	*
	*  @param[in] button
	*    Pressed mouse button
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onMouseButtonDown(uint32_t button);

	/**
	*  @brief
	*    Called in case a mouse button was released
	*
	*  @param[in] button
	*    Released mouse button
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onMouseButtonUp(uint32_t button);

	/**
	*  @brief
	*    Called in case a mouse wheel was used
	*
	*  @param[in] delta
	*    Mouse wheel delta
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onMouseWheel(float delta);

	/**
	*  @brief
	*    Called in case the mouse moved
	*
	*  @param[in] x
	*    Mouse position along the x-axis
	*  @param[in] y
	*    Mouse position along the y-axis
	*
	*  @note
	*    - The base implementation is empty
	*/
	virtual void onMouseMove(int x, int y);

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
