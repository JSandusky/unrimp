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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline uint8_t CompositorResourcePassScene::getMinimumRenderQueueIndex() const
	{
		return mMinimumRenderQueueIndex;
	}

	inline uint8_t CompositorResourcePassScene::getMaximumRenderQueueIndex() const
	{
		return mMaximumRenderQueueIndex;
	}

	inline bool CompositorResourcePassScene::isTransparentPass() const
	{
		return mTransparentPass;
	}

	inline MaterialTechniqueId CompositorResourcePassScene::getMaterialTechniqueId() const
	{
		return mMaterialTechniqueId;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::ICompositorResourcePass methods ]
	//[-------------------------------------------------------]
	inline CompositorPassTypeId CompositorResourcePassScene::getTypeId() const
	{
		return TYPE_ID;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline CompositorResourcePassScene::CompositorResourcePassScene(const CompositorTarget& compositorTarget) :
		ICompositorResourcePass(compositorTarget),
		mMinimumRenderQueueIndex(0),
		mMaximumRenderQueueIndex(255),
		mTransparentPass(false)
	{
		// Nothing here
	}

	inline CompositorResourcePassScene::~CompositorResourcePassScene()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
