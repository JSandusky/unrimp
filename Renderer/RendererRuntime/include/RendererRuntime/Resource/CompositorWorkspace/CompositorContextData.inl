/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline CompositorContextData::CompositorContextData() :
		mCameraSceneItem(nullptr),
		mLightSceneItem(nullptr),
		mCompositorInstancePassShadowMap(nullptr)
	{
		// Nothing here
	}

	inline CompositorContextData::CompositorContextData(const CameraSceneItem* cameraSceneItem, const LightSceneItem* lightSceneItem, const CompositorInstancePassShadowMap* compositorInstancePassShadowMap) :
		mCameraSceneItem(cameraSceneItem),
		mLightSceneItem(lightSceneItem),
		mCompositorInstancePassShadowMap(compositorInstancePassShadowMap)
	{
		// Nothing here
	}

	inline CompositorContextData::~CompositorContextData()
	{
		// Nothing here
	}

	inline const CameraSceneItem* CompositorContextData::getCameraSceneItem() const
	{
		return mCameraSceneItem;
	}

	inline const LightSceneItem* CompositorContextData::getLightSceneItem() const
	{
		return mLightSceneItem;
	}

	inline const CompositorInstancePassShadowMap* CompositorContextData::getCompositorInstancePassShadowMap() const
	{
		return mCompositorInstancePassShadowMap;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
