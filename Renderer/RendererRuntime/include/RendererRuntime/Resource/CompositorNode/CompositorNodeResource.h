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
#include "RendererRuntime/Resource/CompositorNode/CompositorTarget.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorFramebuffer.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorRenderTargetTexture.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class CompositorNodeResource;
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
	typedef uint32_t																   CompositorNodeResourceId;	///< POD compositor node resource identifier
	typedef PackedElementManager<CompositorNodeResource, CompositorNodeResourceId, 32> CompositorNodeResources;


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class CompositorNodeResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend CompositorNodeResources;	// Type definition of template class
		friend class CompositorNodeResourceLoader;
		friend class CompositorNodeResourceManager;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<CompositorChannelId>		   CompositorChannels;	// TODO(co) Get rid of "std::vector" and dynamic memory handling in here? (need to introduce a maximum number of input channels for this)
		typedef std::vector<CompositorRenderTargetTexture> CompositorRenderTargetTextures;
		typedef std::vector<CompositorFramebuffer>		   CompositorFramebuffers;
		typedef std::vector<CompositorTarget>			   CompositorTargets;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		// TODO(co) Asynchronous loading completion, we might want to move this into "RendererRuntime::IResource"
		RENDERERRUNTIME_API_EXPORT void enforceFullyLoaded();

		//[-------------------------------------------------------]
		//[ Input channels                                        ]
		//[-------------------------------------------------------]
		inline void reserveInputChannels(uint32_t numberOfInputChannels);
		inline void addInputChannel(CompositorChannelId compositorChannelId);
		inline const CompositorChannels& getInputChannels() const;

		//[-------------------------------------------------------]
		//[ Render target textures                                ]
		//[-------------------------------------------------------]
		inline void reserveRenderTargetTextures(uint32_t numberOfRenderTargetTextures);
		inline void addRenderTargetTexture(AssetId assetId, const RenderTargetTextureSignature& renderTargetTextureSignature);
		inline const CompositorRenderTargetTextures& getRenderTargetTextures() const;

		//[-------------------------------------------------------]
		//[ Framebuffers                                          ]
		//[-------------------------------------------------------]
		inline void reserveFramebuffers(uint32_t numberOfFramebuffers);
		inline void addFramebuffer(CompositorFramebufferId compositorFramebufferId, const FramebufferSignature& framebufferSignature);
		inline const CompositorFramebuffers& getFramebuffers() const;

		//[-------------------------------------------------------]
		//[ Targets                                               ]
		//[-------------------------------------------------------]
		inline void reserveCompositorTargets(uint32_t numberOfCompositorTargets);
		inline CompositorTarget& addCompositorTarget(CompositorChannelId compositorChannelId, CompositorFramebufferId compositorFramebufferId);
		inline const CompositorTargets& getCompositorTargets() const;

		//[-------------------------------------------------------]
		//[ Output channels                                       ]
		//[-------------------------------------------------------]
		inline void reserveOutputChannels(uint32_t numberOfOutputChannels);
		inline void addOutputChannel(CompositorChannelId compositorChannelId);
		inline const CompositorChannels& getOutputChannels() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline CompositorNodeResource();
		inline virtual ~CompositorNodeResource();
		CompositorNodeResource(const CompositorNodeResource&) = delete;
		CompositorNodeResource& operator=(const CompositorNodeResource&) = delete;

		//[-------------------------------------------------------]
		//[ "RendererRuntime::PackedElementManager" management    ]
		//[-------------------------------------------------------]
		inline void initializeElement(CompositorNodeResourceId compositorNodeResourceId);
		void deinitializeElement();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		CompositorChannels			   mInputChannels;
		CompositorRenderTargetTextures mCompositorRenderTargetTextures;
		CompositorFramebuffers		   mCompositorFramebuffers;
		CompositorTargets			   mCompositorTargets;
		CompositorChannels			   mOutputChannels;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeResource.inl"
