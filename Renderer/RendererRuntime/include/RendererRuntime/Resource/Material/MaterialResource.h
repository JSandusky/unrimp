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
#include "RendererRuntime/Resource/Detail/IResource.h"
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/Material/MaterialProperties.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	template <class ELEMENT_TYPE, typename ID_TYPE> class PackedElementManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t MaterialResourceId;	///< POD material resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Material resource
	*/
	class MaterialResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MaterialResourceLoader;
		friend class MaterialResourceManager;
		friend class PackedElementManager<MaterialResource, MaterialResourceId>;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<MaterialTechnique> SortedMaterialTechniqueVector;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the sorted material technique vector
		*
		*  @return
		*    The sorted material technique vector
		*/
		inline const SortedMaterialTechniqueVector& getSortedMaterialTechniqueVector() const;

		/**
		*  @brief
		*    Return a material technique by its ID
		*
		*  @param[in] materialTechniqueId
		*    ID of the material technique to return
		*
		*  @return
		*    The requested material technique, null pointer on error, don't destroy the returned instance
		*/
		MaterialTechnique* getMaterialTechniqueById(MaterialTechniqueId materialTechniqueId) const;

		/**
		*  @brief
		*    Return the material properties
		*
		*  @return
		*    The material properties
		*
		*  @notes
		*    - Contains all properties of all material blueprints referenced by this material (material property inheritance is handled elsewhere)
		*/
		inline const MaterialProperties& getMaterialProperties() const;

		/**
		*  @brief
		*    Return the material properties
		*
		*  @return
		*    The material properties
		*
		*  @notes
		*    - Contains all properties of all material blueprints referenced by this material (material property inheritance is handled elsewhere)
		*/
		inline MaterialProperties& getMaterialProperties();

		// TODO(co)
		void releaseTextures();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Default constructor
		*/
		inline MaterialResource();

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] materialResourceId
		*    Material resource ID
		*/
		inline explicit MaterialResource(MaterialResourceId materialResourceId);

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~MaterialResource();

		MaterialResource(const MaterialResource&) = delete;
		MaterialResource& operator=(const MaterialResource&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		SortedMaterialTechniqueVector mSortedMaterialTechniqueVector;
		MaterialProperties			  mMaterialProperties;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Material/MaterialResource.inl"
