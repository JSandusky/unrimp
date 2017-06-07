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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Scene/Item/Light/LightSceneItem.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Sunlight scene item
	*
	*  @note
	*    - Automatically controls light parameters as well as the owner scene node rotation via time-of-day
	*/
	class SunlightSceneItem : public LightSceneItem
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class SceneFactory;	// Needs to be able to create scene item instances


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT static const SceneItemTypeId TYPE_ID;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Usually fixed                                         ]
		//[-------------------------------------------------------]
		inline float getSunriseTime() const;
		inline void setSunriseTime(float sunriseTime);
		inline float getSunsetTime() const;
		inline void setSunsetTime(float sunsetTime);
		inline float getEastDirection() const;
		inline void setEastDirection(float eastDirection);
		inline float getAngleOfIncidence() const;
		inline void setAngleOfIncidence(float angleOfIncidence);

		//[-------------------------------------------------------]
		//[ Usually animated                                      ]
		//[-------------------------------------------------------]
		inline float getTimeOfDay() const;
		inline void setTimeOfDay(float timeOfDay);


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	public:
		inline virtual SceneItemTypeId getSceneItemTypeId() const override;
		virtual void deserialize(uint32_t numberOfBytes, const uint8_t* data) override;


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	public:
		inline virtual void onAttachedToSceneNode(SceneNode& sceneNode) override;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline explicit SunlightSceneItem(SceneResource& sceneResource);
		inline virtual ~SunlightSceneItem();
		explicit SunlightSceneItem(const SunlightSceneItem&) = delete;
		SunlightSceneItem& operator=(const SunlightSceneItem&) = delete;
		RENDERERRUNTIME_API_EXPORT void calculatedDerivedSunlightProperties();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Usually fixed
		float mSunriseTime;			///< Sunrise time in "hour.minute"
		float mSunsetTime;			///< Sunset time in "hour.minute"
		float mEastDirection;		///< East direction in radians, clockwise orientation starting from north for zero
		float mAngleOfIncidence;	///< Angle of incidence in radians
		// Usually animated
		float mTimeOfDay;			///< Current time-of-day in "hour.minute"


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Scene/Item/Light/SunlightSceneItem.inl"
