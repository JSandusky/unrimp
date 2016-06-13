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
#include "RendererToolkit/AssetCompiler/MaterialAssetCompiler.h"
#include "RendererToolkit/Helper/JsonMaterialBlueprintHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/Material/MaterialTechnique.h>
#include <RendererRuntime/Resource/Material/Loader/MaterialFileFormat.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#pragma warning(disable: 4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#pragma warning(disable: 4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	#pragma warning(disable: 4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#pragma warning(disable: 4061)	// warning C4061: enumerator 'rapidjson::GenericReader<rapidjson::UTF8<char>,rapidjson::UTF8<char>,rapidjson::CrtAllocator>::IterativeParsingStartState' in switch of enum 'rapidjson::GenericReader<rapidjson::UTF8<char>,rapidjson::UTF8<char>,rapidjson::CrtAllocator>::IterativeParsingState' is not explicitly handled by a case label
	#include <rapidjson/document.h>
#pragma warning(pop)

#include <fstream>
#include <algorithm>
#include <filesystem>


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
		bool orderByMaterialTechniqueId(const RendererRuntime::v1Material::Technique& left, const RendererRuntime::v1Material::Technique& right)
		{
			return (left.materialTechniqueId < right.materialTechniqueId);
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
	const AssetCompilerTypeId MaterialAssetCompiler::TYPE_ID("Material");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialAssetCompiler::MaterialAssetCompiler()
	{
		// Nothing here
	}

	MaterialAssetCompiler::~MaterialAssetCompiler()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId MaterialAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void MaterialAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
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
			// Read material asset compiler configuration
			const rapidjson::Value& rapidJsonValueMaterialAssetCompiler = rapidJsonValueAsset["MaterialAssetCompiler"];
			inputFile = rapidJsonValueMaterialAssetCompiler["InputFile"].GetString();
		}

		// Open the input file
		const std::string inputFilename = assetInputDirectory + inputFile;
		std::ifstream inputFileStream(inputFilename, std::ios::binary);
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".material";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		{ // Material
			// Parse JSON
			rapidjson::Document rapidJsonDocument;
			JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, inputFilename, "MaterialAsset", "1");

			// Mandatory main sections of the material
			const rapidjson::Value& rapidJsonValueMaterialAsset = rapidJsonDocument["MaterialAsset"];
			const rapidjson::Value& rapidJsonValueTechniques = rapidJsonValueMaterialAsset["Techniques"];
			const rapidjson::Value& rapidJsonValueProperties = rapidJsonValueMaterialAsset["Properties"];

			// Gather the asset IDs of all used material blueprints (one material blueprint per material technique)
			std::vector<RendererRuntime::v1Material::Technique> techniques;
			techniques.reserve(rapidJsonValueTechniques.MemberCount());
			for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorTechniques = rapidJsonValueTechniques.MemberBegin(); rapidJsonMemberIteratorTechniques != rapidJsonValueTechniques.MemberEnd(); ++rapidJsonMemberIteratorTechniques)
			{
				// Add technique
				RendererRuntime::v1Material::Technique technique;
				technique.materialTechniqueId	   = RendererRuntime::StringId(rapidJsonMemberIteratorTechniques->name.GetString());
				technique.materialBlueprintAssetId = static_cast<uint32_t>(std::atoi(rapidJsonMemberIteratorTechniques->value.GetString()));
				techniques.push_back(technique);
			}
			std::sort(techniques.begin(), techniques.end(), detail::orderByMaterialTechniqueId);

			// Gather all material blueprint properties of all referenced material blueprints
			RendererRuntime::MaterialProperties::SortedPropertyVector sortedMaterialPropertyVector;
			for (RendererRuntime::v1Material::Technique& technique : techniques)
			{
				// TODO(co) Error handling and simplification

				// Read material blueprint asset compiler configuration
				std::string materialBlueprintInputFile;
				const std::string absoluteMaterialBlueprintAssetFilename = JsonHelper::getAbsoluteAssetFilename(input, technique.materialBlueprintAssetId);
				{
					// Parse material blueprint asset JSON
					std::ifstream materialBlueprintAssetInputFileStream(absoluteMaterialBlueprintAssetFilename, std::ios::binary);
					rapidjson::Document rapidJsonDocumentMaterialBlueprintAsset;
					JsonHelper::parseDocumentByInputFileStream(rapidJsonDocumentMaterialBlueprintAsset, materialBlueprintAssetInputFileStream, absoluteMaterialBlueprintAssetFilename, "Asset", "1");
					materialBlueprintInputFile = rapidJsonDocumentMaterialBlueprintAsset["Asset"]["MaterialBlueprintAssetCompiler"]["InputFile"].GetString();
				}

				// Parse material blueprint JSON
				const std::string absoluteMaterialBlueprintFilename = std::tr2::sys::path(absoluteMaterialBlueprintAssetFilename).parent_path().generic_string() + '/' + materialBlueprintInputFile;
				std::ifstream materialBlueprintInputFileStream(absoluteMaterialBlueprintFilename, std::ios::binary);
				rapidjson::Document rapidJsonDocumentMaterialBlueprint;
				JsonHelper::parseDocumentByInputFileStream(rapidJsonDocumentMaterialBlueprint, materialBlueprintInputFileStream, absoluteMaterialBlueprintFilename, "MaterialBlueprintAsset", "1");
				RendererRuntime::MaterialProperties::SortedPropertyVector newSortedMaterialPropertyVector;
				JsonMaterialBlueprintHelper::readProperties(input, rapidJsonDocumentMaterialBlueprint["MaterialBlueprintAsset"]["Properties"], newSortedMaterialPropertyVector);

				// Add properties and avoid duplicates while doing so
				for (const RendererRuntime::MaterialProperty& materialProperty : newSortedMaterialPropertyVector)
				{
					const RendererRuntime::MaterialPropertyId materialPropertyId = materialProperty.getMaterialPropertyId();
					RendererRuntime::MaterialProperties::SortedPropertyVector::const_iterator iterator = std::lower_bound(sortedMaterialPropertyVector.cbegin(), sortedMaterialPropertyVector.cend(), materialPropertyId, RendererRuntime::detail::OrderByMaterialPropertyId());
					if (iterator == sortedMaterialPropertyVector.end() || iterator._Ptr->getMaterialPropertyId() != materialPropertyId)
					{
						// Add new material property
						sortedMaterialPropertyVector.insert(iterator, materialProperty);
					}
				}

				// Transform the source asset ID into a local asset ID
				technique.materialBlueprintAssetId = input.getCompiledAssetIdBySourceAssetId(technique.materialBlueprintAssetId);
			}

			{ // Material header
				RendererRuntime::v1Material::Header materialHeader;
				materialHeader.formatType		  = RendererRuntime::v1Material::FORMAT_TYPE;
				materialHeader.formatVersion	  = RendererRuntime::v1Material::FORMAT_VERSION;
				materialHeader.numberOfTechniques = techniques.size();
				materialHeader.numberOfProperties = sortedMaterialPropertyVector.size();

				// Write down the material header
				outputFileStream.write(reinterpret_cast<const char*>(&materialHeader), sizeof(RendererRuntime::v1Material::Header));
			}

			// Write down the material techniques
			outputFileStream.write(reinterpret_cast<const char*>(techniques.data()), sizeof(RendererRuntime::v1Material::Technique) * techniques.size());

			{ // Properties: Update material property values were required
				for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorProperties = rapidJsonValueProperties.MemberBegin(); rapidJsonMemberIteratorProperties != rapidJsonValueProperties.MemberEnd(); ++rapidJsonMemberIteratorProperties)
				{
					// Material property ID
					const char* propertyName = rapidJsonMemberIteratorProperties->name.GetString();
					const RendererRuntime::MaterialPropertyId materialPropertyId(propertyName);

					// Figure out the material property value type by using the material blueprint
					RendererRuntime::MaterialProperties::SortedPropertyVector::const_iterator iterator = std::lower_bound(sortedMaterialPropertyVector.cbegin(), sortedMaterialPropertyVector.cend(), materialPropertyId, RendererRuntime::detail::OrderByMaterialPropertyId());
					if (iterator != sortedMaterialPropertyVector.end())
					{
						RendererRuntime::MaterialProperty* materialProperty = iterator._Ptr;
						if (materialProperty->getMaterialPropertyId() == materialPropertyId)
						{
							// Set the material own property value
							static_cast<RendererRuntime::MaterialPropertyValue&>(*materialProperty) = JsonMaterialBlueprintHelper::mandatoryMaterialPropertyValue(input, rapidJsonValueProperties, propertyName, materialProperty->getValueType());
						}
					}
				}

				// Write down all material properties
				outputFileStream.write(reinterpret_cast<const char*>(sortedMaterialPropertyVector.data()), sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size());
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/Material/" + assetCategory + '/' + assetName;

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
