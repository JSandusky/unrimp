/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Resource/Scene/Item/ISceneItem.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4201)	// warning C4201: nonstandard extension used: nameless struct/union
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/glm.hpp>
PRAGMA_WARNING_POP


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
	*    Light scene item
	*/
	class LightSceneItem : public ISceneItem
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class SceneFactory;			// Needs to be able to create scene item instances
		friend class LightBufferManager;	// Needs access to "RendererRuntime::LightSceneItem::mPackedShaderData"


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT static const SceneItemTypeId TYPE_ID;
		enum class LightType
		{
			DIRECTIONAL = 0,
			POINT,
			SPOT
		};


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline LightType getLightType() const;
		inline void setLightType(LightType lightType);
		inline void setLightTypeAndRadius(LightType lightType, float radius);
		inline const glm::vec3& getColor() const;
		inline void setColor(const glm::vec3& color);
		inline float getRadius() const;
		inline void setRadius(float radius);
		inline float getInnerAngle() const;
		inline void setInnerAngle(float innerAngle);
		inline float getOuterAngle() const;
		inline void setOuterAngle(float outerAngle);
		inline void setInnerOuterAngle(float innerAngle, float outerAngle);
		inline float getNearClipDistance() const;
		inline void setNearClipDistance(float nearClipDistance);
		inline bool isVisible() const;


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	public:
		inline virtual SceneItemTypeId getSceneItemTypeId() const override;
		virtual void deserialize(uint32_t numberOfBytes, const uint8_t* data) override;
		inline virtual void setVisible(bool visible) override;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline explicit LightSceneItem(SceneResource& sceneResource);
		inline virtual ~LightSceneItem() override;
		explicit LightSceneItem(const LightSceneItem&) = delete;
		LightSceneItem& operator=(const LightSceneItem&) = delete;


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Light data packed into a form which can be directly 1:1 copied into a GPU buffer; don't change the layout in here without updating the shaders using the data
		*/
		struct PackedShaderData final
		{
			// float4 0: xyz = world space light position, w = light radius
			glm::vec3 position{0.0f, 0.0f, 0.0f};	///< Parent scene node world space position
			float	  radius = 1.0f;
			// float4 1: xyz = RGB light diffuse color, w = unused
			glm::vec3 color{1.0f, 1.0f, 1.0f};
			float	  lightType = static_cast<float>(LightType::POINT);
			// float4 2: Only used for spot-light: x = spot-light inner angle in radians, y = spot-light outer angle in radians, z = spot-light near clip distance, w = unused
			float innerAngle       = 0.0f;	///< Cosine of the inner angle in radians; interval in degrees: 0..90, must be smaller as the outer angle
			float outerAngle       = 0.0f;	///< Cosine of the outer angle in radians; interval in degrees: 0..90, must be greater as the inner angle
			float nearClipDistance = 0.0f;
			float unused           = 0.0f;
			// float4 3: Only used for spot-light: xyz = normalized view space light direction, w = unused
			glm::vec3 direction{0.0f, 0.0f, 1.0f};	///< Derived from the parent scene node world space rotation
			uint32_t  visible = 1;					///< Boolean, not used inside the shader but well, there's currently space left in here so we're using it
		};


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		PackedShaderData mPackedShaderData;
		float			 mInnerAngle;	///< Inner angle in radians; interval in degrees: 0..90, must be smaller as the outer angle
		float			 mOuterAngle;	///< Outer angle in radians; interval in degrees: 0..90, must be greater as the inner angle


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Scene/Item/Light/LightSceneItem.inl"
