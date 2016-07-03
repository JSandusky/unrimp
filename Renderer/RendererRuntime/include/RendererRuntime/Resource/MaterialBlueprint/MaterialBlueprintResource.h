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
#include "RendererRuntime/Resource/Material/MaterialProperties.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderType.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCacheManager.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class Transform;
	class MaterialTechnique;
	class MaterialBlueprintResourceManager;
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
	typedef StringId AssetId;						///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"
	typedef uint32_t TextureResourceId;				///< POD texture resource identifier
	typedef uint32_t ShaderBlueprintResourceId;		///< POD shader blueprint resource identifier
	typedef uint32_t MaterialBlueprintResourceId;	///< POD material blueprint resource identifier
	typedef StringId ShaderPropertyId;				///< Shader property identifier, internally just a POD "uint32_t", result of hashing the property name


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Material blueprint resource
	*
	*  @note
	*    - Automatic handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
	*    - When writing new material blueprint resources, you might want to take the packing rules for uniform variables into account for an efficient data layout
	*/
	class MaterialBlueprintResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MaterialBlueprintResourceLoader;
		friend class MaterialBlueprintResourceManager;
		friend class MaterialResourceLoader;	// TODO(co) Decent material resource list management inside the material blueprint resource (link, unlink etc.) - remove this
		friend class MaterialResourceManager;	// TODO(co) Remove
		friend class PackedElementManager<MaterialBlueprintResource, MaterialBlueprintResourceId>;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const int MANDATORY_SHADER_PROPERTY;	///< Visual importance value for mandatory shader properties (such properties are not removed when finding a fallback pipeline state)

		/**
		*  @brief
		*    Uniform buffer usage
		*/
		enum class UniformBufferUsage : uint8_t
		{
			UNKNOWN = 0,	///< Unknown uniform buffer usage, supports the following "RendererRuntime::MaterialProperty::Usage": "UNKNOWN_REFERENCE", "GLOBAL_REFERENCE" as well as properties with simple values
			PASS,			///< Pass uniform buffer usage, supports the following "RendererRuntime::MaterialProperty::Usage": "PASS_REFERENCE", "GLOBAL_REFERENCE" as well as properties with simple values
			MATERIAL,		///< Material uniform buffer usage, supports the following "RendererRuntime::MaterialProperty::Usage": "MATERIAL_REFERENCE", "GLOBAL_REFERENCE" as well as properties with simple values
			INSTANCE		///< Instance uniform buffer usage, supports the following "RendererRuntime::MaterialProperty::Usage": "INSTANCE_REFERENCE", "GLOBAL_REFERENCE" as well as properties with simple values
		};

		typedef std::vector<MaterialProperty> UniformBufferElementProperties;
		typedef std::vector<uint8_t> ScratchBuffer;

		struct UniformBuffer
		{
			uint32_t					   rootParameterIndex;
			UniformBufferUsage			   uniformBufferUsage;
			uint32_t					   numberOfElements;
			UniformBufferElementProperties uniformBufferElementProperties;
			uint32_t					   uniformBufferNumberOfBytes;	///< Includes handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
			ScratchBuffer				   scratchBuffer;
			Renderer::IUniformBufferPtr	   uniformBufferPtr;
		};

		struct SamplerState
		{
			uint32_t				   rootParameterIndex;
			Renderer::ISamplerStatePtr samplerStatePtr;
		};

		struct Texture
		{
			// Loaded from material blueprint
			uint32_t		  rootParameterIndex;
			MaterialProperty  materialProperty;

			// Derived data
			TextureResourceId textureResourceId;

			// Constructors
			Texture() :
				rootParameterIndex(getUninitialized<uint32_t>()),
				textureResourceId(getUninitialized<TextureResourceId>())
			{
				// Nothing here
			}
		};

		typedef std::vector<UniformBuffer> UniformBuffers;
		typedef std::vector<SamplerState>  SamplerStates;
		typedef std::vector<Texture>	   Textures;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the owner material blueprint resource manager
		*
		*  @return
		*    The owner material blueprint resource manager
		*/
		inline MaterialBlueprintResourceManager& getMaterialBlueprintResourceManager() const;

		/**
		*  @brief
		*    Return the pipeline state cache manager
		*
		*  @return
		*    The pipeline state cache manager
		*/
		inline PipelineStateCacheManager& getPipelineStateCacheManager();

		/**
		*  @brief
		*    Return the material blueprint properties
		*
		*  @return
		*    The material blueprint properties
		*/
		inline const MaterialProperties& getMaterialProperties() const;

		/**
		*  @brief
		*    Return the visual importance of a requested shader property
		*
		*  @return
		*    The visual importance of the requested shader property, lower visual importance value = lower probability that someone will miss the shader property,
		*    can be "RendererRuntime::MaterialBlueprintResource::MANDATORY_SHADER_PROPERTY" for mandatory shader properties (such properties are not removed when finding a fallback pipeline state)
		*/
		int getVisualImportanceOfShaderProperty(ShaderPropertyId shaderPropertyId) const;

		/**
		*  @brief
		*    Return the vertex attributes
		*
		*  @return
		*    The vertex attributes
		*/
		inline const Renderer::VertexAttributes& getVertexAttributes() const;

		/**
		*  @brief
		*    Return the root signature
		*
		*  @return
		*    The root signature, can be a null pointer, do not destroy the instance
		*/
		inline Renderer::IRootSignaturePtr getRootSignaturePtr() const;

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
		*    Return a shader blueprint resource ID
		*
		*  @return
		*    The requested shader blueprint resource ID, can be uninitialized
		*/
		inline const ShaderBlueprintResourceId getShaderBlueprintResourceId(ShaderType shaderType) const;

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
		*    Fill the unknown uniform buffers
		*
		*  @notes
		*    - Stick to pass, material and instance uniform buffers and avoid unknown uniform buffers whenever possible
		*/
		void fillUnknownUniformBuffers();

		/**
		*  @brief
		*    Fill the pass uniform buffer
		*
		*  @param[in] worldSpaceToViewSpaceTransform
		*    World space to view space transform
		*/
		void fillPassUniformBuffer(const Transform& worldSpaceToViewSpaceTransform);

		/**
		*  @brief
		*    Fill the material uniform buffer
		*/
		void fillMaterialUniformBuffer();

		/**
		*  @brief
		*    Fill the instance uniform buffer
		*
		*  @param[in] objectSpaceToWorldSpaceTransform
		*    Object space to world space transform
		*  @param[in] materialTechnique
		*    Used material technique
		*/
		void fillInstanceUniformBuffer(const Transform& objectSpaceToWorldSpaceTransform, MaterialTechnique& materialTechnique);

		/**
		*  @brief
		*    Bind the material blueprint resource to the used renderer
		*/
		void bindToRenderer() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Default constructor
		*/
		MaterialBlueprintResource();

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] materialBlueprintResourceId
		*    Material blueprint resource ID
		*/
		explicit MaterialBlueprintResource(MaterialBlueprintResourceId materialBlueprintResourceId);

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~MaterialBlueprintResource();

		MaterialBlueprintResource(const MaterialBlueprintResource&) = delete;
		MaterialBlueprintResource& operator=(const MaterialBlueprintResource&) = delete;
		void linkMaterialTechnique(MaterialTechnique& materialTechnique);
		void unlinkMaterialTechnique(MaterialTechnique& materialTechnique);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::unordered_map<uint32_t, int> VisualImportanceOfShaderProperties;	// TODO(co) Key should be "ShaderPropertyId"
		typedef std::vector<MaterialTechnique*>   LinkedMaterialTechniques;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		MaterialBlueprintResourceManager*			mMaterialBlueprintResourceManager;	///< Owner material blueprint resource manager, pointer considered to be always valid
		PipelineStateCacheManager					mPipelineStateCacheManager;
		MaterialProperties							mMaterialProperties;
		VisualImportanceOfShaderProperties			mVisualImportanceOfShaderProperties;
		Renderer::VertexAttributes					mVertexAttributes;
		Renderer::IRootSignaturePtr					mRootSignaturePtr;					///< Root signature, can be a null pointer
		Renderer::PipelineState						mPipelineState;
		ShaderBlueprintResourceId					mShaderBlueprintResourceId[NUMBER_OF_SHADER_TYPES];
		// Resource
		UniformBuffers mUniformBuffers;
		SamplerStates  mSamplerStates;
		Textures	   mTextures;
		// Ease-of-use direct access
		UniformBuffer* mPassUniformBuffer;		///< Can be a null pointer, don't destroy the instance
		UniformBuffer* mMaterialUniformBuffer;	///< Can be a null pointer, don't destroy the instance
		UniformBuffer* mInstanceUniformBuffer;	///< Can be a null pointer, don't destroy the instance

		LinkedMaterialTechniques mLinkedMaterialTechniques;	// TODO(co) Decent material technique list management inside the material blueprint resource (link, unlink etc.)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.inl"
