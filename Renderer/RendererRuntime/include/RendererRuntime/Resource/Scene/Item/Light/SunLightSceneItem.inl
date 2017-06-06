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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline float SunLightSceneItem::getSunriseTime() const
	{
		return mSunriseTime;
	}

	inline void SunLightSceneItem::setSunriseTime(float sunriseTime)
	{
		if (mSunriseTime != sunriseTime)
		{
			// Sanity check
			assert(sunriseTime >= 00.00f && sunriseTime < 24.00f);	// O'clock

			// Set new value and calculate derived sun light properties
			mSunriseTime = sunriseTime;
			calculatedDerivedSunLightProperties();
		}
	}

	inline float SunLightSceneItem::getSunsetTime() const
	{
		return mSunsetTime;
	}

	inline void SunLightSceneItem::setSunsetTime(float sunsetTime)
	{
		if (mSunsetTime != sunsetTime)
		{
			// Sanity check
			assert(sunsetTime >= 00.00f && sunsetTime < 24.00f);	// O'clock

			// Set new value and calculate derived sun light properties
			mSunsetTime = sunsetTime;
			calculatedDerivedSunLightProperties();
		}
	}

	inline float SunLightSceneItem::getEastDirection() const
	{
		return mEastDirection;
	}

	inline void SunLightSceneItem::setEastDirection(float eastDirection)
	{
		if (mEastDirection != eastDirection)
		{
			mEastDirection = eastDirection;
			calculatedDerivedSunLightProperties();
		}
	}

	inline float SunLightSceneItem::getAngleOfIncidence() const
	{
		return mAngleOfIncidence;
	}

	inline void SunLightSceneItem::setAngleOfIncidence(float angleOfIncidence)
	{
		if (mAngleOfIncidence != angleOfIncidence)
		{
			mAngleOfIncidence = angleOfIncidence;
			calculatedDerivedSunLightProperties();
		}
	}

	inline float SunLightSceneItem::getTimeOfDay() const
	{
		return mTimeOfDay;
	}

	void SunLightSceneItem::setTimeOfDay(float timeOfDay)
	{
		if (mTimeOfDay != timeOfDay)
		{
			// Sanity check
			assert(timeOfDay >= 00.00f && timeOfDay < 24.00f);	// O'clock

			// Set new value and calculate derived sun light properties
			mTimeOfDay = timeOfDay;
			calculatedDerivedSunLightProperties();
		}
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	inline SceneItemTypeId SunLightSceneItem::getSceneItemTypeId() const
	{
		return TYPE_ID;
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	inline void SunLightSceneItem::onAttachedToSceneNode(SceneNode& sceneNode)
	{
		// Call base implementation
		LightSceneItem::onAttachedToSceneNode(sceneNode);

		// Calculate derived sun light properties
		calculatedDerivedSunLightProperties();
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline SunLightSceneItem::SunLightSceneItem(SceneResource& sceneResource) :
		LightSceneItem(sceneResource),
		// Usually fixed
		mSunriseTime(07.50f),	// O'clock
		mSunsetTime(20.50f),	// O'clock
		mEastDirection(0.0f),
		mAngleOfIncidence(glm::radians(20.0f)),
		// Usually animated
		mTimeOfDay(10.00f)		// O'clock
	{
		setLightTypeAndRadius(LightType::DIRECTIONAL, 0.0f);
	}

	inline SunLightSceneItem::~SunLightSceneItem()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
