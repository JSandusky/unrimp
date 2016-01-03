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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCacheManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCache.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"

#include <assert.h>


// Disable warnings
// TODO(co) See "RendererRuntime::PipelineStateCacheManager::PipelineStateCacheManager()": How the heck should we avoid such a situation without using complicated solutions like a pointer to an instance? (= more individual allocations/deallocations)
#pragma warning(disable: 4355)	// warning C4355: 'this': used in base member initializer list


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Renderer::IPipelineStatePtr PipelineStateCacheManager::getPipelineStateObjectPtr(const ShaderProperties& shaderProperties, const MaterialProperties& materialProperties)
	{
		// TODO(co) Asserts whether or not e.g. the material resource is using the owning material resource blueprint
		assert(mMaterialBlueprintResource.isFullyLoaded());

		// TODO(co) Pipeline state cache management
		if (nullptr == mPipelineStateCache)
		{
			mPipelineStateCache = new PipelineStateCache(*this, shaderProperties, materialProperties);
		}

		// Done
		return mPipelineStateCache->getPipelineStateObjectPtr();
	}

	void PipelineStateCacheManager::clearCache()
	{
		// TODO(co) Pipeline state cache management
		if (nullptr != mPipelineStateCache)
		{
			delete mPipelineStateCache;
			mPipelineStateCache = nullptr;
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	PipelineStateCacheManager::PipelineStateCacheManager(MaterialBlueprintResource& materialBlueprintResource) :
		mMaterialBlueprintResource(materialBlueprintResource),
		mProgramCacheManager(*this),
		mPipelineStateCache(nullptr)
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
