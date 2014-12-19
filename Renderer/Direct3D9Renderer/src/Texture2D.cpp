/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "Direct3D9Renderer/Texture2D.h"
#include "Direct3D9Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D9Renderer/Mapping.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"
#include "Direct3D9Renderer/Direct3D9RuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture2D::Texture2D(Direct3D9Renderer &direct3D9Renderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage::Enum textureUsage) :
		ITexture2D(direct3D9Renderer, width, height),
		mDirect3DTexture9(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&direct3D9Renderer)

		// Get the Direct3D 9 usage indication
		DWORD direct3D9Usage = (flags & Renderer::TextureFlag::MIPMAPS) ? D3DUSAGE_AUTOGENMIPMAP : 0u;
		switch (textureUsage)
		{
			case Renderer::TextureUsage::DYNAMIC:
				direct3D9Usage |= D3DUSAGE_DYNAMIC;
				break;

			case Renderer::TextureUsage::DEFAULT:
			case Renderer::TextureUsage::IMMUTABLE:
			case Renderer::TextureUsage::STAGING:
			default:
				// "Renderer::TextureUsage::DEFAULT", "Renderer::TextureUsage::IMMUTABLE" and "Renderer::TextureUsage::STAGING" have no Direct3D 9 equivalent
				// -> See "D3DUSAGE"-documentation at http://msdn.microsoft.com/en-us/library/windows/desktop/bb172625%28v=vs.85%29.aspx
				break;
		}

		// Use this texture as render target?
		if (flags & Renderer::TextureFlag::RENDER_TARGET)
		{
			direct3D9Usage |= D3DUSAGE_RENDERTARGET;
		}

		// Get the DirextX 9 format
		const D3DFORMAT d3dFormat = static_cast<D3DFORMAT>(Mapping::getDirect3D9Format(textureFormat));

		// Create Direct3D 9 texture, let Direct3D create the mipmaps for us
		if (direct3D9Renderer.getDirect3DDevice9()->CreateTexture(width, height, 0, direct3D9Usage, d3dFormat, D3DPOOL_DEFAULT, &mDirect3DTexture9, nullptr) == D3D_OK)
		{
			// Upload data?
			if (nullptr != data)
			{
				// Get the surface
				IDirect3DSurface9 *direct3DSurface9 = nullptr;
				mDirect3DTexture9->GetSurfaceLevel(0, &direct3DSurface9);
				if (nullptr != direct3DSurface9)
				{
					// Upload the texture data
					const RECT sourceRect[] = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
					D3DXLoadSurfaceFromMemory(direct3DSurface9, nullptr, nullptr, data, d3dFormat, width * Mapping::getDirect3D9Size(textureFormat), nullptr, sourceRect, D3DX_FILTER_NONE, 0);

					// Release the surface
					direct3DSurface9->Release();
				}
			}
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			setDebugName("2D texture");
		#endif

		// End debug event
		RENDERER_END_DEBUG_EVENT(&direct3D9Renderer)
	}

	Texture2D::~Texture2D()
	{
		if (nullptr != mDirect3DTexture9)
		{
			mDirect3DTexture9->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void Texture2D::setDebugName(const char *name)
	{
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			// Valid Direct3D 9 texture?
			if (nullptr != mDirect3DTexture9)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mDirect3DTexture9->SetPrivateData(WKPDID_D3DDebugObjectName, nullptr, 0, 0);
				mDirect3DTexture9->SetPrivateData(WKPDID_D3DDebugObjectName, name, strlen(name), 0);

				// Set debug name of the texture surfaces
				const DWORD levelCount = mDirect3DTexture9->GetLevelCount();
				for (DWORD level = 0; level < levelCount; ++level)
				{
					// Get the Direct3D 9 surface
					const size_t nameLength = strlen(name);
					IDirect3DSurface9 *direct3DSurface9 = nullptr;
					mDirect3DTexture9->GetSurfaceLevel(level, &direct3DSurface9);
					if (nullptr != direct3DSurface9)
					{
						// Set the debug name
						// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
						direct3DSurface9->SetPrivateData(WKPDID_D3DDebugObjectName, nullptr, 0, 0);
						direct3DSurface9->SetPrivateData(WKPDID_D3DDebugObjectName, name, nameLength, 0);

						// Release the Direct3D 9 surface
						direct3DSurface9->Release();
					}
				}
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
