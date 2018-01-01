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
	inline MeshResourceId MeshSceneItem::getMeshResourceId() const
	{
		return mMeshResourceId;
	}

	inline uint32_t MeshSceneItem::getNumberOfSubMeshes() const
	{
		return static_cast<uint32_t>(mRenderableManager.getRenderables().size());
	}

	inline MaterialResourceId MeshSceneItem::getMaterialResourceIdOfSubMesh(uint32_t subMeshIndex) const
	{
		assert(subMeshIndex < mRenderableManager.getRenderables().size());
		return mRenderableManager.getRenderables()[subMeshIndex].getMaterialResourceId();
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	inline SceneItemTypeId MeshSceneItem::getSceneItemTypeId() const
	{
		return TYPE_ID;
	}

	inline void MeshSceneItem::onDetachedFromSceneNode(SceneNode& sceneNode)
	{
		mRenderableManager.setTransform(nullptr);

		// Call the base implementation
		ISceneItem::onDetachedFromSceneNode(sceneNode);
	}

	inline void MeshSceneItem::setVisible(bool visible)
	{
		mRenderableManager.setVisible(visible);
	}

	inline const RenderableManager* MeshSceneItem::getRenderableManager() const
	{
		return &mRenderableManager;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline MeshSceneItem::MeshSceneItem(SceneResource& sceneResource) :
		ISceneItem(sceneResource),
		mMeshResourceId(getUninitialized<MeshResourceId>())
	{
		// Nothing here
	}

	inline MeshSceneItem::~MeshSceneItem()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
