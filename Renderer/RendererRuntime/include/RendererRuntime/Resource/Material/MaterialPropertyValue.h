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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Core/StringId.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId AssetId;	///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>" (Example: "Example/Font/Default/LinBiolinum_R" will result in asset ID 64363173)


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Material property value
	*
	*  @remarks
	*    Special property value types
	*    - Reference value types to have properties referencing other data
	*    - Declaration only property for value types were we don't need to store a material property value, but only need to know the value type
	*      (examples are float 3x3 and float 4x4 which would blow up the number of bytes required per material property value without a real usage)
	*/
	class MaterialPropertyValue
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MaterialProperty;	// Needs access to the constructor for the reference usage


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Value type
		*/
		enum class ValueType : uint8_t
		{
			UNKNOWN = 0,						///< Value type not known
			BOOLEAN,							///< Boolean value
			INTEGER,							///< Integer value
			INTEGER_2,							///< Integer value with two components
			INTEGER_3,							///< Integer value with three components
			INTEGER_4,							///< Integer value with four components
			FLOAT,								///< Float value
			FLOAT_2,							///< Float value with two components
			FLOAT_3,							///< Float value with three components
			FLOAT_4,							///< Float value with four components
			FLOAT_3_3,							///< Float value with nine components, declaration property only
			FLOAT_4_4,							///< Float value with sixteen components, declaration property only
			// For pipeline rasterizer state property usage
			FILL_MODE,							//< Pipeline rasterizer state fill mode with possible values: "WIREFRAME", "SOLID"
			CULL_MODE,							//< Pipeline rasterizer state cull mode with possible values: "NONE", "FRONT", "BACK"
			CONSERVATIVE_RASTERIZATION_MODE,	//< Pipeline rasterizer state conservative rasterization mode with possible values: "OFF", "ON"
			// For pipeline depth stencil state property usage
			DEPTH_WRITE_MASK,					//< Pipeline depth stencil state depth write mask with possible values: "ZERO", "ALL"
			STENCIL_OP,							//< Pipeline depth stencil state stencil function with possible values: "KEEP", "ZERO", "REPLACE", "INCR_SAT", "DECR_SAT", "INVERT", "INCREASE", "DECREASE"
			// For pipeline depth stencil state and sampler state property usage
			COMPARISON_FUNC,					//< Pipeline depth stencil state and sampler state comparison function with possible values: "NEVER", "LESS", "EQUAL", "LESS_EQUAL", "GREATER", "NOT_EQUAL", "GREATER_EQUAL", "ALWAYS"
			// For pipeline blend state property usage
			BLEND,								//< Pipeline blend state blend with possible values: "ZERO", "ONE", "SRC_COLOR", "INV_SRC_COLOR", "SRC_ALPHA", "INV_SRC_ALPHA", "DEST_ALPHA", "INV_DEST_ALPHA", "DEST_COLOR", "INV_DEST_COLOR", "SRC_ALPHA_SAT", "BLEND_FACTOR", "INV_BLEND_FACTOR", "SRC_1_COLOR", "INV_SRC_1_COLOR", "SRC_1_ALPHA", "INV_SRC_1_ALPHA"
			BLEND_OP,							//< Pipeline blend state blend operation with possible values: "ADD", "SUBTRACT", "REV_SUBTRACT", "MIN", "MAX"
			// For sampler state property usage
			FILTER_MODE,						//< Sampler state filter mode with possible values: "MIN_MAG_MIP_POINT", "MIN_MAG_POINT_MIP_LINEAR", "MIN_POINT_MAG_LINEAR_MIP_POINT", "MIN_POINT_MAG_MIP_LINEAR", "MIN_LINEAR_MAG_MIP_POINT", "MIN_LINEAR_MAG_POINT_MIP_LINEAR", "MIN_MAG_LINEAR_MIP_POINT", "MIN_MAG_MIP_LINEAR", "ANISOTROPIC", "COMPARISON_MIN_MAG_MIP_POINT", "COMPARISON_MIN_MAG_POINT_MIP_LINEAR", "COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT", "COMPARISON_MIN_POINT_MAG_MIP_LINEAR", "COMPARISON_MIN_LINEAR_MAG_MIP_POINT", "COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR", "COMPARISON_MIN_MAG_LINEAR_MIP_POINT", "COMPARISON_MIN_MAG_MIP_LINEAR", "COMPARISON_ANISOTROPIC"
			TEXTURE_ADDRESS_MODE,				//< Sampler state texture address mode with possible values: "WRAP", "MIRROR", "CLAMP", "BORDER", "MIRROR_ONCE"
			// For texture property usage
			ASSET_ID							///< Asset ID
		};


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT static uint32_t getValueTypeNumberOfBytes(ValueType valueType);
		inline static MaterialPropertyValue fromUnknown();
		inline static MaterialPropertyValue fromBoolean(bool value);
		inline static MaterialPropertyValue fromInteger(int value);
		inline static MaterialPropertyValue fromInteger2(int value0, int value1);
		inline static MaterialPropertyValue fromInteger3(int value0, int value1, int value2);
		inline static MaterialPropertyValue fromInteger4(int value0, int value1, int value2, int value3);
		inline static MaterialPropertyValue fromFloat(float value);
		inline static MaterialPropertyValue fromFloat2(float value0, float value1);
		inline static MaterialPropertyValue fromFloat3(float value0, float value1, float value2);
		inline static MaterialPropertyValue fromFloat4(float value0, float value1, float value2, float value3);
		inline static MaterialPropertyValue fromFloat3_3();	// Declaration property only
		inline static MaterialPropertyValue fromFloat4_4();	// Declaration property only
		// For pipeline rasterizer state property usage
		inline static MaterialPropertyValue fromFillMode(Renderer::FillMode value);
		inline static MaterialPropertyValue fromCullMode(Renderer::CullMode value);
		inline static MaterialPropertyValue fromConservativeRasterizationMode(Renderer::ConservativeRasterizationMode value);
		// For pipeline depth stencil state property usage
		inline static MaterialPropertyValue fromDepthWriteMask(Renderer::DepthWriteMask value);
		inline static MaterialPropertyValue fromStencilOp(Renderer::StencilOp value);
		// For pipeline depth stencil state and sampler state property usage
		inline static MaterialPropertyValue fromComparisonFunc(Renderer::ComparisonFunc value);
		// // For pipeline blend state property usage
		inline static MaterialPropertyValue fromBlend(Renderer::Blend value);
		inline static MaterialPropertyValue fromBlendOp(Renderer::BlendOp value);
		// For sampler state property usage
		inline static MaterialPropertyValue fromFilterMode(Renderer::FilterMode value);
		inline static MaterialPropertyValue fromTextureAddressMode(Renderer::TextureAddressMode value);
		// For texture property usage
		inline static MaterialPropertyValue fromAssetId(AssetId value);


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline ~MaterialPropertyValue();

		/**
		*  @brief
		*    Return the value type
		*
		*  @return
		*    The value type
		*/
		inline ValueType getValueType() const;

		//[-------------------------------------------------------]
		//[ Value getter                                          ]
		//[-------------------------------------------------------]
		inline const uint8_t* getData() const;
		inline bool getBooleanValue() const;
		inline int getIntegerValue() const;
		inline const int* getInteger2Value() const;
		inline const int* getInteger3Value() const;
		inline const int* getInteger4Value() const;
		inline float getFloatValue() const;
		inline const float* getFloat2Value() const;
		inline const float* getFloat3Value() const;
		inline const float* getFloat4Value() const;
		// inline const float* getFloat3_3Value() const;	// Declaration property only
		// inline const float* getFloat4_4Value() const;	// Declaration property only
		// For pipeline rasterizer state property usage
		inline Renderer::FillMode getFillModeValue() const;
		inline Renderer::CullMode getCullModeValue() const;
		inline Renderer::ConservativeRasterizationMode getConservativeRasterizationModeValue() const;
		// For pipeline depth stencil state property usage
		inline Renderer::DepthWriteMask getDepthWriteMaskValue() const;
		inline Renderer::StencilOp getStencilOpValue() const;
		// For pipeline depth stencil state and sampler state property usage
		inline Renderer::ComparisonFunc getComparisonFuncValue() const;
		// // For pipeline blend state property usage
		inline Renderer::Blend getBlendValue() const;
		inline Renderer::BlendOp getBlendOpValue() const;
		// For sampler state property usage
		inline Renderer::FilterMode getFilterMode() const;
		inline Renderer::TextureAddressMode getTextureAddressModeValue() const;
		// For texture property usage
		inline AssetId getAssetIdValue() const;


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		ValueType mValueType;

		/**
		*  @brief
		*    Value, depends on "RendererRuntime::MaterialProperty::ValueType"
		*/
		union Value
		{
			bool									Boolean;
			int										Integer;
			int										Integer2[2];
			int										Integer3[3];
			int										Integer4[4];
			float									Float;
			float									Float2[2];
			float									Float3[3];
			float									Float4[4];
			// float								Float3_3[9];	// Declaration property only
			// float								Float4_4[16];	// Declaration property only
			// For pipeline rasterizer state property usage
			Renderer::FillMode						FillMode;
			Renderer::CullMode						CullMode;
			Renderer::ConservativeRasterizationMode	ConservativeRasterizationMode;
			// For pipeline depth stencil state property usage
			Renderer::DepthWriteMask				DepthWriteMask;
			Renderer::StencilOp						StencilOp;
			// For pipeline depth stencil state and sampler state property usage
			Renderer::ComparisonFunc				ComparisonFunc;
			// // For pipeline blend state property usage
			Renderer::Blend							Blend;
			Renderer::BlendOp						BlendOp;
			// For sampler state property usage
			Renderer::FilterMode					FilterMode;
			Renderer::TextureAddressMode			TextureAddressMode;
			// For texture property usage
			uint32_t								AssetId;
		} mValue;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline MaterialPropertyValue();


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Material/MaterialPropertyValue.inl"
