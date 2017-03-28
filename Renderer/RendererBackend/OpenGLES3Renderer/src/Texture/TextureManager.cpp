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
#include "OpenGLES3Renderer/Texture/TextureManager.h"
#include "OpenGLES3Renderer/Texture/Texture1D.h"
#include "OpenGLES3Renderer/Texture/Texture2D.h"
#include "OpenGLES3Renderer/Texture/Texture3D.h"
#include "OpenGLES3Renderer/Texture/TextureCube.h"
#include "OpenGLES3Renderer/Texture/Texture2DArray.h"
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"
#include "OpenGLES3Renderer/IExtensions.h"
#include "OpenGLES3Renderer/IContext.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureManager::TextureManager(OpenGLES3Renderer& openGLES3Renderer) :
		ITextureManager(openGLES3Renderer),
		mExtensions(&openGLES3Renderer.getContext().getExtensions())
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureManager methods      ]
	//[-------------------------------------------------------]
	Renderer::ITexture1D *TextureManager::createTexture1D(uint32_t width, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL ES 3 has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0)
		{
			return new Texture1D(static_cast<OpenGLES3Renderer&>(getRenderer()), width, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2D *TextureManager::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage, uint8_t, const Renderer::OptimizedTextureClearValue*)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL ES 3 has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			return new Texture2D(static_cast<OpenGLES3Renderer&>(getRenderer()), width, height, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2DArray *TextureManager::createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL ES 3 has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0 && numberOfSlices > 0)
		{
			return new Texture2DArray(static_cast<OpenGLES3Renderer&>(getRenderer()), width, height, numberOfSlices, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture3D *TextureManager::createTexture3D(uint32_t width, uint32_t height, uint32_t depth, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL ES 3 has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0 && depth > 0)
		{
			return new Texture3D(static_cast<OpenGLES3Renderer&>(getRenderer()), width, height, depth, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITextureCube *TextureManager::createTextureCube(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL ES 3 has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			return new TextureCube(static_cast<OpenGLES3Renderer&>(getRenderer()), width, height, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
