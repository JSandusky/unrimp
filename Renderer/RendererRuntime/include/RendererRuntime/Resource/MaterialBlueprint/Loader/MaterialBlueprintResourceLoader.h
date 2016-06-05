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
#include "RendererRuntime/Resource/Detail/IResourceLoader.h"
#include "RendererRuntime/Asset/Asset.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
	class MaterialBlueprintResource;
	namespace v1MaterialBlueprint
	{
		struct Texture;
		struct SamplerState;
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class MaterialBlueprintResourceLoader : protected IResourceLoader
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MaterialBlueprintResourceManager;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const ResourceLoaderTypeId TYPE_ID;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	public:
		virtual ResourceLoaderTypeId getResourceLoaderTypeId() const override;
		virtual void onDeserialization() override;
		virtual void onProcessing() override;
		virtual void onRendererBackendDispatch() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline MaterialBlueprintResourceLoader(IResourceManager& resourceManager, IRendererRuntime& rendererRuntime);
		virtual ~MaterialBlueprintResourceLoader();
		MaterialBlueprintResourceLoader(const MaterialBlueprintResourceLoader&) = delete;
		MaterialBlueprintResourceLoader& operator=(const MaterialBlueprintResourceLoader&) = delete;
		inline void initialize(const Asset& asset, MaterialBlueprintResource& materialBlueprintResource);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime& mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance

		// Resource source and destination
		Asset					   mAsset;		///< In order to be multi-threading safe in here, we need an asset copy
		MaterialBlueprintResource* mMaterialBlueprintResource;

		// Temporary data: Root signature
		uint32_t				   mMaximumNumberOfRootParameters;
		Renderer::RootParameter*   mRootParameters;
		uint32_t				   mMaximumNumberOfDescriptorRanges;
		Renderer::DescriptorRange* mDescriptorRanges;
		Renderer::RootSignature	   mRootSignature;

		// Temporary data: Shader blueprints
		AssetId mVertexShaderBlueprintAssetId;
		AssetId mTessellationControlShaderBlueprintAssetId;
		AssetId mTessellationEvaluationShaderBlueprintAssetId;
		AssetId mGeometryShaderBlueprintAssetId;
		AssetId mFragmentShaderBlueprintAssetId;

		// Temporary data: Sampler states
		uint32_t						   mMaximumNumberOfMaterialBlueprintSamplerStates;
		v1MaterialBlueprint::SamplerState* mMaterialBlueprintSamplerStates;

		// Temporary data: Textures
		uint32_t					  mMaximumNumberOfMaterialBlueprintTextures;
		v1MaterialBlueprint::Texture* mMaterialBlueprintTextures;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintResourceLoader.inl"
