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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PrecompiledHeader.h"
#include "Runtime/FirstScene/IController.h"

#include <RendererRuntime/Core/GetUninitialized.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
IController::~IController()
{
	// Nothing here
}

RendererRuntime::CameraSceneItem& IController::getCameraSceneItem()
{
	return mCameraSceneItem;
}

void IController::onKeyDown(uint32_t key)
{
	mPressedKeys.insert(key);
}

void IController::onKeyUp(uint32_t key)
{
	mPressedKeys.erase(key);
}

void IController::onMouseButtonDown(uint32_t button)
{
	mPressedMouseButtons.insert(button);
}

void IController::onMouseButtonUp(uint32_t button)
{
	mPressedMouseButtons.erase(button);
}

void IController::onMouseMove(int x, int y)
{
	const static int MAXIMUM = 100;

	// X
	if (RendererRuntime::isInitialized(mMousePositionX))
	{
		mMouseMoveX = x - mMousePositionX;
		if (mMouseMoveX > MAXIMUM)
		{
			mMouseMoveX = MAXIMUM;
		}
	}
	mMousePositionX = x;

	// Y
	if (RendererRuntime::isInitialized(mMousePositionY))
	{
		mMouseMoveY = y - mMousePositionY;
		if (mMouseMoveY > MAXIMUM)
		{
			mMouseMoveY = MAXIMUM;
		}
	}
	mMousePositionY = y;
}


//[-------------------------------------------------------]
//[ Public virtual IController methods                    ]
//[-------------------------------------------------------]
void IController::onUpdate(float)
{
	// "Jedi gesture": There was no mouse movement
	mMouseMoveX = 0;
	mMouseMoveY = 0;
}


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
IController::IController(RendererRuntime::CameraSceneItem& cameraSceneItem) :
	mCameraSceneItem(cameraSceneItem),
	mMousePositionX(RendererRuntime::getUninitialized<int>()),
	mMousePositionY(RendererRuntime::getUninitialized<int>()),
	mMouseMoveX(0),
	mMouseMoveY(0)
{
	// Nothing here
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
bool IController::isKeyPressed(uint32_t key) const
{
	return (mPressedKeys.find(key) != mPressedKeys.cend());
}

bool IController::isMouseButtonPressed(uint32_t button) const
{
	return (mPressedMouseButtons.find(button) != mPressedMouseButtons.cend());
}
