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
#include "RendererToolkit/Helper/JsonMaterialBlueprintHelper.h"
#include "RendererToolkit/Helper/JsonMaterialHelper.h"
#include "RendererToolkit/Helper/FileSystemHelper.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

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
#include <sstream>
#include <algorithm>
#include <unordered_set>


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

		void optionalBufferUsageProperty(const rapidjson::Value& rapidJsonValueUniformBuffer, const char* propertyName, RendererRuntime::MaterialBlueprintResource::BufferUsage& value)
		{
			if (rapidJsonValueUniformBuffer.HasMember(propertyName))
			{
				const rapidjson::Value& rapidJsonValueUsage = rapidJsonValueUniformBuffer[propertyName];
				const char* valueAsString = rapidJsonValueUsage.GetString();
				const rapidjson::SizeType valueStringLength = rapidJsonValueUsage.GetStringLength();

				// Define helper macros
				#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = RendererRuntime::MaterialBlueprintResource::BufferUsage::name;
				#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = RendererRuntime::MaterialBlueprintResource::BufferUsage::name;

				// Evaluate value
				IF_VALUE(UNKNOWN)
				ELSE_IF_VALUE(PASS)
				ELSE_IF_VALUE(MATERIAL)
				ELSE_IF_VALUE(INSTANCE)
				ELSE_IF_VALUE(LIGHT)
				else
				{
					throw std::runtime_error("Buffer usage of property \"" + std::string(propertyName) + "\" must be \"UNKNOWN\", \"PASS\", \"MATERIAL\", \"INSTANCE\" or \"LIGHT\", but \"" + std::string(valueAsString) + "\" set");
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		uint32_t roundUpToNextIntegerDivisibleByFactor(uint32_t input, uint32_t factor)
		{
			return (input + factor - 1) / factor * factor;
		}

		// TODO(co) Currently unused
		void executeParameterSetInstruction(const std::string& instructionAsString, RendererRuntime::ShaderProperties& shaderProperties)
		{
			// "@pset(<parameter name>, <parameter value to set>)" (same syntax as in "RendererRuntime::ShaderBuilder")

			// Gather required data
			std::vector<std::string> elements;
			RendererToolkit::StringHelper::splitString(instructionAsString, ',', elements);
			if (elements.size() == 2)
			{
				const std::string parameterName = elements[0];
				const int32_t parameterValue = std::atoi(elements[1].c_str());

				// Execute
				shaderProperties.setPropertyValue(RendererRuntime::StringId(parameterName.c_str()), parameterValue);
			}
			else
			{
				throw std::runtime_error("Invalid parameter set instruction, syntax is \"@pset(<parameter name>, <parameter value to set>)\"");
			}
		}

		int32_t executeCounterInstruction(const std::string& instructionAsString, RendererRuntime::ShaderProperties& shaderProperties)
		{
			// "@counter(<parameter name>)" (same syntax as in "RendererRuntime::ShaderBuilder")

			// Get the shader property ID
			const size_t valueEndIndex = instructionAsString.find(")", 9);
			const RendererRuntime::ShaderPropertyId shaderPropertyId = RendererRuntime::StringId(instructionAsString.substr(9, valueEndIndex - 9).c_str());

			// Execute
			int32_t value = 0;
			shaderProperties.getPropertyValue(shaderPropertyId, value);
			shaderProperties.setPropertyValue(shaderPropertyId, value + 1);

			// Return the parameter value
			return value;
		}

		uint32_t getIntegerFromInstructionString(const char* instructionAsString, RendererRuntime::ShaderProperties& shaderProperties)
		{
			// Check for instruction "@counter(<parameter name>)" (same syntax as in "RendererRuntime::ShaderBuilder")
			// -> TODO(co) We might want to get rid of the implicit std::string parameter conversion below
			return static_cast<uint32_t>((strncmp(instructionAsString, "@counter(", 7) == 0) ? executeCounterInstruction(instructionAsString, shaderProperties) : std::atoi(instructionAsString));
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
	void JsonMaterialBlueprintHelper::optionalShaderVisibilityProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::ShaderVisibility& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueUsage = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueUsage.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueUsage.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::ShaderVisibility::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::ShaderVisibility::name;

			// Evaluate value
			IF_VALUE(ALL)
			ELSE_IF_VALUE(VERTEX)
			ELSE_IF_VALUE(TESSELLATION_CONTROL)
			ELSE_IF_VALUE(TESSELLATION_EVALUATION)
			ELSE_IF_VALUE(GEOMETRY)
			ELSE_IF_VALUE(FRAGMENT)
			else
			{
				throw std::runtime_error("Shader visibility of property \"" + std::string(propertyName) + "\" must be \"ALL\", \"VERTEX\", \"TESSELLATION_CONTROL\", \"TESSELLATION_EVALUATION\", \"GEOMETRY\" or \"FRAGMENT\", but \"" + std::string(valueAsString) + "\" set");
			}

			// Undefine helper macros
			#undef IF_VALUE
			#undef ELSE_IF_VALUE
		}
	}

	RendererRuntime::MaterialProperty::Usage JsonMaterialBlueprintHelper::mandatoryMaterialPropertyUsage(const rapidjson::Value& rapidJsonValue)
	{
		const rapidjson::Value& rapidJsonValueUsage = rapidJsonValue["Usage"];
		const char* valueAsString = rapidJsonValueUsage.GetString();
		const rapidjson::SizeType valueStringLength = rapidJsonValueUsage.GetStringLength();
		RendererRuntime::MaterialProperty::Usage usage = RendererRuntime::MaterialProperty::Usage::UNKNOWN;

		// Define helper macros
		#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) usage = RendererRuntime::MaterialProperty::Usage::name;
		#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) usage = RendererRuntime::MaterialProperty::Usage::name;

		// Evaluate value
		IF_VALUE(UNKNOWN)
		ELSE_IF_VALUE(STATIC)
		ELSE_IF_VALUE(SHADER_UNIFORM)
		ELSE_IF_VALUE(SHADER_COMBINATION)
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
		ELSE_IF_VALUE(GLOBAL_REFERENCE_FALLBACK)
		else
		{
			throw std::runtime_error("Invalid property usage \"" + std::string(valueAsString) + '\"');
		}

		// Undefine helper macros
		#undef IF_VALUE
		#undef ELSE_IF_VALUE

		// Done
		return usage;
	}

	RendererRuntime::MaterialProperty::ValueType JsonMaterialBlueprintHelper::mandatoryMaterialPropertyValueType(const rapidjson::Value& rapidJsonValue)
	{
		const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue["ValueType"];
		const char* valueAsString = rapidJsonValueValueType.GetString();
		const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();
		RendererRuntime::MaterialProperty::ValueType valueType = RendererRuntime::MaterialProperty::ValueType::UNKNOWN;

		// Define helper macros
		#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) valueType = RendererRuntime::MaterialProperty::ValueType::name;
		#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) valueType = RendererRuntime::MaterialProperty::ValueType::name;

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
		ELSE_IF_VALUE(TEXTURE_ASSET_ID)
		ELSE_IF_VALUE(GLOBAL_MATERIAL_PROPERTY_ID)
		else
		{
			throw std::runtime_error("Invalid property value type \"" + std::string(valueAsString) + '\"');
		}

		// Undefine helper macros
		#undef IF_VALUE
		#undef ELSE_IF_VALUE

		// Done
		return valueType;
	}

	void JsonMaterialBlueprintHelper::getPropertiesByMaterialBlueprintAssetId(const IAssetCompiler::Input& input, RendererRuntime::AssetId materialBlueprintAssetId, RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector, MaterialPropertyIdToName* materialPropertyIdToName)
	{
		// TODO(co) Error handling and simplification, has several parts of "RendererToolkit::MaterialBlueprintAssetCompiler" in common

		// Read material blueprint asset compiler configuration
		std::string materialBlueprintInputFile;
		const std::string absoluteMaterialBlueprintAssetFilename = JsonHelper::getAbsoluteAssetFilename(input, materialBlueprintAssetId);
		{
			// Parse material blueprint asset JSON
			std::ifstream materialBlueprintAssetInputFileStream(absoluteMaterialBlueprintAssetFilename, std::ios::binary);
			rapidjson::Document rapidJsonDocumentMaterialBlueprintAsset;
			JsonHelper::parseDocumentByInputFileStream(rapidJsonDocumentMaterialBlueprintAsset, materialBlueprintAssetInputFileStream, absoluteMaterialBlueprintAssetFilename, "Asset", "1");
			materialBlueprintInputFile = rapidJsonDocumentMaterialBlueprintAsset["Asset"]["MaterialBlueprintAssetCompiler"]["InputFile"].GetString();
		}

		// Parse material blueprint JSON
		const std::string absoluteMaterialBlueprintFilename = std_filesystem::path(absoluteMaterialBlueprintAssetFilename).parent_path().generic_string() + '/' + materialBlueprintInputFile;
		std::ifstream materialBlueprintInputFileStream(absoluteMaterialBlueprintFilename, std::ios::binary);
		rapidjson::Document rapidJsonDocument;
		JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, materialBlueprintInputFileStream, absoluteMaterialBlueprintFilename, "MaterialBlueprintAsset", "1");
		RendererRuntime::ShaderProperties visualImportanceOfShaderProperties;
		RendererRuntime::ShaderProperties maximumIntegerValueOfShaderProperties;
		readProperties(input, rapidJsonDocument["MaterialBlueprintAsset"]["Properties"], sortedMaterialPropertyVector, visualImportanceOfShaderProperties, maximumIntegerValueOfShaderProperties, true, true, materialPropertyIdToName);
	}

	RendererRuntime::MaterialPropertyValue JsonMaterialBlueprintHelper::mandatoryMaterialPropertyValue(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValue, const char* propertyName, const RendererRuntime::MaterialProperty::ValueType valueType)
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
				JsonHelper::optionalBooleanProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromBoolean(0 != value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER:
			{
				int value = 0;
				JsonHelper::optionalIntegerProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromInteger(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_2:
			{
				int values[2] = { 0, 0 };
				JsonHelper::optionalIntegerNProperty(rapidJsonValue, propertyName, values, 2);
				return RendererRuntime::MaterialPropertyValue::fromInteger2(values[0], values[1]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_3:
			{
				int values[3] = { 0, 0, 0 };
				JsonHelper::optionalIntegerNProperty(rapidJsonValue, propertyName, values, 3);
				return RendererRuntime::MaterialPropertyValue::fromInteger3(values[0], values[1], values[2]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_4:
			{
				int values[4] = { 0, 0, 0, 0 };
				JsonHelper::optionalIntegerNProperty(rapidJsonValue, propertyName, values, 4);
				return RendererRuntime::MaterialPropertyValue::fromInteger4(values[0], values[1], values[2], values[3]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT:
			{
				float value = 0.0f;
				JsonHelper::optionalFloatProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromFloat(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_2:
			{
				float values[2] = { 0.0f, 0.0f };
				JsonHelper::optionalFloatNProperty(rapidJsonValue, propertyName, values, 2);
				return RendererRuntime::MaterialPropertyValue::fromFloat2(values[0], values[1]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_3:
			{
				float values[3] = { 0.0f, 0.0f, 0.0f };
				JsonHelper::optionalFloatNProperty(rapidJsonValue, propertyName, values, 3);
				return RendererRuntime::MaterialPropertyValue::fromFloat3(values[0], values[1], values[2]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_4:
			{
				float values[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				JsonHelper::optionalFloatNProperty(rapidJsonValue, propertyName, values, 4);
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
				JsonMaterialHelper::optionalFillModeProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromFillMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::CULL_MODE:
			{
				Renderer::CullMode value = Renderer::CullMode::BACK;
				JsonMaterialHelper::optionalCullModeProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromCullMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::CONSERVATIVE_RASTERIZATION_MODE:
			{
				Renderer::ConservativeRasterizationMode value = Renderer::ConservativeRasterizationMode::OFF;
				JsonMaterialHelper::optionalConservativeRasterizationModeProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromConservativeRasterizationMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::DEPTH_WRITE_MASK:
			{
				Renderer::DepthWriteMask value = Renderer::DepthWriteMask::ALL;
				JsonMaterialHelper::optionalDepthWriteMaskProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromDepthWriteMask(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::STENCIL_OP:
			{
				Renderer::StencilOp value = Renderer::StencilOp::KEEP;
				JsonMaterialHelper::optionalStencilOpProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromStencilOp(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::COMPARISON_FUNC:
			{
				Renderer::ComparisonFunc value = Renderer::ComparisonFunc::LESS;
				JsonMaterialHelper::optionalComparisonFuncProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromComparisonFunc(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::BLEND:
			{
				Renderer::Blend value = Renderer::Blend::ONE;
				JsonMaterialHelper::optionalBlendProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromBlend(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::BLEND_OP:
			{
				Renderer::BlendOp value = Renderer::BlendOp::ADD;
				JsonMaterialHelper::optionalBlendOpProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromBlendOp(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FILTER_MODE:
			{
				Renderer::FilterMode value = Renderer::FilterMode::MIN_MAG_MIP_LINEAR;
				JsonMaterialHelper::optionalFilterProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromFilterMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::TEXTURE_ADDRESS_MODE:
			{
				Renderer::TextureAddressMode value = Renderer::TextureAddressMode::CLAMP;
				JsonMaterialHelper::optionalTextureAddressModeProperty(rapidJsonValue, propertyName, value);
				return RendererRuntime::MaterialPropertyValue::fromTextureAddressMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::TEXTURE_ASSET_ID:
			{
				RendererRuntime::AssetId textureAssetId = RendererRuntime::getUninitialized<RendererRuntime::AssetId>();
				if (rapidJsonValue.HasMember(propertyName))
				{
					// Usage of asset IDs is the preferred way to go, but we also need to support the asset ID naming scheme "<project name>/<asset type>/<asset category>/<asset name>"
					textureAssetId = StringHelper::getAssetIdByString(rapidJsonValue[propertyName].GetString(), input);
				}
				if (RendererRuntime::isUninitialized(textureAssetId))
				{
					throw std::runtime_error("Inside material blueprints, texture asset reference material properties must always have a value");
				}

				// Done
				return RendererRuntime::MaterialPropertyValue::fromTextureAssetId(textureAssetId);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::GLOBAL_MATERIAL_PROPERTY_ID:
			{
				RendererRuntime::MaterialPropertyId materialPropertyId = RendererRuntime::getUninitialized<RendererRuntime::MaterialPropertyId>();
				if (rapidJsonValue.HasMember(propertyName))
				{
					// Get the reference value as string
					static const uint32_t NAME_LENGTH = 128;
					char referenceAsString[NAME_LENGTH];
					memset(&referenceAsString[0], 0, sizeof(char) * NAME_LENGTH);
					JsonHelper::optionalStringProperty(rapidJsonValue, propertyName, referenceAsString, NAME_LENGTH);

					// The character "@" is used to reference e.g. a material property value
					if (referenceAsString[0] == '@')
					{
						// Write down the material property
						materialPropertyId = RendererRuntime::StringId(&referenceAsString[1]);
					}
					else
					{
						throw std::runtime_error("Inside material blueprints, global material property ID material property values must begin with a @");
					}
				}
				if (RendererRuntime::isUninitialized(materialPropertyId))
				{
					throw std::runtime_error("Inside material blueprints, global material property ID material properties must always have a value");
				}

				// Done
				return RendererRuntime::MaterialPropertyValue::fromGlobalMaterialPropertyId(materialPropertyId);
			}
		}

		// TODO(co) Error, we should never ever end up in here
		return RendererRuntime::MaterialPropertyValue::fromBoolean(false);
	}

	void JsonMaterialBlueprintHelper::readRootSignature(const rapidjson::Value& rapidJsonValueRootSignature, std::ostream& outputMemoryStream, RendererRuntime::ShaderProperties& shaderProperties)
	{
		// First: Collect everything we need instead of directly writing it down using an inefficient data layout
		std::vector<Renderer::RootParameterData> rootParameters;
		std::vector<Renderer::DescriptorRange> descriptorRanges;
		{
			const rapidjson::Value& rapidJsonValueRootParameters = rapidJsonValueRootSignature["RootParameters"];
			rootParameters.reserve(rapidJsonValueRootParameters.MemberCount());
			descriptorRanges.reserve(rapidJsonValueRootParameters.MemberCount());

			for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorRootParameters = rapidJsonValueRootParameters.MemberBegin(); rapidJsonMemberIteratorRootParameters != rapidJsonValueRootParameters.MemberEnd(); ++rapidJsonMemberIteratorRootParameters)
			{
				const rapidjson::Value& rapidJsonValueRootParameter = rapidJsonMemberIteratorRootParameters->value;
				const rapidjson::Value& rapidJsonValueParameterType = rapidJsonValueRootParameter["ParameterType"];
				const char* parameterTypeAsString = rapidJsonValueParameterType.GetString();
				const rapidjson::SizeType parameterTypeStringLength = rapidJsonValueParameterType.GetStringLength();

				// TODO(co) Add support for the other root parameter types
				if (strncmp(parameterTypeAsString, "DESCRIPTOR_TABLE", parameterTypeStringLength) == 0)
				{
					const rapidjson::Value& rapidJsonValueDescriptorRanges = rapidJsonValueRootParameter["DescriptorRanges"];

					{ // Collect the root parameter
						Renderer::RootParameterData rootParameter;
						rootParameter.parameterType = Renderer::RootParameterType::DESCRIPTOR_TABLE;
						rootParameter.numberOfDescriptorRanges = rapidJsonValueDescriptorRanges.MemberCount();
						rootParameter.shaderVisibility = Renderer::ShaderVisibility::ALL;
						optionalShaderVisibilityProperty(rapidJsonValueRootParameter, "ShaderVisibility", rootParameter.shaderVisibility);
						rootParameters.push_back(rootParameter);
					}

					// Descriptor ranges
					for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorDescriptorRanges = rapidJsonValueDescriptorRanges.MemberBegin(); rapidJsonMemberIteratorDescriptorRanges != rapidJsonValueDescriptorRanges.MemberEnd(); ++rapidJsonMemberIteratorDescriptorRanges)
					{
						const rapidjson::Value& rapidJsonValueDescriptorRange = rapidJsonMemberIteratorDescriptorRanges->value;
						Renderer::DescriptorRange descriptorRange;

						{ // Range type
							const rapidjson::Value& rapidJsonValueRangeType = rapidJsonValueDescriptorRange["RangeType"];
							const char* rangeTypeAsString = rapidJsonValueRangeType.GetString();
							const rapidjson::SizeType rangeTypeStringLength = rapidJsonValueRangeType.GetStringLength();

							// Define helper macros
							#define IF_VALUE(name)			 if (strncmp(rangeTypeAsString, #name, rangeTypeStringLength) == 0) descriptorRange.rangeType = Renderer::DescriptorRangeType::name;
							#define ELSE_IF_VALUE(name) else if (strncmp(rangeTypeAsString, #name, rangeTypeStringLength) == 0) descriptorRange.rangeType = Renderer::DescriptorRangeType::name;

							// Evaluate value
							IF_VALUE(SRV)
							ELSE_IF_VALUE(UAV)
							ELSE_IF_VALUE(UBV)
							ELSE_IF_VALUE(SAMPLER)
							else
							{
								throw std::runtime_error("Invalid range type \"" + std::string(rangeTypeAsString) + "\", must be \"SRV\", \"UAV\", \"UBV\" or \"SAMPLER\"");
							}

							// Undefine helper macros
							#undef IF_VALUE
							#undef ELSE_IF_VALUE
						}

						// Optional number of descriptors
						descriptorRange.numberOfDescriptors = 1;
						JsonHelper::optionalIntegerProperty(rapidJsonValueDescriptorRange, "NumberOfDescriptors", descriptorRange.numberOfDescriptors);

						// Mandatory base shader register
						descriptorRange.baseShaderRegister = ::detail::getIntegerFromInstructionString(rapidJsonValueDescriptorRange["BaseShaderRegister"].GetString(), shaderProperties);

						// Optional register space
						descriptorRange.registerSpace = 0;
						JsonHelper::optionalIntegerProperty(rapidJsonValueDescriptorRange, "RegisterSpace", descriptorRange.registerSpace);

						// Optional offset in descriptors from table start
						descriptorRange.offsetInDescriptorsFromTableStart = 0;
						JsonHelper::optionalIntegerProperty(rapidJsonValueDescriptorRange, "OffsetInDescriptorsFromTableStart", descriptorRange.offsetInDescriptorsFromTableStart);

						// Optional base shader register name
						descriptorRange.baseShaderRegisterName[0] = '\0';
						JsonHelper::optionalStringProperty(rapidJsonValueDescriptorRange, "BaseShaderRegisterName", descriptorRange.baseShaderRegisterName, Renderer::DescriptorRange::NAME_LENGTH);

						// Optional sampler root parameter index
						descriptorRange.samplerRootParameterIndex = 0;
						JsonHelper::optionalIntegerProperty(rapidJsonValueDescriptorRange, "SamplerRootParameterIndex", descriptorRange.samplerRootParameterIndex);

						// Collect the descriptor range
						descriptorRanges.push_back(descriptorRange);
					}
				}
				else
				{
					throw std::runtime_error("Root parameter type must be \"DESCRIPTOR_TABLE\"");
				}
			}
		}

		{ // Now that we have collected everything we need, perform some editing sanity and security checks before writing down the root signature
		  // -> Base shader register clashes: Direct3D has completely separated shader stages allowing one to e.g. bind a texture buffer at vertex shader texture stage 0 while binding
		  //    a 2D texture at fragment shader stage 0. OpenGL doesn't support something like this and one has to ensure there are no base shader register clashes between separate shader
		  //    stages. Horrible error prone and inflexible restriction, but we can't change that so we have to check for it and sparing the material blueprint editor crazy debugging efforts.
			typedef std::unordered_set<uint32_t> BaseShaderRegisterUsed;	// Key = Base shader register
			BaseShaderRegisterUsed rangeTypeBaseShaderRegisterUsed[static_cast<uint32_t>(Renderer::DescriptorRangeType::NUMBER_OF_RANGE_TYPES)];
			for (const Renderer::DescriptorRange& descriptorRange : descriptorRanges)
			{
				BaseShaderRegisterUsed& baseShaderRegisterUsed = rangeTypeBaseShaderRegisterUsed[static_cast<uint32_t>(descriptorRange.rangeType)];
				if (baseShaderRegisterUsed.find(descriptorRange.baseShaderRegister) == baseShaderRegisterUsed.cend())
				{
					baseShaderRegisterUsed.insert(descriptorRange.baseShaderRegister);
				}
				else
				{
					throw std::runtime_error("Base shader register " + std::to_string(descriptorRange.baseShaderRegister) + " is already used. Please note that to be renderer API independent, base shader register usage is considered to be across all shader stages like OpenGL does.");
				}
			}
		}

		{ // Write down the root signature header
			RendererRuntime::v1MaterialBlueprint::RootSignatureHeader rootSignatureHeader;
			rootSignatureHeader.numberOfRootParameters	 = static_cast<uint32_t>(rootParameters.size());
			rootSignatureHeader.numberOfDescriptorRanges = static_cast<uint32_t>(descriptorRanges.size());
			rootSignatureHeader.numberOfStaticSamplers	 = 0;									// TODO(co) Add support for static samplers
			rootSignatureHeader.flags					 = Renderer::RootSignatureFlags::NONE;	// TODO(co) Add support for flags
			outputMemoryStream.write(reinterpret_cast<const char*>(&rootSignatureHeader), sizeof(RendererRuntime::v1MaterialBlueprint::RootSignatureHeader));
		}

		// Write down the root parameters
		outputMemoryStream.write(reinterpret_cast<const char*>(rootParameters.data()), static_cast<std::streamsize>(sizeof(Renderer::RootParameterData) * rootParameters.size()));

		// Write down the descriptor ranges
		outputMemoryStream.write(reinterpret_cast<const char*>(descriptorRanges.data()), static_cast<std::streamsize>(sizeof(Renderer::DescriptorRange) * descriptorRanges.size()));
	}

	void JsonMaterialBlueprintHelper::readProperties(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValueProperties, RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector, RendererRuntime::ShaderProperties& visualImportanceOfShaderProperties, RendererRuntime::ShaderProperties& maximumIntegerValueOfShaderProperties, bool ignoreGlobalReferenceFallback, bool sort, MaterialPropertyIdToName* materialPropertyIdToName)
	{
		for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIterator = rapidJsonValueProperties.MemberBegin(); rapidJsonMemberIterator != rapidJsonValueProperties.MemberEnd(); ++rapidJsonMemberIterator)
		{
			const rapidjson::Value& rapidJsonValueProperty = rapidJsonMemberIterator->value;

			// Material property ID
			const RendererRuntime::MaterialPropertyId materialPropertyId(rapidJsonMemberIterator->name.GetString());
			if (nullptr != materialPropertyIdToName)
			{
				materialPropertyIdToName->emplace(materialPropertyId, rapidJsonMemberIterator->name.GetString());
			}

			// Material property usage
			// -> Optimisation: Material resources don't need to store global reference fallbacks, it's sufficient if those are just stored inside material blueprint resources
			const RendererRuntime::MaterialProperty::Usage usage = mandatoryMaterialPropertyUsage(rapidJsonValueProperty);
			if (!ignoreGlobalReferenceFallback || RendererRuntime::MaterialProperty::Usage::GLOBAL_REFERENCE_FALLBACK != usage)
			{
				const RendererRuntime::MaterialProperty::ValueType valueType = mandatoryMaterialPropertyValueType(rapidJsonValueProperty);
				if (RendererRuntime::MaterialProperty::isReferenceUsage(usage))
				{
					// Get the reference value as string
					static const uint32_t NAME_LENGTH = 128;
					char referenceAsString[NAME_LENGTH];
					memset(&referenceAsString[0], 0, sizeof(char) * NAME_LENGTH);
					JsonHelper::optionalStringProperty(rapidJsonValueProperty, "Value", referenceAsString, NAME_LENGTH);

					// The character "@" is used to reference e.g. a material property value
					if (referenceAsString[0] == '@')
					{
						// Write down the material property
						const RendererRuntime::StringId referenceAsInteger(&referenceAsString[1]);
						sortedMaterialPropertyVector.emplace_back(RendererRuntime::MaterialProperty(materialPropertyId, usage, RendererRuntime::MaterialProperty::materialPropertyValueFromReference(valueType, referenceAsInteger)));
					}
					else
					{
						// Write down the material property
						sortedMaterialPropertyVector.emplace_back(RendererRuntime::MaterialProperty(materialPropertyId, usage, mandatoryMaterialPropertyValue(input, rapidJsonValueProperty, "Value", valueType)));
					}
				}
				else
				{
					// Write down the material property
					sortedMaterialPropertyVector.emplace_back(RendererRuntime::MaterialProperty(materialPropertyId, usage, mandatoryMaterialPropertyValue(input, rapidJsonValueProperty, "Value", valueType)));
				}

				// Optional visual importance of shader property
				if (rapidJsonValueProperty.HasMember("VisualImportance"))
				{
					// Sanity check: "VisualImportance" is only valid for shader combination properties
					// TODO(co) Error handling
					assert(RendererRuntime::MaterialProperty::Usage::SHADER_COMBINATION == usage);
					if (RendererRuntime::MaterialProperty::Usage::SHADER_COMBINATION == usage)
					{
						const rapidjson::Value& rapidJsonValueVisualImportance = rapidJsonValueProperty["VisualImportance"];
						const char* valueAsString = rapidJsonValueVisualImportance.GetString();
						int32_t visualImportanceOfShaderProperty = RendererRuntime::MaterialBlueprintResource::MANDATORY_SHADER_PROPERTY;
						if (strncmp(valueAsString, "MANDATORY", 9) != 0)
						{
							visualImportanceOfShaderProperty = std::atoi(valueAsString);
						}
						visualImportanceOfShaderProperties.setPropertyValue(materialPropertyId, visualImportanceOfShaderProperty);	// We're using the same string hashing for material property ID and shader property ID
					}
				}

				// Mandatory maximum value for integer type shader combination properties to be able to keep the total number of shader combinations manageable
				if (RendererRuntime::MaterialProperty::Usage::SHADER_COMBINATION == usage && RendererRuntime::MaterialProperty::ValueType::INTEGER == valueType)
				{
					// TODO(co) Error handling

					// "MaximumIntegerValue" (inclusive)
					const bool hasMaximumIntegerValue = rapidJsonValueProperty.HasMember("MaximumIntegerValue");
					assert(hasMaximumIntegerValue);
					if (hasMaximumIntegerValue)
					{
						const int maximumIntegerValue = std::atoi(rapidJsonValueProperty["MaximumIntegerValue"].GetString());
						assert(maximumIntegerValue > 0);
						maximumIntegerValueOfShaderProperties.setPropertyValue(materialPropertyId, maximumIntegerValue);	// We're using the same string hashing for material property ID and shader property ID
					}
				}
			}
		}

		// Ensure the material properties are sorted, if requested
		if (sort)
		{
			std::sort(sortedMaterialPropertyVector.begin(), sortedMaterialPropertyVector.end(), detail::orderByMaterialPropertyId);
		}
	}

	void JsonMaterialBlueprintHelper::readPipelineStateObject(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValuePipelineState, std::ostream& outputMemoryStream, const RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector)
	{
		{ // Vertex attributes asset ID
			const RendererRuntime::AssetId vertexAttributesAssetId = StringHelper::getAssetIdByString(rapidJsonValuePipelineState["VertexAttributesAssetId"].GetString(), input);
			outputMemoryStream.write(reinterpret_cast<const char*>(&vertexAttributesAssetId), sizeof(RendererRuntime::AssetId));
		}

		{ // Shader blueprints
			const rapidjson::Value& rapidJsonValueShaderBlueprints = rapidJsonValuePipelineState["ShaderBlueprints"];

			RendererRuntime::AssetId shaderBlueprintAssetId[RendererRuntime::NUMBER_OF_SHADER_TYPES];
			memset(shaderBlueprintAssetId, static_cast<int>(RendererRuntime::getUninitialized<RendererRuntime::AssetId>()), sizeof(RendererRuntime::AssetId) * RendererRuntime::NUMBER_OF_SHADER_TYPES);
			shaderBlueprintAssetId[static_cast<uint8_t>(RendererRuntime::ShaderType::Vertex)] = JsonHelper::getCompiledAssetId(input, rapidJsonValueShaderBlueprints, "VertexShaderBlueprintAssetId");
			JsonHelper::optionalCompiledAssetId(input, rapidJsonValueShaderBlueprints, "TessellationControlShaderBlueprintAssetId", shaderBlueprintAssetId[static_cast<uint8_t>(RendererRuntime::ShaderType::TessellationControl)]);
			JsonHelper::optionalCompiledAssetId(input, rapidJsonValueShaderBlueprints, "TessellationEvaluationShaderBlueprintAssetId", shaderBlueprintAssetId[static_cast<uint8_t>(RendererRuntime::ShaderType::TessellationEvaluation)]);
			JsonHelper::optionalCompiledAssetId(input, rapidJsonValueShaderBlueprints, "GeometryShaderBlueprintAssetId", shaderBlueprintAssetId[static_cast<uint8_t>(RendererRuntime::ShaderType::Geometry)]);
			JsonHelper::optionalCompiledAssetId(input, rapidJsonValueShaderBlueprints, "FragmentShaderBlueprintAssetId", shaderBlueprintAssetId[static_cast<uint8_t>(RendererRuntime::ShaderType::Fragment)]);

			// Write down the shader blueprints
			outputMemoryStream.write(reinterpret_cast<const char*>(&shaderBlueprintAssetId), sizeof(RendererRuntime::AssetId) * RendererRuntime::NUMBER_OF_SHADER_TYPES);
		}

		// Start with the default settings
		Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder();

		// Optional rasterizer state
		if (rapidJsonValuePipelineState.HasMember("RasterizerState"))
		{
			const rapidjson::Value& rapidJsonValueRasterizerState = rapidJsonValuePipelineState["RasterizerState"];
			Renderer::RasterizerState& rasterizerState = pipelineState.rasterizerState;

			// The optional properties
			JsonMaterialHelper::optionalFillModeProperty(rapidJsonValueRasterizerState, "FillMode", rasterizerState.fillMode, &sortedMaterialPropertyVector);
			JsonMaterialHelper::optionalCullModeProperty(rapidJsonValueRasterizerState, "CullMode", rasterizerState.cullMode, &sortedMaterialPropertyVector);
			JsonHelper::optionalBooleanProperty(rapidJsonValueRasterizerState, "FrontCounterClockwise", rasterizerState.frontCounterClockwise);
			JsonHelper::optionalIntegerProperty(rapidJsonValueRasterizerState, "DepthBias", rasterizerState.depthBias);
			JsonHelper::optionalFloatProperty(rapidJsonValueRasterizerState, "DepthBiasClamp", rasterizerState.depthBiasClamp);
			JsonHelper::optionalFloatProperty(rapidJsonValueRasterizerState, "SlopeScaledDepthBias", rasterizerState.slopeScaledDepthBias);
			JsonHelper::optionalBooleanProperty(rapidJsonValueRasterizerState, "DepthClipEnable", rasterizerState.depthClipEnable);
			JsonHelper::optionalBooleanProperty(rapidJsonValueRasterizerState, "MultisampleEnable", rasterizerState.multisampleEnable);
			JsonHelper::optionalBooleanProperty(rapidJsonValueRasterizerState, "AntialiasedLineEnable", rasterizerState.antialiasedLineEnable);
			JsonHelper::optionalIntegerProperty(rapidJsonValueRasterizerState, "ForcedSampleCount", rasterizerState.forcedSampleCount);
			JsonMaterialHelper::optionalConservativeRasterizationModeProperty(rapidJsonValueRasterizerState, "ConservativeRasterizationMode", rasterizerState.conservativeRasterizationMode, &sortedMaterialPropertyVector);
			JsonHelper::optionalBooleanProperty(rapidJsonValueRasterizerState, "ScissorEnable", rasterizerState.scissorEnable);
		}

		// Optional depth stencil state
		if (rapidJsonValuePipelineState.HasMember("DepthStencilState"))
		{
			const rapidjson::Value& rapidJsonValueDepthStencilState = rapidJsonValuePipelineState["DepthStencilState"];
			Renderer::DepthStencilState& depthStencilState = pipelineState.depthStencilState;

			// The optional properties
			JsonHelper::optionalBooleanProperty(rapidJsonValueDepthStencilState, "DepthEnable", depthStencilState.depthEnable);
			JsonMaterialHelper::optionalDepthWriteMaskProperty(rapidJsonValueDepthStencilState, "DepthWriteMask", depthStencilState.depthWriteMask, &sortedMaterialPropertyVector);
			JsonMaterialHelper::optionalComparisonFuncProperty(rapidJsonValueDepthStencilState, "DepthFunc", depthStencilState.depthFunc, &sortedMaterialPropertyVector);

			// TODO(co) Depth stencil state: Read in the rest of the PSO
			/*
			"DepthStencilState":
			{
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

		// Optional blend state
		if (rapidJsonValuePipelineState.HasMember("BlendState"))
		{
			const rapidjson::Value& rapidJsonValueBlendState = rapidJsonValuePipelineState["BlendState"];
			Renderer::BlendState& blendState = pipelineState.blendState;

			// The optional properties
			JsonHelper::optionalBooleanProperty(rapidJsonValueBlendState, "AlphaToCoverageEnable", blendState.alphaToCoverageEnable);
			JsonHelper::optionalBooleanProperty(rapidJsonValueBlendState, "IndependentBlendEnable", blendState.independentBlendEnable);

			// The optional render target properties
			for (int i = 0; i < 8; ++i)
			{
				const std::string renderTarget = "RenderTarget[" + std::to_string(i) + ']';
				if (rapidJsonValueBlendState.HasMember(renderTarget.c_str()))
				{
					const rapidjson::Value& rapidJsonValueRenderTarget = rapidJsonValueBlendState[renderTarget.c_str()];
					Renderer::RenderTargetBlendDesc& renderTargetBlendDesc = blendState.renderTarget[i];

					// The optional properties
					JsonHelper::optionalBooleanProperty(rapidJsonValueRenderTarget, "BlendEnable", renderTargetBlendDesc.blendEnable);
					JsonMaterialHelper::optionalBlendProperty(rapidJsonValueRenderTarget, "SrcBlend", renderTargetBlendDesc.srcBlend, &sortedMaterialPropertyVector);
					JsonMaterialHelper::optionalBlendProperty(rapidJsonValueRenderTarget, "DestBlend", renderTargetBlendDesc.destBlend, &sortedMaterialPropertyVector);
					JsonMaterialHelper::optionalBlendOpProperty(rapidJsonValueRenderTarget, "BlendOp", renderTargetBlendDesc.blendOp, &sortedMaterialPropertyVector);
					JsonMaterialHelper::optionalBlendProperty(rapidJsonValueRenderTarget, "SrcBlendAlpha", renderTargetBlendDesc.srcBlendAlpha, &sortedMaterialPropertyVector);
					JsonMaterialHelper::optionalBlendProperty(rapidJsonValueRenderTarget, "DestBlendAlpha", renderTargetBlendDesc.destBlendAlpha, &sortedMaterialPropertyVector);
					JsonMaterialHelper::optionalBlendOpProperty(rapidJsonValueRenderTarget, "BlendOpAlpha", renderTargetBlendDesc.blendOpAlpha, &sortedMaterialPropertyVector);

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

		// Write down the pipeline state object (PSO)
		outputMemoryStream.write(reinterpret_cast<const char*>(&pipelineState), sizeof(Renderer::SerializedPipelineState));
	}

	void JsonMaterialBlueprintHelper::readUniformBuffers(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValueUniformBuffers, std::ostream& outputMemoryStream, RendererRuntime::ShaderProperties& shaderProperties)
	{
		for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorUniformBuffers = rapidJsonValueUniformBuffers.MemberBegin(); rapidJsonMemberIteratorUniformBuffers != rapidJsonValueUniformBuffers.MemberEnd(); ++rapidJsonMemberIteratorUniformBuffers)
		{
			const rapidjson::Value& rapidJsonValueUniformBuffer = rapidJsonMemberIteratorUniformBuffers->value;
			const rapidjson::Value& rapidJsonValueElementProperties = rapidJsonValueUniformBuffer["ElementProperties"];

			// Gather all element properties, don't sort because the user defined order is important in here (data layout in memory)
			RendererRuntime::MaterialProperties::SortedPropertyVector elementProperties;
			RendererRuntime::ShaderProperties visualImportanceOfShaderProperties;
			RendererRuntime::ShaderProperties maximumIntegerValueOfShaderProperties;
			readProperties(input, rapidJsonValueElementProperties, elementProperties, visualImportanceOfShaderProperties, maximumIntegerValueOfShaderProperties, true, false);

			// Calculate the uniform buffer size, including handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
			// -> Sum up the number of bytes required by all uniform buffer element properties
			uint32_t numberOfPackageBytes = 0;
			uint32_t numberOfBytesPerElement = 0;
			const size_t numberOfUniformBufferElementProperties = elementProperties.size();
			for (size_t i = 0; i < numberOfUniformBufferElementProperties; ++i)
			{
				// Get value type number of bytes
				const uint32_t valueTypeNumberOfBytes = RendererRuntime::MaterialPropertyValue::getValueTypeNumberOfBytes(elementProperties[i].getValueType());
				numberOfBytesPerElement += valueTypeNumberOfBytes;

				// Handling of packing rules for uniform variables
				//  -> We have to take into account HLSL packing (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
				//  -> GLSL is even more restrictive, with aligning e.g. float2 to an offset divisible by 2 * 4 bytes (float2 size) and float3 to an offset divisible by 4 * 4 bytes (float4 size -- yes, there is no actual float3 alignment)
				if (0 != numberOfPackageBytes)	// No problem if no package was started yet
				{
					// Taking into account GLSL rules here, for HLSL this would always be "numberOfPackageBytes"
					const uint32_t alignmentStartByteOffsetInPackage = ::detail::roundUpToNextIntegerDivisibleByFactor(numberOfPackageBytes, valueTypeNumberOfBytes);

					// Check for float4-size package "overflow" (relevant for both HLSL and GLSL)
					if (numberOfPackageBytes + valueTypeNumberOfBytes > 16)
					{
						// Take the wasted bytes due to aligned packaging into account and restart the package bytes counter
						numberOfBytesPerElement += 4 * 4 - numberOfPackageBytes;
						numberOfPackageBytes = 0;

						// TODO(co) Profiling information: We could provide the material blueprint resource writer with information how many bytes get wasted with the defined layout
					}

					// For GLSL, we are running into problems if there is not overflow, but alignment is not correct
					// TODO(co) Check the documentation, whether there are 16-byte packages at all for GLSL - otherwise this has to be rewritten!
					else if (numberOfPackageBytes != alignmentStartByteOffsetInPackage)
					{
						// TODO(co) Error handling: General error handling strategy required
						const std::string materialBlueprint = "TODO";
						const std::string propertyName = "TODO";
						throw std::runtime_error("Material blueprint " + materialBlueprint + ": Uniform buffer element property alignment is problematic for property " + propertyName + " at offset " + std::to_string(numberOfPackageBytes) + ", which would be aligned to offset " + std::to_string(alignmentStartByteOffsetInPackage));
					}
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
				uniformBufferHeader.rootParameterIndex = ::detail::getIntegerFromInstructionString(rapidJsonValueUniformBuffer["RootParameterIndex"].GetString(), shaderProperties);
				detail::optionalBufferUsageProperty(rapidJsonValueUniformBuffer, "BufferUsage", uniformBufferHeader.bufferUsage);
				JsonHelper::optionalIntegerProperty(rapidJsonValueUniformBuffer, "NumberOfElements", uniformBufferHeader.numberOfElements);
				uniformBufferHeader.numberOfElementProperties = static_cast<uint32_t>(elementProperties.size());
				uniformBufferHeader.uniformBufferNumberOfBytes = numberOfBytesPerElement * uniformBufferHeader.numberOfElements;
				outputMemoryStream.write(reinterpret_cast<const char*>(&uniformBufferHeader), sizeof(RendererRuntime::v1MaterialBlueprint::UniformBufferHeader));
			}

			// Write down the uniform buffer element properties
			outputMemoryStream.write(reinterpret_cast<const char*>(elementProperties.data()), static_cast<std::streamsize>(sizeof(RendererRuntime::MaterialProperty) * elementProperties.size()));
		}
	}

	void JsonMaterialBlueprintHelper::readTextureBuffers(const rapidjson::Value& rapidJsonValueTextureBuffers, std::ostream& outputMemoryStream, RendererRuntime::ShaderProperties& shaderProperties)
	{
		for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorTextureBuffers = rapidJsonValueTextureBuffers.MemberBegin(); rapidJsonMemberIteratorTextureBuffers != rapidJsonValueTextureBuffers.MemberEnd(); ++rapidJsonMemberIteratorTextureBuffers)
		{
			const rapidjson::Value& rapidJsonValueTextureBuffer = rapidJsonMemberIteratorTextureBuffers->value;

			{ // Write down the texture buffer header
				RendererRuntime::v1MaterialBlueprint::TextureBufferHeader textureBufferHeader;
				{ // Value type and value
					const RendererRuntime::MaterialProperty::ValueType valueType = mandatoryMaterialPropertyValueType(rapidJsonValueTextureBuffer);

					// Get the reference value as string
					static const uint32_t NAME_LENGTH = 128;
					char referenceAsString[NAME_LENGTH];
					memset(&referenceAsString[0], 0, sizeof(char) * NAME_LENGTH);
					JsonHelper::optionalStringProperty(rapidJsonValueTextureBuffer, "Value", referenceAsString, NAME_LENGTH);

					// Construct the material property value
					const RendererRuntime::StringId referenceAsInteger(&referenceAsString[1]);	// Skip the '@'
					textureBufferHeader.materialPropertyValue = RendererRuntime::MaterialProperty::materialPropertyValueFromReference(valueType, referenceAsInteger);
				}
				textureBufferHeader.rootParameterIndex = ::detail::getIntegerFromInstructionString(rapidJsonValueTextureBuffer["RootParameterIndex"].GetString(), shaderProperties);
				detail::optionalBufferUsageProperty(rapidJsonValueTextureBuffer, "BufferUsage", textureBufferHeader.bufferUsage);
				outputMemoryStream.write(reinterpret_cast<const char*>(&textureBufferHeader), sizeof(RendererRuntime::v1MaterialBlueprint::TextureBufferHeader));
			}
		}
	}

	void JsonMaterialBlueprintHelper::readSamplerStates(const rapidjson::Value& rapidJsonValueSamplerStates, std::ostream& outputMemoryStream, RendererRuntime::ShaderProperties& shaderProperties, const RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector)
	{
		for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorSamplerStates = rapidJsonValueSamplerStates.MemberBegin(); rapidJsonMemberIteratorSamplerStates != rapidJsonValueSamplerStates.MemberEnd(); ++rapidJsonMemberIteratorSamplerStates)
		{
			const rapidjson::Value& rapidJsonValueSamplerState = rapidJsonMemberIteratorSamplerStates->value;

			// Start with the default sampler state
			RendererRuntime::v1MaterialBlueprint::SamplerState materialBlueprintSamplerState;
			materialBlueprintSamplerState.rootParameterIndex = 0;
			Renderer::SamplerState& samplerState = materialBlueprintSamplerState;
			samplerState = Renderer::ISamplerState::getDefaultSamplerState();

			// The optional properties
			materialBlueprintSamplerState.rootParameterIndex = ::detail::getIntegerFromInstructionString(rapidJsonValueSamplerState["RootParameterIndex"].GetString(), shaderProperties);
			JsonMaterialHelper::optionalFilterProperty(rapidJsonValueSamplerState, "Filter", samplerState.filter, &sortedMaterialPropertyVector);
			JsonMaterialHelper::optionalTextureAddressModeProperty(rapidJsonValueSamplerState, "AddressU", samplerState.addressU, &sortedMaterialPropertyVector);
			JsonMaterialHelper::optionalTextureAddressModeProperty(rapidJsonValueSamplerState, "AddressV", samplerState.addressV, &sortedMaterialPropertyVector);
			JsonMaterialHelper::optionalTextureAddressModeProperty(rapidJsonValueSamplerState, "AddressW", samplerState.addressW, &sortedMaterialPropertyVector);
			JsonHelper::optionalFloatProperty(rapidJsonValueSamplerState, "MipLODBias", samplerState.mipLODBias);
			JsonHelper::optionalIntegerProperty(rapidJsonValueSamplerState, "MaxAnisotropy", samplerState.maxAnisotropy);
			JsonMaterialHelper::optionalComparisonFuncProperty(rapidJsonValueSamplerState, "ComparisonFunc", samplerState.comparisonFunc, &sortedMaterialPropertyVector);
			JsonHelper::optionalFloatNProperty(rapidJsonValueSamplerState, "BorderColor", samplerState.borderColor, 4);
			JsonHelper::optionalFloatProperty(rapidJsonValueSamplerState, "MinLOD", samplerState.minLOD);
			JsonHelper::optionalFloatProperty(rapidJsonValueSamplerState, "MaxLOD", samplerState.maxLOD);

			// Write down the sampler state
			outputMemoryStream.write(reinterpret_cast<const char*>(&materialBlueprintSamplerState), sizeof(RendererRuntime::v1MaterialBlueprint::SamplerState));
		}
	}

	void JsonMaterialBlueprintHelper::readTextures(const IAssetCompiler::Input& input, const RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector, const rapidjson::Value& rapidJsonValueTextures, std::ostream& outputMemoryStream, RendererRuntime::ShaderProperties& shaderProperties)
	{
		for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorTextures = rapidJsonValueTextures.MemberBegin(); rapidJsonMemberIteratorTextures != rapidJsonValueTextures.MemberEnd(); ++rapidJsonMemberIteratorTextures)
		{
			const rapidjson::Value& rapidJsonValueTexture = rapidJsonMemberIteratorTextures->value;

			// Mandatory root parameter index
			const uint32_t rootParameterIndex = ::detail::getIntegerFromInstructionString(rapidJsonValueTexture["RootParameterIndex"].GetString(), shaderProperties);

			// Mandatory fallback texture asset ID
			// -> We could make this optional, but it's better to be totally restrictive in here so asynchronous texture loading always works nicely (easy when done from the beginning, hard to add this afterwards)
			// -> Often, but not always this value is identical to a texture asset referencing material property. So, we really have to define an own property for this.
			const RendererRuntime::AssetId fallbackTextureAssetId = JsonHelper::getCompiledAssetId(input, rapidJsonValueTexture, "FallbackTextureAssetId");

			// Optional RGB hardware gamma correction
			bool rgbHardwareGammaCorrection = false;
			JsonHelper::optionalBooleanProperty(rapidJsonValueTexture, "RgbHardwareGammaCorrection", rgbHardwareGammaCorrection);

			// "MipmapsUsed" with the default value "TRUE" isn't used, but it should be defined if mipmaps are not used to support debugging and optimization possibility spotting

			// Mandatory usage
			const RendererRuntime::MaterialProperty::Usage usage = mandatoryMaterialPropertyUsage(rapidJsonValueTexture);
			const RendererRuntime::MaterialProperty::ValueType valueType = mandatoryMaterialPropertyValueType(rapidJsonValueTexture);
			switch (usage)
			{
				case RendererRuntime::MaterialProperty::Usage::STATIC:
				{
					if (RendererRuntime::MaterialProperty::ValueType::TEXTURE_ASSET_ID == valueType)
					{
						// Mandatory asset ID
						const RendererRuntime::MaterialPropertyValue materialPropertyValue = RendererRuntime::MaterialPropertyValue::fromTextureAssetId(StringHelper::getAssetIdByString(rapidJsonValueTexture["Value"].GetString(), input));

						// Write down the texture
						const RendererRuntime::v1MaterialBlueprint::Texture materialBlueprintTexture(rootParameterIndex, RendererRuntime::MaterialProperty(RendererRuntime::getUninitialized<RendererRuntime::MaterialPropertyId>(), usage, materialPropertyValue), fallbackTextureAssetId, rgbHardwareGammaCorrection);
						outputMemoryStream.write(reinterpret_cast<const char*>(&materialBlueprintTexture), sizeof(RendererRuntime::v1MaterialBlueprint::Texture));

						// TODO(co) Error handling: Compiled asset ID not found (meaning invalid source asset ID given)
						break;
					}
					else
					{
						throw std::runtime_error("Textures with \"STATIC\"-usage must have the value type \"TEXTURE_ASSET_ID\"");
					}
				}

				case RendererRuntime::MaterialProperty::Usage::MATERIAL_REFERENCE:
				{
					if (RendererRuntime::MaterialProperty::ValueType::TEXTURE_ASSET_ID == valueType)
					{
						// Get mandatory asset ID
						// -> The character "@" is used to reference a material property value
						const std::string sourceAssetIdAsString = rapidJsonValueTexture["Value"].GetString();
						if (sourceAssetIdAsString.length() > 0 && sourceAssetIdAsString[0] == '@')
						{
							// Reference a material property value
							const RendererRuntime::MaterialPropertyId materialPropertyId(sourceAssetIdAsString.substr(1).c_str());

							// Figure out the material property value
							RendererRuntime::MaterialProperties::SortedPropertyVector::const_iterator iterator = std::lower_bound(sortedMaterialPropertyVector.cbegin(), sortedMaterialPropertyVector.cend(), materialPropertyId, RendererRuntime::detail::OrderByMaterialPropertyId());
							if (iterator != sortedMaterialPropertyVector.end())
							{
								const RendererRuntime::MaterialProperty& materialProperty = *iterator;
								if (materialProperty.getMaterialPropertyId() == materialPropertyId)
								{
									// TODO(co) Error handling: Usage mismatch etc.

									// Write down the texture
									const RendererRuntime::v1MaterialBlueprint::Texture materialBlueprintTexture(rootParameterIndex, RendererRuntime::MaterialProperty(materialPropertyId, usage, materialProperty), fallbackTextureAssetId, rgbHardwareGammaCorrection);
									outputMemoryStream.write(reinterpret_cast<const char*>(&materialBlueprintTexture), sizeof(RendererRuntime::v1MaterialBlueprint::Texture));
								}
							}
						}
						else
						{
							throw std::runtime_error("Textures with \"MATERIAL_REFERENCE\"-usage and the value type \"TEXTURE_ASSET_ID\" must have a value starting with @");
						}
					}
					else
					{
						throw std::runtime_error("Textures with \"MATERIAL_REFERENCE\"-usage must have the value type \"TEXTURE_ASSET_ID\"");
					}
					break;
				}

				case RendererRuntime::MaterialProperty::Usage::UNKNOWN:
				case RendererRuntime::MaterialProperty::Usage::SHADER_UNIFORM:
				case RendererRuntime::MaterialProperty::Usage::SHADER_COMBINATION:
				case RendererRuntime::MaterialProperty::Usage::RASTERIZER_STATE:
				case RendererRuntime::MaterialProperty::Usage::DEPTH_STENCIL_STATE:
				case RendererRuntime::MaterialProperty::Usage::BLEND_STATE:
				case RendererRuntime::MaterialProperty::Usage::SAMPLER_STATE:
				case RendererRuntime::MaterialProperty::Usage::TEXTURE_REFERENCE:
				case RendererRuntime::MaterialProperty::Usage::GLOBAL_REFERENCE:
				case RendererRuntime::MaterialProperty::Usage::UNKNOWN_REFERENCE:
				case RendererRuntime::MaterialProperty::Usage::PASS_REFERENCE:
				case RendererRuntime::MaterialProperty::Usage::INSTANCE_REFERENCE:
				case RendererRuntime::MaterialProperty::Usage::GLOBAL_REFERENCE_FALLBACK:
				default:
				{
					throw std::runtime_error("Invalid texture usage");
				}
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
