/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderType.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderProperties.h"

#include <Renderer/Public/Renderer.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'std::_Tree<std::_Tmap_traits<_Kty,_Ty,_Pr,_Alloc,false>>': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	#include <map>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
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
	typedef std::map<StringId, std::string>	DynamicShaderPieces;			// TODO(co) Unordered map might perform better
	typedef uint32_t						MaterialBlueprintResourceId;	///< POD material blueprint resource identifier
	typedef uint32_t						PipelineStateSignatureId;		///< Pipeline state signature identifier, result of hashing the referenced shaders as well as other pipeline state properties
	typedef uint32_t						ShaderCombinationId;			///< Shader combination identifier, result of hashing the shader combination generating shader blueprint resource, shader properties and dynamic shader pieces


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Pipeline state signature
	*
	*  @see
	*    - See "RendererRuntime::PipelineStateCacheManager" for additional information
	*/
	class PipelineStateSignature final
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		inline PipelineStateSignature();

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] materialBlueprintResource
		*    Material blueprint resource to use
		*  @param[in] serializedPipelineStateHash
		*    FNV1a hash of "Renderer::SerializedPipelineState"
		*  @param[in] shaderProperties
		*    Shader properties to use, you should ensure that this shader properties are already optimized by using e.g. "RendererRuntime::MaterialBlueprintResource::optimizeShaderProperties()"
		*  @param[in] dynamicShaderPieces
		*    Dynamic via C++ generated shader pieces to use
		*/
		inline PipelineStateSignature(const MaterialBlueprintResource& materialBlueprintResource, uint32_t serializedPipelineStateHash, const ShaderProperties& shaderProperties, const DynamicShaderPieces dynamicShaderPieces[NUMBER_OF_SHADER_TYPES]);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] pipelineStateSignature
		*    Pipeline state signature to copy from
		*/
		explicit PipelineStateSignature(const PipelineStateSignature& pipelineStateSignature);

		/**
		*  @brief
		*    Destructor
		*/
		inline ~PipelineStateSignature();

		/**
		*  @brief
		*    Copy operator
		*/
		PipelineStateSignature& operator=(const PipelineStateSignature& pipelineStateSignature);

		/**
		*  @brief
		*    Setter
		*
		*  @param[in] materialBlueprintResource
		*    Material blueprint resource to use
		*  @param[in] serializedPipelineStateHash
		*    FNV1a hash of "Renderer::SerializedPipelineState"
		*  @param[in] shaderProperties
		*    Shader properties to use, you should ensure that this shader properties are already optimized by using e.g. "RendererRuntime::MaterialBlueprintResource::optimizeShaderProperties()"
		*  @param[in] dynamicShaderPieces
		*    Dynamic via C++ generated shader pieces to use
		*/
		void set(const MaterialBlueprintResource& materialBlueprintResource, uint32_t serializedPipelineStateHash, const ShaderProperties& shaderProperties, const DynamicShaderPieces dynamicShaderPieces[NUMBER_OF_SHADER_TYPES]);

		//[-------------------------------------------------------]
		//[ Getter for input data                                 ]
		//[-------------------------------------------------------]
		inline MaterialBlueprintResourceId getMaterialBlueprintResourceId() const;
		inline uint32_t getSerializedPipelineStateHash() const;
		inline const ShaderProperties& getShaderProperties() const;
		inline const DynamicShaderPieces& getDynamicShaderPieces(ShaderType shaderType) const;

		//[-------------------------------------------------------]
		//[ Getter for derived data                               ]
		//[-------------------------------------------------------]
		inline PipelineStateSignatureId getPipelineStateSignatureId() const;
		inline ShaderCombinationId getShaderCombinationId(ShaderType shaderType) const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Input data
		MaterialBlueprintResourceId mMaterialBlueprintResourceId;
		uint32_t					mSerializedPipelineStateHash;
		ShaderProperties			mShaderProperties;
		DynamicShaderPieces			mDynamicShaderPieces[NUMBER_OF_SHADER_TYPES];
		// Derived data
		PipelineStateSignatureId mPipelineStateSignatureId;
		ShaderCombinationId		 mShaderCombinationId[NUMBER_OF_SHADER_TYPES];


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateSignature.inl"
