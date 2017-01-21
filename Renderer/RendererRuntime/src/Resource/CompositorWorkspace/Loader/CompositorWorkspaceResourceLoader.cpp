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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/CompositorWorkspace/Loader/CompositorWorkspaceResourceLoader.h"
#include "RendererRuntime/Resource/CompositorWorkspace/Loader/CompositorWorkspaceFileFormat.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceResource.h"
#include "RendererRuntime/Core/File/IFile.h"


// TODO(co) Possible performance improvement: Inside "CompositorWorkspaceResourceLoader::onDeserialization()" load everything directly into memory,
// (compositor hasn't much data), create the instances inside "CompositorWorkspaceResourceLoader::onProcessing()" while other stuff can continue reading
// from disk.
// TODO(co) Error handling


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void nodesDeserialization(RendererRuntime::IFile& file, RendererRuntime::CompositorWorkspaceResource& compositorWorkspaceResource)
		{
			// Read in the compositor workspace resource nodes
			RendererRuntime::v1CompositorWorkspace::Nodes nodes;
			file.read(&nodes, sizeof(RendererRuntime::v1CompositorWorkspace::Nodes));

			// Read in the compositor node asset IDs
			compositorWorkspaceResource.reserveCompositorNodes(nodes.numberOfNodes);
			// TODO(co) Get rid of the evil const-cast
			RendererRuntime::CompositorWorkspaceResource::CompositorNodeAssetIds& compositorNodeAssetIds = const_cast<RendererRuntime::CompositorWorkspaceResource::CompositorNodeAssetIds&>(compositorWorkspaceResource.getCompositorNodeAssetIds());
			compositorNodeAssetIds.resize(nodes.numberOfNodes);
			file.read(compositorNodeAssetIds.data(), sizeof(RendererRuntime::AssetId) * nodes.numberOfNodes);
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId CompositorWorkspaceResourceLoader::TYPE_ID("compositor_workspace");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void CompositorWorkspaceResourceLoader::onDeserialization(IFile& file)
	{
		// Read in the compositor workspace header
		v1CompositorWorkspace::Header compositorWorkspaceHeader;
		file.read(&compositorWorkspaceHeader, sizeof(v1CompositorWorkspace::Header));

		// Read in the compositor workspace resource nodes
		::detail::nodesDeserialization(file, *mCompositorWorkspaceResource);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
