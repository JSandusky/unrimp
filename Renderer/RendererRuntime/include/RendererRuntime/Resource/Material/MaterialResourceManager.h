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
#include "RendererRuntime/Resource/IResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t												   MaterialResourceId;	///< POD material resource identifier
	typedef PackedElementManager<MaterialResource, MaterialResourceId> MaterialResources;


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class MaterialResourceManager : private IResourceManager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		const MaterialResources& getMaterialResources() const;

		// TODO(co) Work-in-progress
		RENDERERRUNTIME_API_EXPORT MaterialResourceId loadMaterialResourceByAssetId(AssetId assetId, bool reload = false);


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	public:
		virtual void reloadResourceByAssetId(AssetId assetId) override;
		virtual void update() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit MaterialResourceManager(IRendererRuntime& rendererRuntime);
		virtual ~MaterialResourceManager();
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
