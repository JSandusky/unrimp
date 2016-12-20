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
#include "RendererRuntime/Core/Renderer/RenderTargetTextureManager.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <algorithm>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		inline bool orderRenderTargetTextureElementByRenderTargetTextureSignatureId(const RendererRuntime::RenderTargetTextureManager::RenderTargetTextureElement& left, const RendererRuntime::RenderTargetTextureManager::RenderTargetTextureElement& right)
		{
			return (left.renderTargetTextureSignature.getRenderTargetTextureSignatureId() < right.renderTargetTextureSignature.getRenderTargetTextureSignatureId());
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void RenderTargetTextureManager::clear()
	{
		clearRendererResources();
		mSortedRenderTargetTextureVector.clear();
		mAssetIdToRenderTargetTextureSignatureId.clear();
	}

	void RenderTargetTextureManager::clearRendererResources()
	{
		for (RenderTargetTextureElement& renderTargetTextureElement : mSortedRenderTargetTextureVector)
		{
			if (nullptr != renderTargetTextureElement.texture)
			{
				renderTargetTextureElement.texture->release();
				renderTargetTextureElement.texture = nullptr;
			}
		}
	}

	void RenderTargetTextureManager::addRenderTargetTexture(AssetId assetId, const RenderTargetTextureSignature& renderTargetTextureSignature)
	{
		RenderTargetTextureElement renderTargetTextureElement(renderTargetTextureSignature);
		SortedRenderTargetTextureVector::iterator iterator = std::lower_bound(mSortedRenderTargetTextureVector.begin(), mSortedRenderTargetTextureVector.end(), renderTargetTextureElement, ::detail::orderRenderTargetTextureElementByRenderTargetTextureSignatureId);
		if (iterator == mSortedRenderTargetTextureVector.end() || iterator->renderTargetTextureSignature.getRenderTargetTextureSignatureId() != renderTargetTextureElement.renderTargetTextureSignature.getRenderTargetTextureSignatureId())
		{
			// Add new render target texture

			// Register the new render target texture element
			++renderTargetTextureElement.numberOfReferences;
			mSortedRenderTargetTextureVector.insert(iterator, renderTargetTextureElement);
		}
		else
		{
			// Just increase the number of references
			++iterator->numberOfReferences;
		}
		mAssetIdToRenderTargetTextureSignatureId.emplace(assetId, renderTargetTextureSignature.getRenderTargetTextureSignatureId());
	}

	Renderer::ITexture* RenderTargetTextureManager::getTextureByAssetId(AssetId assetId, const Renderer::IRenderTarget& renderTarget, float resolutionScale)
	{
		Renderer::ITexture* texture = nullptr;

		// Map asset ID to render target texture signature ID
		AssetIdToRenderTargetTextureSignatureId::const_iterator iterator = mAssetIdToRenderTargetTextureSignatureId.find(assetId);
		if (mAssetIdToRenderTargetTextureSignatureId.cend() != iterator)
		{
			// TODO(co) Is there need for a more efficient search?
			const RenderTargetTextureSignatureId renderTargetTextureSignatureId = iterator->second;
			for (RenderTargetTextureElement& renderTargetTextureElement : mSortedRenderTargetTextureVector)
			{
				const RenderTargetTextureSignature& renderTargetTextureSignature = renderTargetTextureElement.renderTargetTextureSignature;
				if (renderTargetTextureSignature.getRenderTargetTextureSignatureId() == renderTargetTextureSignatureId)
				{
					// Do we need to create the renderer texture instance right now?
					if (nullptr == renderTargetTextureElement.texture)
					{
						// Get the texture width and height
						uint32_t width = renderTargetTextureSignature.getWidth();
						uint32_t height = renderTargetTextureSignature.getHeight();
						if (isUninitialized(width) || isUninitialized(height))
						{
							uint32_t renderTargetWidth = 0;
							uint32_t renderTargetHeight = 0;
							renderTarget.getWidthAndHeight(renderTargetWidth, renderTargetHeight);
							if (isUninitialized(width))
							{
								width = static_cast<uint32_t>(static_cast<float>(renderTargetWidth) * resolutionScale);
							}
							if (isUninitialized(height))
							{
								height = static_cast<uint32_t>(static_cast<float>(renderTargetHeight) * resolutionScale);
							}
						}

						// Create the texture instance, but without providing texture data (we use the texture as render target)
						// -> Use the "Renderer::TextureFlag::RENDER_TARGET"-flag to mark this texture as a render target
						// -> Required for Direct3D 9, Direct3D 10, Direct3D 11 and Direct3D 12
						// -> Not required for OpenGL and OpenGL ES 2
						// -> The optimized texture clear value is a Direct3D 12 related option
						renderTargetTextureElement.texture = mRendererRuntime.getTextureManager().createTexture2D(width, height, renderTargetTextureSignature.getTextureFormat(), nullptr, Renderer::TextureFlag::RENDER_TARGET, Renderer::TextureUsage::DEFAULT);
						renderTargetTextureElement.texture->addReference();

						{ // Tell the texture resource manager about our render target texture
							TextureResourceManager& textureResourceManager = mRendererRuntime.getTextureResourceManager();
							TextureResource* textureResource = textureResourceManager.getTextureResourceByAssetId(assetId);
							if (nullptr == textureResource)
							{
								// Create texture resource
								textureResourceManager.createTextureResourceByAssetId(assetId, *renderTargetTextureElement.texture);
							}
							else
							{
								// Update texture resource
								textureResource->setTexture(*renderTargetTextureElement.texture);
							}
						}
					}
					texture = renderTargetTextureElement.texture;
					break;
				}
			}
			assert(nullptr != texture);
		}
		else
		{
			// Error! Unknown asset ID, this shouldn't have happened.
			assert(false);
		}

		// Done
		return texture;
	}

	void RenderTargetTextureManager::releaseRenderTargetTextureBySignature(const RenderTargetTextureSignature& renderTargetTextureSignature)
	{
		const RenderTargetTextureElement renderTargetTextureElement(renderTargetTextureSignature);
		SortedRenderTargetTextureVector::iterator iterator = std::lower_bound(mSortedRenderTargetTextureVector.begin(), mSortedRenderTargetTextureVector.end(), renderTargetTextureElement, ::detail::orderRenderTargetTextureElementByRenderTargetTextureSignatureId);
		if (iterator != mSortedRenderTargetTextureVector.end() && iterator->renderTargetTextureSignature.getRenderTargetTextureSignatureId() == renderTargetTextureElement.renderTargetTextureSignature.getRenderTargetTextureSignatureId())
		{
			// Was this the last reference?
			if (1 == iterator->numberOfReferences)
			{
				if (nullptr != iterator->texture)
				{
					iterator->texture->release();
				}
				mSortedRenderTargetTextureVector.erase(iterator);
			}
			else
			{
				--iterator->numberOfReferences;
			}
		}
		else
		{
			// Error! Render target texture signature isn't registered.
			assert(false);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
