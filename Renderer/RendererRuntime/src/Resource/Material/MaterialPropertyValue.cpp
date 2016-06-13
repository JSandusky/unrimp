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
#include "RendererRuntime/Resource/Material/MaterialPropertyValue.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t MaterialPropertyValue::getValueTypeNumberOfBytes(ValueType valueType)
	{
		switch (valueType)
		{
			case ValueType::UNKNOWN:
				return 0;

			case ValueType::BOOLEAN:
				return sizeof(bool);

			case ValueType::INTEGER:
				return sizeof(int);

			case ValueType::INTEGER_2:
				return sizeof(int) * 2;

			case ValueType::INTEGER_3:
				return sizeof(int) * 3;

			case ValueType::INTEGER_4:
				return sizeof(int) * 4;

			case ValueType::FLOAT:
				return sizeof(float);

			case ValueType::FLOAT_2:
				return sizeof(float) * 2;

			case ValueType::FLOAT_3:
				return sizeof(float) * 3;

			case ValueType::FLOAT_4:
				return sizeof(float) * 4;

			case ValueType::FLOAT_3_3:
				return sizeof(float) * 3 * 3;

			case ValueType::FLOAT_4_4:
				return sizeof(float) * 4 * 4;

			case ValueType::FILL_MODE:
				return sizeof(Renderer::FillMode);

			case ValueType::CULL_MODE:
				return sizeof(Renderer::CullMode);

			case ValueType::CONSERVATIVE_RASTERIZATION_MODE:
				return sizeof(Renderer::ConservativeRasterizationMode);

			case ValueType::DEPTH_WRITE_MASK:
				return sizeof(Renderer::DepthWriteMask);

			case ValueType::STENCIL_OP:
				return sizeof(Renderer::StencilOp);

			case ValueType::COMPARISON_FUNC:
				return sizeof(Renderer::ComparisonFunc);

			case ValueType::BLEND:
				return sizeof(Renderer::Blend);

			case ValueType::BLEND_OP:
				return sizeof(Renderer::BlendOp);

			case ValueType::FILTER_MODE:
				return sizeof(Renderer::FilterMode);

			case ValueType::TEXTURE_ADDRESS_MODE:
				return sizeof(Renderer::TextureAddressMode);

			case ValueType::TEXTURE_ASSET_ID:
				return sizeof(AssetId);

			case ValueType::COMPOSITOR_TEXTURE_REFERENCE:
				return sizeof(CompositorTextureReference);
		}

		// Error, we should never ever end up in here
		return 0;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
