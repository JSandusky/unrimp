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
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class Transform;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class CameraSceneItem final : public ISceneItem
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
		RENDERERRUNTIME_API_EXPORT static const float DEFAULT_FOV_Y;	///< Default Y field of view in radians
		RENDERERRUNTIME_API_EXPORT static const float DEFAULT_NEAR_Z;
		RENDERERRUNTIME_API_EXPORT static const float DEFAULT_FAR_Z;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Data                                                  ]
		//[-------------------------------------------------------]
		inline float getFovY() const;
		inline void setFovY(float fovY);
		inline float getNearZ() const;
		inline void setNearZ(float nearZ);
		inline float getFarZ() const;
		inline void setFarZ(float farZ);

		//[-------------------------------------------------------]
		//[ Derived or custom data                                ]
		//[-------------------------------------------------------]
		// World space to view space matrix (Aka "view matrix")
		RENDERERRUNTIME_API_EXPORT const Transform& getWorldSpaceToViewSpaceTransform() const;
		RENDERERRUNTIME_API_EXPORT const Transform& getPreviousWorldSpaceToViewSpaceTransform() const;
		RENDERERRUNTIME_API_EXPORT const glm::mat4& getWorldSpaceToViewSpaceMatrix() const;
		RENDERERRUNTIME_API_EXPORT void getPreviousWorldSpaceToViewSpaceMatrix(glm::mat4& previousWorldSpaceToViewSpaceMatrix) const;
		inline bool hasCustomWorldSpaceToViewSpaceMatrix() const;
		inline void unsetCustomWorldSpaceToViewSpaceMatrix();
		inline void setCustomWorldSpaceToViewSpaceMatrix(const glm::mat4& customWorldSpaceToViewSpaceMatrix);

		// View space to clip space matrix (aka "projection matrix")
		RENDERERRUNTIME_API_EXPORT const glm::mat4& getViewSpaceToClipSpaceMatrix(float aspectRatio) const;
		RENDERERRUNTIME_API_EXPORT const glm::mat4& getViewSpaceToClipSpaceMatrixReversedZ(float aspectRatio) const;
		inline bool hasCustomViewSpaceToClipSpaceMatrix() const;
		inline void unsetCustomViewSpaceToClipSpaceMatrix();
		inline void setCustomViewSpaceToClipSpaceMatrix(const glm::mat4& customViewSpaceToClipSpaceMatrix, const glm::mat4& customViewSpaceToClipSpaceMatrixReversedZ);


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	public:
		inline virtual SceneItemTypeId getSceneItemTypeId() const override;
		virtual void deserialize(uint32_t numberOfBytes, const uint8_t* data) override;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		explicit CameraSceneItem(SceneResource& sceneResource);
		inline virtual ~CameraSceneItem() override;
		explicit CameraSceneItem(const CameraSceneItem&) = delete;
		CameraSceneItem& operator=(const CameraSceneItem&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Data
		float mFovY;	///< Y field of view in radians
		float mNearZ;
		float mFarZ;
		// Derived or custom data
		mutable glm::mat4 mWorldSpaceToViewSpaceMatrix;				// Aka "view matrix"
		mutable glm::mat4 mViewSpaceToClipSpaceMatrix;				// Aka "projection matrix"
		mutable glm::mat4 mViewSpaceToClipSpaceMatrixReversedZ;		// Aka "projection matrix"
		bool			  mHasCustomWorldSpaceToViewSpaceMatrix;	// Aka "view matrix"
		bool			  mHasCustomViewSpaceToClipSpaceMatrix;		// Aka "projection matrix"


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Scene/Item/Camera/CameraSceneItem.inl"
