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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline PipelineStateCacheManager& MaterialBlueprintResource::getPipelineStateCacheManager()
	{
		return mPipelineStateCacheManager;
	}

	inline const MaterialProperties& MaterialBlueprintResource::getMaterialProperties() const
	{
		return mMaterialProperties;
	}

	inline int32_t MaterialBlueprintResource::getVisualImportanceOfShaderProperty(ShaderPropertyId shaderPropertyId) const
	{
		return mVisualImportanceOfShaderProperties.getPropertyValueUnsafe(shaderPropertyId);
	}

	inline int32_t MaterialBlueprintResource::getMaximumIntegerValueOfShaderProperty(ShaderPropertyId shaderPropertyId) const
	{
		return mMaximumIntegerValueOfShaderProperties.getPropertyValueUnsafe(shaderPropertyId);
	}

	inline const Renderer::VertexAttributes& MaterialBlueprintResource::getVertexAttributes() const
	{
		return mVertexAttributes;
	}

	inline Renderer::IRootSignaturePtr MaterialBlueprintResource::getRootSignaturePtr() const
	{
		return mRootSignaturePtr;
	}

	inline const Renderer::PipelineState& MaterialBlueprintResource::getPipelineState() const
	{
		return mPipelineState;
	}

	inline const ShaderBlueprintResourceId MaterialBlueprintResource::getShaderBlueprintResourceId(ShaderType shaderType) const
	{
		return mShaderBlueprintResourceId[static_cast<uint8_t>(shaderType)];
	}

	inline const MaterialBlueprintResource::UniformBuffers& MaterialBlueprintResource::getUniformBuffers() const
	{
		return mUniformBuffers;
	}

	inline const MaterialBlueprintResource::TextureBuffers& MaterialBlueprintResource::getTextureBuffers() const
	{
		return mTextureBuffers;
	}

	inline const MaterialBlueprintResource::SamplerStates& MaterialBlueprintResource::getSamplerStates() const
	{
		return mSamplerStates;
	}

	inline const MaterialBlueprintResource::Textures& MaterialBlueprintResource::getTextures() const
	{
		return mTextures;
	}

	inline const MaterialBlueprintResource::UniformBuffer* MaterialBlueprintResource::getPassUniformBuffer() const
	{
		return mPassUniformBuffer;
	}

	inline const MaterialBlueprintResource::UniformBuffer* MaterialBlueprintResource::getMaterialUniformBuffer() const
	{
		return mMaterialUniformBuffer;
	}

	inline const MaterialBlueprintResource::UniformBuffer* MaterialBlueprintResource::getInstanceUniformBuffer() const
	{
		return mInstanceUniformBuffer;
	}

	inline const MaterialBlueprintResource::TextureBuffer* MaterialBlueprintResource::getInstanceTextureBuffer() const
	{
		return mInstanceTextureBuffer;
	}

	inline PassBufferManager* MaterialBlueprintResource::getPassBufferManager() const
	{
		return mPassBufferManager;
	}

	inline MaterialBufferManager* MaterialBlueprintResource::getMaterialBufferManager() const
	{
		return mMaterialBufferManager;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
