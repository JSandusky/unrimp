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

		void optionalBooleanProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, int& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				if ("FALSE" == valueAsString)
				{
					value = 0;
				}
				else if ("TRUE" == valueAsString)
				{
					value = 1;
				}
				else
				{
					// TODO(co) Error handling
				}
			}
		}

		void optionalIntegerProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, int& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();
				value = std::atoi(valueAsString.c_str());
			}
		}

		void optionalIntegerProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, unsigned int& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();
				value = static_cast<unsigned int>(std::atoi(valueAsString.c_str()));
			}
		}

		void optionalFloatProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, float& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();
				value = std::stof(valueAsString.c_str());
			}
		}

		void optionalFillModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::FillMode& fillMode)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				if ("WIREFRAME" == valueAsString)
				{
					fillMode = Renderer::FillMode::WIREFRAME;
				}
				else if ("SOLID" == valueAsString)
				{
					fillMode = Renderer::FillMode::SOLID;
				}
				else
				{
					// TODO(co) Error handling
				}
			}
		}

		void optionalCullModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::CullMode& cullMode)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				if ("NONE" == valueAsString)
				{
					cullMode = Renderer::CullMode::NONE;
				}
				else if ("FRONT" == valueAsString)
				{
					cullMode = Renderer::CullMode::FRONT;
				}
				else if ("BACK" == valueAsString)
				{
					cullMode = Renderer::CullMode::BACK;
				}
				else
				{
					// TODO(co) Error handling
				}
			}
		}

		void optionalConservativeRasterizationModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::ConservativeRasterizationMode& conservativeRasterizationMode)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				if ("OFF" == valueAsString)
				{
					conservativeRasterizationMode = Renderer::ConservativeRasterizationMode::OFF;
				}
				else if ("ON" == valueAsString)
				{
					conservativeRasterizationMode = Renderer::ConservativeRasterizationMode::ON;
				}
			}
		}

		void optionalDepthWriteMaskProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::DepthWriteMask& depthWriteMask)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				if ("ZERO" == valueAsString)
				{
					depthWriteMask = Renderer::DepthWriteMask::ZERO;
				}
				else if ("ALL" == valueAsString)
				{
					depthWriteMask = Renderer::DepthWriteMask::ALL;
				}
			}
		}

		void optionalBlendProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::Blend& blend)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				if ("ZERO" == valueAsString)
				{
					blend = Renderer::Blend::ZERO;
				}
				else if ("ONE" == valueAsString)
				{
					blend = Renderer::Blend::ONE;
				}
				else if ("SRC_COLOR" == valueAsString)
				{
					blend = Renderer::Blend::SRC_COLOR;
				}
				else if ("INV_SRC_COLOR" == valueAsString)
				{
					blend = Renderer::Blend::INV_SRC_COLOR;
				}
				else if ("SRC_ALPHA" == valueAsString)
				{
					blend = Renderer::Blend::SRC_ALPHA;
				}
				else if ("INV_SRC_ALPHA" == valueAsString)
				{
					blend = Renderer::Blend::INV_SRC_ALPHA;
				}
				else if ("DEST_ALPHA" == valueAsString)
				{
					blend = Renderer::Blend::DEST_ALPHA;
				}
				else if ("INV_DEST_ALPHA" == valueAsString)
				{
					blend = Renderer::Blend::INV_DEST_ALPHA;
				}
				else if ("DEST_COLOR" == valueAsString)
				{
					blend = Renderer::Blend::DEST_COLOR;
				}
				else if ("INV_DEST_COLOR" == valueAsString)
				{
					blend = Renderer::Blend::INV_DEST_COLOR;
				}
				else if ("SRC_ALPHA_SAT" == valueAsString)
				{
					blend = Renderer::Blend::SRC_ALPHA_SAT;
				}
				else if ("BLEND_FACTOR" == valueAsString)
				{
					blend = Renderer::Blend::BLEND_FACTOR;
				}
				else if ("INV_BLEND_FACTOR" == valueAsString)
				{
					blend = Renderer::Blend::INV_BLEND_FACTOR;
				}
				else if ("SRC_1_COLOR" == valueAsString)
				{
					blend = Renderer::Blend::SRC_1_COLOR;
				}
				else if ("INV_SRC_1_COLOR" == valueAsString)
				{
					blend = Renderer::Blend::INV_SRC_1_COLOR;
				}
				else if ("SRC_1_ALPHA" == valueAsString)
				{
					blend = Renderer::Blend::SRC_1_ALPHA;
				}
				else if ("INV_SRC_1_ALPHA" == valueAsString)
				{
					blend = Renderer::Blend::INV_SRC_1_ALPHA;
				}
			}
		}

		void optionalBlendOpProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::BlendOp& blendOp)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				if ("ADD" == valueAsString)
				{
					blendOp = Renderer::BlendOp::ADD;
				}
				else if ("SUBTRACT" == valueAsString)
				{
					blendOp = Renderer::BlendOp::SUBTRACT;
				}
				else if ("REV_SUBTRACT" == valueAsString)
				{
					blendOp = Renderer::BlendOp::REV_SUBTRACT;
				}
				else if ("MIN" == valueAsString)
				{
					blendOp = Renderer::BlendOp::MIN;
				}
				else if ("MAX" == valueAsString)
				{
					blendOp = Renderer::BlendOp::MAX;
				}
			}
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

			{ // Material blueprint header
				RendererRuntime::v1MaterialBlueprint::Header materialBlueprintHeader;
				materialBlueprintHeader.formatType		= RendererRuntime::v1MaterialBlueprint::FORMAT_TYPE;
				materialBlueprintHeader.formatVersion	= RendererRuntime::v1MaterialBlueprint::FORMAT_VERSION;

				// Write down the material blueprint header
				outputFileStream.write(reinterpret_cast<const char*>(&materialBlueprintHeader), sizeof(RendererRuntime::v1MaterialBlueprint::Header));
			}

			{ // Pipeline state object (PSO)
				Poco::JSON::Object::Ptr jsonPipelineStateObject = jsonMaterialBlueprintObject->get("PipelineState").extract<Poco::JSON::Object::Ptr>();

				// Start with the default settings
				Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder();

				{ // Optional rasterizer state
					Poco::Dynamic::Var jsonRasterizerStateDynamicVar = jsonPipelineStateObject->get("RasterizerState");
					if (!jsonRasterizerStateDynamicVar.isEmpty())
					{
						Poco::JSON::Object::Ptr jsonRasterizerStateObject = jsonRasterizerStateDynamicVar.extract<Poco::JSON::Object::Ptr>();
						Renderer::RasterizerState& rasterizerState = pipelineState.rasterizerState;

						// The optional properties
						detail::optionalFillModeProperty(jsonRasterizerStateObject, "FillMode", rasterizerState.fillMode);
						detail::optionalCullModeProperty(jsonRasterizerStateObject, "CullMode", rasterizerState.cullMode);
						detail::optionalBooleanProperty(jsonRasterizerStateObject, "FrontCounterClockwise", rasterizerState.frontCounterClockwise);
						detail::optionalIntegerProperty(jsonRasterizerStateObject, "DepthBias", rasterizerState.depthBias);
						detail::optionalFloatProperty(jsonRasterizerStateObject, "DepthBiasClamp", rasterizerState.depthBiasClamp);
						detail::optionalFloatProperty(jsonRasterizerStateObject, "SlopeScaledDepthBias", rasterizerState.slopeScaledDepthBias);
						detail::optionalBooleanProperty(jsonRasterizerStateObject, "DepthClipEnable", rasterizerState.depthClipEnable);
						detail::optionalBooleanProperty(jsonRasterizerStateObject, "MultisampleEnable", rasterizerState.multisampleEnable);
						detail::optionalBooleanProperty(jsonRasterizerStateObject, "AntialiasedLineEnable", rasterizerState.antialiasedLineEnable);
						detail::optionalIntegerProperty(jsonRasterizerStateObject, "ForcedSampleCount", rasterizerState.forcedSampleCount);
						detail::optionalConservativeRasterizationModeProperty(jsonRasterizerStateObject, "ConservativeRasterizationMode", rasterizerState.conservativeRasterizationMode);
						detail::optionalBooleanProperty(jsonRasterizerStateObject, "ScissorEnable", rasterizerState.scissorEnable);
					}
				}

				{ // Optional depth stencil state
					Poco::Dynamic::Var jsonDepthStencilStateDynamicVar = jsonPipelineStateObject->get("DepthStencilState");
					if (!jsonDepthStencilStateDynamicVar.isEmpty())
					{
						Poco::JSON::Object::Ptr jsonDepthStencilStateObject = jsonDepthStencilStateDynamicVar.extract<Poco::JSON::Object::Ptr>();
						Renderer::DepthStencilState& depthStencilState = pipelineState.depthStencilState;

						// The optional properties
						detail::optionalBooleanProperty(jsonDepthStencilStateObject, "DepthEnable", depthStencilState.depthEnable);
						detail::optionalDepthWriteMaskProperty(jsonDepthStencilStateObject, "DepthWriteMask", depthStencilState.depthWriteMask);

						// TODO(co) Depth stencil state: Read in the rest of the PSO
						/*
						"DepthStencilState":
						{
							"DepthFunc": "LESS",
							"StencilEnable": "FALSE",
							"StencilReadMask": "255",
							"StencilWriteMask": "255",
							"FrontFace":
							{
								"StencilFailOp": "KEEP",
								"StencilDepthFailOp": "KEEP",
								"StencilPassOp": "KEEP",
								"StencilFunc": "ALWAYS"
							},
							"BackFace":
							{
								"StencilFailOp": "KEEP",
								"StencilDepthFailOp": "KEEP",
								"StencilPassOp": "KEEP",
								"StencilFunc": "ALWAYS"
							}
						},
						*/
					}
				}

				{ // Optional blend state
					Poco::Dynamic::Var jsonBlendStateDynamicVar = jsonPipelineStateObject->get("BlendState");
					if (!jsonBlendStateDynamicVar.isEmpty())
					{
						Poco::JSON::Object::Ptr jsonBlendStateObject = jsonBlendStateDynamicVar.extract<Poco::JSON::Object::Ptr>();
						Renderer::BlendState& blendState = pipelineState.blendState;

						// The optional properties
						detail::optionalBooleanProperty(jsonBlendStateObject, "AlphaToCoverageEnable", blendState.alphaToCoverageEnable);
						detail::optionalBooleanProperty(jsonBlendStateObject, "IndependentBlendEnable", blendState.independentBlendEnable);

						// The optional render target properties
						for (int i = 0; i < 8; ++i)
						{
							Poco::Dynamic::Var jsonRenderTargetDynamicVar = jsonBlendStateObject->get("RenderTarget[" + std::to_string(i) + ']');
							if (!jsonRenderTargetDynamicVar.isEmpty())
							{
								Poco::JSON::Object::Ptr jsonRenderTargetObject = jsonRenderTargetDynamicVar.extract<Poco::JSON::Object::Ptr>();
								Renderer::RenderTargetBlendDesc& renderTargetBlendDesc = blendState.renderTarget[i];

								// The optional properties
								detail::optionalBooleanProperty(jsonRenderTargetObject, "BlendEnable", renderTargetBlendDesc.blendEnable);
								detail::optionalBlendProperty(jsonRenderTargetObject, "SrcBlend", renderTargetBlendDesc.srcBlend);
								detail::optionalBlendProperty(jsonRenderTargetObject, "DestBlend", renderTargetBlendDesc.destBlend);
								detail::optionalBlendOpProperty(jsonRenderTargetObject, "BlendOp", renderTargetBlendDesc.blendOp);
								detail::optionalBlendProperty(jsonRenderTargetObject, "SrcBlendAlpha", renderTargetBlendDesc.srcBlendAlpha);
								detail::optionalBlendProperty(jsonRenderTargetObject, "DestBlendAlpha", renderTargetBlendDesc.destBlendAlpha);
								detail::optionalBlendOpProperty(jsonRenderTargetObject, "BlendOpAlpha", renderTargetBlendDesc.blendOpAlpha);

								// TODO(co) Blend state: Read in the rest of the PSO
								/*
								"RenderTarget[0]":
								{

									"RenderTargetWriteMask": "ALL"
								},
								*/

							}
						}
					}
				}

				// TODO(co) The first few bytes are unused and there are probably byte alignment issues which can come up. On the other hand, this solution is wonderful simple.
				// Write down the pipeline state object (PSO)
				outputFileStream.write(reinterpret_cast<const char*>(&pipelineState), sizeof(Renderer::PipelineState));
			}

			{ // Shader blueprints
				Poco::JSON::Object::Ptr jsonShaderBlueprintsObject = jsonMaterialBlueprintObject->get("ShaderBlueprints").extract<Poco::JSON::Object::Ptr>();

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
