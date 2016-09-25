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
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4201)	// warning C4201: nonstandard extension used: nameless struct/union
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#pragma warning(disable: 4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/glm.hpp>
#pragma warning(pop)


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


	//[-------------------------------------------------------]
	//[ Private virtual RendererRuntime::IMaterialBlueprintResourceListener methods ]
	//[-------------------------------------------------------]
	private:
		inline virtual void beginFillUnknown() override;
		inline virtual bool fillUnknownValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) override;
		virtual void beginFillPass(IRendererRuntime& rendererRuntime, const Transform& worldSpaceToViewSpaceTransform) override;
		virtual bool fillPassValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) override;
		inline virtual void beginFillMaterial() override;
		inline virtual bool fillMaterialValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) override;
		inline virtual void beginFillInstance(const Transform& objectSpaceToWorldSpaceTransform, MaterialTechnique& materialTechnique) override;
		virtual bool fillInstanceValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Pass
		glm::mat4 mWorldSpaceToViewSpaceMatrix;
		glm::mat4 mWorldSpaceToClipSpaceMatrix;

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
