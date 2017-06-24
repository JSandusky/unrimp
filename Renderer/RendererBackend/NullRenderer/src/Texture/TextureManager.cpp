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
#include "NullRenderer/Texture/TextureManager.h"
#include "NullRenderer/Texture/Texture1D.h"
#include "NullRenderer/Texture/Texture2D.h"
#include "NullRenderer/Texture/Texture3D.h"
#include "NullRenderer/Texture/TextureCube.h"
#include "NullRenderer/Texture/Texture2DArray.h"
#include "NullRenderer/NullRenderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace NullRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureManager::TextureManager(NullRenderer& nullRenderer) :
		ITextureManager(nullRenderer)
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureManager methods      ]
	//[-------------------------------------------------------]
	Renderer::ITexture1D* TextureManager::createTexture1D(uint32_t width, Renderer::TextureFormat::Enum, const void *, uint32_t, Renderer::TextureUsage)
	{
		// Check whether or not the given texture dimension is valid
		if (width > 0)
		{
			return new Texture1D(static_cast<NullRenderer&>(getRenderer()), width);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2D* TextureManager::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum, const void *, uint32_t, Renderer::TextureUsage, uint8_t, const Renderer::OptimizedTextureClearValue*)
	{
		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			return new Texture2D(static_cast<NullRenderer&>(getRenderer()), width, height);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2DArray* TextureManager::createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum, const void *, uint32_t, Renderer::TextureUsage)
	{
		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0 && numberOfSlices > 0)
		{
			return new Texture2DArray(static_cast<NullRenderer&>(getRenderer()), width, height, numberOfSlices);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture3D* TextureManager::createTexture3D(uint32_t width, uint32_t height, uint32_t depth, Renderer::TextureFormat::Enum, const void *, uint32_t, Renderer::TextureUsage)
	{
		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0 && depth > 0)
		{
			return new Texture3D(static_cast<NullRenderer&>(getRenderer()), width, height, depth);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITextureCube* TextureManager::createTextureCube(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum, const void *, uint32_t, Renderer::TextureUsage)
	{
		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			return new TextureCube(static_cast<NullRenderer&>(getRenderer()), width, height);
		}
		else
		{
			return nullptr;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // NullRenderer
