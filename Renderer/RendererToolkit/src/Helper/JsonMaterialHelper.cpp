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
#include "RendererToolkit/Helper/JsonMaterialHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintFileFormat.h>

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t JsonMaterialHelper::getCompiledAssetId(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonShaderBlueprintsObject, const std::string& propertyName)
	{
		const uint32_t sourceAssetId = static_cast<uint32_t>(std::atoi(jsonShaderBlueprintsObject->get(propertyName).convert<std::string>().c_str()));
		SourceAssetIdToCompiledAssetId::const_iterator iterator = input.sourceAssetIdToCompiledAssetId.find(sourceAssetId);
		const uint32_t compiledAssetId = (iterator != input.sourceAssetIdToCompiledAssetId.cend()) ? iterator->second : 0;
		// TODO(co) Error handling: Compiled asset ID not found (meaning invalid source asset ID given)
		return compiledAssetId;
	}

	void JsonMaterialHelper::optionalFillModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::FillMode& value)
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

	void JsonMaterialHelper::optionalCullModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::CullMode& value)
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

	void JsonMaterialHelper::optionalConservativeRasterizationModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::ConservativeRasterizationMode& value)
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

	void JsonMaterialHelper::optionalDepthWriteMaskProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::DepthWriteMask& value)
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

	void JsonMaterialHelper::optionalStencilOpProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::StencilOp& value)
	{
		Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
		if (!jsonDynamicVar.isEmpty())
		{
			const std::string valueAsString = jsonDynamicVar.convert<std::string>();

			// Define helper macros
			#define IF_VALUE(name)			 if (#name == valueAsString) value = Renderer::StencilOp::name;
			#define ELSE_IF_VALUE(name) else if (#name == valueAsString) value = Renderer::StencilOp::name;

			// Evaluate value
			IF_VALUE(KEEP)
			ELSE_IF_VALUE(ZERO)
			ELSE_IF_VALUE(REPLACE)
			ELSE_IF_VALUE(INCR_SAT)
			ELSE_IF_VALUE(DECR_SAT)
			ELSE_IF_VALUE(INVERT)
			ELSE_IF_VALUE(INCREASE)
			ELSE_IF_VALUE(DECREASE)
			else
			{
				// TODO(co) Error handling
			}

			// Undefine helper macros
			#undef IF_VALUE
			#undef ELSE_IF_VALUE
		}
	}

	void JsonMaterialHelper::optionalBlendProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::Blend& value)
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

	void JsonMaterialHelper::optionalBlendOpProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::BlendOp& value)
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

	void JsonMaterialHelper::optionalShaderVisibilityProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::ShaderVisibility& value)
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

	void JsonMaterialHelper::optionalFilterProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::FilterMode& value)
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

	void JsonMaterialHelper::optionalTextureAddressModeProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::TextureAddressMode& value)
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

	void JsonMaterialHelper::optionalComparisonFuncProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, Renderer::ComparisonFunc& value)
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

	RendererRuntime::MaterialProperty::Usage JsonMaterialHelper::mandatoryMaterialPropertyUsage(Poco::JSON::Object::Ptr jsonObject)
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
		ELSE_IF_VALUE(TEXTURE)
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

	RendererRuntime::MaterialPropertyValue JsonMaterialHelper::mandatoryMaterialPropertyValue(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonObject)
	{
		// Get the material property value type, first
		RendererRuntime::MaterialProperty::ValueType valueType = RendererRuntime::MaterialProperty::ValueType::UNKNOWN;
		{
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
		}

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
				JsonHelper::optionalBooleanProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromBoolean(0 != value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER:
			{
				int value = 0;
				JsonHelper::optionalIntegerProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromInteger(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_2:
			{
				int value[2] = { 0, 0 };
				JsonHelper::optionalIntegerNProperty(jsonObject, "DefaultValue", value, 2);
				return RendererRuntime::MaterialPropertyValue::fromInteger2(value[0], value[1]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_3:
			{
				int value[3] = { 0, 0, 0 };
				JsonHelper::optionalIntegerNProperty(jsonObject, "DefaultValue", value, 3);
				return RendererRuntime::MaterialPropertyValue::fromInteger3(value[0], value[1], value[2]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_4:
			{
				int value[4] = { 0, 0, 0, 0 };
				JsonHelper::optionalIntegerNProperty(jsonObject, "DefaultValue", value, 4);
				return RendererRuntime::MaterialPropertyValue::fromInteger4(value[0], value[1], value[2], value[3]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT:
			{
				float value = 0.0f;
				JsonHelper::optionalFloatProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromFloat(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_2:
			{
				float value[2] = { 0.0f, 0.0f };
				JsonHelper::optionalFloatNProperty(jsonObject, "DefaultValue", value, 2);
				return RendererRuntime::MaterialPropertyValue::fromFloat2(value[0], value[1]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_3:
			{
				float value[3] = { 0.0f, 0.0f, 0.0f };
				JsonHelper::optionalFloatNProperty(jsonObject, "DefaultValue", value, 3);
				return RendererRuntime::MaterialPropertyValue::fromFloat3(value[0], value[1], value[2]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_4:
			{
				float value[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				JsonHelper::optionalFloatNProperty(jsonObject, "DefaultValue", value, 4);
				return RendererRuntime::MaterialPropertyValue::fromFloat4(value[0], value[1], value[2], value[3]);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FILL_MODE:
			{
				Renderer::FillMode value = Renderer::FillMode::SOLID;
				optionalFillModeProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromFillMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::CULL_MODE:
			{
				Renderer::CullMode value = Renderer::CullMode::BACK;
				optionalCullModeProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromCullMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::CONSERVATIVE_RASTERIZATION_MODE:
			{
				Renderer::ConservativeRasterizationMode value = Renderer::ConservativeRasterizationMode::OFF;
				optionalConservativeRasterizationModeProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromConservativeRasterizationMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::DEPTH_WRITE_MASK:
			{
				Renderer::DepthWriteMask value = Renderer::DepthWriteMask::ALL;
				optionalDepthWriteMaskProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromDepthWriteMask(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::STENCIL_OP:
			{
				Renderer::StencilOp value = Renderer::StencilOp::KEEP;
				optionalStencilOpProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromStencilOp(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::COMPARISON_FUNC:
			{
				Renderer::ComparisonFunc value = Renderer::ComparisonFunc::LESS;
				optionalComparisonFuncProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromComparisonFunc(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::BLEND:
			{
				Renderer::Blend value = Renderer::Blend::ONE;
				optionalBlendProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromBlend(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::BLEND_OP:
			{
				Renderer::BlendOp value = Renderer::BlendOp::ADD;
				optionalBlendOpProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromBlendOp(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::FILTER_MODE:
			{
				Renderer::FilterMode value = Renderer::FilterMode::MIN_MAG_MIP_LINEAR;
				optionalFilterProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromFilterMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::TEXTURE_ADDRESS_MODE:
			{
				Renderer::TextureAddressMode value = Renderer::TextureAddressMode::CLAMP;
				optionalTextureAddressModeProperty(jsonObject, "DefaultValue", value);
				return RendererRuntime::MaterialPropertyValue::fromTextureAddressMode(value);
			}

			case RendererRuntime::MaterialPropertyValue::ValueType::ASSET_ID:
			{
				return RendererRuntime::MaterialPropertyValue::fromAssetId(getCompiledAssetId(input, jsonObject, "DefaultValue"));
			}
		}

		// TODO(co) Error, we should never ever end up in here
		return RendererRuntime::MaterialPropertyValue::fromBoolean(false);
	}

	void JsonMaterialHelper::readRootSignature(Poco::JSON::Object::Ptr jsonRootSignatureObject, std::ofstream& outputFileStream)
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

	void JsonMaterialHelper::readProperties(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonPropertiesObject, std::ofstream& outputFileStream)
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

			// Material property default value
			const RendererRuntime::MaterialPropertyValue materialPropertyValue = mandatoryMaterialPropertyValue(input, jsonPropertyObject);

			// Write down the material property
			const RendererRuntime::MaterialProperty materialProperty(materialPropertyId, usage, materialPropertyValue);
			outputFileStream.write(reinterpret_cast<const char*>(&materialProperty), sizeof(RendererRuntime::MaterialProperty));

			// Next property, please
			++propertiesIterator;
		}
	}

	void JsonMaterialHelper::readPipelineStateObject(Poco::JSON::Object::Ptr jsonPipelineStateObject, std::ofstream& outputFileStream)
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
				JsonHelper::optionalBooleanProperty(jsonRasterizerStateObject, "FrontCounterClockwise", rasterizerState.frontCounterClockwise);
				JsonHelper::optionalIntegerProperty(jsonRasterizerStateObject, "DepthBias", rasterizerState.depthBias);
				JsonHelper::optionalFloatProperty(jsonRasterizerStateObject, "DepthBiasClamp", rasterizerState.depthBiasClamp);
				JsonHelper::optionalFloatProperty(jsonRasterizerStateObject, "SlopeScaledDepthBias", rasterizerState.slopeScaledDepthBias);
				JsonHelper::optionalBooleanProperty(jsonRasterizerStateObject, "DepthClipEnable", rasterizerState.depthClipEnable);
				JsonHelper::optionalBooleanProperty(jsonRasterizerStateObject, "MultisampleEnable", rasterizerState.multisampleEnable);
				JsonHelper::optionalBooleanProperty(jsonRasterizerStateObject, "AntialiasedLineEnable", rasterizerState.antialiasedLineEnable);
				JsonHelper::optionalIntegerProperty(jsonRasterizerStateObject, "ForcedSampleCount", rasterizerState.forcedSampleCount);
				optionalConservativeRasterizationModeProperty(jsonRasterizerStateObject, "ConservativeRasterizationMode", rasterizerState.conservativeRasterizationMode);
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

	void JsonMaterialHelper::readSamplerStates(Poco::JSON::Object::Ptr jsonSamplerStatesObject, std::ofstream& outputFileStream)
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
			JsonHelper::optionalIntegerProperty(jsonSamplerStateObject, "SamplerRootParameterIndex", materialBlueprintSamplerState.samplerRootParameterIndex);
			optionalFilterProperty(jsonSamplerStateObject, "Filter", samplerState.filter);
			optionalTextureAddressModeProperty(jsonSamplerStateObject, "AddressU", samplerState.addressU);
			optionalTextureAddressModeProperty(jsonSamplerStateObject, "AddressV", samplerState.addressV);
			optionalTextureAddressModeProperty(jsonSamplerStateObject, "AddressW", samplerState.addressW);
			JsonHelper::optionalFloatProperty(jsonSamplerStateObject, "MipLODBias", samplerState.mipLODBias);
			JsonHelper::optionalIntegerProperty(jsonSamplerStateObject, "MaxAnisotropy", samplerState.maxAnisotropy);
			optionalComparisonFuncProperty(jsonSamplerStateObject, "ComparisonFunc", samplerState.comparisonFunc);
			JsonHelper::optionalFloatNProperty(jsonSamplerStateObject, "BorderColor", samplerState.borderColor, 4);
			JsonHelper::optionalFloatProperty(jsonSamplerStateObject, "MinLOD", samplerState.minLOD);
			JsonHelper::optionalFloatProperty(jsonSamplerStateObject, "MaxLOD", samplerState.maxLOD);

			// Write down the sampler state
			outputFileStream.write(reinterpret_cast<const char*>(&materialBlueprintSamplerState), sizeof(RendererRuntime::v1MaterialBlueprint::SamplerState));

			// Next sampler state, please
			++rootSamplerStatesIterator;
		}
	}

	void JsonMaterialHelper::readTextures(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonTexturesObject, std::ofstream& outputFileStream)
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
			JsonHelper::optionalIntegerProperty(jsonTextureObject, "TextureRootParameterIndex", materialBlueprintTexture.textureRootParameterIndex);

			// The mandatory properties
			materialBlueprintTexture.textureAssetId = getCompiledAssetId(input, jsonTextureObject, "TextureAssetId");

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
