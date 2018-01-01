/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "OpenGLRenderer/RenderTarget/RenderPass.h"

#include <Renderer/IRenderer.h>
#include <Renderer/IAllocator.h>

#include <cstring>	// For "memcpy()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RenderPass::RenderPass(Renderer::IRenderer& renderer, uint32_t numberOfColorAttachments, const Renderer::TextureFormat::Enum* colorAttachmentTextureFormats, Renderer::TextureFormat::Enum depthStencilAttachmentTextureFormat, uint8_t numberOfMultisamples) :
		IRenderPass(renderer),
		mNumberOfColorAttachments(numberOfColorAttachments),
		mDepthStencilAttachmentTextureFormat(depthStencilAttachmentTextureFormat),
		mNumberOfMultisamples(numberOfMultisamples)
	{
		RENDERER_ASSERT(renderer.getContext(), mNumberOfColorAttachments < 8, "Invalid number of OpenGL color attachments")
		memcpy(mColorAttachmentTextureFormats, colorAttachmentTextureFormats, sizeof(Renderer::TextureFormat::Enum) * mNumberOfColorAttachments);
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void RenderPass::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), RenderPass, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
