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


// TODO(co) Remove unused stuff when done with the Direct3D 12 renderer backend implementation


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D12Renderer/D3D12.h"


//[-------------------------------------------------------]
//[ Global variables                                      ]
//[-------------------------------------------------------]
struct CD3DX12_DEFAULT {};
extern const DECLSPEC_SELECTANY CD3DX12_DEFAULT D3D12_DEFAULT;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
//------------------------------------------------------------------------------------------------
struct CD3DX12_CPU_DESCRIPTOR_HANDLE : public D3D12_CPU_DESCRIPTOR_HANDLE
{

	CD3DX12_CPU_DESCRIPTOR_HANDLE()
	{}

	explicit CD3DX12_CPU_DESCRIPTOR_HANDLE(const D3D12_CPU_DESCRIPTOR_HANDLE &o) :
		D3D12_CPU_DESCRIPTOR_HANDLE(o)
	{}

	CD3DX12_CPU_DESCRIPTOR_HANDLE(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &other, INT offsetScaledByIncrementSize)
	{
		InitOffsetted(other, offsetScaledByIncrementSize);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &other, INT offsetInDescriptors, UINT descriptorIncrementSize)
	{
		InitOffsetted(other, offsetInDescriptors, descriptorIncrementSize);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE& Offset(INT offsetInDescriptors, UINT descriptorIncrementSize)
	{
		ptr += offsetInDescriptors * descriptorIncrementSize;
		return *this;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE& Offset(INT offsetScaledByIncrementSize) 
	{
		ptr += offsetScaledByIncrementSize;
		return *this;
	}

	bool operator==(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other)
	{
		return (ptr == other.ptr);
	}

	bool operator!=(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other)
	{
		return (ptr != other.ptr);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE &operator=(const D3D12_CPU_DESCRIPTOR_HANDLE &other)
	{
		ptr = other.ptr;
		return *this;
	}

	inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetScaledByIncrementSize)
	{
		InitOffsetted(*this, base, offsetScaledByIncrementSize);
	}

	inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetInDescriptors, UINT descriptorIncrementSize)
	{
		InitOffsetted(*this, base, offsetInDescriptors, descriptorIncrementSize);
	}

	static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE &handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetScaledByIncrementSize)
	{
		handle.ptr = base.ptr + offsetScaledByIncrementSize;
	}

	static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE &handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetInDescriptors, UINT descriptorIncrementSize)
	{
		handle.ptr = base.ptr + offsetInDescriptors * descriptorIncrementSize;
	}
};

//------------------------------------------------------------------------------------------------
struct CD3DX12_RESOURCE_BARRIER : public D3D12_RESOURCE_BARRIER
{
	CD3DX12_RESOURCE_BARRIER()
	{}

	explicit CD3DX12_RESOURCE_BARRIER(const D3D12_RESOURCE_BARRIER &o) :
		D3D12_RESOURCE_BARRIER(o)
	{}

	static inline CD3DX12_RESOURCE_BARRIER Transition(
		_In_ ID3D12Resource* pResource,
		D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter,
		UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
		D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE)
	{
		CD3DX12_RESOURCE_BARRIER result;
		ZeroMemory(&result, sizeof(result));
		D3D12_RESOURCE_BARRIER &barrier = result;
		result.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		result.Flags = flags;
		barrier.Transition.pResource = pResource;
		barrier.Transition.StateBefore = stateBefore;
		barrier.Transition.StateAfter = stateAfter;
		barrier.Transition.Subresource = subresource;
		return result;
	}

	static inline CD3DX12_RESOURCE_BARRIER Aliasing(
		_In_ ID3D12Resource* pResourceBefore,
		_In_ ID3D12Resource* pResourceAfter)
	{
		CD3DX12_RESOURCE_BARRIER result;
		ZeroMemory(&result, sizeof(result));
		D3D12_RESOURCE_BARRIER &barrier = result;
		result.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
		barrier.Aliasing.pResourceBefore = pResourceBefore;
		barrier.Aliasing.pResourceAfter = pResourceAfter;
		return result;
	}

	static inline CD3DX12_RESOURCE_BARRIER UAV(_In_ ID3D12Resource* pResource)
	{
		CD3DX12_RESOURCE_BARRIER result;
		ZeroMemory(&result, sizeof(result));
		D3D12_RESOURCE_BARRIER &barrier = result;
		result.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		barrier.UAV.pResource = pResource;
		return result;
	}

