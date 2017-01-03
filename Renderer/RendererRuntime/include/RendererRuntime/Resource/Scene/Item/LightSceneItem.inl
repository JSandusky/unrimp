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
#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline LightSceneItem::LightType LightSceneItem::getLightType() const
	{
		return mLightType;
	}

	inline void LightSceneItem::setLightType(LightType lightType)
	{
		mLightType = lightType;

		// Sanity checks
		assert(mLightType == LightType::DIRECTIONAL || mRadius > 0.0f);
		assert(mLightType != LightType::DIRECTIONAL || 0.0f == mRadius);
	}

	inline void LightSceneItem::setLightTypeAndRadius(LightType lightType, float radius)
	{
		mLightType = lightType;
		mRadius = radius;

		// Sanity checks
		assert(mLightType == LightType::DIRECTIONAL || mRadius > 0.0f);
		assert(mLightType != LightType::DIRECTIONAL || 0.0f == mRadius);
	}

	inline const glm::vec3& LightSceneItem::getColor() const
	{
		return mColor;
	}

	inline void LightSceneItem::setColor(const glm::vec3& color)
	{
		mColor = color;

		// Sanity checks
		assert(mColor.r >= 0.0f && mColor.g >= 0.0f && mColor.b >= 0.0f);
	}

	inline float LightSceneItem::getRadius() const
	{
		return mRadius;
	}

	inline void LightSceneItem::setRadius(float radius)
	{
		mRadius = radius;

		// Sanity checks
		assert(mLightType == LightType::DIRECTIONAL || mRadius > 0.0f);
		assert(mLightType != LightType::DIRECTIONAL || 0.0f == mRadius);
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	inline SceneItemTypeId LightSceneItem::getSceneItemTypeId() const
	{
		return TYPE_ID;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline LightSceneItem::LightSceneItem(ISceneResource& sceneResource) :
		ISceneItem(sceneResource),
		mLightType(LightType::POINT),
		mColor(1.0f, 1.0f, 1.0f),
		mRadius(1.0f)
	{
		// Nothing here
	}

	inline LightSceneItem::~LightSceneItem()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
