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


/*
  We don't use the Direct3D headers from the DirectX SDK because there are several issues:
  - Licensing: It's not allowed to redistribute the Direct3D headers, meaning everyone would
    have to get them somehow before compiling this project
  - The Direct3D headers are somewhat chaotic and include tons of other headers.
    This slows down compilation and the more headers are included, the higher the risk of
    naming or redefinition conflicts.

    Do not include this header within headers which are usually used by users as well, do only
    use it inside cpp-files. It must still be possible that users of this renderer interface
    can use the Direct3D headers for features not covered by this renderer interface.
*/


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/WindowsHeader.h>

__pragma(warning(push))
	__pragma(warning(disable: 4668))	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <Unknwn.h>	// For "IUnknown"
__pragma(warning(pop))


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct DXGI_RESIDENCY;
struct DXGI_SURFACE_DESC;
struct DXGI_ADAPTER_DESC;
struct DXGI_SHARED_RESOURCE;
struct DXGI_FRAME_STATISTICS;
struct DXGI_SWAP_CHAIN_DESC1;
struct DXGI_SWAP_CHAIN_FULLSCREEN_DESC;
struct IDXGIOutput;
struct IDXGISurface;
struct IDXGIAdapter;
struct IDXGIAdapter1;
struct IDXGISwapChain1;
struct D3D12_HEAP_DESC;
struct D3D12_TILE_SHAPE;
struct D3D12_SAMPLER_DESC;
struct D3D12_RESOURCE_DESC;
struct D3D12_QUERY_HEAP_DESC;
struct D3D12_PACKED_MIP_INFO;
struct D3D12_TILE_REGION_SIZE;
struct D3D12_TILE_RANGE_FLAGS;
struct D3D12_SUBRESOURCE_TILING;
struct D3D12_COMMAND_SIGNATURE_DESC;
struct D3D12_RENDER_TARGET_VIEW_DESC;
struct D3D12_DEPTH_STENCIL_VIEW_DESC;
struct D3D12_TILED_RESOURCE_COORDINATE;
struct D3D12_CONSTANT_BUFFER_VIEW_DESC;
struct D3D12_SHADER_RESOURCE_VIEW_DESC;
struct D3D12_UNORDERED_ACCESS_VIEW_DESC;
struct D3D12_COMPUTE_PIPELINE_STATE_DESC;
struct D3D12_GRAPHICS_PIPELINE_STATE_DESC;
struct D3D12_PLACED_SUBRESOURCE_FOOTPRINT;
struct ID3D12Heap;
struct ID3D12Fence;
struct ID3D12CommandList;
struct ID3D12RootSignature;
struct ID3D12PipelineState;
struct ID3D12CommandAllocator;

