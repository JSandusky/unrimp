/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererRuntime/Core/NonCopyable.h"

#include <inttypes.h>	// For uint32_t, uint64_t etc.


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderer;
}
namespace RendererRuntime
{
	class Transform;
	class MaterialResource;
}


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
	*    Abstract material blueprint resource listener interface
	*/
	class IMaterialBlueprintResourceListener : public NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Friends methods                                       ]
	//[-------------------------------------------------------]
		friend class MaterialBlueprintResource;	///< Is calling the private interface methods


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline IMaterialBlueprintResourceListener();
		inline virtual ~IMaterialBlueprintResourceListener();
		IMaterialBlueprintResourceListener(const IMaterialBlueprintResourceListener&) = delete;
		IMaterialBlueprintResourceListener& operator=(const IMaterialBlueprintResourceListener&) = delete;


	//[-------------------------------------------------------]
	//[ Private virtual RendererRuntime::IMaterialBlueprintResourceListener methods ]
	//[-------------------------------------------------------]
	private:
		virtual void beginFillUnknown() = 0;
		virtual bool fillUnknownValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) = 0;
		virtual void beginFillPass(Renderer::IRenderer& renderer, const Transform& worldSpaceToViewSpaceTransform) = 0;
		virtual bool fillPassValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) = 0;
		virtual void beginFillMaterial() = 0;
		virtual bool fillMaterialValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) = 0;

		// TODO(co) It might make sense to remove those instance methods from the interface and directly hard-code them for performance reasons. Profiling later on with real world scenes will show.
		virtual void beginFillInstance(const Transform& objectSpaceToWorldSpaceTransform, MaterialResource& materialResource) = 0;
		virtual bool fillInstanceValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) = 0;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.inl"
