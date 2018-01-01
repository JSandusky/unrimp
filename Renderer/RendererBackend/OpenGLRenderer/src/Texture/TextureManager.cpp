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
#include "OpenGLRenderer/Texture/TextureManager.h"
#include "OpenGLRenderer/Texture/Texture1DDsa.h"
#include "OpenGLRenderer/Texture/Texture2DDsa.h"
#include "OpenGLRenderer/Texture/Texture3DDsa.h"
#include "OpenGLRenderer/Texture/TextureCubeDsa.h"
#include "OpenGLRenderer/Texture/Texture1DBind.h"
#include "OpenGLRenderer/Texture/Texture2DBind.h"
#include "OpenGLRenderer/Texture/Texture3DBind.h"
#include "OpenGLRenderer/Texture/TextureCubeBind.h"
#include "OpenGLRenderer/Texture/Texture2DArrayDsa.h"
#include "OpenGLRenderer/Texture/Texture2DArrayBind.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/Extensions.h"

#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureManager::TextureManager(OpenGLRenderer& openGLRenderer) :
		ITextureManager(openGLRenderer),
		mExtensions(&openGLRenderer.getExtensions())
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureManager methods      ]
	//[-------------------------------------------------------]
	Renderer::ITexture1D* TextureManager::createTexture1D(uint32_t width, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0)
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access() || mExtensions->isGL_ARB_direct_state_access())
			{
				// Effective direct state access (DSA)
				return RENDERER_NEW(getRenderer().getContext(), Texture1DDsa)(static_cast<OpenGLRenderer&>(getRenderer()), width, textureFormat, data, flags);
			}
			else
			{
				// Traditional bind version
				return RENDERER_NEW(getRenderer().getContext(), Texture1DBind)(static_cast<OpenGLRenderer&>(getRenderer()), width, textureFormat, data, flags);
			}
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2D* TextureManager::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage, uint8_t numberOfMultisamples, const Renderer::OptimizedTextureClearValue*)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access() || mExtensions->isGL_ARB_direct_state_access())
			{
				// Effective direct state access (DSA)
				return RENDERER_NEW(getRenderer().getContext(), Texture2DDsa)(static_cast<OpenGLRenderer&>(getRenderer()), width, height, textureFormat, data, flags, numberOfMultisamples);
			}
			else
			{
				// Traditional bind version
				return RENDERER_NEW(getRenderer().getContext(), Texture2DBind)(static_cast<OpenGLRenderer&>(getRenderer()), width, height, textureFormat, data, flags, numberOfMultisamples);
			}
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2DArray* TextureManager::createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL has no texture usage indication

		// Check whether or not the given texture dimension is valid, "GL_EXT_texture_array" required
		if (width > 0 && height > 0 && numberOfSlices > 0 && mExtensions->isGL_EXT_texture_array())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access() || mExtensions->isGL_ARB_direct_state_access())
			{
				// Effective direct state access (DSA)
				return RENDERER_NEW(getRenderer().getContext(), Texture2DArrayDsa)(static_cast<OpenGLRenderer&>(getRenderer()), width, height, numberOfSlices, textureFormat, data, flags);
			}
			else
			{
				// Traditional bind version
				return RENDERER_NEW(getRenderer().getContext(), Texture2DArrayBind)(static_cast<OpenGLRenderer&>(getRenderer()), width, height, numberOfSlices, textureFormat, data, flags);
			}
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture3D* TextureManager::createTexture3D(uint32_t width, uint32_t height, uint32_t depth, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0 && depth > 0)
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access() || mExtensions->isGL_ARB_direct_state_access())
			{
				// Effective direct state access (DSA)
				return RENDERER_NEW(getRenderer().getContext(), Texture3DDsa)(static_cast<OpenGLRenderer&>(getRenderer()), width, height, depth, textureFormat, data, flags);
			}
			else
			{
				// Traditional bind version
				return RENDERER_NEW(getRenderer().getContext(), Texture3DBind)(static_cast<OpenGLRenderer&>(getRenderer()), width, height, depth, textureFormat, data, flags);
			}
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITextureCube* TextureManager::createTextureCube(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			// Is "GL_EXT_direct_state_access" or "GL_ARB_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access() || mExtensions->isGL_ARB_direct_state_access())
			{
				// Effective direct state access (DSA)
				return RENDERER_NEW(getRenderer().getContext(), TextureCubeDsa)(static_cast<OpenGLRenderer&>(getRenderer()), width, height, textureFormat, data, flags);
			}
			else
			{
				// Traditional bind version
				return RENDERER_NEW(getRenderer().getContext(), TextureCubeBind)(static_cast<OpenGLRenderer&>(getRenderer()), width, height, textureFormat, data, flags);
			}
		}
		else
		{
			return nullptr;
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void TextureManager::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), TextureManager, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
