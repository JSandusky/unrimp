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
#include "RendererRuntime/Core/PackedElementManager.h"
#include "RendererRuntime/Resource/Detail/IResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
	class IResourceListener;
	class LightBufferManager;
	class InstanceBufferManager;
	class IMaterialBlueprintResourceListener;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t																		 MaterialBlueprintResourceId;	///< POD material blueprint resource identifier
	typedef PackedElementManager<MaterialBlueprintResource, MaterialBlueprintResourceId, 64> MaterialBlueprintResources;


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class MaterialBlueprintResourceManager : private IResourceManager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;
		friend class IResource;	// Needed so that inside this classes an static_cast<CompositorNodeResourceManager*>(IResourceManager*) works


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline IRendererRuntime& getRendererRuntime() const;
		inline const MaterialBlueprintResources& getMaterialBlueprintResources() const;
		RENDERERRUNTIME_API_EXPORT MaterialBlueprintResourceId loadMaterialBlueprintResourceByAssetId(AssetId assetId, IResourceListener* resourceListener = nullptr, bool reload = false);	// Asynchronous

		inline IMaterialBlueprintResourceListener& getMaterialBlueprintResourceListener() const;
		RENDERERRUNTIME_API_EXPORT void setMaterialBlueprintResourceListener(IMaterialBlueprintResourceListener* materialBlueprintResourceListener);	// Does not take over the control of the memory

		/**
		*  @brief
		*    Return the global material properties
		*
		*  @return
		*    The global material properties
		*/
		inline MaterialProperties& getGlobalMaterialProperties();
		inline const MaterialProperties& getGlobalMaterialProperties() const;

		inline InstanceBufferManager& getInstanceBufferManager() const;
		inline LightBufferManager& getLightBufferManager() const;


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
		explicit MaterialBlueprintResourceManager(IRendererRuntime& rendererRuntime);
		virtual ~MaterialBlueprintResourceManager();
		MaterialBlueprintResourceManager(const MaterialBlueprintResourceManager&) = delete;
		MaterialBlueprintResourceManager& operator=(const MaterialBlueprintResourceManager&) = delete;
		IResourceLoader* acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&					mRendererRuntime;					///< Renderer runtime instance, do not destroy the instance
		MaterialBlueprintResources			mMaterialBlueprintResources;
		IMaterialBlueprintResourceListener*	mMaterialBlueprintResourceListener;	///< Material blueprint resource listener, always valid, do not destroy the instance
		MaterialProperties					mGlobalMaterialProperties;			///< Global material properties
		InstanceBufferManager*				mInstanceBufferManager;				///< Instance buffer manager, always valid
		LightBufferManager*					mLightBufferManager;				///< Light buffer manager, always valid


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.inl"
