/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/GetUninitialized.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline PipelineStateSignature::PipelineStateSignature() :
		mMaterialBlueprintResourceId(getUninitialized<MaterialBlueprintResourceId>()),
		mPrimitiveTopology(getUninitialized<Renderer::PrimitiveTopology>()),
		mSerializedPipelineStateHash(getUninitialized<uint32_t>()),
		mPipelineStateSignatureId(getUninitialized<PipelineStateSignatureId>()),
		mShaderCombinationId{getUninitialized<ShaderCombinationId>(), getUninitialized<ShaderCombinationId>(), getUninitialized<ShaderCombinationId>(), getUninitialized<ShaderCombinationId>(), getUninitialized<ShaderCombinationId>()}
	{
		// Nothing here
	}

	inline PipelineStateSignature::PipelineStateSignature(const MaterialBlueprintResource& materialBlueprintResource, Renderer::PrimitiveTopology primitiveTopology, uint32_t serializedPipelineStateHash, const ShaderProperties& shaderProperties, const DynamicShaderPieces dynamicShaderPieces[NUMBER_OF_SHADER_TYPES])
	{
		set(materialBlueprintResource, primitiveTopology, serializedPipelineStateHash, shaderProperties, dynamicShaderPieces);
	}

	inline PipelineStateSignature::~PipelineStateSignature()
	{
		// Nothing here
	}

	inline MaterialBlueprintResourceId PipelineStateSignature::getMaterialBlueprintResourceId() const
	{
		return mMaterialBlueprintResourceId;
	}

	inline Renderer::PrimitiveTopology PipelineStateSignature::getPrimitiveTopology() const
	{
		return mPrimitiveTopology;
	}

	inline uint32_t PipelineStateSignature::getSerializedPipelineStateHash() const
	{
		return mSerializedPipelineStateHash;
	}

	inline const ShaderProperties& PipelineStateSignature::getShaderProperties() const
	{
		return mShaderProperties;
	}

	inline const DynamicShaderPieces& PipelineStateSignature::getDynamicShaderPieces(ShaderType shaderType) const
	{
		return mDynamicShaderPieces[static_cast<uint8_t>(shaderType)];
	}

	inline PipelineStateSignatureId PipelineStateSignature::getPipelineStateSignatureId() const
	{
		return mPipelineStateSignatureId;
	}

	inline ShaderCombinationId PipelineStateSignature::getShaderCombinationId(ShaderType shaderType) const
	{
		return mShaderCombinationId[static_cast<uint8_t>(shaderType)];
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
