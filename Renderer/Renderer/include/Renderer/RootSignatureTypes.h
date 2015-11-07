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
#include "Renderer/SamplerStateTypes.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Definitions                                           ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Descriptor range type
	*
	*  @note
	*    - These constants directly map to Direct3D 12 constants, do not change them
	*
	*  @see
	*    - "D3D12_DESCRIPTOR_RANGE_TYPE"-documentation for details
	*/
	struct DescriptorRangeType
	{
		enum Enum
		{
			SRV     = 0,
			UAV     = SRV + 1,
			CBV     = UAV + 1,
			SAMPLER = CBV + 1
		};
	};

	/**
	*  @brief
	*    Descriptor range
	*
	*  @note
	*    - "Renderer::DescriptorRange" is not identical to "D3D12_DESCRIPTOR_RANGE" because it had to be extended by information required by OpenGL
	*
	*  @see
	*    - "D3D12_DESCRIPTOR_RANGE"-documentation for details
	*/
	struct DescriptorRange
	{
		DescriptorRangeType::Enum rangeType;
		uint32_t				  numberOfDescriptors;
		uint32_t				  baseShaderRegister;					///< When using explicit binding locations
		uint32_t				  registerSpace;
		uint32_t				  offsetInDescriptorsFromTableStart;

		// The rest is not part of "D3D12_DESCRIPTOR_RANGE" and was added to support OpenGL and Direct3D 9 as well
		static const uint32_t NAME_LENGTH = 32;
		char					  baseShaderRegisterName[NAME_LENGTH];	///< When not using explicit binding locations (OpenGL ES 2, legacy GLSL profiles)
		uint32_t				  samplerRootParameterIndex;			///< In case this is a texture, the sampler root parameter index is required for OpenGL, OpenGL ES 2 and DirectX 9
	};
	struct DescriptorRangeBuilder : public DescriptorRange
	{
		static const uint32_t OFFSET_APPEND	= 0xffffffff;
		DescriptorRangeBuilder()
		{
		}
		explicit DescriptorRangeBuilder(const DescriptorRangeBuilder&)
		{
		}
		DescriptorRangeBuilder(
			DescriptorRangeType::Enum _rangeType,
			uint32_t _numberOfDescriptors,
			uint32_t _baseShaderRegister,
			char	 _baseShaderRegisterName[NAME_LENGTH],
			uint32_t _samplerRootParameterIndex,
			uint32_t _registerSpace = 0,
			uint32_t _offsetInDescriptorsFromTableStart = OFFSET_APPEND)
		{
			initialize(_rangeType, _numberOfDescriptors, _baseShaderRegister, _baseShaderRegisterName, _samplerRootParameterIndex, _registerSpace, _offsetInDescriptorsFromTableStart);
		}
		inline void initializeSampler(
			uint32_t _numberOfDescriptors,
			uint32_t _baseShaderRegister,
			uint32_t _registerSpace = 0,
			uint32_t _offsetInDescriptorsFromTableStart = OFFSET_APPEND)
		{
			initialize(*this, DescriptorRangeType::SAMPLER, _numberOfDescriptors, _baseShaderRegister, "", 0, _registerSpace, _offsetInDescriptorsFromTableStart);
		}
		inline void initialize(
			DescriptorRangeType::Enum _rangeType,
			uint32_t _numberOfDescriptors,
			uint32_t _baseShaderRegister,
			char	 _baseShaderRegisterName[NAME_LENGTH],
			uint32_t _samplerRootParameterIndex,
			uint32_t _registerSpace = 0,
			uint32_t _offsetInDescriptorsFromTableStart = OFFSET_APPEND)
		{
			initialize(*this, _rangeType, _numberOfDescriptors, _baseShaderRegister, _baseShaderRegisterName, _samplerRootParameterIndex, _registerSpace, _offsetInDescriptorsFromTableStart);
		}
		static inline void initialize(
			DescriptorRange& range,
			DescriptorRangeType::Enum _rangeType,
			uint32_t _numberOfDescriptors,
			uint32_t _baseShaderRegister,
			char	 _baseShaderRegisterName[NAME_LENGTH],
			uint32_t _samplerRootParameterIndex,
			uint32_t _registerSpace = 0,
			uint32_t _offsetInDescriptorsFromTableStart = OFFSET_APPEND)
		{
			range.rangeType = _rangeType;
			range.numberOfDescriptors = _numberOfDescriptors;
			range.baseShaderRegister = _baseShaderRegister;
			range.registerSpace = _registerSpace;
			range.offsetInDescriptorsFromTableStart = _offsetInDescriptorsFromTableStart;
			strcpy(range.baseShaderRegisterName, _baseShaderRegisterName);
			range.samplerRootParameterIndex = _samplerRootParameterIndex;
		}
	};

	/**
	*  @brief
	*    Root descriptor table
	*
	*  @note
	*    - This structure directly maps to Direct3D 12 structure, do not change it
	*
	*  @see
	*    - "D3D12_ROOT_DESCRIPTOR_TABLE"-documentation for details
	*/
	struct RootDescriptorTable
	{
		uint32_t			   numberOfDescriptorRanges;
		const DescriptorRange* descriptorRanges;
	};
	struct RootDescriptorTableBuilder : public RootDescriptorTable
	{
		RootDescriptorTableBuilder()
		{
		}
		explicit RootDescriptorTableBuilder(const RootDescriptorTableBuilder&)
		{
		}
		RootDescriptorTableBuilder(
			uint32_t _numberOfDescriptorRanges,
			const DescriptorRange* _descriptorRanges)
		{
			initialize(_numberOfDescriptorRanges, _descriptorRanges);
		}
		inline void initialize(
			uint32_t _numberOfDescriptorRanges,
			const DescriptorRange* _descriptorRanges)
		{
			initialize(*this, _numberOfDescriptorRanges, _descriptorRanges);
		}
		static inline void initialize(
			RootDescriptorTable &rootDescriptorTable,
			uint32_t _numberOfDescriptorRanges,
			const DescriptorRange* _descriptorRanges)
		{
			rootDescriptorTable.numberOfDescriptorRanges = _numberOfDescriptorRanges;
			rootDescriptorTable.descriptorRanges = _descriptorRanges;
		}
	};

	/**
	*  @brief
	*    Root parameter type
	*
	*  @note
	*    - These constants directly map to Direct3D 12 constants, do not change them
	*
	*  @see
	*    - "D3D12_ROOT_PARAMETER_TYPE"-documentation for details
	*/
	struct RootParameterType
	{
		enum Enum
		{
			DESCRIPTOR_TABLE = 0,
			CONSTANTS_32BIT  = DESCRIPTOR_TABLE + 1,
			CBV              = CONSTANTS_32BIT + 1,
			SRV              = CBV + 1,
			UAV              = SRV + 1
		};
	};

	/**
	*  @brief
	*    Root constants
	*
	*  @note
	*    - This structure directly maps to Direct3D 12 structure, do not change it
	*
	*  @see
	*    - "D3D12_ROOT_CONSTANTS"-documentation for details
	*/
	struct RootConstants
	{
		uint32_t shaderRegister;
		uint32_t registerSpace;
		uint32_t numberOf32BitValues;
	};
	struct RootConstantsBuilder : public RootConstants
	{
		RootConstantsBuilder()
		{
		}
		explicit RootConstantsBuilder(const RootConstantsBuilder&)
		{
		}
		RootConstantsBuilder(
			uint32_t _numberOf32BitValues,
			uint32_t _shaderRegister,
			uint32_t _registerSpace = 0)
		{
			initialize(_numberOf32BitValues, _shaderRegister, _registerSpace);
		}
		inline void initialize(
			uint32_t _numberOf32BitValues,
			uint32_t _shaderRegister,
			uint32_t _registerSpace = 0)
		{
			initialize(*this, _numberOf32BitValues, _shaderRegister, _registerSpace);
		}
		static inline void initialize(
			RootConstants& rootConstants,
			uint32_t _numberOf32BitValues,
			uint32_t _shaderRegister,
			uint32_t _registerSpace = 0)
		{
			rootConstants.numberOf32BitValues = _numberOf32BitValues;
			rootConstants.shaderRegister = _shaderRegister;
			rootConstants.registerSpace = _registerSpace;
		}
	};

	/**
	*  @brief
	*    Root descriptor
	*
	*  @note
	*    - This structure directly maps to Direct3D 12 structure, do not change it
	*
	*  @see
	*    - "D3D12_ROOT_DESCRIPTOR"-documentation for details
	*/
	struct RootDescriptor
	{
		uint32_t shaderRegister;
		uint32_t registerSpace;
	};
	struct RootDescriptorBuilder : public RootDescriptor
	{
		RootDescriptorBuilder()
		{
		}
		explicit RootDescriptorBuilder(const RootDescriptorBuilder&)
		{
		}
		RootDescriptorBuilder(
			uint32_t _shaderRegister,
			uint32_t _registerSpace = 0)
		{
			initialize(_shaderRegister, _registerSpace);
		}
		inline void initialize(
			uint32_t _shaderRegister,
			uint32_t _registerSpace = 0)
		{
			initialize(*this, _shaderRegister, _registerSpace);
		}
		static inline void initialize(RootDescriptor& table, uint32_t _shaderRegister, uint32_t _registerSpace = 0)
		{
			table.shaderRegister = _shaderRegister;
			table.registerSpace = _registerSpace;
		}
	};

	/**
	*  @brief
	*    Shader visibility
	*
	*  @note
	*    - These constants directly map to Direct3D 12 constants, do not change them
	*
	*  @see
	*    - "D3D12_SHADER_VISIBILITY"-documentation for details
	*/
	struct ShaderVisibility
	{
		enum Enum
		{
			ALL                     = 0,
			VERTEX                  = 1,
			TESSELLATION_CONTROL    = 2,
			TESSELLATION_EVALUATION = 3,
			GEOMETRY                = 4,
			FRAGMENT                = 5
		};
	};

	/**
	*  @brief
	*    Root parameter
	*
	*  @note
	*    - This structure directly maps to Direct3D 12 structure, do not change it
	*
	*  @see
	*    - "D3D12_ROOT_PARAMETER"-documentation for details
	*/
	struct RootParameter
	{
		RootParameterType::Enum	parameterType;
		union
		{
			RootDescriptorTable	descriptorTable;
			RootConstants		constants;
			RootDescriptor		descriptor;
		};
		ShaderVisibility::Enum	shaderVisibility;
	};
	struct RootParameterBuilder : public RootParameter
	{
		RootParameterBuilder()
		{
		}
		explicit RootParameterBuilder(const RootParameterBuilder&)
		{
		}
		static inline void initializeAsDescriptorTable(
			RootParameter& rootParam,
			uint32_t numberOfDescriptorRanges,
			const DescriptorRange* descriptorRanges,
			ShaderVisibility::Enum visibility = ShaderVisibility::ALL)
		{
			rootParam.parameterType = RootParameterType::DESCRIPTOR_TABLE;
			rootParam.shaderVisibility = visibility;
			RootDescriptorTableBuilder::initialize(rootParam.descriptorTable, numberOfDescriptorRanges, descriptorRanges);
		}
		static inline void initializeAsConstants(
			RootParameter& rootParam,
			uint32_t numberOf32BitValues,
			uint32_t shaderRegister,
			uint32_t registerSpace = 0,
			ShaderVisibility::Enum visibility = ShaderVisibility::ALL)
		{
			rootParam.parameterType = RootParameterType::CONSTANTS_32BIT;
			rootParam.shaderVisibility = visibility;
			RootConstantsBuilder::initialize(rootParam.constants, numberOf32BitValues, shaderRegister, registerSpace);
		}
		static inline void initializeAsConstantBufferView(
			RootParameter& rootParam,
			uint32_t shaderRegister,
			uint32_t registerSpace = 0,
			ShaderVisibility::Enum visibility = ShaderVisibility::ALL)
		{
			rootParam.parameterType = RootParameterType::CBV;
			rootParam.shaderVisibility = visibility;
			RootDescriptorBuilder::initialize(rootParam.descriptor, shaderRegister, registerSpace);
		}
		static inline void initializeAsShaderResourceView(
			RootParameter& rootParam,
			uint32_t shaderRegister,
			uint32_t registerSpace = 0,
			ShaderVisibility::Enum visibility = ShaderVisibility::ALL)
		{
			rootParam.parameterType = RootParameterType::SRV;
			rootParam.shaderVisibility = visibility;
			RootDescriptorBuilder::initialize(rootParam.descriptor, shaderRegister, registerSpace);
		}
		static inline void initializeAsUnorderedAccessView(
			RootParameter& rootParam,
			uint32_t shaderRegister,
			uint32_t registerSpace = 0,
			ShaderVisibility::Enum visibility = ShaderVisibility::ALL)
		{
			rootParam.parameterType = RootParameterType::UAV;
			rootParam.shaderVisibility = visibility;
			RootDescriptorBuilder::initialize(rootParam.descriptor, shaderRegister, registerSpace);
		}
		inline void initializeAsDescriptorTable(
			uint32_t numberOfDescriptorRanges,
			const DescriptorRange* descriptorRanges,
			ShaderVisibility::Enum visibility = ShaderVisibility::ALL)
		{
			initializeAsDescriptorTable(*this, numberOfDescriptorRanges, descriptorRanges, visibility);
		}
		inline void initializeAsConstants(
			uint32_t numberOf32BitValues,
			uint32_t shaderRegister,
			uint32_t registerSpace = 0,
			ShaderVisibility::Enum visibility = ShaderVisibility::ALL)
		{
			initializeAsConstants(*this, numberOf32BitValues, shaderRegister, registerSpace, visibility);
		}
		inline void initializeAsConstantBufferView(
			uint32_t shaderRegister,
			uint32_t registerSpace = 0,
			ShaderVisibility::Enum visibility = ShaderVisibility::ALL)
		{
			initializeAsConstantBufferView(*this, shaderRegister, registerSpace, visibility);
		}
		inline void initializeAsShaderResourceView(
			uint32_t shaderRegister,
			uint32_t registerSpace = 0,
			ShaderVisibility::Enum visibility = ShaderVisibility::ALL)
		{
			initializeAsShaderResourceView(*this, shaderRegister, registerSpace, visibility);
		}
		inline void initializeAsUnorderedAccessView(
			uint32_t shaderRegister,
			uint32_t registerSpace = 0,
			ShaderVisibility::Enum visibility = ShaderVisibility::ALL)
		{
			initializeAsUnorderedAccessView(*this, shaderRegister, registerSpace, visibility);
		}
	};

	/**
	*  @brief
	*    Root signature flags
	*
	*  @note
	*    - These constants directly map to Direct3D 12 constants, do not change them
	*
	*  @see
	*    - "D3D12_ROOT_SIGNATURE_FLAGS"-documentation for details
	*/
	struct RootSignatureFlags
	{
		enum Enum
		{
			NONE                                            = 0,
			ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT              = 0x1,
			DENY_VERTEX_SHADER_ROOT_ACCESS                  = 0x2,
			DENY_TESSELLATION_CONTROL_SHADER_ROOT_ACCESS    = 0x4,
			DENY_TESSELLATION_EVALUATION_SHADER_ROOT_ACCESS = 0x8,
			DENY_GEOMETRY_SHADER_ROOT_ACCESS                = 0x10,
			DENY_FRAGMENT_SHADER_ROOT_ACCESS                = 0x20,
			ALLOW_STREAM_OUTPUT                             = 0x40
		};
	};

	/**
	*  @brief
	*    Static border color
	*
	*  @note
	*    - These constants directly map to Direct3D 12 constants, do not change them
	*
	*  @see
	*    - "D3D12_STATIC_BORDER_COLOR"-documentation for details
	*/
	struct StaticBorderColor
	{
		enum Enum
		{
			TRANSPARENT_BLACK = 0,
			OPAQUE_BLACK      = TRANSPARENT_BLACK + 1,
			OPAQUE_WHITE      = OPAQUE_BLACK + 1
		};
	};

	/**
	*  @brief
	*    Static sampler
	*
	*  @note
	*    - This structure directly maps to Direct3D 12 structure, do not change it
	*
	*  @see
	*    - "D3D12_STATIC_SAMPLER_DESC"-documentation for details
	*/
	struct StaticSampler
	{
		FilterMode::Enum		 filter;
		TextureAddressMode::Enum addressU;
		TextureAddressMode::Enum addressV;
		TextureAddressMode::Enum addressW;
		float					 mipLodBias;
		uint32_t				 maxAnisotropy;
		ComparisonFunc::Enum	 comparisonFunc;
		StaticBorderColor::Enum  borderColor;
		float					 minLod;
		float					 maxLod;
		uint32_t				 shaderRegister;
		uint32_t				 registerSpace;
		ShaderVisibility::Enum	 shaderVisibility;
	};

	/**
	*  @brief
	*    Root signature
	*
	*  @note
	*    - "Renderer::RootSignature" is not totally identical to "D3D12_ROOT_SIGNATURE_DESC" because it had to be extended by information required by OpenGL, so can't cast to Direct3D 12 structure
	*    - In order to be renderer API independent, do always define and set samplers first
	*    - "Renderer::DescriptorRange": In order to be renderer API independent, do always provide "baseShaderRegisterName" and "samplerRootParameterIndex" for "Renderer::DescriptorRangeType::SRV" range types
	*
	*  @see
	*    - "D3D12_ROOT_SIGNATURE_DESC"-documentation for details
	*/
	struct RootSignature
	{
		uint32_t				 numberOfParameters;
		const RootParameter*	 parameters;
		uint32_t				 numberOfStaticSamplers;
		const StaticSampler*	 staticSamplers;
		RootSignatureFlags::Enum flags;
	};
	struct RootSignatureBuilder : public RootSignature
	{
		RootSignatureBuilder()
		{
		}
		explicit RootSignatureBuilder(const RootSignatureBuilder&)
		{
		}
		RootSignatureBuilder(
			uint32_t _numberOfParameters,
			const RootParameter* _parameters,
			uint32_t _numberOfStaticSamplers = 0,
			const StaticSampler* _staticSamplers = nullptr,
			RootSignatureFlags::Enum _flags = RootSignatureFlags::NONE)
		{
			initialize(_numberOfParameters, _parameters, _numberOfStaticSamplers, _staticSamplers, _flags);
		}
		inline void initialize(
			uint32_t _numberOfParameters,
			const RootParameter* _parameters,
			uint32_t _numberOfStaticSamplers = 0,
			const StaticSampler* _staticSamplers = nullptr,
			RootSignatureFlags::Enum _flags = RootSignatureFlags::NONE)
		{
			initialize(*this, _numberOfParameters, _parameters, _numberOfStaticSamplers, _staticSamplers, _flags);
		}
		static inline void initialize(
			RootSignature &rootSignature,
			uint32_t _numberOfParameters,
			const RootParameter* _parameters,
			uint32_t _numberOfStaticSamplers = 0,
			const StaticSampler* _staticSamplers = nullptr,
			RootSignatureFlags::Enum _flags = RootSignatureFlags::NONE)
		{
			rootSignature.numberOfParameters = _numberOfParameters;
			rootSignature.parameters = _parameters;
			rootSignature.numberOfStaticSamplers = _numberOfStaticSamplers;
			rootSignature.staticSamplers = _staticSamplers;
			rootSignature.flags = _flags;
		}

	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
