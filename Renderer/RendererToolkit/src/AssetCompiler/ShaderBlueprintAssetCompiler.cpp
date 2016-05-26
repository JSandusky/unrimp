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
#include "RendererToolkit/AssetCompiler/ShaderBlueprintAssetCompiler.h"
#include "RendererToolkit/Helper/StringHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/ShaderBlueprint/Loader/ShaderBlueprintFileFormat.h>

#include <memory>
#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId ShaderBlueprintAssetCompiler::TYPE_ID("ShaderBlueprint");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderBlueprintAssetCompiler::ShaderBlueprintAssetCompiler()
	{
	}

	ShaderBlueprintAssetCompiler::~ShaderBlueprintAssetCompiler()
	{
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId ShaderBlueprintAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void ShaderBlueprintAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
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
			// Read shader blueprint asset compiler configuration
			Poco::JSON::Object::Ptr jsonConfigurationObject = jsonAssetObject->get("ShaderBlueprintAssetCompiler").extract<Poco::JSON::Object::Ptr>();
			inputFile = jsonConfigurationObject->getValue<std::string>("InputFile");
			test	  = jsonConfigurationObject->optValue<uint32_t>("Test", test);
		}

		// Open the input file
		std::ifstream inputFileStream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetName");
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".shader_blueprint";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		{ // Shader blueprint
			// Get file size and file data
			std::string sourceCode;
			{
				std::string originalSourceCode;
				inputFileStream.seekg(0, std::ifstream::end);
				const std::streampos numberOfBytes = inputFileStream.tellg();
				inputFileStream.seekg(0, std::ifstream::beg);
				originalSourceCode.resize(static_cast<size_t>(numberOfBytes));
				inputFileStream.read(const_cast<char*>(originalSourceCode.c_str()), numberOfBytes);

				// Strip comments from source code
				StringHelper::stripCommentsFromSourceCode(originalSourceCode, sourceCode);
			}
			const std::streampos numberOfBytes = sourceCode.length();

			// Collect shader piece resources to include
			std::vector<RendererRuntime::AssetId> includeShaderPieceAssetIds;
			{
				// Gather "@includepiece(<asset ID>)" and then remove them from the shader source code
				// TODO(co) This is hacked on-the-fly, we certainly need something more robust
				size_t includePiecePosition = sourceCode.find("@includepiece");
				while (std::string::npos != includePiecePosition)
				{
					// ( asset ID )
					size_t openingPosition = sourceCode.find("(", includePiecePosition);
					size_t closingPosition = sourceCode.find(")", openingPosition);
					const size_t numberOfCharacters = closingPosition - openingPosition - 1;
					const std::string assetIdAsString = sourceCode.substr(openingPosition + 1, numberOfCharacters);
					includeShaderPieceAssetIds.push_back(static_cast<uint32_t>(std::atoi(assetIdAsString.c_str())));
					for (size_t i = includePiecePosition; i < closingPosition + 1; ++i)
					{
						sourceCode[i] = ' ';
					}

					// Next
					includePiecePosition = sourceCode.find("@includepiece", closingPosition);
				}

				// Map the source asset IDs to the compiled asset IDs
				for (RendererRuntime::AssetId& assetId : includeShaderPieceAssetIds)
				{
					SourceAssetIdToCompiledAssetId::const_iterator iterator = input.sourceAssetIdToCompiledAssetId.find(assetId);
					assetId = (iterator != input.sourceAssetIdToCompiledAssetId.cend()) ? iterator->second : 0;
				}
			}

			{ // Shader blueprint header
				RendererRuntime::v1ShaderBlueprint::Header shaderBlueprintHeader;
				shaderBlueprintHeader.formatType						 = RendererRuntime::v1ShaderBlueprint::FORMAT_TYPE;
				shaderBlueprintHeader.formatVersion						 = RendererRuntime::v1ShaderBlueprint::FORMAT_VERSION;
				shaderBlueprintHeader.numberOfIncludeShaderPieceAssetIds = includeShaderPieceAssetIds.size();
				shaderBlueprintHeader.numberOfShaderSourceCodeBytes		 = static_cast<uint32_t>(numberOfBytes);

				// Write down the shader blueprint header
				outputFileStream.write(reinterpret_cast<const char*>(&shaderBlueprintHeader), sizeof(RendererRuntime::v1ShaderBlueprint::Header));
			}

			// Write down the asset IDs of the shader pieces to include
			outputFileStream.write(reinterpret_cast<char*>(includeShaderPieceAssetIds.data()), sizeof(RendererRuntime::AssetId) * includeShaderPieceAssetIds.size());

			// Dump the unchanged content into the output file stream
			outputFileStream.write(sourceCode.c_str(), numberOfBytes);
		}

		{ // Update the output asset package
			const std::string assetCategory = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetCategory");
			const std::string assetIdAsString = input.projectName + "/ShaderBlueprint/" + assetCategory + '/' + assetName;

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
