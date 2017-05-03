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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Vr/OpenVR/Loader/OpenVRTextureResourceLoader.h"
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <openvr/openvr.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId OpenVRTextureResourceLoader::TYPE_ID("openvr_texture");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void OpenVRTextureResourceLoader::onProcessing()
	{
		// Load the render model texture
		vr::IVRRenderModels* vrRenderModels = vr::VRRenderModels();
		vr::EVRRenderModelError vrRenderModelError = vr::VRRenderModelError_Loading;
		const vr::TextureID_t diffuseTextureId = static_cast<vr::TextureID_t>(std::atoi(getAsset().assetFilename));
		while (vrRenderModelError == vr::VRRenderModelError_Loading)
		{
			vrRenderModelError = vrRenderModels->LoadTexture_Async(diffuseTextureId, &mVrRenderModelTextureMap);
			if (vrRenderModelError == vr::VRRenderModelError_Loading)
			{
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(1ms);
			}
		}
		if (vr::VRRenderModelError_None != vrRenderModelError)
		{
			RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF("Error: Unable to load OpenVR diffuse texture %d: %s", diffuseTextureId, vrRenderModels->GetRenderModelErrorNameFromEnum(vrRenderModelError));
			return;
		}

		// Can we create the renderer resource asynchronous as well?
		if (mRendererRuntime.getRenderer().getCapabilities().nativeMultiThreading)
		{
			mTexture = createRendererTexture();
		}
	}

	bool OpenVRTextureResourceLoader::onDispatch()
	{
		// Create the renderer texture instance
		if (nullptr != mTexture || nullptr != mVrRenderModelTextureMap)
		{
			mTextureResource->setTexture(*(mRendererRuntime.getRenderer().getCapabilities().nativeMultiThreading ? mTexture : createRendererTexture()));
		}

		// Fully loaded
		return true;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	Renderer::ITexture* OpenVRTextureResourceLoader::createRendererTexture()
	{
		// Create the renderer texture instance
		const bool rgbHardwareGammaCorrection = true;	// TODO(co) It must be possible to set the property name from the outside: Ask the material blueprint whether or not hardware gamma correction should be used
		Renderer::ITexture2D* texture2D = mRendererRuntime.getTextureManager().createTexture2D(mVrRenderModelTextureMap->unWidth, mVrRenderModelTextureMap->unHeight, rgbHardwareGammaCorrection ? Renderer::TextureFormat::R8G8B8A8_SRGB : Renderer::TextureFormat::R8G8B8A8, static_cast<const void*>(mVrRenderModelTextureMap->rubTextureMapData), Renderer::TextureFlag::GENERATE_MIPMAPS);
		RENDERER_SET_RESOURCE_DEBUG_NAME(texture2D, getAsset().assetFilename)

		// Free the render model texture, if necessary
		if (nullptr != mVrRenderModelTextureMap)
		{
			vr::VRRenderModels()->FreeTexture(mVrRenderModelTextureMap);
			mVrRenderModelTextureMap = nullptr;
		}

		// Done
		return texture2D;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
