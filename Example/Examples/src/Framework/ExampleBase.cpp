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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PrecompiledHeader.h"
#include "Framework/ExampleBase.h"


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
ExampleBase::~ExampleBase()
{
	// Nothing here
}

void ExampleBase::initialize()
{
	if (!mInitialized)
	{
		onInitialization();
		mInitialized = true;
	}
}

void ExampleBase::deinitialize()
{
	if (mInitialized)
	{
		onDeinitialization();
		mInitialized = false;
	}
}

void ExampleBase::draw()
{
	onDraw();
}

void ExampleBase::setApplicationFrontend(IApplicationFrontend* applicationFrontend)
{
	mApplicationFrontend = applicationFrontend;
}


//[-------------------------------------------------------]
//[ Public virtual ExampleBase methods                    ]
//[-------------------------------------------------------]
void ExampleBase::onInitialization()
{
	// Base does nothing
}

void ExampleBase::onDeinitialization()
{
	// Base does nothing
}

void ExampleBase::onKeyDown(uint32_t)
{
	// Base does nothing
}

void ExampleBase::onKeyUp(uint32_t)
{
	// Base does nothing
}

void ExampleBase::onMouseButtonDown(uint32_t)
{
	// Base does nothing
}

void ExampleBase::onMouseButtonUp(uint32_t)
{
	// Base does nothing
}

void ExampleBase::onMouseMove(int, int)
{
	// Base does nothing
}

void ExampleBase::onUpdate()
{
	// Base does nothing
}

void ExampleBase::onDraw()
{
	// Base does nothing
}


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
ExampleBase::ExampleBase() :
	mInitialized(false),
	mApplicationFrontend(nullptr)
{
	// Nothing here
}