// TODO(co) Direct3D 12 update
struct ID3D10Blob;
struct D3D10_SHADER_MACRO;
typedef __interface ID3DInclude *LPD3D10INCLUDE;
struct ID3DX12ThreadPump;
typedef DWORD D3DCOLOR;


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "dxgiformat.h"
typedef enum DXGI_FORMAT
{
	DXGI_FORMAT_UNKNOWN						= 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS		= 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT			= 2,
	DXGI_FORMAT_R32G32B32A32_UINT			= 3,
	DXGI_FORMAT_R32G32B32A32_SINT			= 4,
	DXGI_FORMAT_R32G32B32_TYPELESS			= 5,
	DXGI_FORMAT_R32G32B32_FLOAT				= 6,
	DXGI_FORMAT_R32G32B32_UINT				= 7,
	DXGI_FORMAT_R32G32B32_SINT				= 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS		= 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT			= 10,
	DXGI_FORMAT_R16G16B16A16_UNORM			= 11,
	DXGI_FORMAT_R16G16B16A16_UINT			= 12,
	DXGI_FORMAT_R16G16B16A16_SNORM			= 13,
	DXGI_FORMAT_R16G16B16A16_SINT			= 14,
	DXGI_FORMAT_R32G32_TYPELESS				= 15,
	DXGI_FORMAT_R32G32_FLOAT				= 16,
	DXGI_FORMAT_R32G32_UINT					= 17,
	DXGI_FORMAT_R32G32_SINT					= 18,
	DXGI_FORMAT_R32G8X24_TYPELESS			= 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT		= 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS	= 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT		= 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS		= 23,
	DXGI_FORMAT_R10G10B10A2_UNORM			= 24,
	DXGI_FORMAT_R10G10B10A2_UINT			= 25,
	DXGI_FORMAT_R11G11B10_FLOAT				= 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS			= 27,
	DXGI_FORMAT_R8G8B8A8_UNORM				= 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB			= 29,
	DXGI_FORMAT_R8G8B8A8_UINT				= 30,
	DXGI_FORMAT_R8G8B8A8_SNORM				= 31,
	DXGI_FORMAT_R8G8B8A8_SINT				= 32,
	DXGI_FORMAT_R16G16_TYPELESS				= 33,
	DXGI_FORMAT_R16G16_FLOAT				= 34,
	DXGI_FORMAT_R16G16_UNORM				= 35,
	DXGI_FORMAT_R16G16_UINT					= 36,
	DXGI_FORMAT_R16G16_SNORM				= 37,
	DXGI_FORMAT_R16G16_SINT					= 38,
	DXGI_FORMAT_R32_TYPELESS				= 39,
	DXGI_FORMAT_D32_FLOAT					= 40,
	DXGI_FORMAT_R32_FLOAT					= 41,
	DXGI_FORMAT_R32_UINT					= 42,
	DXGI_FORMAT_R32_SINT					= 43,
	DXGI_FORMAT_R24G8_TYPELESS				= 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT			= 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS		= 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT		= 47,
	DXGI_FORMAT_R8G8_TYPELESS				= 48,
	DXGI_FORMAT_R8G8_UNORM					= 49,
	DXGI_FORMAT_R8G8_UINT					= 50,
	DXGI_FORMAT_R8G8_SNORM					= 51,
	DXGI_FORMAT_R8G8_SINT					= 52,
	DXGI_FORMAT_R16_TYPELESS				= 53,
	DXGI_FORMAT_R16_FLOAT					= 54,
	DXGI_FORMAT_D16_UNORM					= 55,
	DXGI_FORMAT_R16_UNORM					= 56,
	DXGI_FORMAT_R16_UINT					= 57,
	DXGI_FORMAT_R16_SNORM					= 58,
	DXGI_FORMAT_R16_SINT					= 59,
	DXGI_FORMAT_R8_TYPELESS					= 60,
	DXGI_FORMAT_R8_UNORM					= 61,
	DXGI_FORMAT_R8_UINT						= 62,
	DXGI_FORMAT_R8_SNORM					= 63,
	DXGI_FORMAT_R8_SINT						= 64,
	DXGI_FORMAT_A8_UNORM					= 65,
	DXGI_FORMAT_R1_UNORM					= 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP			= 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM				= 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM				= 69,
	DXGI_FORMAT_BC1_TYPELESS				= 70,
	DXGI_FORMAT_BC1_UNORM					= 71,
	DXGI_FORMAT_BC1_UNORM_SRGB				= 72,
	DXGI_FORMAT_BC2_TYPELESS				= 73,
	DXGI_FORMAT_BC2_UNORM					= 74,
	DXGI_FORMAT_BC2_UNORM_SRGB				= 75,
	DXGI_FORMAT_BC3_TYPELESS				= 76,
	DXGI_FORMAT_BC3_UNORM					= 77,
	DXGI_FORMAT_BC3_UNORM_SRGB				= 78,
	DXGI_FORMAT_BC4_TYPELESS				= 79,
	DXGI_FORMAT_BC4_UNORM					= 80,
	DXGI_FORMAT_BC4_SNORM					= 81,
	DXGI_FORMAT_BC5_TYPELESS				= 82,
	DXGI_FORMAT_BC5_UNORM					= 83,
	DXGI_FORMAT_BC5_SNORM					= 84,
	DXGI_FORMAT_B5G6R5_UNORM				= 85,
	DXGI_FORMAT_B5G5R5A1_UNORM				= 86,
	DXGI_FORMAT_B8G8R8A8_UNORM				= 87,
	DXGI_FORMAT_B8G8R8X8_UNORM				= 8,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM	= 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS			= 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB			= 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS			= 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB			= 93,
	DXGI_FORMAT_BC6H_TYPELESS				= 94,
	DXGI_FORMAT_BC6H_UF16					= 95,
	DXGI_FORMAT_BC6H_SF16					= 96,
	DXGI_FORMAT_BC7_TYPELESS				= 97,
	DXGI_FORMAT_BC7_UNORM					= 98,
	DXGI_FORMAT_BC7_UNORM_SRGB				= 99,
	DXGI_FORMAT_AYUV						= 100,
	DXGI_FORMAT_Y410						= 101,
	DXGI_FORMAT_Y416						= 102,
	DXGI_FORMAT_NV12						= 103,
	DXGI_FORMAT_P010						= 104,
	DXGI_FORMAT_P016						= 105,
	DXGI_FORMAT_420_OPAQUE					= 106,
	DXGI_FORMAT_YUY2						= 107,
	DXGI_FORMAT_Y210						= 108,
	DXGI_FORMAT_Y216						= 109,
	DXGI_FORMAT_NV11						= 110,
	DXGI_FORMAT_AI44						= 111,
	DXGI_FORMAT_IA44						= 112,
	DXGI_FORMAT_P8							= 113,
	DXGI_FORMAT_A8P8						= 114,
	DXGI_FORMAT_B4G4R4A4_UNORM				= 115,
	DXGI_FORMAT_P208						= 130,
	DXGI_FORMAT_V208						= 131,
	DXGI_FORMAT_V408						= 132,
	DXGI_FORMAT_FORCE_UINT					= 0xffffffff
} DXGI_FORMAT;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "dxgitype.h"
typedef struct DXGI_RATIONAL
{
	UINT Numerator;
	UINT Denominator;
} DXGI_RATIONAL;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "dxgitype.h"
typedef enum DXGI_MODE_SCANLINE_ORDER
{
	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED		= 0,
	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE		= 1,
	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST	= 2,
	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST	= 3
} DXGI_MODE_SCANLINE_ORDER;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "dxgitype.h"
typedef enum DXGI_MODE_SCALING
{
	DXGI_MODE_SCALING_UNSPECIFIED	= 0,
	DXGI_MODE_SCALING_CENTERED		= 1,
	DXGI_MODE_SCALING_STRETCHED		= 2
} DXGI_MODE_SCALING;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "dxgitype.h"
typedef struct DXGI_MODE_DESC
{
	UINT Width;
	UINT Height;
	DXGI_RATIONAL RefreshRate;
	DXGI_FORMAT Format;
	DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
	DXGI_MODE_SCALING Scaling;
} DXGI_MODE_DESC;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "dxgitype.h"
typedef struct DXGI_SAMPLE_DESC
{
	UINT Count;
	UINT Quality;
} DXGI_SAMPLE_DESC;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
typedef UINT DXGI_USAGE;
#define DXGI_MWA_NO_ALT_ENTER			(1 << 1)
#define DXGI_USAGE_RENDER_TARGET_OUTPUT	0x00000020UL

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
typedef enum DXGI_SWAP_EFFECT
{
	DXGI_SWAP_EFFECT_DISCARD			= 0,
	DXGI_SWAP_EFFECT_SEQUENTIAL			= 1,
	DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL	= 3,
	DXGI_SWAP_EFFECT_FLIP_DISCARD		= 4
} DXGI_SWAP_EFFECT;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
typedef struct DXGI_SWAP_CHAIN_DESC
{
	DXGI_MODE_DESC BufferDesc;
	DXGI_SAMPLE_DESC SampleDesc;
	DXGI_USAGE BufferUsage;
	UINT BufferCount;
	HWND OutputWindow;
	BOOL Windowed;
	DXGI_SWAP_EFFECT SwapEffect;
	UINT Flags;
} DXGI_SWAP_CHAIN_DESC;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3Dcommon.h"
enum D3D_FEATURE_LEVEL
{
	D3D_FEATURE_LEVEL_9_1	= 0x9100,
	D3D_FEATURE_LEVEL_9_2	= 0x9200,
	D3D_FEATURE_LEVEL_9_3	= 0x9300,
	D3D_FEATURE_LEVEL_10_0	= 0xa000,
	D3D_FEATURE_LEVEL_10_1	= 0xa100,
	D3D_FEATURE_LEVEL_11_0	= 0xb000,
	D3D_FEATURE_LEVEL_11_1	= 0xb100,
	D3D_FEATURE_LEVEL_12_0	= 0xc000,
	D3D_FEATURE_LEVEL_12_1	= 0xc100
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_COMMAND_LIST_TYPE
{
	D3D12_COMMAND_LIST_TYPE_DIRECT	= 0,
	D3D12_COMMAND_LIST_TYPE_BUNDLE	= 1,
	D3D12_COMMAND_LIST_TYPE_COMPUTE	= 2,
	D3D12_COMMAND_LIST_TYPE_COPY	= 3
} D3D12_COMMAND_LIST_TYPE;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
struct D3D12_RANGE;
struct D3D12_BOX;
typedef UINT64 D3D12_GPU_VIRTUAL_ADDRESS;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_COMMAND_QUEUE_FLAGS
{
	D3D12_COMMAND_QUEUE_FLAG_NONE					= 0,
	D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT	= 0x1
} D3D12_COMMAND_QUEUE_FLAGS;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_COMMAND_QUEUE_PRIORITY
{
	D3D12_COMMAND_QUEUE_PRIORITY_NORMAL	= 0,
	D3D12_COMMAND_QUEUE_PRIORITY_HIGH	= 100
} D3D12_COMMAND_QUEUE_PRIORITY;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef struct D3D12_COMMAND_QUEUE_DESC
{
	D3D12_COMMAND_LIST_TYPE Type;
	INT Priority;
	D3D12_COMMAND_QUEUE_FLAGS Flags;
	UINT NodeMask;
} D3D12_COMMAND_QUEUE_DESC;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef struct D3D12_RESOURCE_ALLOCATION_INFO
{
	UINT64 SizeInBytes;
	UINT64 Alignment;
} D3D12_RESOURCE_ALLOCATION_INFO;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_HEAP_TYPE
{
	D3D12_HEAP_TYPE_DEFAULT		= 1,
	D3D12_HEAP_TYPE_UPLOAD		= 2,
	D3D12_HEAP_TYPE_READBACK	= 3,
	D3D12_HEAP_TYPE_CUSTOM		= 4
} D3D12_HEAP_TYPE;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_CPU_PAGE_PROPERTY
{
	D3D12_CPU_PAGE_PROPERTY_UNKNOWN			= 0,
	D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE	= 1,
	D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE	= 2,
	D3D12_CPU_PAGE_PROPERTY_WRITE_BACK		= 3
} D3D12_CPU_PAGE_PROPERTY;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_MEMORY_POOL
{
	D3D12_MEMORY_POOL_UNKNOWN	= 0,
	D3D12_MEMORY_POOL_L0		= 1,
	D3D12_MEMORY_POOL_L1		= 2
} D3D12_MEMORY_POOL;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef struct D3D12_HEAP_PROPERTIES
{
	D3D12_HEAP_TYPE Type;
	D3D12_CPU_PAGE_PROPERTY CPUPageProperty;
	D3D12_MEMORY_POOL MemoryPoolPreference;
	UINT CreationNodeMask;
	UINT VisibleNodeMask;
} D3D12_HEAP_PROPERTIES;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_HEAP_FLAGS
{
	D3D12_HEAP_FLAG_NONE							= 0,
	D3D12_HEAP_FLAG_SHARED							= 0x1,
	D3D12_HEAP_FLAG_DENY_BUFFERS					= 0x4,
	D3D12_HEAP_FLAG_ALLOW_DISPLAY					= 0x8,
	D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER			= 0x20,
	D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES				= 0x40,
	D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES			= 0x80,
	D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES	= 0,
	D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS				= 0xc0,
	D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES	= 0x44,
	D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES		= 0x84
} D3D12_HEAP_FLAGS;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_RESOURCE_STATES
{
	D3D12_RESOURCE_STATE_COMMON						= 0,
	D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER	= 0x1,
	D3D12_RESOURCE_STATE_INDEX_BUFFER				= 0x2,
	D3D12_RESOURCE_STATE_RENDER_TARGET				= 0x4,
	D3D12_RESOURCE_STATE_UNORDERED_ACCESS			= 0x8,
	D3D12_RESOURCE_STATE_DEPTH_WRITE				= 0x10,
	D3D12_RESOURCE_STATE_DEPTH_READ					= 0x20,
	D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE	= 0x40,
	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE		= 0x80,
	D3D12_RESOURCE_STATE_STREAM_OUT					= 0x100,
	D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT			= 0x200,
	D3D12_RESOURCE_STATE_COPY_DEST					= 0x400,
	D3D12_RESOURCE_STATE_COPY_SOURCE				= 0x800,
	D3D12_RESOURCE_STATE_RESOLVE_DEST				= 0x1000,
	D3D12_RESOURCE_STATE_RESOLVE_SOURCE				= 0x2000,
	D3D12_RESOURCE_STATE_GENERIC_READ				= ((((( 0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
	D3D12_RESOURCE_STATE_PRESENT					= 0,
	D3D12_RESOURCE_STATE_PREDICATION				= 0x200
} D3D12_RESOURCE_STATES;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef struct D3D12_DEPTH_STENCIL_VALUE
{
	FLOAT Depth;
	UINT8 Stencil;
} D3D12_DEPTH_STENCIL_VALUE;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef struct D3D12_CLEAR_VALUE
{
	DXGI_FORMAT Format;
	union
	{
		FLOAT Color[4];
		D3D12_DEPTH_STENCIL_VALUE DepthStencil;
	};
} D3D12_CLEAR_VALUE;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_FENCE_FLAGS
{
	D3D12_FENCE_FLAG_NONE					= 0,
	D3D12_FENCE_FLAG_SHARED					= 0x1,
	D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER	= 0x2
} D3D12_FENCE_FLAGS;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_FEATURE
{
	D3D12_FEATURE_D3D12_OPTIONS					= 0,
	D3D12_FEATURE_ARCHITECTURE					= (D3D12_FEATURE_D3D12_OPTIONS + 1),
	D3D12_FEATURE_FEATURE_LEVELS				= (D3D12_FEATURE_ARCHITECTURE + 1),
	D3D12_FEATURE_FORMAT_SUPPORT				= (D3D12_FEATURE_FEATURE_LEVELS + 1),
	D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS	= (D3D12_FEATURE_FORMAT_SUPPORT + 1),
	D3D12_FEATURE_FORMAT_INFO					= (D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS + 1),
	D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT	= (D3D12_FEATURE_FORMAT_INFO + 1) 
} D3D12_FEATURE;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_TILE_MAPPING_FLAGS
{
	D3D12_TILE_MAPPING_FLAG_NONE		= 0,
	D3D12_TILE_MAPPING_FLAG_NO_HAZARD	= 0x1
} D3D12_TILE_MAPPING_FLAGS;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef struct D3D12_CPU_DESCRIPTOR_HANDLE
{
	SIZE_T ptr;
} D3D12_CPU_DESCRIPTOR_HANDLE;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef struct D3D12_GPU_DESCRIPTOR_HANDLE
{
	UINT64 ptr;
} D3D12_GPU_DESCRIPTOR_HANDLE;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_DESCRIPTOR_HEAP_TYPE
{
	D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV	= 0,
	D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER		= (D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV + 1),
	D3D12_DESCRIPTOR_HEAP_TYPE_RTV			= (D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER + 1),
	D3D12_DESCRIPTOR_HEAP_TYPE_DSV			= (D3D12_DESCRIPTOR_HEAP_TYPE_RTV + 1),
	D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES	= (D3D12_DESCRIPTOR_HEAP_TYPE_DSV + 1) 
} D3D12_DESCRIPTOR_HEAP_TYPE;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef enum D3D12_DESCRIPTOR_HEAP_FLAGS
{
	D3D12_DESCRIPTOR_HEAP_FLAG_NONE				= 0,
	D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE	= 0x1
} D3D12_DESCRIPTOR_HEAP_FLAGS;

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
typedef struct D3D12_DESCRIPTOR_HEAP_DESC
{
	D3D12_DESCRIPTOR_HEAP_TYPE Type;
	UINT NumDescriptors;
	D3D12_DESCRIPTOR_HEAP_FLAGS Flags;
	UINT NodeMask;
} D3D12_DESCRIPTOR_HEAP_DESC;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
MIDL_INTERFACE("aec22fb8-76f3-4639-9be0-28eb43a67a2e")
IDXGIObject : public IUnknown
{
	public:
		virtual HRESULT STDMETHODCALLTYPE SetPrivateData(_In_ REFGUID Name, UINT DataSize, _In_reads_bytes_(DataSize) const void *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(_In_ REFGUID Name, _In_ const IUnknown *pUnknown) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetPrivateData(_In_ REFGUID Name, _Inout_ UINT *pDataSize, _Out_writes_bytes_(*pDataSize) void *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetParent(_In_ REFIID riid, _COM_Outptr_ void **ppParent) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
MIDL_INTERFACE("3d3e0379-f9de-4d58-bb6c-18d62992f1a6")
IDXGIDeviceSubObject : public IDXGIObject
{
	public:
		virtual HRESULT STDMETHODCALLTYPE GetDevice(_In_ REFIID riid, _COM_Outptr_ void **ppDevice) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
MIDL_INTERFACE("310d36a0-d2e7-4c0a-aa04-6a9d23b8886a")
IDXGISwapChain : public IDXGIDeviceSubObject
{
	public:
		virtual HRESULT STDMETHODCALLTYPE Present(UINT SyncInterval, UINT Flags) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetBuffer(UINT Buffer, _In_ REFIID riid, _COM_Outptr_ void **ppSurface) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL Fullscreen, _In_opt_ IDXGIOutput *pTarget) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetFullscreenState(_Out_opt_ BOOL *pFullscreen, _COM_Outptr_opt_result_maybenull_ IDXGIOutput **ppTarget) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetDesc(_Out_ DXGI_SWAP_CHAIN_DESC *pDesc) = 0;
		virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = 0;
		virtual HRESULT STDMETHODCALLTYPE ResizeTarget(_In_ const DXGI_MODE_DESC *pNewTargetParameters) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetContainingOutput(_COM_Outptr_ IDXGIOutput **ppOutput) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(_Out_ DXGI_FRAME_STATISTICS *pStats) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(_Out_ UINT *pLastPresentCount) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
MIDL_INTERFACE("7b7166ec-21c7-44ae-b21a-c9ae321ae369")
IDXGIFactory : public IDXGIObject
{
	public:
		virtual HRESULT STDMETHODCALLTYPE EnumAdapters(UINT Adapter, _COM_Outptr_ IDXGIAdapter **ppAdapter) = 0;
		virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation(HWND WindowHandle, UINT Flags) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation(_Out_  HWND *pWindowHandle) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(_In_ IUnknown *pDevice, _In_ DXGI_SWAP_CHAIN_DESC *pDesc, _COM_Outptr_ IDXGISwapChain **ppSwapChain) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(HMODULE Module, _COM_Outptr_ IDXGIAdapter **ppAdapter) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
MIDL_INTERFACE("770aae78-f26f-4dba-a829-253c83d1b387")
IDXGIFactory1 : public IDXGIFactory
{
	public:
		virtual HRESULT STDMETHODCALLTYPE EnumAdapters1(UINT Adapter, __out IDXGIAdapter1 **ppAdapter) = 0;
		virtual BOOL STDMETHODCALLTYPE IsCurrent(void) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
MIDL_INTERFACE("50c83a1c-e072-4c48-87b0-3630fa36a6d0")
IDXGIFactory2 : public IDXGIFactory1
{
	public:
		virtual BOOL STDMETHODCALLTYPE IsWindowedStereoEnabled(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForHwnd(_In_ IUnknown *pDevice, _In_ HWND hWnd, _In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc, _In_opt_ const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc, _In_opt_ IDXGIOutput *pRestrictToOutput, _COM_Outptr_ IDXGISwapChain1 **ppSwapChain) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForCoreWindow(_In_ IUnknown *pDevice, _In_ IUnknown *pWindow, _In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc, _In_opt_ IDXGIOutput *pRestrictToOutput, _COM_Outptr_ IDXGISwapChain1 **ppSwapChain) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetSharedResourceAdapterLuid(_In_ HANDLE hResource, _Out_  LUID *pLuid) = 0;
		virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusWindow(_In_ HWND WindowHandle, _In_ UINT wMsg, _Out_ DWORD *pdwCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusEvent(_In_ HANDLE hEvent, _Out_ DWORD *pdwCookie) = 0;
		virtual void STDMETHODCALLTYPE UnregisterStereoStatus(_In_ DWORD dwCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusWindow(_In_ HWND WindowHandle, _In_ UINT wMsg, _Out_ DWORD *pdwCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusEvent(_In_ HANDLE hEvent, _Out_ DWORD *pdwCookie) = 0;
		virtual void STDMETHODCALLTYPE UnregisterOcclusionStatus(_In_ DWORD dwCookie) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForComposition(_In_ IUnknown *pDevice, _In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc, _In_opt_ IDXGIOutput *pRestrictToOutput, _COM_Outptr_ IDXGISwapChain1 **ppSwapChain) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
MIDL_INTERFACE("25483823-cd46-4c7d-86ca-47aa95b837bd")
IDXGIFactory3 : public IDXGIFactory2
{
	public:
		virtual UINT STDMETHODCALLTYPE GetCreationFlags(void) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
MIDL_INTERFACE("1bc6ea02-ef36-464f-bf0c-21ca39e5168a")
IDXGIFactory4 : public IDXGIFactory3
{
	public:
		virtual HRESULT STDMETHODCALLTYPE EnumAdapterByLuid(_In_ LUID AdapterLuid, _In_ REFIID riid, _COM_Outptr_ void **ppvAdapter) = 0;
		virtual HRESULT STDMETHODCALLTYPE EnumWarpAdapter(_In_ REFIID riid, _COM_Outptr_ void **ppvAdapter) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
MIDL_INTERFACE("2411e7e1-12ac-4ccf-bd14-9798e8534dc0")
IDXGIAdapter : public IDXGIObject
{
	public:
		virtual HRESULT STDMETHODCALLTYPE EnumOutputs(UINT Output, __out IDXGIOutput **ppOutput) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetDesc(__out DXGI_ADAPTER_DESC *pDesc) = 0;
		virtual HRESULT STDMETHODCALLTYPE CheckInterfaceSupport(__in REFGUID InterfaceName, __out LARGE_INTEGER *pUMDVersion) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "DXGI.h"
MIDL_INTERFACE("54ec77fa-1377-44e6-8c32-88fd5f44c84c")
IDXGIDevice : public IDXGIObject
{
	public:
		virtual HRESULT STDMETHODCALLTYPE GetAdapter(_COM_Outptr_ IDXGIAdapter **pAdapter) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSurface(_In_ const DXGI_SURFACE_DESC *pDesc, UINT NumSurfaces, DXGI_USAGE Usage, _In_opt_ const DXGI_SHARED_RESOURCE *pSharedResource, _COM_Outptr_ IDXGISurface **ppSurface) = 0;
		virtual HRESULT STDMETHODCALLTYPE QueryResourceResidency(_In_reads_(NumResources) IUnknown *const *ppResources, _Out_writes_(NumResources) DXGI_RESIDENCY *pResidencyStatus, UINT NumResources) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetGPUThreadPriority(INT Priority) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetGPUThreadPriority(_Out_ INT *pPriority) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
MIDL_INTERFACE("c4fec28f-7966-4e95-9f94-f431cb56c3b8")
ID3D12Object : public IUnknown
{
	public:
		virtual HRESULT STDMETHODCALLTYPE GetPrivateData(_In_ REFGUID guid, _Inout_ UINT *pDataSize, _Out_writes_bytes_opt_( *pDataSize ) void *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetPrivateData(_In_ REFGUID guid, _In_ UINT DataSize, _In_reads_bytes_opt_(DataSize) const void *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(_In_ REFGUID guid, _In_opt_ const IUnknown *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetName(_In_z_ LPCWSTR Name) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
MIDL_INTERFACE("905db94b-a00c-4140-9df5-2b64ca9ea357")
ID3D12DeviceChild : public ID3D12Object
{
	public:
		virtual HRESULT STDMETHODCALLTYPE GetDevice(REFIID riid, _COM_Outptr_opt_ void **ppvDevice) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
MIDL_INTERFACE("63ee58fb-1268-4835-86da-f008ce62f0d6")
ID3D12Pageable : public ID3D12DeviceChild
{
	// Nothing here
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
MIDL_INTERFACE("696442be-a72e-4059-bc79-5b5c98040fad")
ID3D12Resource : public ID3D12Pageable
{
	public:
		virtual HRESULT STDMETHODCALLTYPE Map(UINT Subresource, _In_opt_ const D3D12_RANGE *pReadRange, _Outptr_opt_result_bytebuffer_(_Inexpressible_("Dependent on resource")) void **ppData) = 0;
		virtual void STDMETHODCALLTYPE Unmap(UINT Subresource, _In_opt_ const D3D12_RANGE *pWrittenRange) = 0;
		virtual D3D12_RESOURCE_DESC STDMETHODCALLTYPE GetDesc(void) = 0;
		virtual D3D12_GPU_VIRTUAL_ADDRESS STDMETHODCALLTYPE GetGPUVirtualAddress( void) = 0;
		virtual HRESULT STDMETHODCALLTYPE WriteToSubresource(UINT DstSubresource, _In_opt_ const D3D12_BOX *pDstBox, _In_ const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch) = 0;
		virtual HRESULT STDMETHODCALLTYPE ReadFromSubresource(_Out_ void *pDstData, UINT DstRowPitch, UINT DstDepthPitch, UINT SrcSubresource, _In_opt_ const D3D12_BOX *pSrcBox) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetHeapProperties(_Out_opt_ D3D12_HEAP_PROPERTIES *pHeapProperties, _Out_opt_ D3D12_HEAP_FLAGS *pHeapFlags) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
MIDL_INTERFACE("8efb471d-616c-4f49-90f7-127bb763fa51")
ID3D12DescriptorHeap : public ID3D12Pageable
{
	public:
		virtual D3D12_DESCRIPTOR_HEAP_DESC STDMETHODCALLTYPE GetDesc(void) = 0;
		virtual D3D12_CPU_DESCRIPTOR_HANDLE STDMETHODCALLTYPE GetCPUDescriptorHandleForHeapStart(void) = 0;
		virtual D3D12_GPU_DESCRIPTOR_HANDLE STDMETHODCALLTYPE GetGPUDescriptorHandleForHeapStart(void) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
MIDL_INTERFACE("0ec870a6-5d7e-4c22-8cfc-5baae07616ed")
ID3D12CommandQueue : public ID3D12Pageable
{
	public:
		virtual void STDMETHODCALLTYPE UpdateTileMappings(_In_ ID3D12Resource *pResource, UINT NumResourceRegions, _In_reads_opt_(NumResourceRegions) const D3D12_TILED_RESOURCE_COORDINATE *pResourceRegionStartCoordinates, _In_reads_opt_(NumResourceRegions) const D3D12_TILE_REGION_SIZE *pResourceRegionSizes, _In_opt_ ID3D12Heap *pHeap, UINT NumRanges, _In_reads_opt_(NumRanges) const D3D12_TILE_RANGE_FLAGS *pRangeFlags, _In_reads_opt_(NumRanges) const UINT *pHeapRangeStartOffsets, _In_reads_opt_(NumRanges) const UINT *pRangeTileCounts, D3D12_TILE_MAPPING_FLAGS Flags) = 0;
		virtual void STDMETHODCALLTYPE CopyTileMappings(_In_ ID3D12Resource *pDstResource, _In_ const D3D12_TILED_RESOURCE_COORDINATE *pDstRegionStartCoordinate, _In_ ID3D12Resource *pSrcResource, _In_ const D3D12_TILED_RESOURCE_COORDINATE *pSrcRegionStartCoordinate, _In_ const D3D12_TILE_REGION_SIZE *pRegionSize, D3D12_TILE_MAPPING_FLAGS Flags) = 0;
		virtual void STDMETHODCALLTYPE ExecuteCommandLists(_In_ UINT NumCommandLists, _In_reads_(NumCommandLists) ID3D12CommandList *const *ppCommandLists) = 0;
		virtual void STDMETHODCALLTYPE SetMarker(UINT Metadata, _In_reads_bytes_opt_(Size) const void *pData, UINT Size) = 0;
		virtual void STDMETHODCALLTYPE BeginEvent(UINT Metadata, _In_reads_bytes_opt_(Size) const void *pData, UINT Size) = 0;
		virtual void STDMETHODCALLTYPE EndEvent(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE Signal(ID3D12Fence *pFence, UINT64 Value) = 0;
		virtual HRESULT STDMETHODCALLTYPE Wait(ID3D12Fence *pFence, UINT64 Value) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetTimestampFrequency(_Out_ UINT64 *pFrequency) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetClockCalibration(_Out_ UINT64 *pGpuTimestamp, _Out_ UINT64 *pCpuTimestamp) = 0;
		virtual D3D12_COMMAND_QUEUE_DESC STDMETHODCALLTYPE GetDesc(void) = 0;
};

// "Microsoft Windows 10 SDK" -> "10.0.10240.0" -> "D3D12.h"
MIDL_INTERFACE("189819f1-1db6-4b57-be54-1821339b85f7")
ID3D12Device : public ID3D12Object
{
	public:
		virtual UINT STDMETHODCALLTYPE GetNodeCount(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateCommandQueue(_In_ const D3D12_COMMAND_QUEUE_DESC *pDesc, REFIID riid, _COM_Outptr_ void **ppCommandQueue) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateCommandAllocator(_In_ D3D12_COMMAND_LIST_TYPE type, REFIID riid, _COM_Outptr_ void **ppCommandAllocator) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateGraphicsPipelineState(_In_ const D3D12_GRAPHICS_PIPELINE_STATE_DESC *pDesc, REFIID riid, _COM_Outptr_ void **ppPipelineState) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateComputePipelineState(_In_ const D3D12_COMPUTE_PIPELINE_STATE_DESC *pDesc, REFIID riid, _COM_Outptr_ void **ppPipelineState) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateCommandList(_In_ UINT nodeMask, _In_ D3D12_COMMAND_LIST_TYPE type, _In_ ID3D12CommandAllocator *pCommandAllocator, _In_opt_ ID3D12PipelineState *pInitialState, REFIID riid, _COM_Outptr_ void **ppCommandList) = 0;
		virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport(D3D12_FEATURE Feature, _Inout_updates_bytes_(FeatureSupportDataSize) void *pFeatureSupportData, UINT FeatureSupportDataSize) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateDescriptorHeap(_In_ const D3D12_DESCRIPTOR_HEAP_DESC *pDescriptorHeapDesc, REFIID riid, _COM_Outptr_ void **ppvHeap) = 0;
		virtual UINT STDMETHODCALLTYPE GetDescriptorHandleIncrementSize(_In_ D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateRootSignature(_In_ UINT nodeMask, _In_reads_(blobLengthInBytes) const void *pBlobWithRootSignature, _In_ SIZE_T blobLengthInBytes, REFIID riid, _COM_Outptr_ void **ppvRootSignature) = 0;
		virtual void STDMETHODCALLTYPE CreateConstantBufferView(_In_opt_ const D3D12_CONSTANT_BUFFER_VIEW_DESC *pDesc, _In_ D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) = 0;
		virtual void STDMETHODCALLTYPE CreateShaderResourceView(_In_opt_ ID3D12Resource *pResource, _In_opt_ const D3D12_SHADER_RESOURCE_VIEW_DESC *pDesc, _In_ D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) = 0;
		virtual void STDMETHODCALLTYPE CreateUnorderedAccessView(_In_opt_ ID3D12Resource *pResource, _In_opt_ ID3D12Resource *pCounterResource, _In_opt_ const D3D12_UNORDERED_ACCESS_VIEW_DESC *pDesc, _In_ D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) = 0;
		virtual void STDMETHODCALLTYPE CreateRenderTargetView(_In_opt_ ID3D12Resource *pResource, _In_opt_ const D3D12_RENDER_TARGET_VIEW_DESC *pDesc, _In_ D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) = 0;
		virtual void STDMETHODCALLTYPE CreateDepthStencilView(_In_opt_ ID3D12Resource *pResource, _In_opt_ const D3D12_DEPTH_STENCIL_VIEW_DESC *pDesc, _In_ D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) = 0;
		virtual void STDMETHODCALLTYPE CreateSampler(_In_ const D3D12_SAMPLER_DESC *pDesc, _In_ D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) = 0;
		virtual void STDMETHODCALLTYPE CopyDescriptors(_In_ UINT NumDestDescriptorRanges, _In_reads_(NumDestDescriptorRanges) const D3D12_CPU_DESCRIPTOR_HANDLE *pDestDescriptorRangeStarts, _In_reads_opt_(NumDestDescriptorRanges) const UINT *pDestDescriptorRangeSizes, _In_ UINT NumSrcDescriptorRanges, _In_reads_(NumSrcDescriptorRanges) const D3D12_CPU_DESCRIPTOR_HANDLE *pSrcDescriptorRangeStarts, _In_reads_opt_(NumSrcDescriptorRanges) const UINT *pSrcDescriptorRangeSizes, _In_ D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapsType) = 0;
		virtual void STDMETHODCALLTYPE CopyDescriptorsSimple(_In_ UINT NumDescriptors, _In_ D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorRangeStart, _In_ D3D12_CPU_DESCRIPTOR_HANDLE SrcDescriptorRangeStart, _In_ D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapsType) = 0;
		virtual D3D12_RESOURCE_ALLOCATION_INFO STDMETHODCALLTYPE GetResourceAllocationInfo(_In_ UINT visibleMask, _In_ UINT numResourceDescs, _In_reads_(numResourceDescs) const D3D12_RESOURCE_DESC *pResourceDescs) = 0;
		virtual D3D12_HEAP_PROPERTIES STDMETHODCALLTYPE GetCustomHeapProperties(_In_ UINT nodeMask, D3D12_HEAP_TYPE heapType) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateCommittedResource(_In_ const D3D12_HEAP_PROPERTIES *pHeapProperties, D3D12_HEAP_FLAGS HeapFlags, _In_ const D3D12_RESOURCE_DESC *pResourceDesc, D3D12_RESOURCE_STATES InitialResourceState, _In_opt_ const D3D12_CLEAR_VALUE *pOptimizedClearValue, REFIID riidResource, _COM_Outptr_opt_ void **ppvResource) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateHeap(_In_ const D3D12_HEAP_DESC *pDesc, REFIID riid, _COM_Outptr_opt_ void **ppvHeap) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreatePlacedResource(_In_ ID3D12Heap *pHeap, UINT64 HeapOffset, _In_ const D3D12_RESOURCE_DESC *pDesc, D3D12_RESOURCE_STATES InitialState, _In_opt_ const D3D12_CLEAR_VALUE *pOptimizedClearValue, REFIID riid, _COM_Outptr_opt_ void **ppvResource) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateReservedResource(_In_ const D3D12_RESOURCE_DESC *pDesc, D3D12_RESOURCE_STATES InitialState, _In_opt_ const D3D12_CLEAR_VALUE *pOptimizedClearValue, REFIID riid, _COM_Outptr_opt_ void **ppvResource) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSharedHandle(_In_ ID3D12DeviceChild *pObject, _In_opt_ const SECURITY_ATTRIBUTES *pAttributes, DWORD Access, _In_opt_ LPCWSTR Name, _Out_ HANDLE *pHandle) = 0;
		virtual HRESULT STDMETHODCALLTYPE OpenSharedHandle(_In_ HANDLE NTHandle, REFIID riid, _COM_Outptr_opt_ void **ppvObj) = 0;
		virtual HRESULT STDMETHODCALLTYPE OpenSharedHandleByName(_In_ LPCWSTR Name, DWORD Access, _Out_ HANDLE *pNTHandle) = 0;
		virtual HRESULT STDMETHODCALLTYPE MakeResident(UINT NumObjects, _In_reads_(NumObjects) ID3D12Pageable *const *ppObjects) = 0;
		virtual HRESULT STDMETHODCALLTYPE Evict(UINT NumObjects, _In_reads_(NumObjects) ID3D12Pageable *const *ppObjects) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateFence(UINT64 InitialValue, D3D12_FENCE_FLAGS Flags, REFIID riid, _COM_Outptr_ void **ppFence) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason(void) = 0;
		virtual void STDMETHODCALLTYPE GetCopyableFootprints(_In_ const D3D12_RESOURCE_DESC *pResourceDesc, _In_range_(0,D3D12_REQ_SUBRESOURCES) UINT FirstSubresource, _In_range_(0,D3D12_REQ_SUBRESOURCES-FirstSubresource) UINT NumSubresources, UINT64 BaseOffset, _Out_writes_opt_(NumSubresources) D3D12_PLACED_SUBRESOURCE_FOOTPRINT *pLayouts, _Out_writes_opt_(NumSubresources) UINT *pNumRows, _Out_writes_opt_(NumSubresources) UINT64 *pRowSizeInBytes, _Out_opt_ UINT64 *pTotalBytes) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateQueryHeap(_In_ const D3D12_QUERY_HEAP_DESC *pDesc, REFIID riid, _COM_Outptr_opt_ void **ppvHeap) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetStablePowerState(BOOL Enable) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateCommandSignature(_In_ const D3D12_COMMAND_SIGNATURE_DESC *pDesc, _In_opt_ ID3D12RootSignature *pRootSignature, REFIID riid, _COM_Outptr_opt_ void **ppvCommandSignature) = 0;
		virtual void STDMETHODCALLTYPE GetResourceTiling(_In_ ID3D12Resource *pTiledResource, _Out_opt_ UINT *pNumTilesForEntireResource, _Out_opt_ D3D12_PACKED_MIP_INFO *pPackedMipDesc, _Out_opt_ D3D12_TILE_SHAPE *pStandardTileShapeForNonPackedMips, _Inout_opt_ UINT *pNumSubresourceTilings, _In_ UINT FirstSubresourceTilingToGet, _Out_writes_(*pNumSubresourceTilings) D3D12_SUBRESOURCE_TILING *pSubresourceTilingsForNonPackedMips) = 0;
		virtual LUID STDMETHODCALLTYPE GetAdapterLuid(void) = 0;
};