	operator const D3D12_RESOURCE_BARRIER&() const
	{
		return *this;
	}
};

//------------------------------------------------------------------------------------------------
struct CD3DX12_HEAP_PROPERTIES : public D3D12_HEAP_PROPERTIES
{
	CD3DX12_HEAP_PROPERTIES()
	{}

	explicit CD3DX12_HEAP_PROPERTIES(const D3D12_HEAP_PROPERTIES &o) :
		D3D12_HEAP_PROPERTIES(o)
	{}

	CD3DX12_HEAP_PROPERTIES( 
		D3D12_CPU_PAGE_PROPERTY cpuPageProperty, 
		D3D12_MEMORY_POOL memoryPoolPreference,
		UINT creationNodeMask = 1, 
		UINT nodeMask = 1 )
	{
		Type = D3D12_HEAP_TYPE_CUSTOM;
		CPUPageProperty = cpuPageProperty;
		MemoryPoolPreference = memoryPoolPreference;
		CreationNodeMask = creationNodeMask;
		VisibleNodeMask = nodeMask;
	}

	explicit CD3DX12_HEAP_PROPERTIES( 
		D3D12_HEAP_TYPE type, 
		UINT creationNodeMask = 1, 
		UINT nodeMask = 1 )
	{
		Type = type;
		CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		CreationNodeMask = creationNodeMask;
		VisibleNodeMask = nodeMask;
	}

	operator const D3D12_HEAP_PROPERTIES&() const
	{
		return *this;
	}

	bool IsCPUAccessible() const
	{
		return Type == D3D12_HEAP_TYPE_UPLOAD || Type == D3D12_HEAP_TYPE_READBACK || (Type == D3D12_HEAP_TYPE_CUSTOM &&
			(CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE || CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_BACK));
	}
};

//------------------------------------------------------------------------------------------------
struct CD3DX12_RESOURCE_DESC : public D3D12_RESOURCE_DESC
{
	CD3DX12_RESOURCE_DESC()
	{}

	explicit CD3DX12_RESOURCE_DESC( const D3D12_RESOURCE_DESC& o ) :
		D3D12_RESOURCE_DESC( o )
	{}

	CD3DX12_RESOURCE_DESC( 
		D3D12_RESOURCE_DIMENSION dimension,
		UINT64 alignment,
		UINT64 width,
		UINT height,
		UINT16 depthOrArraySize,
		UINT16 mipLevels,
		DXGI_FORMAT format,
		UINT sampleCount,
		UINT sampleQuality,
		D3D12_TEXTURE_LAYOUT layout,
		D3D12_RESOURCE_FLAGS flags )
	{
		Dimension = dimension;
		Alignment = alignment;
		Width = width;
		Height = height;
		DepthOrArraySize = depthOrArraySize;
		MipLevels = mipLevels;
		Format = format;
		SampleDesc.Count = sampleCount;
		SampleDesc.Quality = sampleQuality;
		Layout = layout;
		Flags = flags;
	}

	static inline CD3DX12_RESOURCE_DESC Buffer( 
		const D3D12_RESOURCE_ALLOCATION_INFO& resAllocInfo,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE )
	{
		return CD3DX12_RESOURCE_DESC( D3D12_RESOURCE_DIMENSION_BUFFER, resAllocInfo.Alignment, resAllocInfo.SizeInBytes, 
			1, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, flags );
	}

	static inline CD3DX12_RESOURCE_DESC Buffer( 
		UINT64 width,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
		UINT64 alignment = 0 )
	{
		return CD3DX12_RESOURCE_DESC( D3D12_RESOURCE_DIMENSION_BUFFER, alignment, width, 1, 1, 1, 
			DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, flags );
	}

	static inline CD3DX12_RESOURCE_DESC Tex1D( 
		DXGI_FORMAT format,
		UINT64 width,
		UINT16 arraySize = 1,
		UINT16 mipLevels = 0,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
		D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		UINT64 alignment = 0 )
	{
		return CD3DX12_RESOURCE_DESC( D3D12_RESOURCE_DIMENSION_TEXTURE1D, alignment, width, 1, arraySize, 
			mipLevels, format, 1, 0, layout, flags );
	}

	static inline CD3DX12_RESOURCE_DESC Tex2D( 
		DXGI_FORMAT format,
		UINT64 width,
		UINT height,
		UINT16 arraySize = 1,
		UINT16 mipLevels = 0,
		UINT sampleCount = 1,
		UINT sampleQuality = 0,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
		D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		UINT64 alignment = 0 )
	{
		return CD3DX12_RESOURCE_DESC( D3D12_RESOURCE_DIMENSION_TEXTURE2D, alignment, width, height, arraySize, 
			mipLevels, format, sampleCount, sampleQuality, layout, flags );
	}

