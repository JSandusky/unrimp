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
#include "RendererRuntime/Resource/Detail/ResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialProperties.h"

#include <mutex>
#include <unordered_map>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IFile;
	class MemoryFile;
	class IRendererRuntime;
	class LightBufferManager;
	class InstanceBufferManager;
	class IndirectBufferManager;
	class MaterialBlueprintResource;
	class MaterialBlueprintResourceLoader;
	class IMaterialBlueprintResourceListener;
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
	typedef uint32_t MaterialBlueprintResourceId;	///< POD material blueprint resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class MaterialBlueprintResourceManager : public ResourceManager<MaterialBlueprintResource>
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;
		friend class IResource;				// Needed so that inside this classes an static_cast<CompositorNodeResourceManager*>(IResourceManager*) works
		friend class MaterialTechnique;		// Needs to be able to call "RendererRuntime::MaterialBlueprintResourceManager::addSerializedPipelineState()"
		friend class PipelineStateCompiler;	// Needs to be able to call "RendererRuntime::MaterialBlueprintResourceManager::applySerializedPipelineState()"


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::unordered_map<uint32_t, Renderer::SerializedPipelineState> SerializedPipelineStates;	///< Key = FNV1a hash of "Renderer::SerializedPipelineState"


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline IRendererRuntime& getRendererRuntime() const;
		inline bool getCreateInitialPipelineStateCaches() const;
		inline void setCreateInitialPipelineStateCaches(bool createInitialPipelineStateCaches);
		RENDERERRUNTIME_API_EXPORT void loadMaterialBlueprintResourceByAssetId(AssetId assetId, MaterialBlueprintResourceId& materialBlueprintResourceId, IResourceListener* resourceListener = nullptr, bool reload = false, ResourceLoaderTypeId resourceLoaderTypeId = getUninitialized<ResourceLoaderTypeId>());	// Asynchronous
		inline IMaterialBlueprintResourceListener& getMaterialBlueprintResourceListener() const;
		RENDERERRUNTIME_API_EXPORT void setMaterialBlueprintResourceListener(IMaterialBlueprintResourceListener* materialBlueprintResourceListener);	// Does not take over the control of the memory

		/**
		*  @brief
		*    Return the global material properties
		*
		*  @return
		*    The global material properties
		*
		*  @remarks
		*    The material blueprint resource manager itself is setting the following global material properties:
		*    - Floating point property "GlobalPastSecondsSinceLastFrame"
		*    - Floating point property "GlobalTimeInSeconds"
		*    - Integer property "GlobalNumberOfMultisamples" (see "RendererRuntime::CompositorWorkspaceInstance::setNumberOfMultisamples()")
		*/
		inline MaterialProperties& getGlobalMaterialProperties();
		inline const MaterialProperties& getGlobalMaterialProperties() const;

		/**
		*  @brief
		*    Called pre command buffer execution
		*/
		void onPreCommandBufferExecution();

		//[-------------------------------------------------------]
		//[ Default texture filtering                             ]
		//[-------------------------------------------------------]
		inline Renderer::FilterMode getDefaultTextureFilterMode() const;
		inline uint8_t getDefaultMaximumTextureAnisotropy() const;
		RENDERERRUNTIME_API_EXPORT void setDefaultTextureFiltering(Renderer::FilterMode filterMode, uint8_t maximumAnisotropy);

		//[-------------------------------------------------------]
		//[ Manager                                               ]
		//[-------------------------------------------------------]
		inline InstanceBufferManager& getInstanceBufferManager() const;
		inline IndirectBufferManager& getIndirectBufferManager() const;
		inline LightBufferManager& getLightBufferManager() const;


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
		virtual IResourceLoader* createResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit MaterialBlueprintResourceManager(IRendererRuntime& rendererRuntime);
		virtual ~MaterialBlueprintResourceManager();
		explicit MaterialBlueprintResourceManager(const MaterialBlueprintResourceManager&) = delete;
		MaterialBlueprintResourceManager& operator=(const MaterialBlueprintResourceManager&) = delete;

		//[-------------------------------------------------------]
		//[ Pipeline state object cache                           ]
		//[-------------------------------------------------------]
		void addSerializedPipelineState(uint32_t serializedPipelineStateHash, const Renderer::SerializedPipelineState& serializedPipelineState);
		void applySerializedPipelineState(uint32_t serializedPipelineStateHash, Renderer::PipelineState& pipelineState);
		void clearPipelineStateObjectCache();
		void loadPipelineStateObjectCache(IFile& file);
		bool doesPipelineStateObjectCacheNeedSaving() const;
		void savePipelineStateObjectCache(MemoryFile& memoryFile);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&					mRendererRuntime;					///< Renderer runtime instance, do not destroy the instance
		bool								mCreateInitialPipelineStateCaches;	///< Create initial pipeline state caches after a material blueprint has been loaded?
		IMaterialBlueprintResourceListener*	mMaterialBlueprintResourceListener;	///< Material blueprint resource listener, always valid, do not destroy the instance
		MaterialProperties					mGlobalMaterialProperties;			///< Global material properties
		Renderer::FilterMode				mDefaultTextureFilterMode;			///< Default texture filter mode
		uint8_t								mDefaultMaximumTextureAnisotropy;	///< Default maximum texture anisotropy
		std::mutex							mSerializedPipelineStatesMutex;		///< "RendererRuntime::PipelineStateCompiler" is running asynchronous, hence we need to synchronize the serialized pipeline states access
		SerializedPipelineStates			mSerializedPipelineStates;			///< Serialized pipeline states
		InstanceBufferManager*				mInstanceBufferManager;				///< Instance buffer manager, always valid in a sane none-legacy environment
		IndirectBufferManager*				mIndirectBufferManager;				///< Indirect buffer manager, always valid in a sane none-legacy environment
		LightBufferManager*					mLightBufferManager;				///< Light buffer manager, always valid in a sane none-legacy environment

		// Internal resource manager implementation
		ResourceManagerTemplate<MaterialBlueprintResource, MaterialBlueprintResourceLoader, MaterialBlueprintResourceId, 64>* mInternalResourceManager;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.inl"
