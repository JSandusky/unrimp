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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Detail/IResourceManager.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ITexture;
}
namespace RendererRuntime
{
	class TextureResource;
	class IRendererRuntime;
	class ITextureResourceLoader;
	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS> class ResourceManagerTemplate;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t TextureResourceId;		///< POD texture resource identifier
	typedef StringId ResourceLoaderTypeId;	///< Resource loader type identifier, internally just a POD "uint32_t", usually created by hashing the file format extension (if the resource loader is processing file data in the first place)


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Texture resource manager class
	*
	*  @remarks
	*    The texture manager automatically generates some dynamic default texture assets one can reference e.g. inside material blueprint resources:
	*    - "Unrimp/Texture/DynamicByCode/WhiteMap1D"
	*    - "Unrimp/Texture/DynamicByCode/WhiteMap2D"
	*    - "Unrimp/Texture/DynamicByCode/WhiteMapCube"
	*    - "Unrimp/Texture/DynamicByCode/BlackMap1D"
	*    - "Unrimp/Texture/DynamicByCode/BlackMap2D"
	*    - "Unrimp/Texture/DynamicByCode/BlackMapCube"
	*    - "Unrimp/Texture/DynamicByCode/IdentityDiffuseMap2D"
	*    - "Unrimp/Texture/DynamicByCode/IdentityAlphaMap2D"
	*    - "Unrimp/Texture/DynamicByCode/IdentityNormalMap2D"
	*    - "Unrimp/Texture/DynamicByCode/IdentityRoughnessMap2D"
	*    - "Unrimp/Texture/DynamicByCode/IdentityMetallicMap2D"
	*    - "Unrimp/Texture/DynamicByCode/IdentityEmissiveMap2D"
	*/
	class TextureResourceManager : private IResourceManager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline uint8_t getNumberOfTopMipmapsToRemove() const;
		RENDERERRUNTIME_API_EXPORT void setNumberOfTopMipmapsToRemove(uint8_t numberOfTopMipmapsToRemove);
		RENDERERRUNTIME_API_EXPORT TextureResource* getTextureResourceByAssetId(AssetId assetId) const;	// Considered to be inefficient, avoid method whenever possible
		RENDERERRUNTIME_API_EXPORT void loadTextureResourceByAssetId(AssetId assetId, AssetId fallbackTextureAssetId, TextureResourceId& textureResourceId, IResourceListener* resourceListener = nullptr, bool rgbHardwareGammaCorrection = false, bool reload = false);	// Asynchronous
		RENDERERRUNTIME_API_EXPORT TextureResourceId createTextureResourceByAssetId(AssetId assetId, Renderer::ITexture& texture, bool rgbHardwareGammaCorrection = false);	// Texture resource is not allowed to exist, yet
		RENDERERRUNTIME_API_EXPORT void destroyTextureResource(TextureResourceId textureResourceId);


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	public:
		virtual uint32_t getNumberOfResources() const override;
		virtual IResource& getResourceByIndex(uint32_t index) const override;
		virtual IResource& getResourceByResourceId(ResourceId resourceId) const override;
		virtual IResource* tryGetResourceByResourceId(ResourceId resourceId) const override;
		virtual void reloadResourceByAssetId(AssetId assetId) override;
		virtual void update() override;


	//[-------------------------------------------------------]
	//[ Private virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	private:
		virtual void releaseResourceLoaderInstance(IResourceLoader& resourceLoader) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit TextureResourceManager(IRendererRuntime& rendererRuntime);
		virtual ~TextureResourceManager();
		TextureResourceManager(const TextureResourceManager&) = delete;
		TextureResourceManager& operator=(const TextureResourceManager&) = delete;
		IResourceLoader* acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint8_t mNumberOfTopMipmapsToRemove;	///< The number of top mipmaps to remove while loading textures for efficient texture quality reduction. By setting this to e.g. two a 4096x4096 texture will become 1024x1024.

		// Internal resource manager implementation
		ResourceManagerTemplate<TextureResource, ITextureResourceLoader, TextureResourceId, 2048>* mInternalResourceManager;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Texture/TextureResourceManager.inl"
