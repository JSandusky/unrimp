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


/*
  We don't use the Direct3D headers from the DirectX SDK because there are several issues:
  - Licensing: It's not allowed to redistribute the Direct3D headers, meaning everyone would
    have to get them somehow before compiling this project
  - The Direct3D headers are somewhat chaotic and include tons of other headers.
    This slows down compilation and the more headers are included, the higher the risk of
    naming or redefinition conflicts.
  - Starting with Windows 8, Direct3D is part of the Windows SDK. When using VisualStudio 2012
    and the Direct3D headers from "Microsoft DirectX SDK (June 2010)" you will get a lot of
    "
      <path>\external\directx\include\dxgitype.h(12): warning C4005: 'DXGI_STATUS_OCCLUDED' : macro redefinition
      c:\program files (x86)\windows kits\8.0\include\shared\winerror.h(49449) : see previous definition of 'DXGI_STATUS_OCCLUDED'
    "
    warnings.

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
struct IDXGIOutput;
struct ID3D10Query;
struct IDXGIDevice;
struct IDXGISurface;
struct IDXGIAdapter;
struct ID3D10Buffer;
struct ID3D10Counter;
struct ID3D10Resource;
struct IDXGISwapChain;
struct ID3D10Texture1D;
struct ID3D10Texture2D;
struct ID3D10Texture3D;
struct ID3D10Predicate;
struct ID3D10BlendState;
struct D3D10_BLEND_DESC;
struct D3D10_QUERY_DESC;
struct ID3DX10ThreadPump;
struct ID3D10PixelShader;
struct D3D10_BUFFER_DESC;
struct ID3D10InputLayout;
struct D3D10_COUNTER_INFO;
struct D3D10_COUNTER_TYPE;
struct D3D10_COUNTER_DESC;
struct ID3D10Asynchronous;
struct ID3D10VertexShader;
struct D3D10_SAMPLER_DESC;
struct ID3D10SamplerState;
struct D3D10_TEXTURE1D_DESC;
struct D3D10_TEXTURE2D_DESC;
struct D3D10_TEXTURE3D_DESC;
struct ID3D10GeometryShader;
struct DXGI_FRAME_STATISTICS;
struct ID3D10RasterizerState;
struct D3D10_SUBRESOURCE_DATA;
struct ID3D10RenderTargetView;
struct ID3D10DepthStencilView;
struct ID3D10DepthStencilState;
struct D3D10_DEPTH_STENCIL_DESC;
struct ID3D10ShaderResourceView;
struct D3D10_RESOURCE_DIMENSION;
struct D3D10_SO_DECLARATION_ENTRY;
struct D3D10_DEPTH_STENCIL_VIEW_DESC;
struct D3D10_RENDER_TARGET_VIEW_DESC;
struct D3D10_SHADER_RESOURCE_VIEW_DESC;


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
// "Microsoft DirectX SDK (June 2010)" -> "D3Dcompiler.h"
#define D3DCOMPILE_DEBUG				(1 << 0)
#define D3DCOMPILE_ENABLE_STRICTNESS	(1 << 11)

// "Microsoft DirectX SDK (June 2010)" -> "d3d9types.h"
typedef DWORD D3DCOLOR;

// "Microsoft DirectX SDK (June 2010)" -> "d3d9types.h"
#define D3DCOLOR_ARGB(a,r,g,b) ((D3DCOLOR)((((a)&0xff)<<24) | (((r)&0xff)<<16) | (((g)&0xff)<<8) | ((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a, r, g, b)

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
#define D3D10_SDK_VERSION							(29)
#define	D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX	(15)
#define	D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT		(8)

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
#define	D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT	(16)

// "Microsoft DirectX SDK (June 2010)" -> "D3DX10.h"
#define D3DX10_DEFAULT	(static_cast<UINT>(-1))

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
#define DXGI_MWA_NO_ALT_ENTER	(1 << 1)

// "Microsoft DirectX SDK (June 2010)" -> "DXGIType.h"
#define DXGI_USAGE_RENDER_TARGET_OUTPUT	(1L << (1 + 4))

// "Microsoft DirectX SDK (June 2010)" -> "d3d10misc.h"
enum D3D10_DRIVER_TYPE
{
	D3D10_DRIVER_TYPE_HARDWARE  = 0,
	D3D10_DRIVER_TYPE_REFERENCE = 1,
	D3D10_DRIVER_TYPE_NULL      = 2,
	D3D10_DRIVER_TYPE_SOFTWARE  = 3,
	D3D10_DRIVER_TYPE_WARP      = 5
};

// "Microsoft DirectX SDK (June 2010)" -> "D3Dcommon.h"
enum D3D_SRV_DIMENSION
{
	D3D_SRV_DIMENSION_UNKNOWN				= 0,
	D3D_SRV_DIMENSION_BUFFER				= 1,
	D3D_SRV_DIMENSION_TEXTURE1D				= 2,
	D3D_SRV_DIMENSION_TEXTURE1DARRAY		= 3,
	D3D_SRV_DIMENSION_TEXTURE2D				= 4,
	D3D_SRV_DIMENSION_TEXTURE2DARRAY		= 5,
	D3D_SRV_DIMENSION_TEXTURE2DMS			= 6,
	D3D_SRV_DIMENSION_TEXTURE2DMSARRAY		= 7,
	D3D_SRV_DIMENSION_TEXTURE3D				= 8,
	D3D_SRV_DIMENSION_TEXTURECUBE			= 9,
	D3D10_SRV_DIMENSION_UNKNOWN				= D3D_SRV_DIMENSION_UNKNOWN,
	D3D10_SRV_DIMENSION_BUFFER				= D3D_SRV_DIMENSION_BUFFER,
	D3D10_SRV_DIMENSION_TEXTURE1D			= D3D_SRV_DIMENSION_TEXTURE1D,
	D3D10_SRV_DIMENSION_TEXTURE1DARRAY		= D3D_SRV_DIMENSION_TEXTURE1DARRAY,
	D3D10_SRV_DIMENSION_TEXTURE2D			= D3D_SRV_DIMENSION_TEXTURE2D,
	D3D10_SRV_DIMENSION_TEXTURE2DARRAY		= D3D_SRV_DIMENSION_TEXTURE2DARRAY,
	D3D10_SRV_DIMENSION_TEXTURE2DMS			= D3D_SRV_DIMENSION_TEXTURE2DMS,
	D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY	= D3D_SRV_DIMENSION_TEXTURE2DMSARRAY,
	D3D10_SRV_DIMENSION_TEXTURE3D			= D3D_SRV_DIMENSION_TEXTURE3D,
	D3D10_SRV_DIMENSION_TEXTURECUBE			= D3D_SRV_DIMENSION_TEXTURECUBE
};
typedef D3D_SRV_DIMENSION D3D10_SRV_DIMENSION;

// "Microsoft DirectX SDK (June 2010)" -> "D3Dcommon.h"
enum D3D10_PRIMITIVE_TOPOLOGY
{
	D3D_PRIMITIVE_TOPOLOGY_UNDEFINED					= 0,
	D3D_PRIMITIVE_TOPOLOGY_POINTLIST					= 1,
	D3D_PRIMITIVE_TOPOLOGY_LINELIST						= 2,
	D3D_PRIMITIVE_TOPOLOGY_LINESTRIP					= 3,
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST					= 4,
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP				= 5,
	D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ					= 10,
	D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ				= 11,
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ				= 12,
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ			= 13,
	D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED					= D3D_PRIMITIVE_TOPOLOGY_UNDEFINED,
	D3D10_PRIMITIVE_TOPOLOGY_POINTLIST					= D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
	D3D10_PRIMITIVE_TOPOLOGY_LINELIST					= D3D_PRIMITIVE_TOPOLOGY_LINELIST,
	D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP					= D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
	D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST				= D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP				= D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ				= D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
	D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ				= D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
	D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ			= D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
	D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ			= D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
};

// "Microsoft DirectX SDK (June 2010)" -> "D3Dcommon.h"
struct D3D_SHADER_MACRO
{
	LPCSTR Name;
	LPCSTR Definition;
};
enum D3D_INCLUDE_TYPE
{
	D3D_INCLUDE_LOCAL		= 0,
	D3D_INCLUDE_SYSTEM		= (D3D_INCLUDE_LOCAL + 1),
	D3D10_INCLUDE_LOCAL		= D3D_INCLUDE_LOCAL,
	D3D10_INCLUDE_SYSTEM	= D3D_INCLUDE_SYSTEM,
	D3D_INCLUDE_FORCE_DWORD	= 0x7fffffff
};
DECLARE_INTERFACE(ID3DInclude)
{
	STDMETHOD(Open)(THIS_ D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) PURE;
	STDMETHOD(Close)(THIS_ LPCVOID pData) PURE;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10shader.h"
typedef D3D_SHADER_MACRO D3D10_SHADER_MACRO;
typedef __interface ID3DInclude *LPD3D10INCLUDE;

// "Microsoft DirectX SDK (June 2010)" -> "DXGIFormat.h"
enum DXGI_FORMAT
{
	DXGI_FORMAT_UNKNOWN	                    = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT          = 2,
	DXGI_FORMAT_R32G32B32A32_UINT           = 3,
	DXGI_FORMAT_R32G32B32A32_SINT           = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS          = 5,
	DXGI_FORMAT_R32G32B32_FLOAT             = 6,
	DXGI_FORMAT_R32G32B32_UINT              = 7,
	DXGI_FORMAT_R32G32B32_SINT              = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT          = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM          = 11,
	DXGI_FORMAT_R16G16B16A16_UINT           = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM          = 13,
	DXGI_FORMAT_R16G16B16A16_SINT           = 14,
	DXGI_FORMAT_R32G32_TYPELESS             = 15,
	DXGI_FORMAT_R32G32_FLOAT                = 16,
	DXGI_FORMAT_R32G32_UINT                 = 17,
	DXGI_FORMAT_R32G32_SINT                 = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS           = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM           = 24,
	DXGI_FORMAT_R10G10B10A2_UINT            = 25,
	DXGI_FORMAT_R11G11B10_FLOAT             = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM              = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
	DXGI_FORMAT_R8G8B8A8_UINT               = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM              = 31,
	DXGI_FORMAT_R8G8B8A8_SINT               = 32,
	DXGI_FORMAT_R16G16_TYPELESS             = 33,
	DXGI_FORMAT_R16G16_FLOAT                = 34,
	DXGI_FORMAT_R16G16_UNORM                = 35,
	DXGI_FORMAT_R16G16_UINT                 = 36,
	DXGI_FORMAT_R16G16_SNORM                = 37,
	DXGI_FORMAT_R16G16_SINT                 = 38,
	DXGI_FORMAT_R32_TYPELESS                = 39,
	DXGI_FORMAT_D32_FLOAT                   = 40,
	DXGI_FORMAT_R32_FLOAT                   = 41,
	DXGI_FORMAT_R32_UINT                    = 42,
	DXGI_FORMAT_R32_SINT                    = 43,
	DXGI_FORMAT_R24G8_TYPELESS              = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT           = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
	DXGI_FORMAT_R8G8_TYPELESS               = 48,
	DXGI_FORMAT_R8G8_UNORM                  = 49,
	DXGI_FORMAT_R8G8_UINT                   = 50,
	DXGI_FORMAT_R8G8_SNORM                  = 51,
	DXGI_FORMAT_R8G8_SINT                   = 52,
	DXGI_FORMAT_R16_TYPELESS                = 53,
	DXGI_FORMAT_R16_FLOAT                   = 54,
	DXGI_FORMAT_D16_UNORM                   = 55,
	DXGI_FORMAT_R16_UNORM                   = 56,
	DXGI_FORMAT_R16_UINT                    = 57,
	DXGI_FORMAT_R16_SNORM                   = 58,
	DXGI_FORMAT_R16_SINT                    = 59,
	DXGI_FORMAT_R8_TYPELESS                 = 60,
	DXGI_FORMAT_R8_UNORM                    = 61,
	DXGI_FORMAT_R8_UINT                     = 62,
	DXGI_FORMAT_R8_SNORM                    = 63,
	DXGI_FORMAT_R8_SINT                     = 64,
	DXGI_FORMAT_A8_UNORM                    = 65,
	DXGI_FORMAT_R1_UNORM                    = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM             = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM             = 69,
	DXGI_FORMAT_BC1_TYPELESS                = 70,
	DXGI_FORMAT_BC1_UNORM                   = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB              = 72,
	DXGI_FORMAT_BC2_TYPELESS                = 73,
	DXGI_FORMAT_BC2_UNORM                   = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB              = 75,
	DXGI_FORMAT_BC3_TYPELESS                = 76,
	DXGI_FORMAT_BC3_UNORM                   = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB              = 78,
	DXGI_FORMAT_BC4_TYPELESS                = 79,
	DXGI_FORMAT_BC4_UNORM                   = 80,
	DXGI_FORMAT_BC4_SNORM                   = 81,
	DXGI_FORMAT_BC5_TYPELESS                = 82,
	DXGI_FORMAT_BC5_UNORM                   = 83,
	DXGI_FORMAT_BC5_SNORM                   = 84,
	DXGI_FORMAT_B5G6R5_UNORM                = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM              = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM              = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM              = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
	DXGI_FORMAT_BC6H_TYPELESS               = 94,
	DXGI_FORMAT_BC6H_UF16                   = 95,
	DXGI_FORMAT_BC6H_SF16                   = 96,
	DXGI_FORMAT_BC7_TYPELESS                = 97,
	DXGI_FORMAT_BC7_UNORM                   = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB              = 99,
	DXGI_FORMAT_FORCE_UINT                  = 0xffffffff
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
inline UINT D3D10CalcSubresource(UINT MipSlice, UINT ArraySlice, UINT MipLevels)
{
	return MipSlice + ArraySlice * MipLevels;
}

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_CREATE_DEVICE_FLAG
{
	D3D10_CREATE_DEVICE_SINGLETHREADED								= 0x1,
	D3D10_CREATE_DEVICE_DEBUG										= 0x2,
	D3D10_CREATE_DEVICE_SWITCH_TO_REF								= 0x4,
	D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS	= 0x8,
	D3D10_CREATE_DEVICE_ALLOW_NULL_FROM_MAP							= 0x10,
	D3D10_CREATE_DEVICE_BGRA_SUPPORT								= 0x20,
	D3D10_CREATE_DEVICE_STRICT_VALIDATION							= 0x200
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_BOX
{
	UINT left;
	UINT top;
	UINT front;
	UINT right;
	UINT bottom;
	UINT back;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_VIEWPORT
{
	INT TopLeftX;
	INT TopLeftY;
	UINT Width;
	UINT Height;
	FLOAT MinDepth;
	FLOAT MaxDepth;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
typedef RECT D3D10_RECT;

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_RTV_DIMENSION
{
	D3D10_RTV_DIMENSION_UNKNOWN				= 0,
	D3D10_RTV_DIMENSION_BUFFER				= 1,
	D3D10_RTV_DIMENSION_TEXTURE1D			= 2,
	D3D10_RTV_DIMENSION_TEXTURE1DARRAY		= 3,
	D3D10_RTV_DIMENSION_TEXTURE2D			= 4,
	D3D10_RTV_DIMENSION_TEXTURE2DARRAY		= 5,
	D3D10_RTV_DIMENSION_TEXTURE2DMS			= 6,
	D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY	= 7,
	D3D10_RTV_DIMENSION_TEXTURE3D			= 8
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_DSV_DIMENSION
{
	D3D10_DSV_DIMENSION_UNKNOWN				= 0,
	D3D10_DSV_DIMENSION_TEXTURE1D			= 1,
	D3D10_DSV_DIMENSION_TEXTURE1DARRAY		= 2,
	D3D10_DSV_DIMENSION_TEXTURE2D			= 3,
	D3D10_DSV_DIMENSION_TEXTURE2DARRAY		= 4,
	D3D10_DSV_DIMENSION_TEXTURE2DMS			= 5,
	D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY	= 6
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGIType.h"
struct DXGI_SAMPLE_DESC
{
	UINT Count;
	UINT Quality;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
struct IDXGIObject : public IUnknown
{
	public:
		virtual HRESULT STDMETHODCALLTYPE SetPrivateData(__in REFGUID Name, UINT DataSize, __in_bcount(DataSize) const void *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(__in REFGUID Name, __in const IUnknown *pUnknown) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetPrivateData(__in REFGUID Name, __inout UINT *pDataSize, __out_bcount(*pDataSize) void *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetParent(__in REFIID riid, __out void **ppParent) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGIType.h"
struct DXGI_RATIONAL
{
	UINT Numerator;
	UINT Denominator;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGIType.h"
enum DXGI_MODE_SCANLINE_ORDER
{
	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED		= 0,
	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE		= 1,
	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST	= 2,
	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST	= 3
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGIType.h"
enum DXGI_MODE_SCALING
{
	DXGI_MODE_SCALING_UNSPECIFIED	= 0,
	DXGI_MODE_SCALING_CENTERED		= 1,
	DXGI_MODE_SCALING_STRETCHED		= 2
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGIType.h"
struct DXGI_MODE_DESC
{
	UINT Width;
	UINT Height;
	DXGI_RATIONAL RefreshRate;
	DXGI_FORMAT Format;
	DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
	DXGI_MODE_SCALING Scaling;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
typedef UINT DXGI_USAGE;

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
enum DXGI_SWAP_EFFECT
{
	DXGI_SWAP_EFFECT_DISCARD	= 0,
	DXGI_SWAP_EFFECT_SEQUENTIAL	= 1
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
struct DXGI_SWAP_CHAIN_DESC
{
	DXGI_MODE_DESC BufferDesc;
	DXGI_SAMPLE_DESC SampleDesc;
	DXGI_USAGE BufferUsage;
	UINT BufferCount;
	HWND OutputWindow;
	BOOL Windowed;
	DXGI_SWAP_EFFECT SwapEffect;
	UINT Flags;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
MIDL_INTERFACE("7b7166ec-21c7-44ae-b21a-c9ae321ae369")
IDXGIFactory : public IDXGIObject
{
	public:
		virtual HRESULT STDMETHODCALLTYPE EnumAdapters(UINT Adapter, __out IDXGIAdapter **ppAdapter) = 0;
		virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation(HWND WindowHandle, UINT Flags) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation(__out HWND *pWindowHandle) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(__in IUnknown *pDevice, __in DXGI_SWAP_CHAIN_DESC *pDesc, __out IDXGISwapChain **ppSwapChain) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(HMODULE Module, __out IDXGIAdapter **ppAdapter) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
struct DXGI_ADAPTER_DESC
{
	WCHAR Description[128];
	UINT VendorId;
	UINT DeviceId;
	UINT SubSysId;
	UINT Revision;
	SIZE_T DedicatedVideoMemory;
	SIZE_T DedicatedSystemMemory;
	SIZE_T SharedSystemMemory;
	LUID AdapterLuid;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
struct IDXGIAdapter : public IDXGIObject
{
	public:
		virtual HRESULT STDMETHODCALLTYPE EnumOutputs(UINT Output, __out IDXGIOutput **ppOutput) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetDesc(__out DXGI_ADAPTER_DESC *pDesc) = 0;
		virtual HRESULT STDMETHODCALLTYPE CheckInterfaceSupport(__in REFGUID InterfaceName, __out LARGE_INTEGER *pUMDVersion) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
struct DXGI_SURFACE_DESC
{
	UINT Width;
	UINT Height;
	DXGI_FORMAT Format;
	DXGI_SAMPLE_DESC SampleDesc;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
struct DXGI_SHARED_RESOURCE
{
	HANDLE Handle;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
enum DXGI_RESIDENCY
{
	DXGI_RESIDENCY_FULLY_RESIDENT				= 1,
	DXGI_RESIDENCY_RESIDENT_IN_SHARED_MEMORY	= 2,
	DXGI_RESIDENCY_EVICTED_TO_DISK				= 3
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
MIDL_INTERFACE("54ec77fa-1377-44e6-8c32-88fd5f44c84c")
IDXGIDevice : public IDXGIObject
{
	public:
		virtual HRESULT STDMETHODCALLTYPE GetAdapter(__out IDXGIAdapter **pAdapter) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSurface(__in const DXGI_SURFACE_DESC *pDesc, UINT NumSurfaces, DXGI_USAGE Usage, __in_opt const DXGI_SHARED_RESOURCE *pSharedResource, __out IDXGISurface **ppSurface) = 0;
		virtual HRESULT STDMETHODCALLTYPE QueryResourceResidency(__in_ecount(NumResources) IUnknown *const *ppResources, __out_ecount(NumResources) DXGI_RESIDENCY *pResidencyStatus, UINT NumResources) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetGPUThreadPriority(INT Priority) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetGPUThreadPriority(__out INT *pPriority) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
struct IDXGIDeviceSubObject : public IDXGIObject
{
	public:
		virtual HRESULT STDMETHODCALLTYPE GetDevice(__in REFIID riid, __out void **ppDevice) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "DXGI.h"
struct IDXGISwapChain : public IDXGIDeviceSubObject
{
	public:
		virtual HRESULT STDMETHODCALLTYPE Present(UINT SyncInterval, UINT Flags) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetBuffer(UINT Buffer, __in REFIID riid, __out void **ppSurface) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL Fullscreen, __in_opt IDXGIOutput *pTarget) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetFullscreenState(__out BOOL *pFullscreen, __out IDXGIOutput **ppTarget) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetDesc(__out DXGI_SWAP_CHAIN_DESC *pDesc) = 0;
		virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = 0;
		virtual HRESULT STDMETHODCALLTYPE ResizeTarget(__in const DXGI_MODE_DESC *pNewTargetParameters) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetContainingOutput(__out IDXGIOutput **ppOutput) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(__out DXGI_FRAME_STATISTICS *pStats) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(__out UINT *pLastPresentCount) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_CLEAR_FLAG
{
	D3D10_CLEAR_DEPTH	= 0x1L,
	D3D10_CLEAR_STENCIL	= 0x2L
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_USAGE
{
	D3D10_USAGE_DEFAULT		= 0,
	D3D10_USAGE_IMMUTABLE	= 1,
	D3D10_USAGE_DYNAMIC		= 2,
	D3D10_USAGE_STAGING		= 3
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_MAP
{
	D3D10_MAP_READ					= 1,
	D3D10_MAP_WRITE					= 2,
	D3D10_MAP_READ_WRITE			= 3,
	D3D10_MAP_WRITE_DISCARD			= 4,
	D3D10_MAP_WRITE_NO_OVERWRITE	= 5
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_BIND_FLAG
{
	D3D10_BIND_VERTEX_BUFFER	= 0x1L,
	D3D10_BIND_INDEX_BUFFER		= 0x2L,
	D3D10_BIND_CONSTANT_BUFFER	= 0x4L,
	D3D10_BIND_SHADER_RESOURCE	= 0x8L,
	D3D10_BIND_STREAM_OUTPUT	= 0x10L,
	D3D10_BIND_RENDER_TARGET	= 0x20L,
	D3D10_BIND_DEPTH_STENCIL	= 0x40L
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_CPU_ACCESS_FLAG
{
	D3D10_CPU_ACCESS_WRITE	= 0x10000L,
	D3D10_CPU_ACCESS_READ	= 0x20000L
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_BUFFER_DESC
{
	UINT ByteWidth;
	D3D10_USAGE Usage;
	UINT BindFlags;
	UINT CPUAccessFlags;
	UINT MiscFlags;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_BUFFER_RTV
{
	union
	{
		UINT FirstElement;
		UINT ElementOffset;
	};
	union
	{
		UINT NumElements;
		UINT ElementWidth;
	};
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX1D_RTV
{
	UINT MipSlice;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX1D_ARRAY_RTV
{
	UINT MipSlice;
	UINT FirstArraySlice;
	UINT ArraySize;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2D_RTV
{
	UINT MipSlice;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2DMS_RTV
{
	UINT UnusedField_NothingToDefine;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2D_ARRAY_RTV
{
	UINT MipSlice;
	UINT FirstArraySlice;
	UINT ArraySize;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2DMS_ARRAY_RTV
{
	UINT FirstArraySlice;
	UINT ArraySize;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX3D_RTV
{
	UINT MipSlice;
	UINT FirstWSlice;
	UINT WSize;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEXTURE2D_DESC
{
	UINT Width;
	UINT Height;
	UINT MipLevels;
	UINT ArraySize;
	DXGI_FORMAT Format;
	DXGI_SAMPLE_DESC SampleDesc;
	D3D10_USAGE Usage;
	UINT BindFlags;
	UINT CPUAccessFlags;
	UINT MiscFlags;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX1D_DSV
{
	UINT MipSlice;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX1D_ARRAY_DSV
{
	UINT MipSlice;
	UINT FirstArraySlice;
	UINT ArraySize;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2D_DSV
{
	UINT MipSlice;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2D_ARRAY_DSV
{
	UINT MipSlice;
	UINT FirstArraySlice;
	UINT ArraySize;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2DMS_DSV
{
	UINT UnusedField_NothingToDefine;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2DMS_ARRAY_DSV
{
	UINT FirstArraySlice;
	UINT ArraySize;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_SUBRESOURCE_DATA
{
	const void *pSysMem;
	UINT SysMemPitch;
	UINT SysMemSlicePitch;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_MAPPED_TEXTURE2D
{
	void *pData;
	UINT RowPitch;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_BUFFER_SRV
{
	union
	{
		UINT FirstElement;
		UINT ElementOffset;
	};
	union
	{
		UINT NumElements;
		UINT ElementWidth;
	};
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX1D_SRV
{
	UINT MostDetailedMip;
	UINT MipLevels;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX1D_ARRAY_SRV
{
	UINT MostDetailedMip;
	UINT MipLevels;
	UINT FirstArraySlice;
	UINT ArraySize;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D01.h"
struct D3D10_TEX2D_SRV
{
	UINT MostDetailedMip;
	UINT MipLevels;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2D_ARRAY_SRV
{
	UINT MostDetailedMip;
	UINT MipLevels;
	UINT FirstArraySlice;
	UINT ArraySize;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX3D_SRV
{
	UINT MostDetailedMip;
	UINT MipLevels;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEXCUBE_SRV
{
	UINT MostDetailedMip;
	UINT MipLevels;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2DMS_SRV
{
	UINT UnusedField_NothingToDefine;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_TEX2DMS_ARRAY_SRV
{
	UINT FirstArraySlice;
	UINT ArraySize;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_SHADER_RESOURCE_VIEW_DESC
{
	DXGI_FORMAT Format;
	D3D10_SRV_DIMENSION ViewDimension;
	union
	{
		D3D10_BUFFER_SRV Buffer;
		D3D10_TEX1D_SRV Texture1D;
		D3D10_TEX1D_ARRAY_SRV Texture1DArray;
		D3D10_TEX2D_SRV Texture2D;
		D3D10_TEX2D_ARRAY_SRV Texture2DArray;
		D3D10_TEX2DMS_SRV Texture2DMS;
		D3D10_TEX2DMS_ARRAY_SRV Texture2DMSArray;
		D3D10_TEX3D_SRV Texture3D;
		D3D10_TEXCUBE_SRV TextureCube;
	};
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_DEPTH_STENCIL_VIEW_DESC
{
	DXGI_FORMAT Format;
	D3D10_DSV_DIMENSION ViewDimension;
	UINT Flags;
	union
	{
		D3D10_TEX1D_DSV Texture1D;
		D3D10_TEX1D_ARRAY_DSV Texture1DArray;
		D3D10_TEX2D_DSV Texture2D;
		D3D10_TEX2D_ARRAY_DSV Texture2DArray;
		D3D10_TEX2DMS_DSV Texture2DMS;
		D3D10_TEX2DMS_ARRAY_DSV Texture2DMSArray;
	};
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_RENDER_TARGET_VIEW_DESC
{
	DXGI_FORMAT Format;
	D3D10_RTV_DIMENSION ViewDimension;
	union
	{
		D3D10_BUFFER_RTV Buffer;
		D3D10_TEX1D_RTV Texture1D;
		D3D10_TEX1D_ARRAY_RTV Texture1DArray;
		D3D10_TEX2D_RTV Texture2D;
		D3D10_TEX2D_ARRAY_RTV Texture2DArray;
		D3D10_TEX2DMS_RTV Texture2DMS;
		D3D10_TEX2DMS_ARRAY_RTV Texture2DMSArray;
		D3D10_TEX3D_RTV Texture3D;
	};
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_INPUT_CLASSIFICATION
{
	D3D10_INPUT_PER_VERTEX_DATA		= 0,
	D3D10_INPUT_PER_INSTANCE_DATA	= 1
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_INPUT_ELEMENT_DESC
{
	LPCSTR SemanticName;
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	D3D10_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
typedef enum D3D10_FILL_MODE
{
	D3D10_FILL_WIREFRAME	= 2,
	D3D10_FILL_SOLID		= 3
} D3D10_FILL_MODE;

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
typedef enum D3D10_CULL_MODE
{
	D3D10_CULL_NONE		= 1,
	D3D10_CULL_FRONT	= 2,
	D3D10_CULL_BACK		= 3 
} D3D10_CULL_MODE;

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
typedef struct D3D10_RASTERIZER_DESC
{
	D3D10_FILL_MODE FillMode;
	D3D10_CULL_MODE CullMode;
	BOOL            FrontCounterClockwise;
	INT             DepthBias;
	FLOAT           DepthBiasClamp;
	FLOAT           SlopeScaledDepthBias;
	BOOL            DepthClipEnable;
	BOOL            ScissorEnable;
	BOOL            MultisampleEnable;
	BOOL            AntialiasedLineEnable;
} D3D10_RASTERIZER_DESC;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10Device : public IUnknown
{
	public:
		virtual void STDMETHODCALLTYPE VSSetConstantBuffers(__in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot) UINT NumBuffers, __in_ecount(NumBuffers) ID3D10Buffer *const *ppConstantBuffers) = 0;
		virtual void STDMETHODCALLTYPE PSSetShaderResources(__in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot) UINT NumViews, __in_ecount(NumViews) ID3D10ShaderResourceView *const *ppShaderResourceViews) = 0;
		virtual void STDMETHODCALLTYPE PSSetShader(__in_opt ID3D10PixelShader *pPixelShader) = 0;
		virtual void STDMETHODCALLTYPE PSSetSamplers(__in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot) UINT NumSamplers, __in_ecount(NumSamplers) ID3D10SamplerState *const *ppSamplers) = 0;
		virtual void STDMETHODCALLTYPE VSSetShader(__in_opt ID3D10VertexShader *pVertexShader) = 0;
		virtual void STDMETHODCALLTYPE DrawIndexed(__in UINT IndexCount, __in UINT StartIndexLocation, __in INT BaseVertexLocation) = 0;
		virtual void STDMETHODCALLTYPE Draw(__in UINT VertexCount, __in UINT StartVertexLocation) = 0;
		virtual void STDMETHODCALLTYPE PSSetConstantBuffers(__in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot) UINT NumBuffers, __in_ecount(NumBuffers) ID3D10Buffer *const *ppConstantBuffers) = 0;
		virtual void STDMETHODCALLTYPE IASetInputLayout(__in_opt ID3D10InputLayout *pInputLayout) = 0;
		virtual void STDMETHODCALLTYPE IASetVertexBuffers(__in_range(0, D3D10_1_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_1_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot) UINT NumBuffers, __in_ecount(NumBuffers) ID3D10Buffer *const *ppVertexBuffers, __in_ecount(NumBuffers) const UINT *pStrides, __in_ecount(NumBuffers) const UINT *pOffsets) = 0;
		virtual void STDMETHODCALLTYPE IASetIndexBuffer(__in_opt ID3D10Buffer *pIndexBuffer, __in DXGI_FORMAT Format, __in UINT Offset) = 0;
		virtual void STDMETHODCALLTYPE DrawIndexedInstanced(__in UINT IndexCountPerInstance, __in UINT InstanceCount, __in UINT StartIndexLocation, __in INT BaseVertexLocation, __in UINT StartInstanceLocation) = 0;
		virtual void STDMETHODCALLTYPE DrawInstanced(__in UINT VertexCountPerInstance, __in UINT InstanceCount, __in UINT StartVertexLocation, __in UINT StartInstanceLocation) = 0;
		virtual void STDMETHODCALLTYPE GSSetConstantBuffers(__in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot) UINT NumBuffers, __in_ecount(NumBuffers) ID3D10Buffer *const *ppConstantBuffers) = 0;
		virtual void STDMETHODCALLTYPE GSSetShader(__in_opt ID3D10GeometryShader *pShader) = 0;
		virtual void STDMETHODCALLTYPE IASetPrimitiveTopology(__in D3D10_PRIMITIVE_TOPOLOGY Topology) = 0;
		virtual void STDMETHODCALLTYPE VSSetShaderResources(__in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot) UINT NumViews, __in_ecount(NumViews) ID3D10ShaderResourceView *const *ppShaderResourceViews) = 0;
		virtual void STDMETHODCALLTYPE VSSetSamplers(__in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot) UINT NumSamplers, __in_ecount(NumSamplers) ID3D10SamplerState *const *ppSamplers) = 0;
		virtual void STDMETHODCALLTYPE SetPredication(__in_opt ID3D10Predicate *pPredicate, __in BOOL PredicateValue) = 0;
		virtual void STDMETHODCALLTYPE GSSetShaderResources(__in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot) UINT NumViews, __in_ecount(NumViews) ID3D10ShaderResourceView *const *ppShaderResourceViews) = 0;
		virtual void STDMETHODCALLTYPE GSSetSamplers(__in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot) UINT NumSamplers, __in_ecount(NumSamplers) ID3D10SamplerState *const *ppSamplers) = 0;
		virtual void STDMETHODCALLTYPE OMSetRenderTargets(__in_range(0, D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT) UINT NumViews, __in_ecount_opt(NumViews) ID3D10RenderTargetView *const *ppRenderTargetViews, __in_opt ID3D10DepthStencilView *pDepthStencilView) = 0;
		virtual void STDMETHODCALLTYPE OMSetBlendState(__in_opt ID3D10BlendState *pBlendState, __in const FLOAT BlendFactor[4], __in UINT SampleMask) = 0;
		virtual void STDMETHODCALLTYPE OMSetDepthStencilState(__in_opt ID3D10DepthStencilState *pDepthStencilState, __in UINT StencilRef) = 0;
		virtual void STDMETHODCALLTYPE SOSetTargets(__in_range(0, D3D10_SO_BUFFER_SLOT_COUNT) UINT NumBuffers, __in_ecount_opt(NumBuffers) ID3D10Buffer *const *ppSOTargets, __in_ecount_opt(NumBuffers) const UINT *pOffsets) = 0;
		virtual void STDMETHODCALLTYPE DrawAuto(void) = 0;
		virtual void STDMETHODCALLTYPE RSSetState(__in_opt ID3D10RasterizerState *pRasterizerState) = 0;
		virtual void STDMETHODCALLTYPE RSSetViewports(__in_range(0, D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE) UINT NumViewports, __in_ecount_opt(NumViewports) const D3D10_VIEWPORT *pViewports) = 0;
		virtual void STDMETHODCALLTYPE RSSetScissorRects(__in_range(0, D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE) UINT NumRects, __in_ecount_opt(NumRects) const D3D10_RECT *pRects) = 0;
		virtual void STDMETHODCALLTYPE CopySubresourceRegion(__in ID3D10Resource *pDstResource, __in UINT DstSubresource, __in UINT DstX, __in UINT DstY, __in UINT DstZ, __in ID3D10Resource *pSrcResource, __in UINT SrcSubresource, __in_opt const D3D10_BOX *pSrcBox) = 0;
		virtual void STDMETHODCALLTYPE CopyResource(__in ID3D10Resource *pDstResource, __in ID3D10Resource *pSrcResource) = 0;
		virtual void STDMETHODCALLTYPE UpdateSubresource(__in ID3D10Resource *pDstResource, __in UINT DstSubresource, __in_opt const D3D10_BOX *pDstBox, __in const void *pSrcData, __in UINT SrcRowPitch, __in UINT SrcDepthPitch) = 0;
		virtual void STDMETHODCALLTYPE ClearRenderTargetView(__in ID3D10RenderTargetView *pRenderTargetView, __in const FLOAT ColorRGBA[4]) = 0;
		virtual void STDMETHODCALLTYPE ClearDepthStencilView(__in ID3D10DepthStencilView *pDepthStencilView, __in UINT ClearFlags, __in FLOAT Depth, __in UINT8 Stencil) = 0;
		virtual void STDMETHODCALLTYPE GenerateMips(__in ID3D10ShaderResourceView *pShaderResourceView) = 0;
		virtual void STDMETHODCALLTYPE ResolveSubresource(__in ID3D10Resource *pDstResource, __in UINT DstSubresource, __in ID3D10Resource *pSrcResource, __in UINT SrcSubresource, __in DXGI_FORMAT Format) = 0;
		virtual void STDMETHODCALLTYPE VSGetConstantBuffers(__in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot) UINT NumBuffers, __out_ecount(NumBuffers) ID3D10Buffer **ppConstantBuffers) = 0;
		virtual void STDMETHODCALLTYPE PSGetShaderResources(__in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot) UINT NumViews, __out_ecount(NumViews) ID3D10ShaderResourceView **ppShaderResourceViews) = 0;
		virtual void STDMETHODCALLTYPE PSGetShader(__out ID3D10PixelShader **ppPixelShader) = 0;
		virtual void STDMETHODCALLTYPE PSGetSamplers(__in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot) UINT NumSamplers, __out_ecount(NumSamplers) ID3D10SamplerState **ppSamplers) = 0;
		virtual void STDMETHODCALLTYPE VSGetShader(__out ID3D10VertexShader **ppVertexShader) = 0;
		virtual void STDMETHODCALLTYPE PSGetConstantBuffers(__in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot) UINT NumBuffers, __out_ecount(NumBuffers) ID3D10Buffer **ppConstantBuffers) = 0;
		virtual void STDMETHODCALLTYPE IAGetInputLayout(__out ID3D10InputLayout **ppInputLayout) = 0;
		virtual void STDMETHODCALLTYPE IAGetVertexBuffers(__in_range(0, D3D10_1_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_1_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot) UINT NumBuffers, __out_ecount_opt(NumBuffers) ID3D10Buffer **ppVertexBuffers, __out_ecount_opt(NumBuffers) UINT *pStrides, __out_ecount_opt(NumBuffers) UINT *pOffsets) = 0;
		virtual void STDMETHODCALLTYPE IAGetIndexBuffer(__out_opt ID3D10Buffer **pIndexBuffer, __out_opt DXGI_FORMAT *Format, __out_opt UINT *Offset) = 0;
		virtual void STDMETHODCALLTYPE GSGetConstantBuffers(__in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot) UINT NumBuffers, __out_ecount(NumBuffers) ID3D10Buffer **ppConstantBuffers) = 0;
		virtual void STDMETHODCALLTYPE GSGetShader(__out ID3D10GeometryShader **ppGeometryShader) = 0;
		virtual void STDMETHODCALLTYPE IAGetPrimitiveTopology(__out D3D10_PRIMITIVE_TOPOLOGY *pTopology) = 0;
		virtual void STDMETHODCALLTYPE VSGetShaderResources(__in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot) UINT NumViews, __out_ecount(NumViews) ID3D10ShaderResourceView **ppShaderResourceViews) = 0;
		virtual void STDMETHODCALLTYPE VSGetSamplers(__in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot) UINT NumSamplers, __out_ecount(NumSamplers) ID3D10SamplerState **ppSamplers) = 0;
		virtual void STDMETHODCALLTYPE GetPredication(__out_opt ID3D10Predicate **ppPredicate, __out_opt BOOL *pPredicateValue) = 0;
		virtual void STDMETHODCALLTYPE GSGetShaderResources(__in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot) UINT NumViews, __out_ecount(NumViews) ID3D10ShaderResourceView **ppShaderResourceViews) = 0;
		virtual void STDMETHODCALLTYPE GSGetSamplers(__in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - 1) UINT StartSlot, __in_range(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot) UINT NumSamplers, __out_ecount(NumSamplers) ID3D10SamplerState **ppSamplers) = 0;
		virtual void STDMETHODCALLTYPE OMGetRenderTargets(__in_range(0, D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT) UINT NumViews, __out_ecount_opt(NumViews) ID3D10RenderTargetView **ppRenderTargetViews, __out_opt ID3D10DepthStencilView **ppDepthStencilView) = 0;
		virtual void STDMETHODCALLTYPE OMGetBlendState(__out_opt ID3D10BlendState **ppBlendState, __out_opt FLOAT BlendFactor[4], __out_opt UINT *pSampleMask) = 0;
		virtual void STDMETHODCALLTYPE OMGetDepthStencilState(__out_opt ID3D10DepthStencilState **ppDepthStencilState, __out_opt UINT *pStencilRef) = 0;
		virtual void STDMETHODCALLTYPE SOGetTargets(__in_range(0, D3D10_SO_BUFFER_SLOT_COUNT) UINT NumBuffers, __out_ecount_opt(NumBuffers) ID3D10Buffer **ppSOTargets, __out_ecount_opt(NumBuffers) UINT *pOffsets) = 0;
		virtual void STDMETHODCALLTYPE RSGetState(__out ID3D10RasterizerState **ppRasterizerState) = 0;
		virtual void STDMETHODCALLTYPE RSGetViewports(__inout UINT *NumViewports, __out_ecount_opt(*NumViewports) D3D10_VIEWPORT *pViewports) = 0;
		virtual void STDMETHODCALLTYPE RSGetScissorRects(__inout UINT *NumRects, __out_ecount_opt(*NumRects) D3D10_RECT *pRects) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetExceptionMode(UINT RaiseFlags) = 0;
		virtual UINT STDMETHODCALLTYPE GetExceptionMode(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetPrivateData(__in REFGUID guid, __inout UINT *pDataSize, __out_bcount_opt(*pDataSize) void *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetPrivateData(__in REFGUID guid, __in UINT DataSize, __in_bcount_opt(DataSize) const void *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(__in REFGUID guid, __in_opt const IUnknown *pData) = 0;
		virtual void STDMETHODCALLTYPE ClearState(void) = 0;
		virtual void STDMETHODCALLTYPE Flush(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateBuffer(__in const D3D10_BUFFER_DESC *pDesc, __in_opt const D3D10_SUBRESOURCE_DATA *pInitialData, __out_opt ID3D10Buffer **ppBuffer) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateTexture1D(__in const D3D10_TEXTURE1D_DESC *pDesc, __in_xcount_opt(pDesc->MipLevels * pDesc->ArraySize) const D3D10_SUBRESOURCE_DATA *pInitialData, __out ID3D10Texture1D **ppTexture1D) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateTexture2D(__in const D3D10_TEXTURE2D_DESC *pDesc, __in_xcount_opt(pDesc->MipLevels * pDesc->ArraySize) const D3D10_SUBRESOURCE_DATA *pInitialData, __out ID3D10Texture2D **ppTexture2D) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateTexture3D(__in const D3D10_TEXTURE3D_DESC *pDesc, __in_xcount_opt(pDesc->MipLevels) const D3D10_SUBRESOURCE_DATA *pInitialData, __out ID3D10Texture3D **ppTexture3D) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView(__in ID3D10Resource *pResource, __in_opt const D3D10_SHADER_RESOURCE_VIEW_DESC *pDesc, __out_opt ID3D10ShaderResourceView **ppSRView) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView(__in ID3D10Resource *pResource, __in_opt const D3D10_RENDER_TARGET_VIEW_DESC *pDesc, __out_opt ID3D10RenderTargetView **ppRTView) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView(__in ID3D10Resource *pResource, __in_opt const D3D10_DEPTH_STENCIL_VIEW_DESC *pDesc, __out_opt ID3D10DepthStencilView **ppDepthStencilView) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateInputLayout(__in_ecount(NumElements) const D3D10_INPUT_ELEMENT_DESC *pInputElementDescs, __in_range(0, D3D10_1_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT) UINT NumElements, __in const void *pShaderBytecodeWithInputSignature, __in SIZE_T BytecodeLength, __out_opt ID3D10InputLayout **ppInputLayout) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateVertexShader(__in const void *pShaderBytecode, __in SIZE_T BytecodeLength, __out_opt ID3D10VertexShader **ppVertexShader) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader(__in const void *pShaderBytecode, __in SIZE_T BytecodeLength, __out_opt ID3D10GeometryShader **ppGeometryShader) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput(__in const void *pShaderBytecode, __in SIZE_T BytecodeLength, __in_ecount_opt(NumEntries) const D3D10_SO_DECLARATION_ENTRY *pSODeclaration, __in_range(0, D3D10_SO_SINGLE_BUFFER_COMPONENT_LIMIT) UINT NumEntries, __in UINT OutputStreamStride, __out_opt ID3D10GeometryShader **ppGeometryShader) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreatePixelShader(__in const void *pShaderBytecode, __in SIZE_T BytecodeLength, __out_opt ID3D10PixelShader **ppPixelShader) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateBlendState(__in const D3D10_BLEND_DESC *pBlendStateDesc, __out_opt ID3D10BlendState **ppBlendState) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState(__in const D3D10_DEPTH_STENCIL_DESC *pDepthStencilDesc, __out_opt ID3D10DepthStencilState **ppDepthStencilState) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState(__in const D3D10_RASTERIZER_DESC *pRasterizerDesc, __out_opt ID3D10RasterizerState **ppRasterizerState) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateSamplerState(__in const D3D10_SAMPLER_DESC *pSamplerDesc, __out_opt ID3D10SamplerState **ppSamplerState) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateQuery(__in const D3D10_QUERY_DESC *pQueryDesc, __out_opt ID3D10Query **ppQuery) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreatePredicate(__in const D3D10_QUERY_DESC *pPredicateDesc, __out_opt ID3D10Predicate **ppPredicate) = 0;
		virtual HRESULT STDMETHODCALLTYPE CreateCounter(__in const D3D10_COUNTER_DESC *pCounterDesc, __out_opt ID3D10Counter **ppCounter) = 0;
		virtual HRESULT STDMETHODCALLTYPE CheckFormatSupport(__in DXGI_FORMAT Format, __out UINT *pFormatSupport) = 0;
		virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels(__in DXGI_FORMAT Format, __in UINT SampleCount, __out UINT *pNumQualityLevels) = 0;
		virtual void STDMETHODCALLTYPE CheckCounterInfo(__out D3D10_COUNTER_INFO *pCounterInfo) = 0;
		virtual HRESULT STDMETHODCALLTYPE CheckCounter(__in const D3D10_COUNTER_DESC *pDesc, __out D3D10_COUNTER_TYPE *pType, __out UINT *pActiveCounters, __out_ecount_opt(*pNameLength) LPSTR szName, __inout_opt UINT *pNameLength, __out_ecount_opt(*pUnitsLength) LPSTR szUnits, __inout_opt UINT *pUnitsLength, __out_ecount_opt(*pDescriptionLength) LPSTR szDescription, __inout_opt UINT *pDescriptionLength) = 0;
		virtual UINT STDMETHODCALLTYPE GetCreationFlags(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE OpenSharedResource(__in HANDLE hResource, __in REFIID ReturnedInterface, __out_opt void **ppResource) = 0;
		virtual void STDMETHODCALLTYPE SetTextFilterSize(__in UINT Width, __in UINT Height) = 0;
		virtual void STDMETHODCALLTYPE GetTextFilterSize(__out_opt UINT *pWidth, __out_opt UINT *pHeight) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10DeviceChild : public IUnknown
{
	public:
		virtual void STDMETHODCALLTYPE GetDevice(__out ID3D10Device **ppDevice) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetPrivateData(__in REFGUID guid, __inout UINT *pDataSize, __out_bcount_opt(*pDataSize) void *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetPrivateData(__in REFGUID guid, __in UINT DataSize, __in_bcount_opt(DataSize) const void *pData) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(__in REFGUID guid, __in_opt const IUnknown *pData) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10InputLayout : public ID3D10DeviceChild
{
	// Nothing in here
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10SamplerState : public ID3D10DeviceChild
{
	public:
		virtual void STDMETHODCALLTYPE GetDesc(__out D3D10_SAMPLER_DESC *pDesc) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10RasterizerState : public ID3D10DeviceChild
{
	public:
		virtual void STDMETHODCALLTYPE GetDesc(__out D3D10_RASTERIZER_DESC *pDesc) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_BLEND
{
	D3D10_BLEND_ZERO				= 1,
	D3D10_BLEND_ONE					= 2,
	D3D10_BLEND_SRC_COLOR			= 3,
	D3D10_BLEND_INV_SRC_COLOR		= 4,
	D3D10_BLEND_SRC_ALPHA			= 5,
	D3D10_BLEND_INV_SRC_ALPHA		= 6,
	D3D10_BLEND_DEST_ALPHA			= 7,
	D3D10_BLEND_INV_DEST_ALPHA		= 8,
	D3D10_BLEND_DEST_COLOR			= 9,
	D3D10_BLEND_INV_DEST_COLOR		= 10,
	D3D10_BLEND_SRC_ALPHA_SAT		= 11,
	D3D10_BLEND_BLEND_FACTOR		= 14,
	D3D10_BLEND_INV_BLEND_FACTOR	= 15,
	D3D10_BLEND_SRC1_COLOR			= 16,
	D3D10_BLEND_INV_SRC1_COLOR		= 17,
	D3D10_BLEND_SRC1_ALPHA			= 18,
	D3D10_BLEND_INV_SRC1_ALPHA		= 19
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_BLEND_OP
{
	D3D10_BLEND_OP_ADD			= 1,
	D3D10_BLEND_OP_SUBTRACT		= 2,
	D3D10_BLEND_OP_REV_SUBTRACT	= 3,
	D3D10_BLEND_OP_MIN			= 4,
	D3D10_BLEND_OP_MAX			= 5
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_BLEND_DESC
{
	BOOL AlphaToCoverageEnable;
	BOOL BlendEnable[8];
	D3D10_BLEND SrcBlend;
	D3D10_BLEND DestBlend;
	D3D10_BLEND_OP BlendOp;
	D3D10_BLEND SrcBlendAlpha;
	D3D10_BLEND DestBlendAlpha;
	D3D10_BLEND_OP BlendOpAlpha;
	UINT8 RenderTargetWriteMask[8];
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10BlendState : public ID3D10DeviceChild
{
	public:
		virtual void STDMETHODCALLTYPE GetDesc(__out D3D10_BLEND_DESC *pDesc) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10DepthStencilState : public ID3D10DeviceChild
{
	public:
		virtual void STDMETHODCALLTYPE GetDesc(__out D3D10_DEPTH_STENCIL_DESC *pDesc) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10VertexShader : public ID3D10DeviceChild
{
	// Nothing in here
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10GeometryShader : public ID3D10DeviceChild
{
	// Nothing in here
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10PixelShader : public ID3D10DeviceChild
{
	// Nothing in here
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10Resource : public ID3D10DeviceChild
{
	public:
		virtual void STDMETHODCALLTYPE GetType(__out D3D10_RESOURCE_DIMENSION *pResourceDimension) = 0;
		virtual void STDMETHODCALLTYPE SetEvictionPriority(__in UINT EvictionPriority) = 0;
		virtual UINT STDMETHODCALLTYPE GetEvictionPriority(void) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10Buffer : public ID3D10Resource
{
	public:
		virtual HRESULT STDMETHODCALLTYPE Map(__in D3D10_MAP MapType, __in UINT MapFlags, __out void **ppData) = 0;
		virtual void STDMETHODCALLTYPE Unmap(void) = 0;
		virtual void STDMETHODCALLTYPE GetDesc(__out D3D10_BUFFER_DESC *pDesc) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
MIDL_INTERFACE("9B7E4C04-342C-4106-A19F-4F2704F689F0")
ID3D10Texture2D : public ID3D10Resource
{
	public:
		virtual HRESULT STDMETHODCALLTYPE Map(__in UINT Subresource, __in D3D10_MAP MapType, __in UINT MapFlags, __out D3D10_MAPPED_TEXTURE2D *pMappedTex2D) = 0;
		virtual void STDMETHODCALLTYPE Unmap(__in UINT Subresource) = 0;
		virtual void STDMETHODCALLTYPE GetDesc(__out D3D10_TEXTURE2D_DESC *pDesc) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10View : public ID3D10DeviceChild
{
	public:
		virtual void STDMETHODCALLTYPE GetResource(__out ID3D10Resource **ppResource) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10ShaderResourceView : public ID3D10View
{
	public:
		virtual void STDMETHODCALLTYPE GetDesc(__out D3D10_SHADER_RESOURCE_VIEW_DESC *pDesc) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10RenderTargetView : public ID3D10View
{
	public:
		virtual void STDMETHODCALLTYPE GetDesc(__out D3D10_RENDER_TARGET_VIEW_DESC *pDesc) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10DepthStencilView : public ID3D10View
{
	public:
		virtual void STDMETHODCALLTYPE GetDesc(__out D3D10_DEPTH_STENCIL_VIEW_DESC *pDesc) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3Dcommon.h"
struct ID3D10Blob : public IUnknown
{
	public:
		virtual LPVOID STDMETHODCALLTYPE GetBufferPointer(void) = 0;
		virtual SIZE_T STDMETHODCALLTYPE GetBufferSize(void) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10Asynchronous : public ID3D10DeviceChild
{
	public:
		virtual void STDMETHODCALLTYPE Begin(void) = 0;
		virtual void STDMETHODCALLTYPE End(void) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetData(__out_bcount_opt(DataSize) void *pData, __in UINT DataSize, __in UINT GetDataFlags) = 0;
		virtual UINT STDMETHODCALLTYPE GetDataSize(void) = 0;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
enum D3D10_QUERY
{
	D3D10_QUERY_EVENT					= 0,
	D3D10_QUERY_OCCLUSION				= (D3D10_QUERY_EVENT + 1),
	D3D10_QUERY_TIMESTAMP				= (D3D10_QUERY_OCCLUSION + 1),
	D3D10_QUERY_TIMESTAMP_DISJOINT		= (D3D10_QUERY_TIMESTAMP + 1),
	D3D10_QUERY_PIPELINE_STATISTICS		= (D3D10_QUERY_TIMESTAMP_DISJOINT + 1),
	D3D10_QUERY_OCCLUSION_PREDICATE		= (D3D10_QUERY_PIPELINE_STATISTICS + 1),
	D3D10_QUERY_SO_STATISTICS			= (D3D10_QUERY_OCCLUSION_PREDICATE + 1),
	D3D10_QUERY_SO_OVERFLOW_PREDICATE	= (D3D10_QUERY_SO_STATISTICS + 1)
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct D3D10_QUERY_DESC
{
	D3D10_QUERY Query;
	UINT MiscFlags;
};

// "Microsoft DirectX SDK (June 2010)" -> "D3D10.h"
struct ID3D10Query : public ID3D10Asynchronous
{
	public:
		virtual void STDMETHODCALLTYPE GetDesc(__out D3D10_QUERY_DESC *pDesc) = 0;
};
