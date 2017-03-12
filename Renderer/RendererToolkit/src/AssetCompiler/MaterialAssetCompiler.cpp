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
#include "RendererToolkit/Helper/CacheManager.h"
#include "RendererToolkit/Helper/JsonMaterialBlueprintHelper.h"
#include "RendererToolkit/Helper/JsonMaterialHelper.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>

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
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".material";

		// Ask cache manager if we need to compile the source file (e.g. source changed or target not there)
		if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.assetFilename, inputFilename, outputAssetFilename))
		{
			std::ifstream inputFileStream(inputFilename, std::ios::binary);
			std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

			{ // Material
				// Parse JSON
				rapidjson::Document rapidJsonDocument;
				JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, inputFilename, "MaterialAsset", "1");
				std::vector<RendererRuntime::v1Material::Technique> techniques;
				RendererRuntime::MaterialProperties::SortedPropertyVector sortedMaterialPropertyVector;
				JsonMaterialHelper::getTechniquesAndPropertiesByMaterialAssetId(input, rapidJsonDocument, techniques, sortedMaterialPropertyVector);

				{ // Material header
					RendererRuntime::v1Material::Header materialHeader;
					materialHeader.formatType		  = RendererRuntime::v1Material::FORMAT_TYPE;
					materialHeader.formatVersion	  = RendererRuntime::v1Material::FORMAT_VERSION;
					materialHeader.numberOfTechniques = static_cast<uint32_t>(techniques.size());
					materialHeader.numberOfProperties = static_cast<uint32_t>(sortedMaterialPropertyVector.size());

					// Write down the material header
					outputFileStream.write(reinterpret_cast<const char*>(&materialHeader), sizeof(RendererRuntime::v1Material::Header));
				}

				// Write down the material techniques
				outputFileStream.write(reinterpret_cast<const char*>(techniques.data()), static_cast<std::streamsize>(sizeof(RendererRuntime::v1Material::Technique) * techniques.size()));

				// Write down all material properties
				outputFileStream.write(reinterpret_cast<const char*>(sortedMaterialPropertyVector.data()), static_cast<std::streamsize>(sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size()));
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/Material/" + assetCategory + '/' + assetName;

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
