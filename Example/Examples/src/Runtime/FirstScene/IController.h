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
#include "RendererRuntime/Core/NonCopyable.h"

#include <inttypes.h>	// For uint32_t, uint64_t etc.
#include <unordered_set>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class CameraSceneItem;
}


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Abstract controller interface
*
*  @note
*    - Remember: Unrimp is only about rendering and not about being a fully featured game engine, so just super basic stuff in here
*/
class IController : private RendererRuntime::NonCopyable
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Destructor
	*/
	virtual ~IController();

	RendererRuntime::CameraSceneItem& getCameraSceneItem();

	/**
	*  @brief
	*    Called in case a key is now pressed
	*
	*  @param[in] key
	*    Pressed key
	*/
	void onKeyDown(uint32_t key);

	/**
	*  @brief
	*    Called in case a key was released
	*
	*  @param[in] key
	*    Released key
	*/
	void onKeyUp(uint32_t key);

	/**
	*  @brief
	*    Called in case a mouse button is now pressed
	*
	*  @param[in] button
	*    Pressed mouse button
	*/
	void onMouseButtonDown(uint32_t button);

	/**
	*  @brief
	*    Called in case a mouse button was released
	*
	*  @param[in] button
	*    Released mouse button
	*/
	void onMouseButtonUp(uint32_t button);

	/**
	*  @brief
	*    Called in case the mouse moved
	*
	*  @param[in] x
	*    Mouse position along the x-axis
	*  @param[in] y
	*    Mouse position along the y-axis
	*/
	void onMouseMove(int x, int y);


//[-------------------------------------------------------]
//[ Public virtual IController methods                    ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Called on update request
	*
	*  @param[in] pastMilliseconds
	*    Past time in milliseconds
	*/
	virtual void onUpdate(float pastMilliseconds);


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
protected:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] cameraSceneItem
	*    Camera scene item to control, instance must stay valid as long as this controller instance exists
	*/
	explicit IController(RendererRuntime::CameraSceneItem& cameraSceneItem);

	IController(const IController&) = delete;
	IController& operator=(const IController&) = delete;
	bool isKeyPressed(uint32_t key) const;
	bool isMouseButtonPressed(uint32_t button) const;


//[-------------------------------------------------------]
//[ Protected definitions                                 ]
//[-------------------------------------------------------]
protected:
	typedef std::unordered_set<uint32_t> PressedKeys;
	typedef std::unordered_set<uint32_t> PressedMouseButtons;


//[-------------------------------------------------------]
//[ Protected data                                        ]
//[-------------------------------------------------------]
protected:
	RendererRuntime::CameraSceneItem& mCameraSceneItem;
	PressedKeys						  mPressedKeys;
	PressedMouseButtons				  mPressedMouseButtons;
	int								  mMousePositionX;
	int								  mMousePositionY;
	int								  mMouseMoveX;
	int								  mMouseMoveY;


};
