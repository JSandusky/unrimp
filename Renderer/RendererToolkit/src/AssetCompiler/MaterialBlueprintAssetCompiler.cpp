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
#include "RendererToolkit/AssetCompiler/MaterialBlueprintAssetCompiler.h"
#include "RendererToolkit/Helper/JsonHelper.h"
#include "RendererToolkit/Helper/JsonMaterialHelper.h"
#include "RendererToolkit/Helper/JsonMaterialBlueprintHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>
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
		// TODO(co) Add required properties
		std::string inputFile;
		{
			// Read material blueprint asset compiler configuration
			const rapidjson::Value& rapidJsonValueMaterialBlueprintAssetCompiler = rapidJsonValueAsset["MaterialBlueprintAssetCompiler"];
			inputFile = rapidJsonValueMaterialBlueprintAssetCompiler["InputFile"].GetString();
		}

		// Open the input file
		const std::string inputFilename = assetInputDirectory + inputFile;
		std::ifstream inputFileStream(inputFilename, std::ios::binary);
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".material_blueprint";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

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
				JsonMaterialBlueprintHelper::readProperties(input, rapidJsonValueProperties, sortedMaterialPropertyVector, visualImportanceOfShaderProperties, maximumIntegerValueOfShaderProperties);
			}

			{ // Material blueprint header
				RendererRuntime::v1MaterialBlueprint::Header materialBlueprintHeader;
				materialBlueprintHeader.formatType									= RendererRuntime::v1MaterialBlueprint::FORMAT_TYPE;
				materialBlueprintHeader.formatVersion								= RendererRuntime::v1MaterialBlueprint::FORMAT_VERSION;
				materialBlueprintHeader.numberOfProperties							= rapidJsonValueProperties.MemberCount();
				materialBlueprintHeader.numberOfShaderCombinationProperties			= static_cast<uint32_t>(visualImportanceOfShaderPropertiesVector.size());
				materialBlueprintHeader.numberOfIntegerShaderCombinationProperties	= static_cast<uint32_t>(maximumIntegerValueOfShaderPropertiesVector.size());	// Each integer shader combination property must have a defined maximum value
				materialBlueprintHeader.numberOfUniformBuffers						= rapidJsonValueUniformBuffers.IsObject() ? rapidJsonValueUniformBuffers.MemberCount() : 0;
				materialBlueprintHeader.numberOfTextureBuffers						= rapidJsonValueTextureBuffers.IsObject() ? rapidJsonValueTextureBuffers.MemberCount() : 0;
				materialBlueprintHeader.numberOfSamplerStates						= rapidJsonValueSamplerStates.IsObject() ? rapidJsonValueSamplerStates.MemberCount() : 0;
				materialBlueprintHeader.numberOfTextures							= rapidJsonValueTextures.IsObject() ? rapidJsonValueTextures.MemberCount() : 0;

				// Write down the material blueprint header
				outputFileStream.write(reinterpret_cast<const char*>(&materialBlueprintHeader), sizeof(RendererRuntime::v1MaterialBlueprint::Header));
			}

			// Write down all material properties
			outputFileStream.write(reinterpret_cast<const char*>(sortedMaterialPropertyVector.data()), sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size());

			// Write down visual importance of shader properties
			outputFileStream.write(reinterpret_cast<const char*>(visualImportanceOfShaderPropertiesVector.data()), sizeof(RendererRuntime::ShaderProperties::Property) * visualImportanceOfShaderPropertiesVector.size());

			// Write down maximum integer value of shader properties
			outputFileStream.write(reinterpret_cast<const char*>(maximumIntegerValueOfShaderPropertiesVector.data()), sizeof(RendererRuntime::ShaderProperties::Property) * maximumIntegerValueOfShaderPropertiesVector.size());

			// Root signature
			RendererRuntime::ShaderProperties shaderProperties;
			JsonMaterialBlueprintHelper::readRootSignature(rapidJsonValueMaterialBlueprintAsset["RootSignature"], outputFileStream, shaderProperties);

			// Pipeline state object (PSO)
			JsonMaterialBlueprintHelper::readPipelineStateObject(input, rapidJsonValueMaterialBlueprintAsset["PipelineState"], outputFileStream);

			{ // Resources
				// Uniform buffers
				if (rapidJsonValueUniformBuffers.IsObject())
				{
					JsonMaterialBlueprintHelper::readUniformBuffers(input, rapidJsonValueUniformBuffers, outputFileStream, shaderProperties);
				}

				// Texture buffers
				if (rapidJsonValueTextureBuffers.IsObject())
				{
					JsonMaterialBlueprintHelper::readTextureBuffers(rapidJsonValueTextureBuffers, outputFileStream, shaderProperties);
				}

				// Sampler states
				if (rapidJsonValueSamplerStates.IsObject())
				{
					JsonMaterialBlueprintHelper::readSamplerStates(rapidJsonValueSamplerStates, outputFileStream, shaderProperties);
				}

				// Textures
				if (rapidJsonValueTextures.IsObject())
				{
					JsonMaterialBlueprintHelper::readTextures(input, sortedMaterialPropertyVector, rapidJsonValueTextures, outputFileStream, shaderProperties);
				}
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/MaterialBlueprint/" + assetCategory + '/' + assetName;

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
