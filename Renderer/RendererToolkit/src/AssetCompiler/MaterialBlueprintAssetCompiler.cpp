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

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4251)	// warning C4251: 'Poco::StringTokenizer::_tokens': class 'std::vector<std::string,std::allocator<_Kty>>' needs to have dll-interface to be used by clients of class 'Poco::StringTokenizer'
	#include <Poco/StringTokenizer.h>
#pragma warning(pop)

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
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

		void optionalFloat4Property(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, float value[4])
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				Poco::StringTokenizer stringTokenizer(jsonDynamicVar.convert<std::string>(), " ");
				if (stringTokenizer.count() == 4)
				{
					for (size_t i = 0; i < 4; ++i)
					{
						value[i] = std::stof(stringTokenizer[i].c_str());
					}
				}
				else
				{
					// TODO(co) Error handling
				}
			}
		}

		void optionalStringProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, char* value, uint32_t maximumLength)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();
				const size_t valueLength = valueAsString.length();

				// +1 for the terminating zero
				if (valueLength + 1 <= maximumLength)
				{
					memcpy(value, valueAsString.data(), valueLength);
					value[valueLength] = '\0';
				}
				else
				{
					// TODO(co) Error handling
				}
			}
		}

		void optionalFillModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::FillMode& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::FillMode::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::FillMode::name;

				// Evaluate value
				IF_VALUE(WIREFRAME)
				ELSE_IF_VALUE(SOLID)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void optionalCullModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::CullMode& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::CullMode::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::CullMode::name;

				// Evaluate value
				IF_VALUE(NONE)
				ELSE_IF_VALUE(FRONT)
				ELSE_IF_VALUE(BACK)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void optionalConservativeRasterizationModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::ConservativeRasterizationMode& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::ConservativeRasterizationMode::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::ConservativeRasterizationMode::name;

				// Evaluate value
				IF_VALUE(OFF)
				ELSE_IF_VALUE(ON)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void optionalDepthWriteMaskProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::DepthWriteMask& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::DepthWriteMask::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::DepthWriteMask::name;

				// Evaluate value
				IF_VALUE(ZERO)
				ELSE_IF_VALUE(ALL)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void optionalBlendProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::Blend& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::Blend::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::Blend::name;

				// Evaluate value
				IF_VALUE(ZERO)
				ELSE_IF_VALUE(ONE)
				ELSE_IF_VALUE(SRC_COLOR)
				ELSE_IF_VALUE(INV_SRC_COLOR)
				ELSE_IF_VALUE(SRC_ALPHA)
				ELSE_IF_VALUE(INV_SRC_ALPHA)
				ELSE_IF_VALUE(DEST_ALPHA)
				ELSE_IF_VALUE(INV_DEST_ALPHA)
				ELSE_IF_VALUE(DEST_COLOR)
				ELSE_IF_VALUE(INV_DEST_COLOR)
				ELSE_IF_VALUE(SRC_ALPHA_SAT)
				ELSE_IF_VALUE(BLEND_FACTOR)
				ELSE_IF_VALUE(INV_BLEND_FACTOR)
				ELSE_IF_VALUE(SRC_1_COLOR)
				ELSE_IF_VALUE(INV_SRC_1_COLOR)
				ELSE_IF_VALUE(SRC_1_ALPHA)
				ELSE_IF_VALUE(INV_SRC_1_ALPHA)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void optionalBlendOpProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::BlendOp& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::BlendOp::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::BlendOp::name;

				// Evaluate value
				IF_VALUE(ADD)
				ELSE_IF_VALUE(SUBTRACT)
				ELSE_IF_VALUE(REV_SUBTRACT)
				ELSE_IF_VALUE(MIN)
				ELSE_IF_VALUE(MAX)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void optionalShaderVisibilityProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::ShaderVisibility& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::ShaderVisibility::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::ShaderVisibility::name;

				// Evaluate value
				IF_VALUE(ALL)
				ELSE_IF_VALUE(VERTEX)
				ELSE_IF_VALUE(TESSELLATION_CONTROL)
				ELSE_IF_VALUE(TESSELLATION_EVALUATION)
				ELSE_IF_VALUE(GEOMETRY)
				ELSE_IF_VALUE(FRAGMENT)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void optionalFilterProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::FilterMode& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::FilterMode::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::FilterMode::name;

				// Evaluate value
				IF_VALUE(MIN_MAG_MIP_POINT)
				ELSE_IF_VALUE(MIN_MAG_POINT_MIP_LINEAR)
				ELSE_IF_VALUE(MIN_POINT_MAG_LINEAR_MIP_POINT)
				ELSE_IF_VALUE(MIN_POINT_MAG_MIP_LINEAR)
				ELSE_IF_VALUE(MIN_LINEAR_MAG_MIP_POINT)
				ELSE_IF_VALUE(MIN_LINEAR_MAG_POINT_MIP_LINEAR)
				ELSE_IF_VALUE(MIN_MAG_LINEAR_MIP_POINT)
				ELSE_IF_VALUE(MIN_MAG_MIP_LINEAR)
				ELSE_IF_VALUE(ANISOTROPIC)
				ELSE_IF_VALUE(COMPARISON_MIN_MAG_MIP_POINT)
				ELSE_IF_VALUE(COMPARISON_MIN_MAG_POINT_MIP_LINEAR)
				ELSE_IF_VALUE(COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT)
				ELSE_IF_VALUE(COMPARISON_MIN_POINT_MAG_MIP_LINEAR)
				ELSE_IF_VALUE(COMPARISON_MIN_LINEAR_MAG_MIP_POINT)
				ELSE_IF_VALUE(COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR)
				ELSE_IF_VALUE(COMPARISON_MIN_MAG_LINEAR_MIP_POINT)
				ELSE_IF_VALUE(COMPARISON_MIN_MAG_MIP_LINEAR)
				ELSE_IF_VALUE(COMPARISON_ANISOTROPIC)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void optionalTextureAddressModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::TextureAddressMode& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::TextureAddressMode::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::TextureAddressMode::name;

				// Evaluate value
				IF_VALUE(WRAP)
				ELSE_IF_VALUE(MIRROR)
				ELSE_IF_VALUE(CLAMP)
				ELSE_IF_VALUE(BORDER)
				ELSE_IF_VALUE(MIRROR_ONCE)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void optionalComparisonFuncProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::ComparisonFunc& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::ComparisonFunc::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::ComparisonFunc::name;

				// Evaluate value
				IF_VALUE(NEVER)
				ELSE_IF_VALUE(LESS)
				ELSE_IF_VALUE(EQUAL)
				ELSE_IF_VALUE(LESS_EQUAL)
				ELSE_IF_VALUE(GREATER)
				ELSE_IF_VALUE(NOT_EQUAL)
				ELSE_IF_VALUE(GREATER_EQUAL)
				ELSE_IF_VALUE(ALWAYS)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void readRootSignature(Poco::JSON::Object::Ptr jsonRootSignatureObject, std::ofstream& outputFileStream)
		{
			// First: Collect everything we need instead of directly writing it down using an inefficient data layout
			// -> We don't care that "Renderer::RootDescriptorTable::descriptorRanges" has unused bogus content, makes loading the root signature much easier because there this content just has to be set
			std::vector<Renderer::RootParameter> rootParameters;
			std::vector<Renderer::DescriptorRange> descriptorRanges;
			{
				Poco::JSON::Object::Ptr jsonRootParametersObject = jsonRootSignatureObject->get("RootParameters").extract<Poco::JSON::Object::Ptr>();
				rootParameters.reserve(jsonRootParametersObject->size());
				descriptorRanges.reserve(jsonRootParametersObject->size());

				Poco::JSON::Object::ConstIterator rootParametersIterator = jsonRootParametersObject->begin();
				Poco::JSON::Object::ConstIterator rootParametersIteratorEnd = jsonRootParametersObject->end();
				while (rootParametersIterator != rootParametersIteratorEnd)
				{
					Poco::JSON::Object::Ptr jsonRootParameterObject = rootParametersIterator->second.extract<Poco::JSON::Object::Ptr>();

					// TODO(co) Add support for the other root parameter types
					const std::string rootParameterType = jsonRootParameterObject->getValue<std::string>("ParameterType");
					if ("DESCRIPTOR_TABLE" == rootParameterType)
					{
						Poco::JSON::Object::Ptr jsonDescriptorRangesObject = jsonRootParameterObject->get("DescriptorRanges").extract<Poco::JSON::Object::Ptr>();

						{ // Collect the root parameter
							Renderer::RootParameter rootParameter;
							rootParameter.parameterType = Renderer::RootParameterType::DESCRIPTOR_TABLE;
							rootParameter.descriptorTable.numberOfDescriptorRanges = jsonDescriptorRangesObject->size();
							optionalShaderVisibilityProperty(jsonRootParameterObject, "ShaderVisibility", rootParameter.shaderVisibility);
							rootParameters.push_back(rootParameter);
						}

						{ // Descriptor ranges
							Poco::JSON::Object::ConstIterator descriptorRangesIterator = jsonDescriptorRangesObject->begin();
							Poco::JSON::Object::ConstIterator descriptorRangesIteratorEnd = jsonDescriptorRangesObject->end();
							while (descriptorRangesIterator != descriptorRangesIteratorEnd)
							{
								Poco::JSON::Object::Ptr jsonDescriptorRangeObject = descriptorRangesIterator->second.extract<Poco::JSON::Object::Ptr>();
								Renderer::DescriptorRange descriptorRange;

								{ // Range type
									const std::string descriptorRangeType = jsonDescriptorRangeObject->getValue<std::string>("RangeType");
									if ("SRV" == descriptorRangeType)
									{
										descriptorRange.rangeType = Renderer::DescriptorRangeType::SRV;
									}
									else if ("UAV" == descriptorRangeType)
									{
										descriptorRange.rangeType = Renderer::DescriptorRangeType::UAV;
									}
									else if ("CBV" == descriptorRangeType)
									{
										descriptorRange.rangeType = Renderer::DescriptorRangeType::CBV;
									}
									else if ("SAMPLER" == descriptorRangeType)
									{
										descriptorRange.rangeType = Renderer::DescriptorRangeType::SAMPLER;
									}
									else
									{
										// TODO(co) Error handling
									}
								}

								// Optional number of descriptors
								descriptorRange.numberOfDescriptors = 1;
								optionalIntegerProperty(jsonDescriptorRangeObject, "NumberOfDescriptors", descriptorRange.numberOfDescriptors);

								// Optional base shader register
								descriptorRange.baseShaderRegister = 0;
								optionalIntegerProperty(jsonDescriptorRangeObject, "BaseShaderRegister", descriptorRange.baseShaderRegister);

								// Optional register space
								descriptorRange.registerSpace = 0;
								optionalIntegerProperty(jsonDescriptorRangeObject, "RegisterSpace", descriptorRange.registerSpace);

								// Optional offset in descriptors from table start
								descriptorRange.offsetInDescriptorsFromTableStart = 0;
								optionalIntegerProperty(jsonDescriptorRangeObject, "OffsetInDescriptorsFromTableStart", descriptorRange.offsetInDescriptorsFromTableStart);

								// Optional base shader register name
								descriptorRange.baseShaderRegisterName[0] = '\0';
								optionalStringProperty(jsonDescriptorRangeObject, "BaseShaderRegisterName", descriptorRange.baseShaderRegisterName, Renderer::DescriptorRange::NAME_LENGTH);

								// Optional sampler root parameter index
								descriptorRange.samplerRootParameterIndex = 0;
								optionalIntegerProperty(jsonDescriptorRangeObject, "SamplerRootParameterIndex", descriptorRange.samplerRootParameterIndex);

								// Collect the descriptor range
								descriptorRanges.push_back(descriptorRange);

								// Next root descriptor range, please
								++descriptorRangesIterator;
							}
						}
					}
					else
					{
						// TODO(co) Error handling
					}

					// Next root parameter, please
					++rootParametersIterator;
				}
			}

			// Now that we have collect everything we need, write it down

			{ // Write down the root signature header
				RendererRuntime::v1MaterialBlueprint::RootSignatureHeader rootSignatureHeader;
				rootSignatureHeader.numberOfRootParameters	 = rootParameters.size();
				rootSignatureHeader.numberOfDescriptorRanges = descriptorRanges.size();
				rootSignatureHeader.numberOfStaticSamplers	 = 0;									// TODO(co) Add support for static samplers
				rootSignatureHeader.flags					 = Renderer::RootSignatureFlags::NONE;	// TODO(co) Add support for flags
				outputFileStream.write(reinterpret_cast<const char*>(&rootSignatureHeader), sizeof(RendererRuntime::v1MaterialBlueprint::RootSignatureHeader));
			}

			// Write down the root parameters
			outputFileStream.write(reinterpret_cast<const char*>(rootParameters.data()), sizeof(Renderer::RootParameter) * rootParameters.size());

			// Write down the descriptor ranges
			outputFileStream.write(reinterpret_cast<const char*>(descriptorRanges.data()), sizeof(Renderer::DescriptorRange) * descriptorRanges.size());
		}

		void readPipelineStateObject(Poco::JSON::Object::Ptr jsonPipelineStateObject, std::ofstream& outputFileStream)
		{
			// Start with the default settings
			Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder();

			{ // Optional rasterizer state
				Poco::Dynamic::Var jsonRasterizerStateDynamicVar = jsonPipelineStateObject->get("RasterizerState");
				if (!jsonRasterizerStateDynamicVar.isEmpty())
				{
					Poco::JSON::Object::Ptr jsonRasterizerStateObject = jsonRasterizerStateDynamicVar.extract<Poco::JSON::Object::Ptr>();
					Renderer::RasterizerState& rasterizerState = pipelineState.rasterizerState;

					// The optional properties
					optionalFillModeProperty(jsonRasterizerStateObject, "FillMode", rasterizerState.fillMode);
					optionalCullModeProperty(jsonRasterizerStateObject, "CullMode", rasterizerState.cullMode);
					optionalBooleanProperty(jsonRasterizerStateObject, "FrontCounterClockwise", rasterizerState.frontCounterClockwise);
					optionalIntegerProperty(jsonRasterizerStateObject, "DepthBias", rasterizerState.depthBias);
					optionalFloatProperty(jsonRasterizerStateObject, "DepthBiasClamp", rasterizerState.depthBiasClamp);
					optionalFloatProperty(jsonRasterizerStateObject, "SlopeScaledDepthBias", rasterizerState.slopeScaledDepthBias);
					optionalBooleanProperty(jsonRasterizerStateObject, "DepthClipEnable", rasterizerState.depthClipEnable);
					optionalBooleanProperty(jsonRasterizerStateObject, "MultisampleEnable", rasterizerState.multisampleEnable);
					optionalBooleanProperty(jsonRasterizerStateObject, "AntialiasedLineEnable", rasterizerState.antialiasedLineEnable);
					optionalIntegerProperty(jsonRasterizerStateObject, "ForcedSampleCount", rasterizerState.forcedSampleCount);
					optionalConservativeRasterizationModeProperty(jsonRasterizerStateObject, "ConservativeRasterizationMode", rasterizerState.conservativeRasterizationMode);
					optionalBooleanProperty(jsonRasterizerStateObject, "ScissorEnable", rasterizerState.scissorEnable);
				}
			}

			{ // Optional depth stencil state
				Poco::Dynamic::Var jsonDepthStencilStateDynamicVar = jsonPipelineStateObject->get("DepthStencilState");
				if (!jsonDepthStencilStateDynamicVar.isEmpty())
				{
					Poco::JSON::Object::Ptr jsonDepthStencilStateObject = jsonDepthStencilStateDynamicVar.extract<Poco::JSON::Object::Ptr>();
					Renderer::DepthStencilState& depthStencilState = pipelineState.depthStencilState;

					// The optional properties
					optionalBooleanProperty(jsonDepthStencilStateObject, "DepthEnable", depthStencilState.depthEnable);
					optionalDepthWriteMaskProperty(jsonDepthStencilStateObject, "DepthWriteMask", depthStencilState.depthWriteMask);

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
					optionalBooleanProperty(jsonBlendStateObject, "AlphaToCoverageEnable", blendState.alphaToCoverageEnable);
					optionalBooleanProperty(jsonBlendStateObject, "IndependentBlendEnable", blendState.independentBlendEnable);

					// The optional render target properties
					for (int i = 0; i < 8; ++i)
					{
						Poco::Dynamic::Var jsonRenderTargetDynamicVar = jsonBlendStateObject->get("RenderTarget[" + std::to_string(i) + ']');
						if (!jsonRenderTargetDynamicVar.isEmpty())
						{
							Poco::JSON::Object::Ptr jsonRenderTargetObject = jsonRenderTargetDynamicVar.extract<Poco::JSON::Object::Ptr>();
							Renderer::RenderTargetBlendDesc& renderTargetBlendDesc = blendState.renderTarget[i];

							// The optional properties
							optionalBooleanProperty(jsonRenderTargetObject, "BlendEnable", renderTargetBlendDesc.blendEnable);
							optionalBlendProperty(jsonRenderTargetObject, "SrcBlend", renderTargetBlendDesc.srcBlend);
							optionalBlendProperty(jsonRenderTargetObject, "DestBlend", renderTargetBlendDesc.destBlend);
							optionalBlendOpProperty(jsonRenderTargetObject, "BlendOp", renderTargetBlendDesc.blendOp);
							optionalBlendProperty(jsonRenderTargetObject, "SrcBlendAlpha", renderTargetBlendDesc.srcBlendAlpha);
							optionalBlendProperty(jsonRenderTargetObject, "DestBlendAlpha", renderTargetBlendDesc.destBlendAlpha);
							optionalBlendOpProperty(jsonRenderTargetObject, "BlendOpAlpha", renderTargetBlendDesc.blendOpAlpha);

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

		void readSamplerStates(Poco::JSON::Object::Ptr jsonSamplerStatesObject, std::ofstream& outputFileStream)
		{
			Poco::JSON::Object::ConstIterator rootSamplerStatesIterator = jsonSamplerStatesObject->begin();
			Poco::JSON::Object::ConstIterator rootSamplerStatesIteratorEnd = jsonSamplerStatesObject->end();
			while (rootSamplerStatesIterator != rootSamplerStatesIteratorEnd)
			{
				Poco::JSON::Object::Ptr jsonSamplerStateObject = rootSamplerStatesIterator->second.extract<Poco::JSON::Object::Ptr>();

				// Start with the default sampler state
				RendererRuntime::v1MaterialBlueprint::SamplerState materialBlueprintSamplerState;
				materialBlueprintSamplerState.samplerRootParameterIndex = 0;
				Renderer::SamplerState& samplerState = materialBlueprintSamplerState;
				samplerState = Renderer::ISamplerState::getDefaultSamplerState();

				// The optional properties
				optionalIntegerProperty(jsonSamplerStateObject, "SamplerRootParameterIndex", materialBlueprintSamplerState.samplerRootParameterIndex);
				optionalFilterProperty(jsonSamplerStateObject, "Filter", samplerState.filter);
				optionalTextureAddressModeProperty(jsonSamplerStateObject, "AddressU", samplerState.addressU);
				optionalTextureAddressModeProperty(jsonSamplerStateObject, "AddressV", samplerState.addressV);
				optionalTextureAddressModeProperty(jsonSamplerStateObject, "AddressW", samplerState.addressW);
				optionalFloatProperty(jsonSamplerStateObject, "MipLODBias", samplerState.mipLODBias);
				optionalIntegerProperty(jsonSamplerStateObject, "MaxAnisotropy", samplerState.maxAnisotropy);
				optionalComparisonFuncProperty(jsonSamplerStateObject, "ComparisonFunc", samplerState.comparisonFunc);
				optionalFloat4Property(jsonSamplerStateObject, "BorderColor", samplerState.borderColor);
				optionalFloatProperty(jsonSamplerStateObject, "MinLOD", samplerState.minLOD);
				optionalFloatProperty(jsonSamplerStateObject, "MaxLOD", samplerState.maxLOD);

				// Write down the sampler state
				outputFileStream.write(reinterpret_cast<const char*>(&materialBlueprintSamplerState), sizeof(RendererRuntime::v1MaterialBlueprint::SamplerState));

				// Next sampler state, please
				++rootSamplerStatesIterator;
			}
		}

		void readTextures(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonTexturesObject, std::ofstream& outputFileStream)
		{
			Poco::JSON::Object::ConstIterator rootTexturesIterator = jsonTexturesObject->begin();
			Poco::JSON::Object::ConstIterator rootTexturesIteratorEnd = jsonTexturesObject->end();
			while (rootTexturesIterator != rootTexturesIteratorEnd)
			{
				Poco::JSON::Object::Ptr jsonTextureObject = rootTexturesIterator->second.extract<Poco::JSON::Object::Ptr>();

				// Start with the default texture
				RendererRuntime::v1MaterialBlueprint::Texture materialBlueprintTexture;
				materialBlueprintTexture.textureRootParameterIndex = 0;

				// The optional properties
				optionalIntegerProperty(jsonTextureObject, "TextureRootParameterIndex", materialBlueprintTexture.textureRootParameterIndex);

				// The mandatory properties
				materialBlueprintTexture.textureAssetId = detail::getCompiledAssetId(input, jsonTextureObject, "TextureAssetId");

				// Write down the texture
				outputFileStream.write(reinterpret_cast<const char*>(&materialBlueprintTexture), sizeof(RendererRuntime::v1MaterialBlueprint::Texture));

				// Next texture, please
				++rootTexturesIterator;
			}
		}


	//[-------------------------------------------------------]
	//[ Namespace                                             ]
	//[-------------------------------------------------------]
	} // detail


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
			Poco::JSON::Object::Ptr jsonSamplerStatesObject = jsonMaterialBlueprintObject->get("SamplerStates").extract<Poco::JSON::Object::Ptr>();
			Poco::JSON::Object::Ptr jsonTexturesObject = jsonMaterialBlueprintObject->get("Textures").extract<Poco::JSON::Object::Ptr>();

			{ // Material blueprint header
				RendererRuntime::v1MaterialBlueprint::Header materialBlueprintHeader;
				materialBlueprintHeader.formatType			  = RendererRuntime::v1MaterialBlueprint::FORMAT_TYPE;
				materialBlueprintHeader.formatVersion		  = RendererRuntime::v1MaterialBlueprint::FORMAT_VERSION;
				materialBlueprintHeader.numberOfParameters	  = jsonPropertiesObject->size();
				materialBlueprintHeader.numberOfSamplerStates = jsonSamplerStatesObject->size();
				materialBlueprintHeader.numberOfTextures	  = jsonTexturesObject->size();

				// Write down the material blueprint header
				outputFileStream.write(reinterpret_cast<const char*>(&materialBlueprintHeader), sizeof(RendererRuntime::v1MaterialBlueprint::Header));
			}

			// Root signature
			detail::readRootSignature(jsonMaterialBlueprintObject->get("RootSignature").extract<Poco::JSON::Object::Ptr>(), outputFileStream);

			// Pipeline state object (PSO)
			detail::readPipelineStateObject(jsonMaterialBlueprintObject->get("PipelineState").extract<Poco::JSON::Object::Ptr>(), outputFileStream);

			// Sampler states
			detail::readSamplerStates(jsonSamplerStatesObject, outputFileStream);

			// Textures
			detail::readTextures(input, jsonTexturesObject, outputFileStream);

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
