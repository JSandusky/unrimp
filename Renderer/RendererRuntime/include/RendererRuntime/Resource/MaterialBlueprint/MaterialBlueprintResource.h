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
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderProperties.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCacheManager.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class PassBufferManager;
	class MaterialBufferManager;
	template <class ELEMENT_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS> class PackedElementManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId																		 AssetId;						///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"
	typedef uint32_t																		 TextureResourceId;				///< POD texture resource identifier
	typedef uint32_t																		 ShaderBlueprintResourceId;		///< POD shader blueprint resource identifier
	typedef uint32_t																		 MaterialBlueprintResourceId;	///< POD material blueprint resource identifier
	typedef StringId																		 ShaderPropertyId;				///< Shader property identifier, internally just a POD "uint32_t", result of hashing the property name
	typedef PackedElementManager<MaterialBlueprintResource, MaterialBlueprintResourceId, 64> MaterialBlueprintResources;


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Material blueprint resource
	*
	*  @remarks
	*    == Regarding shader combination explosion ==
	*    The texture manager automatically generates some dynamic default texture assets like "Unrimp/Texture/Dynamic/IdentityDiffuseMap" or
	*    "Unrimp/Texture/Dynamic/IdentityNormalMap" one can reference e.g. inside material blueprint resources. Especially the identity texture maps
	*    can be used as default material property value. While it's tempting to add shader combination material properties like "UseDiffuseMap",
	*    "UseNormalMap" etc. one has to keep the problem of shader combination explosion in mind. Especially in more complex material blueprints the
	*    number of shader combinations can quickly reach a point were it's practically impossible to e.g. generate a shader cache for shipped products
	*    or in case of Mac OS X (no OpenGL binary shader support) generate the required shaders during program start. The problem can be fought with
	*    complex heuristics to filter out unused or rarely used shader combinations, this is an art form of itself. Please note that shader combination
	*    explosion is a real existing serious problem which especially inside the product shipping phase can create stressful situations. Additionally,
	*    if the shader complexity of different materials vary too extreme, the framerate might get instable. So, when designing material blueprints do
	*    carefully thing about which shader combinations you really need.
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
		friend MaterialBlueprintResources;			// Type definition of template class
		friend class MaterialBlueprintResourceLoader;
		friend class MaterialBlueprintResourceManager;
		friend class MaterialResourceLoader;		// TODO(co) Decent material resource list management inside the material blueprint resource (link, unlink etc.) - remove this
		friend class MaterialResourceManager;		// TODO(co) Remove
		friend class MaterialBufferManager;			// TODO(co) Remove. Decent material technique list management inside the material blueprint resource (link, unlink etc.)


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT static const int32_t MANDATORY_SHADER_PROPERTY;	///< Visual importance value for mandatory shader properties (such properties are not removed when finding a fallback pipeline state)

		/**
		*  @brief
		*    Uniform/texture buffer usage
		*/
		enum class BufferUsage : uint8_t
		{
			UNKNOWN = 0,	///< Unknown buffer usage, supports the following "RendererRuntime::MaterialProperty::Usage": "UNKNOWN_REFERENCE", "GLOBAL_REFERENCE" as well as properties with simple values
			PASS,			///< Pass buffer usage, supports the following "RendererRuntime::MaterialProperty::Usage": "PASS_REFERENCE", "GLOBAL_REFERENCE" as well as properties with simple values
			MATERIAL,		///< Material buffer usage, supports the following "RendererRuntime::MaterialProperty::Usage": "MATERIAL_REFERENCE", "GLOBAL_REFERENCE" as well as properties with simple values
			INSTANCE,		///< Instance buffer usage, supports the following "RendererRuntime::MaterialProperty::Usage": "INSTANCE_REFERENCE", "GLOBAL_REFERENCE" as well as properties with simple values
			LIGHT			///< Light buffer usage for texture buffer only
		};

		typedef std::vector<MaterialProperty> UniformBufferElementProperties;

		struct UniformBuffer
		{
			uint32_t					   rootParameterIndex;
			BufferUsage					   bufferUsage;
			uint32_t					   numberOfElements;
			UniformBufferElementProperties uniformBufferElementProperties;
			uint32_t					   uniformBufferNumberOfBytes;	///< Includes handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
		};

		struct TextureBuffer
		{
			uint32_t			  rootParameterIndex;
			BufferUsage			  bufferUsage;
			MaterialPropertyValue materialPropertyValue;

			TextureBuffer() :
				rootParameterIndex(getUninitialized<uint32_t>()),
				bufferUsage(BufferUsage::UNKNOWN),
				materialPropertyValue(MaterialPropertyValue::fromUnknown())
			{
				// Nothing here
			}

			TextureBuffer(uint32_t rootParameterIndex, BufferUsage bufferUsage, const MaterialPropertyValue& _materialPropertyValue) :
				rootParameterIndex(rootParameterIndex),
				bufferUsage(bufferUsage),
				materialPropertyValue(MaterialProperty(getUninitialized<MaterialPropertyId>(), getMaterialPropertyUsageFromBufferUsage(bufferUsage), _materialPropertyValue))
			{
				// Nothing here
			}

		};
		typedef std::vector<TextureBuffer> TextureBuffers;

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
			bool			  rgbHardwareGammaCorrection;

			// Derived data
			TextureResourceId textureResourceId;

			// Constructors
			Texture() :
				rootParameterIndex(getUninitialized<uint32_t>()),
				textureResourceId(getUninitialized<TextureResourceId>()),
				rgbHardwareGammaCorrection(false)
			{
				// Nothing here
			}
		};

		typedef std::vector<UniformBuffer> UniformBuffers;
		typedef std::vector<SamplerState>  SamplerStates;
		typedef std::vector<Texture>	   Textures;


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		static MaterialProperty::Usage getMaterialPropertyUsageFromBufferUsage(BufferUsage bufferUsage);


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
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
		inline int32_t getVisualImportanceOfShaderProperty(ShaderPropertyId shaderPropertyId) const;

		/**
		*  @brief
		*    Return the maximum integer value (inclusive) of a shader property
		*
		*  @return
		*    The maximum integer value (inclusive) of the requested shader property
		*/
		inline int32_t getMaximumIntegerValueOfShaderProperty(ShaderPropertyId shaderPropertyId) const;

		/**
		*  @brief
		*    Optimize the given shader properties
		*
		*  @param[in] shaderProperties
		*    Shader properties to optimize
		*
		*  @remarks
		*    Performed optimizations:
		*    - Removes all shader properties which have a zero value
		*    - Removes all shader properties which are unknown to the material blueprint
		*
		*  @note
		*    - This method should only be used at high level to reduce the shader properties to a bare minimum as soon as possible
		*/
		RENDERERRUNTIME_API_EXPORT void optimizeShaderProperties(ShaderProperties& shaderProperties) const;

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
		inline ShaderBlueprintResourceId getShaderBlueprintResourceId(ShaderType shaderType) const;

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
		*    Return the texture buffers
		*
		*  @return
		*    The texture buffers
		*/
		inline const TextureBuffers& getTextureBuffers() const;

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
		//[ Ease-of-use direct access                             ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the pass uniform buffer
		*
		*  @return
		*    The pass uniform buffer, can be a null pointer, don't destroy the instance
		*/
		inline const UniformBuffer* getPassUniformBuffer() const;

		/**
		*  @brief
		*    Return the material uniform buffer
		*
		*  @return
		*    The material uniform buffer, can be a null pointer, don't destroy the instance
		*/
		inline const UniformBuffer* getMaterialUniformBuffer() const;

		/**
		*  @brief
		*    Return the instance uniform buffer
		*
		*  @return
		*    The instance uniform buffer, can be a null pointer, don't destroy the instance
		*/
		inline const UniformBuffer* getInstanceUniformBuffer() const;

		/**
		*  @brief
		*    Return the instance texture buffer
		*
		*  @return
		*    The instance texture buffer, can be a null pointer, don't destroy the instance
		*/
		inline const TextureBuffer* getInstanceTextureBuffer() const;

		/**
		*  @brief
		*    Return the light texture buffer
		*
		*  @return
		*    The light texture buffer, can be a null pointer, don't destroy the instance
		*/
		inline const TextureBuffer* getLightTextureBuffer() const;

		//[-------------------------------------------------------]
		//[ Buffer manager                                        ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the pass buffer manager
		*
		*  @return
		*    The pass buffer manager, can be a null pointer, don't destroy the instance
		*/
		inline PassBufferManager* getPassBufferManager() const;

		/**
		*  @brief
		*    Return the material buffer manager
		*
		*  @return
		*    The material buffer manager, can be a null pointer, don't destroy the instance
		*/
		inline MaterialBufferManager* getMaterialBufferManager() const;

		//[-------------------------------------------------------]
		//[ Misc                                                  ]
		//[-------------------------------------------------------]
		// TODO(co) Asynchronous loading completion, we might want to move this into "RendererRuntime::IResource"
		RENDERERRUNTIME_API_EXPORT void enforceFullyLoaded();

		/**
		*  @brief
		*    Bind the material blueprint resource into the given commando buffer
		*
		*  @param[out] commandBuffer
		*    Command buffer to fill
		*/
		RENDERERRUNTIME_API_EXPORT void fillCommandBuffer(Renderer::CommandBuffer& commandBuffer) const;

		/**
		*  @brief
		*    Create pipeline state cache instances for this material blueprint
		*
		*  @param[in] mandatoryOnly
		*    Do only create mandatory combinations?
		*
		*  @remarks
		*    Use mandatory only to ensure that for every material blueprint there's a pipeline state cache for the most basic pipeline state signature
		*    -> With this in place, a fallback pipeline state signature can always be found for pipeline state cache misses
		*    -> Without this, we'll end up with runtime hiccups for pipeline state cache misses
		*    In order to reduce visual artifacts, a material blueprint can define a set of shader combination properties for which pipeline
		*    state caches must also be created. Inside the JSON material blueprint files, those properties are marked by
		*    "VisualImportance": "MANDATORY". Good examples for such shader properties are diffuse map or GPU skinning. It's the responsibility
		*    of the material blueprint author to keep the number of such shader properties to a bare minimum.
		*
		*    When setting mandatory only to "false", all possible combinations will be created. This might take a while, depending on the number of
		*    combinations. The creation of all possible combinations is usually done at tool-time before shipping a product. Runtime pipeline state
		*    compilation should only be the last resort for performance reasons, even if it's asynchronous.
		*
		*  @notes
		*    - The material blueprint resource must be fully loaded for this to work
		*/
		RENDERERRUNTIME_API_EXPORT void createPipelineStateCaches(bool mandatoryOnly);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		MaterialBlueprintResource();
		virtual ~MaterialBlueprintResource();
		MaterialBlueprintResource(const MaterialBlueprintResource&) = delete;
		MaterialBlueprintResource& operator=(const MaterialBlueprintResource&) = delete;

		//[-------------------------------------------------------]
		//[ "RendererRuntime::PackedElementManager" management    ]
		//[-------------------------------------------------------]
		void initializeElement(MaterialBlueprintResourceId materialBlueprintResourceId);
		void deinitializeElement();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		PipelineStateCacheManager			 mPipelineStateCacheManager;
		MaterialProperties					 mMaterialProperties;
		ShaderProperties					 mVisualImportanceOfShaderProperties;		///< Every shader property known to the material blueprint has a visual importance entry in here
		ShaderProperties					 mMaximumIntegerValueOfShaderProperties;	///< The maximum integer value (inclusive) of a shader property
		Renderer::VertexAttributes			 mVertexAttributes;
		Renderer::IRootSignaturePtr			 mRootSignaturePtr;							///< Root signature, can be a null pointer
		Renderer::PipelineState				 mPipelineState;
		ShaderBlueprintResourceId			 mShaderBlueprintResourceId[NUMBER_OF_SHADER_TYPES];
		// Resource
		UniformBuffers mUniformBuffers;
		TextureBuffers mTextureBuffers;
		SamplerStates  mSamplerStates;
		Textures	   mTextures;
		// Ease-of-use direct access
		UniformBuffer* mPassUniformBuffer;		///< Can be a null pointer, don't destroy the instance
		UniformBuffer* mMaterialUniformBuffer;	///< Can be a null pointer, don't destroy the instance
		UniformBuffer* mInstanceUniformBuffer;	///< Can be a null pointer, don't destroy the instance
		TextureBuffer* mInstanceTextureBuffer;	///< Can be a null pointer, don't destroy the instance
		TextureBuffer* mLightTextureBuffer;		///< Can be a null pointer, don't destroy the instance
		// Buffer manager
		PassBufferManager*	   mPassBufferManager;		///< Pass buffer manager, can be a null pointer, destroy the instance if you no longer need it
		MaterialBufferManager* mMaterialBufferManager;	///< Material buffer manager, can be a null pointer, destroy the instance if you no longer need it


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.inl"
