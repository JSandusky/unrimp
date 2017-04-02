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
#include "RendererToolkit/AssetCompiler/ShaderBlueprintAssetCompiler.h"
#include "RendererToolkit/Helper/CacheManager.h"
#include "RendererToolkit/Helper/StringHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderProperties.h>
#include <RendererRuntime/Resource/ShaderBlueprint/Loader/ShaderBlueprintFileFormat.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

#include <memory>
#include <fstream>


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
		void gatherReferencedShaderProperties(const std::string& sourceString, const std::string& instructionName, RendererRuntime::ShaderProperties& referencedShaderProperties)
		{
			const size_t instructionNameLength = instructionName.length();
			const size_t endPosition = sourceString.length();
			size_t currentPosition = 0;

			// Till the end...
			while (currentPosition < endPosition)
			{
				const size_t index = sourceString.find(instructionName, currentPosition);
				if (std::string::npos != index)
				{
					currentPosition = index + instructionNameLength;

					// Find the end of the logical property expression
					size_t expressionEndPosition = sourceString.find("(", currentPosition);
					if (std::string::npos != expressionEndPosition)
					{
						// Skip '('
						++expressionEndPosition;

						// Find the end of the logical property expression
						int numberOfOpeningBrackets = 1;
						int numberOfClosingBrackets = 0;
						for (; (expressionEndPosition < endPosition) && (numberOfOpeningBrackets != numberOfClosingBrackets); ++expressionEndPosition)
						{
							const char currentCharacter = sourceString[expressionEndPosition];
							if ('(' == currentCharacter)
							{
								++numberOfOpeningBrackets;
							}
							else if (')' == currentCharacter)
							{
								++numberOfClosingBrackets;
							}
						}

						// TODO(co) I'm sure we can optimize this, but to have something to start with stick to a simple to implement solution
						static std::vector<std::string> stringParts;	// Optimization: To avoid constant allocations/deallocations, use a static instance (not multi-threading safe, of course)
						stringParts.clear();
						RendererToolkit::StringHelper::splitString(sourceString.substr(currentPosition, expressionEndPosition - currentPosition), " 	()!,", stringParts);
						for (const std::string& stringPart : stringParts)
						{
							if (stringPart != "&&" && stringPart != "||")
							{
								referencedShaderProperties.setPropertyValue(RendererRuntime::StringId(stringPart.c_str()), 1);
							}
						}

						// Next, please
						currentPosition = expressionEndPosition;
					}
					else
					{
						// We're done, end-of-file
						break;
					}
				}
				else
				{
					// We're done, end-of-file
					break;
				}
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
		std::string inputFile;
		{
			// Read shader blueprint asset compiler configuration
			const rapidjson::Value& rapidJsonValueShaderBlueprintAssetCompiler = rapidJsonValueAsset["ShaderBlueprintAssetCompiler"];
			inputFile = rapidJsonValueShaderBlueprintAssetCompiler["InputFile"].GetString();
		}

		// Open the input file
		const std::string inputFilename = assetInputDirectory + inputFile;
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".shader_blueprint";

		// Ask the cache manager whether or not we need to compile the source file (e.g. source changed or target not there)
		if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.assetFilename, inputFilename, outputAssetFilename, RendererRuntime::v1ShaderBlueprint::FORMAT_VERSION))
		{
			std::ifstream inputFileStream(assetInputDirectory + inputFile, std::ios::binary);
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
				const std::size_t numberOfBytes = sourceCode.length();

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

				// Gather IDs of shader properties known to the shader blueprint resource
				// -> Directly use "RendererRuntime::ShaderProperties" to keep things simple, although we don't need a shader property value
				RendererRuntime::ShaderProperties referencedShaderProperties;
				::detail::gatherReferencedShaderProperties(sourceCode, "@property", referencedShaderProperties);
				::detail::gatherReferencedShaderProperties(sourceCode, "@foreach", referencedShaderProperties);

				{ // Shader blueprint header
					RendererRuntime::v1ShaderBlueprint::Header shaderBlueprintHeader;
					shaderBlueprintHeader.formatType						 = RendererRuntime::v1ShaderBlueprint::FORMAT_TYPE;
					shaderBlueprintHeader.formatVersion						 = RendererRuntime::v1ShaderBlueprint::FORMAT_VERSION;
					shaderBlueprintHeader.numberOfIncludeShaderPieceAssetIds = static_cast<uint16_t>(includeShaderPieceAssetIds.size());
					shaderBlueprintHeader.numberReferencedShaderProperties   = static_cast<uint16_t>(referencedShaderProperties.getSortedPropertyVector().size());
					shaderBlueprintHeader.numberOfShaderSourceCodeBytes		 = static_cast<uint32_t>(numberOfBytes);

					// Write down the shader blueprint header
					outputFileStream.write(reinterpret_cast<const char*>(&shaderBlueprintHeader), sizeof(RendererRuntime::v1ShaderBlueprint::Header));
				}

				// Write down the asset IDs of the shader pieces to include
				outputFileStream.write(reinterpret_cast<char*>(includeShaderPieceAssetIds.data()), static_cast<std::streamsize>(sizeof(RendererRuntime::AssetId) * includeShaderPieceAssetIds.size()));

				// Write down the referenced shader properties
				outputFileStream.write(reinterpret_cast<char*>(referencedShaderProperties.getSortedPropertyVector().data()), static_cast<std::streamsize>(sizeof(RendererRuntime::ShaderProperties::Property) * referencedShaderProperties.getSortedPropertyVector().size()));

				// Dump the unchanged content into the output file stream
				outputFileStream.write(sourceCode.c_str(), static_cast<std::streamsize>(numberOfBytes));
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/ShaderBlueprint/" + assetCategory + '/' + assetName;

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
