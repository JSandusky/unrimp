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
#include "RendererRuntime/Core/NonCopyable.h"
#include "RendererRuntime/Asset/Asset.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IFile;
	class IResourceManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId ResourceLoaderTypeId;	///< Resource loader type identifier, internally just a POD "uint32_t", usually created by hashing the file format extension (if the resource loader is processing file data in the first place)


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class IResourceLoader : protected NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class IResourceManager;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the owner resource manager
		*
		*  @return
		*    The owner resource manager
		*/
		inline IResourceManager& getResourceManager() const;

		/**
		*  @brief
		*    Return the asset the resource is using
		*
		*  @return
		*    The asset the resource is using
		*/
		inline const Asset& getAsset() const;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the resource loader type ID
		*/
		virtual ResourceLoaderTypeId getResourceLoaderTypeId() const = 0;

		/**
		*  @brief
		*    Called when the resource loader has to deserialize (usually from file) the internal data into memory
		*
		*  @param[in] file
		*    File to read from
		*/
		virtual void onDeserialization(IFile& file) = 0;

		/**
		*  @brief
		*    Called when the resource loader has to perform internal in-memory data processing
		*/
		virtual void onProcessing() = 0;

		/**
		*  @brief
		*    Called when the resource loader has to dispatch the data (e.g. to the renderer backend)
		*
		*  @return
		*    "true" if the resource is fully loaded, else "false" (e.g. asset dependencies are not fully loaded, yet) meaning this method will be called later on again
		*/
		virtual bool onDispatch() = 0;

		/**
		*  @brief
		*    Called when the resource loader is about to switch the resource into the loaded state
		*
		*  @return
		*    "true" if the resource is fully loaded, else "false" (e.g. asset dependencies are not fully loaded, yet) meaning this method will be called later on again
		*/
		virtual bool isFullyLoaded() = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline IResourceLoader(IResourceManager& resourceManager);
		inline virtual ~IResourceLoader();
		IResourceLoader(const IResourceLoader&) = delete;
		IResourceLoader& operator=(const IResourceLoader&) = delete;
		inline void initialize(const Asset& asset);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IResourceManager& mResourceManager;	///< Owner resource manager
		Asset			  mAsset;			///< In order to be multi-threading safe in here, we need an asset copy


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Detail/IResourceLoader.inl"
