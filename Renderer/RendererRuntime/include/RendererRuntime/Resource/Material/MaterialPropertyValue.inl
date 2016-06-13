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
#include <assert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	inline MaterialPropertyValue MaterialPropertyValue::fromUnknown()
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType = ValueType::UNKNOWN;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromBoolean(bool value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType	 = ValueType::BOOLEAN;
		materialPropertyValue.mValue.Boolean = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromInteger(int value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType	 = ValueType::INTEGER;
		materialPropertyValue.mValue.Integer = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromInteger2(int value0, int value1)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType		 = ValueType::INTEGER_2;
		materialPropertyValue.mValue.Integer2[0] = value0;
		materialPropertyValue.mValue.Integer2[1] = value1;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromInteger3(int value0, int value1, int value2)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType		 = ValueType::INTEGER_3;
		materialPropertyValue.mValue.Integer3[0] = value0;
		materialPropertyValue.mValue.Integer3[1] = value1;
		materialPropertyValue.mValue.Integer3[2] = value2;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromInteger4(int value0, int value1, int value2, int value3)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType		 = ValueType::INTEGER_4;
		materialPropertyValue.mValue.Integer4[0] = value0;
		materialPropertyValue.mValue.Integer4[1] = value1;
		materialPropertyValue.mValue.Integer4[2] = value2;
		materialPropertyValue.mValue.Integer4[3] = value3;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromFloat(float value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType   = ValueType::FLOAT;
		materialPropertyValue.mValue.Float = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromFloat2(float value0, float value1)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType	   = ValueType::FLOAT_2;
		materialPropertyValue.mValue.Float2[0] = value0;
		materialPropertyValue.mValue.Float2[1] = value1;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromFloat3(float value0, float value1, float value2)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType	   = ValueType::FLOAT_3;
		materialPropertyValue.mValue.Float3[0] = value0;
		materialPropertyValue.mValue.Float3[1] = value1;
		materialPropertyValue.mValue.Float3[2] = value2;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromFloat4(float value0, float value1, float value2, float value3)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType	   = ValueType::FLOAT_4;
		materialPropertyValue.mValue.Float4[0] = value0;
		materialPropertyValue.mValue.Float4[1] = value1;
		materialPropertyValue.mValue.Float4[2] = value2;
		materialPropertyValue.mValue.Float4[3] = value3;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromFloat3_3()
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType = ValueType::FLOAT_3_3;
		materialPropertyValue.mValue.Boolean = false;	// Declaration property only
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromFloat4_4()
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType = ValueType::FLOAT_4_4;
		materialPropertyValue.mValue.Boolean = false;	// Declaration property only
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromFillMode(Renderer::FillMode value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType	  = ValueType::FILL_MODE;
		materialPropertyValue.mValue.FillMode = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromCullMode(Renderer::CullMode value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType	  = ValueType::CULL_MODE;
		materialPropertyValue.mValue.CullMode = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromConservativeRasterizationMode(Renderer::ConservativeRasterizationMode value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType						   = ValueType::CONSERVATIVE_RASTERIZATION_MODE;
		materialPropertyValue.mValue.ConservativeRasterizationMode = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromDepthWriteMask(Renderer::DepthWriteMask value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType			= ValueType::DEPTH_WRITE_MASK;
		materialPropertyValue.mValue.DepthWriteMask = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromStencilOp(Renderer::StencilOp value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType	   = ValueType::STENCIL_OP;
		materialPropertyValue.mValue.StencilOp = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromComparisonFunc(Renderer::ComparisonFunc value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType			= ValueType::COMPARISON_FUNC;
		materialPropertyValue.mValue.ComparisonFunc = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromBlend(Renderer::Blend value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType   = ValueType::BLEND;
		materialPropertyValue.mValue.Blend = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromBlendOp(Renderer::BlendOp value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType	 = ValueType::BLEND_OP;
		materialPropertyValue.mValue.BlendOp = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromFilterMode(Renderer::FilterMode value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType		= ValueType::FILTER_MODE;
		materialPropertyValue.mValue.FilterMode = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromTextureAddressMode(Renderer::TextureAddressMode value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType				= ValueType::TEXTURE_ADDRESS_MODE;
		materialPropertyValue.mValue.TextureAddressMode = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromTextureAssetId(AssetId value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType			= ValueType::TEXTURE_ASSET_ID;
		materialPropertyValue.mValue.TextureAssetId = value;
		return materialPropertyValue;
	}

	inline MaterialPropertyValue MaterialPropertyValue::fromCompositorTextureReference(const CompositorTextureReference& value)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType						= ValueType::COMPOSITOR_TEXTURE_REFERENCE;
		materialPropertyValue.mValue.CompositorTextureReference = value;
		return materialPropertyValue;
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline MaterialPropertyValue::~MaterialPropertyValue()
	{
		// Nothing here
	}

	inline MaterialPropertyValue::ValueType MaterialPropertyValue::getValueType() const
	{
		return mValueType;
	}

	inline const uint8_t* MaterialPropertyValue::getData() const
	{
		return reinterpret_cast<const uint8_t*>(&mValue);
	}

	inline bool MaterialPropertyValue::getBooleanValue() const
	{
		assert(ValueType::BOOLEAN == mValueType);
		return mValue.Boolean;
	}

	inline int MaterialPropertyValue::getIntegerValue() const
	{
		assert(ValueType::INTEGER == mValueType);
		return mValue.Integer;
	}

	inline const int* MaterialPropertyValue::getInteger2Value() const
	{
		assert(ValueType::INTEGER_2 == mValueType);
		return &mValue.Integer2[0];
	}

	inline const int* MaterialPropertyValue::getInteger3Value() const
	{
		assert(ValueType::INTEGER_3 == mValueType);
		return &mValue.Integer3[0];
	}

	inline const int* MaterialPropertyValue::getInteger4Value() const
	{
		assert(ValueType::INTEGER_4 == mValueType);
		return &mValue.Integer4[0];
	}

	inline float MaterialPropertyValue::getFloatValue() const
	{
		assert(ValueType::FLOAT == mValueType);
		return mValue.Float;
	}

	inline const float* MaterialPropertyValue::getFloat2Value() const
	{
		assert(ValueType::FLOAT_2 == mValueType);
		return &mValue.Float2[0];
	}

	inline const float* MaterialPropertyValue::getFloat3Value() const
	{
		assert(ValueType::FLOAT_3 == mValueType);
		return &mValue.Float3[0];
	}

	inline const float* MaterialPropertyValue::getFloat4Value() const
	{
		assert(ValueType::FLOAT_4 == mValueType);
		return &mValue.Float4[0];
	}

	inline Renderer::FillMode MaterialPropertyValue::getFillModeValue() const
	{
		assert(ValueType::FILL_MODE == mValueType);
		return mValue.FillMode;
	}

	inline Renderer::CullMode MaterialPropertyValue::getCullModeValue() const
	{
		assert(ValueType::CULL_MODE == mValueType);
		return mValue.CullMode;
	}

	inline Renderer::ConservativeRasterizationMode MaterialPropertyValue::getConservativeRasterizationModeValue() const
	{
		assert(ValueType::CONSERVATIVE_RASTERIZATION_MODE == mValueType);
		return mValue.ConservativeRasterizationMode;
	}

	inline Renderer::DepthWriteMask MaterialPropertyValue::getDepthWriteMaskValue() const
	{
		assert(ValueType::DEPTH_WRITE_MASK == mValueType);
		return mValue.DepthWriteMask;
	}

	inline Renderer::StencilOp MaterialPropertyValue::getStencilOpValue() const
	{
		assert(ValueType::STENCIL_OP == mValueType);
		return mValue.StencilOp;
	}

	inline Renderer::ComparisonFunc MaterialPropertyValue::getComparisonFuncValue() const
	{
		assert(ValueType::COMPARISON_FUNC == mValueType);
		return mValue.ComparisonFunc;
	}

	inline Renderer::Blend MaterialPropertyValue::getBlendValue() const
	{
		assert(ValueType::BLEND == mValueType);
		return mValue.Blend;
	}

	inline Renderer::BlendOp MaterialPropertyValue::getBlendOpValue() const
	{
		assert(ValueType::BLEND_OP == mValueType);
		return mValue.BlendOp;
	}

	inline Renderer::FilterMode MaterialPropertyValue::getFilterMode() const
	{
		assert(ValueType::FILTER_MODE == mValueType);
		return mValue.FilterMode;
	}

	inline Renderer::TextureAddressMode MaterialPropertyValue::getTextureAddressModeValue() const
	{
		assert(ValueType::TEXTURE_ADDRESS_MODE == mValueType);
		return mValue.TextureAddressMode;
	}

	inline AssetId MaterialPropertyValue::getTextureAssetIdValue() const
	{
		assert(ValueType::TEXTURE_ASSET_ID == mValueType);
		return mValue.TextureAssetId;
	}

	inline const MaterialPropertyValue::CompositorTextureReference& MaterialPropertyValue::getCompositorTextureReference() const
	{
		assert(ValueType::COMPOSITOR_TEXTURE_REFERENCE == mValueType);
		return mValue.CompositorTextureReference;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline MaterialPropertyValue::MaterialPropertyValue()
	{
		// Nothing here, no member initialization by intent in here (see "RendererRuntime::MaterialPropertyValue::fromBoolean()" etc.)
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
