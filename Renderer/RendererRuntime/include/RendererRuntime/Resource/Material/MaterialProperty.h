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
#include "RendererRuntime/Resource/Material/MaterialPropertyValue.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId MaterialPropertyId;	///< Material property identifier, internally just a POD "uint32_t", result of hashing the property name


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Material property
	*/
	class MaterialProperty : public MaterialPropertyValue
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MaterialProperties;


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
			UNKNOWN = 0,					///< Usage not known
			STATIC,							///< Static property is used for fixed build in values which usually don't change during runtime (for example hard wired material blueprint textures, hard wired uniform buffer element values or static material properties which the renderer should ignore)
			SHADER_UNIFORM,					///< Dynamic property is considered to change regularly and hence will be handled as shader uniform managed in a combined uniform buffer
			SHADER_COMBINATION,				///< Static property is considered to not change regularly and results in shader combinations
			RASTERIZER_STATE,				///< Pipeline rasterizer state, property is considered to not change regularly
			DEPTH_STENCIL_STATE,			///< Pipeline depth stencil state, property is considered to not change regularly
			BLEND_STATE,					///< Pipeline blend state, property is considered to not change regularly
			SAMPLER_STATE,					///< Sampler state, property is considered to not change regularly
			TEXTURE_REFERENCE,				///< Property is a texture asset reference, property is considered to not change regularly
			COMPOSITOR_TEXTURE_REFERENCE,	///< Property is an automatic compositor texture reference, value type can be "INTEGER" (= by compositor quad input index) or "COMPOSITOR_TEXTURE_REFERENCE" (= by global/local compositor texture name, value syntax: "@<texture name>@<MRT-index>")
			SHADOW_TEXTURE_REFERENCE,		///< Property is an automatic shadow texture reference, value type can be "INTEGER" (= by shadow texture index)
			GLOBAL_REFERENCE,				///< Property is a global material property reference
			UNKNOWN_REFERENCE,				///< Property is an automatic unknown uniform buffer property reference
			PASS_REFERENCE,					///< Property is an automatic pass uniform buffer property reference
			MATERIAL_REFERENCE,				///< Property is a material uniform buffer property reference
			INSTANCE_REFERENCE				///< Property is an automatic instance uniform buffer property reference
		};


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		inline static MaterialPropertyValue materialPropertyValueFromReference(ValueType valueType, uint32_t reference);

		/**
		*  @brief
		*    Return whether or not the provided material blueprint property usage is a reference to something else
		*
		*  @param[in] usage
		*    Usage to check
		*
		*  @return
		*    "true" if the provided material blueprint property usage is a reference to something else, else "false"
		*/
		inline static bool isReferenceUsage(Usage usage);


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @note
		*    - For internal usage only
		*/
		inline MaterialProperty();

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] materialPropertyId
		*    Material property ID
		*  @param[in] usage
		*    Material property usage
		*  @param[in] materialPropertyValue
		*    Material property value
		*/
		inline MaterialProperty(MaterialPropertyId materialPropertyId, Usage usage, const MaterialPropertyValue& materialPropertyValue);

		/**
		*  @brief
		*    Destructor
		*/
		inline ~MaterialProperty();

		/**
		*  @brief
		*    Return the material property ID
		*
		*  @return
		*    The material property ID
		*/
		inline MaterialPropertyId getMaterialPropertyId() const;

		/**
		*  @brief
		*    Return the material blueprint property usage
		*
		*  @return
		*    The material blueprint property usage
		*/
		inline Usage getUsage() const;

		/**
		*  @brief
		*    Return whether or not this is an overwritten property
		*
		*  @return
		*    "true" if this is an overwritten property, else "false"
		*/
		inline bool isOverwritten() const;

		/**
		*  @brief
		*    Set whether or not this is an overwritten property
		*
		*  @param[in] overwritten
		*    "true" if this is an overwritten property, else "false"
		*
		*  @note
		*    - Usually you might not want to manually change the overwritten state
		*/
		inline void setOverwritten(bool overwritten);

		/**
		*  @brief
		*    Return whether or not the material blueprint property is a reference to something else
		*
		*  @return
		*    "true" if the material blueprint property is a reference to something else, else "false"
		*/
		inline bool isReferenceUsage() const;

		//[-------------------------------------------------------]
		//[ Value getter                                          ]
		//[-------------------------------------------------------]
		inline uint32_t getReferenceValue() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		MaterialPropertyId mMaterialPropertyId;
		Usage			   mUsage;
		bool			   mOverwritten;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Material/MaterialProperty.inl"
