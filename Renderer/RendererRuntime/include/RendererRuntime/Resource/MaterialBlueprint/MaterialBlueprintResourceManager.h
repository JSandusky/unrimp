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
#include "RendererRuntime/Resource/IResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialProperty.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
	class MaterialBlueprintResource;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class MaterialBlueprintResourceManager : private IResourceManager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<MaterialProperty> SortedGlobalMaterialPropertyVector;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		// TODO(co) Work-in-progress
		RENDERERRUNTIME_API_EXPORT MaterialBlueprintResource* loadMaterialBlueprintResourceByAssetId(AssetId assetId, bool reload = false);

		//[-------------------------------------------------------]
		//[ Global material properties                            ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the sorted global material property vector
		*
		*  @return
		*    The sorted global material property vector
		*/
		inline const SortedGlobalMaterialPropertyVector& getSortedGlobalMaterialPropertyVector() const;

		/**
		*  @brief
		*    Return a global material property by its ID
		*
		*  @param[in] materialPropertyId
		*    ID of the global material property to return
		*
		*  @return
		*    The requested global material property, null pointer on error, don't destroy the returned instance
		*/
		RENDERERRUNTIME_API_EXPORT const MaterialProperty* getGlobalMaterialPropertyById(MaterialPropertyId materialPropertyId) const;

		/**
		*  @brief
		*    Set a global material property value by its ID
		*
		*  @param[in] materialPropertyId
		*    ID of the global material property to set the value from
		*  @param[in] materialPropertyValue
		*    The material property value to set
		*/
		RENDERERRUNTIME_API_EXPORT void setGlobalMaterialPropertyById(MaterialPropertyId materialPropertyId, const MaterialPropertyValue& materialPropertyValue);


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	public:
		virtual void reloadResourceByAssetId(AssetId assetId) override;
		virtual void update() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit MaterialBlueprintResourceManager(IRendererRuntime& rendererRuntime);
		virtual ~MaterialBlueprintResourceManager();
		MaterialBlueprintResourceManager(const MaterialBlueprintResourceManager&) = delete;
		MaterialBlueprintResourceManager& operator=(const MaterialBlueprintResourceManager&) = delete;
		IResourceLoader* acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&				   mRendererRuntime;						///< Renderer runtime instance, do not destroy the instance
		SortedGlobalMaterialPropertyVector mSortedGlobalMaterialPropertyVector;


		// TODO(co) Implement decent resource handling
	public:
		std::vector<MaterialBlueprintResource*> mResources;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.inl"
