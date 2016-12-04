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
#include "RendererRuntime/Core/Renderer/FramebufferManager.h"
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
	Renderer::IFramebufferPtr FramebufferManager::addFramebufferBySignature(const FramebufferSignature& framebufferSignature)
	{
		FramebufferElement framebufferElement(framebufferSignature);
		SortedFramebufferVector::iterator iterator = std::lower_bound(mSortedFramebufferVector.begin(), mSortedFramebufferVector.end(), framebufferElement, ::detail::orderFramebufferElementByFramebufferSignatureId);
		if (iterator == mSortedFramebufferVector.end() || iterator->framebufferSignature.getFramebufferSignatureId() != framebufferElement.framebufferSignature.getFramebufferSignatureId())
		{
			// Add new framebuffer

			// Create the texture instance, but without providing texture data (we use the texture as render target)
			// -> Use the "Renderer::TextureFlag::RENDER_TARGET"-flag to mark this texture as a render target
			// -> Required for Direct3D 9, Direct3D 10, Direct3D 11 and Direct3D 12
			// -> Not required for OpenGL and OpenGL ES 2
			// -> The optimized texture clear value is a Direct3D 12 related option
			Renderer::ITexture* texture2D = mRendererRuntime.getTextureManager().createTexture2D(framebufferSignature.getWidth(), framebufferSignature.getHeight(), framebufferSignature.getTextureFormat(), nullptr, Renderer::TextureFlag::RENDER_TARGET, Renderer::TextureUsage::DEFAULT);

			// Create the framebuffer object (FBO) instance
			// -> The framebuffer automatically adds a reference to the provided textures
			framebufferElement.framebuffer = mRendererRuntime.getRenderer().createFramebuffer(1, &texture2D);
			framebufferElement.framebuffer->addReference();

			// Register the new framebuffer element
			++framebufferElement.numberOfReferences;
			mSortedFramebufferVector.insert(iterator, framebufferElement);

			// Done
			return Renderer::IFramebufferPtr(framebufferElement.framebuffer);
		}
		else
		{
			// Just increase the number of references
			++iterator->numberOfReferences;
			return Renderer::IFramebufferPtr(iterator->framebuffer);
		}
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
				iterator->framebuffer->release();
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
