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
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Core/NonCopyable.h"
#include "RendererRuntime/Core/StringId.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IResourceManager;
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
	typedef StringId AssetId;		///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"
	typedef uint32_t ResourceId;	///< POD resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class IResource : protected NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class ResourceStreamer;	// Is changing the resource loading state
		friend class IResourceManager;	// Is changing the resource loading state


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		enum class LoadingState
		{
			UNLOADED,	///< Not loaded
			LOADING,	///< Loading is in progress
			LOADED,		///< Fully loaded
			UNLOADING	///< Currently unloading	// TODO(co) Currently unused
		};


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline IResourceManager& getResourceManager() const;
		template <typename T> T& getResourceManager() const;
		inline ResourceId getId() const;
		inline AssetId getAssetId() const;
		inline LoadingState getLoadingState() const;
		RENDERERRUNTIME_API_EXPORT void connectResourceListener(IResourceListener& resourceListener);	// No guaranteed resource listener caller order
		RENDERERRUNTIME_API_EXPORT void disconnectResourceListener(IResourceListener& resourceListener);


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline IResource();
		inline virtual ~IResource();
		IResource(const IResource&) = delete;
		IResource& operator=(const IResource&) = delete;
		inline void setResourceManager(IResourceManager* resourceManager);
		inline void setAssetId(AssetId assetId);
		void setLoadingState(LoadingState loadingState);

		//[-------------------------------------------------------]
		//[ "RendererRuntime::PackedElementManager" management    ]
		//[-------------------------------------------------------]
		inline void initializeElement(ResourceId resourceId);
		RENDERERRUNTIME_API_EXPORT void deinitializeElement();


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<IResourceListener*> SortedResourceListeners;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IResourceManager*		mResourceManager;	///< Owner resource manager, always valid
		ResourceId				mResourceId;		///< Unique resource ID inside the resource manager
		AssetId					mAssetId;			///< In case the resource is an instance of an asset, this is the ID of this asset
		LoadingState			mLoadingState;
		SortedResourceListeners mSortedResourceListeners;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Detail/IResource.inl"
