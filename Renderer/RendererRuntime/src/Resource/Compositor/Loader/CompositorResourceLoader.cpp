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
#include "RendererRuntime/Resource/Compositor/Loader/CompositorResourceLoader.h"
#include "RendererRuntime/Resource/Compositor/Loader/CompositorFileFormat.h"
#include "RendererRuntime/Resource/Compositor/Pass/ICompositorResourcePass.h"
#include "RendererRuntime/Resource/Compositor/CompositorResource.h"
#include "RendererRuntime/Resource/Compositor/CompositorResourceNode.h"
#include "RendererRuntime/Resource/Compositor/CompositorResourceManager.h"

#include <fstream>


// TODO(co) Possible performance improvement: Inside "CompositorResourceLoader::onDeserialization()" load everything directly into memory,
// (compositor hasn't much data), create the instances inside "CompositorResourceLoader::onProcessing()" while other stuff can continue reading
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
		void nodeTargetDeserialization(std::istream& inputStream, RendererRuntime::CompositorResourceNode& compositorResourceNode, const RendererRuntime::ICompositorPassFactory& compositorPassFactory)
		{
			// Read in the compositor resource target
			RendererRuntime::v1Compositor::Target target;
			inputStream.read(reinterpret_cast<char*>(&target), sizeof(RendererRuntime::v1Compositor::Target));

			// Create the compositor resource target instance
			RendererRuntime::CompositorResourceTarget& compositorResourceTarget = compositorResourceNode.addCompositorResourceTarget(target.channelId);

			// Read in the compositor resource node target passes
			compositorResourceTarget.setNumberOfCompositorResourcePasses(target.numberOfPasses);
			for (uint32_t i = 0; i < target.numberOfPasses; ++i)
			{
				// Read the pass header
				RendererRuntime::v1Compositor::PassHeader passHeader;
				inputStream.read(reinterpret_cast<char*>(&passHeader), sizeof(RendererRuntime::v1Compositor::PassHeader));

				// Create the compositor resource pass
				RendererRuntime::ICompositorResourcePass* compositorResourcePass = compositorResourceTarget.addCompositorResourcePass(compositorPassFactory, passHeader.typeId);

				// Deserialize the compositor resource pass
				if (nullptr != compositorResourcePass && 0 != passHeader.numberOfBytes)
				{
					// Load in the compositor resource pass data
					// TODO(co) Get rid of the new/delete in here
					uint8_t* data = new uint8_t[passHeader.numberOfBytes];
					inputStream.read(reinterpret_cast<char*>(data), passHeader.numberOfBytes);

					// Deserialize the compositor resource pass
					compositorResourcePass->deserialize(passHeader.numberOfBytes, data);

					// Cleanup
					delete [] data;
				}
				else
				{
					// TODO(co) Error handling
				}
			}
		}

		void nodeDeserialization(std::istream& inputStream, RendererRuntime::CompositorResource& compositorResource, const RendererRuntime::ICompositorPassFactory& compositorPassFactory)
		{
			// Read in the compositor resource node
			RendererRuntime::v1Compositor::Node node;
			inputStream.read(reinterpret_cast<char*>(&node), sizeof(RendererRuntime::v1Compositor::Node));

			// Create the compositor resource node instance
			// TODO(co) Handle already existing compositor resource nodes
			RendererRuntime::CompositorResourceNode* compositorResourceNode = compositorResource.addCompositorResourceNode(node.id);

			// Read in the compositor resource node input channels
			// TODO(co) Read all input channels in a single burst? (need to introduce a maximum number of input channels for this)
			compositorResourceNode->setNumberOfInputChannels(node.numberOfInputChannels);
			for (uint32_t i = 0; i < node.numberOfInputChannels; ++i)
			{
				RendererRuntime::CompositorChannelId channelId;
				inputStream.read(reinterpret_cast<char*>(&channelId), sizeof(RendererRuntime::CompositorChannelId));
				compositorResourceNode->addInputChannel(channelId);
			}

			// Read in the compositor resource node targets
			compositorResourceNode->setNumberOfCompositorResourceTargets(node.numberOfTargets);
			for (uint32_t i = 0; i < node.numberOfTargets; ++i)
			{
				nodeTargetDeserialization(inputStream, *compositorResourceNode, compositorPassFactory);
			}

			// Read in the compositor resource node output channels
			// TODO(co) Read all output channels in a single burst? (need to introduce a maximum number of output channels for this)
			compositorResourceNode->setNumberOfOutputChannels(node.numberOfOutputChannels);
			for (uint32_t i = 0; i < node.numberOfOutputChannels; ++i)
			{
				RendererRuntime::CompositorChannelId channelId;
				inputStream.read(reinterpret_cast<char*>(&channelId), sizeof(RendererRuntime::CompositorChannelId));
				compositorResourceNode->addOutputChannel(channelId);
			}
		}

		void nodesDeserialization(std::istream& inputStream, RendererRuntime::CompositorResource& compositorResource, const RendererRuntime::ICompositorPassFactory& compositorPassFactory)
		{
			// Read in the compositor resource nodes
			RendererRuntime::v1Compositor::Nodes nodes;
			inputStream.read(reinterpret_cast<char*>(&nodes), sizeof(RendererRuntime::v1Compositor::Nodes));

			// Read in the compositor resource nodes
			compositorResource.setNumberOfCompositorResourceNodes(nodes.numberOfNodes);
			for (uint32_t i = 0; i < nodes.numberOfNodes; ++i)
			{
				nodeDeserialization(inputStream, compositorResource, compositorPassFactory);
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
	const ResourceLoaderTypeId CompositorResourceLoader::TYPE_ID("compositor");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	ResourceLoaderTypeId CompositorResourceLoader::getResourceLoaderTypeId() const
	{
		return TYPE_ID;
	}

	void CompositorResourceLoader::onDeserialization()
	{
		const ICompositorPassFactory& compositorPassFactory = static_cast<CompositorResourceManager&>(getResourceManager()).getCompositorPassFactory();
		try
		{
			std::ifstream inputFileStream(mAsset.assetFilename, std::ios::binary);

			// Read in the compositor header
			v1Compositor::Header compositorHeader;
			inputFileStream.read(reinterpret_cast<char*>(&compositorHeader), sizeof(v1Compositor::Header));

			// Read in the compositor resource nodes
			::detail::nodesDeserialization(inputFileStream, *mCompositorResource, compositorPassFactory);
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load compositor asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void CompositorResourceLoader::onProcessing()
	{
		// Nothing here
	}

	void CompositorResourceLoader::onRendererBackendDispatch()
	{
		// Nothing here

		// TODO(co) Create compositor textures in here?
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	CompositorResourceLoader::~CompositorResourceLoader()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
