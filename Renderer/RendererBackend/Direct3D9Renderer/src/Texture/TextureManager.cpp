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
#include "Direct3D9Renderer/Texture/TextureManager.h"
#include "Direct3D9Renderer/Texture/Texture1D.h"
#include "Direct3D9Renderer/Texture/Texture2D.h"
#include "Direct3D9Renderer/Texture/Texture3D.h"
#include "Direct3D9Renderer/Texture/TextureCube.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureManager::TextureManager(Direct3D9Renderer& direct3D9Renderer) :
		ITextureManager(direct3D9Renderer)
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureManager methods      ]
	//[-------------------------------------------------------]
	Renderer::ITexture1D *TextureManager::createTexture1D(uint32_t width, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage textureUsage)
	{
		// Check whether or not the given texture dimension is valid
		if (width > 0)
		{
			return new Texture1D(static_cast<Direct3D9Renderer&>(getRenderer()), width, textureFormat, data, flags, textureUsage);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2D *TextureManager::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage textureUsage, uint8_t, const Renderer::OptimizedTextureClearValue*)
	{
		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			return new Texture2D(static_cast<Direct3D9Renderer&>(getRenderer()), width, height, textureFormat, data, flags, textureUsage);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2DArray *TextureManager::createTexture2DArray(uint32_t, uint32_t, uint32_t, Renderer::TextureFormat::Enum, const void *, uint32_t, Renderer::TextureUsage)
	{
		// Direct3D 9 has no 2D texture arrays
		return nullptr;
	}

	Renderer::ITexture3D *TextureManager::createTexture3D(uint32_t width, uint32_t height, uint32_t depth, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage textureUsage)
	{
		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0 && depth > 0)
		{
			return new Texture3D(static_cast<Direct3D9Renderer&>(getRenderer()), width, height, depth, textureFormat, data, flags, textureUsage);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITextureCube *TextureManager::createTextureCube(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage textureUsage)
	{
		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			return new TextureCube(static_cast<Direct3D9Renderer&>(getRenderer()), width, height, textureFormat, data, flags, textureUsage);
		}
		else
		{
			return nullptr;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
