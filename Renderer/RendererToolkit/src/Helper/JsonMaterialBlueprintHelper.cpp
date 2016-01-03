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
#include "RendererToolkit/Helper/JsonMaterialBlueprintHelper.h"
#include "RendererToolkit/Helper/JsonMaterialHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintFileFormat.h>

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
		bool orderByMaterialPropertyId(const RendererRuntime::MaterialProperty& left, const RendererRuntime::MaterialProperty& right)
		{
			return (left.getMaterialPropertyId() < right.getMaterialPropertyId());
		}

		void optionalUniformBufferUsageProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, RendererRuntime::MaterialBlueprintResource::UniformBufferUsage& value)
		{
			Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
			if (!jsonDynamicVar.isEmpty())
			{
				const std::string valueAsString = jsonDynamicVar.convert<std::string>();

				// Define helper macros
				#define IF_VALUE(name)			 if (#name == valueAsString) value = RendererRuntime::MaterialBlueprintResource::UniformBufferUsage::name;
				#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = RendererRuntime::MaterialBlueprintResource::UniformBufferUsage::name;

				// Evaluate value
				IF_VALUE(UNKNOWN)
				ELSE_IF_VALUE(PASS)
				ELSE_IF_VALUE(MATERIAL)
				ELSE_IF_VALUE(INSTANCE)
				else
				{
					// TODO(co) Error handling
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
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
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	void JsonMaterialBlueprintHelper::optionalShaderVisibilityProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::ShaderVisibility& value)
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

	RendererRuntime::MaterialProperty::Usage JsonMaterialBlueprintHelper::mandatoryMaterialPropertyUsage(Poco::JSON::Object::Ptr jsonObject)
	{
		const std::string valueAsString = jsonObject->get("Usage").convert<std::string>();
		RendererRuntime::MaterialProperty::Usage usage = RendererRuntime::MaterialProperty::Usage::UNKNOWN;

		// Define helper macros
		#define IF_VALUE(name)			 if (#name == valueAsString) usage = RendererRuntime::MaterialProperty::Usage::name;
		#define ELSE_IF_VALUE(name) else if (#name == valueAsString) usage = RendererRuntime::MaterialProperty::Usage::name;

		// Evaluate value
		IF_VALUE(UNKNOWN)
		ELSE_IF_VALUE(STATIC)
		ELSE_IF_VALUE(DYNAMIC)
		ELSE_IF_VALUE(RASTERIZER_STATE)
		ELSE_IF_VALUE(DEPTH_STENCIL_STATE)
		ELSE_IF_VALUE(BLEND_STATE)
		ELSE_IF_VALUE(SAMPLER_STATE)
		ELSE_IF_VALUE(TEXTURE_REFERENCE)
		ELSE_IF_VALUE(GLOBAL_REFERENCE)
		ELSE_IF_VALUE(UNKNOWN_REFERENCE)
		ELSE_IF_VALUE(PASS_REFERENCE)
		ELSE_IF_VALUE(MATERIAL_REFERENCE)
		ELSE_IF_VALUE(INSTANCE_REFERENCE)
		else
		{
			// TODO(co) Error handling
		}

		// Undefine helper macros
		#undef IF_VALUE
		#undef ELSE_IF_VALUE

		// Done
		return usage;
	}

	RendererRuntime::MaterialProperty::ValueType JsonMaterialBlueprintHelper::mandatoryMaterialPropertyValueType(Poco::JSON::Object::Ptr jsonObject)
	{
		RendererRuntime::MaterialProperty::ValueType valueType = RendererRuntime::MaterialProperty::ValueType::UNKNOWN;
		const std::string valueAsString = jsonObject->get("ValueType").convert<std::string>();

		// Define helper macros
		#define IF_VALUE(name)			 if (#name == valueAsString) valueType = RendererRuntime::MaterialProperty::ValueType::name;
		#define ELSE_IF_VALUE(name) else if (#name == valueAsString) valueType = RendererRuntime::MaterialProperty::ValueType::name;

		// Evaluate value
		IF_VALUE(UNKNOWN)
		ELSE_IF_VALUE(BOOLEAN)
		ELSE_IF_VALUE(INTEGER)
		ELSE_IF_VALUE(INTEGER_2)
		ELSE_IF_VALUE(INTEGER_3)
		ELSE_IF_VALUE(INTEGER_4)
		ELSE_IF_VALUE(FLOAT)
		ELSE_IF_VALUE(FLOAT_2)
		ELSE_IF_VALUE(FLOAT_3)
		ELSE_IF_VALUE(FLOAT_4)
		ELSE_IF_VALUE(FLOAT_3_3)
		ELSE_IF_VALUE(FLOAT_4_4)
		ELSE_IF_VALUE(FILL_MODE)
		ELSE_IF_VALUE(CULL_MODE)
		ELSE_IF_VALUE(CONSERVATIVE_RASTERIZATION_MODE)
		ELSE_IF_VALUE(DEPTH_WRITE_MASK)
		ELSE_IF_VALUE(STENCIL_OP)
		ELSE_IF_VALUE(COMPARISON_FUNC)
		ELSE_IF_VALUE(BLEND)
		ELSE_IF_VALUE(BLEND_OP)
		ELSE_IF_VALUE(FILTER_MODE)
		ELSE_IF_VALUE(TEXTURE_ADDRESS_MODE)
		ELSE_IF_VALUE(ASSET_ID)
		else
		{
			// TODO(co) Error handling
		}

		// Undefine helper macros
		#undef IF_VALUE
		#undef ELSE_IF_VALUE

		// Done
		return valueType;
	}

	RendererRuntime::MaterialPropertyValue JsonMaterialBlueprintHelper::mandatoryMaterialPropertyValue(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, const RendererRuntime::MaterialProperty::ValueType valueType)
	{
		// Get the material property default value
		switch (valueType)
		{
			case RendererRuntime::MaterialPropertyValue::ValueType::UNKNOWN:
			{
				// TODO(co) Error, unknown is nothing which is valid when read from assets
				return RendererRuntime::MaterialPropertyValue::fromBoolean(false);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::BOOLEAN:
			{
				int value = 0;
				JsonHelper::optionalBooleanProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromBoolean(0 != value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER:
			{
				int value = 0;
				JsonHelper::optionalIntegerProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromInteger(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_2:
			{
				int values[2] = { 0, 0 };
				JsonHelper::optionalIntegerNProperty(jsonObject, propertyName, values, 2);
				return RendererRuntime::MaterialPropertyValue::fromInteger2(values[0], values[1]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_3:
			{
				int values[3] = { 0, 0, 0 };
				JsonHelper::optionalIntegerNProperty(jsonObject, propertyName, values, 3);
				return RendererRuntime::MaterialPropertyValue::fromInteger3(values[0], values[1], values[2]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_4:
			{
				int values[4] = { 0, 0, 0, 0 };
				JsonHelper::optionalIntegerNProperty(jsonObject, propertyName, values, 4);
				return RendererRuntime::MaterialPropertyValue::fromInteger4(values[0], values[1], values[2], values[3]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT:
			{
				float value = 0.0f;
				JsonHelper::optionalFloatProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromFloat(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_2:
			{
				float values[2] = { 0.0f, 0.0f };
				JsonHelper::optionalFloatNProperty(jsonObject, propertyName, values, 2);
				return RendererRuntime::MaterialPropertyValue::fromFloat2(values[0], values[1]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_3:
			{
				float values[3] = { 0.0f, 0.0f, 0.0f };
				JsonHelper::optionalFloatNProperty(jsonObject, propertyName, values, 3);
				return RendererRuntime::MaterialPropertyValue::fromFloat3(values[0], values[1], values[2]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_4:
			{
				float values[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				JsonHelper::optionalFloatNProperty(jsonObject, propertyName, values, 4);
				return RendererRuntime::MaterialPropertyValue::fromFloat4(values[0], values[1], values[2], values[3]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_3_3:
			{
				// Declaration property only
				return RendererRuntime::MaterialPropertyValue::fromFloat3_3();
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_4_4:
			{
				// Declaration property only
				return RendererRuntime::MaterialPropertyValue::fromFloat4_4();
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FILL_MODE:
			{
				Renderer::FillMode value = Renderer::FillMode::SOLID;
				JsonMaterialHelper::optionalFillModeProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromFillMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::CULL_MODE:
			{
				Renderer::CullMode value = Renderer::CullMode::BACK;
				JsonMaterialHelper::optionalCullModeProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromCullMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::CONSERVATIVE_RASTERIZATION_MODE:
			{
				Renderer::ConservativeRasterizationMode value = Renderer::ConservativeRasterizationMode::OFF;
				JsonMaterialHelper::optionalConservativeRasterizationModeProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromConservativeRasterizationMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::DEPTH_WRITE_MASK:
			{
				Renderer::DepthWriteMask value = Renderer::DepthWriteMask::ALL;
				JsonMaterialHelper::optionalDepthWriteMaskProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromDepthWriteMask(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::STENCIL_OP:
			{
				Renderer::StencilOp value = Renderer::StencilOp::KEEP;
				JsonMaterialHelper::optionalStencilOpProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromStencilOp(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::COMPARISON_FUNC:
			{
				Renderer::ComparisonFunc value = Renderer::ComparisonFunc::LESS;
				JsonMaterialHelper::optionalComparisonFuncProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromComparisonFunc(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::BLEND:
			{
				Renderer::Blend value = Renderer::Blend::ONE;
				JsonMaterialHelper::optionalBlendProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromBlend(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::BLEND_OP:
			{
				Renderer::BlendOp value = Renderer::BlendOp::ADD;
				JsonMaterialHelper::optionalBlendOpProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromBlendOp(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FILTER_MODE:
			{
				Renderer::FilterMode value = Renderer::FilterMode::MIN_MAG_MIP_LINEAR;
				JsonMaterialHelper::optionalFilterProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromFilterMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::TEXTURE_ADDRESS_MODE:
			{
				Renderer::TextureAddressMode value = Renderer::TextureAddressMode::CLAMP;
				JsonMaterialHelper::optionalTextureAddressModeProperty(jsonObject, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromTextureAddressMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::ASSET_ID:
			{
				return RendererRuntime::MaterialPropertyValue::fromAssetId(JsonHelper::getCompiledAssetId(input, jsonObject, propertyName));
			}
		}

		// TODO(co) Error, we should never ever end up in here
		return RendererRuntime::MaterialPropertyValue::fromBoolean(false);
	}

	void JsonMaterialBlueprintHelper::readRootSignature(Poco::JSON::Object::Ptr jsonRootSignatureObject, std::ofstream& outputFileStream)
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
							JsonHelper::optionalIntegerProperty(jsonDescriptorRangeObject, "NumberOfDescriptors", descriptorRange.numberOfDescriptors);

							// Optional base shader register
							descriptorRange.baseShaderRegister = 0;
							JsonHelper::optionalIntegerProperty(jsonDescriptorRangeObject, "BaseShaderRegister", descriptorRange.baseShaderRegister);

							// Optional register space
							descriptorRange.registerSpace = 0;
							JsonHelper::optionalIntegerProperty(jsonDescriptorRangeObject, "RegisterSpace", descriptorRange.registerSpace);

							// Optional offset in descriptors from table start
							descriptorRange.offsetInDescriptorsFromTableStart = 0;
							JsonHelper::optionalIntegerProperty(jsonDescriptorRangeObject, "OffsetInDescriptorsFromTableStart", descriptorRange.offsetInDescriptorsFromTableStart);

							// Optional base shader register name
							descriptorRange.baseShaderRegisterName[0] = '\0';
							JsonHelper::optionalStringProperty(jsonDescriptorRangeObject, "BaseShaderRegisterName", descriptorRange.baseShaderRegisterName, Renderer::DescriptorRange::NAME_LENGTH);

							// Optional sampler root parameter index
							descriptorRange.samplerRootParameterIndex = 0;
							JsonHelper::optionalIntegerProperty(jsonDescriptorRangeObject, "SamplerRootParameterIndex", descriptorRange.samplerRootParameterIndex);

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

	void JsonMaterialBlueprintHelper::readProperties(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonPropertiesObject, RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector, bool sort)
	{
		Poco::JSON::Object::ConstIterator propertiesIterator = jsonPropertiesObject->begin();
		Poco::JSON::Object::ConstIterator propertiesIteratorEnd = jsonPropertiesObject->end();
		while (propertiesIterator != propertiesIteratorEnd)
		{
			Poco::JSON::Object::Ptr jsonPropertyObject = propertiesIterator->second.extract<Poco::JSON::Object::Ptr>();

			// Material property ID
			const RendererRuntime::MaterialPropertyId materialPropertyId(propertiesIterator->first.c_str());

			// Material property usage
			const RendererRuntime::MaterialProperty::Usage usage = mandatoryMaterialPropertyUsage(jsonPropertyObject);
			const RendererRuntime::MaterialProperty::ValueType valueType = mandatoryMaterialPropertyValueType(jsonPropertyObject);
			if (RendererRuntime::MaterialProperty::Usage::TEXTURE_REFERENCE != usage && RendererRuntime::MaterialProperty::isReferenceUsage(usage))
			{
				// Get the reference value as string
				static const uint32_t NAME_LENGTH = 128;
				char referenceAsString[NAME_LENGTH];
				memset(&referenceAsString[0], 0, sizeof(char) * NAME_LENGTH);
				JsonHelper::optionalStringProperty(jsonPropertyObject, "Value", referenceAsString, NAME_LENGTH);

				// The character "@" is used to reference e.g. a material property value
				if (referenceAsString[0] == '@')
				{
					// Write down the material property
					const RendererRuntime::StringId referenceAsInteger(&referenceAsString[1]);
					sortedMaterialPropertyVector.emplace_back(RendererRuntime::MaterialProperty(materialPropertyId, usage, RendererRuntime::MaterialProperty::materialPropertyValueFromReference(valueType, referenceAsInteger)));
				}
				else
				{
					// TODO(co) Error handling
				}
			}
			else
			{
				// Write down the material property
				sortedMaterialPropertyVector.emplace_back(RendererRuntime::MaterialProperty(materialPropertyId, usage, mandatoryMaterialPropertyValue(input, jsonPropertyObject, "Value", valueType)));
			}

			// Next property, please
			++propertiesIterator;
		}

		// Ensure the material properties are sorted, if requested
		if (sort)
		{
			std::sort(sortedMaterialPropertyVector.begin(), sortedMaterialPropertyVector.end(), detail::orderByMaterialPropertyId);
		}
	}

	void JsonMaterialBlueprintHelper::readPipelineStateObject(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonPipelineStateObject, std::ofstream& outputFileStream)
	{
		{ // Shader blueprints
			Poco::JSON::Object::Ptr jsonShaderBlueprintsObject = jsonPipelineStateObject->get("ShaderBlueprints").extract<Poco::JSON::Object::Ptr>();

			RendererRuntime::v1MaterialBlueprint::ShaderBlueprints shaderBlueprints;
			shaderBlueprints.vertexShaderBlueprintAssetId				  = JsonHelper::getCompiledAssetId(input, jsonShaderBlueprintsObject, "VertexShaderBlueprintAssetId");
			shaderBlueprints.tessellationControlShaderBlueprintAssetId	  = 0; // TODO(co) Add shader type
			shaderBlueprints.tessellationEvaluationShaderBlueprintAssetId = 0; // TODO(co) Add shader type
			shaderBlueprints.geometryShaderBlueprintAssetId				  = 0; // TODO(co) Add shader type
			shaderBlueprints.fragmentShaderBlueprintAssetId				  = JsonHelper::getCompiledAssetId(input, jsonShaderBlueprintsObject, "FragmentShaderBlueprintAssetId");

			// Write down the shader blueprints
			outputFileStream.write(reinterpret_cast<const char*>(&shaderBlueprints), sizeof(RendererRuntime::v1MaterialBlueprint::ShaderBlueprints));
		}

		// Start with the default settings
		Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder();

		{ // Optional rasterizer state
			Poco::Dynamic::Var jsonRasterizerStateDynamicVar = jsonPipelineStateObject->get("RasterizerState");
			if (!jsonRasterizerStateDynamicVar.isEmpty())
			{
				Poco::JSON::Object::Ptr jsonRasterizerStateObject = jsonRasterizerStateDynamicVar.extract<Poco::JSON::Object::Ptr>();
				Renderer::RasterizerState& rasterizerState = pipelineState.rasterizerState;

				// The optional properties
				JsonMaterialHelper::optionalFillModeProperty(jsonRasterizerStateObject, "FillMode", rasterizerState.fillMode);
				JsonMaterialHelper::optionalCullModeProperty(jsonRasterizerStateObject, "CullMode", rasterizerState.cullMode);
				JsonHelper::optionalBooleanProperty(jsonRasterizerStateObject, "FrontCounterClockwise", rasterizerState.frontCounterClockwise);
				JsonHelper::optionalIntegerProperty(jsonRasterizerStateObject, "DepthBias", rasterizerState.depthBias);
				JsonHelper::optionalFloatProperty(jsonRasterizerStateObject, "DepthBiasClamp", rasterizerState.depthBiasClamp);
				JsonHelper::optionalFloatProperty(jsonRasterizerStateObject, "SlopeScaledDepthBias", rasterizerState.slopeScaledDepthBias);
				JsonHelper::optionalBooleanProperty(jsonRasterizerStateObject, "DepthClipEnable", rasterizerState.depthClipEnable);
				JsonHelper::optionalBooleanProperty(jsonRasterizerStateObject, "MultisampleEnable", rasterizerState.multisampleEnable);
				JsonHelper::optionalBooleanProperty(jsonRasterizerStateObject, "AntialiasedLineEnable", rasterizerState.antialiasedLineEnable);
				JsonHelper::optionalIntegerProperty(jsonRasterizerStateObject, "ForcedSampleCount", rasterizerState.forcedSampleCount);
				JsonMaterialHelper::optionalConservativeRasterizationModeProperty(jsonRasterizerStateObject, "ConservativeRasterizationMode", rasterizerState.conservativeRasterizationMode);
				JsonHelper::optionalBooleanProperty(jsonRasterizerStateObject, "ScissorEnable", rasterizerState.scissorEnable);
			}
		}

		{ // Optional depth stencil state
			Poco::Dynamic::Var jsonDepthStencilStateDynamicVar = jsonPipelineStateObject->get("DepthStencilState");
			if (!jsonDepthStencilStateDynamicVar.isEmpty())
			{
				Poco::JSON::Object::Ptr jsonDepthStencilStateObject = jsonDepthStencilStateDynamicVar.extract<Poco::JSON::Object::Ptr>();
				Renderer::DepthStencilState& depthStencilState = pipelineState.depthStencilState;

				// The optional properties
				JsonHelper::optionalBooleanProperty(jsonDepthStencilStateObject, "DepthEnable", depthStencilState.depthEnable);
				JsonMaterialHelper::optionalDepthWriteMaskProperty(jsonDepthStencilStateObject, "DepthWriteMask", depthStencilState.depthWriteMask);

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
				JsonHelper::optionalBooleanProperty(jsonBlendStateObject, "AlphaToCoverageEnable", blendState.alphaToCoverageEnable);
				JsonHelper::optionalBooleanProperty(jsonBlendStateObject, "IndependentBlendEnable", blendState.independentBlendEnable);

				// The optional render target properties
				for (int i = 0; i < 8; ++i)
				{
					Poco::Dynamic::Var jsonRenderTargetDynamicVar = jsonBlendStateObject->get("RenderTarget[" + std::to_string(i) + ']');
					if (!jsonRenderTargetDynamicVar.isEmpty())
					{
						Poco::JSON::Object::Ptr jsonRenderTargetObject = jsonRenderTargetDynamicVar.extract<Poco::JSON::Object::Ptr>();
						Renderer::RenderTargetBlendDesc& renderTargetBlendDesc = blendState.renderTarget[i];

						// The optional properties
						JsonHelper::optionalBooleanProperty(jsonRenderTargetObject, "BlendEnable", renderTargetBlendDesc.blendEnable);
						JsonMaterialHelper::optionalBlendProperty(jsonRenderTargetObject, "SrcBlend", renderTargetBlendDesc.srcBlend);
						JsonMaterialHelper::optionalBlendProperty(jsonRenderTargetObject, "DestBlend", renderTargetBlendDesc.destBlend);
						JsonMaterialHelper::optionalBlendOpProperty(jsonRenderTargetObject, "BlendOp", renderTargetBlendDesc.blendOp);
						JsonMaterialHelper::optionalBlendProperty(jsonRenderTargetObject, "SrcBlendAlpha", renderTargetBlendDesc.srcBlendAlpha);
						JsonMaterialHelper::optionalBlendProperty(jsonRenderTargetObject, "DestBlendAlpha", renderTargetBlendDesc.destBlendAlpha);
						JsonMaterialHelper::optionalBlendOpProperty(jsonRenderTargetObject, "BlendOpAlpha", renderTargetBlendDesc.blendOpAlpha);

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

	void JsonMaterialBlueprintHelper::readUniformBuffers(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonUniformBuffersObject, std::ofstream& outputFileStream)
	{
		Poco::JSON::Object::ConstIterator rootUniformBuffersIterator = jsonUniformBuffersObject->begin();
		Poco::JSON::Object::ConstIterator rootUniformBuffersIteratorEnd = jsonUniformBuffersObject->end();
		while (rootUniformBuffersIterator != rootUniformBuffersIteratorEnd)
		{
			Poco::JSON::Object::Ptr jsonUniformBufferObject = rootUniformBuffersIterator->second.extract<Poco::JSON::Object::Ptr>();
			Poco::JSON::Object::Ptr jsonElementPropertiesObject = jsonUniformBufferObject->get("ElementProperties").extract<Poco::JSON::Object::Ptr>();

			// Gather all element properties, don't sort because the user defined order is important in here (data layout in memory)
			RendererRuntime::MaterialProperties::SortedPropertyVector elementProperties;
			readProperties(input, jsonElementPropertiesObject, elementProperties, false);

			// Sum up the number of bytes required by all uniform buffer element properties
			uint32_t numberOfPackageBytes = 0;
			uint32_t numberOfBytesPerElement = 0;
			for (size_t i = 0; i < elementProperties.size(); ++i)
			{
				// Get value type number of bytes
				const uint32_t valueTypeNumberOfBytes = RendererRuntime::MaterialPropertyValue::getValueTypeNumberOfBytes(elementProperties[i].getValueType());
				numberOfBytesPerElement += valueTypeNumberOfBytes;

				// Handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
				if (0 != numberOfPackageBytes && numberOfPackageBytes + valueTypeNumberOfBytes > 16)
				{
					// Take the wasted bytes due to aligned packaging into account and restart the package bytes counter
					numberOfBytesPerElement += 4 * 4 - numberOfPackageBytes;
					numberOfPackageBytes = 0;

					// TODO(co) Profiling information: We could provide the material blueprint resource writer with information how many bytes get wasted with the defined layout
				}
				numberOfPackageBytes += valueTypeNumberOfBytes % 16;
			}

			// Handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
			// -> Make a "float 4"-full-house, if required
			if (0 != numberOfPackageBytes)
			{
				// Take the wasted bytes due to aligned packaging into account
				numberOfBytesPerElement += 4 * 4 - numberOfPackageBytes;
			}

			{ // Write down the uniform buffer header
				RendererRuntime::v1MaterialBlueprint::UniformBufferHeader uniformBufferHeader;
				JsonHelper::optionalIntegerProperty(jsonUniformBufferObject, "RootParameterIndex", uniformBufferHeader.rootParameterIndex);
				detail::optionalUniformBufferUsageProperty(jsonUniformBufferObject, "UniformBufferUsage", uniformBufferHeader.uniformBufferUsage);
				JsonHelper::optionalIntegerProperty(jsonUniformBufferObject, "NumberOfElements", uniformBufferHeader.numberOfElements);
				uniformBufferHeader.numberOfElementProperties = jsonElementPropertiesObject->size();
				uniformBufferHeader.uniformBufferNumberOfBytes = numberOfBytesPerElement * uniformBufferHeader.numberOfElements;
				outputFileStream.write(reinterpret_cast<const char*>(&uniformBufferHeader), sizeof(RendererRuntime::v1MaterialBlueprint::UniformBufferHeader));
			}

			// Write down the uniform buffer element properties
			outputFileStream.write(reinterpret_cast<const char*>(elementProperties.data()), sizeof(RendererRuntime::MaterialProperty) * elementProperties.size());

			// Next uniform buffer, please
			++rootUniformBuffersIterator;
		}
	}

	void JsonMaterialBlueprintHelper::readSamplerStates(Poco::JSON::Object::Ptr jsonSamplerStatesObject, std::ofstream& outputFileStream)
	{
		Poco::JSON::Object::ConstIterator rootSamplerStatesIterator = jsonSamplerStatesObject->begin();
		Poco::JSON::Object::ConstIterator rootSamplerStatesIteratorEnd = jsonSamplerStatesObject->end();
		while (rootSamplerStatesIterator != rootSamplerStatesIteratorEnd)
		{
			Poco::JSON::Object::Ptr jsonSamplerStateObject = rootSamplerStatesIterator->second.extract<Poco::JSON::Object::Ptr>();

			// Start with the default sampler state
			RendererRuntime::v1MaterialBlueprint::SamplerState materialBlueprintSamplerState;
			materialBlueprintSamplerState.rootParameterIndex = 0;
			Renderer::SamplerState& samplerState = materialBlueprintSamplerState;
			samplerState = Renderer::ISamplerState::getDefaultSamplerState();

			// The optional properties
			JsonHelper::optionalIntegerProperty(jsonSamplerStateObject, "RootParameterIndex", materialBlueprintSamplerState.rootParameterIndex);
			JsonMaterialHelper::optionalFilterProperty(jsonSamplerStateObject, "Filter", samplerState.filter);
			JsonMaterialHelper::optionalTextureAddressModeProperty(jsonSamplerStateObject, "AddressU", samplerState.addressU);
			JsonMaterialHelper::optionalTextureAddressModeProperty(jsonSamplerStateObject, "AddressV", samplerState.addressV);
			JsonMaterialHelper::optionalTextureAddressModeProperty(jsonSamplerStateObject, "AddressW", samplerState.addressW);
			JsonHelper::optionalFloatProperty(jsonSamplerStateObject, "MipLODBias", samplerState.mipLODBias);
			JsonHelper::optionalIntegerProperty(jsonSamplerStateObject, "MaxAnisotropy", samplerState.maxAnisotropy);
			JsonMaterialHelper::optionalComparisonFuncProperty(jsonSamplerStateObject, "ComparisonFunc", samplerState.comparisonFunc);
			JsonHelper::optionalFloatNProperty(jsonSamplerStateObject, "BorderColor", samplerState.borderColor, 4);
			JsonHelper::optionalFloatProperty(jsonSamplerStateObject, "MinLOD", samplerState.minLOD);
			JsonHelper::optionalFloatProperty(jsonSamplerStateObject, "MaxLOD", samplerState.maxLOD);

			// Write down the sampler state
			outputFileStream.write(reinterpret_cast<const char*>(&materialBlueprintSamplerState), sizeof(RendererRuntime::v1MaterialBlueprint::SamplerState));

			// Next sampler state, please
			++rootSamplerStatesIterator;
		}
	}

	void JsonMaterialBlueprintHelper::readTextures(const IAssetCompiler::Input& input, const RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector, Poco::JSON::Object::Ptr jsonTexturesObject, std::ofstream& outputFileStream)
	{
		Poco::JSON::Object::ConstIterator rootTexturesIterator = jsonTexturesObject->begin();
		Poco::JSON::Object::ConstIterator rootTexturesIteratorEnd = jsonTexturesObject->end();
		while (rootTexturesIterator != rootTexturesIteratorEnd)
		{
			Poco::JSON::Object::Ptr jsonTextureObject = rootTexturesIterator->second.extract<Poco::JSON::Object::Ptr>();

			// Start with the default texture
			RendererRuntime::v1MaterialBlueprint::Texture materialBlueprintTexture;
			materialBlueprintTexture.rootParameterIndex = 0;

			// The optional properties
			JsonHelper::optionalIntegerProperty(jsonTextureObject, "RootParameterIndex", materialBlueprintTexture.rootParameterIndex);

			{ // Get mandatory asset ID
			  // -> The character "@" is used to reference a material property value
				const std::string sourceAssetIdAsString = jsonTextureObject->get("TextureAssetId").convert<std::string>();
				if (sourceAssetIdAsString.length() > 0 && sourceAssetIdAsString[0] == '@')
				{
					// Reference a material property value
					const RendererRuntime::MaterialPropertyId materialPropertyId(sourceAssetIdAsString.substr(1).c_str());
					materialBlueprintTexture.materialPropertyId = materialPropertyId;

					// Figure out the material property value
					RendererRuntime::MaterialProperties::SortedPropertyVector::const_iterator iterator = std::lower_bound(sortedMaterialPropertyVector.cbegin(), sortedMaterialPropertyVector.cend(), materialPropertyId, RendererRuntime::detail::OrderByMaterialPropertyId());
					if (iterator != sortedMaterialPropertyVector.end())
					{
						RendererRuntime::MaterialProperty* materialProperty = iterator._Ptr;
						if (materialProperty->getMaterialPropertyId() == materialPropertyId)
						{
							// TODO(co) Error handling: Usage mismatch etc.
							materialBlueprintTexture.textureAssetId = materialProperty->getAssetIdValue();
						}
					}
				}
				else
				{
					materialBlueprintTexture.materialPropertyId = 0;

					// Map the source asset ID to the compiled asset ID
					const uint32_t sourceAssetId = static_cast<uint32_t>(std::atoi(sourceAssetIdAsString.c_str()));
					SourceAssetIdToCompiledAssetId::const_iterator iterator = input.sourceAssetIdToCompiledAssetId.find(sourceAssetId);
					materialBlueprintTexture.textureAssetId = (iterator != input.sourceAssetIdToCompiledAssetId.cend()) ? iterator->second : 0;
					// TODO(co) Error handling: Compiled asset ID not found (meaning invalid source asset ID given)
				}
			}

			// Write down the texture
			outputFileStream.write(reinterpret_cast<const char*>(&materialBlueprintTexture), sizeof(RendererRuntime::v1MaterialBlueprint::Texture));

			// Next texture, please
			++rootTexturesIterator;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
