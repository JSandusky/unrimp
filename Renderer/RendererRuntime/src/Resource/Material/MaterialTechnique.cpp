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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <algorithm>

#include <assert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void MaterialTechnique::bindToRenderer(const IRendererRuntime& rendererRuntime)
	{
		assert(nullptr != mMaterialBlueprintResource);

		// TODO(co) This is experimental and will certainly look different when everything is in place
		Renderer::IRenderer& renderer = rendererRuntime.getRenderer();

		// Need for gathering the textures now?
		if (mTextures.empty())
		{
			TextureResourceManager& textureResourceManager = rendererRuntime.getTextureResourceManager();
			const MaterialProperties& materialProperties = mMaterialResource->getMaterialProperties();
			const MaterialBlueprintResource::Textures& textures = mMaterialBlueprintResource->getTextures();
			const size_t numberOfTextures = textures.size();
			for (size_t i = 0; i < numberOfTextures; ++i)
			{
				const MaterialBlueprintResource::Texture& blueprintTexture = textures[i];

				// Start with the material blueprint textures
				Texture texture;
				texture.rootParameterIndex = blueprintTexture.rootParameterIndex;
				texture.textureAssetId = blueprintTexture.textureAssetId;
				texture.materialPropertyId = blueprintTexture.materialPropertyId;
				texture.textureResource = blueprintTexture.textureResource;	// TODO(co) Implement decent resource management

				// Apply material specific modifications
				if (0 != texture.materialPropertyId)
				{
					// Figure out the material property value
					const MaterialProperty* materialProperty = materialProperties.getPropertyById(texture.materialPropertyId);
					if (nullptr != materialProperty)
					{
						// TODO(co) Error handling: Usage mismatch etc.
						texture.textureAssetId = materialProperty->getAssetIdValue();
						texture.textureResource = textureResourceManager.loadTextureResourceByAssetId(texture.textureAssetId);	// TODO(co) Implement decent resource management
					}
				}

				// Insert texture
				mTextures.push_back(texture);
			}
		}

		{ // Graphics root descriptor table: Set textures
			const size_t numberOfTextures = mTextures.size();
			for (size_t i = 0; i < numberOfTextures; ++i)
			{
				const Texture& texture = mTextures[i];

				// Due to background texture loading, some textures might not be ready, yet
				// TODO(co) Add dummy textures so rendering also works when textures are not ready, yet
				if (nullptr != texture.textureResource)
				{
					Renderer::ITexturePtr texturePtr = texture.textureResource->getTexture();
					if (nullptr != texturePtr)
					{
						renderer.setGraphicsRootDescriptorTable(texture.rootParameterIndex, texturePtr);
					}
				}
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime