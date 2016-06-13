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

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#pragma warning(disable: 4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#pragma warning(disable: 4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	#pragma warning(disable: 4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#pragma warning(disable: 4061)	// warning C4061: enumerator 'rapidjson::GenericReader<rapidjson::UTF8<char>,rapidjson::UTF8<char>,rapidjson::CrtAllocator>::IterativeParsingStartState' in switch of enum 'rapidjson::GenericReader<rapidjson::UTF8<char>,rapidjson::UTF8<char>,rapidjson::CrtAllocator>::IterativeParsingState' is not explicitly handled by a case label
	#include <rapidjson/document.h>
#pragma warning(pop)

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
		// Nothing here
	}

	ShaderBlueprintAssetCompiler::~ShaderBlueprintAssetCompiler()
	{
		// Nothing here
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
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read configuration
		// TODO(co) Add required properties
		std::string inputFile;
		{
			// Read shader blueprint asset compiler configuration
			const rapidjson::Value& rapidJsonValueShaderBlueprintAssetCompiler = rapidJsonValueAsset["ShaderBlueprintAssetCompiler"];
			inputFile = rapidJsonValueShaderBlueprintAssetCompiler["InputFile"].GetString();
		}

		// Open the input file
		std::ifstream inputFileStream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
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
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
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
