/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererToolkit/AssetCompiler/CompositorAssetCompiler.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/Compositor/Loader/CompositorFileFormat.h>
#include <RendererRuntime/Resource/Compositor/Pass/Quad/CompositorResourcePassQuad.h>
#include <RendererRuntime/Resource/Compositor/Pass/Clear/CompositorResourcePassClear.h>
#include <RendererRuntime/Resource/Compositor/Pass/Scene/CompositorResourcePassScene.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4251)	// warning C4251: 'Poco::StringTokenizer::_tokens': class 'std::vector<std::string,std::allocator<_Kty>>' needs to have dll-interface to be used by clients of class 'Poco::StringTokenizer'
	#include <Poco/StringTokenizer.h>
#pragma warning(pop)

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId CompositorAssetCompiler::TYPE_ID("Compositor");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	CompositorAssetCompiler::CompositorAssetCompiler()
	{
	}

	CompositorAssetCompiler::~CompositorAssetCompiler()
	{
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId CompositorAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void CompositorAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory	= input.assetOutputDirectory;
		Poco::JSON::Object::Ptr		   jsonAssetRootObject	= configuration.jsonAssetRootObject;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		Poco::JSON::Object::Ptr jsonAssetObject = jsonAssetRootObject->get("Asset").extract<Poco::JSON::Object::Ptr>();

		// Read configuration
		// TODO(co) Add required properties
		std::string inputFile;
		uint32_t test = 0;
		{
			// Read Compositor asset compiler configuration
			Poco::JSON::Object::Ptr jsonConfigurationObject = jsonAssetObject->get("CompositorAssetCompiler").extract<Poco::JSON::Object::Ptr>();
			inputFile = jsonConfigurationObject->getValue<std::string>("InputFile");
			test	  = jsonConfigurationObject->optValue<uint32_t>("Test", test);
		}

		// Open the input file
		std::ifstream inputFileStream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetName");
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".compositor";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		{ // Compositor
			// Parse JSON
			Poco::JSON::Parser jsonParser;
			jsonParser.parse(inputFileStream);
			Poco::JSON::Object::Ptr jsonRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();
		
			{ // Check whether or not the file format matches
				Poco::JSON::Object::Ptr jsonFormatObject = jsonRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
				if (jsonFormatObject->get("Type").convert<std::string>() != "CompositorAsset")
				{
					throw std::exception("Invalid JSON format type, must be \"CompositorAsset\"");
				}
				if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
				{
					throw std::exception("Invalid JSON format version, must be 1");
				}
			}

			{ // Write down the compositor resource header
				RendererRuntime::v1Compositor::Header compositorHeader;
				compositorHeader.formatType	   = RendererRuntime::v1Compositor::FORMAT_TYPE;
				compositorHeader.formatVersion = RendererRuntime::v1Compositor::FORMAT_VERSION;
				outputFileStream.write(reinterpret_cast<const char*>(&compositorHeader), sizeof(RendererRuntime::v1Compositor::Header));
			}

			Poco::JSON::Object::Ptr jsonCompositorObject = jsonRootObject->get("CompositorAsset").extract<Poco::JSON::Object::Ptr>();
			Poco::JSON::Object::Ptr jsonCompositorNodesObject = jsonCompositorObject->get("Nodes").extract<Poco::JSON::Object::Ptr>();

			{ // Write down the compositor resource nodes
				RendererRuntime::v1Compositor::Nodes nodes;
				nodes.numberOfNodes = jsonCompositorNodesObject->size();
				outputFileStream.write(reinterpret_cast<const char*>(&nodes), sizeof(RendererRuntime::v1Compositor::Nodes));

				// Loop through all compositor resource nodes
				Poco::JSON::Object::ConstIterator nodesIterator = jsonCompositorNodesObject->begin();
				Poco::JSON::Object::ConstIterator nodesIteratorEnd = jsonCompositorNodesObject->end();
				while (nodesIterator != nodesIteratorEnd)
				{
					Poco::JSON::Object::Ptr jsonCompositorNodeObject = nodesIterator->second.extract<Poco::JSON::Object::Ptr>();
					Poco::JSON::Object::Ptr jsonInputChannelsObject = jsonCompositorNodeObject->get("InputChannels").extract<Poco::JSON::Object::Ptr>();
					Poco::JSON::Object::Ptr jsonTargetsObject = jsonCompositorNodeObject->get("Targets").extract<Poco::JSON::Object::Ptr>();
					Poco::JSON::Object::Ptr jsonOutputChannelsObject = jsonCompositorNodeObject->get("OutputChannels").extract<Poco::JSON::Object::Ptr>();

					{ // Write down the compositor resource node
						RendererRuntime::v1Compositor::Node node;
						node.id						= RendererRuntime::StringId(nodesIterator->first.c_str());
						node.numberOfInputChannels	= jsonInputChannelsObject->size();
						node.numberOfTargets		= jsonTargetsObject->size();
						node.numberOfOutputChannels	= jsonOutputChannelsObject->size();
						outputFileStream.write(reinterpret_cast<const char*>(&node), sizeof(RendererRuntime::v1Compositor::Node));
					}

					{ // Write down the compositor resource node input channels
						Poco::JSON::Object::ConstIterator channelsIterator = jsonInputChannelsObject->begin();
						Poco::JSON::Object::ConstIterator channelsIteratorEnd = jsonInputChannelsObject->end();
						while (channelsIterator != channelsIteratorEnd)
						{
							RendererRuntime::v1Compositor::Channel channel;
							channel.id = RendererRuntime::StringId(channelsIterator->second.convert<std::string>().c_str());
							outputFileStream.write(reinterpret_cast<const char*>(&channel), sizeof(RendererRuntime::v1Compositor::Channel));

							// Next compositor resource node input channel, please
							++channelsIterator;
						}
					}

					{ // Write down the compositor resource node targets
						Poco::JSON::Object::ConstIterator targetsIterator = jsonTargetsObject->begin();
						Poco::JSON::Object::ConstIterator targetsIteratorEnd = jsonTargetsObject->end();
						while (targetsIterator != targetsIteratorEnd)
						{
							Poco::JSON::Object::Ptr jsonCompositorTargetObject = targetsIterator->second.extract<Poco::JSON::Object::Ptr>();
							Poco::JSON::Object::Ptr jsonPassesObject = jsonCompositorTargetObject->get("Passes").extract<Poco::JSON::Object::Ptr>();

							{ // Write down the compositor resource node target
								RendererRuntime::v1Compositor::Target target;
								target.channelId	  = RendererRuntime::StringId(targetsIterator->first.c_str());
								target.numberOfPasses = jsonPassesObject->size();
								outputFileStream.write(reinterpret_cast<const char*>(&target), sizeof(RendererRuntime::v1Compositor::Target));
							}

							{ // Write down the compositor resource node target passes
								Poco::JSON::Object::ConstIterator passesIterator = jsonPassesObject->begin();
								Poco::JSON::Object::ConstIterator passesIteratorEnd = jsonPassesObject->end();
								while (passesIterator != passesIteratorEnd)
								{
									Poco::JSON::Object::Ptr jsonPassObject = passesIterator->second.extract<Poco::JSON::Object::Ptr>();
									const RendererRuntime::CompositorPassTypeId typeId = RendererRuntime::StringId(passesIterator->first.c_str());

									// Get the compositor resource node target pass type specific data number of bytes
									// TODO(co) Make this more generic via compositor pass factory
									uint32_t numberOfBytes = 0;
									if (RendererRuntime::CompositorResourcePassClear::TYPE_ID == typeId)
									{
										numberOfBytes = sizeof(RendererRuntime::v1Compositor::PassClear);
									}
									else if (RendererRuntime::CompositorResourcePassQuad::TYPE_ID == typeId)
									{
										numberOfBytes = sizeof(RendererRuntime::v1Compositor::PassQuad);
									}
									else if (RendererRuntime::CompositorResourcePassScene::TYPE_ID == typeId)
									{
										numberOfBytes = sizeof(RendererRuntime::v1Compositor::PassScene);
									}

									{ // Write down the compositor resource node target pass header
										RendererRuntime::v1Compositor::PassHeader passHeader;
										passHeader.typeId		 = typeId;
										passHeader.numberOfBytes = numberOfBytes;
										outputFileStream.write(reinterpret_cast<const char*>(&passHeader), sizeof(RendererRuntime::v1Compositor::PassHeader));
									}

									// Write down the compositor resource node target pass type specific data, if there is any
									if (0 != numberOfBytes)
									{
										if (RendererRuntime::CompositorResourcePassClear::TYPE_ID == typeId)
										{
											RendererRuntime::v1Compositor::PassClear passClear;

											// Parse the color string
											Poco::StringTokenizer stringTokenizer(jsonPassObject->get("Color").convert<std::string>(), " ");
											if (stringTokenizer.count() == 4)
											{
												for (size_t i = 0; i < 4; ++i)
												{
													passClear.color[i] = std::stof(stringTokenizer[i].c_str());
												}
											}
											else
											{
												// TODO(co) Error handling
											}

											outputFileStream.write(reinterpret_cast<const char*>(&passClear), sizeof(RendererRuntime::v1Compositor::PassClear));
										}
										else if (RendererRuntime::CompositorResourcePassQuad::TYPE_ID == typeId)
										{
											RendererRuntime::v1Compositor::PassQuad passQuad;
											outputFileStream.write(reinterpret_cast<const char*>(&passQuad), sizeof(RendererRuntime::v1Compositor::PassQuad));
										}
										else if (RendererRuntime::CompositorResourcePassScene::TYPE_ID == typeId)
										{
											RendererRuntime::v1Compositor::PassScene passScene;
											outputFileStream.write(reinterpret_cast<const char*>(&passScene), sizeof(RendererRuntime::v1Compositor::PassScene));
										}
									}

									// Next compositor resource node target pass, please
									++passesIterator;
								}
							}

							// Next compositor resource node target, please
							++targetsIterator;
						}
					}

					{ // Write down the compositor resource node output channels
						Poco::JSON::Object::ConstIterator channelsIterator = jsonOutputChannelsObject->begin();
						Poco::JSON::Object::ConstIterator channelsIteratorEnd = jsonOutputChannelsObject->end();
						while (channelsIterator != channelsIteratorEnd)
						{
							RendererRuntime::v1Compositor::Channel channel;
							channel.id = RendererRuntime::StringId(channelsIterator->second.convert<std::string>().c_str());
							outputFileStream.write(reinterpret_cast<const char*>(&channel), sizeof(RendererRuntime::v1Compositor::Channel));

							// Next compositor resource node output channel, please
							++channelsIterator;
						}
					}

					// Next compositor resource node, please
					++nodesIterator;
				}
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetCategory");
			const std::string assetIdAsString = input.projectName + "/Compositor/" + assetCategory + '/' + assetName;

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
