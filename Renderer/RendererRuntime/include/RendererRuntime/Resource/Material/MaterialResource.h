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
#include "RendererRuntime/Resource/IResource.h"
#include "RendererRuntime/Resource/Material/MaterialProperty.h"

#include <Renderer/Public/Renderer.h>

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderer;
	class IPipelineState;
}
namespace RendererRuntime
{
	class TextureResource;
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
		friend class MaterialBlueprintResource;	///< Sets "RendererRuntime::MaterialResource::mMaterialUniformBufferIndex"


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		struct Texture
		{
			uint32_t		   rootParameterIndex;
			AssetId			   textureAssetId;
			MaterialPropertyId materialPropertyId;
			TextureResource*   textureResource;	// TODO(co) Implement decent resource management
		};

		typedef std::vector<MaterialProperty> SortedMaterialPropertyVector;
		typedef std::vector<Texture> Textures;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] resourceId
		*    Resource ID
		*/
		explicit MaterialResource(ResourceId resourceId);

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~MaterialResource();

		/**
		*  @brief
		*    Return the used material blueprint resource
		*
		*  @return
		*    The used material blueprint resource, can be a null pointer, don't destroy the instance
		*/
		inline MaterialBlueprintResource* getMaterialBlueprintResource() const;

		/**
		*  @brief
		*    Return the material uniform buffer index inside the used material blueprint
		*
		*  @return
		*    The material uniform buffer index inside the used material blueprint
		*/
		inline uint32_t getMaterialUniformBufferIndex() const;

		/**
		*  @brief
		*    Return the sorted material property vector
		*
		*  @return
		*    The sorted material property vector
		*/
		inline const SortedMaterialPropertyVector& getSortedMaterialPropertyVector() const;

		/**
		*  @brief
		*    Return a material property by its ID
		*
		*  @param[in] materialPropertyId
		*    ID of the material property to return
		*
		*  @return
		*    The requested material property, null pointer on error, don't destroy the returned instance
		*/
		const MaterialProperty* getMaterialPropertyById(MaterialPropertyId materialPropertyId) const;

		/**
		*  @brief
		*    Return the pipeline state
		*
		*  @return
		*    The pipeline state
		*/
		inline const Renderer::PipelineState& getPipelineState() const;

		/**
		*  @brief
		*    Return the pipeline state object (PSO)
		*
		*  @return
		*    The pipeline state object (PSO), can be a null pointer, don't destroy the instance
		*/
		Renderer::IPipelineState* getPipelineStateObject();

		// TODO(co)
		void releasePipelineState();
		void bindToRenderer(const IRendererRuntime& rendererRuntime);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		MaterialResource(const MaterialResource&) = delete;
		MaterialResource& operator=(const MaterialResource&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		MaterialBlueprintResource*	 mMaterialBlueprintResource;	///< Material blueprint resource, can be a null pointer, don't destroy the instance
		uint32_t					 mMaterialUniformBufferIndex;	///< Material uniform buffer index inside the used material blueprint
		SortedMaterialPropertyVector mSortedMaterialPropertyVector;
		Renderer::PipelineState		 mPipelineState;
		Renderer::IPipelineState*    mPipelineStateObject;			///< Pipeline state object (PSO), can be a null pointer
		Textures					 mTextures;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Material/MaterialResource.inl"
