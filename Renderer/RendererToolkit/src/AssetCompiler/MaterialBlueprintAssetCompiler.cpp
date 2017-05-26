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
#include "RendererToolkit/AssetCompiler/MaterialBlueprintAssetCompiler.h"
#include "RendererToolkit/Helper/JsonMaterialBlueprintHelper.h"
#include "RendererToolkit/Helper/FileSystemHelper.h"
#include "RendererToolkit/Helper/CacheManager.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Core/File/MemoryFile.h>
#include <RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderProperties.h>
#include <RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintFileFormat.h>

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
	const AssetCompilerTypeId MaterialBlueprintAssetCompiler::TYPE_ID("MaterialBlueprint");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialBlueprintAssetCompiler::MaterialBlueprintAssetCompiler()
	{
		// Nothing here
	}

	MaterialBlueprintAssetCompiler::~MaterialBlueprintAssetCompiler()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId MaterialBlueprintAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void MaterialBlueprintAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory	= input.assetOutputDirectory;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read configuration
		std::string inputFile;
		bool allowCrazyNumberOfShaderCombinations = false;
		{
			// Read material blueprint asset compiler configuration
			const rapidjson::Value& rapidJsonValueMaterialBlueprintAssetCompiler = rapidJsonValueAsset["MaterialBlueprintAssetCompiler"];
			inputFile = rapidJsonValueMaterialBlueprintAssetCompiler["InputFile"].GetString();
			JsonHelper::optionalBooleanProperty(rapidJsonValueMaterialBlueprintAssetCompiler, "AllowCrazyNumberOfShaderCombinations", allowCrazyNumberOfShaderCombinations);
		}

		// Open the input file
		const std::string inputFilename = assetInputDirectory + inputFile;
		const std::string assetName = std_filesystem::path(input.assetFilename).stem().generic_string();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".material_blueprint";

		// Ask the cache manager whether or not we need to compile the source file (e.g. source changed or target not there)
		CacheManager::CacheEntries cacheEntries;
		if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.assetFilename, inputFilename, outputAssetFilename, RendererRuntime::v1MaterialBlueprint::FORMAT_VERSION, cacheEntries))
		{
			std::ifstream inputFileStream(inputFilename, std::ios::binary);
			RendererRuntime::MemoryFile memoryFile(0, 4096);

			{ // Material blueprint
				// Parse JSON
				rapidjson::Document rapidJsonDocument;
				JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, inputFilename, "MaterialBlueprintAsset", "1");

				// Mandatory and optional main sections of the material blueprint
				static const rapidjson::Value EMPTY_VALUE;
				const rapidjson::Value& rapidJsonValueMaterialBlueprintAsset = rapidJsonDocument["MaterialBlueprintAsset"];
				const rapidjson::Value& rapidJsonValueProperties			 = rapidJsonValueMaterialBlueprintAsset.HasMember("Properties")								   ? rapidJsonValueMaterialBlueprintAsset["Properties"]	: EMPTY_VALUE;
				const rapidjson::Value& rapidJsonValueResources				 = rapidJsonValueMaterialBlueprintAsset.HasMember("Resources")								   ? rapidJsonValueMaterialBlueprintAsset["Resources"]	: EMPTY_VALUE;
				const rapidjson::Value& rapidJsonValueUniformBuffers		 = (rapidJsonValueResources.IsObject() && rapidJsonValueResources.HasMember("UniformBuffers")) ? rapidJsonValueResources["UniformBuffers"]			: EMPTY_VALUE;
				const rapidjson::Value& rapidJsonValueTextureBuffers		 = (rapidJsonValueResources.IsObject() && rapidJsonValueResources.HasMember("TextureBuffers")) ? rapidJsonValueResources["TextureBuffers"]			: EMPTY_VALUE;
				const rapidjson::Value& rapidJsonValueSamplerStates			 = (rapidJsonValueResources.IsObject() && rapidJsonValueResources.HasMember("SamplerStates"))  ? rapidJsonValueResources["SamplerStates"]			: EMPTY_VALUE;
				const rapidjson::Value& rapidJsonValueTextures				 = (rapidJsonValueResources.IsObject() && rapidJsonValueResources.HasMember("Textures"))	   ? rapidJsonValueResources["Textures"]				: EMPTY_VALUE;

				// Gather all material properties
				RendererRuntime::MaterialProperties::SortedPropertyVector sortedMaterialPropertyVector;
				RendererRuntime::ShaderProperties visualImportanceOfShaderProperties;
				RendererRuntime::ShaderProperties maximumIntegerValueOfShaderProperties;
				const RendererRuntime::ShaderProperties::SortedPropertyVector& visualImportanceOfShaderPropertiesVector = visualImportanceOfShaderProperties.getSortedPropertyVector();
				const RendererRuntime::ShaderProperties::SortedPropertyVector& maximumIntegerValueOfShaderPropertiesVector = maximumIntegerValueOfShaderProperties.getSortedPropertyVector();
				if (rapidJsonValueProperties.IsObject())
				{
					JsonMaterialBlueprintHelper::readProperties(input, rapidJsonValueProperties, sortedMaterialPropertyVector, visualImportanceOfShaderProperties, maximumIntegerValueOfShaderProperties, false, true);

					// Child protection: Throw an exception if there are too many shader combination properties to protect the material blueprint designer of over-engineering material blueprints
					if (!allowCrazyNumberOfShaderCombinations)
					{
						uint32_t numberOfShaderCombinationProperties = 0;
						for (const RendererRuntime::MaterialProperty& materialProperty : sortedMaterialPropertyVector)
						{
							if (materialProperty.getUsage() == RendererRuntime::MaterialProperty::Usage::SHADER_COMBINATION)
							{
								++numberOfShaderCombinationProperties;
							}
						}
						static const uint32_t MAXIMUM_NUMBER_OF_SHADER_COMBINATIONS = 4;	// This is no technical limit. See "RendererRuntime::MaterialBlueprintResource" class documentation regarding shader combination explosion for background information.
						if (numberOfShaderCombinationProperties > MAXIMUM_NUMBER_OF_SHADER_COMBINATIONS)
						{
							throw std::runtime_error("Material blueprint asset \"" + inputFilename + "\" is using " + std::to_string(numberOfShaderCombinationProperties) + " shader combination material properties. In order to prevent an shader combination explosion, only " + std::to_string(MAXIMUM_NUMBER_OF_SHADER_COMBINATIONS) + " shader combination material properties are allowed. If you know what you're doing, the child protection can be disabled by using \"AllowCrazyNumberOfShaderCombinations\"=\"TRUE\" inside the material blueprint asset compiler configuration.");
						}
					}
				}

				{ // Write down the material blueprint header
					RendererRuntime::v1MaterialBlueprint::MaterialBlueprintHeader materialBlueprintHeader;
					materialBlueprintHeader.numberOfProperties							= rapidJsonValueProperties.MemberCount();
					materialBlueprintHeader.numberOfShaderCombinationProperties			= static_cast<uint32_t>(visualImportanceOfShaderPropertiesVector.size());
					materialBlueprintHeader.numberOfIntegerShaderCombinationProperties	= static_cast<uint32_t>(maximumIntegerValueOfShaderPropertiesVector.size());	// Each integer shader combination property must have a defined maximum value
					materialBlueprintHeader.numberOfUniformBuffers						= rapidJsonValueUniformBuffers.IsObject() ? rapidJsonValueUniformBuffers.MemberCount() : 0;
					materialBlueprintHeader.numberOfTextureBuffers						= rapidJsonValueTextureBuffers.IsObject() ? rapidJsonValueTextureBuffers.MemberCount() : 0;
					materialBlueprintHeader.numberOfSamplerStates						= rapidJsonValueSamplerStates.IsObject() ? rapidJsonValueSamplerStates.MemberCount() : 0;
					materialBlueprintHeader.numberOfTextures							= rapidJsonValueTextures.IsObject() ? rapidJsonValueTextures.MemberCount() : 0;
					memoryFile.write(&materialBlueprintHeader, sizeof(RendererRuntime::v1MaterialBlueprint::MaterialBlueprintHeader));
				}

				// Write down all material properties
				memoryFile.write(sortedMaterialPropertyVector.data(), sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size());

				// Write down visual importance of shader properties
				memoryFile.write(visualImportanceOfShaderPropertiesVector.data(), sizeof(RendererRuntime::ShaderProperties::Property) * visualImportanceOfShaderPropertiesVector.size());

				// Write down maximum integer value of shader properties
				memoryFile.write(maximumIntegerValueOfShaderPropertiesVector.data(), sizeof(RendererRuntime::ShaderProperties::Property) * maximumIntegerValueOfShaderPropertiesVector.size());

				// Root signature
				RendererRuntime::ShaderProperties shaderProperties;
				JsonMaterialBlueprintHelper::readRootSignature(rapidJsonValueMaterialBlueprintAsset["RootSignature"], memoryFile, shaderProperties);

				// Pipeline state object (PSO)
				JsonMaterialBlueprintHelper::readPipelineStateObject(input, rapidJsonValueMaterialBlueprintAsset["PipelineState"], memoryFile, sortedMaterialPropertyVector);

				{ // Resources
					// Uniform buffers
					if (rapidJsonValueUniformBuffers.IsObject())
					{
						JsonMaterialBlueprintHelper::readUniformBuffers(input, rapidJsonValueUniformBuffers, memoryFile, shaderProperties);
					}

					// Texture buffers
					if (rapidJsonValueTextureBuffers.IsObject())
					{
						JsonMaterialBlueprintHelper::readTextureBuffers(rapidJsonValueTextureBuffers, memoryFile, shaderProperties);
					}

					// Sampler states
					if (rapidJsonValueSamplerStates.IsObject())
					{
						JsonMaterialBlueprintHelper::readSamplerStates(rapidJsonValueSamplerStates, memoryFile, shaderProperties, sortedMaterialPropertyVector);
					}

					// Textures
					if (rapidJsonValueTextures.IsObject())
					{
						JsonMaterialBlueprintHelper::readTextures(input, sortedMaterialPropertyVector, rapidJsonValueTextures, memoryFile, shaderProperties);
					}
				}
			}

			// Write LZ4 compressed output
			memoryFile.writeLz4CompressedDataToFile(RendererRuntime::v1MaterialBlueprint::FORMAT_TYPE, RendererRuntime::v1MaterialBlueprint::FORMAT_VERSION, outputAssetFilename, input.fileManager);

			// Store new cache entries or update existing ones
			input.cacheManager.storeOrUpdateCacheEntriesInDatabase(cacheEntries);
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/MaterialBlueprint/" + assetCategory + '/' + assetName;
			outputAsset(assetIdAsString, outputAssetFilename, outputAssetPackage);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
