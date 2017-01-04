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
#include "RendererToolkit/AssetCompiler/SceneAssetCompiler.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/Scene/Item/MeshSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/LightSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/CameraSceneItem.h>
#include <RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	#include <glm/gtc/epsilon.hpp>
PRAGMA_WARNING_POP

#include <string>
#include <fstream>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Functions                                             ]
		//[-------------------------------------------------------]
		void optionalLightTypeProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, RendererRuntime::LightSceneItem::LightType& value)
		{
			if (rapidJsonValue.HasMember(propertyName))
			{
				const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
				const char* valueAsString = rapidJsonValueValueType.GetString();
				const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

				// Define helper macros
				#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = RendererRuntime::LightSceneItem::LightType::name;
				#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = RendererRuntime::LightSceneItem::LightType::name;

				// Evaluate value
				IF_VALUE(DIRECTIONAL)
				ELSE_IF_VALUE(POINT)
				ELSE_IF_VALUE(SPOT)
				else
				{
					throw std::runtime_error("Light type \"" + std::string(valueAsString) + "\" is unknown. The light type must be one of the following constants: DIRECTIONAL, POINT or SPOT");
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
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
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId SceneAssetCompiler::TYPE_ID("Scene");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SceneAssetCompiler::SceneAssetCompiler()
	{
		// Nothing here
	}

	SceneAssetCompiler::~SceneAssetCompiler()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId SceneAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void SceneAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory	= input.assetOutputDirectory;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read configuration
		// TODO(co) Add required properties
		std::string inputFile;
		{
			// Read scene asset compiler configuration
			const rapidjson::Value& rapidJsonValueSceneAssetCompiler = rapidJsonValueAsset["SceneAssetCompiler"];
			inputFile = rapidJsonValueSceneAssetCompiler["InputFile"].GetString();
		}

		// Open the input file
		const std::string inputFilename = assetInputDirectory + inputFile;
		std::ifstream inputFileStream(inputFilename, std::ios::binary);
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".scene";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		{ // Scene
			// Parse JSON
			rapidjson::Document rapidJsonDocument;
			JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, inputFilename, "SceneAsset", "1");

			{ // Write down the scene resource header
				RendererRuntime::v1Scene::Header sceneHeader;
				sceneHeader.formatType	  = RendererRuntime::v1Scene::FORMAT_TYPE;
				sceneHeader.formatVersion = RendererRuntime::v1Scene::FORMAT_VERSION;
				outputFileStream.write(reinterpret_cast<const char*>(&sceneHeader), sizeof(RendererRuntime::v1Scene::Header));
			}

			// Mandatory main sections of the material blueprint
			const rapidjson::Value& rapidJsonValueSceneAsset = rapidJsonDocument["SceneAsset"];
			const rapidjson::Value& rapidJsonValueNodes = rapidJsonValueSceneAsset["Nodes"];

			{ // Write down the scene nodes
				RendererRuntime::v1Scene::Nodes nodes;
				nodes.numberOfNodes = rapidJsonValueNodes.MemberCount();
				outputFileStream.write(reinterpret_cast<const char*>(&nodes), sizeof(RendererRuntime::v1Scene::Nodes));

				// Loop through all scene nodes
				for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorNodes = rapidJsonValueNodes.MemberBegin(); rapidJsonMemberIteratorNodes != rapidJsonValueNodes.MemberEnd(); ++rapidJsonMemberIteratorNodes)
				{
					const rapidjson::Value& rapidJsonValueNode = rapidJsonMemberIteratorNodes->value;
					const rapidjson::Value* rapidJsonValueItems = rapidJsonValueNode.HasMember("Items") ? &rapidJsonValueNode["Items"] : nullptr;

					{ // Write down the scene node
						RendererRuntime::v1Scene::Node node;

						// Get the scene node transform
						node.transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
						if (rapidJsonValueNode.HasMember("Properties"))
						{
							const rapidjson::Value& rapidJsonValueProperties = rapidJsonValueNode["Properties"];

							// Position, rotation and scale
							JsonHelper::optionalFloatNProperty(rapidJsonValueProperties, "Position", &node.transform.position.x, 3);
							JsonHelper::optionalFloatNProperty(rapidJsonValueProperties, "Rotation", &node.transform.rotation.x, 4);	// Rotation quaternion
							JsonHelper::optionalFloatNProperty(rapidJsonValueProperties, "Scale", &node.transform.scale.x, 3);
						}

						{ // Sanity check
							const float length = glm::length(node.transform.rotation);
							if (!glm::epsilonEqual(length, 1.0f, 0.0000001f))
							{
								throw std::runtime_error("The rotation quaternion of scene node \"" + std::string(rapidJsonMemberIteratorNodes->name.GetString()) + "\" does not appear to be normalized (length is " + std::to_string(length) + ")");
							}
						}

						// Write down the scene node
						node.numberOfItems = (nullptr != rapidJsonValueItems) ? rapidJsonValueItems->MemberCount() : 0;
						outputFileStream.write(reinterpret_cast<const char*>(&node), sizeof(RendererRuntime::v1Scene::Node));
					}

					// Write down the scene items
					if (nullptr != rapidJsonValueItems)
					{
						for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorItems = rapidJsonValueItems->MemberBegin(); rapidJsonMemberIteratorItems != rapidJsonValueItems->MemberEnd(); ++rapidJsonMemberIteratorItems)
						{
							const rapidjson::Value& rapidJsonValueItem = rapidJsonMemberIteratorItems->value;
							const RendererRuntime::SceneItemTypeId typeId = RendererRuntime::StringId(rapidJsonMemberIteratorItems->name.GetString());

							// Get the scene item type specific data number of bytes
							// TODO(co) Make this more generic via scene factory
							uint32_t numberOfBytes = 0;
							if (RendererRuntime::CameraSceneItem::TYPE_ID == typeId)
							{
								// Nothing here
							}
							else if (RendererRuntime::LightSceneItem::TYPE_ID == typeId)
							{
								numberOfBytes = sizeof(RendererRuntime::v1Scene::LightItem);
							}
							else if (RendererRuntime::MeshSceneItem::TYPE_ID == typeId)
							{
								numberOfBytes = sizeof(RendererRuntime::v1Scene::MeshItem);
							}

							{ // Write down the scene item header
								RendererRuntime::v1Scene::ItemHeader itemHeader;
								itemHeader.typeId		 = typeId;
								itemHeader.numberOfBytes = numberOfBytes;
								outputFileStream.write(reinterpret_cast<const char*>(&itemHeader), sizeof(RendererRuntime::v1Scene::ItemHeader));
							}

							// Write down the scene item type specific data, if there is any
							if (0 != numberOfBytes)
							{
								if (RendererRuntime::CameraSceneItem::TYPE_ID == typeId)
								{
									// Nothing here
								}
								else if (RendererRuntime::LightSceneItem::TYPE_ID == typeId)
								{
									RendererRuntime::v1Scene::LightItem lightItem;

									// Read properties
									::detail::optionalLightTypeProperty(rapidJsonValueItem, "LightType", lightItem.lightType);
									JsonHelper::optionalFloatNProperty(rapidJsonValueItem, "Color", lightItem.color, 3);
									JsonHelper::optionalFloatProperty(rapidJsonValueItem, "Radius", lightItem.radius);

									// Sanity checks
									if (lightItem.color[0] < 0.0f || lightItem.color[1] < 0.0f || lightItem.color[2] < 0.0f)
									{
										throw std::runtime_error("All light item color components must be positive");
									}
									if (lightItem.lightType != RendererRuntime::LightSceneItem::LightType::DIRECTIONAL && lightItem.radius <= 0.0f)
									{
										throw std::runtime_error("For point or spot light items the radius must be greater as zero");
									}
									if (lightItem.lightType == RendererRuntime::LightSceneItem::LightType::DIRECTIONAL && lightItem.radius != 0.0f)
									{
										throw std::runtime_error("For directional light items the radius must be zero");
									}

									// Write down
									outputFileStream.write(reinterpret_cast<const char*>(&lightItem), sizeof(RendererRuntime::v1Scene::LightItem));
								}
								else if (RendererRuntime::MeshSceneItem::TYPE_ID == typeId)
								{
									// Map the source asset ID to the compiled asset ID
									const RendererRuntime::AssetId compiledAssetId = JsonHelper::getCompiledAssetId(input, rapidJsonValueItem, "MeshAssetId");

									// Write the mesh item data
									RendererRuntime::v1Scene::MeshItem meshItem;
									meshItem.meshAssetId = compiledAssetId;
									outputFileStream.write(reinterpret_cast<const char*>(&meshItem), sizeof(RendererRuntime::v1Scene::MeshItem));
								}
							}
						}
					}
				}
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/Scene/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::Asset outputAsset;
			outputAsset.assetId = StringHelper::getAssetIdByString(assetIdAsString.c_str());
			strcpy(outputAsset.assetFilename, outputAssetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
