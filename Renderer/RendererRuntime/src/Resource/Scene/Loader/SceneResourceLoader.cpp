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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneResourceLoader.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h"
#include "RendererRuntime/Resource/Scene/Item/ISceneItem.h"
#include "RendererRuntime/Resource/Scene/ISceneResource.h"
#include "RendererRuntime/Core/File/IFile.h"


// TODO(co) Possible performance improvement: Inside "SceneResourceLoader::onDeserialization()" load everything directly into memory,
// create the instances inside "SceneResourceLoader::onProcessing()" while other stuff can continue reading from disk.
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
		void itemDeserialization(RendererRuntime::IFile& file, RendererRuntime::ISceneResource& sceneResource, RendererRuntime::ISceneNode& sceneNode)
		{
			// Read the scene item header
			RendererRuntime::v1Scene::ItemHeader itemHeader;
			file.read(&itemHeader, sizeof(RendererRuntime::v1Scene::ItemHeader));

			// Create the scene item
			RendererRuntime::ISceneItem* sceneItem = sceneResource.createSceneItem(itemHeader.typeId, sceneNode);
			if (nullptr != sceneItem && 0 != itemHeader.numberOfBytes)
			{
				// Load in the scene item data
				// TODO(co) Get rid of the new/delete in here
				uint8_t* data = new uint8_t[itemHeader.numberOfBytes];
				file.read(data, itemHeader.numberOfBytes);

				// Deserialize the scene item
				sceneItem->deserialize(itemHeader.numberOfBytes, data);

				// Cleanup
				delete [] data;
			}
			else
			{
				// TODO(co) Error handling
			}
		}

		void nodeDeserialization(RendererRuntime::IFile& file, RendererRuntime::ISceneResource& sceneResource)
		{
			// Read in the scene node
			RendererRuntime::v1Scene::Node node;
			file.read(&node, sizeof(RendererRuntime::v1Scene::Node));

			// Create the scene node
			RendererRuntime::ISceneNode* sceneNode = sceneResource.createSceneNode(node.transform);
			if (nullptr != sceneNode)
			{
				// Read in the scene items
				for (uint32_t i = 0; i < node.numberOfItems; ++i)
				{
					itemDeserialization(file, sceneResource, *sceneNode);
				}
			}
			else
			{
				// TODO(co) Error handling
			}
		}

		void nodesDeserialization(RendererRuntime::IFile& file, RendererRuntime::ISceneResource& sceneResource)
		{
			// Read in the scene nodes
			RendererRuntime::v1Scene::Nodes nodes;
			file.read(&nodes, sizeof(RendererRuntime::v1Scene::Nodes));

			// Read in the scene nodes
			for (uint32_t i = 0; i < nodes.numberOfNodes; ++i)
			{
				nodeDeserialization(file, sceneResource);
			}
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
	const ResourceLoaderTypeId SceneResourceLoader::TYPE_ID("scene");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void SceneResourceLoader::onDeserialization(IFile& file)
	{
		// Read in the scene header
		v1Scene::Header sceneHeader;
		file.read(&sceneHeader, sizeof(v1Scene::Header));
		assert(v1Scene::FORMAT_TYPE == sceneHeader.formatType);
		assert(v1Scene::FORMAT_VERSION == sceneHeader.formatVersion);

		// Read in the scene resource nodes
		::detail::nodesDeserialization(file, *mSceneResource);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
