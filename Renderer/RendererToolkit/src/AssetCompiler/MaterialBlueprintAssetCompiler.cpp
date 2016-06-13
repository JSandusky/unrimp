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
#pragma warning(push)
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#pragma warning(disable: 4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#pragma warning(disable: 4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	#pragma warning(disable: 4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#pragma warning(disable: 4061)	// warning C4061: enumerator 'rapidjson::GenericReader<rapidjson::UTF8<char>,rapidjson::UTF8<char>,rapidjson::CrtAllocator>::IterativeParsingStartState' in switch of enum 'rapidjson::GenericReader<rapidjson::UTF8<char>,rapidjson::UTF8<char>,rapidjson::CrtAllocator>::IterativeParsingState' is not explicitly handled by a case label
	#include <rapidjson/document.h>
#pragma warning(pop)

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

			// Mandatory main sections of the material blueprint
			const rapidjson::Value& rapidJsonValueMaterialBlueprintAsset = rapidJsonDocument["MaterialBlueprintAsset"];
			const rapidjson::Value& rapidJsonValueProperties = rapidJsonValueMaterialBlueprintAsset["Properties"];
			const rapidjson::Value& rapidJsonValueResources = rapidJsonValueMaterialBlueprintAsset["Resources"];
			const rapidjson::Value& rapidJsonValueUniformBuffers = rapidJsonValueResources["UniformBuffers"];
			const rapidjson::Value& rapidJsonValueSamplerStates = rapidJsonValueResources["SamplerStates"];
			const rapidjson::Value& rapidJsonValueTextures = rapidJsonValueResources["Textures"];

			{ // Material blueprint header
				RendererRuntime::v1MaterialBlueprint::Header materialBlueprintHeader;
				materialBlueprintHeader.formatType			   = RendererRuntime::v1MaterialBlueprint::FORMAT_TYPE;
				materialBlueprintHeader.formatVersion		   = RendererRuntime::v1MaterialBlueprint::FORMAT_VERSION;
				materialBlueprintHeader.numberOfProperties	   = rapidJsonValueProperties.MemberCount();
				materialBlueprintHeader.numberOfUniformBuffers = rapidJsonValueUniformBuffers.MemberCount();
				materialBlueprintHeader.numberOfSamplerStates  = rapidJsonValueSamplerStates.MemberCount();
				materialBlueprintHeader.numberOfTextures	   = rapidJsonValueTextures.MemberCount();

				// Write down the material blueprint header
				outputFileStream.write(reinterpret_cast<const char*>(&materialBlueprintHeader), sizeof(RendererRuntime::v1MaterialBlueprint::Header));
			}

			// Properties
			RendererRuntime::MaterialProperties::SortedPropertyVector sortedMaterialPropertyVector;
			{
				// Gather all material properties
				JsonMaterialBlueprintHelper::readProperties(input, rapidJsonValueProperties, sortedMaterialPropertyVector);

				// Write down all material properties
				outputFileStream.write(reinterpret_cast<const char*>(sortedMaterialPropertyVector.data()), sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size());
			}

			// Root signature
			RendererRuntime::ShaderProperties shaderProperties;
			JsonMaterialBlueprintHelper::readRootSignature(rapidJsonValueMaterialBlueprintAsset["RootSignature"], outputFileStream, shaderProperties);

			// Pipeline state object (PSO)
			JsonMaterialBlueprintHelper::readPipelineStateObject(input, rapidJsonValueMaterialBlueprintAsset["PipelineState"], outputFileStream);

			{ // Resources
				// Uniform buffers
				JsonMaterialBlueprintHelper::readUniformBuffers(input, rapidJsonValueUniformBuffers, outputFileStream, shaderProperties);

				// Sampler states
				JsonMaterialBlueprintHelper::readSamplerStates(rapidJsonValueSamplerStates, outputFileStream, shaderProperties);

				// Textures
				JsonMaterialBlueprintHelper::readTextures(input, sortedMaterialPropertyVector, rapidJsonValueTextures, outputFileStream, shaderProperties);
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
