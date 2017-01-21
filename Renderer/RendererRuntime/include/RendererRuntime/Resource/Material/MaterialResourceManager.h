/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include "RendererRuntime/Core/PackedElementManager.h"
#include "RendererRuntime/Resource/Detail/IResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
	class IResourceListener;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t														 MaterialResourceId;	///< POD material resource identifier
	typedef PackedElementManager<MaterialResource, MaterialResourceId, 4096> MaterialResources;
	typedef StringId														 MaterialTechniqueId;	///< Material technique identifier, internally just a POD "uint32_t", result of hashing the material technique name


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class MaterialResourceManager : private IResourceManager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;
		friend class IResource;			// Needed so that inside this classes an static_cast<CompositorNodeResourceManager*>(IResourceManager*) works
		friend class MaterialResource;	// Needed so that inside this classes an static_cast<CompositorNodeResourceManager*>(IResourceManager*) works


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT static const MaterialTechniqueId DEFAULT_MATERIAL_TECHNIQUE_ID;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline IRendererRuntime& getRendererRuntime() const;
		inline const MaterialResources& getMaterialResources() const;
		RENDERERRUNTIME_API_EXPORT MaterialResource* getMaterialResourceByAssetId(AssetId assetId) const;
		inline MaterialResourceId getMaterialResourceIdByAssetId(AssetId assetId) const;
		RENDERERRUNTIME_API_EXPORT MaterialResourceId loadMaterialResourceByAssetId(AssetId assetId, IResourceListener* resourceListener = nullptr, bool reload = false);	// Asynchronous
		RENDERERRUNTIME_API_EXPORT MaterialResourceId createMaterialResourceByAssetId(AssetId assetId, AssetId materialBlueprintAssetId, MaterialTechniqueId materialTechniqueId);	// Material resource is not allowed to exist, yet
		RENDERERRUNTIME_API_EXPORT MaterialResourceId createMaterialResourceByCloning(MaterialResourceId parentMaterialResourceId, AssetId assetId = getUninitialized<AssetId>());	// Parent material resource must be fully loaded
		inline void destroyMaterialResource(MaterialResourceId materialResourceId);


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	public:
		inline virtual IResource& getResourceByResourceId(ResourceId resourceId) const override;
		inline virtual IResource* tryGetResourceByResourceId(ResourceId resourceId) const override;
		virtual void reloadResourceByAssetId(AssetId assetId) override;
		virtual void update() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline explicit MaterialResourceManager(IRendererRuntime& rendererRuntime);
		inline virtual ~MaterialResourceManager();
		MaterialResourceManager(const MaterialResourceManager&) = delete;
		MaterialResourceManager& operator=(const MaterialResourceManager&) = delete;
		IResourceLoader* acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime& mRendererRuntime;		///< Renderer runtime instance, do not destroy the instance
		MaterialResources mMaterialResources;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Material/MaterialResourceManager.inl"
