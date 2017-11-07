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
//[ Public methods                                        ]
//[-------------------------------------------------------]
inline ExampleBase::~ExampleBase()
{
	// Nothing here
}

inline Renderer::ILog* ExampleBase::getCustomLog() const
{
	return mCustomLog;
}

inline void ExampleBase::initialize()
{
	if (!mInitialized)
	{
		onInitialization();
		mInitialized = true;
	}
}

inline void ExampleBase::deinitialize()
{
	if (mInitialized)
	{
		onDeinitialization();
		mInitialized = false;
	}
}

inline void ExampleBase::draw()
{
	onDraw();
}

inline void ExampleBase::setApplicationFrontend(IApplicationFrontend* applicationFrontend)
{
	mApplicationFrontend = applicationFrontend;
}


//[-------------------------------------------------------]
//[ Public virtual ExampleBase methods                    ]
//[-------------------------------------------------------]
inline void ExampleBase::onInitialization()
{
	// Base does nothing
}

inline void ExampleBase::onDeinitialization()
{
	// Base does nothing
}

inline void ExampleBase::onKeyDown(uint32_t)
{
	// Base does nothing
}

inline void ExampleBase::onKeyUp(uint32_t)
{
	// Base does nothing
}

inline void ExampleBase::onMouseButtonDown(uint32_t)
{
	// Base does nothing
}

inline void ExampleBase::onMouseButtonUp(uint32_t)
{
	// Base does nothing
}

inline void ExampleBase::onMouseWheel(float)
{
	// Base does nothing
}

inline void ExampleBase::onMouseMove(int, int)
{
	// Base does nothing
}

inline void ExampleBase::onUpdate()
{
	// Base does nothing
}

inline void ExampleBase::onDraw()
{
	// Base does nothing
}

inline bool ExampleBase::doesCompleteOwnDrawing() const
{
	// Default implementation does not complete own drawing
	return false;
}


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
inline ExampleBase::ExampleBase() :
	mCustomLog(nullptr),
	mInitialized(false),
	mApplicationFrontend(nullptr)
{
	// Nothing here
}

inline void ExampleBase::setCustomLog(Renderer::ILog* customLog)
{
	mCustomLog = customLog;
}
