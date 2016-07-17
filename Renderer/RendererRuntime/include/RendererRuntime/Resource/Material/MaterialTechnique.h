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
#include "RendererRuntime/Resource/Material/MaterialProperty.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
	class MaterialResource;
	class MaterialResourceManager;
	class MaterialBlueprintResource;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId AssetId;						///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"
	typedef uint32_t TextureResourceId;				///< POD texture resource identifier
	typedef StringId MaterialPropertyId;			///< Material property identifier, internally just a POD "uint32_t", result of hashing the property name
	typedef StringId MaterialTechniqueId;			///< Material technique identifier, internally just a POD "uint32_t", result of hashing the property name
	typedef uint32_t MaterialResourceId;			///< POD material resource identifier
	typedef uint32_t MaterialBlueprintResourceId;	///< POD material blueprint resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Material technique
	*/
	class MaterialTechnique
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MaterialResource;			// TODO(co) Remove
		friend class MaterialResourceLoader;	// TODO(co) Remove
		friend class MaterialResourceManager;	// TODO(co) Remove
		friend class MaterialBlueprintResource;	///< Sets "RendererRuntime::MaterialTechnique::mMaterialUniformBufferIndex"


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		struct Texture
		{
			uint32_t		  rootParameterIndex;
			MaterialProperty  materialProperty;
			TextureResourceId textureResourceId;
		};
		typedef std::vector<Texture> Textures;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] materialTechniqueId
		*    Material technique ID
		*  @param[in] materialResource
		*    Owner material resource, only material resource manager and material resource ID will internally be stored
		*/
		MaterialTechnique(MaterialTechniqueId materialTechniqueId, MaterialResource& materialResource);

		/**
		*  @brief
		*    Destructor
		*/
		inline ~MaterialTechnique();

		/**
		*  @brief
		*    Return the material technique ID
		*
		*  @return
		*    The material technique ID
		*/
		inline MaterialTechniqueId getMaterialTechniqueId() const;

		/**
		*  @brief
		*    Return the owner material resource manager
		*
		*  @return
		*    The owner material resource manager
		*/
		inline MaterialResourceManager& getMaterialResourceManager() const;

		/**
		*  @brief
		*    Return the owner material resource ID
		*
		*  @return
		*    The owner material resource ID
		*/
		inline MaterialResourceId getMaterialResourceId() const;

		/**
		*  @brief
		*    Return the owner material resource instance
		*
		*  @return
		*    The owner material resource instance
		*
		*  @note
		*    - Ease of use method
		*/
		const MaterialResource& getMaterialResource() const;

		/**
		*  @brief
		*    Return the ID of the used material blueprint resource
		*
		*  @return
		*    The ID of the used material blueprint resource, can be uninitialized
		*/
		inline MaterialBlueprintResourceId getMaterialBlueprintResourceId() const;

		/**
		*  @brief
		*    Return the material uniform buffer index inside the used material blueprint
		*
		*  @return
		*    The material uniform buffer index inside the used material blueprint, can be uninitialized value
		*/
		inline uint32_t getMaterialUniformBufferIndex() const;

		// TODO(co)
		void bindToRenderer(const IRendererRuntime& rendererRuntime);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		MaterialTechniqueId			mMaterialTechniqueId;			///< Material technique ID
		MaterialResourceManager*	mMaterialResourceManager;		///< Owner material resource manager, always valid
		MaterialResourceId			mMaterialResourceId;			///< Owner material resource ID, always valid
		MaterialBlueprintResourceId	mMaterialBlueprintResourceId;	///< Material blueprint resource ID, can be set to uninitialized value
		uint32_t					mMaterialUniformBufferIndex;	///< Material uniform buffer index inside the used material blueprint, can be set to uninitialized value
		Textures					mTextures;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Material/MaterialTechnique.inl"
