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
#include "RendererRuntime/Core/StringId.h"
#include "RendererRuntime/Core/Renderer/FramebufferSignature.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId CompositorChannelId;		///< Compositor channel identifier, internally just a POD "uint32_t"
	typedef StringId CompositorFramebufferId;	///< Compositor framebuffer identifier, internally just a POD "uint32_t"
	typedef StringId CompositorPassTypeId;		///< Compositor pass type identifier, internally just a POD "uint32_t"


	// -> Compositor node file format content:
	//    - Compositor node header
	namespace v1CompositorNode
	{


		//[-------------------------------------------------------]
		//[ Definitions                                           ]
		//[-------------------------------------------------------]
		static const uint32_t FORMAT_TYPE	 = StringId("CompositorNode");
		static const uint32_t FORMAT_VERSION = 1;

		#pragma pack(push)
		#pragma pack(1)
			struct Header
			{
				uint32_t formatType;
				uint16_t formatVersion;
				uint32_t numberOfInputChannels;
				uint32_t numberOfFramebuffers;
				uint32_t numberOfTargets;
				uint32_t numberOfOutputChannels;
			};

			struct Channel
			{
				CompositorChannelId id;
			};

			struct Framebuffer
			{
				CompositorFramebufferId compositorFramebufferId;
				FramebufferSignature	framebufferSignature;
			};

			struct Target
			{
				CompositorChannelId		compositorChannelId;
				CompositorFramebufferId compositorFramebufferId;
				uint32_t				numberOfPasses;
			};

			struct PassHeader
			{
				CompositorPassTypeId compositorPassTypeId;
				uint32_t			 numberOfBytes;
			};

			struct PassClear
			{
				float color[4];

				PassClear() :
					color{ 0.0f, 0.0f, 0.0f, 0.0f }
				{}
			};

			struct PassQuad
			{
			};

			struct PassScene
			{
				uint8_t	minimumRenderQueueIndex;	///< Inclusive
				uint8_t	maximumRenderQueueIndex;	///< Inclusive
				bool	transparentPass;

				PassScene() :
					minimumRenderQueueIndex(0),
					maximumRenderQueueIndex(255),
					transparentPass(false)
				{}
			};

			struct PassDebugGui
			{
			};
		#pragma pack(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
	} // v1CompositorNode
} // RendererRuntime