	static inline CD3DX12_RESOURCE_DESC Tex3D( 
		DXGI_FORMAT format,
		UINT64 width,
		UINT height,
		UINT16 depth,
		UINT16 mipLevels = 0,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
		D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		UINT64 alignment = 0 )
	{
		return CD3DX12_RESOURCE_DESC( D3D12_RESOURCE_DIMENSION_TEXTURE3D, alignment, width, height, depth, 
			mipLevels, format, 1, 0, layout, flags );
	}

	inline UINT16 Depth() const
	{
		return (Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ? DepthOrArraySize : 1u);
	}

	inline UINT16 ArraySize() const
	{
		return (Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE3D ? DepthOrArraySize : 1u);
	}

	inline UINT8 PlaneCount(_In_ ID3D12Device*) const
	{
		// TODO(co) Implement me
		// return D3D12GetFormatPlaneCount(pDevice, Format);
		return 0;
	}

	inline UINT Subresources(_In_ ID3D12Device* pDevice) const
	{
		return static_cast<UINT>(MipLevels * ArraySize() * PlaneCount(pDevice));
	}

	inline UINT CalcSubresource(UINT, UINT, UINT)
	{
		// TODO(co) Implement me
		// return D3D12CalcSubresource(MipSlice, ArraySlice, PlaneSlice, MipLevels, ArraySize());
		return 0;
	}

	operator const D3D12_RESOURCE_DESC&() const
	{
		return *this;
	}
};

inline bool operator==( const D3D12_RESOURCE_DESC& l, const D3D12_RESOURCE_DESC& r )
{
	return l.Dimension == r.Dimension &&
		l.Alignment == r.Alignment &&
		l.Width == r.Width &&
		l.Height == r.Height &&
		l.DepthOrArraySize == r.DepthOrArraySize &&
		l.MipLevels == r.MipLevels &&
		l.Format == r.Format &&
		l.SampleDesc.Count == r.SampleDesc.Count &&
		l.SampleDesc.Quality == r.SampleDesc.Quality &&
		l.Layout == r.Layout &&
		l.Flags == r.Flags;
}

inline bool operator!=( const D3D12_RESOURCE_DESC& l, const D3D12_RESOURCE_DESC& r )
{
	return !( l == r );
}

//------------------------------------------------------------------------------------------------
struct CD3DX12_RANGE : public D3D12_RANGE
{
	CD3DX12_RANGE()
	{}

	explicit CD3DX12_RANGE(const D3D12_RANGE &o) :
		D3D12_RANGE(o)
	{}

	CD3DX12_RANGE( 
		SIZE_T begin, 
		SIZE_T end )
	{
		Begin = begin;
		End = end;
	}

	operator const D3D12_RANGE&() const
	{
		return *this;
	}

};

//------------------------------------------------------------------------------------------------
struct CD3DX12_DESCRIPTOR_RANGE : public D3D12_DESCRIPTOR_RANGE
{
	CD3DX12_DESCRIPTOR_RANGE()
	{}

	explicit CD3DX12_DESCRIPTOR_RANGE(const D3D12_DESCRIPTOR_RANGE &o) :
		D3D12_DESCRIPTOR_RANGE(o)
	{}

	CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
		UINT numDescriptors,
		UINT baseShaderRegister,
		UINT registerSpace = 0,
		UINT offsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
	{
		Init(rangeType, numDescriptors, baseShaderRegister, registerSpace, offsetInDescriptorsFromTableStart);
	}

