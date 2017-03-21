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
		return static_cast<LightType>(static_cast<int>(mPackedShaderData.lightType));
	}

	inline void LightSceneItem::setLightType(LightType lightType)
	{
		mPackedShaderData.lightType = static_cast<float>(lightType);

		// Sanity checks
		assert(lightType == LightType::DIRECTIONAL || mPackedShaderData.radius > 0.0f);
		assert(lightType != LightType::DIRECTIONAL || 0.0f == mPackedShaderData.radius);
	}

	inline void LightSceneItem::setLightTypeAndRadius(LightType lightType, float radius)
	{
		mPackedShaderData.lightType = static_cast<float>(lightType);
		mPackedShaderData.radius = radius;

		// Sanity checks
		assert(lightType == LightType::DIRECTIONAL || mPackedShaderData.radius > 0.0f);
		assert(lightType != LightType::DIRECTIONAL || 0.0f == mPackedShaderData.radius);
	}

	inline const glm::vec3& LightSceneItem::getColor() const
	{
		return mPackedShaderData.color;
	}

	inline void LightSceneItem::setColor(const glm::vec3& color)
	{
		mPackedShaderData.color = color;

		// Sanity checks
		assert(mPackedShaderData.color.r >= 0.0f && mPackedShaderData.color.g >= 0.0f && mPackedShaderData.color.b >= 0.0f);
	}

	inline float LightSceneItem::getRadius() const
	{
		return mPackedShaderData.radius;
	}

	inline void LightSceneItem::setRadius(float radius)
	{
		mPackedShaderData.radius = radius;

		// Sanity checks
		assert(mPackedShaderData.lightType == static_cast<float>(LightType::DIRECTIONAL) || mPackedShaderData.radius > 0.0f);
		assert(mPackedShaderData.lightType != static_cast<float>(LightType::DIRECTIONAL) || 0.0f == mPackedShaderData.radius);
	}

	inline float LightSceneItem::getInnerAngle() const
	{
		return mInnerAngle;
	}

	inline void LightSceneItem::setInnerAngle(float innerAngle)
	{
		mInnerAngle = innerAngle;

		// Derive data
		mPackedShaderData.innerAngle = glm::cos(glm::radians(mInnerAngle));

		// Sanity checks
		assert(mInnerAngle >= 0.0f);
		assert(mInnerAngle < mOuterAngle);
	}

	inline float LightSceneItem::getOuterAngle() const
	{
		return mOuterAngle;
	}

	inline void LightSceneItem::setOuterAngle(float outerAngle)
	{
		mOuterAngle = outerAngle;

		// Derive data
		mPackedShaderData.outerAngle = glm::cos(glm::radians(mOuterAngle));

		// Sanity checks
		assert(mOuterAngle < 90.0f);
		assert(mInnerAngle < mOuterAngle);
	}

	inline void LightSceneItem::setInnerOuterAngle(float innerAngle, float outerAngle)
	{
		mInnerAngle = innerAngle;
		mOuterAngle = outerAngle;

		// Derive data
		mPackedShaderData.innerAngle = glm::cos(glm::radians(mInnerAngle));
		mPackedShaderData.outerAngle = glm::cos(glm::radians(mOuterAngle));

		// Sanity checks
		assert(mInnerAngle >= 0.0f);
		assert(mOuterAngle < 90.0f);
		assert(mInnerAngle < mOuterAngle);
	}

	inline float LightSceneItem::getNearClipDistance() const
	{
		return mPackedShaderData.nearClipDistance;
	}

	inline void LightSceneItem::setNearClipDistance(float nearClipDistance)
	{
		mPackedShaderData.nearClipDistance = nearClipDistance;

		// Sanity check
		assert(mPackedShaderData.nearClipDistance >= 0.0f);
	}

	inline bool LightSceneItem::isVisible() const
	{
		return (mPackedShaderData.visible != 0);
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	inline SceneItemTypeId LightSceneItem::getSceneItemTypeId() const
	{
		return TYPE_ID;
	}

	inline void LightSceneItem::setVisible(bool visible)
	{
		mPackedShaderData.visible = static_cast<uint32_t>(visible);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline LightSceneItem::LightSceneItem(ISceneResource& sceneResource) :
		ISceneItem(sceneResource),
		mInnerAngle(40.0f),
		mOuterAngle(50.0f)
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
