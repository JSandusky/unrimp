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
#include "RendererToolkit/AssetCompiler/SceneAssetCompiler.h"
#include "RendererToolkit/Helper/JsonMaterialBlueprintHelper.h"
#include "RendererToolkit/Helper/JsonMaterialHelper.h"
#include "RendererToolkit/Helper/FileSystemHelper.h"
#include "RendererToolkit/Helper/CacheManager.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"
#include "RendererToolkit/Context.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Core/Math/Math.h>
#include <RendererRuntime/Core/File/MemoryFile.h>
#include <RendererRuntime/Resource/Scene/Item/Sky/SkyboxSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/Camera/CameraSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/Light/SunlightSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/Mesh/SkeletonMeshSceneItem.h>
#include <RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h>
#include <RendererRuntime/Resource/Material/MaterialProperties.h>
#include <RendererRuntime/Resource/Material/MaterialResourceManager.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::codecvt_base': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4774)	// warning C4774: 'sprintf_s' : format string expected in argument 3 is not a string literal
	PRAGMA_WARNING_DISABLE_MSVC(5026)	// warning C5026: 'std::_Generic_error_category': move constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'std::_Generic_error_category': move assignment operator was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP


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

		void fillSortedMaterialPropertyVector(const RendererToolkit::IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValueSceneItem, RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector)
		{
			// Check whether or not material properties should be set
			if (rapidJsonValueSceneItem.HasMember("SetMaterialProperties"))
			{
				if (rapidJsonValueSceneItem.HasMember("MaterialAssetId"))
				{
					RendererToolkit::JsonMaterialHelper::getPropertiesByMaterialAssetId(input, RendererToolkit::StringHelper::getSourceAssetIdByString(rapidJsonValueSceneItem["MaterialAssetId"].GetString()), sortedMaterialPropertyVector);
				}
				else if (rapidJsonValueSceneItem.HasMember("MaterialBlueprint"))
				{
					RendererToolkit::JsonMaterialBlueprintHelper::getPropertiesByMaterialBlueprintAssetId(input, RendererToolkit::StringHelper::getSourceAssetIdByString(rapidJsonValueSceneItem["MaterialBlueprint"].GetString()), sortedMaterialPropertyVector);
				}
				if (!sortedMaterialPropertyVector.empty())
				{
					// Update material property values were required
					const rapidjson::Value& rapidJsonValueProperties = rapidJsonValueSceneItem["SetMaterialProperties"];
					RendererToolkit::JsonMaterialHelper::readMaterialPropertyValues(input, rapidJsonValueProperties, sortedMaterialPropertyVector);

					// Collect all material property IDs explicitly defined inside the scene item
					typedef std::unordered_map<uint32_t, std::string> DefinedMaterialPropertyIds;	// Key = "RendererRuntime::RendererRuntime::MaterialPropertyId"
					DefinedMaterialPropertyIds definedMaterialPropertyIds;
					for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorProperties = rapidJsonValueProperties.MemberBegin(); rapidJsonMemberIteratorProperties != rapidJsonValueProperties.MemberEnd(); ++rapidJsonMemberIteratorProperties)
					{
						definedMaterialPropertyIds.emplace(RendererRuntime::MaterialPropertyId(rapidJsonMemberIteratorProperties->name.GetString()), rapidJsonMemberIteratorProperties->value.GetString());
					}

					// Mark material properties as overwritten
					for (RendererRuntime::MaterialProperty& materialProperty : sortedMaterialPropertyVector)
					{
						DefinedMaterialPropertyIds::const_iterator iterator = definedMaterialPropertyIds.find(materialProperty.getMaterialPropertyId());
						if (iterator!= definedMaterialPropertyIds.end())
						{
							materialProperty.setOverwritten(true);
						}
					}
				}
			}
		}

		void readSkyboxSceneItem(const RendererToolkit::IAssetCompiler::Input& input, const RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector, const rapidjson::Value& rapidJsonValueSceneItem, RendererRuntime::v1Scene::SkyboxItem& skyboxItem)
		{
			// Set data
			RendererRuntime::AssetId materialAssetId;
			RendererRuntime::AssetId materialBlueprintAssetId;
			RendererToolkit::JsonHelper::optionalCompiledAssetId(input, rapidJsonValueSceneItem, "MaterialAssetId", materialAssetId);
			RendererToolkit::JsonHelper::optionalStringIdProperty(rapidJsonValueSceneItem, "MaterialTechnique", skyboxItem.materialTechniqueId);
			RendererToolkit::JsonHelper::optionalCompiledAssetId(input, rapidJsonValueSceneItem, "MaterialBlueprint", materialBlueprintAssetId);
			skyboxItem.materialAssetId = materialAssetId;
			skyboxItem.materialBlueprintAssetId = materialBlueprintAssetId;
			skyboxItem.numberOfMaterialProperties = static_cast<uint32_t>(sortedMaterialPropertyVector.size());

			// Sanity checks
			if (RendererRuntime::isUninitialized(skyboxItem.materialAssetId) && RendererRuntime::isUninitialized(skyboxItem.materialBlueprintAssetId))
			{
				throw std::runtime_error("Material asset ID or material blueprint asset ID must be defined");
			}
			if (RendererRuntime::isInitialized(skyboxItem.materialAssetId) && RendererRuntime::isInitialized(skyboxItem.materialBlueprintAssetId))
			{
				throw std::runtime_error("Material asset ID is defined, but material blueprint asset ID is defined as well. Only one asset ID is allowed.");
			}
			if (RendererRuntime::isInitialized(skyboxItem.materialAssetId) && RendererRuntime::isUninitialized(skyboxItem.materialTechniqueId))
			{
				throw std::runtime_error("Material asset ID is defined, but material technique is not defined");
			}
			if (RendererRuntime::isInitialized(skyboxItem.materialBlueprintAssetId) && RendererRuntime::isUninitialized(skyboxItem.materialTechniqueId))
			{
				skyboxItem.materialTechniqueId = RendererRuntime::MaterialResourceManager::DEFAULT_MATERIAL_TECHNIQUE_ID;
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

	bool SceneAssetCompiler::checkIfChanged(const Input& input, const Configuration& configuration) const
	{
		// Let the cache manager check whether or not the files have been changed in order to speed up later checks and to support dependency tracking
		const std::string virtualInputFilename = input.virtualAssetInputDirectory + '/' + configuration.rapidJsonDocumentAsset["Asset"]["SceneAssetCompiler"]["InputFile"].GetString();
		return input.cacheManager.checkIfFileIsModified(configuration.rendererTarget, input.virtualAssetFilename, {virtualInputFilename}, RendererRuntime::v1Scene::FORMAT_VERSION);
	}

	void SceneAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Get relevant data
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];
		const std::string virtualInputFilename = input.virtualAssetInputDirectory + '/' + rapidJsonValueAsset["SceneAssetCompiler"]["InputFile"].GetString();
		const std::string assetName = std_filesystem::path(input.virtualAssetFilename).stem().generic_string();
		const std::string virtualOutputAssetFilename = input.virtualAssetOutputDirectory + '/' + assetName + ".scene";

		// Ask the cache manager whether or not we need to compile the source file (e.g. source changed or target not there)
		CacheManager::CacheEntries cacheEntries;
		if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.virtualAssetFilename, virtualInputFilename, virtualOutputAssetFilename, RendererRuntime::v1Scene::FORMAT_VERSION, cacheEntries))
		{
			RendererRuntime::MemoryFile memoryFile(0, 4096);

			{ // Scene
				// Parse JSON
				rapidjson::Document rapidJsonDocument;
				JsonHelper::loadDocumentByFilename(input.context.getFileManager(), virtualInputFilename, "SceneAsset", "1", rapidJsonDocument);

				{ // Write down the scene resource header
					RendererRuntime::v1Scene::SceneHeader sceneHeader;
					sceneHeader.unused = 42;	// TODO(co) Currently the scene header is unused
					memoryFile.write(&sceneHeader, sizeof(RendererRuntime::v1Scene::SceneHeader));
				}

				// Mandatory main sections of the material blueprint
				const rapidjson::Value& rapidJsonValueSceneAsset = rapidJsonDocument["SceneAsset"];
				const rapidjson::Value& rapidJsonValueNodes = rapidJsonValueSceneAsset["Nodes"];

				{ // Write down the scene nodes
					const rapidjson::SizeType numberOfNodes = rapidJsonValueNodes.Size();
					RendererRuntime::v1Scene::Nodes nodes;
					nodes.numberOfNodes = numberOfNodes;
					memoryFile.write(&nodes, sizeof(RendererRuntime::v1Scene::Nodes));

					// Loop through all scene nodes
					for (rapidjson::SizeType nodeIndex = 0; nodeIndex < numberOfNodes; ++nodeIndex)
					{
						const rapidjson::Value& rapidJsonValueNode = rapidJsonValueNodes[nodeIndex];
						const rapidjson::Value* rapidJsonValueItems = rapidJsonValueNode.HasMember("Items") ? &rapidJsonValueNode["Items"] : nullptr;

						{ // Write down the scene node
							RendererRuntime::v1Scene::Node node;

							// Get the scene node transform
							node.transform.scale = RendererRuntime::Math::VEC3_ONE;
							if (rapidJsonValueNode.HasMember("Properties"))
							{
								const rapidjson::Value& rapidJsonValueProperties = rapidJsonValueNode["Properties"];

								// Position, rotation and scale
								JsonHelper::optionalUnitNProperty(rapidJsonValueProperties, "Position", &node.transform.position.x, 3);
								JsonHelper::optionalRotationQuaternionProperty(rapidJsonValueProperties, "Rotation", node.transform.rotation);
								JsonHelper::optionalFactorNProperty(rapidJsonValueProperties, "Scale", &node.transform.scale.x, 3);
							}

							// Write down the scene node
							node.numberOfItems = (nullptr != rapidJsonValueItems) ? rapidJsonValueItems->MemberCount() : 0;
							memoryFile.write(&node, sizeof(RendererRuntime::v1Scene::Node));
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
								RendererRuntime::MaterialProperties::SortedPropertyVector sortedMaterialPropertyVector;
								if (RendererRuntime::CameraSceneItem::TYPE_ID == typeId)
								{
									// Nothing here
								}
								else if (RendererRuntime::LightSceneItem::TYPE_ID == typeId)
								{
									numberOfBytes = sizeof(RendererRuntime::v1Scene::LightItem);
								}
								else if (RendererRuntime::SunlightSceneItem::TYPE_ID == typeId)
								{
									numberOfBytes = sizeof(RendererRuntime::v1Scene::SunlightItem);
								}
								else if (RendererRuntime::MeshSceneItem::TYPE_ID == typeId || RendererRuntime::SkeletonMeshSceneItem::TYPE_ID == typeId)
								{
									const uint32_t numberOfSubMeshMaterialAssetIds = rapidJsonValueItem.HasMember("SubMeshMaterials") ? rapidJsonValueItem["SubMeshMaterials"].Size() : 0;
									numberOfBytes = sizeof(RendererRuntime::v1Scene::MeshItem) + sizeof(RendererRuntime::AssetId) * numberOfSubMeshMaterialAssetIds;
									if (RendererRuntime::SkeletonMeshSceneItem::TYPE_ID == typeId)
									{
										numberOfBytes += sizeof(RendererRuntime::v1Scene::SkeletonMeshItem);
									}
								}
								else if (RendererRuntime::SkyboxSceneItem::TYPE_ID == typeId)
								{
									::detail::fillSortedMaterialPropertyVector(input, rapidJsonValueItem, sortedMaterialPropertyVector);
									numberOfBytes = static_cast<uint32_t>(sizeof(RendererRuntime::v1Scene::SkyboxItem) + sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size());
								}
								else
								{
									// Error!
									throw std::runtime_error("Scene item type \"" + std::string(rapidJsonMemberIteratorItems->name.GetString()) + "\" is unknown");
								}

								{ // Write down the scene item header
									RendererRuntime::v1Scene::ItemHeader itemHeader;
									itemHeader.typeId		 = typeId;
									itemHeader.numberOfBytes = numberOfBytes;
									memoryFile.write(&itemHeader, sizeof(RendererRuntime::v1Scene::ItemHeader));
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
										JsonHelper::optionalRgbColorProperty(rapidJsonValueItem, "Color", lightItem.color);
										JsonHelper::optionalUnitNProperty(rapidJsonValueItem, "Radius", &lightItem.radius, 1);
										JsonHelper::optionalAngleProperty(rapidJsonValueItem, "InnerAngle", lightItem.innerAngle);
										JsonHelper::optionalAngleProperty(rapidJsonValueItem, "OuterAngle", lightItem.outerAngle);
										JsonHelper::optionalUnitNProperty(rapidJsonValueItem, "NearClipDistance", &lightItem.nearClipDistance, 1);

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
										if (lightItem.innerAngle < 0.0f)
										{
											throw std::runtime_error("The inner spot light angle must be >= 0 degrees");
										}
										if (lightItem.outerAngle >= glm::radians(90.0f))
										{
											throw std::runtime_error("The outer spot light angle must be < 90 degrees");
										}
										if (lightItem.innerAngle >= lightItem.outerAngle)
										{
											throw std::runtime_error("The inner spot light angle must be smaller as the outer spot light angle");
										}
										if (lightItem.nearClipDistance < 0.0f)
										{
											throw std::runtime_error("The spot light near clip distance must be greater as zero");
										}

										// Write down
										memoryFile.write(&lightItem, sizeof(RendererRuntime::v1Scene::LightItem));
									}
									else if (RendererRuntime::SunlightSceneItem::TYPE_ID == typeId)
									{
										RendererRuntime::v1Scene::SunlightItem sunlightItem;

										// Read properties
										JsonHelper::optionalTimeOfDayProperty(rapidJsonValueItem, "SunriseTime", sunlightItem.sunriseTime);
										JsonHelper::optionalTimeOfDayProperty(rapidJsonValueItem, "SunsetTime", sunlightItem.sunsetTime);
										JsonHelper::optionalAngleProperty(rapidJsonValueItem, "EastDirection", sunlightItem.eastDirection);
										JsonHelper::optionalAngleProperty(rapidJsonValueItem, "AngleOfIncidence", sunlightItem.angleOfIncidence);
										JsonHelper::optionalTimeOfDayProperty(rapidJsonValueItem, "TimeOfDay", sunlightItem.timeOfDay);

										// Write down
										memoryFile.write(&sunlightItem, sizeof(RendererRuntime::v1Scene::SunlightItem));
									}
									else if (RendererRuntime::MeshSceneItem::TYPE_ID == typeId || RendererRuntime::SkeletonMeshSceneItem::TYPE_ID == typeId)
									{
										// Skeleton mesh scene item
										if (RendererRuntime::SkeletonMeshSceneItem::TYPE_ID == typeId)
										{
											RendererRuntime::v1Scene::SkeletonMeshItem skeletonMeshItem;

											// Optional skeleton animation: Map the source asset ID to the compiled asset ID
											skeletonMeshItem.skeletonAnimationAssetId = RendererRuntime::getUninitialized<RendererRuntime::AssetId>();
											JsonHelper::optionalCompiledAssetId(input, rapidJsonValueItem, "SkeletonAnimation", skeletonMeshItem.skeletonAnimationAssetId);

											// Write down
											memoryFile.write(&skeletonMeshItem, sizeof(RendererRuntime::v1Scene::SkeletonMeshItem));
										}

										// Mesh scene item
										RendererRuntime::v1Scene::MeshItem meshItem;

										// Map the source asset ID to the compiled asset ID
										meshItem.meshAssetId = JsonHelper::getCompiledAssetId(input, rapidJsonValueItem, "Mesh");

										// Optional sub-mesh material asset IDs to be able to overwrite the original material asset ID of sub-meshes
										std::vector<RendererRuntime::AssetId> subMeshMaterialAssetIds;
										if (rapidJsonValueItem.HasMember("SubMeshMaterials"))
										{
											const rapidjson::Value& rapidJsonValueSubMeshMaterialAssetIds = rapidJsonValueItem["SubMeshMaterials"];
											const uint32_t numberOfSubMeshMaterialAssetIds = rapidJsonValueSubMeshMaterialAssetIds.Size();
											subMeshMaterialAssetIds.resize(numberOfSubMeshMaterialAssetIds);
											for (uint32_t i = 0; i < numberOfSubMeshMaterialAssetIds; ++i)
											{
												// Empty string means "Don't overwrite the original material asset ID of the sub-mesh"
												const std::string valueAsString = rapidJsonValueSubMeshMaterialAssetIds[i].GetString();
												subMeshMaterialAssetIds[i] = valueAsString.empty() ? RendererRuntime::getUninitialized<RendererRuntime::AssetId>() : StringHelper::getAssetIdByString(valueAsString, input);
											}
										}
										meshItem.numberOfSubMeshMaterialAssetIds = static_cast<uint32_t>(subMeshMaterialAssetIds.size());

										// Write down
										memoryFile.write(&meshItem, sizeof(RendererRuntime::v1Scene::MeshItem));
										if (!subMeshMaterialAssetIds.empty())
										{
											// Write down all sub-mesh material asset IDs
											memoryFile.write(subMeshMaterialAssetIds.data(), sizeof(RendererRuntime::AssetId) * subMeshMaterialAssetIds.size());
										}
									}
									else if (RendererRuntime::SkyboxSceneItem::TYPE_ID == typeId)
									{
										RendererRuntime::v1Scene::SkyboxItem skyboxItem;
										::detail::readSkyboxSceneItem(input, sortedMaterialPropertyVector, rapidJsonValueItem, skyboxItem);

										// Write down
										memoryFile.write(&skyboxItem, sizeof(RendererRuntime::v1Scene::SkyboxItem));
										if (!sortedMaterialPropertyVector.empty())
										{
											// Write down all material properties
											memoryFile.write(sortedMaterialPropertyVector.data(), sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size());
										}
									}
								}
							}
						}
					}
				}
			}

			// Write LZ4 compressed output
			memoryFile.writeLz4CompressedDataToFile(RendererRuntime::v1Scene::FORMAT_TYPE, RendererRuntime::v1Scene::FORMAT_VERSION, virtualOutputAssetFilename, input.context.getFileManager());

			// Store new cache entries or update existing ones
			input.cacheManager.storeOrUpdateCacheEntries(cacheEntries);
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/Scene/" + assetCategory + '/' + assetName;
			outputAsset(input.context.getFileManager(), assetIdAsString, virtualOutputAssetFilename, *output.outputAssetPackage);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
