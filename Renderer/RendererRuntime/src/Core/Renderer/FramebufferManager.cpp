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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Core/Renderer/FramebufferManager.h"
#include "RendererRuntime/Core/Renderer/RenderTargetTextureManager.h"
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
		inline bool orderFramebufferElementByFramebufferSignatureId(const RendererRuntime::FramebufferManager::FramebufferElement& left, const RendererRuntime::FramebufferManager::FramebufferElement& right)
		{
			return (left.framebufferSignature.getFramebufferSignatureId() < right.framebufferSignature.getFramebufferSignatureId());
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
	void FramebufferManager::clear()
	{
		clearRendererResources();
		mSortedFramebufferVector.clear();
		mCompositorFramebufferIdToFramebufferSignatureId.clear();
	}

	void FramebufferManager::clearRendererResources()
	{
		for (FramebufferElement& framebufferElement : mSortedFramebufferVector)
		{
			if (nullptr != framebufferElement.framebuffer)
			{
				framebufferElement.framebuffer->releaseReference();
				framebufferElement.framebuffer = nullptr;
			}
		}
	}

	void FramebufferManager::addFramebuffer(CompositorFramebufferId compositorFramebufferId, const FramebufferSignature& framebufferSignature)
	{
		FramebufferElement framebufferElement(framebufferSignature);
		SortedFramebufferVector::iterator iterator = std::lower_bound(mSortedFramebufferVector.begin(), mSortedFramebufferVector.end(), framebufferElement, ::detail::orderFramebufferElementByFramebufferSignatureId);
		if (iterator == mSortedFramebufferVector.end() || iterator->framebufferSignature.getFramebufferSignatureId() != framebufferElement.framebufferSignature.getFramebufferSignatureId())
		{
			// Add new framebuffer

			// Register the new framebuffer element
			++framebufferElement.numberOfReferences;
			mSortedFramebufferVector.insert(iterator, framebufferElement);
		}
		else
		{
			// Just increase the number of references
			++iterator->numberOfReferences;
		}
		mCompositorFramebufferIdToFramebufferSignatureId.emplace(compositorFramebufferId, framebufferSignature.getFramebufferSignatureId());
	}

	Renderer::IFramebuffer* FramebufferManager::getFramebufferByCompositorFramebufferId(CompositorFramebufferId compositorFramebufferId) const
	{
		Renderer::IFramebuffer* framebuffer = nullptr;

		// Map compositor framebuffer ID to framebuffer signature ID
		CompositorFramebufferIdToFramebufferSignatureId::const_iterator iterator = mCompositorFramebufferIdToFramebufferSignatureId.find(compositorFramebufferId);
		if (mCompositorFramebufferIdToFramebufferSignatureId.cend() != iterator)
		{
			// TODO(co) Is there need for a more efficient search?
			const FramebufferSignatureId framebufferSignatureId = iterator->second;
			for (const FramebufferElement& framebufferElement : mSortedFramebufferVector)
			{
				const FramebufferSignature& framebufferSignature = framebufferElement.framebufferSignature;
				if (framebufferSignature.getFramebufferSignatureId() == framebufferSignatureId)
				{
					framebuffer = framebufferElement.framebuffer;
					break;
				}
			}
			assert(nullptr != framebuffer);
		}
		else
		{
			// Error! Unknown compositor framebuffer ID, this shouldn't have happened.
			assert(false);
		}

		// Done
		return framebuffer;
	}

	Renderer::IFramebuffer* FramebufferManager::getFramebufferByCompositorFramebufferId(CompositorFramebufferId compositorFramebufferId, const Renderer::IRenderTarget& renderTarget, uint8_t numberOfMultisamples, float resolutionScale)
	{
		Renderer::IFramebuffer* framebuffer = nullptr;

		// Map compositor framebuffer ID to framebuffer signature ID
		CompositorFramebufferIdToFramebufferSignatureId::const_iterator iterator = mCompositorFramebufferIdToFramebufferSignatureId.find(compositorFramebufferId);
		if (mCompositorFramebufferIdToFramebufferSignatureId.cend() != iterator)
		{
			// TODO(co) Is there need for a more efficient search?
			const FramebufferSignatureId framebufferSignatureId = iterator->second;
			for (FramebufferElement& framebufferElement : mSortedFramebufferVector)
			{
				const FramebufferSignature& framebufferSignature = framebufferElement.framebufferSignature;
				if (framebufferSignature.getFramebufferSignatureId() == framebufferSignatureId)
				{
					// Do we need to create the renderer framebuffer instance right now?
					if (nullptr == framebufferElement.framebuffer)
					{
						// Get the texture instances
						const uint8_t numberOfColorTextures = framebufferSignature.getNumberOfColorTextures();
						Renderer::FramebufferAttachment colorFramebufferAttachments[8];
						for (uint8_t i = 0; i < numberOfColorTextures; ++i)
						{
							const AssetId colorTextureAssetId = framebufferSignature.getColorTextureAssetId(i);
							colorFramebufferAttachments[i].texture = isInitialized(colorTextureAssetId) ? mRenderTargetTextureManager.getTextureByAssetId(colorTextureAssetId, renderTarget, numberOfMultisamples, resolutionScale) : nullptr;
						}
						Renderer::FramebufferAttachment depthStencilFramebufferAttachment(isInitialized(framebufferSignature.getDepthStencilTextureAssetId()) ? mRenderTargetTextureManager.getTextureByAssetId(framebufferSignature.getDepthStencilTextureAssetId(), renderTarget, numberOfMultisamples, resolutionScale) : nullptr);

						// Create the framebuffer object (FBO) instance
						// -> The framebuffer automatically adds a reference to the provided textures
						framebufferElement.framebuffer = mRenderTargetTextureManager.getRendererRuntime().getRenderer().createFramebuffer(numberOfColorTextures, colorFramebufferAttachments, (nullptr != depthStencilFramebufferAttachment.texture) ? &depthStencilFramebufferAttachment : nullptr);
						RENDERER_SET_RESOURCE_DEBUG_NAME(framebufferElement.framebuffer, "Framebuffer manager")
						framebufferElement.framebuffer->addReference();
					}
					framebuffer = framebufferElement.framebuffer;
					break;
				}
			}
			assert(nullptr != framebuffer);
		}
		else
		{
			// Error! Unknown compositor framebuffer ID, this shouldn't have happened.
			assert(false);
		}

		// Done
		return framebuffer;
	}

	void FramebufferManager::releaseFramebufferBySignature(const FramebufferSignature& framebufferSignature)
	{
		const FramebufferElement framebufferElement(framebufferSignature);
		SortedFramebufferVector::iterator iterator = std::lower_bound(mSortedFramebufferVector.begin(), mSortedFramebufferVector.end(), framebufferElement, ::detail::orderFramebufferElementByFramebufferSignatureId);
		if (iterator != mSortedFramebufferVector.end() && iterator->framebufferSignature.getFramebufferSignatureId() == framebufferElement.framebufferSignature.getFramebufferSignatureId())
		{
			// Was this the last reference?
			if (1 == iterator->numberOfReferences)
			{
				if (nullptr != iterator->framebuffer)
				{
					iterator->framebuffer->releaseReference();
				}
				mSortedFramebufferVector.erase(iterator);
			}
			else
			{
				--iterator->numberOfReferences;
			}
		}
		else
		{
			// Error! Framebuffer signature isn't registered.
			assert(false);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
