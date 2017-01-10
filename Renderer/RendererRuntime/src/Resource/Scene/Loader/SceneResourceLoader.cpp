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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneResourceLoader.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h"
#include "RendererRuntime/Resource/Scene/Item/ISceneItem.h"
#include "RendererRuntime/Resource/Scene/ISceneResource.h"

#include <fstream>


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
		void itemDeserialization(std::istream& inputStream, RendererRuntime::ISceneResource& sceneResource, RendererRuntime::ISceneNode& sceneNode)
		{
			// Read the scene item header
			RendererRuntime::v1Scene::ItemHeader itemHeader;
			inputStream.read(reinterpret_cast<char*>(&itemHeader), sizeof(RendererRuntime::v1Scene::ItemHeader));

			// Create the scene item
			RendererRuntime::ISceneItem* sceneItem = sceneResource.createSceneItem(itemHeader.typeId, sceneNode);
			if (nullptr != sceneItem && 0 != itemHeader.numberOfBytes)
			{
				// Load in the scene item data
				// TODO(co) Get rid of the new/delete in here
				uint8_t* data = new uint8_t[itemHeader.numberOfBytes];
				inputStream.read(reinterpret_cast<char*>(data), itemHeader.numberOfBytes);

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

		void nodeDeserialization(std::istream& inputStream, RendererRuntime::ISceneResource& sceneResource)
		{
			// Read in the scene node
			RendererRuntime::v1Scene::Node node;
			inputStream.read(reinterpret_cast<char*>(&node), sizeof(RendererRuntime::v1Scene::Node));

			// Create the scene node
			RendererRuntime::ISceneNode* sceneNode = sceneResource.createSceneNode(node.transform);
			if (nullptr != sceneNode)
			{
				// Read in the scene items
				for (uint32_t i = 0; i < node.numberOfItems; ++i)
				{
					itemDeserialization(inputStream, sceneResource, *sceneNode);
				}
			}
			else
			{
				// TODO(co) Error handling
			}
		}

		void nodesDeserialization(std::istream& inputStream, RendererRuntime::ISceneResource& sceneResource)
		{
			// Read in the scene nodes
			RendererRuntime::v1Scene::Nodes nodes;
			inputStream.read(reinterpret_cast<char*>(&nodes), sizeof(RendererRuntime::v1Scene::Nodes));

			// Read in the scene nodes
			for (uint32_t i = 0; i < nodes.numberOfNodes; ++i)
			{
				nodeDeserialization(inputStream, sceneResource);
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
	void SceneResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream inputFileStream(mAsset.assetFilename, std::ios::binary);
			if (!inputFileStream)
			{
				// This error handling shouldn't be there since everything the asset package says exists
				// must exist, else it's as fatal as "new" returning a null pointer due to out-of-memory.
				throw std::runtime_error("Could not open file \"" + std::string(mAsset.assetFilename) + '\"');
			}

			// Read in the scene header
			v1Scene::Header sceneHeader;
			inputFileStream.read(reinterpret_cast<char*>(&sceneHeader), sizeof(v1Scene::Header));

			// Read in the scene resource nodes
			::detail::nodesDeserialization(inputFileStream, *mSceneResource);
		}
		catch (const std::exception& e)
		{
			// TODO(sw) the getId is needed because clang3.9/gcc 4.9 cannot determine to use the uint32_t conversion operator on it when passed to a printf method: error: cannot pass non-trivial object of type 'AssetId' (aka 'RendererRuntime::StringId') to variadic function; expected type from format string was 'int' [-Wnon-pod-varargs]
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load scene asset %u: %s", mAsset.assetId.getId(), e.what());
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
