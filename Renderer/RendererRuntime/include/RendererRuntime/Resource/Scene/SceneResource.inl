/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline SceneCullingManager& SceneResource::getSceneCullingManager() const
	{
		// We know that this pointer is always valid
		assert(nullptr != mSceneCullingManager);
		return *mSceneCullingManager;
	}

	inline void SceneResource::destroyAllSceneNodesAndItems()
	{
		destroyAllSceneNodes();
		destroyAllSceneItems();

		// Update the resource loading state
		setLoadingState(IResource::LoadingState::UNLOADED);
	}

	inline const SceneResource::SceneNodes& SceneResource::getSceneNodes() const
	{
		return mSceneNodes;
	}

	template <typename T> T* SceneResource::createSceneItem(SceneNode& sceneNode)
	{
		return static_cast<T*>(createSceneItem(T::TYPE_ID, sceneNode));
	}

	inline const SceneResource::SceneItems& SceneResource::getSceneItems() const
	{
		return mSceneItems;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline SceneResource::SceneResource() :
		mSceneFactory(nullptr),
		mSceneCullingManager(nullptr)
	{
		// Nothing here
	}

	inline SceneResource::~SceneResource()
	{
		// Sanity checks
		assert(nullptr == mSceneFactory);
		assert(nullptr == mSceneCullingManager);
		assert(mSceneNodes.empty());
		assert(mSceneItems.empty());
	}

	inline SceneResource& SceneResource::operator=(SceneResource&& sceneResource)
	{
		// Call base implementation
		IResource::operator=(std::move(sceneResource));

		// Swap data
		std::swap(mSceneFactory, sceneResource.mSceneFactory);
		std::swap(mSceneCullingManager, sceneResource.mSceneCullingManager);
		std::swap(mSceneNodes, sceneResource.mSceneNodes);
		std::swap(mSceneItems, sceneResource.mSceneItems);

		// Done
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
