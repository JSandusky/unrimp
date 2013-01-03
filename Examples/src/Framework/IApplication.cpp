/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/IApplication.h"
#include "Framework/ApplicationImplWindows.h"


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
/**
*  @brief
*    Destructor
*/
IApplication::~IApplication()
{
	// Destroy the application instance
	delete mApplicationImpl;
}

/**
*  @brief
*    Run the application
*/
int IApplication::run()
{
	// Call application implementation initialization method
	mApplicationImpl->onInitialization();
	onInitialization();

	// Main loop - Process OS messages (non-blocking) first
	while (!mApplicationImpl->processMessages())
	{
		// Update the application logic
		onUpdate();

		// Redraw request
		redraw();
	}

	// Call application implementation de-initialization method
	onDeinitialization();
	mApplicationImpl->onDeinitialization();

	// Done, no error
	return 0;
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
/**
*  @brief
*    Called on application initialization
*/
void IApplication::onInitialization()
{
	// Nothing to do in here
}

/**
*  @brief
*    Called on application de-initialization
*/
void IApplication::onDeinitialization()
{
	// Nothing to do in here
}

/**
*  @brief
*    Called in case the window size changed
*/
void IApplication::onResize()
{
	// Nothing to do in here
}

/**
*  @brief
*    Called in case the fullscreen state should be toggled
*/
void IApplication::onToggleFullscreenState()
{
	// Nothing to do in here
}

/**
*  @brief
*    Called in case a key is now pressed
*/
void IApplication::onKeyDown(unsigned int)
{
	// Nothing to do in here
}

/**
*  @brief
*    Called in case a key was releaed
*/
void IApplication::onKeyUp(unsigned int)
{
	// Nothing to do in here
}

/**
*  @brief
*    Update the logic
*/
void IApplication::onUpdate()
{
	// Nothing to do in here
}

/**
*  @brief
*    Draw request method
*/
void IApplication::onDrawRequest()
{
	// Nothing to do in here
}

/**
*  @brief
*    Draw method
*/
void IApplication::onDraw()
{
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
IApplication::IApplication(const char *windowTitle) :
	mApplicationImpl(nullptr)
{
	// We're using "this" in here, so we are not allowed to write the following within the initializer list
	#ifdef WIN32
		mApplicationImpl = new ApplicationImplWindows(*this, windowTitle);
	#else
		#error "Unsupported platform"
	#endif
}

/**
*  @brief
*    Copy constructor
*/
IApplication::IApplication(const IApplication &)
{
	// Not supported
}

/**
*  @brief
*    Copy operator
*/
IApplication &IApplication::operator =(const IApplication &)
{
	// Not supported
	return *this;
}
