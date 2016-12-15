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
#include "RendererToolkit/AssetCompiler/CompositorNodeAssetCompiler.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/CompositorNode/Loader/CompositorNodeFileFormat.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/Quad/CompositorResourcePassQuad.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/Clear/CompositorResourcePassClear.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/Scene/CompositorResourcePassScene.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/DebugGui/CompositorResourcePassDebugGui.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#pragma warning(disable: 4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#pragma warning(disable: 4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	#pragma warning(disable: 4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#pragma warning(disable: 4061)	// warning C4061: enumerator 'rapidjson::GenericReader<rapidjson::UTF8<char>,rapidjson::UTF8<char>,rapidjson::CrtAllocator>::IterativeParsingStartState' in switch of enum 'rapidjson::GenericReader<rapidjson::UTF8<char>,rapidjson::UTF8<char>,rapidjson::CrtAllocator>::IterativeParsingState' is not explicitly handled by a case label
	#pragma warning(disable: 4770)	// warning C4770: partially validated enum 'type' used as index"
	#include <rapidjson/document.h>
#pragma warning(pop)

#include <fstream>
#include <unordered_set>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId CompositorNodeAssetCompiler::TYPE_ID("CompositorNode");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	CompositorNodeAssetCompiler::CompositorNodeAssetCompiler()
	{
		// Nothing here
	}

	CompositorNodeAssetCompiler::~CompositorNodeAssetCompiler()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId CompositorNodeAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void CompositorNodeAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory = input.assetOutputDirectory;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read configuration
		// TODO(co) Add required properties
		std::string inputFile;
		{
			// Read compositor node asset compiler configuration
			const rapidjson::Value& rapidJsonValueCompositorNodeAssetCompiler = rapidJsonValueAsset["CompositorNodeAssetCompiler"];
			inputFile = rapidJsonValueCompositorNodeAssetCompiler["InputFile"].GetString();
		}

		// Open the input file
		const std::string inputFilename = assetInputDirectory + inputFile;
		std::ifstream inputFileStream(inputFilename, std::ios::binary);
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".compositor_node";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		{ // Compositor node
			// Parse JSON
			rapidjson::Document rapidJsonDocument;
			JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, inputFilename, "CompositorNodeAsset", "1");

			// Mandatory main sections of the compositor node
			const rapidjson::Value& rapidJsonValueCompositorNodeAsset = rapidJsonDocument["CompositorNodeAsset"];
			const rapidjson::Value& rapidJsonValueInputChannels = rapidJsonValueCompositorNodeAsset["InputChannels"];
			const rapidjson::Value& rapidJsonValueTargets = rapidJsonValueCompositorNodeAsset["Targets"];
			const rapidjson::Value& rapidJsonValueOutputChannels = rapidJsonValueCompositorNodeAsset["OutputChannels"];

			{ // Write down the compositor node resource header
				RendererRuntime::v1CompositorNode::Header compositorNodeHeader;
				compositorNodeHeader.formatType			   = RendererRuntime::v1CompositorNode::FORMAT_TYPE;
				compositorNodeHeader.formatVersion		   = RendererRuntime::v1CompositorNode::FORMAT_VERSION;
				compositorNodeHeader.numberOfInputChannels = rapidJsonValueInputChannels.MemberCount();
				compositorNodeHeader.numberOfFramebuffers  = rapidJsonValueCompositorNodeAsset.HasMember("Framebuffers") ? rapidJsonValueCompositorNodeAsset["Framebuffers"].MemberCount() : 0;
				compositorNodeHeader.numberOfTargets	   = rapidJsonValueTargets.MemberCount();
				compositorNodeHeader.numberOfOutputChannels	= rapidJsonValueOutputChannels.MemberCount();
				outputFileStream.write(reinterpret_cast<const char*>(&compositorNodeHeader), sizeof(RendererRuntime::v1CompositorNode::Header));
			}

			// Write down the compositor resource node input channels
			std::unordered_set<uint32_t> compositorChannelIds;	// "RendererRuntime::CompositorChannelId"-type
			for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorInputChannels = rapidJsonValueInputChannels.MemberBegin(); rapidJsonMemberIteratorInputChannels != rapidJsonValueInputChannels.MemberEnd(); ++rapidJsonMemberIteratorInputChannels)
			{
				RendererRuntime::v1CompositorNode::Channel channel;
				channel.id = RendererRuntime::StringId(rapidJsonMemberIteratorInputChannels->value.GetString());
				outputFileStream.write(reinterpret_cast<const char*>(&channel), sizeof(RendererRuntime::v1CompositorNode::Channel));

				// Remember that there's a compositor channel with this ID
				compositorChannelIds.insert(channel.id);
			}

			// Write down the compositor framebuffers
			std::unordered_set<uint32_t> compositorFramebufferIds;	// "RendererRuntime::CompositorFramebufferId"-type
			if (rapidJsonValueCompositorNodeAsset.HasMember("Framebuffers"))
			{
				const rapidjson::Value& rapidJsonValueFramebuffers = rapidJsonValueCompositorNodeAsset["Framebuffers"];
				for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorFramebuffers = rapidJsonValueFramebuffers.MemberBegin(); rapidJsonMemberIteratorFramebuffers != rapidJsonValueFramebuffers.MemberEnd(); ++rapidJsonMemberIteratorFramebuffers)
				{
					RendererRuntime::v1CompositorNode::Framebuffer framebuffer;
					framebuffer.compositorFramebufferId = RendererRuntime::StringId(rapidJsonMemberIteratorFramebuffers->name.GetString());
					{ // Framebuffer signature
						const rapidjson::Value& rapidJsonValueFramebuffer = rapidJsonMemberIteratorFramebuffers->value;

						// Width
						uint32_t width = RendererRuntime::getUninitialized<uint32_t>();
						const char* valueAsString = rapidJsonValueFramebuffer["Width"].GetString();
						if (strcmp(valueAsString, "TARGET_WIDTH") != 0)
						{
							width = static_cast<uint32_t>(std::atoi(valueAsString));
						}

						// Height
						uint32_t height = RendererRuntime::getUninitialized<uint32_t>();
						valueAsString = rapidJsonValueFramebuffer["Height"].GetString();
						if (strcmp(valueAsString, "TARGET_HEIGHT") != 0)
						{
							height = static_cast<uint32_t>(std::atoi(valueAsString));
						}

						// Texture format
						const Renderer::TextureFormat::Enum textureFormat = JsonHelper::mandatoryTextureFormat(rapidJsonValueFramebuffer);

						// Write framebuffer signature
						// TODO(co) Add sanity checks to be able to detect editing errors (compressed formats are not supported nor unknown formats, check for name conflicts with channels, unused framebuffers etc.)
						framebuffer.framebufferSignature = RendererRuntime::FramebufferSignature(width, height, textureFormat);
					}
					outputFileStream.write(reinterpret_cast<const char*>(&framebuffer), sizeof(RendererRuntime::v1CompositorNode::Framebuffer));

					// Remember that there's a compositor framebuffer with this ID
					compositorFramebufferIds.insert(framebuffer.compositorFramebufferId);
				}
			}

			// Write down the compositor resource node targets
			for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorTargets = rapidJsonValueTargets.MemberBegin(); rapidJsonMemberIteratorTargets != rapidJsonValueTargets.MemberEnd(); ++rapidJsonMemberIteratorTargets)
			{
				const rapidjson::Value& rapidJsonValueTarget = rapidJsonMemberIteratorTargets->value;
				const rapidjson::Value& rapidJsonValuePasses = rapidJsonValueTarget["Passes"];

				{ // Write down the compositor resource node target
				  // -> Render target might be compositor channel (external interconnection) or compositor framebuffer (node internal processing)
					const uint32_t id = RendererRuntime::StringId(rapidJsonMemberIteratorTargets->name.GetString());
					RendererRuntime::v1CompositorNode::Target target;
					target.compositorChannelId	   = (compositorChannelIds.find(id) != compositorChannelIds.end()) ? id : RendererRuntime::getUninitialized<uint32_t>();
					target.compositorFramebufferId = (compositorFramebufferIds.find(id) != compositorFramebufferIds.end()) ? id : RendererRuntime::getUninitialized<uint32_t>();
					target.numberOfPasses		   = rapidJsonValuePasses.MemberCount();
					outputFileStream.write(reinterpret_cast<const char*>(&target), sizeof(RendererRuntime::v1CompositorNode::Target));
				}

				// Write down the compositor resource node target passes
				for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorPasses = rapidJsonValuePasses.MemberBegin(); rapidJsonMemberIteratorPasses != rapidJsonValuePasses.MemberEnd(); ++rapidJsonMemberIteratorPasses)
				{
					const rapidjson::Value& rapidJsonValuePass = rapidJsonMemberIteratorPasses->value;
					const RendererRuntime::CompositorPassTypeId compositorPassTypeId = RendererRuntime::StringId(rapidJsonMemberIteratorPasses->name.GetString());

					// Get the compositor resource node target pass type specific data number of bytes
					// TODO(co) Make this more generic via compositor pass factory
					uint32_t numberOfBytes = 0;
					if (RendererRuntime::CompositorResourcePassClear::TYPE_ID == compositorPassTypeId)
					{
						numberOfBytes = sizeof(RendererRuntime::v1CompositorNode::PassClear);
					}
					else if (RendererRuntime::CompositorResourcePassQuad::TYPE_ID == compositorPassTypeId)
					{
						numberOfBytes = sizeof(RendererRuntime::v1CompositorNode::PassQuad);
					}
					else if (RendererRuntime::CompositorResourcePassScene::TYPE_ID == compositorPassTypeId)
					{
						numberOfBytes = sizeof(RendererRuntime::v1CompositorNode::PassScene);
					}
					else if (RendererRuntime::CompositorResourcePassDebugGui::TYPE_ID == compositorPassTypeId)
					{
						numberOfBytes = sizeof(RendererRuntime::v1CompositorNode::PassDebugGui);
					}

					{ // Write down the compositor resource node target pass header
						RendererRuntime::v1CompositorNode::PassHeader passHeader;
						passHeader.compositorPassTypeId = compositorPassTypeId;
						passHeader.numberOfBytes		= numberOfBytes;
						outputFileStream.write(reinterpret_cast<const char*>(&passHeader), sizeof(RendererRuntime::v1CompositorNode::PassHeader));
					}

					// Write down the compositor resource node target pass type specific data, if there is any
					if (0 != numberOfBytes)
					{
						if (RendererRuntime::CompositorResourcePassClear::TYPE_ID == compositorPassTypeId)
						{
							RendererRuntime::v1CompositorNode::PassClear passClear;
							JsonHelper::optionalFloatNProperty(rapidJsonValuePass, "Color", passClear.color, 4);
							outputFileStream.write(reinterpret_cast<const char*>(&passClear), sizeof(RendererRuntime::v1CompositorNode::PassClear));
						}
						else if (RendererRuntime::CompositorResourcePassQuad::TYPE_ID == compositorPassTypeId)
						{
							RendererRuntime::v1CompositorNode::PassQuad passQuad;
							outputFileStream.write(reinterpret_cast<const char*>(&passQuad), sizeof(RendererRuntime::v1CompositorNode::PassQuad));
						}
						else if (RendererRuntime::CompositorResourcePassScene::TYPE_ID == compositorPassTypeId)
						{
							RendererRuntime::v1CompositorNode::PassScene passScene;

							// Read properties
							JsonHelper::optionalByteProperty(rapidJsonValuePass, "MinimumRenderQueueIndex", passScene.minimumRenderQueueIndex);
							JsonHelper::optionalByteProperty(rapidJsonValuePass, "MaximumRenderQueueIndex", passScene.maximumRenderQueueIndex);
							JsonHelper::optionalBooleanProperty(rapidJsonValuePass, "TransparentPass", passScene.transparentPass);

							// Sanity check
							if (passScene.maximumRenderQueueIndex < passScene.minimumRenderQueueIndex)
							{
								throw std::runtime_error("The maximum render queue index must be equal or greater as the minimum render queue index");
							}

							// Write down
							outputFileStream.write(reinterpret_cast<const char*>(&passScene), sizeof(RendererRuntime::v1CompositorNode::PassScene));
						}
						else if (RendererRuntime::CompositorResourcePassDebugGui::TYPE_ID == compositorPassTypeId)
						{
							RendererRuntime::v1CompositorNode::PassDebugGui passDebugGui;
							outputFileStream.write(reinterpret_cast<const char*>(&passDebugGui), sizeof(RendererRuntime::v1CompositorNode::PassDebugGui));
						}
					}
				}
			}

			// Write down the compositor resource node output channels
			for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorOutputChannels = rapidJsonValueOutputChannels.MemberBegin(); rapidJsonMemberIteratorOutputChannels != rapidJsonValueOutputChannels.MemberEnd(); ++rapidJsonMemberIteratorOutputChannels)
			{
				RendererRuntime::v1CompositorNode::Channel channel;
				channel.id = RendererRuntime::StringId(rapidJsonMemberIteratorOutputChannels->name.GetString());
				outputFileStream.write(reinterpret_cast<const char*>(&channel), sizeof(RendererRuntime::v1CompositorNode::Channel));
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/CompositorNode/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::Asset outputAsset;
			outputAsset.assetId = RendererRuntime::StringId(assetIdAsString.c_str());
			strcpy(outputAsset.assetFilename, outputAssetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit