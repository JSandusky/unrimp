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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"

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
	*    Material blueprint resource listener
	*/
	class MaterialBlueprintResourceListener : public IMaterialBlueprintResourceListener
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		inline MaterialBlueprintResourceListener();

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~MaterialBlueprintResourceListener();


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		MaterialBlueprintResourceListener(const MaterialBlueprintResourceListener&) = delete;
		MaterialBlueprintResourceListener& operator=(const MaterialBlueprintResourceListener&) = delete;
		inline const PassBufferManager::PassData& getPassData() const;	// Memory address received via "RendererRuntime::MaterialBlueprintResourceListener::beginFillPass()", can be invalid outside the correct scope, don't destroy the memory


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IMaterialBlueprintResourceListener methods ]
	//[-------------------------------------------------------]
	protected:
		inline virtual void beginFillUnknown() override;
		inline virtual bool fillUnknownValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) override;
		RENDERERRUNTIME_API_EXPORT virtual void beginFillPass(IRendererRuntime& rendererRuntime, const Renderer::IRenderTarget& renderTarget, const CompositorContextData& compositorContextData, PassBufferManager::PassData& passData) override;
		RENDERERRUNTIME_API_EXPORT virtual bool fillPassValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) override;
		inline virtual void beginFillMaterial() override;
		inline virtual bool fillMaterialValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) override;
		inline virtual void beginFillInstance(const PassBufferManager::PassData& passData, const Transform& objectSpaceToWorldSpaceTransform, MaterialTechnique& materialTechnique) override;
		RENDERERRUNTIME_API_EXPORT virtual bool fillInstanceValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Pass
		IRendererRuntime*			 mRendererRuntime;	///< Memory address received via "RendererRuntime::MaterialBlueprintResourceListener::beginFillPass()", can be a null pointer outside the correct scope, don't destroy the memory
		PassBufferManager::PassData* mPassData;			///< Memory address received via "RendererRuntime::MaterialBlueprintResourceListener::beginFillPass()", can be a null pointer outside the correct scope, don't destroy the memory
		const CompositorContextData* mCompositorContextData;
		uint32_t					 mRenderTargetWidth;
		uint32_t					 mRenderTargetHeight;
		float						 mNearZ;
		float						 mFarZ;

		// Instance
		const Transform*   mObjectSpaceToWorldSpaceTransform;
		MaterialTechnique* mMaterialTechnique;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/MaterialBlueprintResourceListener.inl"
