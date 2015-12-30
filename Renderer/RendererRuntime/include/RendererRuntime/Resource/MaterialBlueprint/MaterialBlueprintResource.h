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
	class Transform;
	class TextureResource;
	class MaterialResource;
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
		friend class MaterialResourceLoader;			// TODO(co) Decent material resource list management inside the material blueprint resource (link, unlink etc.) - remove this


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Uniform buffer usage
		*/
		enum class UniformBufferUsage : uint8_t
		{
			UNKNOWN = 0,	///< Unknown uniform buffer usage
			PASS,			///< Pass uniform buffer usage
			MATERIAL,		///< Material uniform buffer usage
			INSTANCE		///< Instance uniform buffer usage
		};

		typedef std::vector<MaterialProperty> UniformBufferElementProperties;
		typedef std::vector<uint8_t> ScratchBuffer;

		struct UniformBuffer
		{
			uint32_t					   uniformBufferRootParameterIndex;
			UniformBufferUsage			   uniformBufferUsage;
			uint32_t					   numberOfElements;
			UniformBufferElementProperties uniformBufferElementProperties;
			uint32_t					   uniformBufferNumberOfBytes;
			ScratchBuffer				   scratchBuffer;
			Renderer::IUniformBufferPtr	   uniformBufferPtr;
		};

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
		typedef std::vector<UniformBuffer> UniformBuffers;
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

		//[-------------------------------------------------------]
		//[ Resource                                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the uniform buffers
		*
		*  @return
		*    The uniform buffers
		*/
		inline const UniformBuffers& getUniformBuffers() const;

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
		//[ Misc                                                  ]
		//[-------------------------------------------------------]
		// TODO(co) Asynchronous loading completion
		bool isFullyLoaded() const;

		/**
		*  @brief
		*    Fill the pass uniform buffer
		*
		*  @param[in] worldSpaceToViewSpaceTransform
		*    World space to view space transform
		*/
		void fillPassUniformBuffer(const Transform& worldSpaceToViewSpaceTransform) const;

		/**
		*  @brief
		*    Fill the material uniform buffer
		*/
		void fillMaterialUniformBuffer() const;

		/**
		*  @brief
		*    Fill the instance uniform buffer
		*
		*  @param[in] objectSpaceToWorldSpaceTransform
		*    Object space to world space transform
		*  @param[in] materialResource
		*    Used material resource
		*/
		void fillInstanceUniformBuffer(const Transform& objectSpaceToWorldSpaceTransform, MaterialResource& materialResource) const;

		/**
		*  @brief
		*    Bind the material blueprint resource to the used renderer
		*/
		void bindToRenderer() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		MaterialBlueprintResource(const MaterialBlueprintResource&) = delete;
		MaterialBlueprintResource& operator=(const MaterialBlueprintResource&) = delete;
		void linkedMaterialResource(MaterialResource& materialResource);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<MaterialResource*> LinkedMaterialResources;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		SortedMaterialPropertyVector mSortedMaterialPropertyVector;
		Renderer::IRootSignature*	 mRootSignature;	///< Root signature, can be a null pointer
		Renderer::PipelineState		 mPipelineState;
		// Resource
		UniformBuffers				 mUniformBuffers;
		SamplerStates				 mSamplerStates;
		Textures					 mTextures;
		// Ease-of-use direct access
		UniformBuffer* mPassUniformBuffer;		///< Can be a null pointer, don't destroy the instance
		UniformBuffer* mMaterialUniformBuffer;	///< Can be a null pointer, don't destroy the instance
		UniformBuffer* mInstanceUniformBuffer;	///< Can be a null pointer, don't destroy the instance

		LinkedMaterialResources mLinkedMaterialResources;	// TODO(co) Decent material resource list management inside the material blueprint resource (link, unlink etc.)


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
