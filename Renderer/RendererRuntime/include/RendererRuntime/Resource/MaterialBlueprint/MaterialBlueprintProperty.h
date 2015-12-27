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
#include "RendererRuntime/Resource/Material/MaterialProperty.h"


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
	*    Material blueprint property
	*/
	class MaterialBlueprintProperty : public MaterialProperty
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Usage
		*/
		enum class Usage : uint8_t
		{
			UNKNOWN = 0,			///< Usage not known
			STATIC,					///< Property is considered to not change regularly and results in shader permutations
			DYNAMIC,				///< Property is considered to change regularly and hence will be handled as shader uniform managed in a combined uniform buffer
			RASTERIZER_STATE,		///< Pipeline rasterizer state, property is considered to not change regularly
			DEPTH_STENCIL_STATE,	///< Pipeline depth stencil state, property is considered to not change regularly
			BLEND_STATE,			///< Pipeline blend state, property is considered to not change regularly
			SAMPLER_STATE,			///< Sampler state, property is considered to not change regularly
			TEXTURE					///< Property is a texture reference, property is considered to not change regularly
		};


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] materialPropertyId
		*    Material property ID
		*  @param[in] materialPropertyValue
		*    Material property value
		*/
		MaterialBlueprintProperty(MaterialPropertyId materialPropertyId, const MaterialPropertyValue& materialPropertyValue);

		/**
		*  @brief
		*    Destructor
		*/
		inline ~MaterialBlueprintProperty();

		/**
		*  @brief
		*    Return the material blueprint property usage
		*
		*  @return
		*    The material blueprint property usage
		*/
		inline Usage getUsage() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		MaterialBlueprintProperty(const MaterialBlueprintProperty&) = delete;
		MaterialBlueprintProperty& operator=(const MaterialBlueprintProperty&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Usage mUsage;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintProperty.inl"
