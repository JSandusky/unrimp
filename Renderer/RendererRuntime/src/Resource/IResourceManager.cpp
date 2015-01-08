/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererRuntime/Resource/IResourceManager.h"
#include "RendererRuntime/Resource/IResourceLoader.h"

#include <assert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void IResourceManager::releaseResourceLoaderInstance(IResourceLoader& resourceLoader)
	{
		for (ResourceLoaderVector::iterator iterator = mUsedResourceLoaderInstances.begin(); iterator != mUsedResourceLoaderInstances.end(); ++iterator)
		{
			if (*iterator == &resourceLoader)
			{
				mUsedResourceLoaderInstances.erase(iterator);
				mFreeResourceLoaderInstances.push_back(&resourceLoader);
				return;
			}
		}

		// There's something funny going on here, an resource loader instance has been released which is not owned by this resource manager
		assert(false);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	IResourceManager::~IResourceManager()
	{
		{ // Destroy resource loader instances
			const size_t numberOfFreeResourceLoaderInstances = mFreeResourceLoaderInstances.size();
			for (size_t i = 0; i < numberOfFreeResourceLoaderInstances; ++i)
			{
				delete mFreeResourceLoaderInstances[i];
			}

			// At this point in time, there shouldn't be any used resource loader instances left
			assert(mUsedResourceLoaderInstances.empty());
		}
	}

	IResourceLoader* IResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		// TODO(co) A reverse iterator might be the better choice in here, check it (less moving around stuff when erasing an element)
		for (ResourceLoaderVector::iterator iterator = mFreeResourceLoaderInstances.begin(); iterator != mFreeResourceLoaderInstances.end(); ++iterator)
		{
			IResourceLoader* resourceLoader = *iterator;
			if (resourceLoader->getResourceLoaderTypeId() == resourceLoaderTypeId)
			{
				mFreeResourceLoaderInstances.erase(iterator);
				mUsedResourceLoaderInstances.push_back(resourceLoader);
				return resourceLoader;
			}
		}

		// Sorry, no free resource loader instance left
		return nullptr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
