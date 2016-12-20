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
#include "RendererRuntime/Core/Manager.h"
#include "RendererRuntime/Core/Renderer/FramebufferSignature.h"

#include <vector>
#include <unordered_map>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IFramebuffer;
	class IRenderTarget;
}
namespace RendererRuntime
{
	class RenderTargetTextureManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId CompositorFramebufferId;	///< Compositor framebuffer identifier, internally just a POD "uint32_t"


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class FramebufferManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		struct FramebufferElement
		{
			FramebufferSignature    framebufferSignature;
			Renderer::IFramebuffer* framebuffer;		///< Can be a null pointer, no "Renderer::IFramebufferPtr" to not have overhead when internally reallocating
			uint32_t				numberOfReferences;	///< Number of framebuffer references (don't misuse the renderer framebuffer reference counter for this)

			inline FramebufferElement() :
				framebuffer(nullptr),
				numberOfReferences(0)
			{
				// Nothing here
			}

			inline explicit FramebufferElement(const FramebufferSignature& _framebufferSignature) :
				framebufferSignature(_framebufferSignature),
				framebuffer(nullptr),
				numberOfReferences(0)
			{
				// Nothing here
			}

			inline FramebufferElement(const FramebufferSignature& _framebufferSignature, Renderer::IFramebuffer& _framebuffer) :
				framebufferSignature(_framebufferSignature),
				framebuffer(&_framebuffer),
				numberOfReferences(0)
			{
				// Nothing here
			}
		};


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline explicit FramebufferManager(RenderTargetTextureManager& renderTargetTextureManager);
		inline ~FramebufferManager();
		FramebufferManager(const FramebufferManager&) = delete;
		FramebufferManager& operator=(const FramebufferManager&) = delete;
		void clear();
		void clearRendererResources();
		void addFramebuffer(CompositorFramebufferId compositorFramebufferId, const FramebufferSignature& framebufferSignature);
		Renderer::IFramebuffer* getFramebufferByCompositorFramebufferId(CompositorFramebufferId compositorFramebufferId, const Renderer::IRenderTarget& mainRenderTarget, float resolutionScale);
		void releaseFramebufferBySignature(const FramebufferSignature& framebufferSignature);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<FramebufferElement> SortedFramebufferVector;
		typedef std::unordered_map<uint32_t, FramebufferSignatureId> CompositorFramebufferIdToFramebufferSignatureId;	///< Key = "RendererRuntime::CompositorFramebufferId"


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		RenderTargetTextureManager&						mRenderTargetTextureManager;	///< Render target texture manager, just shared so don't destroy the instance
		SortedFramebufferVector							mSortedFramebufferVector;
		CompositorFramebufferIdToFramebufferSignatureId	mCompositorFramebufferIdToFramebufferSignatureId;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/Renderer/FramebufferManager.inl"
