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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Scene/Item/LightSceneItem.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h"

#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const SceneItemTypeId LightSceneItem::TYPE_ID("LightSceneItem");


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	void LightSceneItem::deserialize(uint32_t numberOfBytes, const uint8_t* data)
	{
		assert(sizeof(v1Scene::LightItem) == numberOfBytes);
		std::ignore = numberOfBytes;

		// Read data
		const v1Scene::LightItem* lightItem = reinterpret_cast<const v1Scene::LightItem*>(data);
		mPackedShaderData.lightType = static_cast<float>(lightItem->lightType);
		memcpy(glm::value_ptr(mPackedShaderData.color), lightItem->color, sizeof(float) * 3);
		mPackedShaderData.radius = lightItem->radius;
		setInnerOuterAngle(lightItem->innerAngle, lightItem->outerAngle);
		mPackedShaderData.nearClipDistance = lightItem->nearClipDistance;

		// Sanity checks
		assert(mPackedShaderData.color.r >= 0.0f && mPackedShaderData.color.g >= 0.0f && mPackedShaderData.color.b >= 0.0f);
		assert(lightItem->lightType == LightType::DIRECTIONAL || mPackedShaderData.radius > 0.0f);
		assert(lightItem->lightType != LightType::DIRECTIONAL || 0.0f == mPackedShaderData.radius);
		assert(mInnerAngle >= 0.0f);
		assert(mOuterAngle < glm::radians(90.0f));
		assert(mInnerAngle < mOuterAngle);
		assert(mPackedShaderData.nearClipDistance >= 0.0f);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
