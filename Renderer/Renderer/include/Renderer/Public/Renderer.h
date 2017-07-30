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


// Public comfort header putting everything within a single header


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <inttypes.h>	// For "uint32_t", "uint64_t" etc.
#include <string.h>		// For "strcpy()"
#ifndef RENDERER_NO_DEBUG
	#include <cassert>
#endif
#ifndef RENDERER_NO_STATISTICS
	#include <atomic>	// For "std::atomic<>"
#endif
#ifdef LINUX
	// Copied from Xlib.h
	struct _XDisplay;
#endif


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ILog;
	class Context;
	class IRenderer;
	class IShaderLanguage;
	class IResource;
		class IRootSignature;
		class IResourceGroup;
		class IProgram;
		class IVertexArray;
		class IRenderTarget;
			class ISwapChain;
			class IFramebuffer;
			struct FramebufferAttachment;
		class CommandBuffer;
		class IBufferManager;
		class IBuffer;
			class IIndexBuffer;
			class IVertexBuffer;
			class IUniformBuffer;
			class ITextureBuffer;
			class IIndirectBuffer;
		class ITextureManager;
		class ITexture;
			class ITexture1D;
			class ITexture2D;
			class ITexture2DArray;
			class ITexture3D;
			class ITextureCube;
		class IState;
			class IPipelineState;
			class ISamplerState;
		class IShader;
			class IVertexShader;
			class ITessellationControlShader;
			class ITessellationEvaluationShader;
			class IGeometryShader;
			class IFragmentShader;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	// Renderer/PlatformTypes.h
	#ifndef __RENDERER_PLATFORM_TYPES_H__
	#define __RENDERER_PLATFORM_TYPES_H__
		#ifdef WIN32
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
			#define PRAGMA_WARNING_PUSH __pragma(warning(push))
			#define PRAGMA_WARNING_POP __pragma(warning(pop))
			#define PRAGMA_WARNING_DISABLE_MSVC(id) __pragma(warning(disable: id))
			#define PRAGMA_WARNING_DISABLE_CLANG(id)
			#define PRAGMA_WARNING_DISABLE_GCC(id)
		#elif LINUX
			#if X64_ARCHITECTURE
				typedef uint64_t handle;
			#else
				typedef uint32_t handle;
			#endif
			#ifndef NULL_HANDLE
				#define NULL_HANDLE 0
			#endif
			#define FORCEINLINE __attribute__((always_inline))
			#define NOP asm ("nop");
			#ifdef __clang__
				#define PRAGMA_WARNING_PUSH _Pragma("clang diagnostic push")
				#define PRAGMA_WARNING_POP _Pragma("clang diagnostic pop")
				#define PRAGMA_WARNING_DISABLE_MSVC(id)
				#define PRAGMA_WARNING_DISABLE_GCC(id)
				#define PRAGMA_STRINGIFY(a) #a
				#define PRAGMA_WARNING_DISABLE_CLANG(id) _Pragma(PRAGMA_STRINGIFY(clang diagnostic ignored id) )
			#elif __GNUC__
				#define PRAGMA_WARNING_PUSH _Pragma("GCC diagnostic push")
				#define PRAGMA_WARNING_POP _Pragma("GCC diagnostic pop")
				#define PRAGMA_WARNING_DISABLE_MSVC(id)
				#define PRAGMA_STRINGIFY(a) #a
				#define PRAGMA_WARNING_DISABLE_GCC(id) _Pragma(PRAGMA_STRINGIFY(GCC diagnostic ignored id) )
				#define PRAGMA_WARNING_DISABLE_CLANG(id)
			#else
				#error "Unsupported compiler"
			#endif
		#else
			#error "Unsupported platform"
		#endif
		#ifdef RENDERER_NO_DEBUG
			#define RENDERER_SET_RESOURCE_DEBUG_NAME(resource, name)
		#else
			#define RENDERER_SET_RESOURCE_DEBUG_NAME(resource, name) if (nullptr != resource) { (resource)->setDebugName(name); }
		#endif
	#endif

	// Renderer/Context.h
	#ifndef __RENDERER_RENDERER_CONTEXT_H__
	#define __RENDERER_RENDERER_CONTEXT_H__
		class Context
		{
		public:
			enum class ContextType
			{
				WINDOWS,
				X11,
				WAYLAND
			};
		public:
			inline Context(ContextType contextType, ILog& log, handle nativeWindowHandle = 0, bool useExternalContext = false) :
				mContextType(contextType),
				mLog(log),
				mNativeWindowHandle(nativeWindowHandle),
				mUseExternalContext(useExternalContext),
				mRendererApiSharedLibrary(nullptr)
			{}
			inline virtual ~Context()
			{}
			inline ContextType getType() const
			{
				return mContextType;
			}
			inline ILog& getLog() const
			{
				return mLog;
			}
			inline handle getNativeWindowHandle() const
			{
				return mNativeWindowHandle;
			}
			inline bool isUsingExternalContext() const
			{
				return mUseExternalContext;
			}
			inline void* getRendererApiSharedLibrary() const
			{
				return mRendererApiSharedLibrary;
			}
			inline void setRendererApiSharedLibrary(void* rendererApiSharedLibrary)
			{
				mRendererApiSharedLibrary = rendererApiSharedLibrary;
			}
		private:
			explicit Context(const Context&) = delete;
			Context& operator=(const Context&) = delete;
		private:
			ContextType	mContextType;
			ILog&		mLog;
			handle		mNativeWindowHandle;
			bool		mUseExternalContext;
			void*		mRendererApiSharedLibrary;
		};

	#ifdef LINUX
		class X11Context : public Context
		{
		public:
			inline X11Context(ILog& log, _XDisplay* display, handle nativeWindowHandle = 0, bool useExternalContext = false) :
				Context(Context::ContextType::X11, log, nativeWindowHandle, useExternalContext),
				mDisplay(display)
			{}
			inline _XDisplay* getDisplay() const
			{
				return mDisplay;
			}
		private:
			_XDisplay* mDisplay;
		};
	#endif
		#define RENDERER_LOG(context, type, format, ...) (context).getLog().print(Renderer::ILog::Type::type, format, ##__VA_ARGS__);
	#endif

	// Renderer/Log/ILog.h
	#ifndef __RENDERER_RENDERER_ILOG_H__
	#define __RENDERER_RENDERER_ILOG_H__
		class ILog
		{
		public:
			enum class Type
			{
				TRACE,
				DEBUG,
				INFORMATION,
				WARNING,
				PERFORMANCE_WARNING,
				COMPATIBILITY_WARNING,
				CRITICAL
			};
		public:
			virtual void print(Type type, const char* format, ...) = 0;
		protected:
			inline ILog()
			{}
			inline virtual ~ILog()
			{}
			explicit ILog(const ILog&) = delete;
			ILog& operator=(const ILog&) = delete;
		};
	#endif

	// Renderer/RendererTypes.h
	#ifndef __RENDERER_RENDERER_TYPES_H__
	#define __RENDERER_RENDERER_TYPES_H__
		enum class MapType
		{
			READ			   = 1,
			WRITE			   = 2,
			READ_WRITE		   = 3,
			WRITE_DISCARD	   = 4,
			WRITE_NO_OVERWRITE = 5
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
				COLOR       = 1 << 0,
				DEPTH       = 1 << 1,
				STENCIL     = 1 << 2,
				COLOR_DEPTH = COLOR | DEPTH
			};
		};
		enum class ComparisonFunc
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
		struct ColorWriteEnableFlag
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
			void*	  data;
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
		enum class ResourceType
		{
			ROOT_SIGNATURE				   = 0,
			RESOURCE_GROUP				   = 1,
			PROGRAM						   = 2,
			VERTEX_ARRAY				   = 3,
			SWAP_CHAIN					   = 4,
			FRAMEBUFFER					   = 5,
			INDEX_BUFFER				   = 6,
			VERTEX_BUFFER				   = 7,
			UNIFORM_BUFFER				   = 8,
			TEXTURE_BUFFER				   = 9,
			INDIRECT_BUFFER				   = 10,
			TEXTURE_1D					   = 11,
			TEXTURE_2D					   = 12,
			TEXTURE_2D_ARRAY			   = 13,
			TEXTURE_3D					   = 14,
			TEXTURE_CUBE				   = 15,
			PIPELINE_STATE				   = 16,
			SAMPLER_STATE				   = 17,
			VERTEX_SHADER				   = 18,
			TESSELLATION_CONTROL_SHADER	   = 19,
			TESSELLATION_EVALUATION_SHADER = 20,
			GEOMETRY_SHADER				   = 21,
			FRAGMENT_SHADER				   = 22
		};
	#endif

	// Renderer/State/SamplerStateTypes.h
	#ifndef __RENDERER_SAMPLERSTATE_TYPES_H__
	#define __RENDERER_SAMPLERSTATE_TYPES_H__
		enum class FilterMode
		{
			MIN_MAG_MIP_POINT						   = 0,
			MIN_MAG_POINT_MIP_LINEAR				   = 0x1,
			MIN_POINT_MAG_LINEAR_MIP_POINT			   = 0x4,
			MIN_POINT_MAG_MIP_LINEAR				   = 0x5,
			MIN_LINEAR_MAG_MIP_POINT				   = 0x10,
			MIN_LINEAR_MAG_POINT_MIP_LINEAR			   = 0x11,
			MIN_MAG_LINEAR_MIP_POINT				   = 0x14,
			MIN_MAG_MIP_LINEAR						   = 0x15,
			ANISOTROPIC								   = 0x55,
			COMPARISON_MIN_MAG_MIP_POINT			   = 0x80,
			COMPARISON_MIN_MAG_POINT_MIP_LINEAR		   = 0x81,
			COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT  = 0x84,
			COMPARISON_MIN_POINT_MAG_MIP_LINEAR		   = 0x85,
			COMPARISON_MIN_LINEAR_MAG_MIP_POINT		   = 0x90,
			COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
			COMPARISON_MIN_MAG_LINEAR_MIP_POINT		   = 0x94,
			COMPARISON_MIN_MAG_MIP_LINEAR			   = 0x95,
			COMPARISON_ANISOTROPIC					   = 0xd5,
			UNKNOWN									   = 0xd6
		};
		enum class TextureAddressMode
		{
			WRAP		= 1,
			MIRROR		= 2,
			CLAMP		= 3,
			BORDER		= 4,
			MIRROR_ONCE	= 5
		};
		struct SamplerState
		{
			FilterMode		   filter;
			TextureAddressMode addressU;
			TextureAddressMode addressV;
			TextureAddressMode addressW;
			float			   mipLODBias;
			uint32_t		   maxAnisotropy;
			ComparisonFunc	   comparisonFunc;
			float			   borderColor[4];
			float			   minLOD;
			float			   maxLOD;
		};
	#endif

	// Renderer/RootSignatureTypes.h
	#ifndef __RENDERER_ROOTSIGNATURE_TYPES_H__
	#define __RENDERER_ROOTSIGNATURE_TYPES_H__
		enum class DescriptorRangeType
		{
			SRV					  = 0,
			UAV					  = SRV + 1,
			UBV					  = UAV + 1,
			SAMPLER				  = UBV + 1,
			NUMBER_OF_RANGE_TYPES = SAMPLER + 1
		};
		enum class ShaderVisibility
		{
			ALL                     = 0,
			VERTEX                  = 1,
			TESSELLATION_CONTROL    = 2,
			TESSELLATION_EVALUATION = 3,
			GEOMETRY                = 4,
			FRAGMENT                = 5
		};
		struct DescriptorRange
		{
			static const uint32_t NAME_LENGTH = 32;
			DescriptorRangeType   rangeType;
			uint32_t			  numberOfDescriptors;
			uint32_t			  baseShaderRegister;
			uint32_t			  registerSpace;
			uint32_t			  offsetInDescriptorsFromTableStart;
			char				  baseShaderRegisterName[NAME_LENGTH];
			uint32_t			  samplerRootParameterIndex;
			ShaderVisibility	  shaderVisibility;
		};
		struct DescriptorRangeBuilder : public DescriptorRange
		{
			static const uint32_t OFFSET_APPEND	= 0xffffffff;
			DescriptorRangeBuilder()
			{}
			explicit DescriptorRangeBuilder(const DescriptorRangeBuilder&)
			{}
			DescriptorRangeBuilder(
				DescriptorRangeType _rangeType,
				uint32_t _numberOfDescriptors,
				uint32_t _baseShaderRegister,
				const char _baseShaderRegisterName[NAME_LENGTH],
				uint32_t _samplerRootParameterIndex,
				ShaderVisibility _shaderVisibility,
				uint32_t _registerSpace = 0,
				uint32_t _offsetInDescriptorsFromTableStart = OFFSET_APPEND)
			{
				initialize(_rangeType, _numberOfDescriptors, _baseShaderRegister, _baseShaderRegisterName, _samplerRootParameterIndex, _shaderVisibility, _registerSpace, _offsetInDescriptorsFromTableStart);
			}
			inline void initializeSampler(
				uint32_t _numberOfDescriptors,
				uint32_t _baseShaderRegister,
				ShaderVisibility _shaderVisibility,
				uint32_t _registerSpace = 0,
				uint32_t _offsetInDescriptorsFromTableStart = OFFSET_APPEND)
			{
				initialize(*this, DescriptorRangeType::SAMPLER, _numberOfDescriptors, _baseShaderRegister, "", 0, _shaderVisibility, _registerSpace, _offsetInDescriptorsFromTableStart);
			}
			inline void initialize(
				DescriptorRangeType _rangeType,
				uint32_t _numberOfDescriptors,
				uint32_t _baseShaderRegister,
				const char _baseShaderRegisterName[NAME_LENGTH],
				uint32_t _samplerRootParameterIndex,
				ShaderVisibility _shaderVisibility,
				uint32_t _registerSpace = 0,
				uint32_t _offsetInDescriptorsFromTableStart = OFFSET_APPEND)
			{
				initialize(*this, _rangeType, _numberOfDescriptors, _baseShaderRegister, _baseShaderRegisterName, _samplerRootParameterIndex, _shaderVisibility, _registerSpace, _offsetInDescriptorsFromTableStart);
			}
			static inline void initialize(
				DescriptorRange& range,
				DescriptorRangeType _rangeType,
				uint32_t _numberOfDescriptors,
				uint32_t _baseShaderRegister,
				const char _baseShaderRegisterName[NAME_LENGTH],
				uint32_t _samplerRootParameterIndex,
				ShaderVisibility _shaderVisibility,
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
				range.shaderVisibility = _shaderVisibility;
			}
		};
		struct RootDescriptorTable
		{
			uint32_t numberOfDescriptorRanges;
			uint64_t descriptorRanges;
		};
		struct RootDescriptorTableBuilder : public RootDescriptorTable
		{
			RootDescriptorTableBuilder()
			{}
			explicit RootDescriptorTableBuilder(const RootDescriptorTableBuilder&)
			{}
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
				RootDescriptorTable& rootDescriptorTable,
				uint32_t _numberOfDescriptorRanges,
				const DescriptorRange* _descriptorRanges)
			{
				rootDescriptorTable.numberOfDescriptorRanges = _numberOfDescriptorRanges;
				rootDescriptorTable.descriptorRanges = reinterpret_cast<uintptr_t>(_descriptorRanges);
			}
		};
		enum class RootParameterType
		{
			DESCRIPTOR_TABLE = 0,
			CONSTANTS_32BIT  = DESCRIPTOR_TABLE + 1,
			UBV              = CONSTANTS_32BIT + 1,
			SRV              = UBV + 1,
			UAV              = SRV + 1
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
			{}
			explicit RootConstantsBuilder(const RootConstantsBuilder&)
			{}
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
			{}
			explicit RootDescriptorBuilder(const RootDescriptorBuilder&)
			{}
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
		struct RootParameter
		{
			RootParameterType		parameterType;
			union
			{
				RootDescriptorTable	descriptorTable;
				RootConstants		constants;
				RootDescriptor		descriptor;
			};
		};
		struct RootParameterData
		{
			RootParameterType parameterType;
			uint32_t		  numberOfDescriptorRanges;
		};
		struct RootParameterBuilder : public RootParameter
		{
			RootParameterBuilder()
			{}
			explicit RootParameterBuilder(const RootParameterBuilder&)
			{}
			static inline void initializeAsDescriptorTable(
				RootParameter& rootParam,
				uint32_t numberOfDescriptorRanges,
				const DescriptorRange* descriptorRanges)
			{
				rootParam.parameterType = RootParameterType::DESCRIPTOR_TABLE;
				RootDescriptorTableBuilder::initialize(rootParam.descriptorTable, numberOfDescriptorRanges, descriptorRanges);
			}
			static inline void initializeAsConstants(
				RootParameter& rootParam,
				uint32_t numberOf32BitValues,
				uint32_t shaderRegister,
				uint32_t registerSpace = 0)
			{
				rootParam.parameterType = RootParameterType::CONSTANTS_32BIT;
				RootConstantsBuilder::initialize(rootParam.constants, numberOf32BitValues, shaderRegister, registerSpace);
			}
			static inline void initializeAsConstantBufferView(
				RootParameter& rootParam,
				uint32_t shaderRegister,
				uint32_t registerSpace = 0)
			{
				rootParam.parameterType = RootParameterType::UBV;
				RootDescriptorBuilder::initialize(rootParam.descriptor, shaderRegister, registerSpace);
			}
			static inline void initializeAsShaderResourceView(
				RootParameter& rootParam,
				uint32_t shaderRegister,
				uint32_t registerSpace = 0)
			{
				rootParam.parameterType = RootParameterType::SRV;
				RootDescriptorBuilder::initialize(rootParam.descriptor, shaderRegister, registerSpace);
			}
			static inline void initializeAsUnorderedAccessView(
				RootParameter& rootParam,
				uint32_t shaderRegister,
				uint32_t registerSpace = 0)
			{
				rootParam.parameterType = RootParameterType::UAV;
				RootDescriptorBuilder::initialize(rootParam.descriptor, shaderRegister, registerSpace);
			}
			inline void initializeAsDescriptorTable(
				uint32_t numberOfDescriptorRanges,
				const DescriptorRange* descriptorRanges)
			{
				initializeAsDescriptorTable(*this, numberOfDescriptorRanges, descriptorRanges);
			}
			inline void initializeAsConstants(
				uint32_t numberOf32BitValues,
				uint32_t shaderRegister,
				uint32_t registerSpace = 0)
			{
				initializeAsConstants(*this, numberOf32BitValues, shaderRegister, registerSpace);
			}
			inline void initializeAsConstantBufferView(
				uint32_t shaderRegister,
				uint32_t registerSpace = 0)
			{
				initializeAsConstantBufferView(*this, shaderRegister, registerSpace);
			}
			inline void initializeAsShaderResourceView(
				uint32_t shaderRegister,
				uint32_t registerSpace = 0)
			{
				initializeAsShaderResourceView(*this, shaderRegister, registerSpace);
			}
			inline void initializeAsUnorderedAccessView(
				uint32_t shaderRegister,
				uint32_t registerSpace = 0)
			{
				initializeAsUnorderedAccessView(*this, shaderRegister, registerSpace);
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
		enum class StaticBorderColor
		{
			TRANSPARENT_BLACK = 0,
			OPAQUE_BLACK      = TRANSPARENT_BLACK + 1,
			OPAQUE_WHITE      = OPAQUE_BLACK + 1
		};
		struct StaticSampler
		{
			FilterMode			filter;
			TextureAddressMode	addressU;
			TextureAddressMode	addressV;
			TextureAddressMode	addressW;
			float				mipLodBias;
			uint32_t			maxAnisotropy;
			ComparisonFunc		comparisonFunc;
			StaticBorderColor	borderColor;
			float				minLod;
			float				maxLod;
			uint32_t			shaderRegister;
			uint32_t			registerSpace;
			ShaderVisibility	shaderVisibility;
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
			{}
			explicit RootSignatureBuilder(const RootSignatureBuilder&)
			{}
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
				RootSignature& rootSignature,
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

	// Renderer/Texture/TextureTypes.h
	#ifndef __RENDERER_TEXTURE_TYPES_H__
	#define __RENDERER_TEXTURE_TYPES_H__
		struct TextureFormat
		{
			enum Enum
			{
				R8				  = 0,
				R8G8B8			  = 1,
				R8G8B8A8		  = 2,
				R8G8B8A8_SRGB	  = 3,
				R11G11B10F		  = 4,
				R16G16B16A16F	  = 5,
				R32G32B32A32F	  = 6,
				BC1				  = 7,
				BC1_SRGB		  = 8,
				BC2				  = 9,
				BC2_SRGB		  = 10,
				BC3				  = 11,
				BC3_SRGB		  = 12,
				BC4				  = 13,
				BC5				  = 14,
				ETC1			  = 15,
				R32_UINT		  = 16,
				R32_FLOAT		  = 17,
				D32_FLOAT		  = 18,
				R16G16_SNORM	  = 19,
				R16G16_FLOAT	  = 20,
				UNKNOWN			  = 21,
				NUMBER_OF_FORMATS = 22
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
					false,
					false,
					true,
					true,
					true,
					true,
					true,
					true,
					true,
					true,
					true,
					false,
					false,
					false,
					false,
					false,
					false
				};
				return MAPPING[textureFormat];
			}
			inline static bool isDepth(Enum textureFormat)
			{
				static bool MAPPING[] =
				{
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					false,
					true,
					false,
					false,
					false
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
					sizeof(uint8_t) * 4,
					sizeof(float),
					sizeof(float) * 2,
					sizeof(float) * 4,
					sizeof(uint8_t) * 3,
					sizeof(uint8_t) * 3,
					sizeof(uint8_t) * 4,
					sizeof(uint8_t) * 4,
					sizeof(uint8_t) * 4,
					sizeof(uint8_t) * 4,
					sizeof(uint8_t) * 1,
					sizeof(uint8_t) * 2,
					sizeof(uint8_t) * 3,
					sizeof(uint32_t),
					sizeof(float),
					sizeof(float),
					sizeof(uint32_t),
					sizeof(float),
					0
				};
				return MAPPING[textureFormat];
			}
			inline static uint32_t getNumberOfBytesPerRow(Enum textureFormat, uint32_t width)
			{
				switch (textureFormat)
				{
					case R8:
						return width;
					case R8G8B8:
						return 3 * width;
					case R8G8B8A8:
					case R8G8B8A8_SRGB:
						return 4 * width;
					case R11G11B10F:
						return 4 * width;
					case R16G16B16A16F:
						return 8 * width;
					case R32G32B32A32F:
						return 16 * width;
					case BC1:
					case BC1_SRGB:
						return ((width + 3) >> 2) * 8;
					case BC2:
					case BC2_SRGB:
						return ((width + 3) >> 2) * 16;
					case BC3:
					case BC3_SRGB:
						return ((width + 3) >> 2) * 16;
					case BC4:
						return ((width + 3) >> 2) * 8;
					case BC5:
						return ((width + 3) >> 2) * 16;
					case ETC1:
						return (width >> 1);
					case R32_UINT:
						return sizeof(uint32_t) * width;
					case R32_FLOAT:
					case D32_FLOAT:
						return sizeof(float) * width;
					case R16G16_SNORM:
						return sizeof(uint32_t) * width;
					case R16G16_FLOAT:
						return sizeof(float) * width;
					case UNKNOWN:
					case NUMBER_OF_FORMATS:
						return 0;
					default:
						return 0;
				}
			}
			inline static uint32_t getNumberOfBytesPerSlice(Enum textureFormat, uint32_t width, uint32_t height)
			{
				switch (textureFormat)
				{
					case R8:
						return width * height;
					case R8G8B8:
						return 3 * width * height;
					case R8G8B8A8:
					case R8G8B8A8_SRGB:
						return 4 * width * height;
					case R11G11B10F:
						return 4 * width * height;
					case R16G16B16A16F:
						return 8 * width * height;
					case R32G32B32A32F:
						return 16 * width * height;
					case BC1:
					case BC1_SRGB:
						return ((width + 3) >> 2) * ((height + 3) >> 2) * 8;
					case BC2:
					case BC2_SRGB:
						return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;
					case BC3:
					case BC3_SRGB:
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
					case R32_UINT:
						return sizeof(uint32_t) * width * height;
					case R32_FLOAT:
					case D32_FLOAT:
						return sizeof(float) * width * height;
					case R16G16_SNORM:
						return sizeof(uint32_t) * width * height;
					case R16G16_FLOAT:
						return sizeof(float) * width * height;
					case UNKNOWN:
					case NUMBER_OF_FORMATS:
						return 0;
					default:
						return 0;
				}
			}
		};
		struct TextureFlag
		{
			enum Enum
			{
				DATA_CONTAINS_MIPMAPS = 1 << 0,
				GENERATE_MIPMAPS      = 1 << 1,
				RENDER_TARGET         = 1 << 2
			};
		};
		enum class TextureUsage
		{
			DEFAULT   = 0,
			IMMUTABLE = 1,
			DYNAMIC   = 2,
			STAGING   = 3
		};
		struct OptimizedTextureClearValue
		{
			union
			{
				float color[4];
				struct
				{
					float depth;
					uint8_t stencil;
				} DepthStencil;
			};
		};
	#endif

	// Renderer/State/BlendStateTypes.h
	#ifndef __RENDERER_BLENDSTATE_TYPES_H__
	#define __RENDERER_BLENDSTATE_TYPES_H__
		enum class Blend
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
		enum class BlendOp
		{
			ADD			 = 1,
			SUBTRACT	 = 2,
			REV_SUBTRACT = 3,
			MIN			 = 4,
			MAX			 = 5
		};
		struct RenderTargetBlendDesc
		{
			int		blendEnable;
			Blend	srcBlend;
			Blend	destBlend;
			BlendOp	blendOp;
			Blend	srcBlendAlpha;
			Blend	destBlendAlpha;
			BlendOp	blendOpAlpha;
			uint8_t	renderTargetWriteMask;
		};
		struct BlendState
		{
			int					  alphaToCoverageEnable;
			int					  independentBlendEnable;
			RenderTargetBlendDesc renderTarget[8];
		};
		struct BlendStateBuilder
		{
			static inline const BlendState& getDefaultBlendState()
			{
				static const BlendState BLEND_STATE =
				{
					false,
					false,
					{
						{
							false,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							ColorWriteEnableFlag::ALL,
						},
						{
							false,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							ColorWriteEnableFlag::ALL,
						},
						{
							false,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							ColorWriteEnableFlag::ALL,
						},
						{
							false,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							ColorWriteEnableFlag::ALL,
						},
						{
							false,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							ColorWriteEnableFlag::ALL,
						},
						{
							false,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							ColorWriteEnableFlag::ALL,
						},
						{
							false,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							ColorWriteEnableFlag::ALL,
						},
						{
							false,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							Blend::ONE,
							Blend::ZERO,
							BlendOp::ADD,
							ColorWriteEnableFlag::ALL,
						},
					}
				};
				return BLEND_STATE;
			}
		};
	#endif

	// Renderer/Buffer/BufferTypes.h
	#ifndef __RENDERER_BUFFER_TYPES_H__
	#define __RENDERER_BUFFER_TYPES_H__
		enum class BufferUsage
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
	#endif

	// Renderer/Buffer/VertexArrayTypes.h
	#ifndef __RENDERER_VERTEXARRAY_TYPES_H__
	#define __RENDERER_VERTEXARRAY_TYPES_H__
		enum class VertexAttributeFormat : uint8_t
		{
			FLOAT_1			= 0,
			FLOAT_2			= 1,
			FLOAT_3			= 2,
			FLOAT_4			= 3,
			R8G8B8A8_UNORM	= 4,
			R8G8B8A8_UINT	= 5,
			SHORT_2			= 6,
			SHORT_4			= 7,
			UINT_1			= 8
		};
		#pragma pack(push)
		#pragma pack(1)
			struct VertexAttribute
			{
				VertexAttributeFormat vertexAttributeFormat;
				char				  name[32];
				char				  semanticName[32];
				uint32_t			  semanticIndex;
				uint32_t			  inputSlot;
				uint32_t			  alignedByteOffset;
				uint32_t			  strideInBytes;
				uint32_t			  instancesPerElement;
			};
			struct VertexAttributes
			{
				uint32_t				numberOfAttributes;
				const VertexAttribute*	attributes;
				VertexAttributes() :
					numberOfAttributes(0),
					attributes(nullptr)
				{}
				VertexAttributes(uint32_t _numberOfAttributes, const VertexAttribute* _attributes) :
					numberOfAttributes(_numberOfAttributes),
					attributes(_attributes)
				{}
			};
		#pragma pack(pop)
		struct VertexArrayVertexBuffer
		{
			IVertexBuffer* vertexBuffer;
		};
	#endif

	// Renderer/Buffer/IndexBufferTypes.h
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

	// Renderer/Buffer/IndirectBufferTypes.h
	#ifndef __RENDERER_INDIRECTBUFFER_TYPES_H__
	#define __RENDERER_INDIRECTBUFFER_TYPES_H__
		struct DrawInstancedArguments
		{
			uint32_t vertexCountPerInstance;
			uint32_t instanceCount;
			uint32_t startVertexLocation;
			uint32_t startInstanceLocation;
			DrawInstancedArguments(uint32_t _vertexCountPerInstance, uint32_t _instanceCount = 1, uint32_t _startVertexLocation = 0, uint32_t _startInstanceLocation = 0) :
				vertexCountPerInstance(_vertexCountPerInstance),
				instanceCount(_instanceCount),
				startVertexLocation(_startVertexLocation),
				startInstanceLocation(_startInstanceLocation)
			{}
		};
		struct DrawIndexedInstancedArguments
		{
			uint32_t indexCountPerInstance;
			uint32_t instanceCount;
			uint32_t startIndexLocation;
			int32_t  baseVertexLocation;
			uint32_t startInstanceLocation;
			DrawIndexedInstancedArguments(uint32_t _indexCountPerInstance, uint32_t _instanceCount = 1, uint32_t _startIndexLocation = 0, int32_t _baseVertexLocation = 0, uint32_t _startInstanceLocation = 0) :
				indexCountPerInstance(_indexCountPerInstance),
				instanceCount(_instanceCount),
				startIndexLocation(_startIndexLocation),
				baseVertexLocation(_baseVertexLocation),
				startInstanceLocation(_startInstanceLocation)
			{}
		};
	#endif

	// Renderer/State/RasterizerStateTypes.h
	#ifndef __RENDERER_RASTERIZERSTATE_TYPES_H__
	#define __RENDERER_RASTERIZERSTATE_TYPES_H__
		enum class FillMode
		{
			WIREFRAME = 2,
			SOLID     = 3
		};
		enum class CullMode
		{
			NONE  = 1,
			FRONT = 2,
			BACK  = 3
		};
		enum class ConservativeRasterizationMode
		{
			OFF	= 0,
			ON	= 1
		};
		struct RasterizerState
		{
			FillMode						fillMode;
			CullMode						cullMode;
			int								frontCounterClockwise;
			int								depthBias;
			float							depthBiasClamp;
			float							slopeScaledDepthBias;
			int								depthClipEnable;
			int								multisampleEnable;
			int								antialiasedLineEnable;
			unsigned int					forcedSampleCount;
			ConservativeRasterizationMode	conservativeRasterizationMode;
			int								scissorEnable;
		};
		struct RasterizerStateBuilder
		{
			static inline const RasterizerState& getDefaultRasterizerState()
			{
				static const RasterizerState RASTERIZER_STATE =
				{
					FillMode::SOLID,
					CullMode::BACK,
					false,
					0,
					0.0f,
					0.0f,
					true,
					false,
					false,
					0,
					ConservativeRasterizationMode::OFF,
					false
				};
				return RASTERIZER_STATE;
			}
		};
	#endif

	// Renderer/State/DepthStencilStateTypes.h
	#ifndef __RENDERER_DEPTHSTENCILSTATE_TYPES_H__
	#define __RENDERER_DEPTHSTENCILSTATE_TYPES_H__
		enum class DepthWriteMask
		{
			ZERO = 0,
			ALL  = 1
		};
		enum class StencilOp
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
		struct DepthStencilOpDesc
		{
			StencilOp		stencilFailOp;
			StencilOp		stencilDepthFailOp;
			StencilOp		stencilPassOp;
			ComparisonFunc	stencilFunc;
		};
		struct DepthStencilState
		{
			int					depthEnable;
			DepthWriteMask		depthWriteMask;
			ComparisonFunc		depthFunc;
			int					stencilEnable;
			uint8_t				stencilReadMask;
			uint8_t				stencilWriteMask;
			DepthStencilOpDesc	frontFace;
			DepthStencilOpDesc	backFace;
		};
		struct DepthStencilStateBuilder
		{
			static inline const DepthStencilState& getDefaultDepthStencilState()
			{
				static const DepthStencilState DEPTH_STENCIL_STATE =
				{
					true,
					DepthWriteMask::ALL,
					ComparisonFunc::LESS,
					false,
					0xff,
					0xff,
					{
						StencilOp::KEEP,
						StencilOp::KEEP,
						StencilOp::KEEP,
						ComparisonFunc::ALWAYS
					},
					{
						StencilOp::KEEP,
						StencilOp::KEEP,
						StencilOp::KEEP,
						ComparisonFunc::ALWAYS
					}
				};
				return DEPTH_STENCIL_STATE;
			}
		};
	#endif

	// Renderer/State/PipelineStateTypes.h
	#ifndef __RENDERER_PIPELINESTATE_TYPES_H__
	#define __RENDERER_PIPELINESTATE_TYPES_H__
		enum class PrimitiveTopology
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
		enum class PrimitiveTopologyType
		{
			UNDEFINED	= 0,
			POINT		= 1,
			LINE		= 2,
			TRIANGLE	= 3,
			PATCH		= 4
		};
		struct SerializedPipelineState
		{
			PrimitiveTopology	  primitiveTopology;
			PrimitiveTopologyType primitiveTopologyType;
			RasterizerState		  rasterizerState;
			DepthStencilState	  depthStencilState;
			BlendState			  blendState;
			uint32_t			  numberOfRenderTargets;
			TextureFormat::Enum	  renderTargetViewFormats[8];
			TextureFormat::Enum	  depthStencilViewFormat;
		};
		struct PipelineState : public SerializedPipelineState
		{
			IRootSignature*  rootSignature;
			IProgram*		 program;
			VertexAttributes vertexAttributes;
		};
		struct PipelineStateBuilder : public PipelineState
		{
			PipelineStateBuilder()
			{
				rootSignature						= nullptr;
				program								= nullptr;
				vertexAttributes.numberOfAttributes	= 0;
				vertexAttributes.attributes			= nullptr;
				primitiveTopology					= PrimitiveTopology::TRIANGLE_LIST;
				primitiveTopologyType				= PrimitiveTopologyType::TRIANGLE;
				rasterizerState						= RasterizerStateBuilder::getDefaultRasterizerState();
				depthStencilState					= DepthStencilStateBuilder::getDefaultDepthStencilState();
				blendState							= BlendStateBuilder::getDefaultBlendState();
				numberOfRenderTargets				= 1;
				renderTargetViewFormats[0]			= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[1]			= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[2]			= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[3]			= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[4]			= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[5]			= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[6]			= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[7]			= TextureFormat::R8G8B8A8;
				depthStencilViewFormat				= TextureFormat::D32_FLOAT;
			}
			PipelineStateBuilder(IRootSignature* _rootSignature, IProgram* _program, const VertexAttributes& _vertexAttributes)
			{
				rootSignature				= _rootSignature;
				program						= _program;
				vertexAttributes			= _vertexAttributes;
				primitiveTopology			= PrimitiveTopology::TRIANGLE_LIST;
				primitiveTopologyType		= PrimitiveTopologyType::TRIANGLE;
				rasterizerState				= RasterizerStateBuilder::getDefaultRasterizerState();
				depthStencilState			= DepthStencilStateBuilder::getDefaultDepthStencilState();
				blendState					= BlendStateBuilder::getDefaultBlendState();
				numberOfRenderTargets		= 1;
				renderTargetViewFormats[0]	= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[1]	= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[2]	= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[3]	= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[4]	= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[5]	= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[6]	= TextureFormat::R8G8B8A8;
				renderTargetViewFormats[7]	= TextureFormat::R8G8B8A8;
				depthStencilViewFormat		= TextureFormat::D32_FLOAT;
			}
		};
	#endif

	// Renderer/Shader/ShaderTypes.h
	#ifndef __RENDERER_SHADER_TYPES_H__
	#define __RENDERER_SHADER_TYPES_H__
		enum class GsInputPrimitiveTopology
		{
			POINTS				= 0x0000,
			LINES				= 0x0001,
			LINES_ADJACENCY		= 0x000A,
			TRIANGLES			= 0x0004,
			TRIANGLES_ADJACENCY	= 0x000C
		};
		enum class GsOutputPrimitiveTopology
		{
			POINTS		   = 0x0000,
			LINES		   = 0x0001,
			TRIANGLE_STRIP = 0x0005
		};
		class ShaderBytecode
		{
		public:
			inline ShaderBytecode() :
				mNumberOfBytes(0),
				mBytecode(nullptr)
			{}
			inline ~ShaderBytecode()
			{
				delete [] mBytecode;
			}
			inline uint32_t getNumberOfBytes() const
			{
				return mNumberOfBytes;
			}
			inline const uint8_t* getBytecode() const
			{
				return mBytecode;
			}
			inline void setBytecodeCopy(uint32_t numberOfBytes, uint8_t* bytecode)
			{
				delete [] mBytecode;
				mNumberOfBytes = numberOfBytes;
				mBytecode = new uint8_t[mNumberOfBytes];
				memcpy(mBytecode, bytecode, mNumberOfBytes);
			}
		private:
			uint32_t mNumberOfBytes;
			uint8_t* mBytecode;
		};
		struct ShaderSourceCode
		{
			const char* sourceCode	= nullptr;
			const char* profile		= nullptr;
			const char* arguments	= nullptr;
			const char* entry		= nullptr;
			inline ShaderSourceCode(const char* _sourceCode) :
				sourceCode(_sourceCode)
			{};
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
			{}
			virtual ~RefCount()
			{}
			virtual const AType* getPointer() const
			{
				return static_cast<const AType*>(this);
			}
			virtual AType* getPointer()
			{
				return static_cast<AType*>(this);
			}
			uint32_t addReference()
			{
				++mRefCount;
				return mRefCount;
			}
			uint32_t releaseReference()
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
			{}
			explicit SmartRefCount(AType* ptr) :
				mPtr(nullptr)
			{
				setPtr(ptr);
			}
			SmartRefCount(const SmartRefCount<AType>& ptr) :
				mPtr(nullptr)
			{
				setPtr(ptr.getPtr());
			}
			~SmartRefCount()
			{
				setPtr(nullptr);
			}
			SmartRefCount<AType>& operator =(AType* ptr)
			{
				if (getPointer() != ptr)
				{
					setPtr(ptr);
				}
				return *this;
			}
			SmartRefCount<AType>& operator =(const SmartRefCount<AType>& ptr)
			{
				if (getPointer() != ptr.getPointer())
				{
					setPtr(ptr.getPtr());
				}
				return *this;
			}
			AType* getPointer() const
			{
				return mPtr ? static_cast<AType*>(mPtr->getPointer()) : nullptr;
			}
			AType* operator ->() const
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
			bool operator ==(AType* ptr) const
			{
				return (getPointer() == ptr);
			}
			bool operator ==(const SmartRefCount<AType>& ptr) const
			{
				return (getPointer() == ptr.getPointer());
			}
			bool operator !=(AType* ptr) const
			{
				return (getPointer() != ptr);
			}
			bool operator !=(const SmartRefCount<AType>& ptr) const
			{
				return (getPointer() != ptr.getPointer());
			}
		private:
			void setPtr(AType* ptr)
			{
				if (nullptr != mPtr)
				{
					mPtr->releaseReference();
				}
				if (nullptr != ptr)
				{
					ptr->addReference();
				}
				mPtr = ptr;
			}
			AType* getPtr() const
			{
				return mPtr;
			}
		private:
			AType* mPtr;
		};
	#endif

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
			uint32_t maximumUniformBufferSize;
			uint32_t maximumTextureBufferSize;
			uint32_t maximumIndirectBufferSize;
			uint8_t  maximumNumberOfMultisamples;
			uint8_t  maximumAnisotropy;
			bool	 individualUniforms;
			bool	 instancedArrays;
			bool	 drawInstanced;
			bool	 baseVertex;
			bool	 nativeMultiThreading;
			bool	 shaderBytecode;
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
				maximumUniformBufferSize(0),
				maximumTextureBufferSize(0),
				maximumIndirectBufferSize(0),
				maximumNumberOfMultisamples(1),
				maximumAnisotropy(1),
				individualUniforms(false),
				instancedArrays(false),
				drawInstanced(false),
				baseVertex(false),
				nativeMultiThreading(false),
				shaderBytecode(false),
				vertexShader(false),
				maximumNumberOfPatchVertices(0),
				maximumNumberOfGsOutputVertices(0),
				fragmentShader(false)
			{}
			inline ~Capabilities()
			{}
		private:
			explicit Capabilities(const Capabilities&) = delete;
			Capabilities& operator =(const Capabilities&) = delete;
		};
	#endif

	// Renderer/Statistics.h
	#ifndef RENDERER_NO_STATISTICS
		#ifndef __RENDERER_STATISTICS_H__
		#define __RENDERER_STATISTICS_H__
		class Statistics
		{
		public:
			std::atomic<uint32_t> currentNumberOfRootSignatures;
			std::atomic<uint32_t> numberOfCreatedRootSignatures;
			std::atomic<uint32_t> currentNumberOfResourceGroups;
			std::atomic<uint32_t> numberOfCreatedResourceGroups;
			std::atomic<uint32_t> currentNumberOfPrograms;
			std::atomic<uint32_t> numberOfCreatedPrograms;
			std::atomic<uint32_t> currentNumberOfVertexArrays;
			std::atomic<uint32_t> numberOfCreatedVertexArrays;
			std::atomic<uint32_t> currentNumberOfSwapChains;
			std::atomic<uint32_t> numberOfCreatedSwapChains;
			std::atomic<uint32_t> currentNumberOfFramebuffers;
			std::atomic<uint32_t> numberOfCreatedFramebuffers;
			std::atomic<uint32_t> currentNumberOfIndexBuffers;
			std::atomic<uint32_t> numberOfCreatedIndexBuffers;
			std::atomic<uint32_t> currentNumberOfVertexBuffers;
			std::atomic<uint32_t> numberOfCreatedVertexBuffers;
			std::atomic<uint32_t> currentNumberOfUniformBuffers;
			std::atomic<uint32_t> numberOfCreatedUniformBuffers;
			std::atomic<uint32_t> currentNumberOfTextureBuffers;
			std::atomic<uint32_t> numberOfCreatedTextureBuffers;
			std::atomic<uint32_t> currentNumberOfIndirectBuffers;
			std::atomic<uint32_t> numberOfCreatedIndirectBuffers;
			std::atomic<uint32_t> currentNumberOfTexture1Ds;
			std::atomic<uint32_t> numberOfCreatedTexture1Ds;
			std::atomic<uint32_t> currentNumberOfTexture2Ds;
			std::atomic<uint32_t> numberOfCreatedTexture2Ds;
			std::atomic<uint32_t> currentNumberOfTexture2DArrays;
			std::atomic<uint32_t> numberOfCreatedTexture2DArrays;
			std::atomic<uint32_t> currentNumberOfTexture3Ds;
			std::atomic<uint32_t> numberOfCreatedTexture3Ds;
			std::atomic<uint32_t> currentNumberOfTextureCubes;
			std::atomic<uint32_t> numberOfCreatedTextureCubes;
			std::atomic<uint32_t> currentNumberOfPipelineStates;
			std::atomic<uint32_t> numberOfCreatedPipelineStates;
			std::atomic<uint32_t> currentNumberOfSamplerStates;
			std::atomic<uint32_t> numberOfCreatedSamplerStates;
			std::atomic<uint32_t> currentNumberOfVertexShaders;
			std::atomic<uint32_t> numberOfCreatedVertexShaders;
			std::atomic<uint32_t> currentNumberOfTessellationControlShaders;
			std::atomic<uint32_t> numberOfCreatedTessellationControlShaders;
			std::atomic<uint32_t> currentNumberOfTessellationEvaluationShaders;
			std::atomic<uint32_t> numberOfCreatedTessellationEvaluationShaders;
			std::atomic<uint32_t> currentNumberOfGeometryShaders;
			std::atomic<uint32_t> numberOfCreatedGeometryShaders;
			std::atomic<uint32_t> currentNumberOfFragmentShaders;
			std::atomic<uint32_t> numberOfCreatedFragmentShaders;
		public:
			inline Statistics() :
				currentNumberOfRootSignatures(0),
				numberOfCreatedRootSignatures(0),
				currentNumberOfResourceGroups(0),
				numberOfCreatedResourceGroups(0),
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
				currentNumberOfIndirectBuffers(0),
				numberOfCreatedIndirectBuffers(0),
				currentNumberOfTexture1Ds(0),
				numberOfCreatedTexture1Ds(0),
				currentNumberOfTexture2Ds(0),
				numberOfCreatedTexture2Ds(0),
				currentNumberOfTexture2DArrays(0),
				numberOfCreatedTexture2DArrays(0),
				currentNumberOfTexture3Ds(0),
				numberOfCreatedTexture3Ds(0),
				currentNumberOfTextureCubes(0),
				numberOfCreatedTextureCubes(0),
				currentNumberOfPipelineStates(0),
				numberOfCreatedPipelineStates(0),
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
				numberOfCreatedFragmentShaders(0)
			{}
			inline ~Statistics()
			{}
		private:
			explicit Statistics(const Statistics&) = delete;
			Statistics& operator =(const Statistics&) = delete;
		};
		#endif
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
			inline const Context& getContext() const
			{
				return mContext;
			}
			inline const Capabilities& getCapabilities() const
			{
				return mCapabilities;
			}
			#ifndef RENDERER_NO_STATISTICS
				inline const Statistics& getStatistics() const
				{
					return mStatistics;
				}
			#endif
		public:
			virtual const char* getName() const = 0;
			virtual bool isInitialized() const = 0;
			virtual bool isDebugEnabled() = 0;
			virtual ISwapChain* getMainSwapChain() const = 0;
			virtual uint32_t getNumberOfShaderLanguages() const = 0;
			virtual const char* getShaderLanguageName(uint32_t index) const = 0;
			virtual IShaderLanguage* getShaderLanguage(const char* shaderLanguageName = nullptr) = 0;
			virtual ISwapChain* createSwapChain(handle nativeWindowHandle, bool useExternalContext = false) = 0;
			virtual IFramebuffer* createFramebuffer(uint32_t numberOfColorFramebufferAttachments, const FramebufferAttachment* colorFramebufferAttachments, const FramebufferAttachment* depthStencilFramebufferAttachment = nullptr) = 0;
			virtual IBufferManager *createBufferManager() = 0;
			virtual ITextureManager *createTextureManager() = 0;
			virtual IRootSignature* createRootSignature(const RootSignature& rootSignature) = 0;
			virtual IPipelineState* createPipelineState(const PipelineState& pipelineState) = 0;
			virtual ISamplerState* createSamplerState(const SamplerState& samplerState) = 0;
			virtual bool map(IResource& resource, uint32_t subresource, MapType mapType, uint32_t mapFlags, MappedSubresource& mappedSubresource) = 0;
			virtual void unmap(IResource& resource, uint32_t subresource) = 0;
			virtual bool beginScene() = 0;
			virtual void submitCommandBuffer(const CommandBuffer& commandBuffer) = 0;
			virtual void endScene() = 0;
			virtual void flush() = 0;
			virtual void finish() = 0;
		protected:
			explicit IRenderer(const Context& context) :
				mContext(context)
			{}
			explicit IRenderer(const IRenderer& source) = delete;
			IRenderer& operator =(const IRenderer& source) = delete;
		protected:
			const Context& mContext;
			Capabilities   mCapabilities;
		#ifndef RENDERER_NO_STATISTICS
			private:
				Statistics mStatistics;
		#endif
		};
		typedef SmartRefCount<IRenderer> IRendererPtr;
	#endif

	// Renderer/Shader/IShaderLanguage.h
	#ifndef __RENDERER_ISHADERLANGUAGE_H__
	#define __RENDERER_ISHADERLANGUAGE_H__
		class IShaderLanguage : public RefCount<IShaderLanguage>
		{
		public:
			enum class OptimizationLevel
			{
				Debug = 0,
				None,
				Low,
				Medium,
				High,
				Ultra
			};
		public:
			virtual ~IShaderLanguage();
			inline IRenderer& getRenderer() const
			{
				return *mRenderer;
			}
			inline OptimizationLevel getOptimizationLevel() const
			{
				return mOptimizationLevel;
			}
			inline void setOptimizationLevel(OptimizationLevel optimizationLevel)
			{
				mOptimizationLevel = optimizationLevel;
			}
			inline IProgram* createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, IFragmentShader* fragmentShader)
			{
				return createProgram(rootSignature, vertexAttributes, vertexShader, nullptr, nullptr, nullptr, fragmentShader);
			}
			inline IProgram* createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, IGeometryShader* geometryShader, IFragmentShader* fragmentShader)
			{
				return createProgram(rootSignature, vertexAttributes, vertexShader, nullptr, nullptr, geometryShader, fragmentShader);
			}
			inline IProgram* createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, ITessellationControlShader* tessellationControlShader, ITessellationEvaluationShader* tessellationEvaluationShader, IFragmentShader* fragmentShader)
			{
				return createProgram(rootSignature, vertexAttributes, vertexShader, tessellationControlShader, tessellationEvaluationShader, nullptr, fragmentShader);
			}
		public:
			virtual const char* getShaderLanguageName() const = 0;
			virtual IVertexShader* createVertexShaderFromBytecode(const VertexAttributes& vertexAttributes, const ShaderBytecode& shaderBytecode) = 0;
			virtual IVertexShader* createVertexShaderFromSourceCode(const VertexAttributes& vertexAttributes, const ShaderSourceCode& shaderSourceCode, ShaderBytecode* shaderBytecode = nullptr) = 0;
			virtual ITessellationControlShader* createTessellationControlShaderFromBytecode(const ShaderBytecode& shaderBytecode) = 0;
			virtual ITessellationControlShader* createTessellationControlShaderFromSourceCode(const ShaderSourceCode& shaderSourceCode, ShaderBytecode* shaderBytecode = nullptr) = 0;
			virtual ITessellationEvaluationShader* createTessellationEvaluationShaderFromBytecode(const ShaderBytecode& shaderBytecode) = 0;
			virtual ITessellationEvaluationShader* createTessellationEvaluationShaderFromSourceCode(const ShaderSourceCode& shaderSourceCode, ShaderBytecode* shaderBytecode = nullptr) = 0;
			virtual IGeometryShader* createGeometryShaderFromBytecode(const ShaderBytecode& shaderBytecode, GsInputPrimitiveTopology gsInputPrimitiveTopology, GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices) = 0;
			virtual IGeometryShader* createGeometryShaderFromSourceCode(const ShaderSourceCode& shaderSourceCode, GsInputPrimitiveTopology gsInputPrimitiveTopology, GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, ShaderBytecode* shaderBytecode = nullptr) = 0;
			virtual IFragmentShader* createFragmentShaderFromBytecode(const ShaderBytecode& shaderBytecode) = 0;
			virtual IFragmentShader* createFragmentShaderFromSourceCode(const ShaderSourceCode& shaderSourceCode, ShaderBytecode* shaderBytecode = nullptr) = 0;
			virtual IProgram* createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, ITessellationControlShader* tessellationControlShader, ITessellationEvaluationShader* tessellationEvaluationShader, IGeometryShader* geometryShader, IFragmentShader* fragmentShader) = 0;
		protected:
			explicit IShaderLanguage(IRenderer& renderer);
			explicit IShaderLanguage(const IShaderLanguage& source) = delete;
			IShaderLanguage& operator =(const IShaderLanguage& source) = delete;
		private:
			IRenderer*		  mRenderer;
			OptimizationLevel mOptimizationLevel;
		};
		typedef SmartRefCount<IShaderLanguage> IShaderLanguagePtr;
	#endif

	// Renderer/IResource.h
	#ifndef __RENDERER_IRESOURCE_H__
	#define __RENDERER_IRESOURCE_H__
		class IResource : public RefCount<IResource>
		{
		public:
			inline virtual ~IResource()
			{}
			inline ResourceType getResourceType() const
			{
				return mResourceType;
			}
			inline IRenderer& getRenderer() const
			{
				return *mRenderer;
			}
		public:
			virtual void setDebugName(const char*)
			{}
			virtual void* getInternalResourceHandle() const
			{
				return nullptr;
			}
		protected:
			inline explicit IResource(ResourceType resourceType) :
				mResourceType(resourceType),
				mRenderer(nullptr)
			{}
			inline IResource(ResourceType resourceType, IRenderer& renderer) :
				mResourceType(resourceType),
				mRenderer(&renderer)
			{}
			explicit IResource(const IResource& source) = delete;
			IResource& operator =(const IResource& source) = delete;
		private:
			ResourceType  mResourceType;
			IRenderer*	  mRenderer;
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
		public:
			virtual IResourceGroup* createResourceGroup(uint32_t rootParameterIndex, uint32_t numberOfResources, IResource** resources) = 0;
		protected:
			explicit IRootSignature(IRenderer& renderer);
			explicit IRootSignature(const IRootSignature& source) = delete;
			IRootSignature& operator =(const IRootSignature& source) = delete;
		};
		typedef SmartRefCount<IRootSignature> IRootSignaturePtr;
	#endif

	// Renderer/ResourceGroup.h
	#ifndef __RENDERER_IRESOURCEGROUP_H__
	#define __RENDERER_IRESOURCEGROUP_H__
		class IResourceGroup : public IResource
		{
		public:
			virtual ~IResourceGroup();
		protected:
			explicit IResourceGroup(IRenderer& renderer);
			explicit IResourceGroup(const IResourceGroup& source) = delete;
			IResourceGroup& operator =(const IResourceGroup& source) = delete;
		};
		typedef SmartRefCount<IResourceGroup> IResourceGroupPtr;
	#endif

	// Renderer/Shader/IProgram.h
	#ifndef __RENDERER_IPROGRAM_H__
	#define __RENDERER_IPROGRAM_H__
		class IProgram : public IResource
		{
		public:
			virtual ~IProgram();
		public:
			virtual handle getUniformHandle(const char* uniformName) = 0;
			virtual void setUniform1i(handle uniformHandle, int value) = 0;
			virtual void setUniform1f(handle uniformHandle, float value) = 0;
			virtual void setUniform2fv(handle uniformHandle, const float* value) = 0;
			virtual void setUniform3fv(handle uUniformHandle, const float* value) = 0;
			virtual void setUniform4fv(handle uniformHandle, const float* value) = 0;
			virtual void setUniformMatrix3fv(handle uniformHandle, const float* value) = 0;
			virtual void setUniformMatrix4fv(handle uniformHandle, const float* value) = 0;
		protected:
			explicit IProgram(IRenderer& renderer);
			explicit IProgram(const IProgram& source) = delete;
			IProgram& operator =(const IProgram& source) = delete;
		};
		typedef SmartRefCount<IProgram> IProgramPtr;
	#endif

	// Renderer/RenderTarget/IRenderTarget.h
	#ifndef __RENDERER_IRENDERTARGET_H__
	#define __RENDERER_IRENDERTARGET_H__
		class IRenderTarget : public IResource
		{
		public:
			virtual ~IRenderTarget();
		public:
			virtual void getWidthAndHeight(uint32_t& width, uint32_t& height) const = 0;
		protected:
			IRenderTarget(ResourceType resourceType, IRenderer& renderer);
			explicit IRenderTarget(const IRenderTarget& source) = delete;
			IRenderTarget& operator =(const IRenderTarget& source) = delete;
		};
		typedef SmartRefCount<IRenderTarget> IRenderTargetPtr;
	#endif

	// Renderer/RenderTarget/ISwapChain.h
	#ifndef __RENDERER_ISWAPCHAIN_H__
	#define __RENDERER_ISWAPCHAIN_H__
		class IRenderWindow
		{
		public:
			inline virtual ~IRenderWindow()
			{}
		public:
			virtual void getWidthAndHeight(uint32_t& width, uint32_t& height) const = 0;
			virtual void present() = 0;
		protected:
			inline IRenderWindow()
			{}
			explicit IRenderWindow(const IRenderWindow& source) = delete;
			IRenderWindow& operator =(const IRenderWindow& source) = delete;
		};

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
			virtual void setRenderWindow(IRenderWindow* renderWindow) = 0;
		protected:
			explicit ISwapChain(IRenderer& renderer);
			explicit ISwapChain(const ISwapChain& source) = delete;
			ISwapChain& operator =(const ISwapChain& source) = delete;
		};
		typedef SmartRefCount<ISwapChain> ISwapChainPtr;
	#endif

	// Renderer/RenderTarget/IFramebuffer.h
	#ifndef __RENDERER_IFRAMEBUFFER_H__
	#define __RENDERER_IFRAMEBUFFER_H__
		struct FramebufferAttachment
		{
			ITexture* texture;
			uint32_t  mipmapIndex;
			uint32_t  layerIndex;
			inline FramebufferAttachment() :
				texture(nullptr),
				mipmapIndex(0),
				layerIndex(0)
			{}
			inline FramebufferAttachment(ITexture* _texture, uint32_t _mipmapIndex = 0, uint32_t _layerIndex = 0) :
				texture(_texture),
				mipmapIndex(_mipmapIndex),
				layerIndex(_layerIndex)
			{}
		};
		class IFramebuffer : public IRenderTarget
		{
		public:
			virtual ~IFramebuffer();
		protected:
			explicit IFramebuffer(IRenderer& renderer);
			explicit IFramebuffer(const IFramebuffer& source) = delete;
			IFramebuffer& operator =(const IFramebuffer& source) = delete;
		};
		typedef SmartRefCount<IFramebuffer> IFramebufferPtr;
	#endif

	// Renderer/Buffer/IBufferManager.h
	#ifndef __RENDERER_IBUFFERMANAGER_H__
	#define __RENDERER_IBUFFERMANAGER_H__
		class IBufferManager : public RefCount<IBufferManager>
		{
		public:
			inline virtual ~IBufferManager();
			inline IRenderer& getRenderer() const
			{
				return mRenderer;
			}
		public:
			virtual IVertexBuffer* createVertexBuffer(uint32_t numberOfBytes, const void* data = nullptr, BufferUsage bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;
			virtual IIndexBuffer* createIndexBuffer(uint32_t numberOfBytes, IndexBufferFormat::Enum indexBufferFormat, const void* data = nullptr, BufferUsage bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;
			virtual IVertexArray* createVertexArray(const VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const VertexArrayVertexBuffer* vertexBuffers, IIndexBuffer* indexBuffer = nullptr) = 0;
			virtual IUniformBuffer* createUniformBuffer(uint32_t numberOfBytes, const void* data = nullptr, BufferUsage bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;
			virtual ITextureBuffer* createTextureBuffer(uint32_t numberOfBytes, TextureFormat::Enum textureFormat, const void* data = nullptr, BufferUsage bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;
			virtual IIndirectBuffer* createIndirectBuffer(uint32_t numberOfBytes, const void* data = nullptr, BufferUsage bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;
		protected:
			inline explicit IBufferManager(IRenderer& renderer);
			explicit IBufferManager(const IBufferManager& source) = delete;
			IBufferManager& operator =(const IBufferManager& source) = delete;
		private:
			IRenderer& mRenderer;
		};
		typedef SmartRefCount<IBufferManager> IBufferManagerPtr;
	#endif

	// Renderer/Buffer/IVertexArray.h
	#ifndef __RENDERER_IVERTEXARRAY_H__
	#define __RENDERER_IVERTEXARRAY_H__
		class IVertexArray : public IResource
		{
		public:
			virtual ~IVertexArray();
		protected:
			explicit IVertexArray(IRenderer& renderer);
			explicit IVertexArray(const IVertexArray& source) = delete;
			IVertexArray& operator =(const IVertexArray& source) = delete;
		};
		typedef SmartRefCount<IVertexArray> IVertexArrayPtr;
	#endif

	// Renderer/Buffer/IBuffer.h
	#ifndef __RENDERER_IBUFFER_H__
	#define __RENDERER_IBUFFER_H__
		class IBuffer : public IResource
		{
		public:
			inline virtual ~IBuffer()
			{}
		protected:
			inline explicit IBuffer(ResourceType resourceType) :
				IResource(resourceType)
			{}
			inline IBuffer(ResourceType resourceType, IRenderer& renderer) :
				IResource(resourceType, renderer)
			{}
			explicit IBuffer(const IBuffer& source) = delete;
			IBuffer& operator =(const IBuffer& source) = delete;
		};
		typedef SmartRefCount<IBuffer> IBufferPtr;
	#endif

	// Renderer/Buffer/IIndexBuffer.h
	#ifndef __RENDERER_IINDEXBUFFER_H__
	#define __RENDERER_IINDEXBUFFER_H__
		class IIndexBuffer : public IBuffer
		{
		public:
			virtual ~IIndexBuffer();
		protected:
			explicit IIndexBuffer(IRenderer& renderer);
			explicit IIndexBuffer(const IIndexBuffer& source) = delete;
			IIndexBuffer& operator =(const IIndexBuffer& source) = delete;
		};
		typedef SmartRefCount<IIndexBuffer> IIndexBufferPtr;
	#endif

	// Renderer/Buffer/IVertexBuffer.h
	#ifndef __RENDERER_IVERTEXBUFFER_H__
	#define __RENDERER_IVERTEXBUFFER_H__
		class IVertexBuffer : public IBuffer
		{
		public:
			virtual ~IVertexBuffer();
		protected:
			explicit IVertexBuffer(IRenderer& renderer);
			explicit IVertexBuffer(const IVertexBuffer& source) = delete;
			IVertexBuffer& operator =(const IVertexBuffer& source) = delete;
		};
		typedef SmartRefCount<IVertexBuffer> IVertexBufferPtr;
	#endif

	// Renderer/Buffer/IUniformBuffer.h
	#ifndef __RENDERER_IUNIFORMBUFFER_H__
	#define __RENDERER_IUNIFORMBUFFER_H__
		class IUniformBuffer : public IBuffer
		{
		public:
			virtual ~IUniformBuffer();
		public:
			virtual void copyDataFrom(uint32_t numberOfBytes, const void* data) = 0;
		protected:
			explicit IUniformBuffer(IRenderer& renderer);
			explicit IUniformBuffer(const IUniformBuffer& source) = delete;
			IUniformBuffer& operator =(const IUniformBuffer& source) = delete;
		};
		typedef SmartRefCount<IUniformBuffer> IUniformBufferPtr;
	#endif

	// Renderer/Buffer/ITextureBuffer.h
	#ifndef __RENDERER_ITEXTUREBUFFER_H__
	#define __RENDERER_ITEXTUREBUFFER_H__
		class ITextureBuffer : public IBuffer
		{
		public:
			virtual ~ITextureBuffer();
		public:
			virtual void copyDataFrom(uint32_t numberOfBytes, const void* data) = 0;
		protected:
			explicit ITextureBuffer(IRenderer& renderer);
			explicit ITextureBuffer(const ITextureBuffer& source) = delete;
			ITextureBuffer& operator =(const ITextureBuffer& source) = delete;
		};
		typedef SmartRefCount<ITextureBuffer> ITextureBufferPtr;
	#endif

	// Renderer/Buffer/IIndirectBuffer.h
	#ifndef __RENDERER_IINDIRECTBUFFER_H__
	#define __RENDERER_IINDIRECTBUFFER_H__
		class IIndirectBuffer : public IBuffer
		{
		public:
			inline virtual ~IIndirectBuffer()
			{}
		public:
			virtual const uint8_t* getEmulationData() const = 0;
			virtual void copyDataFrom(uint32_t numberOfBytes, const void* data) = 0;
		protected:
			inline IIndirectBuffer() :
				IBuffer(ResourceType::INDIRECT_BUFFER)
			{}
			inline explicit IIndirectBuffer(IRenderer& renderer) :
				IBuffer(ResourceType::INDIRECT_BUFFER, renderer)
			{}
			explicit IIndirectBuffer(const IIndirectBuffer& source) = delete;
			IIndirectBuffer& operator =(const IIndirectBuffer& source) = delete;
		};
		typedef SmartRefCount<IIndirectBuffer> IIndirectBufferPtr;
	#endif

	// Renderer/Texture/ITextureManager.h
	#ifndef __RENDERER_ITEXTUREMANAGER_H__
	#define __RENDERER_ITEXTUREMANAGER_H__
		class ITextureManager : public RefCount<ITextureManager>
		{
		public:
			inline virtual ~ITextureManager();
			inline IRenderer& getRenderer() const
			{
				return mRenderer;
			}
		public:
			virtual ITexture1D* createTexture1D(uint32_t width, TextureFormat::Enum textureFormat, const void* data = nullptr, uint32_t flags = 0, TextureUsage textureUsage = TextureUsage::DEFAULT) = 0;
			virtual ITexture2D* createTexture2D(uint32_t width, uint32_t height, TextureFormat::Enum textureFormat, const void* data = nullptr, uint32_t flags = 0, TextureUsage textureUsage = TextureUsage::DEFAULT, uint8_t numberOfMultisamples = 1, const OptimizedTextureClearValue* optimizedTextureClearValue = nullptr) = 0;
			virtual ITexture2DArray* createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, TextureFormat::Enum textureFormat, const void* data = nullptr, uint32_t flags = 0, TextureUsage textureUsage = TextureUsage::DEFAULT) = 0;
			virtual ITexture3D* createTexture3D(uint32_t width, uint32_t height, uint32_t depth, TextureFormat::Enum textureFormat, const void* data = nullptr, uint32_t flags = 0, TextureUsage textureUsage = TextureUsage::DEFAULT) = 0;
			virtual ITextureCube* createTextureCube(uint32_t width, uint32_t height, TextureFormat::Enum textureFormat, const void* data = nullptr, uint32_t flags = 0, TextureUsage textureUsage = TextureUsage::DEFAULT) = 0;
		protected:
			inline explicit ITextureManager(IRenderer& renderer);
			explicit ITextureManager(const ITextureManager& source) = delete;
			ITextureManager& operator =(const ITextureManager& source) = delete;
		private:
			IRenderer& mRenderer;
		};
		typedef SmartRefCount<ITextureManager> ITextureManagerPtr;
	#endif

	// Renderer/Texture/ITexture.h
	#ifndef __RENDERER_ITEXTURE_H__
	#define __RENDERER_ITEXTURE_H__
		class ITexture : public IResource
		{
		public:
			virtual ~ITexture();
		protected:
			ITexture(ResourceType resourceType, IRenderer& renderer);
			explicit ITexture(const ITexture& source) = delete;
			ITexture& operator =(const ITexture& source) = delete;
		};
		typedef SmartRefCount<ITexture> ITexturePtr;
	#endif

	// Renderer/Texture/ITexture1D.h
	#ifndef __RENDERER_ITEXTURE1D_H__
	#define __RENDERER_ITEXTURE1D_H__
		class ITexture1D : public ITexture
		{
		public:
			virtual ~ITexture1D();
			inline uint32_t getWidth() const
			{
				return mWidth;
			}
		protected:
			ITexture1D(IRenderer& renderer, uint32_t width);
			explicit ITexture1D(const ITexture1D& source) = delete;
			ITexture1D& operator =(const ITexture1D& source) = delete;
		private:
			uint32_t mWidth;
		};
		typedef SmartRefCount<ITexture1D> ITexture1DPtr;
	#endif

	// Renderer/Texture/ITexture2D.h
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
			ITexture2D(IRenderer& renderer, uint32_t width, uint32_t height);
			explicit ITexture2D(const ITexture2D& source) = delete;
			ITexture2D& operator =(const ITexture2D& source) = delete;
		private:
			uint32_t mWidth;
			uint32_t mHeight;
		};
		typedef SmartRefCount<ITexture2D> ITexture2DPtr;
	#endif

	// Renderer/Texture/ITexture2DArray.h
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
			ITexture2DArray(IRenderer& renderer, uint32_t width, uint32_t height, uint32_t numberOfSlices);
			explicit ITexture2DArray(const ITexture2DArray& source) = delete;
			ITexture2DArray& operator =(const ITexture2DArray& source) = delete;
		private:
			uint32_t mWidth;
			uint32_t mHeight;
			uint32_t mNumberOfSlices;
		};
		typedef SmartRefCount<ITexture2DArray> ITexture2DArrayPtr;
	#endif

	// Renderer/Texture/ITexture3D.h
	#ifndef __RENDERER_ITEXTURE3D_H__
	#define __RENDERER_ITEXTURE3D_H__
		class ITexture3D : public ITexture
		{
		public:
			virtual ~ITexture3D();
			inline uint32_t getWidth() const
			{
				return mWidth;
			}
			inline uint32_t getHeight() const
			{
				return mHeight;
			}
			inline uint32_t getDepth() const
			{
				return mDepth;
			}
		public:
			virtual void copyDataFrom(uint32_t numberOfBytes, const void* data) = 0;
		protected:
			ITexture3D(IRenderer& renderer, uint32_t width, uint32_t height, uint32_t depth);
			explicit ITexture3D(const ITexture3D& source) = delete;
			ITexture3D& operator =(const ITexture3D& source) = delete;
		private:
			uint32_t mWidth;
			uint32_t mHeight;
			uint32_t mDepth;
		};
		typedef SmartRefCount<ITexture3D> ITexture3DPtr;
	#endif

	// Renderer/Texture/ITextureCube.h
	#ifndef __RENDERER_ITEXTURECUBE_H__
	#define __RENDERER_ITEXTURECUBE_H__
		class ITextureCube : public ITexture
		{
		public:
			virtual ~ITextureCube();
			inline uint32_t getWidth() const
			{
				return mWidth;
			}
			inline uint32_t getHeight() const
			{
				return mHeight;
			}
		protected:
			ITextureCube(IRenderer& renderer, uint32_t width, uint32_t height);
			explicit ITextureCube(const ITextureCube& source) = delete;
			ITextureCube& operator =(const ITextureCube& source) = delete;
		private:
			uint32_t mWidth;
			uint32_t mHeight;
		};
		typedef SmartRefCount<ITextureCube> ITextureCubePtr;
	#endif

	// Renderer/State/IState.h
	#ifndef __RENDERER_ISTATE_H__
	#define __RENDERER_ISTATE_H__
		class IState : public IResource
		{
		public:
			virtual ~IState();
		protected:
			IState(ResourceType resourceType, IRenderer& renderer);
			explicit IState(const IState& source) = delete;
			IState& operator =(const IState& source) = delete;
		};
		typedef SmartRefCount<IState> IStatePtr;
	#endif

	// Renderer/State/IPipelineState.h
	#ifndef __RENDERER_IPIPELINESTATE_H__
	#define __RENDERER_IPIPELINESTATE_H__
		class IPipelineState : public IState
		{
		public:
			virtual ~IPipelineState();
		protected:
			explicit IPipelineState(IRenderer& renderer);
			explicit IPipelineState(const IPipelineState& source) = delete;
			IPipelineState& operator =(const IPipelineState& source) = delete;
		};
		typedef SmartRefCount<IPipelineState> IPipelineStatePtr;
	#endif

	// Renderer/State/ISamplerState.h
	#ifndef __RENDERER_ISAMPLERSTATE_H__
	#define __RENDERER_ISAMPLERSTATE_H__
		class ISamplerState : public IState
		{
		public:
			static inline const SamplerState& getDefaultSamplerState()
			{
				static const SamplerState SAMPLER_STATE =
				{
					FilterMode::MIN_MAG_MIP_LINEAR,
					TextureAddressMode::CLAMP,
					TextureAddressMode::CLAMP,
					TextureAddressMode::CLAMP,
					0.0f,
					16,
					ComparisonFunc::NEVER,
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
			explicit ISamplerState(IRenderer& renderer);
			explicit ISamplerState(const ISamplerState& source) = delete;
			ISamplerState& operator =(const ISamplerState& source) = delete;
		};
		typedef SmartRefCount<ISamplerState> ISamplerStatePtr;
	#endif

	// Renderer/Shader/IShader.h
	#ifndef __RENDERER_ISHADER_H__
	#define __RENDERER_ISHADER_H__
		class IShader : public IResource
		{
		public:
			virtual ~IShader();
		public:
			virtual const char* getShaderLanguageName() const = 0;
		protected:
			IShader(ResourceType resourceType, IRenderer& renderer);
			explicit IShader(const IShader& source) = delete;
			IShader& operator =(const IShader& source) = delete;
		};
		typedef SmartRefCount<IShader> IShaderPtr;
	#endif

	// Renderer/Shader/IVertexShader.h
	#ifndef __RENDERER_IVERTEXSHADER_H__
	#define __RENDERER_IVERTEXSHADER_H__
		class IVertexShader : public IShader
		{
		public:
			virtual ~IVertexShader();
		protected:
			explicit IVertexShader(IRenderer& renderer);
			explicit IVertexShader(const IVertexShader& source) = delete;
			IVertexShader& operator =(const IVertexShader& source) = delete;
		};
		typedef SmartRefCount<IVertexShader> IVertexShaderPtr;
	#endif

	// Renderer/Shader/ITessellationControlShader.h
	#ifndef __RENDERER_ITESSELLATIONCONTROLSHADER_H__
	#define __RENDERER_ITESSELLATIONCONTROLSHADER_H__
		class ITessellationControlShader : public IShader
		{
		public:
			virtual ~ITessellationControlShader();
		protected:
			explicit ITessellationControlShader(IRenderer& renderer);
			explicit ITessellationControlShader(const ITessellationControlShader& source) = delete;
			ITessellationControlShader& operator =(const ITessellationControlShader& source) = delete;
		};
		typedef SmartRefCount<ITessellationControlShader> ITessellationControlShaderPtr;
	#endif

	// Renderer/Shader/ITessellationEvaluationShader.h
	#ifndef __RENDERER_ITESSELATIONEVALUATIONSHADER_H__
	#define __RENDERER_ITESSELATIONEVALUATIONSHADER_H__
		class ITessellationEvaluationShader : public IShader
		{
		public:
			virtual ~ITessellationEvaluationShader();
		protected:
			explicit ITessellationEvaluationShader(IRenderer& renderer);
			explicit ITessellationEvaluationShader(const ITessellationEvaluationShader& source) = delete;
			ITessellationEvaluationShader& operator =(const ITessellationEvaluationShader& source) = delete;
		};
		typedef SmartRefCount<ITessellationEvaluationShader> ITessellationEvaluationShaderPtr;
	#endif

	// Renderer/Shader/IGeometryShader.h
	#ifndef __RENDERER_IGEOMETRYSHADER_H__
	#define __RENDERER_IGEOMETRYSHADER_H__
		class IGeometryShader : public IShader
		{
		public:
			virtual ~IGeometryShader();
		protected:
			explicit IGeometryShader(IRenderer& renderer);
			explicit IGeometryShader(const IGeometryShader& source) = delete;
			IGeometryShader& operator =(const IGeometryShader& source) = delete;
		};
		typedef SmartRefCount<IGeometryShader> IGeometryShaderPtr;
	#endif

	// Renderer/Shader/IFragmentShader.h
	#ifndef __RENDERER_IFRAGMENTSHADER_H__
	#define __RENDERER_IFRAGMENTSHADER_H__
		class IFragmentShader : public IShader
		{
		public:
			virtual ~IFragmentShader();
		protected:
			explicit IFragmentShader(IRenderer& renderer);
			explicit IFragmentShader(const IFragmentShader& source) = delete;
			IFragmentShader& operator =(const IFragmentShader& source) = delete;
		};
		typedef SmartRefCount<IFragmentShader> IFragmentShaderPtr;
	#endif

	// Renderer/Buffer/CommandBuffer.h
	#ifndef __RENDERER_COMMANDBUFFER_H__
	#define __RENDERER_COMMANDBUFFER_H__
		enum CommandDispatchFunctionIndex : uint8_t
		{
			ExecuteCommandBuffer = 0,
			CopyUniformBufferData,
			CopyTextureBufferData,
			SetGraphicsRootSignature,
			SetGraphicsResourceGroup,
			SetPipelineState,
			SetVertexArray,
			SetViewports,
			SetScissorRectangles,
			SetRenderTarget,
			Clear,
			ResolveMultisampleFramebuffer,
			CopyResource,
			Draw,
			DrawIndexed,
			SetDebugMarker,
			BeginDebugEvent,
			EndDebugEvent,
			NumberOfFunctions
		};
		typedef void (*BackendDispatchFunction)(const void*, IRenderer& renderer);
		typedef void* CommandPacket;
		namespace CommandPacketHelper
		{
			static const uint32_t OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX	= 0u;
			static const uint32_t OFFSET_BACKEND_DISPATCH_FUNCTION		= OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX + sizeof(uint32_t);
			static const uint32_t OFFSET_COMMAND						= OFFSET_BACKEND_DISPATCH_FUNCTION + sizeof(uint32_t);
			template <typename T>
			uint32_t getNumberOfBytes(uint32_t numberOfAuxiliaryBytes)
			{
				return OFFSET_COMMAND + sizeof(T) + numberOfAuxiliaryBytes;
			}
			inline uint32_t getNextCommandPacketByteIndex(const CommandPacket commandPacket)
			{
				return *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(commandPacket) + OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX);
			}
			inline void storeNextCommandPacketByteIndex(const CommandPacket commandPacket, uint32_t nextPacketByteIndex)
			{
				*reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(commandPacket) + OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX) = nextPacketByteIndex;
			}
			inline CommandDispatchFunctionIndex* getCommandDispatchFunctionIndex(const CommandPacket commandPacket)
			{
				return reinterpret_cast<CommandDispatchFunctionIndex*>(reinterpret_cast<uint8_t*>(commandPacket) + OFFSET_BACKEND_DISPATCH_FUNCTION);
			}
			inline void storeBackendDispatchFunctionIndex(const CommandPacket commandPacket, CommandDispatchFunctionIndex commandDispatchFunctionIndex)
			{
				*getCommandDispatchFunctionIndex(commandPacket) = commandDispatchFunctionIndex;
			}
			inline CommandDispatchFunctionIndex loadCommandDispatchFunctionIndex(const CommandPacket commandPacket)
			{
				return *getCommandDispatchFunctionIndex(commandPacket);
			}
			template <typename T>
			T* getCommand(const CommandPacket commandPacket)
			{
				return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(commandPacket) + OFFSET_COMMAND);
			}
			inline const void* loadCommand(const CommandPacket commandPacket)
			{
				return reinterpret_cast<uint8_t*>(commandPacket) + OFFSET_COMMAND;
			}
			template <typename T>
			uint8_t* getAuxiliaryMemory(T* command)
			{
				return reinterpret_cast<uint8_t*>(command) + sizeof(T);
			}
			template <typename T>
			const uint8_t* getAuxiliaryMemory(const T* command)
			{
				return reinterpret_cast<const uint8_t*>(command) + sizeof(T);
			}
		};
		class CommandBuffer
		{
		public:
			inline CommandBuffer() :
				mCommandPacketBufferNumberOfBytes(0),
				mCommandPacketBuffer(nullptr),
				mPreviousCommandPacketByteIndex(~0u),
				mCurrentCommandPacketByteIndex(0)
				#ifndef RENDERER_NO_STATISTICS
					, mNumberOfCommands(0)
				#endif
			{}
			inline ~CommandBuffer()
			{
				delete [] mCommandPacketBuffer;
			}
			inline bool isEmpty() const
			{
				return (~0u == mPreviousCommandPacketByteIndex);
			}
			#ifndef RENDERER_NO_STATISTICS
				inline uint32_t getNumberOfCommands() const
				{
					return mNumberOfCommands;
				}
			#endif
			inline void clear()
			{
				mPreviousCommandPacketByteIndex = ~0u;
				mCurrentCommandPacketByteIndex = 0;
				#ifndef RENDERER_NO_STATISTICS
					mNumberOfCommands = 0;
				#endif
			}
			template <typename U>
			U* addCommand(uint32_t numberOfAuxiliaryBytes = 0)
			{
				const uint32_t numberOfCommandBytes = CommandPacketHelper::getNumberOfBytes<U>(numberOfAuxiliaryBytes);
				#ifndef RENDERER_NO_DEBUG
					assert((static_cast<uint64_t>(mCurrentCommandPacketByteIndex) + numberOfCommandBytes) < 4294967295u);
				#endif
				if (mCommandPacketBufferNumberOfBytes < mCurrentCommandPacketByteIndex + numberOfCommandBytes)
				{
					const uint32_t newCommandPacketBufferNumberOfBytes = mCommandPacketBufferNumberOfBytes + NUMBER_OF_BYTES_TO_GROW + numberOfCommandBytes;
					uint8_t* newCommandPacketBuffer = new uint8_t[newCommandPacketBufferNumberOfBytes];
					if (nullptr != mCommandPacketBuffer)
					{
						memcpy(newCommandPacketBuffer, mCommandPacketBuffer, mCommandPacketBufferNumberOfBytes);
						delete [] mCommandPacketBuffer;
					}
					mCommandPacketBuffer = newCommandPacketBuffer;
					mCommandPacketBufferNumberOfBytes = newCommandPacketBufferNumberOfBytes;
				}
				CommandPacket commandPacket = &mCommandPacketBuffer[mCurrentCommandPacketByteIndex];
				if (~0u != mPreviousCommandPacketByteIndex)
				{
					CommandPacketHelper::storeNextCommandPacketByteIndex(&mCommandPacketBuffer[mPreviousCommandPacketByteIndex], mCurrentCommandPacketByteIndex);
				}
				CommandPacketHelper::storeNextCommandPacketByteIndex(commandPacket, ~0u);
				CommandPacketHelper::storeBackendDispatchFunctionIndex(commandPacket, U::COMMAND_DISPATCH_FUNCTION_INDEX);
				mPreviousCommandPacketByteIndex = mCurrentCommandPacketByteIndex;
				mCurrentCommandPacketByteIndex += numberOfCommandBytes;
				#ifndef RENDERER_NO_STATISTICS
					++mNumberOfCommands;
				#endif
				return CommandPacketHelper::getCommand<U>(commandPacket);
			}
			inline void submit(IRenderer& renderer) const
			{
				renderer.submitCommandBuffer(*this);
			}
			inline void submitAndClear(IRenderer& renderer)
			{
				renderer.submitCommandBuffer(*this);
				clear();
			}
		private:
			static const uint32_t NUMBER_OF_BYTES_TO_GROW = 8192;
		private:
			uint32_t mCommandPacketBufferNumberOfBytes;
			uint8_t* mCommandPacketBuffer;
			uint32_t mPreviousCommandPacketByteIndex;
			uint32_t mCurrentCommandPacketByteIndex;
			#ifndef RENDERER_NO_STATISTICS
				uint32_t mNumberOfCommands;
			#endif
		};
		namespace Command
		{
			struct ExecuteCommandBuffer
			{
				inline static void create(CommandBuffer& commandBuffer, CommandBuffer* commandBufferToExecute)
				{
					#ifndef RENDERER_NO_DEBUG
						assert(nullptr != commandBufferToExecute);
					#endif
					*commandBuffer.addCommand<ExecuteCommandBuffer>() = ExecuteCommandBuffer(commandBufferToExecute);
				}
				inline ExecuteCommandBuffer(CommandBuffer* _commandBufferToExecute) :
					commandBufferToExecute(_commandBufferToExecute)
				{}
				CommandBuffer* commandBufferToExecute;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::ExecuteCommandBuffer;
			};
			struct CopyUniformBufferData
			{
				inline static void create(CommandBuffer& commandBuffer, IUniformBuffer* uniformBuffer, uint32_t numberOfBytes, void* data)
				{
					Command::CopyUniformBufferData* copyUniformBufferDataCommand = commandBuffer.addCommand<Command::CopyUniformBufferData>(numberOfBytes);
					copyUniformBufferDataCommand->uniformBuffer	= uniformBuffer;
					copyUniformBufferDataCommand->numberOfBytes	= numberOfBytes;
					copyUniformBufferDataCommand->data			= nullptr;
					memcpy(CommandPacketHelper::getAuxiliaryMemory(copyUniformBufferDataCommand), data, numberOfBytes);
				}
				inline CopyUniformBufferData(IUniformBuffer* _uniformBuffer, uint32_t _numberOfBytes, void* _data) :
					uniformBuffer(_uniformBuffer),
					numberOfBytes(_numberOfBytes),
					data(_data)
				{}
				IUniformBuffer* uniformBuffer;
				uint32_t		numberOfBytes;
				void*			data;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::CopyUniformBufferData;
			};
			struct CopyTextureBufferData
			{
				inline static void create(CommandBuffer& commandBuffer, ITextureBuffer* textureBuffer, uint32_t numberOfBytes, void* data)
				{
					Command::CopyTextureBufferData* copyTextureBufferDataCommand = commandBuffer.addCommand<Command::CopyTextureBufferData>(numberOfBytes);
					copyTextureBufferDataCommand->textureBuffer	= textureBuffer;
					copyTextureBufferDataCommand->numberOfBytes	= numberOfBytes;
					copyTextureBufferDataCommand->data			= nullptr;
					memcpy(CommandPacketHelper::getAuxiliaryMemory(copyTextureBufferDataCommand), data, numberOfBytes);
				}
				inline CopyTextureBufferData(ITextureBuffer* _textureBuffer, uint32_t _numberOfBytes, void* _data) :
					textureBuffer(_textureBuffer),
					numberOfBytes(_numberOfBytes),
					data(_data)
				{}
				ITextureBuffer* textureBuffer;
				uint32_t		numberOfBytes;
				void*			data;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::CopyTextureBufferData;
			};
			struct SetGraphicsRootSignature
			{
				inline static void create(CommandBuffer& commandBuffer, IRootSignature* rootSignature)
				{
					*commandBuffer.addCommand<SetGraphicsRootSignature>() = SetGraphicsRootSignature(rootSignature);
				}
				inline SetGraphicsRootSignature(IRootSignature* _rootSignature) :
					rootSignature(_rootSignature)
				{}
				IRootSignature* rootSignature;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetGraphicsRootSignature;
			};
			struct SetGraphicsResourceGroup
			{
				inline static void create(CommandBuffer& commandBuffer, uint32_t rootParameterIndex, IResourceGroup* resourceGroup)
				{
					*commandBuffer.addCommand<SetGraphicsResourceGroup>() = SetGraphicsResourceGroup(rootParameterIndex, resourceGroup);
				}
				inline SetGraphicsResourceGroup(uint32_t _rootParameterIndex, IResourceGroup* _resourceGroup) :
					rootParameterIndex(_rootParameterIndex),
					resourceGroup(_resourceGroup)
				{}
				uint32_t		rootParameterIndex;
				IResourceGroup* resourceGroup;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetGraphicsResourceGroup;
			};
			struct SetPipelineState
			{
				inline static void create(CommandBuffer& commandBuffer, IPipelineState* pipelineState)
				{
					*commandBuffer.addCommand<SetPipelineState>() = SetPipelineState(pipelineState);
				}
				inline SetPipelineState(IPipelineState* _pipelineState) :
					pipelineState(_pipelineState)
				{}
				IPipelineState* pipelineState;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetPipelineState;
			};
			struct SetVertexArray
			{
				inline static void create(CommandBuffer& commandBuffer, IVertexArray* vertexArray)
				{
					*commandBuffer.addCommand<SetVertexArray>() = SetVertexArray(vertexArray);
				}
				inline SetVertexArray(IVertexArray* _vertexArray) :
					vertexArray(_vertexArray)
				{}
				IVertexArray* vertexArray;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetVertexArray;
			};
			struct SetViewports
			{
				inline static void create(CommandBuffer& commandBuffer, uint32_t numberOfViewports, const Viewport* viewports)
				{
					*commandBuffer.addCommand<SetViewports>() = SetViewports(numberOfViewports, viewports);
				}
				inline static void create(CommandBuffer& commandBuffer, uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height, float minimumDepth = 0.0f, float maximumDepth = 1.0f)
				{
					SetViewports* setViewportsCommand = commandBuffer.addCommand<SetViewports>(sizeof(Viewport));
					Viewport* viewport = reinterpret_cast<Viewport*>(CommandPacketHelper::getAuxiliaryMemory(setViewportsCommand));
					viewport->topLeftX = static_cast<float>(topLeftX);
					viewport->topLeftY = static_cast<float>(topLeftY);
					viewport->width	   = static_cast<float>(width);
					viewport->height   = static_cast<float>(height);
					viewport->minDepth = minimumDepth;
					viewport->maxDepth = maximumDepth;
					setViewportsCommand->numberOfViewports = 1;
					setViewportsCommand->viewports		   = nullptr;
				}
				inline SetViewports(uint32_t _numberOfViewports, const Viewport* _viewports) :
					numberOfViewports(_numberOfViewports),
					viewports(_viewports)
				{}
				uint32_t		numberOfViewports;
				const Viewport* viewports;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetViewports;
			};
			struct SetScissorRectangles
			{
				inline static void create(CommandBuffer& commandBuffer, uint32_t numberOfScissorRectangles, const ScissorRectangle* scissorRectangles)
				{
					*commandBuffer.addCommand<SetScissorRectangles>() = SetScissorRectangles(numberOfScissorRectangles, scissorRectangles);
				}
				inline static void create(CommandBuffer& commandBuffer, long topLeftX, long topLeftY, long bottomRightX, long bottomRightY)
				{
					SetScissorRectangles* setScissorRectanglesCommand = commandBuffer.addCommand<SetScissorRectangles>(sizeof(ScissorRectangle));
					ScissorRectangle* scissorRectangle = reinterpret_cast<ScissorRectangle*>(CommandPacketHelper::getAuxiliaryMemory(setScissorRectanglesCommand));
					scissorRectangle->topLeftX	   = topLeftX;
					scissorRectangle->topLeftY	   = topLeftY;
					scissorRectangle->bottomRightX = bottomRightX;
					scissorRectangle->bottomRightY = bottomRightY;
					setScissorRectanglesCommand->numberOfScissorRectangles = 1;
					setScissorRectanglesCommand->scissorRectangles		   = nullptr;
				}
				inline SetScissorRectangles(uint32_t _numberOfScissorRectangles, const ScissorRectangle* _scissorRectangles) :
					numberOfScissorRectangles(_numberOfScissorRectangles),
					scissorRectangles(_scissorRectangles)
				{}
				uint32_t				numberOfScissorRectangles;
				const ScissorRectangle* scissorRectangles;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetScissorRectangles;
			};
			struct SetViewportAndScissorRectangle
			{
				inline static void create(CommandBuffer& commandBuffer, uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height, float minimumDepth = 0.0f, float maximumDepth = 1.0f)
				{
					SetViewports::create(commandBuffer, topLeftX, topLeftY, width, height, minimumDepth, maximumDepth);
					SetScissorRectangles::create(commandBuffer, static_cast<long>(topLeftX), static_cast<long>(topLeftY), static_cast<long>(topLeftX + width), static_cast<long>(topLeftY + height));
				}
			};
			struct SetRenderTarget
			{
				inline static void create(CommandBuffer& commandBuffer, IRenderTarget* renderTarget)
				{
					*commandBuffer.addCommand<SetRenderTarget>() = SetRenderTarget(renderTarget);
				}
				inline SetRenderTarget(IRenderTarget* _renderTarget) :
					renderTarget(_renderTarget)
				{}
				IRenderTarget* renderTarget;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetRenderTarget;
			};
			struct Clear
			{
				inline static void create(CommandBuffer& commandBuffer, uint32_t flags, const float color[4], float z, uint32_t stencil)
				{
					*commandBuffer.addCommand<Clear>() = Clear(flags, color, z, stencil);
				}
				inline Clear(uint32_t _flags, const float _color[4], float _z, uint32_t _stencil) :
					flags(_flags),
					color{_color[0], _color[1], _color[2], _color[3]},
					z(_z),
					stencil(_stencil)
				{}
				uint32_t flags;
				float	 color[4];
				float	 z;
				uint32_t stencil;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::Clear;
			};
			struct ResolveMultisampleFramebuffer
			{
				inline static void create(CommandBuffer& commandBuffer, IRenderTarget& destinationRenderTarget, IFramebuffer& sourceMultisampleFramebuffer)
				{
					*commandBuffer.addCommand<ResolveMultisampleFramebuffer>() = ResolveMultisampleFramebuffer(destinationRenderTarget, sourceMultisampleFramebuffer);
				}
				inline ResolveMultisampleFramebuffer(IRenderTarget& _destinationRenderTarget, IFramebuffer& _sourceMultisampleFramebuffer) :
					destinationRenderTarget(&_destinationRenderTarget),
					sourceMultisampleFramebuffer(&_sourceMultisampleFramebuffer)
				{}
				IRenderTarget* destinationRenderTarget;
				IFramebuffer* sourceMultisampleFramebuffer;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::ResolveMultisampleFramebuffer;
			};
			struct CopyResource
			{
				inline static void create(CommandBuffer& commandBuffer, IResource& destinationResource, IResource& sourceResource)
				{
					*commandBuffer.addCommand<CopyResource>() = CopyResource(destinationResource, sourceResource);
				}
				inline CopyResource(IResource& _destinationResource, IResource& _sourceResource) :
					destinationResource(&_destinationResource),
					sourceResource(&_sourceResource)
				{}
				IResource* destinationResource;
				IResource* sourceResource;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::CopyResource;
			};
			struct Draw
			{
				inline static void create(CommandBuffer& commandBuffer, const IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1)
				{
					*commandBuffer.addCommand<Draw>() = Draw(indirectBuffer, indirectBufferOffset, numberOfDraws);
				}
				inline static void create(CommandBuffer& commandBuffer, uint32_t vertexCountPerInstance, uint32_t instanceCount = 1, uint32_t startVertexLocation = 0, uint32_t startInstanceLocation = 0)
				{
					Draw* drawCommand = commandBuffer.addCommand<Draw>(sizeof(DrawInstancedArguments));
					const DrawInstancedArguments drawInstancedArguments(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
					memcpy(CommandPacketHelper::getAuxiliaryMemory(drawCommand), &drawInstancedArguments, sizeof(DrawInstancedArguments));
					drawCommand->indirectBuffer		  = nullptr;
					drawCommand->indirectBufferOffset = 0;
					drawCommand->numberOfDraws		  = 1;
				}
				inline Draw(const IIndirectBuffer& _indirectBuffer, uint32_t _indirectBufferOffset, uint32_t _numberOfDraws) :
					indirectBuffer(&_indirectBuffer),
					indirectBufferOffset(_indirectBufferOffset),
					numberOfDraws(_numberOfDraws)
				{}
				const IIndirectBuffer* indirectBuffer;
				uint32_t			   indirectBufferOffset;
				uint32_t			   numberOfDraws;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::Draw;
			};
			struct DrawIndexed
			{
				inline static void create(CommandBuffer& commandBuffer, const IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1)
				{
					*commandBuffer.addCommand<DrawIndexed>() = DrawIndexed(indirectBuffer, indirectBufferOffset, numberOfDraws);
				}
				inline static void create(CommandBuffer& commandBuffer, uint32_t indexCountPerInstance, uint32_t instanceCount = 1, uint32_t startIndexLocation = 0, int32_t baseVertexLocation = 0, uint32_t startInstanceLocation = 0)
				{
					DrawIndexed* drawCommand = commandBuffer.addCommand<DrawIndexed>(sizeof(DrawIndexedInstancedArguments));
					const DrawIndexedInstancedArguments drawIndexedInstancedArguments(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
					memcpy(CommandPacketHelper::getAuxiliaryMemory(drawCommand), &drawIndexedInstancedArguments, sizeof(DrawIndexedInstancedArguments));
					drawCommand->indirectBuffer		  = nullptr;
					drawCommand->indirectBufferOffset = 0;
					drawCommand->numberOfDraws		  = 1;
				}
				inline DrawIndexed(const IIndirectBuffer& _indirectBuffer, uint32_t _indirectBufferOffset, uint32_t _numberOfDraws) :
					indirectBuffer(&_indirectBuffer),
					indirectBufferOffset(_indirectBufferOffset),
					numberOfDraws(_numberOfDraws)
				{}
				const IIndirectBuffer* indirectBuffer;
				uint32_t			   indirectBufferOffset;
				uint32_t			   numberOfDraws;
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::DrawIndexed;
			};
			struct SetDebugMarker
			{
				inline static void create(CommandBuffer& commandBuffer, const char* name)
				{
					*commandBuffer.addCommand<SetDebugMarker>() = SetDebugMarker(name);
				}
				inline SetDebugMarker(const char* _name)
				{
					#ifndef RENDERER_NO_DEBUG
						assert(strlen(_name) < 128);
					#endif
					strncpy(name, _name, 128);
					name[127] = '\0';
				};
				char name[128];
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetDebugMarker;
			};
			struct BeginDebugEvent
			{
				inline static void create(CommandBuffer& commandBuffer, const char* name)
				{
					*commandBuffer.addCommand<BeginDebugEvent>() = BeginDebugEvent(name);
				}
				inline BeginDebugEvent(const char* _name)
				{
					#ifndef RENDERER_NO_DEBUG
						assert(strlen(_name) < 128);
					#endif
					strncpy(name, _name, 128);
					name[127] = '\0';
				};
				char name[128];
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::BeginDebugEvent;
			};
			struct EndDebugEvent
			{
				inline static void create(CommandBuffer& commandBuffer)
				{
					commandBuffer.addCommand<EndDebugEvent>();
				}
				static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::EndDebugEvent;
			};
		}
		#ifdef RENDERER_NO_DEBUG
			#define COMMAND_SET_DEBUG_MARKER(commandBuffer, name)
			#define COMMAND_SET_DEBUG_MARKER_FUNCTION(commandBuffer)
			#define COMMAND_BEGIN_DEBUG_EVENT(commandBuffer, name)
			#define COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer)
			#define COMMAND_END_DEBUG_EVENT(commandBuffer)
		#else
			#define COMMAND_SET_DEBUG_MARKER(commandBuffer, name) Renderer::Command::SetDebugMarker::create(commandBuffer, name);
			#define COMMAND_SET_DEBUG_MARKER_FUNCTION(commandBuffer) Renderer::Command::SetDebugMarker::create(commandBuffer, __FUNCTION__);
			#define COMMAND_BEGIN_DEBUG_EVENT(commandBuffer, name) Renderer::Command::BeginDebugEvent::create(commandBuffer, name);
			#define COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer) Renderer::Command::BeginDebugEvent::create(commandBuffer, __FUNCTION__);
			#define COMMAND_END_DEBUG_EVENT(commandBuffer) Renderer::Command::EndDebugEvent::create(commandBuffer);
		#endif
	#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