	inline void Init(
		D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
		UINT numDescriptors,
		UINT baseShaderRegister,
		UINT registerSpace = 0,
		UINT offsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
	{
		Init(*this, rangeType, numDescriptors, baseShaderRegister, registerSpace, offsetInDescriptorsFromTableStart);
	}

	static inline void Init(
		_Out_ D3D12_DESCRIPTOR_RANGE &range,
		D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
		UINT numDescriptors,
		UINT baseShaderRegister,
		UINT registerSpace = 0,
		UINT offsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
	{
		range.RangeType = rangeType;
		range.NumDescriptors = numDescriptors;
		range.BaseShaderRegister = baseShaderRegister;
		range.RegisterSpace = registerSpace;
		range.OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
	}
};

//------------------------------------------------------------------------------------------------
struct CD3DX12_ROOT_SIGNATURE_DESC : public D3D12_ROOT_SIGNATURE_DESC
{
	CD3DX12_ROOT_SIGNATURE_DESC()
	{}

	explicit CD3DX12_ROOT_SIGNATURE_DESC(const D3D12_ROOT_SIGNATURE_DESC &o) :
		D3D12_ROOT_SIGNATURE_DESC(o)
	{}

	CD3DX12_ROOT_SIGNATURE_DESC(
		UINT numParameters,
		_In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
		UINT numStaticSamplers = 0,
		_In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
	{
		Init(numParameters, _pParameters, numStaticSamplers, _pStaticSamplers, flags);
	}

	CD3DX12_ROOT_SIGNATURE_DESC(CD3DX12_DEFAULT)
	{
		Init(0, NULL, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_NONE);
	}

	inline void Init(
		UINT numParameters,
		_In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
		UINT numStaticSamplers = 0,
		_In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
	{
		Init(*this, numParameters, _pParameters, numStaticSamplers, _pStaticSamplers, flags);
	}

	static inline void Init(
		_Out_ D3D12_ROOT_SIGNATURE_DESC &desc,
		UINT numParameters,
		_In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
		UINT numStaticSamplers = 0,
		_In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
	{
		desc.NumParameters = numParameters;
		desc.pParameters = _pParameters;
		desc.NumStaticSamplers = numStaticSamplers;
		desc.pStaticSamplers = _pStaticSamplers;
		desc.Flags = flags;
	}
};

//------------------------------------------------------------------------------------------------
struct CD3DX12_RASTERIZER_DESC : public D3D12_RASTERIZER_DESC
{
	CD3DX12_RASTERIZER_DESC()
	{}

	explicit CD3DX12_RASTERIZER_DESC( const D3D12_RASTERIZER_DESC& o ) :
		D3D12_RASTERIZER_DESC( o )
	{}

	explicit CD3DX12_RASTERIZER_DESC( CD3DX12_DEFAULT )
	{
		FillMode = D3D12_FILL_MODE_SOLID;
		CullMode = D3D12_CULL_MODE_BACK;
		FrontCounterClockwise = FALSE;
		DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		DepthClipEnable = TRUE;
		MultisampleEnable = FALSE;
		AntialiasedLineEnable = FALSE;
		ForcedSampleCount = 0;
		ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	explicit CD3DX12_RASTERIZER_DESC(
		D3D12_FILL_MODE fillMode,
		D3D12_CULL_MODE cullMode,
		BOOL frontCounterClockwise,
		INT depthBias,
		FLOAT depthBiasClamp,
		FLOAT slopeScaledDepthBias,
		BOOL depthClipEnable,
		BOOL multisampleEnable,
		BOOL antialiasedLineEnable, 
		UINT forcedSampleCount, 
		D3D12_CONSERVATIVE_RASTERIZATION_MODE conservativeRaster)
	{
		FillMode = fillMode;
		CullMode = cullMode;
		FrontCounterClockwise = frontCounterClockwise;
		DepthBias = depthBias;
		DepthBiasClamp = depthBiasClamp;
		SlopeScaledDepthBias = slopeScaledDepthBias;
		DepthClipEnable = depthClipEnable;
		MultisampleEnable = multisampleEnable;
		AntialiasedLineEnable = antialiasedLineEnable;
		ForcedSampleCount = forcedSampleCount;
		ConservativeRaster = conservativeRaster;
	}

	~CD3DX12_RASTERIZER_DESC()
	{}

	operator const D3D12_RASTERIZER_DESC&() const
	{
		return *this;
	}

};

//------------------------------------------------------------------------------------------------
struct CD3DX12_BLEND_DESC : public D3D12_BLEND_DESC
{
	CD3DX12_BLEND_DESC()
	{}

	explicit CD3DX12_BLEND_DESC( const D3D12_BLEND_DESC& o ) :
		D3D12_BLEND_DESC( o )
	{}

	explicit CD3DX12_BLEND_DESC( CD3DX12_DEFAULT )
	{
		AlphaToCoverageEnable = FALSE;
		IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
		{
			FALSE,FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			RenderTarget[ i ] = defaultRenderTargetBlendDesc;
	}

	~CD3DX12_BLEND_DESC()
	{}

	operator const D3D12_BLEND_DESC&() const
	{
		return *this;
	}
};
