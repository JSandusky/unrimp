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
	class IRootSignature;
}
namespace RendererRuntime
{
	class TextureResource;
	class ShaderBlueprintResource;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId AssetId;	///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>" (Example: "Example/Font/Default/LinBiolinum_R" will result in asset ID 64363173)


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Material blueprint resource
	*/
	class MaterialBlueprintResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MaterialBlueprintResourceLoader;
		friend class MaterialBlueprintResourceManager;	// TODO(co) Remove this


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		struct SamplerState
		{
			uint32_t				   samplerRootParameterIndex;
			Renderer::ISamplerStatePtr samplerStatePtr;
		};
		struct Texture
		{
			uint32_t		   textureRootParameterIndex;
			AssetId			   textureAssetId;
			MaterialPropertyId materialPropertyId;
			TextureResource*   textureResource;	// TODO(co) Implement decent resource management
		};

		typedef std::vector<MaterialProperty> SortedMaterialPropertyVector;
		typedef std::vector<SamplerState> SamplerStates;
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
		explicit MaterialBlueprintResource(ResourceId resourceId);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~MaterialBlueprintResource();

		// TODO(co) Asynchronous loading completion
		bool isFullyLoaded() const;

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
		*    Return the root signature
		*
		*  @return
		*    The root signature, can be a null pointer, do not destroy the instance
		*/
		inline Renderer::IRootSignature* getRootSignature() const;

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
		*    Return the sampler states
		*
		*  @return
		*    The sampler states
		*/
		inline const SamplerStates& getSamplerStates() const;

		/**
		*  @brief
		*    Return the textures
		*
		*  @return
		*    The textures
		*/
		inline const Textures& getTextures() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		MaterialBlueprintResource(const MaterialBlueprintResource&) = delete;
		MaterialBlueprintResource& operator=(const MaterialBlueprintResource&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		SortedMaterialPropertyVector mSortedMaterialPropertyVector;
		Renderer::IRootSignature*	 mRootSignature;	///< Root signature, can be a null pointer
		Renderer::PipelineState		 mPipelineState;
		SamplerStates				 mSamplerStates;
		Textures					 mTextures;

	// TODO(co) Make this private
	public:
		ShaderBlueprintResource*  mVertexShaderBlueprint;
		ShaderBlueprintResource*  mTessellationControlShaderBlueprint;
		ShaderBlueprintResource*  mTessellationEvaluationShaderBlueprint;
		ShaderBlueprintResource*  mGeometryShaderBlueprint;
		ShaderBlueprintResource*  mFragmentShaderBlueprint;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.inl"
