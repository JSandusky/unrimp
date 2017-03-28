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
#include "RendererRuntime/Core/Manager.h"
#include "RendererRuntime/Core/StringId.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IResourceLoader;
	class IResourceManager;
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
	typedef StringId ResourceLoaderTypeId;	///< Resource loader type identifier, internally just a POD "uint32_t", usually created by hashing the file format extension (if the resource loader is processing file data in the first place)


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Internal resource manager template base; not public used to keep template instantiation overhead under control
	*/
	class ResourceManagerTemplateBase : private Manager
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<IResourceLoader*> ResourceLoaders;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline ResourceManagerTemplateBase(IRendererRuntime& rendererRuntime, IResourceManager& resourceManager);
		~ResourceManagerTemplateBase();
		inline IRendererRuntime& getRendererRuntime() const;
		inline IResourceManager& getResourceManager() const;
		inline ResourceLoaders& getFreeResourceLoaderInstances();
		inline ResourceLoaders& getUsedResourceLoaderInstances();
		void releaseResourceLoaderInstance(IResourceLoader& resourceLoader);
		IResourceLoader* acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		ResourceManagerTemplateBase(const ResourceManagerTemplateBase&) = delete;
		ResourceManagerTemplateBase& operator=(const ResourceManagerTemplateBase&) = delete;


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		IRendererRuntime& mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance
		IResourceManager& mResourceManager;
		// Resource loader instances
		ResourceLoaders mFreeResourceLoaderInstances;
		ResourceLoaders mUsedResourceLoaderInstances;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Detail/ResourceManagerTemplateBase.inl"
