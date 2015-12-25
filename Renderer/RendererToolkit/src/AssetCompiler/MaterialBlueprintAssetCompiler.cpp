/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintFileFormat.h>

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	namespace detail
	{
		uint32_t getCompiledAssetId(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonShaderBlueprintsObject, const std::string& propertyName)
		{
			const uint32_t sourceAssetId = static_cast<uint32_t>(std::atoi(jsonShaderBlueprintsObject->get(propertyName).convert<std::string>().c_str()));
			SourceAssetIdToCompiledAssetId::const_iterator iterator = input.sourceAssetIdToCompiledAssetId.find(sourceAssetId);
			const uint32_t compiledAssetId = (iterator != input.sourceAssetIdToCompiledAssetId.cend()) ? iterator->second : 0;
			// TODO(co) Error handling: Compiled asset ID not found (meaning invalid source asset ID given)
			return compiledAssetId;
		}
	}


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId MaterialBlueprintAssetCompiler::TYPE_ID("MaterialBlueprint");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialBlueprintAssetCompiler::MaterialBlueprintAssetCompiler()
	{
	}

	MaterialBlueprintAssetCompiler::~MaterialBlueprintAssetCompiler()
	{
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
		Poco::JSON::Object::Ptr		   jsonAssetRootObject	= configuration.jsonAssetRootObject;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		Poco::JSON::Object::Ptr jsonAssetObject = jsonAssetRootObject->get("Asset").extract<Poco::JSON::Object::Ptr>();

		// Read configuration
		// TODO(co) Add required properties
		std::string inputFile;
		uint32_t test = 0;
		{
			// Read material blueprint asset compiler configuration
			Poco::JSON::Object::Ptr jsonConfigurationObject = jsonAssetObject->get("MaterialBlueprintAssetCompiler").extract<Poco::JSON::Object::Ptr>();
			inputFile = jsonConfigurationObject->getValue<std::string>("InputFile");
			test	  = jsonConfigurationObject->optValue<uint32_t>("Test", test);
		}

		// Open the input file
		std::ifstream inputFileStream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetName");
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".material_blueprint";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		{ // Material blueprint
			// Parse JSON
			Poco::JSON::Parser jsonParser;
			jsonParser.parse(inputFileStream);
			Poco::JSON::Object::Ptr jsonRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();

			{ // Check whether or not the file format matches
				Poco::JSON::Object::Ptr jsonFormatObject = jsonRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
				if (jsonFormatObject->get("Type").convert<std::string>() != "MaterialBlueprintAsset")
				{
					throw std::exception("Invalid JSON format type, must be \"MaterialBlueprintAsset\"");
				}
				if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
				{
					throw std::exception("Invalid JSON format version, must be 1");
				}
			}

			Poco::JSON::Object::Ptr jsonMaterialBlueprintObject = jsonRootObject->get("MaterialBlueprintAsset").extract<Poco::JSON::Object::Ptr>();
			Poco::JSON::Object::Ptr jsonPropertiesObject = jsonMaterialBlueprintObject->get("Properties").extract<Poco::JSON::Object::Ptr>();
			Poco::JSON::Object::Ptr jsonPipelineStateObject = jsonMaterialBlueprintObject->get("PipelineState").extract<Poco::JSON::Object::Ptr>();

			{ // Material blueprint header
				RendererRuntime::v1MaterialBlueprint::Header materialBlueprintHeader;
				materialBlueprintHeader.formatType		= RendererRuntime::v1MaterialBlueprint::FORMAT_TYPE;
				materialBlueprintHeader.formatVersion	= RendererRuntime::v1MaterialBlueprint::FORMAT_VERSION;

				// Write down the material blueprint header
				outputFileStream.write(reinterpret_cast<const char*>(&materialBlueprintHeader), sizeof(RendererRuntime::v1MaterialBlueprint::Header));
			}

			{ // Pipeline state object (PSO)
				Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder();

				// TODO(co) Read in the rest of the PSO

				// TODO(co) The first few bytes are unused and there are probably byte alignment issues which can come up. On the other hand, this solution is wonderful simple.
				// Write down the pipeline state object (PSO)
				outputFileStream.write(reinterpret_cast<const char*>(&pipelineState), sizeof(Renderer::PipelineState));
			}

			{ // Shader blueprints
				Poco::JSON::Object::Ptr jsonShaderBlueprintsObject = jsonPipelineStateObject->get("ShaderBlueprints").extract<Poco::JSON::Object::Ptr>();

				RendererRuntime::v1MaterialBlueprint::ShaderBlueprints shaderBlueprints;
				shaderBlueprints.vertexShaderBlueprintAssetId				  = detail::getCompiledAssetId(input, jsonShaderBlueprintsObject, "VertexShaderBlueprintAssetId");
				shaderBlueprints.tessellationControlShaderBlueprintAssetId	  = 0; // TODO(co) Add shader type
				shaderBlueprints.tessellationEvaluationShaderBlueprintAssetId = 0; // TODO(co) Add shader type
				shaderBlueprints.geometryShaderBlueprintAssetId				  = 0; // TODO(co) Add shader type
				shaderBlueprints.fragmentShaderBlueprintAssetId				  = detail::getCompiledAssetId(input, jsonShaderBlueprintsObject, "FragmentShaderBlueprintAssetId");

				// Write down the shader blueprints
				outputFileStream.write(reinterpret_cast<const char*>(&shaderBlueprints), sizeof(RendererRuntime::v1MaterialBlueprint::ShaderBlueprints));
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetCategory");
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
