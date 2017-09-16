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
#include "RendererToolkit/AssetCompiler/MaterialAssetCompiler.h"
#include "RendererToolkit/Helper/JsonMaterialBlueprintHelper.h"
#include "RendererToolkit/Helper/JsonMaterialHelper.h"
#include "RendererToolkit/Helper/FileSystemHelper.h"
#include "RendererToolkit/Helper/CacheManager.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"
#include "RendererToolkit/Context.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Core/File/MemoryFile.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

#include <fstream>


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

	bool MaterialAssetCompiler::checkIfChanged(const Input& input, const Configuration& configuration) const
	{
		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read configuration
		std::string inputFile;
		{
			// Read material asset compiler configuration
			const rapidjson::Value& rapidJsonValueMaterialAssetCompiler = rapidJsonValueAsset["MaterialAssetCompiler"];
			inputFile = rapidJsonValueMaterialAssetCompiler["InputFile"].GetString();
		}

		const std::string inputFilename = input.assetInputDirectory + inputFile;

		// Let the cache manager check if the files has changed. This speeds up later checks and supports dependency tracking
		return input.cacheManager.checkIfFileIsModified(configuration.rendererTarget, input.assetFilename, {inputFilename}, RendererRuntime::v1Material::FORMAT_VERSION);
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
		std::string inputFile;
		{
			// Read material asset compiler configuration
			const rapidjson::Value& rapidJsonValueMaterialAssetCompiler = rapidJsonValueAsset["MaterialAssetCompiler"];
			inputFile = rapidJsonValueMaterialAssetCompiler["InputFile"].GetString();
		}

		// Open the input file
		const std::string inputFilename = assetInputDirectory + inputFile;
		const std::string assetName = std_filesystem::path(input.assetFilename).stem().generic_string();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".material";
		
		std::vector<std::string> inputFilenames;
		inputFilenames.emplace_back(inputFilename);

		std::vector<std::string> dependecyFiles;
		
		// Read in dependecy files
		{ // Material
			std::ifstream inputFileStream(inputFilename, std::ios::binary);
			// Parse JSON
			rapidjson::Document rapidJsonDocument;
			JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, inputFilename, "MaterialAsset", "1");
			
			// TODO(sw) parts copied from JsonMaterialHelper::getTechniquesAndPropertiesByMaterialAssetId
			const rapidjson::Value& rapidJsonValueMaterialAsset = rapidJsonDocument["MaterialAsset"];
			const rapidjson::Value& rapidJsonValueTechniques = rapidJsonValueMaterialAsset["Techniques"];
			for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorTechniques = rapidJsonValueTechniques.MemberBegin(); rapidJsonMemberIteratorTechniques != rapidJsonValueTechniques.MemberEnd(); ++rapidJsonMemberIteratorTechniques)
			{
				const RendererRuntime::AssetId materialBlueprintAssetId = StringHelper::getSourceAssetIdByString(rapidJsonMemberIteratorTechniques->value.GetString());
				const std::string absoluteMaterialBlueprintAssetFilename = JsonHelper::getAbsoluteAssetFilename(input, materialBlueprintAssetId);
				dependecyFiles.emplace_back(std::move(absoluteMaterialBlueprintAssetFilename));
			}
		}

		// Ask the cache manager whether or not we need to compile the source file (e.g. source changed or target not there)
		CacheManager::CacheEntries cacheEntries;
		if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.assetFilename, inputFilenames, outputAssetFilename, RendererRuntime::v1Material::FORMAT_VERSION, cacheEntries) || input.cacheManager.dependencyFilesChanged(dependecyFiles))
		{
			std::ifstream inputFileStream(inputFilename, std::ios::binary);
			RendererRuntime::MemoryFile memoryFile(0, 1024);

			{ // Material
				// Parse JSON
				rapidjson::Document rapidJsonDocument;
				JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, inputFilename, "MaterialAsset", "1");
				std::vector<RendererRuntime::v1Material::Technique> techniques;
				RendererRuntime::MaterialProperties::SortedPropertyVector sortedMaterialPropertyVector;
				JsonMaterialHelper::getTechniquesAndPropertiesByMaterialAssetId(input, rapidJsonDocument, techniques, sortedMaterialPropertyVector);

				{ // Write down the material header
					RendererRuntime::v1Material::MaterialHeader materialHeader;
					materialHeader.numberOfTechniques = static_cast<uint32_t>(techniques.size());
					materialHeader.numberOfProperties = static_cast<uint32_t>(sortedMaterialPropertyVector.size());
					memoryFile.write(&materialHeader, sizeof(RendererRuntime::v1Material::MaterialHeader));
				}

				// Write down the material techniques
				memoryFile.write(techniques.data(), sizeof(RendererRuntime::v1Material::Technique) * techniques.size());

				// Write down all material properties
				memoryFile.write(sortedMaterialPropertyVector.data(), sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size());
			}

			// Write LZ4 compressed output
			memoryFile.writeLz4CompressedDataToFile(RendererRuntime::v1Material::FORMAT_TYPE, RendererRuntime::v1Material::FORMAT_VERSION, outputAssetFilename, input.context.getFileManager());

			// Store new cache entries or update existing ones
			input.cacheManager.storeOrUpdateCacheEntriesInDatabase(cacheEntries);
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/Material/" + assetCategory + '/' + assetName;
			outputAsset(assetIdAsString, outputAssetFilename, outputAssetPackage);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
