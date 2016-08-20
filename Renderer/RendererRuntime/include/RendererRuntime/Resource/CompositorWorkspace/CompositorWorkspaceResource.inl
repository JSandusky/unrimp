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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline void CompositorWorkspaceResource::reserveCompositorNodes(uint32_t numberOfCompositorNodes)
	{
		mCompositorNodeAssetIds.reserve(numberOfCompositorNodes);
	}

	inline void CompositorWorkspaceResource::addCompositorNode(AssetId compositorNodeAssetId)
	{
		mCompositorNodeAssetIds.push_back(compositorNodeAssetId);
	}

	inline const CompositorWorkspaceResource::CompositorNodeAssetIds& CompositorWorkspaceResource::getCompositorNodeAssetIds() const
	{
		return mCompositorNodeAssetIds;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline CompositorWorkspaceResource::CompositorWorkspaceResource()
	{
		// Nothing here
	}

	inline CompositorWorkspaceResource::~CompositorWorkspaceResource()
	{
		// Sanity checks
		assert(mCompositorNodeAssetIds.empty());
	}

	inline void CompositorWorkspaceResource::initializeElement(CompositorWorkspaceResourceId compositorWorkspaceResourceId)
	{
		// Sanity checks
		assert(mCompositorNodeAssetIds.empty());

		// Call base implementation
		IResource::initializeElement(compositorWorkspaceResourceId);
	}

	inline void CompositorWorkspaceResource::deinitializeElement()
	{
		mCompositorNodeAssetIds.clear();

		// Call base implementation
		IResource::deinitializeElement();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
