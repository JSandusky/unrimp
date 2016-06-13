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
#include "RendererToolkit/Helper/JsonMaterialHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintFileFormat.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#pragma warning(disable: 4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#pragma warning(disable: 4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	#pragma warning(disable: 4619)	// warning C4619: #pragma warning: there is no warning number '4351'
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
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	void JsonMaterialHelper::optionalFillModeProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::FillMode& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValueType.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::FillMode::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::FillMode::name;

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

	void JsonMaterialHelper::optionalCullModeProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::CullMode& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValueType.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::CullMode::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::CullMode::name;

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

	void JsonMaterialHelper::optionalConservativeRasterizationModeProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::ConservativeRasterizationMode& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValueType.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::ConservativeRasterizationMode::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::ConservativeRasterizationMode::name;

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

	void JsonMaterialHelper::optionalDepthWriteMaskProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::DepthWriteMask& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValueType.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::DepthWriteMask::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::DepthWriteMask::name;

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

	void JsonMaterialHelper::optionalStencilOpProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::StencilOp& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValueType.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::StencilOp::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::StencilOp::name;

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

	void JsonMaterialHelper::optionalBlendProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::Blend& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValueType.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::Blend::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::Blend::name;

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

	void JsonMaterialHelper::optionalBlendOpProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::BlendOp& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValueType.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::BlendOp::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::BlendOp::name;

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

	void JsonMaterialHelper::optionalFilterProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::FilterMode& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValueType.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::FilterMode::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::FilterMode::name;

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

	void JsonMaterialHelper::optionalTextureAddressModeProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::TextureAddressMode& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValueType.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::TextureAddressMode::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::TextureAddressMode::name;

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

	void JsonMaterialHelper::optionalComparisonFuncProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::ComparisonFunc& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValueType.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::ComparisonFunc::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = Renderer::ComparisonFunc::name;

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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
