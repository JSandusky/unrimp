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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/MaterialBufferManager.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialTechnique::MaterialTechnique(MaterialTechniqueId materialTechniqueId, MaterialResource& materialResource, MaterialBlueprintResourceId materialBlueprintResourceId) :
		MaterialBufferSlot(materialResource),
		mMaterialTechniqueId(materialTechniqueId),
		mMaterialBlueprintResourceId(materialBlueprintResourceId)
	{
		MaterialBufferManager* materialBufferManager = getMaterialBufferManager();
		if (nullptr != materialBufferManager)
		{
			materialBufferManager->requestSlot(*this);
		}
	}

	MaterialTechnique::~MaterialTechnique()
	{
		MaterialBufferManager* materialBufferManager = getMaterialBufferManager();
		if (nullptr != materialBufferManager)
		{
			materialBufferManager->releaseSlot(*this);
		}
	}

	const MaterialTechnique::Textures& MaterialTechnique::getTextures(const IRendererRuntime& rendererRuntime) const
	{
		// Need for gathering the textures now?
		if (mTextures.empty())
		{
			const MaterialBlueprintResource* materialBlueprintResource = rendererRuntime.getMaterialBlueprintResourceManager().getMaterialBlueprintResources().tryGetElementById(mMaterialBlueprintResourceId);
			if (nullptr != materialBlueprintResource)
			{
				const MaterialResource& materialResource = getMaterialResource();
				TextureResourceManager& textureResourceManager = rendererRuntime.getTextureResourceManager();
				const MaterialBlueprintResource::Textures& textures = materialBlueprintResource->getTextures();
				const size_t numberOfTextures = textures.size();
				for (size_t i = 0; i < numberOfTextures; ++i)
				{
					const MaterialBlueprintResource::Texture& blueprintTexture = textures[i];

					// Start with the material blueprint textures
					Texture texture;
					texture.rootParameterIndex = blueprintTexture.rootParameterIndex;
					texture.materialProperty   = blueprintTexture.materialProperty;
					texture.textureResourceId  = blueprintTexture.textureResourceId;

					// Apply material specific modifications
					const MaterialPropertyId materialPropertyId = texture.materialProperty.getMaterialPropertyId();
					if (isInitialized(materialPropertyId))
					{
						// Figure out the material property value
						const MaterialProperty* materialProperty = materialResource.getPropertyById(materialPropertyId);
						if (nullptr != materialProperty)
						{
							// TODO(co) Error handling: Usage mismatch etc.
							texture.materialProperty = *materialProperty;
							texture.textureResourceId = textureResourceManager.loadTextureResourceByAssetId(texture.materialProperty.getTextureAssetIdValue(), nullptr, true);	// TODO(co) Ask the material blueprint whether or not hardware gamma correction should be used
						}
					}

					// Insert texture
					mTextures.push_back(texture);
				}
			}
		}
		return mTextures;
	}

	void MaterialTechnique::fillCommandBuffer(const IRendererRuntime& rendererRuntime, Renderer::CommandBuffer& commandBuffer)
	{
		assert(isInitialized(mMaterialBlueprintResourceId));

		// TODO(co) This is experimental and will certainly look different when everything is in place

		{ // Bind the material buffer manager
			MaterialBufferManager* materialBufferManager = getMaterialBufferManager();
			if (nullptr != materialBufferManager)
			{
				materialBufferManager->fillCommandBuffer(*this, commandBuffer);
			}
		}

		{ // Graphics root descriptor table: Set textures
			const Textures& textures = getTextures(rendererRuntime);
			const size_t numberOfTextures = textures.size();
			const TextureResources& textureResources = rendererRuntime.getTextureResourceManager().getTextureResources();
			for (size_t i = 0; i < numberOfTextures; ++i)
			{
				const Texture& texture = textures[i];

				// Due to background texture loading, some textures might not be ready, yet
				// TODO(co) Add dummy textures so rendering also works when textures are not ready, yet
				const TextureResource* textureResource  = textureResources.tryGetElementById(texture.textureResourceId);
				if (nullptr != textureResource)
				{
					Renderer::ITexturePtr texturePtr = textureResource->getTexture();
					if (nullptr != texturePtr)
					{
						Renderer::Command::SetGraphicsRootDescriptorTable::create(commandBuffer, texture.rootParameterIndex, texturePtr);
					}
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MaterialBufferManager* MaterialTechnique::getMaterialBufferManager() const
	{
		// It's valid if a material blueprint resource doesn't contain a material uniform buffer (usually the case for compositor material blueprint resources)
		const MaterialBlueprintResource* materialBlueprintResource = getMaterialResourceManager().getRendererRuntime().getMaterialBlueprintResourceManager().getMaterialBlueprintResources().tryGetElementById(mMaterialBlueprintResourceId);
		return (nullptr != materialBlueprintResource) ? materialBlueprintResource->getMaterialBufferManager() : nullptr;
	}

	void MaterialTechnique::scheduleForShaderUniformUpdate()
	{
		MaterialBufferManager* materialBufferManager = getMaterialBufferManager();
		if (nullptr != materialBufferManager)
		{
			materialBufferManager->scheduleForUpdate(*this);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
