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


// Public comfort header putting everything within a single header


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <inttypes.h>	// For uint32_t, uint64_t etc.


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderer;
	class IShaderLanguage;
	class IResource;
		class IRootSignature;
		class IProgram;
		class IVertexArray;
		class IRenderTarget;
			class ISwapChain;
			class IFramebuffer;
		class IBuffer;
			class IIndexBuffer;
			class IVertexBuffer;
			class IUniformBuffer;
		class ITexture;
			class ITexture2D;
			class ITextureBuffer;
			class ITexture2DArray;
		class IState;
			class IPipelineState;
			class IRasterizerState;
			class IDepthStencilState;
			class IBlendState;
			class ISamplerState;
		class IShader;
			class IVertexShader;
			class ITessellationControlShader;
			class ITessellationEvaluationShader;
			class IGeometryShader;
			class IFragmentShader;
		class ICollection;
			class ITextureCollection;
			class ISamplerStateCollection;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Types                                                 ]
	//[-------------------------------------------------------]
	// Renderer/PlatformTypes.h
	#ifndef __RENDERER_PLATFORM_TYPES_H__
	#define __RENDERER_PLATFORM_TYPES_H__
		#ifdef WIN32
			#if defined(__INTEL_COMPILER) || !defined(_MSC_VER) || _MSC_VER < 1600
				const class
				{
					public:
						template<class T> operator T*() const  { return 0; }
						template<class C, class T> operator T C::*() const  { return 0; }
					private:
						void operator&() const;
				} nullptr = {};
			#endif
			#ifdef X64_ARCHITECTURE
				typedef unsigned __int64 handle;
			#else
				typedef unsigned __int32 handle;
			#endif
			#ifndef NULL_HANDLE
				#define NULL_HANDLE 0
			#endif
			#define FORCEINLINE __forceinline
			#define NOP __nop()
		#elif LINUX
			#if (__GNUC__ == 4 && __GNUC_MINOR__ == 5 && __GNUC_PATCHLEVEL__ < 3)
				#error "Due to a bug in GCC 4.5.x ... 4.5.2 this GCC version is not supported. Please use a newer or older GCC version instead."
			#elif ((__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6)) && !defined(__clang__))
				const class
				{
					public:
						template<class T> operator T*() const  { return 0; }
						template<class C, class T> operator T C::*() const  { return 0; }
					private:
						void operator&() const;
				} nullptr = {};
			#endif
			#if ((__GNUC__ < 4 || __GNUC_MINOR__ < 7) && !defined(__clang__))
				#define override
			#endif
			#if X64_ARCHITECTURE
				typedef uint64_t handle;
			#else
				typedef uint32_t handle;
			#endif
			#ifndef NULL_HANDLE
				#define NULL_HANDLE 0
			#endif
			#define FORCEINLINE __attribute__((always_inline))
			#define NOP
		#else
			#error "Unsupported platform"
		#endif
		#ifdef RENDERER_NO_DEBUG
			#define RENDERER_SET_DEBUG_MARKER(renderer, name)
			#define RENDERER_SET_DEBUG_MARKER_FUNCTION(renderer)
			#define RENDERER_BEGIN_DEBUG_EVENT(renderer, name)
			#define RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)
			#define RENDERER_END_DEBUG_EVENT(renderer)
			#define RENDERER_SET_RESOURCE_DEBUG_NAME(resource, name)
		#else
			#define RENDERER_INTERNAL__WFUNCTION__2(x) L ## x
			#define RENDERER_INTERNAL__WFUNCTION__1(x) RENDERER_INTERNAL__WFUNCTION__2(x)
			#define RENDERER_INTERNAL__WFUNCTION__ RENDERER_INTERNAL__WFUNCTION__1(__FUNCTION__)
			#define RENDERER_SET_DEBUG_MARKER(renderer, name) if (nullptr != renderer) { (renderer)->setDebugMarker(name); }
			#define RENDERER_SET_DEBUG_MARKER_FUNCTION(renderer) if (nullptr != renderer) { (renderer)->setDebugMarker(RENDERER_INTERNAL__WFUNCTION__); }
			#define RENDERER_BEGIN_DEBUG_EVENT(renderer, name) if (nullptr != renderer) { (renderer)->beginDebugEvent(name); }
			#define RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer) if (nullptr != renderer) { (renderer)->beginDebugEvent(RENDERER_INTERNAL__WFUNCTION__); }
			#define RENDERER_END_DEBUG_EVENT(renderer) if (nullptr != renderer) { (renderer)->endDebugEvent(); }
			#define RENDERER_SET_RESOURCE_DEBUG_NAME(resource, name) if (nullptr != resource) { (resource)->setDebugName(name); }
		#endif
	#endif

	// Renderer/RendererTypes.h
	#ifndef __RENDERER_RENDERER_TYPES_H__
	#define __RENDERER_RENDERER_TYPES_H__
		struct MapType
		{
			enum Enum
			{
				READ			   = 1,
				WRITE			   = 2,
				READ_WRITE		   = 3,
				WRITE_DISCARD	   = 4,
				WRITE_NO_OVERWRITE = 5
			};
		};
		struct MapFlag
		{
			enum Enum
			{
				DO_NOT_WAIT = 0x100000L
			};
		};
		struct ClearFlag
		{
			enum Enum
			{
				COLOR       = 1<<0,
				DEPTH       = 1<<1,
				STENCIL     = 1<<2,
				COLOR_DEPTH = COLOR | DEPTH
			};
		};
		struct PrimitiveTopology
		{
			enum Enum
			{
				UNKNOWN        = 0,
				POINT_LIST     = 1,
				LINE_LIST      = 2,
				LINE_STRIP     = 3,
				TRIANGLE_LIST  = 4,
				TRIANGLE_STRIP = 5,
				PATCH_LIST_1   = 33,
				PATCH_LIST_2   = 34,
				PATCH_LIST_3   = 35,
				PATCH_LIST_4   = 36,
				PATCH_LIST_5   = 37,
				PATCH_LIST_6   = 38,
				PATCH_LIST_7   = 39,
				PATCH_LIST_8   = 40,
				PATCH_LIST_9   = 41,
				PATCH_LIST_10  = 42,
				PATCH_LIST_11  = 43,
				PATCH_LIST_12  = 44,
				PATCH_LIST_13  = 45,
				PATCH_LIST_14  = 46,
				PATCH_LIST_15  = 47,
				PATCH_LIST_16  = 48,
				PATCH_LIST_17  = 49,
				PATCH_LIST_18  = 50,
				PATCH_LIST_19  = 51,
				PATCH_LIST_20  = 52,
				PATCH_LIST_21  = 53,
				PATCH_LIST_22  = 54,
				PATCH_LIST_23  = 55,
				PATCH_LIST_24  = 56,
				PATCH_LIST_25  = 57,
				PATCH_LIST_26  = 58,
				PATCH_LIST_27  = 59,
				PATCH_LIST_28  = 60,
				PATCH_LIST_29  = 61,
				PATCH_LIST_30  = 62,
				PATCH_LIST_31  = 63,
				PATCH_LIST_32  = 64
			};
		};
		struct ComparisonFunc
		{
			enum Enum
			{
				NEVER		  = 1,
				LESS		  = 2,
				EQUAL		  = 3,
				LESS_EQUAL	  = 4,
				GREATER		  = 5,
				NOT_EQUAL	  = 6,
				GREATER_EQUAL = 7,
				ALWAYS		  = 8
			};
		};
		struct ColorWriteEnable
		{
			enum Enum
			{
				RED   = 1,
				GREEN = 2,
				BLUE  = 4,
				ALPHA = 8,
				ALL   = (((RED | GREEN)  | BLUE)  | ALPHA)
			};
		};
		struct MappedSubresource
		{
			void	 *data;
			uint32_t  rowPitch;
			uint32_t  depthPitch;
		};
		struct Viewport
		{
			float topLeftX;
			float topLeftY;
			float width;
			float height;
			float minDepth;
			float maxDepth;
		};
		struct ScissorRectangle
		{
			long topLeftX;
			long topLeftY;
			long bottomRightX;
			long bottomRightY;
		};
	#endif

	// Renderer/ResourceType.h
	#ifndef __RENDERER_RESOURCE_TYPES_H__
	#define __RENDERER_RESOURCE_TYPES_H__
		struct ResourceType
		{
			enum Enum
			{
				ROOT_SIGNATURE				   = 0,
				PROGRAM						   = 1,
				VERTEX_ARRAY				   = 2,
				SWAP_CHAIN					   = 3,
				FRAMEBUFFER					   = 4,
				INDEX_BUFFER				   = 5,
				VERTEX_BUFFER				   = 6,
				UNIFORM_BUFFER				   = 7,
				TEXTURE_BUFFER				   = 8,
				TEXTURE_2D					   = 9,
				TEXTURE_2D_ARRAY			   = 10,
				RASTERIZER_STATE			   = 11,
				PIPELINE_STATE				   = 12,
				DEPTH_STENCIL_STATE			   = 13,
				BLEND_STATE					   = 14,
				SAMPLER_STATE				   = 15,
				VERTEX_SHADER				   = 16,
				TESSELLATION_CONTROL_SHADER	   = 17,
				TESSELLATION_EVALUATION_SHADER = 18,
				GEOMETRY_SHADER				   = 19,
				FRAGMENT_SHADER				   = 20,
				TEXTURE_COLLECTION			   = 21,
				SAMPLER_STATE_COLLECTION	   = 22
			};
		};
	#endif

	// Renderer/SamplerStateTypes.h
	#ifndef __RENDERER_SAMPLERSTATE_TYPES_H__
	#define __RENDERER_SAMPLERSTATE_TYPES_H__
		struct FilterMode
		{
			enum Enum
			{
				MIN_MAG_MIP_POINT					 	   = 0,
				MIN_MAG_POINT_MIP_LINEAR			 	   = 0x1,
				MIN_POINT_MAG_LINEAR_MIP_POINT		 	   = 0x4,
				MIN_POINT_MAG_MIP_LINEAR			 	   = 0x5,
				MIN_LINEAR_MAG_MIP_POINT			 	   = 0x10,
				MIN_LINEAR_MAG_POINT_MIP_LINEAR		 	   = 0x11,
				MIN_MAG_LINEAR_MIP_POINT			 	   = 0x14,
				MIN_MAG_MIP_LINEAR					 	   = 0x15,
				ANISOTROPIC								   = 0x55,
				COMPARISON_MIN_MAG_MIP_POINT			   = 0x80,
				COMPARISON_MIN_MAG_POINT_MIP_LINEAR		   = 0x81,
				COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT  = 0x84,
				COMPARISON_MIN_POINT_MAG_MIP_LINEAR		   = 0x85,
				COMPARISON_MIN_LINEAR_MAG_MIP_POINT		   = 0x90,
				COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
				COMPARISON_MIN_MAG_LINEAR_MIP_POINT		   = 0x94,
				COMPARISON_MIN_MAG_MIP_LINEAR			   = 0x95,
				COMPARISON_ANISOTROPIC					   = 0xd5
			};
		};
		struct TextureAddressMode
		{
			enum Enum
			{
				WRAP		= 1,
				MIRROR		= 2,
				CLAMP		= 3,
				BORDER		= 4,
				MIRROR_ONCE	= 5
			};
		};
		struct SamplerState
		{
			FilterMode::Enum		 filter;
			TextureAddressMode::Enum addressU;
			TextureAddressMode::Enum addressV;
			TextureAddressMode::Enum addressW;
			float					 mipLODBias;
			uint32_t				 maxAnisotropy;
			ComparisonFunc::Enum	 comparisonFunc;
			float					 borderColor[4];
			float					 minLOD;
			float					 maxLOD;
		};
	#endif

	// Renderer/RootSignatureTypes.h
	#ifndef __RENDERER_ROOTSIGNATURE_TYPES_H__
	#define __RENDERER_ROOTSIGNATURE_TYPES_H__
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
		struct DescriptorRange
		{
			DescriptorRangeType::Enum rangeType;
			uint32_t				  numberOfDescriptors;
			uint32_t				  baseShaderRegister;
			uint32_t				  registerSpace;
			uint32_t				  offsetInDescriptorsFromTableStart;
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
				uint32_t _registerSpace = 0,
				uint32_t _offsetInDescriptorsFromTableStart = OFFSET_APPEND)
			{
				initialize(_rangeType, _numberOfDescriptors, _baseShaderRegister, _registerSpace, _offsetInDescriptorsFromTableStart);
			}
			inline void initialize(
				DescriptorRangeType::Enum _rangeType,
				uint32_t _numberOfDescriptors,
				uint32_t _baseShaderRegister,
				uint32_t _registerSpace = 0,
				uint32_t _offsetInDescriptorsFromTableStart = OFFSET_APPEND)
			{
				initialize(*this, _rangeType, _numberOfDescriptors, _baseShaderRegister, _registerSpace, _offsetInDescriptorsFromTableStart);
			}
			static inline void initialize(
				DescriptorRange& range,
				DescriptorRangeType::Enum _rangeType,
				uint32_t _numberOfDescriptors,
				uint32_t _baseShaderRegister,
				uint32_t _registerSpace = 0,
				uint32_t _offsetInDescriptorsFromTableStart = OFFSET_APPEND)
			{
				range.rangeType = _rangeType;
				range.numberOfDescriptors = _numberOfDescriptors;
				range.baseShaderRegister = _baseShaderRegister;
				range.registerSpace = _registerSpace;
				range.offsetInDescriptorsFromTableStart = _offsetInDescriptorsFromTableStart;
			}
		};
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
		struct StaticBorderColor
		{
			enum Enum
			{
				TRANSPARENT_BLACK = 0,
				OPAQUE_BLACK      = TRANSPARENT_BLACK + 1,
				OPAQUE_WHITE      = OPAQUE_BLACK + 1
			};
		};
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
	#endif

	// Renderer/BufferTypes.h
	#ifndef __RENDERER_BUFFER_TYPES_H__
	#define __RENDERER_BUFFER_TYPES_H__
		struct BufferUsage
		{
			enum Enum
			{
				STREAM_DRAW  = 0x88E0,
				STREAM_READ  = 0x88E1,
				STREAM_COPY  = 0x88E2,
				STATIC_DRAW  = 0x88E4,
				STATIC_READ  = 0x88E5,
				STATIC_COPY  = 0x88E6,
				DYNAMIC_DRAW = 0x88E8,
				DYNAMIC_READ = 0x88E9,
				DYNAMIC_COPY = 0x88EA
			};
		};
	#endif

	// Renderer/TextureTypes.h
	#ifndef __RENDERER_TEXTURE_TYPES_H__
	#define __RENDERER_TEXTURE_TYPES_H__
		struct TextureFormat
		{
			enum Enum
			{
				A8            = 0,
				R8G8B8        = 1,
				R8G8B8A8      = 2,
				R16G16B16A16F = 3,
				R32G32B32A32F = 4,
				BC1           = 5,
				BC2           = 6,
				BC3           = 7,
				BC4           = 8,
				BC5           = 9,
				ETC1          = 10
			};
			inline static bool isCompressed(Enum textureFormat)
			{
				static bool MAPPING[] =
				{
					false,
					false,
					false,
					false,
					false,
					true,
					true,
					true,
					true,
					true,
					true
				};
				return MAPPING[textureFormat];
			}
			inline static uint32_t getNumberOfBytesPerElement(Enum textureFormat)
			{
				static const uint32_t MAPPING[] =
				{
					sizeof(uint8_t),
					sizeof(uint8_t) * 3,
					sizeof(uint8_t) * 4,
					sizeof(float) * 2,
					sizeof(float) * 4,
					sizeof(uint8_t) * 3,
					sizeof(uint8_t) * 4,
					sizeof(uint8_t) * 4,
					sizeof(uint8_t) * 1,
					sizeof(uint8_t) * 2,
					sizeof(uint8_t) * 3
				};
				return MAPPING[textureFormat];
			}
			inline static uint32_t getNumberOfBytesPerRow(Enum textureFormat, uint32_t width)
			{
				switch (textureFormat)
				{
					case A8:
						return width;
					case R8G8B8:
						return 3 * width;
					case R8G8B8A8:
						return 4 * width;
					case R16G16B16A16F:
						return 8 * width;
					case R32G32B32A32F:
						return 16 * width;
					case BC1:
						return ((width + 3) >> 2) * 8;
					case BC2:
						return ((width + 3) >> 2) * 16;
					case BC3:
						return ((width + 3) >> 2) * 16;
					case BC4:
						return ((width + 3) >> 2) * 8;
					case BC5:
						return ((width + 3) >> 2) * 16;
					case ETC1:
						return (width >> 1);
					default:
						return 0;
				}
			}
			inline static uint32_t getNumberOfBytesPerSlice(Enum textureFormat, uint32_t width, uint32_t height)
			{
				switch (textureFormat)
				{
					case A8:
						return width * height;
					case R8G8B8:
						return 3 * width * height;
					case R8G8B8A8:
						return 4 * width * height;
					case R16G16B16A16F:
						return 8 * width * height;
					case R32G32B32A32F:
						return 16 * width * height;
					case BC1:
						return ((width + 3) >> 2) * ((height + 3) >> 2) * 8;
					case BC2:
						return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;
					case BC3:
						return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;
					case BC4:
						return ((width + 3) >> 2) * ((height + 3) >> 2) * 8;
					case BC5:
						return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;
					case ETC1:
					{
						const uint32_t numberOfBytesPerSlice = (width * height) >> 1;
						return (numberOfBytesPerSlice > 8) ? numberOfBytesPerSlice : 8;
					}
					default:
						return 0;
				}
			}
		};
		struct TextureFlag
		{
			enum Enum
			{
				DATA_CONTAINS_MIPMAPS = 1<<0,
				GENERATE_MIPMAPS      = 1<<1,
				RENDER_TARGET         = 1<<2
			};
		};
		struct TextureUsage
		{
			enum Enum
			{
				DEFAULT   = 0,
				IMMUTABLE = 1,
				DYNAMIC   = 2,
				STAGING   = 3
			};
		};
	#endif

	// Renderer/BlendStateTypes.h
	#ifndef __RENDERER_BLENDSTATE_TYPES_H__
	#define __RENDERER_BLENDSTATE_TYPES_H__
		struct Blend
		{
			enum Enum
			{
				ZERO		     = 1,
				ONE			     = 2,
				SRC_COLOR	     = 3,
				INV_SRC_COLOR    = 4,
				SRC_ALPHA	     = 5,
				INV_SRC_ALPHA    = 6,
				DEST_ALPHA	     = 7,
				INV_DEST_ALPHA   = 8,
				DEST_COLOR	     = 9,
				INV_DEST_COLOR   = 10,
				SRC_ALPHA_SAT    = 11,
				BLEND_FACTOR     = 14,
				INV_BLEND_FACTOR = 15,
				SRC_1_COLOR	     = 16,
				INV_SRC_1_COLOR  = 17,
				SRC_1_ALPHA	     = 18,
				INV_SRC_1_ALPHA  = 19
			};
		};
		struct BlendOp
		{
			enum Enum
			{
				ADD			 = 1,
				SUBTRACT	 = 2,
				REV_SUBTRACT = 3,
				MIN			 = 4,
				MAX			 = 5
			};
		};
		struct RenderTargetBlendDesc
		{
			int			  blendEnable;
			Blend::Enum	  srcBlend;
			Blend::Enum	  destBlend;
			BlendOp::Enum blendOp;
			Blend::Enum	  srcBlendAlpha;
			Blend::Enum	  destBlendAlpha;
			BlendOp::Enum blendOpAlpha;
			uint8_t		  renderTargetWriteMask;
		};
		struct BlendState
		{
			int					  alphaToCoverageEnable;
			int					  independentBlendEnable;
			RenderTargetBlendDesc renderTarget[8];
		};
	#endif

	// Renderer/VertexArrayTypes.h
	#ifndef __RENDERER_VERTEXARRAY_TYPES_H__
	#define __RENDERER_VERTEXARRAY_TYPES_H__
		struct VertexArrayFormat
		{
			enum Enum
			{
				FLOAT_1			= 0,
				FLOAT_2			= 1,
				FLOAT_3			= 2,
				FLOAT_4			= 3,
				UNSIGNED_BYTE_4 = 4,
				SHORT_2			= 5,
				SHORT_4			= 6
			};
		};
		#pragma pack(push)
		#pragma pack(1)
			struct VertexArrayAttribute
			{
				VertexArrayFormat::Enum  vertexArrayFormat;
				char					 name[32];
				char					 semanticName[32];
				uint32_t				 semanticIndex;
				uint32_t				 inputSlot;
				uint32_t				 alignedByteOffset;
				uint32_t				 instancesPerElement;
			};
			struct VertexArrayAttributes
			{
				uint32_t					numberOfAttributes;
				const VertexArrayAttribute*	attributes;
				VertexArrayAttributes()
				{
				}
				VertexArrayAttributes(uint32_t _numberOfAttributes, const VertexArrayAttribute*	_attributes) :
					numberOfAttributes(_numberOfAttributes),
					attributes(_attributes)
				{
				}
			};
		#pragma pack(pop)
		struct VertexArrayVertexBuffer
		{
			IVertexBuffer* vertexBuffer;
			uint32_t	   strideInBytes;
		};
	#endif

	// Renderer/IndexBufferTypes.h
	#ifndef __RENDERER_INDEXBUFFER_TYPES_H__
	#define __RENDERER_INDEXBUFFER_TYPES_H__
		struct IndexBufferFormat
		{
			enum Enum
			{
				UNSIGNED_CHAR  = 0,
				UNSIGNED_SHORT = 1,
				UNSIGNED_INT   = 2
			};
			inline static uint32_t getNumberOfBytesPerElement(Enum indexFormat)
			{
				static uint32_t MAPPING[] =
				{
					1,
					2,
					4
				};
				return MAPPING[indexFormat];
			}
		};
	#endif

	// Renderer/RasterizerStateTypes.h
	#ifndef __RENDERER_RASTERIZERSTATE_TYPES_H__
	#define __RENDERER_RASTERIZERSTATE_TYPES_H__
		struct FillMode
		{
			enum Enum
			{
				WIREFRAME = 2,
				SOLID     = 3
			};
		};
		struct CullMode
		{
			enum Enum
			{
				NONE  = 1,
				FRONT = 2,
				BACK  = 3
			};
		};
		struct RasterizerState
		{
			FillMode::Enum fillMode;
			CullMode::Enum cullMode;
			int			   frontCounterClockwise;
			int			   depthBias;
			float		   depthBiasClamp;
			float		   slopeScaledDepthBias;
			int			   depthClipEnable;
			int			   scissorEnable;
			int			   multisampleEnable;
			int			   antialiasedLineEnable;
		};
	#endif

	// Renderer/PipelineStateTypes.h
	#ifndef __RENDERER_PIPELINESTATE_TYPES_H__
	#define __RENDERER_PIPELINESTATE_TYPES_H__
		struct PipelineState
		{
			IRootSignature*		  rootSignature;
			IProgram*			  program;
			VertexArrayAttributes vertexAttributes;
		};
	#endif

	// Renderer/DepthStencilStateTypes.h
	#ifndef __RENDERER_DEPTHSTENCILSTATE_TYPES_H__
	#define __RENDERER_DEPTHSTENCILSTATE_TYPES_H__
		struct DepthWriteMask
		{
			enum Enum
			{
				ZERO = 0,
				ALL  = 1
			};
		};
		struct StencilOp
		{
			enum Enum
			{
				KEEP	 = 1,
				ZERO	 = 2,
				REPLACE	 = 3,
				INCR_SAT = 4,
				DECR_SAT = 5,
				INVERT	 = 6,
				INCREASE = 7,
				DECREASE = 8
			};
		};
		struct DepthStencilOpDesc
		{
			StencilOp::Enum		 stencilFailOp;
			StencilOp::Enum		 stencilDepthFailOp;
			StencilOp::Enum		 stencilPassOp;
			ComparisonFunc::Enum stencilFunc;
		};
		struct DepthStencilState
		{
			int					 depthEnable;
			DepthWriteMask::Enum depthWriteMask;
			ComparisonFunc::Enum depthFunc;
			int					 stencilEnable;
			uint8_t				 stencilReadMask;
			uint8_t				 stencilWriteMask;
			DepthStencilOpDesc	 frontFace;
			DepthStencilOpDesc	 backFace;
		};
	#endif

	// Renderer/GeometryShaderTypes.h
	#ifndef __RENDERER_GEOMETRYSHADER_TYPES_H__
	#define __RENDERER_GEOMETRYSHADER_TYPES_H__
		struct GsInputPrimitiveTopology
		{
			enum Enum
			{
				POINTS				= 0x0000,
				LINES				= 0x0001,
				LINES_ADJACENCY		= 0x000A,
				TRIANGLES			= 0x0004,
				TRIANGLES_ADJACENCY	= 0x000C
			};
		};
		struct GsOutputPrimitiveTopology
		{
			enum Enum
			{
				POINTS		   = 0x0000,
				LINES		   = 0x0001,
				TRIANGLE_STRIP = 0x0005
			};
		};
	#endif


	//[-------------------------------------------------------]
	//[ Reference counter                                     ]
	//[-------------------------------------------------------]
	// Renderer/RefCount.h
	#ifndef __RENDERER_REFCOUNT_H__
	#define __RENDERER_REFCOUNT_H__
		template <class AType>
		class RefCount
		{
		public:
			RefCount() :
				mRefCount(0)
			{
			}
			virtual ~RefCount()
			{
			}
			virtual const AType *getPointer() const
			{
				return static_cast<const AType*>(this);
			}
			virtual AType *getPointer()
			{
				return static_cast<AType*>(this);
			}
			uint32_t addReference()
			{
				++mRefCount;
				return mRefCount;
			}
			uint32_t release()
			{
				if (mRefCount > 1)
				{
					--mRefCount;
					return mRefCount;
				}
				else
				{
					delete this;
					return 0;
				}
			}
			uint32_t getRefCount() const
			{
				return mRefCount;
			}
		private:
			uint32_t mRefCount;
		};
	#endif

	// Renderer/SmartRefCount.h
	#ifndef __RENDERER_SMARTREFCOUNT_H__
	#define __RENDERER_SMARTREFCOUNT_H__
		template <class AType>
		class SmartRefCount
		{
		public:
			SmartRefCount() :
				mPtr(nullptr)
			{
			}
			explicit SmartRefCount(AType *ptr) :
				mPtr(nullptr)
			{
				setPtr(ptr);
			}
			SmartRefCount(const SmartRefCount<AType> &ptr) :
				mPtr(nullptr)
			{
				setPtr(ptr.getPtr());
			}
			~SmartRefCount()
			{
				setPtr(nullptr);
			}
			SmartRefCount<AType> &operator =(AType *ptr)
			{
				if (getPointer() != ptr)
				{
					setPtr(ptr);
				}
				return *this;
			}
			SmartRefCount<AType> &operator =(const SmartRefCount<AType> &ptr)
			{
				if (getPointer() != ptr.getPointer())
				{
					setPtr(ptr.getPtr());
				}
				return *this;
			}
			AType *getPointer() const
			{
				return mPtr ? static_cast<AType*>(mPtr->getPointer()) : nullptr;
			}
			AType *operator ->() const
			{
				return getPointer();
			}
			operator AType*() const
			{
				return getPointer();
			}
			bool operator !() const
			{
				return (nullptr == getPointer());
			}
			bool operator ==(AType *ptr) const
			{
				return (getPointer() == ptr);
			}
			bool operator ==(const SmartRefCount<AType> &ptr) const
			{
				return (getPointer() == ptr.getPointer());
			}
			bool operator !=(AType *ptr) const
			{
				return (getPointer() != ptr);
			}
			bool operator !=(const SmartRefCount<AType> &ptr) const
			{
				return (getPointer() != ptr.getPointer());
			}
		private:
			void setPtr(AType *ptr)
			{
				if (nullptr != mPtr)
				{
					mPtr->release();
				}
				if (nullptr != ptr)
				{
					ptr->addReference();
				}
				mPtr = ptr;
			}
			AType *getPtr() const
			{
				return mPtr;
			}
		private:
			AType *mPtr;
		};
	#endif


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	// Renderer/Capabilities.h
	#ifndef __RENDERER_CAPABILITIES_H__
	#define __RENDERER_CAPABILITIES_H__
		class Capabilities
		{
		public:
			uint32_t maximumNumberOfViewports;
			uint32_t maximumNumberOfSimultaneousRenderTargets;
			uint32_t maximumTextureDimension;
			uint32_t maximumNumberOf2DTextureArraySlices;
			bool	 uniformBuffer;
			uint32_t maximumTextureBufferSize;
			bool	 individualUniforms;
			bool	 instancedArrays;
			bool	 drawInstanced;
			bool	 baseVertex;
			bool	 vertexShader;
			uint32_t maximumNumberOfPatchVertices;
			uint32_t maximumNumberOfGsOutputVertices;
			bool	 fragmentShader;
		public:
			inline Capabilities() :
				maximumNumberOfViewports(0),
				maximumNumberOfSimultaneousRenderTargets(0),
				maximumTextureDimension(0),
				maximumNumberOf2DTextureArraySlices(0),
				uniformBuffer(false),
				maximumTextureBufferSize(0),
				individualUniforms(false),
				instancedArrays(false),
				drawInstanced(false),
				baseVertex(false),
				vertexShader(false),
				maximumNumberOfPatchVertices(0),
				maximumNumberOfGsOutputVertices(0)
			{
			}
			inline ~Capabilities()
			{
			}
		private:
			inline explicit Capabilities(const Capabilities &) :
				maximumNumberOfViewports(0),
				maximumNumberOfSimultaneousRenderTargets(0),
				maximumTextureDimension(0),
				maximumNumberOf2DTextureArraySlices(0),
				uniformBuffer(false),
				maximumTextureBufferSize(0),
				individualUniforms(false),
				instancedArrays(false),
				drawInstanced(false),
				baseVertex(false),
				vertexShader(false),
				maximumNumberOfPatchVertices(0),
				maximumNumberOfGsOutputVertices(0)
			{
			}
			inline Capabilities &operator =(const Capabilities &)
			{
				return *this;
			}
		};
	#endif

	// Renderer/Statistics.h
	#ifndef __RENDERER_STATISTICS_H__
	#define __RENDERER_STATISTICS_H__
	class Statistics
	{
	public:
		uint32_t currentNumberOfRootSignatures;
		uint32_t numberOfCreatedRootSignatures;
		uint32_t currentNumberOfPrograms;
		uint32_t numberOfCreatedPrograms;
		uint32_t currentNumberOfVertexArrays;
		uint32_t numberOfCreatedVertexArrays;
		uint32_t currentNumberOfSwapChains;
		uint32_t numberOfCreatedSwapChains;
		uint32_t currentNumberOfFramebuffers;
		uint32_t numberOfCreatedFramebuffers;
		uint32_t currentNumberOfIndexBuffers;
		uint32_t numberOfCreatedIndexBuffers;
		uint32_t currentNumberOfVertexBuffers;
		uint32_t numberOfCreatedVertexBuffers;
		uint32_t currentNumberOfUniformBuffers;
		uint32_t numberOfCreatedUniformBuffers;
		uint32_t currentNumberOfTextureBuffers;
		uint32_t numberOfCreatedTextureBuffers;
		uint32_t currentNumberOfTexture2Ds;
		uint32_t numberOfCreatedTexture2Ds;
		uint32_t currentNumberOfTexture2DArrays;
		uint32_t numberOfCreatedTexture2DArrays;
		uint32_t currentNumberOfPipelineStates;
		uint32_t numberOfCreatedPipelineStates;
		uint32_t currentNumberOfRasterizerStates;
		uint32_t numberOfCreatedRasterizerStates;
		uint32_t currentNumberOfDepthStencilStates;
		uint32_t numberOfCreatedDepthStencilStates;
		uint32_t currentNumberOfBlendStates;
		uint32_t numberOfCreatedBlendStates;
		uint32_t currentNumberOfSamplerStates;
		uint32_t numberOfCreatedSamplerStates;
		uint32_t currentNumberOfVertexShaders;
		uint32_t numberOfCreatedVertexShaders;
		uint32_t currentNumberOfTessellationControlShaders;
		uint32_t numberOfCreatedTessellationControlShaders;
		uint32_t currentNumberOfTessellationEvaluationShaders;
		uint32_t numberOfCreatedTessellationEvaluationShaders;
		uint32_t currentNumberOfGeometryShaders;
		uint32_t numberOfCreatedGeometryShaders;
		uint32_t currentNumberOfFragmentShaders;
		uint32_t numberOfCreatedFragmentShaders;
		uint32_t currentNumberOfTextureCollections;
		uint32_t numberOfCreatedTextureCollections;
		uint32_t currentNumberOfSamplerStateCollections;
		uint32_t numberOfCreatedSamplerStateCollections;
	public:
		inline Statistics() :
			currentNumberOfRootSignatures(0),
			numberOfCreatedRootSignatures(0),
			currentNumberOfPrograms(0),
			numberOfCreatedPrograms(0),
			currentNumberOfVertexArrays(0),
			numberOfCreatedVertexArrays(0),
			currentNumberOfSwapChains(0),
			numberOfCreatedSwapChains(0),
			currentNumberOfFramebuffers(0),
			numberOfCreatedFramebuffers(0),
			currentNumberOfIndexBuffers(0),
			numberOfCreatedIndexBuffers(0),
			currentNumberOfVertexBuffers(0),
			numberOfCreatedVertexBuffers(0),
			currentNumberOfUniformBuffers(0),
			numberOfCreatedUniformBuffers(0),
			currentNumberOfTextureBuffers(0),
			numberOfCreatedTextureBuffers(0),
			currentNumberOfTexture2Ds(0),
			numberOfCreatedTexture2Ds(0),
			currentNumberOfTexture2DArrays(0),
			numberOfCreatedTexture2DArrays(0),
			currentNumberOfPipelineStates(0),
			numberOfCreatedPipelineStates(0),
			currentNumberOfRasterizerStates(0),
			numberOfCreatedRasterizerStates(0),
			currentNumberOfDepthStencilStates(0),
			numberOfCreatedDepthStencilStates(0),
			currentNumberOfBlendStates(0),
			numberOfCreatedBlendStates(0),
			currentNumberOfSamplerStates(0),
			numberOfCreatedSamplerStates(0),
			currentNumberOfVertexShaders(0),
			numberOfCreatedVertexShaders(0),
			currentNumberOfTessellationControlShaders(0),
			numberOfCreatedTessellationControlShaders(0),
			currentNumberOfTessellationEvaluationShaders(0),
			numberOfCreatedTessellationEvaluationShaders(0),
			currentNumberOfGeometryShaders(0),
			numberOfCreatedGeometryShaders(0),
			currentNumberOfFragmentShaders(0),
			numberOfCreatedFragmentShaders(0),
			currentNumberOfTextureCollections(0),
			numberOfCreatedTextureCollections(0),
			currentNumberOfSamplerStateCollections(0),
			numberOfCreatedSamplerStateCollections(0)
		{
		}
		inline ~Statistics()
		{
		}
	private:
		inline explicit Statistics(const Statistics &) :
			currentNumberOfRootSignatures(0),
			numberOfCreatedRootSignatures(0),
			currentNumberOfPrograms(0),
			numberOfCreatedPrograms(0),
			currentNumberOfVertexArrays(0),
			numberOfCreatedVertexArrays(0),
			currentNumberOfSwapChains(0),
			numberOfCreatedSwapChains(0),
			currentNumberOfFramebuffers(0),
			numberOfCreatedFramebuffers(0),
			currentNumberOfIndexBuffers(0),
			numberOfCreatedIndexBuffers(0),
			currentNumberOfVertexBuffers(0),
			numberOfCreatedVertexBuffers(0),
			currentNumberOfUniformBuffers(0),
			numberOfCreatedUniformBuffers(0),
			currentNumberOfTextureBuffers(0),
			numberOfCreatedTextureBuffers(0),
			currentNumberOfTexture2Ds(0),
			numberOfCreatedTexture2Ds(0),
			currentNumberOfTexture2DArrays(0),
			numberOfCreatedTexture2DArrays(0),
			currentNumberOfPipelineStates(0),
			numberOfCreatedPipelineStates(0),
			currentNumberOfRasterizerStates(0),
			numberOfCreatedRasterizerStates(0),
			currentNumberOfDepthStencilStates(0),
			numberOfCreatedDepthStencilStates(0),
			currentNumberOfBlendStates(0),
			numberOfCreatedBlendStates(0),
			currentNumberOfSamplerStates(0),
			numberOfCreatedSamplerStates(0),
			currentNumberOfVertexShaders(0),
			numberOfCreatedVertexShaders(0),
			currentNumberOfTessellationControlShaders(0),
			numberOfCreatedTessellationControlShaders(0),
			currentNumberOfTessellationEvaluationShaders(0),
			numberOfCreatedTessellationEvaluationShaders(0),
			currentNumberOfGeometryShaders(0),
			numberOfCreatedGeometryShaders(0),
			currentNumberOfFragmentShaders(0),
			numberOfCreatedFragmentShaders(0),
			currentNumberOfTextureCollections(0),
			numberOfCreatedTextureCollections(0),
			currentNumberOfSamplerStateCollections(0),
			numberOfCreatedSamplerStateCollections(0)
		{
		}
		inline Statistics &operator =(const Statistics &)
		{
			return *this;
		}
	};
	#endif


	//[-------------------------------------------------------]
	//[ Interfaces                                            ]
	//[-------------------------------------------------------]
	// Renderer/IRenderer.h
	#ifndef __RENDERER_IRENDERER_H__
	#define __RENDERER_IRENDERER_H__
		class IRenderer : public RefCount<IRenderer>
		{
		public:
			virtual ~IRenderer();
			inline const Capabilities &getCapabilities() const
			{
				return mCapabilities;
			}
			inline const Statistics &getStatistics() const
			{
				return mStatistics;
			}
		public:
			virtual const char *getName() const = 0;
			virtual bool isInitialized() const = 0;
			virtual ISwapChain *getMainSwapChain() const = 0;
			virtual uint32_t getNumberOfShaderLanguages() const = 0;
			virtual const char *getShaderLanguageName(uint32_t index) const = 0;
			virtual IShaderLanguage *getShaderLanguage(const char *shaderLanguageName = nullptr) = 0;
			virtual ISwapChain *createSwapChain(handle nativeWindowHandle) = 0;
			virtual IFramebuffer *createFramebuffer(uint32_t numberOfColorTextures, ITexture **colorTextures, ITexture *depthStencilTexture = nullptr) = 0;
			virtual IVertexBuffer *createVertexBuffer(uint32_t numberOfBytes, const void *data = nullptr, BufferUsage::Enum bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;
			virtual IIndexBuffer *createIndexBuffer(uint32_t numberOfBytes, IndexBufferFormat::Enum indexBufferFormat, const void *data = nullptr, BufferUsage::Enum bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;
			virtual IVertexArray *createVertexArray(const VertexArrayAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const VertexArrayVertexBuffer *vertexBuffers, IIndexBuffer *indexBuffer = nullptr) = 0;
			virtual ITextureBuffer *createTextureBuffer(uint32_t numberOfBytes, TextureFormat::Enum textureFormat, const void *data = nullptr, BufferUsage::Enum bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;
			virtual ITexture2D *createTexture2D(uint32_t width, uint32_t height, TextureFormat::Enum textureFormat, void *data = nullptr, uint32_t flags = 0, TextureUsage::Enum textureUsage = TextureUsage::DEFAULT) = 0;
			virtual ITexture2DArray *createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, TextureFormat::Enum textureFormat, void *data = nullptr, uint32_t flags = 0, TextureUsage::Enum textureUsage = TextureUsage::DEFAULT) = 0;
			virtual IRootSignature *createRootSignature(const RootSignature &rootSignature) = 0;
			virtual IPipelineState *createPipelineState(const PipelineState &pipelineState) = 0;
			virtual IRasterizerState *createRasterizerState(const RasterizerState &rasterizerState) = 0;
			virtual IDepthStencilState *createDepthStencilState(const DepthStencilState &depthStencilState) = 0;
			virtual IBlendState *createBlendState(const BlendState &blendState) = 0;
			virtual ISamplerState *createSamplerState(const SamplerState &samplerState) = 0;
			virtual ITextureCollection *createTextureCollection(uint32_t numberOfTextures, ITexture **textures) = 0;
			virtual ISamplerStateCollection *createSamplerStateCollection(uint32_t numberOfSamplerStates, ISamplerState **samplerStates) = 0;
			virtual bool map(IResource &resource, uint32_t subresource, MapType::Enum mapType, uint32_t mapFlags, MappedSubresource &mappedSubresource) = 0;
			virtual void unmap(IResource &resource, uint32_t subresource) = 0;
			virtual void setGraphicsRootSignature(IRootSignature *rootSignature) = 0;
			virtual void setPipelineState(IPipelineState *pipelineState) = 0;
			virtual void setProgram(IProgram *program) = 0;
			virtual void iaSetVertexArray(IVertexArray *vertexArray) = 0;
			virtual void iaSetPrimitiveTopology(PrimitiveTopology::Enum primitiveTopology) = 0;
			virtual void vsSetTexture(uint32_t unit, ITexture *texture) = 0;
			virtual void vsSetTextureCollection(uint32_t startUnit, ITextureCollection *textureCollection) = 0;
			virtual void vsSetSamplerState(uint32_t unit, ISamplerState *samplerState) = 0;
			virtual void vsSetSamplerStateCollection(uint32_t startUnit, ISamplerStateCollection *samplerStateCollection) = 0;
			virtual void vsSetUniformBuffer(uint32_t slot, IUniformBuffer *uniformBuffer) = 0;
			virtual void tcsSetTexture(uint32_t unit, ITexture *texture) = 0;
			virtual void tcsSetTextureCollection(uint32_t startUnit, ITextureCollection *textureCollection) = 0;
			virtual void tcsSetSamplerState(uint32_t unit, ISamplerState *samplerState) = 0;
			virtual void tcsSetSamplerStateCollection(uint32_t startUnit, ISamplerStateCollection *samplerStateCollection) = 0;
			virtual void tcsSetUniformBuffer(uint32_t slot, IUniformBuffer *uniformBuffer) = 0;
			virtual void tesSetTexture(uint32_t unit, ITexture *texture) = 0;
			virtual void tesSetTextureCollection(uint32_t startUnit, ITextureCollection *textureCollection) = 0;
			virtual void tesSetSamplerState(uint32_t unit, ISamplerState *samplerState) = 0;
			virtual void tesSetSamplerStateCollection(uint32_t startUnit, ISamplerStateCollection *samplerStateCollection) = 0;
			virtual void tesSetUniformBuffer(uint32_t slot, IUniformBuffer *uniformBuffer) = 0;
			virtual void gsSetTexture(uint32_t unit, ITexture *texture) = 0;
			virtual void gsSetTextureCollection(uint32_t startUnit, ITextureCollection *textureCollection) = 0;
			virtual void gsSetSamplerState(uint32_t unit, ISamplerState *samplerState) = 0;
			virtual void gsSetSamplerStateCollection(uint32_t startUnit, ISamplerStateCollection *samplerStateCollection) = 0;
			virtual void gsSetUniformBuffer(uint32_t slot, IUniformBuffer *uniformBuffer) = 0;
			virtual void rsSetViewports(uint32_t numberOfViewports, const Viewport *viewports) = 0;
			virtual void rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles) = 0;
			virtual void rsSetState(IRasterizerState *rasterizerState) = 0;
			virtual void fsSetTexture(uint32_t unit, ITexture *texture) = 0;
			virtual void fsSetTextureCollection(uint32_t startUnit, ITextureCollection *textureCollection) = 0;
			virtual void fsSetSamplerState(uint32_t unit, ISamplerState *samplerState) = 0;
			virtual void fsSetSamplerStateCollection(uint32_t startUnit, ISamplerStateCollection *samplerStateCollection) = 0;
			virtual void fsSetUniformBuffer(uint32_t slot, IUniformBuffer *uniformBuffer) = 0;
			virtual IRenderTarget *omGetRenderTarget() = 0;
			virtual void omSetRenderTarget(IRenderTarget *renderTarget) = 0;
			virtual void omSetDepthStencilState(IDepthStencilState *depthStencilState) = 0;
			virtual void omSetBlendState(IBlendState *blendState) = 0;
			virtual void clear(uint32_t flags, const float color[4], float z, uint32_t stencil) = 0;
			virtual bool beginScene() = 0;
			virtual void endScene() = 0;
			virtual void draw(uint32_t startVertexLocation, uint32_t numberOfVertices) = 0;
			virtual void drawInstanced(uint32_t startVertexLocation, uint32_t numberOfVertices, uint32_t numberOfInstances) = 0;
			virtual void drawIndexed(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t minimumIndex, uint32_t numberOfVertices) = 0;
			virtual void drawIndexedInstanced(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t minimumIndex, uint32_t numberOfVertices, uint32_t numberOfInstances) = 0;
			virtual void flush() = 0;
			virtual void finish() = 0;
			virtual bool isDebugEnabled() = 0;
			virtual void setDebugMarker(const wchar_t *name) = 0;
			virtual void beginDebugEvent(const wchar_t *name) = 0;
			virtual void endDebugEvent() = 0;
		protected:
			IRenderer();
			explicit IRenderer(const IRenderer &source);
			IRenderer &operator =(const IRenderer &source);
		protected:
			Capabilities mCapabilities;
		private:
			Statistics mStatistics;
		};
		typedef SmartRefCount<IRenderer> IRendererPtr;
	#endif

	// Renderer/IShaderLanguage.h
	#ifndef __RENDERER_ISHADERLANGUAGE_H__
	#define __RENDERER_ISHADERLANGUAGE_H__
		class IShaderLanguage : public RefCount<IShaderLanguage>
		{
		public:
			virtual ~IShaderLanguage();
			inline IRenderer &getRenderer() const
			{
				return *mRenderer;
			}
			inline IProgram *createProgram(const VertexArrayAttributes& vertexAttributes, IVertexShader *vertexShader, IFragmentShader *fragmentShader)
			{
				return createProgram(vertexAttributes, vertexShader, nullptr, nullptr, nullptr, fragmentShader);
			}
			inline IProgram *createProgram(const VertexArrayAttributes& vertexAttributes, IVertexShader *vertexShader, IGeometryShader *geometryShader, IFragmentShader *fragmentShader)
			{
				return createProgram(vertexAttributes, vertexShader, nullptr, nullptr, geometryShader, fragmentShader);
			}
			inline IProgram *createProgram(const VertexArrayAttributes& vertexAttributes, IVertexShader *vertexShader, ITessellationControlShader *tessellationControlShader, ITessellationEvaluationShader *tessellationEvaluationShader, IFragmentShader *fragmentShader)
			{
				return createProgram(vertexAttributes, vertexShader, tessellationControlShader, tessellationEvaluationShader, nullptr, fragmentShader);
			}
		public:
			virtual const char *getShaderLanguageName() const = 0;
			virtual IVertexShader *createVertexShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) = 0;
			virtual IVertexShader *createVertexShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) = 0;
			virtual ITessellationControlShader *createTessellationControlShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) = 0;
			virtual ITessellationControlShader *createTessellationControlShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) = 0;
			virtual ITessellationEvaluationShader *createTessellationEvaluationShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) = 0;
			virtual ITessellationEvaluationShader *createTessellationEvaluationShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) = 0;
			virtual IGeometryShader *createGeometryShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes, GsInputPrimitiveTopology::Enum gsInputPrimitiveTopology, GsOutputPrimitiveTopology::Enum gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices) = 0;
			virtual IGeometryShader *createGeometryShaderFromSourceCode(const char *sourceCode, GsInputPrimitiveTopology::Enum gsInputPrimitiveTopology, GsOutputPrimitiveTopology::Enum gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) = 0;
			virtual IFragmentShader *createFragmentShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) = 0;
			virtual IFragmentShader *createFragmentShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) = 0;
			virtual IProgram *createProgram(const VertexArrayAttributes& vertexAttributes, IVertexShader *vertexShader, ITessellationControlShader *tessellationControlShader, ITessellationEvaluationShader *tessellationEvaluationShader, IGeometryShader *geometryShader, IFragmentShader *fragmentShader) = 0;
			virtual IUniformBuffer *createUniformBuffer(uint32_t numberOfBytes, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) = 0;
		protected:
			explicit IShaderLanguage(IRenderer &renderer);
			explicit IShaderLanguage(const IShaderLanguage &source);
			IShaderLanguage &operator =(const IShaderLanguage &source);
		private:
			IRenderer *mRenderer;
		};
		typedef SmartRefCount<IShaderLanguage> IShaderLanguagePtr;
	#endif

	// Renderer/IResource.h
	#ifndef __RENDERER_IRESOURCE_H__
	#define __RENDERER_IRESOURCE_H__
		class IResource : public RefCount<IResource>
		{
		public:
			virtual ~IResource();
			inline ResourceType::Enum getResourceType() const
			{
				return mResourceType;
			}
			inline IRenderer &getRenderer() const
			{
				return *mRenderer;
			}
		public:
			virtual void setDebugName(const char *name) = 0;
		protected:
			IResource(ResourceType::Enum resourceType, IRenderer &renderer);
			explicit IResource(const IResource &source);
			IResource &operator =(const IResource &source);
		private:
			ResourceType::Enum  mResourceType;
			IRenderer		   *mRenderer;
		};
		typedef SmartRefCount<IResource> IResourcePtr;
	#endif

	// Renderer/RootSignature.h
	#ifndef __RENDERER_IROOTSIGNATURE_H__
	#define __RENDERER_IROOTSIGNATURE_H__
		class IRootSignature : public IResource
		{
		public:
			virtual ~IRootSignature();
		protected:
			explicit IRootSignature(IRenderer &renderer);
			explicit IRootSignature(const IRootSignature &source);
			IRootSignature &operator =(const IRootSignature &source);
		};
		typedef SmartRefCount<IRootSignature> IRootSignaturePtr;
	#endif

	// Renderer/IProgram.h
	#ifndef __RENDERER_IPROGRAM_H__
	#define __RENDERER_IPROGRAM_H__
		class IProgram : public IResource
		{
		public:
			virtual ~IProgram();
		public:
			virtual uint32_t getUniformBlockIndex(const char *uniformBlockName, uint32_t defaultIndex) = 0;
			virtual handle getUniformHandle(const char *uniformName) = 0;
			virtual uint32_t setTextureUnit(handle uniformHandle, uint32_t unit) = 0;
			virtual void setUniform1i(handle uniformHandle, int value) = 0;
			virtual void setUniform1f(handle uniformHandle, float value) = 0;
			virtual void setUniform2fv(handle uniformHandle, const float *value) = 0;
			virtual void setUniform3fv(handle uUniformHandle, const float *value) = 0;
			virtual void setUniform4fv(handle uniformHandle, const float *value) = 0;
			virtual void setUniformMatrix3fv(handle uniformHandle, const float *value) = 0;
			virtual void setUniformMatrix4fv(handle uniformHandle, const float *value) = 0;
		protected:
			explicit IProgram(IRenderer &renderer);
			explicit IProgram(const IProgram &source);
			IProgram &operator =(const IProgram &source);
		};
		typedef SmartRefCount<IProgram> IProgramPtr;
	#endif

	// Renderer/IVertexArray.h
	#ifndef __RENDERER_IVERTEXARRAY_H__
	#define __RENDERER_IVERTEXARRAY_H__
		class IVertexArray : public IResource
		{
		public:
			virtual ~IVertexArray();
		protected:
			explicit IVertexArray(IRenderer &renderer);
			explicit IVertexArray(const IVertexArray &source);
			IVertexArray &operator =(const IVertexArray &source);
		};
		typedef SmartRefCount<IVertexArray> IVertexArrayPtr;
	#endif

	// Renderer/IRenderTarget.h
	#ifndef __RENDERER_IRENDERTARGET_H__
	#define __RENDERER_IRENDERTARGET_H__
		class IRenderTarget : public IResource
		{
		public:
			virtual ~IRenderTarget();
		public:
			virtual void getWidthAndHeight(uint32_t &width, uint32_t &height) const = 0;
		protected:
			IRenderTarget(ResourceType::Enum resourceType, IRenderer &renderer);
			explicit IRenderTarget(const IRenderTarget &source);
			IRenderTarget &operator =(const IRenderTarget &source);
		};
		typedef SmartRefCount<IRenderTarget> IRenderTargetPtr;
	#endif

	// Renderer/ISwapChain.h
	#ifndef __RENDERER_ISWAPCHAIN_H__
	#define __RENDERER_ISWAPCHAIN_H__
		class ISwapChain : public IRenderTarget
		{
		public:
			virtual ~ISwapChain();
		public:
			virtual handle getNativeWindowHandle() const = 0;
			virtual void present() = 0;
			virtual void resizeBuffers() = 0;
			virtual bool getFullscreenState() const = 0;
			virtual void setFullscreenState(bool fullscreen) = 0;
		protected:
			explicit ISwapChain(IRenderer &renderer);
			explicit ISwapChain(const ISwapChain &source);
			ISwapChain &operator =(const ISwapChain &source);
		};
		typedef SmartRefCount<ISwapChain> ISwapChainPtr;
	#endif

	// Renderer/IFramebuffer.h
	#ifndef __RENDERER_IFRAMEBUFFER_H__
	#define __RENDERER_IFRAMEBUFFER_H__
		class IFramebuffer : public IRenderTarget
		{
		public:
			virtual ~IFramebuffer();
		protected:
			explicit IFramebuffer(IRenderer &renderer);
			explicit IFramebuffer(const IFramebuffer &source);
			IFramebuffer &operator =(const IFramebuffer &source);
		};
		typedef SmartRefCount<IFramebuffer> IFramebufferPtr;
	#endif

	// Renderer/IBuffer.h
	#ifndef __RENDERER_IBUFFER_H__
	#define __RENDERER_IBUFFER_H__
		class IBuffer : public IResource
		{
		public:
			virtual ~IBuffer();
		protected:
			IBuffer(ResourceType::Enum resourceType, IRenderer &renderer);
			explicit IBuffer(const IBuffer &source);
			IBuffer &operator =(const IBuffer &source);
		};
		typedef SmartRefCount<IBuffer> IBufferPtr;
	#endif

	// Renderer/IIndexBuffer.h
	#ifndef __RENDERER_IINDEXBUFFER_H__
	#define __RENDERER_IINDEXBUFFER_H__
		class IIndexBuffer : public IBuffer
		{
		public:
			virtual ~IIndexBuffer();
		protected:
			explicit IIndexBuffer(IRenderer &renderer);
			explicit IIndexBuffer(const IIndexBuffer &source);
			IIndexBuffer &operator =(const IIndexBuffer &source);
		};
		typedef SmartRefCount<IIndexBuffer> IIndexBufferPtr;
	#endif

	// Renderer/IVertexBuffer.h
	#ifndef __RENDERER_IVERTEXBUFFER_H__
	#define __RENDERER_IVERTEXBUFFER_H__
		class IVertexBuffer : public IBuffer
		{
		public:
			virtual ~IVertexBuffer();
		protected:
			explicit IVertexBuffer(IRenderer &renderer);
			explicit IVertexBuffer(const IVertexBuffer &source);
			IVertexBuffer &operator =(const IVertexBuffer &source);
		};
		typedef SmartRefCount<IVertexBuffer> IVertexBufferPtr;
	#endif

	// Renderer/IUniformBuffer.h
	#ifndef __RENDERER_IUNIFORMBUFFER_H__
	#define __RENDERER_IUNIFORMBUFFER_H__
		class IUniformBuffer : public IBuffer
		{
		public:
			virtual ~IUniformBuffer();
		public:
			virtual void copyDataFrom(uint32_t numberOfBytes, const void *data) = 0;
		protected:
			explicit IUniformBuffer(IRenderer &renderer);
			explicit IUniformBuffer(const IUniformBuffer &source);
			IUniformBuffer &operator =(const IUniformBuffer &source);
		};
		typedef SmartRefCount<IUniformBuffer> IUniformBufferPtr;
	#endif

	// Renderer/ITexture.h
	#ifndef __RENDERER_ITEXTURE_H__
	#define __RENDERER_ITEXTURE_H__
		class ITexture : public IResource
		{
		public:
			virtual ~ITexture();
		protected:
			ITexture(ResourceType::Enum resourceType, IRenderer &renderer);
			explicit ITexture(const ITexture &source);
			ITexture &operator =(const ITexture &source);
		};
		typedef SmartRefCount<ITexture> ITexturePtr;
	#endif

	// Renderer/ITextureBuffer.h
	#ifndef __RENDERER_ITEXTUREBUFFER_H__
	#define __RENDERER_ITEXTUREBUFFER_H__
		class ITextureBuffer : public ITexture
		{
		public:
			virtual ~ITextureBuffer();
		public:
			virtual void copyDataFrom(uint32_t numberOfBytes, const void *data) = 0;
		protected:
			explicit ITextureBuffer(IRenderer &renderer);
			explicit ITextureBuffer(const ITextureBuffer &source);
			ITextureBuffer &operator =(const ITextureBuffer &source);
		};
		typedef SmartRefCount<ITextureBuffer> ITextureBufferPtr;
	#endif

	// Renderer/ITexture2D.h
	#ifndef __RENDERER_ITEXTURE2D_H__
	#define __RENDERER_ITEXTURE2D_H__
		class ITexture2D : public ITexture
		{
		public:
			virtual ~ITexture2D();
			inline uint32_t getWidth() const
			{
				return mWidth;
			}
			inline uint32_t getHeight() const
			{
				return mHeight;
			}
		protected:
			ITexture2D(IRenderer &renderer, uint32_t width, uint32_t height);
			explicit ITexture2D(const ITexture2D &source);
			ITexture2D &operator =(const ITexture2D &source);
		private:
			uint32_t mWidth;
			uint32_t mHeight;
		};
		typedef SmartRefCount<ITexture2D> ITexture2DPtr;
	#endif

	// Renderer/ITexture2DArray.h
	#ifndef __RENDERER_ITEXTURE2DARRAY_H__
	#define __RENDERER_ITEXTURE2DARRAY_H__
		class ITexture2DArray : public ITexture
		{
		public:
			virtual ~ITexture2DArray();
			inline uint32_t getWidth() const
			{
				return mWidth;
			}
			inline uint32_t getHeight() const
			{
				return mHeight;
			}
			inline uint32_t getNumberOfSlices() const
			{
				return mNumberOfSlices;
			}
		protected:
			ITexture2DArray(IRenderer &renderer, uint32_t width, uint32_t height, uint32_t numberOfSlices);
			explicit ITexture2DArray(const ITexture2DArray &source);
			ITexture2DArray &operator =(const ITexture2DArray &source);
		private:
			uint32_t mWidth;
			uint32_t mHeight;
			uint32_t mNumberOfSlices;
		};
		typedef SmartRefCount<ITexture2DArray> ITexture2DArrayPtr;
	#endif

	// Renderer/IState.h
	#ifndef __RENDERER_ISTATE_H__
	#define __RENDERER_ISTATE_H__
		class IState : public IResource
		{
		public:
			virtual ~IState();
		protected:
			IState(ResourceType::Enum resourceType, IRenderer &renderer);
			explicit IState(const IState &source);
			IState &operator =(const IState &source);
		};
		typedef SmartRefCount<IState> IStatePtr;
	#endif

	// Renderer/IPipelineState.h
	#ifndef __RENDERER_IPIPELINESTATE_H__
	#define __RENDERER_IPIPELINESTATE_H__
		class IPipelineState : public IState
		{
		public:
			virtual ~IPipelineState();
		protected:
			explicit IPipelineState(IRenderer &renderer);
			explicit IPipelineState(const IPipelineState &source);
			IPipelineState &operator =(const IPipelineState &source);
		};
		typedef SmartRefCount<IPipelineState> IPipelineStatePtr;
	#endif

	// Renderer/IRasterizerState.h
	#ifndef __RENDERER_IRASTERIZERSTATE_H__
	#define __RENDERER_IRASTERIZERSTATE_H__
		class IRasterizerState : public IState
		{
		public:
			static inline const RasterizerState &getDefaultRasterizerState()
			{
				static const Renderer::RasterizerState RASTERIZER_STATE =
				{
					Renderer::FillMode::SOLID,
					Renderer::CullMode::BACK,
					false,
					0,
					0.0f,
					0.0f,
					true,
					false,
					false,
					false
				};
				return RASTERIZER_STATE;
			}
		public:
			virtual ~IRasterizerState();
		protected:
			explicit IRasterizerState(IRenderer &renderer);
			explicit IRasterizerState(const IRasterizerState &source);
			IRasterizerState &operator =(const IRasterizerState &source);
		};
		typedef SmartRefCount<IRasterizerState> IRasterizerStatePtr;
	#endif

	// Renderer/IDepthStencilState.h
	#ifndef __RENDERER_IDEPTHSTENCILSTATE_H__
	#define __RENDERER_IDEPTHSTENCILSTATE_H__
		class IDepthStencilState : public IState
		{
		public:
			static inline const DepthStencilState &getDefaultDepthStencilState()
			{
				static const Renderer::DepthStencilState DEPTH_STENCIL_STATE =
				{
					true,
					Renderer::DepthWriteMask::ALL,
					Renderer::ComparisonFunc::LESS,
					false,
					0xff,
					0xff,
					{
						Renderer::StencilOp::KEEP,
						Renderer::StencilOp::KEEP,
						Renderer::StencilOp::KEEP,
						Renderer::ComparisonFunc::ALWAYS
					},
					{
						Renderer::StencilOp::KEEP,
						Renderer::StencilOp::KEEP,
						Renderer::StencilOp::KEEP,
						Renderer::ComparisonFunc::ALWAYS
					}
				};
				return DEPTH_STENCIL_STATE;
			}
		public:
			virtual ~IDepthStencilState();
		protected:
			explicit IDepthStencilState(IRenderer &renderer);
			explicit IDepthStencilState(const IDepthStencilState &source);
			IDepthStencilState &operator =(const IDepthStencilState &source);
		};
		typedef SmartRefCount<IDepthStencilState> IDepthStencilStatePtr;
	#endif

	// Renderer/IBlendState.h
	#ifndef __RENDERER_IBLENDSTATE_H__
	#define __RENDERER_IBLENDSTATE_H__
		class IBlendState : public IState
		{
		public:
			static inline const BlendState &getDefaultBlendState()
			{
				static const Renderer::BlendState BLEND_STATE =
				{
					false,
					false,
					{
						{
							false,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::ColorWriteEnable::ALL,
						},
						{
							false,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::ColorWriteEnable::ALL,
						},
						{
							false,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::ColorWriteEnable::ALL,
						},
						{
							false,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::ColorWriteEnable::ALL,
						},
						{
							false,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::ColorWriteEnable::ALL,
						},
						{
							false,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::ColorWriteEnable::ALL,
						},
						{
							false,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::ColorWriteEnable::ALL,
						},
						{
							false,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::Blend::ONE,
							Renderer::Blend::ZERO,
							Renderer::BlendOp::ADD,
							Renderer::ColorWriteEnable::ALL,
						},
					}
				};
				return BLEND_STATE;
			}
		public:
			virtual ~IBlendState();
		protected:
			explicit IBlendState(IRenderer &renderer);
			explicit IBlendState(const IBlendState &source);
			IBlendState &operator =(const IBlendState &source);
		};
		typedef SmartRefCount<IBlendState> IBlendStatePtr;
	#endif

	// Renderer/ISamplerState.h
	#ifndef __RENDERER_ISAMPLERSTATE_H__
	#define __RENDERER_ISAMPLERSTATE_H__
		class ISamplerState : public IState
		{
		public:
			static inline const SamplerState &getDefaultSamplerState()
			{
				static const Renderer::SamplerState SAMPLER_STATE =
				{
					Renderer::FilterMode::MIN_MAG_MIP_LINEAR,
					Renderer::TextureAddressMode::CLAMP,
					Renderer::TextureAddressMode::CLAMP,
					Renderer::TextureAddressMode::CLAMP,
					0.0f,
					16,
					Renderer::ComparisonFunc::NEVER,
					{
						0.0f,
						0.0f,
						0.0f,
						0.0f
					},
					-3.402823466e+38f,
					3.402823466e+38f
				};
				return SAMPLER_STATE;
			}
		public:
			virtual ~ISamplerState();
		protected:
			explicit ISamplerState(IRenderer &renderer);
			explicit ISamplerState(const ISamplerState &source);
			ISamplerState &operator =(const ISamplerState &source);
		};
		typedef SmartRefCount<ISamplerState> ISamplerStatePtr;
	#endif

	// Renderer/IShader.h
	#ifndef __RENDERER_ISHADER_H__
	#define __RENDERER_ISHADER_H__
		class IShader : public IResource
		{
		public:
			virtual ~IShader();
		public:
			virtual const char *getShaderLanguageName() const = 0;
		protected:
			IShader(ResourceType::Enum resourceType, IRenderer &renderer);
			explicit IShader(const IShader &source);
			IShader &operator =(const IShader &source);
		};
		typedef SmartRefCount<IShader> IShaderPtr;
	#endif

	// Renderer/IVertexShader.h
	#ifndef __RENDERER_IVERTEXSHADER_H__
	#define __RENDERER_IVERTEXSHADER_H__
		class IVertexShader : public IShader
		{
		public:
			virtual ~IVertexShader();
		protected:
			explicit IVertexShader(IRenderer &renderer);
			explicit IVertexShader(const IVertexShader &source);
			IVertexShader &operator =(const IVertexShader &source);
		};
		typedef SmartRefCount<IVertexShader> IVertexShaderPtr;
	#endif

	// Renderer/ITessellationControlShader.h
	#ifndef __RENDERER_ITESSELLATIONCONTROLSHADER_H__
	#define __RENDERER_ITESSELLATIONCONTROLSHADER_H__
		class ITessellationControlShader : public IShader
		{
		public:
			virtual ~ITessellationControlShader();
		protected:
			explicit ITessellationControlShader(IRenderer &renderer);
			explicit ITessellationControlShader(const ITessellationControlShader &source);
			ITessellationControlShader &operator =(const ITessellationControlShader &source);
		};
		typedef SmartRefCount<ITessellationControlShader> ITessellationControlShaderPtr;
	#endif

	// Renderer/ITessellationEvaluationShader.h
	#ifndef __RENDERER_ITESSELATIONEVALUATIONSHADER_H__
	#define __RENDERER_ITESSELATIONEVALUATIONSHADER_H__
		class ITessellationEvaluationShader : public IShader
		{
		public:
			virtual ~ITessellationEvaluationShader();
		protected:
			explicit ITessellationEvaluationShader(IRenderer &renderer);
			explicit ITessellationEvaluationShader(const ITessellationEvaluationShader &source);
			ITessellationEvaluationShader &operator =(const ITessellationEvaluationShader &source);
		};
		typedef SmartRefCount<ITessellationEvaluationShader> ITessellationEvaluationShaderPtr;
	#endif

	// Renderer/IGeometryShader.h
	#ifndef __RENDERER_IGEOMETRYSHADER_H__
	#define __RENDERER_IGEOMETRYSHADER_H__
		class IGeometryShader : public IShader
		{
		public:
			virtual ~IGeometryShader();
		protected:
			explicit IGeometryShader(IRenderer &renderer);
			explicit IGeometryShader(const IGeometryShader &source);
			IGeometryShader &operator =(const IGeometryShader &source);
		};
		typedef SmartRefCount<IGeometryShader> IGeometryShaderPtr;
	#endif

	// Renderer/IFragmentShader.h
	#ifndef __RENDERER_IFRAGMENTSHADER_H__
	#define __RENDERER_IFRAGMENTSHADER_H__
		class IFragmentShader : public IShader
		{
		public:
			virtual ~IFragmentShader();
		protected:
			explicit IFragmentShader(IRenderer &renderer);
			explicit IFragmentShader(const IFragmentShader &source);
			IFragmentShader &operator =(const IFragmentShader &source);
		};
		typedef SmartRefCount<IFragmentShader> IFragmentShaderPtr;
	#endif

	// Renderer/ICollection.h
	#ifndef __RENDERER_ICOLLECTION_H__
	#define __RENDERER_ICOLLECTION_H__
		class ICollection : public IResource
		{
		public:
			virtual ~ICollection();
		protected:
			ICollection(ResourceType::Enum resourceType, IRenderer &renderer);
			explicit ICollection(const ICollection &source);
			ICollection &operator =(const ICollection &source);
		};
		typedef SmartRefCount<ICollection> ICollectionPtr;
	#endif

	// Renderer/ITextureCollection.h
	#ifndef __RENDERER_ITEXTURECOLLECTION_H__
	#define __RENDERER_ITEXTURECOLLECTION_H__
		class ITextureCollection : public ICollection
		{
		public:
			virtual ~ITextureCollection();
		protected:
			ITextureCollection(ResourceType::Enum resourceType, IRenderer &renderer);
			explicit ITextureCollection(const ITextureCollection &source);
			ITextureCollection &operator =(const ITextureCollection &source);
		};
		typedef SmartRefCount<ITextureCollection> ITextureCollectionPtr;
	#endif

	// Renderer/ISamplerStateCollection.h
	#ifndef __RENDERER_ISAMPLERSTATECOLLECTION_H__
	#define __RENDERER_ISAMPLERSTATECOLLECTION_H__
		class ISamplerStateCollection : public ICollection
		{
		public:
			virtual ~ISamplerStateCollection();
		protected:
			ISamplerStateCollection(ResourceType::Enum resourceType, IRenderer &renderer);
			explicit ISamplerStateCollection(const ISamplerStateCollection &source);
			ISamplerStateCollection &operator =(const ISamplerStateCollection &source);
		};
		typedef SmartRefCount<ISamplerStateCollection> ISamplerStateCollectionPtr;
	#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
