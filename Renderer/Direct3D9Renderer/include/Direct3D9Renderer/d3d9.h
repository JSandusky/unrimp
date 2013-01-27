/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#ifndef __DIRECT3D9RENDERER_D3D9_H__
#define __DIRECT3D9RENDERER_D3D9_H__


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
struct D3DRECT;
struct D3DCAPS9;
struct D3DMATRIX;
struct D3DLIGHT9;
struct D3DXMACRO;
struct IDirect3D9;
struct D3DXMATRIX;
struct D3DXVECTOR4;
struct D3DVIEWPORT9;
struct D3DMATERIAL9;
struct D3DGAMMARAMP;
struct ID3DXInclude;
struct D3DLOCKED_RECT;
struct D3DDISPLAYMODE;
struct D3DCLIPSTATUS9;
struct IDirect3DQuery9;
struct D3DSURFACE_DESC;
struct D3DRASTER_STATUS;
struct IDirect3DDevice9;
struct D3DTRIPATCH_INFO;
struct D3DXCONSTANT_DESC;
struct D3DRECTPATCH_INFO;
struct IDirect3DSurface9;
struct IDirect3DTexture9;
struct D3DVERTEXELEMENT9;
struct LPDIRECT3DTEXTURE9;
struct D3DINDEXBUFFER_DESC;
struct IDirect3DSwapChain9;
struct D3DVERTEXBUFFER_DESC;
struct IDirect3DStateBlock9;
struct IDirect3DBaseTexture9;
struct D3DPRESENT_PARAMETERS;
struct IDirect3DCubeTexture9;
struct IDirect3DIndexBuffer9;
struct IDirect3DPixelShader9;
struct D3DADAPTER_IDENTIFIER9;
struct D3DXCONSTANTTABLE_DESC;
struct IDirect3DVertexBuffer9;
struct IDirect3DVertexShader9;
struct IDirect3DVolumeTexture9;
struct IDirect3DVertexDeclaration9;
struct D3DDEVICE_CREATION_PARAMETERS;


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
#define D3D_SDK_VERSION						(32 | 0x80000000)
#define D3D_OK								S_OK
#define D3DADAPTER_DEFAULT					0
#define D3DCREATE_HARDWARE_VERTEXPROCESSING	0x00000040L

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
typedef DWORD D3DCOLOR;
#define MAXD3DDECLLENGTH				64
#define D3DSTREAMSOURCE_INSTANCEDATA	(2<<30)
#define D3DLOCK_READONLY				0x00000010L
#define D3DUSAGE_WRITEONLY				0x00000008L
#define D3DUSAGE_DYNAMIC				0x00000200L
#define D3DUSAGE_AUTOGENMIPMAP			0x00000400L
#define D3DUSAGE_RENDERTARGET			0x00000001L
#ifndef MAKEFOURCC
	#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
				((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
				((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif
#define D3DCOLOR_ARGB(a,r,g,b)			((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a)			D3DCOLOR_ARGB(a,r,g,b)
#define D3DCOLOR_COLORVALUE(r,g,b,a)	D3DCOLOR_RGBA((DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f),(DWORD)((a)*255.f))
#define D3DPS_VERSION(_Major,_Minor)	(0xFFFF0000|((_Major)<<8)|(_Minor))

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
#define D3DDMAPSAMPLER				256
#define D3DVERTEXTEXTURESAMPLER1	(D3DDMAPSAMPLER+2)
#define D3DVERTEXTEXTURESAMPLER2	(D3DDMAPSAMPLER+3)
#define D3DVERTEXTEXTURESAMPLER3	(D3DDMAPSAMPLER+4)
#define D3DCLEAR_TARGET				0x00000001l
#define D3DCLEAR_ZBUFFER			0x00000002l
#define D3DCLEAR_STENCIL			0x00000004l
#define D3DSTREAMSOURCE_INDEXEDDATA	(1<<30)
#define D3DISSUE_END				(1 << 0)
#define D3DGETDATA_FLUSH			(1 << 0)

// "Microsoft Direct3D SDK (June 2010)" -> "d3dx9tex.h"
#define D3DX_FILTER_NONE	(1 << 0)

// "Microsoft Direct3D SDK (June 2010)" -> "d3dx9shader.h"
#define D3DXSHADER_DEBUG	(1 << 0)

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DDEVTYPE
{
	D3DDEVTYPE_HAL			= 1,
	D3DDEVTYPE_REF			= 2,
	D3DDEVTYPE_SW			= 3,
	D3DDEVTYPE_NULLREF		= 4,
	D3DDEVTYPE_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DFORMAT
{
	D3DFMT_UNKNOWN				=  0,
	D3DFMT_R8G8B8				= 20,
	D3DFMT_A8R8G8B8				= 21,
	D3DFMT_X8R8G8B8				= 22,
	D3DFMT_R5G6B5				= 23,
	D3DFMT_X1R5G5B5				= 24,
	D3DFMT_A1R5G5B5				= 25,
	D3DFMT_A4R4G4B4				= 26,
	D3DFMT_R3G3B2				= 27,
	D3DFMT_A8					= 28,
	D3DFMT_A8R3G3B2				= 29,
	D3DFMT_X4R4G4B4				= 30,
	D3DFMT_A2B10G10R10			= 31,
	D3DFMT_A8B8G8R8				= 32,
	D3DFMT_X8B8G8R8				= 33,
	D3DFMT_G16R16				= 34,
	D3DFMT_A2R10G10B10			= 35,
	D3DFMT_A16B16G16R16			= 36,
	D3DFMT_A8P8					= 40,
	D3DFMT_P8					= 41,
	D3DFMT_L8					= 50,
	D3DFMT_A8L8					= 51,
	D3DFMT_A4L4					= 52,
	D3DFMT_V8U8					= 60,
	D3DFMT_L6V5U5				= 61,
	D3DFMT_X8L8V8U8				= 62,
	D3DFMT_Q8W8V8U8				= 63,
	D3DFMT_V16U16				= 64,
	D3DFMT_A2W10V10U10			= 67,
	D3DFMT_UYVY					= MAKEFOURCC('U', 'Y', 'V', 'Y'),
	D3DFMT_R8G8_B8G8			= MAKEFOURCC('R', 'G', 'B', 'G'),
	D3DFMT_YUY2					= MAKEFOURCC('Y', 'U', 'Y', '2'),
	D3DFMT_G8R8_G8B8			= MAKEFOURCC('G', 'R', 'G', 'B'),
	D3DFMT_DXT1					= MAKEFOURCC('D', 'X', 'T', '1'),
	D3DFMT_DXT2					= MAKEFOURCC('D', 'X', 'T', '2'),
	D3DFMT_DXT3					= MAKEFOURCC('D', 'X', 'T', '3'),
	D3DFMT_DXT4					= MAKEFOURCC('D', 'X', 'T', '4'),
	D3DFMT_DXT5					= MAKEFOURCC('D', 'X', 'T', '5'),
	D3DFMT_D16_LOCKABLE			= 70,
	D3DFMT_D32					= 71,
	D3DFMT_D15S1				= 73,
	D3DFMT_D24S8				= 75,
	D3DFMT_D24X8				= 77,
	D3DFMT_D24X4S4				= 79,
	D3DFMT_D16					= 80,
	D3DFMT_D32F_LOCKABLE		= 82,
	D3DFMT_D24FS8				= 83,
	D3DFMT_D32_LOCKABLE			= 84,
	D3DFMT_S8_LOCKABLE			= 85,
	D3DFMT_L16					= 81,
	D3DFMT_VERTEXDATA			= 100,
	D3DFMT_INDEX16				= 101,
	D3DFMT_INDEX32				= 102,
	D3DFMT_Q16W16V16U16			= 110,
	D3DFMT_MULTI2_ARGB8			= MAKEFOURCC('M','E','T','1'),
	D3DFMT_R16F					= 111,
	D3DFMT_G16R16F				= 112,
	D3DFMT_A16B16G16R16F		= 113,
	D3DFMT_R32F					= 114,
	D3DFMT_G32R32F				= 115,
	D3DFMT_A32B32G32R32F		= 116,
	D3DFMT_CxV8U8				= 117,
	D3DFMT_A1					= 118,
	D3DFMT_A2B10G10R10_XR_BIAS	= 119,
	D3DFMT_BINARYBUFFER			= 199,
	D3DFMT_FORCE_DWORD			= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DMULTISAMPLE_TYPE
{
	D3DMULTISAMPLE_NONE			=  0,
	D3DMULTISAMPLE_NONMASKABLE	=  1,
	D3DMULTISAMPLE_2_SAMPLES	=  2,
	D3DMULTISAMPLE_3_SAMPLES	=  3,
	D3DMULTISAMPLE_4_SAMPLES	=  4,
	D3DMULTISAMPLE_5_SAMPLES	=  5,
	D3DMULTISAMPLE_6_SAMPLES	=  6,
	D3DMULTISAMPLE_7_SAMPLES	=  7,
	D3DMULTISAMPLE_8_SAMPLES	=  8,
	D3DMULTISAMPLE_9_SAMPLES	=  9,
	D3DMULTISAMPLE_10_SAMPLES	= 10,
	D3DMULTISAMPLE_11_SAMPLES	= 11,
	D3DMULTISAMPLE_12_SAMPLES	= 12,
	D3DMULTISAMPLE_13_SAMPLES	= 13,
	D3DMULTISAMPLE_14_SAMPLES	= 14,
	D3DMULTISAMPLE_15_SAMPLES	= 15,
	D3DMULTISAMPLE_16_SAMPLES	= 16,
	D3DMULTISAMPLE_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DPOOL
{
	D3DPOOL_DEFAULT		= 0,
	D3DPOOL_MANAGED		= 1,
	D3DPOOL_SYSTEMMEM	= 2,
	D3DPOOL_SCRATCH		= 3,
	D3DPOOL_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DTEXTUREFILTERTYPE
{
	D3DTEXF_NONE			= 0,
	D3DTEXF_POINT			= 1,
	D3DTEXF_LINEAR			= 2,
	D3DTEXF_ANISOTROPIC		= 3,
	D3DTEXF_PYRAMIDALQUAD	= 6,
	D3DTEXF_GAUSSIANQUAD	= 7,
	D3DTEXF_CONVOLUTIONMONO	= 8,
	D3DTEXF_FORCE_DWORD		= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DTEXTUREADDRESS
{
	D3DTADDRESS_WRAP		= 1,
	D3DTADDRESS_MIRROR		= 2,
	D3DTADDRESS_CLAMP		= 3,
	D3DTADDRESS_BORDER		= 4,
	D3DTADDRESS_MIRRORONCE	= 5,
	D3DTADDRESS_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DDECLTYPE
{
	D3DDECLTYPE_FLOAT1		=  0,
	D3DDECLTYPE_FLOAT2		=  1,
	D3DDECLTYPE_FLOAT3		=  2,
	D3DDECLTYPE_FLOAT4		=  3,
	D3DDECLTYPE_D3DCOLOR	=  4,
	D3DDECLTYPE_UBYTE4		=  5,
	D3DDECLTYPE_SHORT2		=  6,
	D3DDECLTYPE_SHORT4		=  7,
	D3DDECLTYPE_UBYTE4N		=  8,
	D3DDECLTYPE_SHORT2N		=  9,
	D3DDECLTYPE_SHORT4N		= 10,
	D3DDECLTYPE_USHORT2N	= 11,
	D3DDECLTYPE_USHORT4N	= 12,
	D3DDECLTYPE_UDEC3		= 13,
	D3DDECLTYPE_DEC3N		= 14,
	D3DDECLTYPE_FLOAT16_2	= 15,
	D3DDECLTYPE_FLOAT16_4	= 16,
	D3DDECLTYPE_UNUSED		= 17
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DDECLUSAGE
{
	D3DDECLUSAGE_POSITION = 0,
	D3DDECLUSAGE_BLENDWEIGHT,
	D3DDECLUSAGE_BLENDINDICES,
	D3DDECLUSAGE_NORMAL,
	D3DDECLUSAGE_PSIZE,
	D3DDECLUSAGE_TEXCOORD,
	D3DDECLUSAGE_TANGENT,
	D3DDECLUSAGE_BINORMAL,
	D3DDECLUSAGE_TESSFACTOR,
	D3DDECLUSAGE_POSITIONT,
	D3DDECLUSAGE_COLOR,
	D3DDECLUSAGE_FOG,
	D3DDECLUSAGE_DEPTH,
	D3DDECLUSAGE_SAMPLE,
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DTRANSFORMSTATETYPE
{
	D3DTS_VIEW			= 2,
	D3DTS_PROJECTION	= 3,
	D3DTS_TEXTURE0		= 16,
	D3DTS_TEXTURE1		= 17,
	D3DTS_TEXTURE2		= 18,
	D3DTS_TEXTURE3		= 19,
	D3DTS_TEXTURE4		= 20,
	D3DTS_TEXTURE5		= 21,
	D3DTS_TEXTURE6		= 22,
	D3DTS_TEXTURE7		= 23,
	D3DTS_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DFILLMODE
{
	D3DFILL_POINT		= 1,
	D3DFILL_WIREFRAME	= 2,
	D3DFILL_SOLID		= 3,
	D3DFILL_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DCULL
{
	D3DCULL_NONE		= 1,
	D3DCULL_CW			= 2,
	D3DCULL_CCW			= 3,
	D3DCULL_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DRENDERSTATETYPE
{
	D3DRS_ZENABLE						= 7,
	D3DRS_FILLMODE						= 8,
	D3DRS_SHADEMODE						= 9,
	D3DRS_ZWRITEENABLE					= 14,
	D3DRS_ALPHATESTENABLE				= 15,
	D3DRS_LASTPIXEL						= 16,
	D3DRS_SRCBLEND						= 19,
	D3DRS_DESTBLEND						= 20,
	D3DRS_CULLMODE						= 22,
	D3DRS_ZFUNC							= 23,
	D3DRS_ALPHAREF						= 24,
	D3DRS_ALPHAFUNC						= 25,
	D3DRS_DITHERENABLE					= 26,
	D3DRS_ALPHABLENDENABLE				= 27,
	D3DRS_FOGENABLE						= 28,
	D3DRS_SPECULARENABLE				= 29,
	D3DRS_FOGCOLOR						= 34,
	D3DRS_FOGTABLEMODE					= 35,
	D3DRS_FOGSTART						= 36,
	D3DRS_FOGEND						= 37,
	D3DRS_FOGDENSITY					= 38,
	D3DRS_RANGEFOGENABLE				= 48,
	D3DRS_STENCILENABLE					= 52,
	D3DRS_STENCILFAIL					= 53,
	D3DRS_STENCILZFAIL					= 54,
	D3DRS_STENCILPASS					= 55,
	D3DRS_STENCILFUNC					= 56,
	D3DRS_STENCILREF					= 57,
	D3DRS_STENCILMASK					= 58,
	D3DRS_STENCILWRITEMASK				= 59,
	D3DRS_TEXTUREFACTOR					= 60,
	D3DRS_WRAP0							= 128,
	D3DRS_WRAP1							= 129,
	D3DRS_WRAP2							= 130,
	D3DRS_WRAP3							= 131,
	D3DRS_WRAP4							= 132,
	D3DRS_WRAP5							= 133,
	D3DRS_WRAP6							= 134,
	D3DRS_WRAP7							= 135,
	D3DRS_CLIPPING						= 136,
	D3DRS_LIGHTING						= 137,
	D3DRS_AMBIENT						= 139,
	D3DRS_FOGVERTEXMODE					= 140,
	D3DRS_COLORVERTEX					= 141,
	D3DRS_LOCALVIEWER					= 142,
	D3DRS_NORMALIZENORMALS				= 143,
	D3DRS_DIFFUSEMATERIALSOURCE			= 145,
	D3DRS_SPECULARMATERIALSOURCE		= 146,
	D3DRS_AMBIENTMATERIALSOURCE			= 147,
	D3DRS_EMISSIVEMATERIALSOURCE		= 148,
	D3DRS_VERTEXBLEND					= 151,
	D3DRS_CLIPPLANEENABLE				= 152,
	D3DRS_POINTSIZE						= 154,
	D3DRS_POINTSIZE_MIN					= 155,
	D3DRS_POINTSPRITEENABLE				= 156,
	D3DRS_POINTSCALEENABLE				= 157,
	D3DRS_POINTSCALE_A					= 158,
	D3DRS_POINTSCALE_B					= 159,
	D3DRS_POINTSCALE_C					= 160,
	D3DRS_MULTISAMPLEANTIALIAS			= 161,
	D3DRS_MULTISAMPLEMASK				= 162,
	D3DRS_PATCHEDGESTYLE				= 163,
	D3DRS_DEBUGMONITORTOKEN				= 165,
	D3DRS_POINTSIZE_MAX					= 166,
	D3DRS_INDEXEDVERTEXBLENDENABLE		= 167,
	D3DRS_COLORWRITEENABLE				= 168,
	D3DRS_TWEENFACTOR					= 170,
	D3DRS_BLENDOP						= 171,
	D3DRS_POSITIONDEGREE				= 172,
	D3DRS_NORMALDEGREE					= 173,
	D3DRS_SCISSORTESTENABLE				= 174,
	D3DRS_SLOPESCALEDEPTHBIAS			= 175,
	D3DRS_ANTIALIASEDLINEENABLE			= 176,
	D3DRS_MINTESSELLATIONLEVEL			= 178,
	D3DRS_MAXTESSELLATIONLEVEL			= 179,
	D3DRS_ADAPTIVETESS_X				= 180,
	D3DRS_ADAPTIVETESS_Y				= 181,
	D3DRS_ADAPTIVETESS_Z				= 182,
	D3DRS_ADAPTIVETESS_W				= 183,
	D3DRS_ENABLEADAPTIVETESSELLATION	= 184,
	D3DRS_TWOSIDEDSTENCILMODE			= 185,
	D3DRS_CCW_STENCILFAIL				= 186,
	D3DRS_CCW_STENCILZFAIL				= 187,
	D3DRS_CCW_STENCILPASS				= 188,
	D3DRS_CCW_STENCILFUNC				= 189,
	D3DRS_COLORWRITEENABLE1				= 190,
	D3DRS_COLORWRITEENABLE2				= 191,
	D3DRS_COLORWRITEENABLE3				= 192,
	D3DRS_BLENDFACTOR					= 193,
	D3DRS_SRGBWRITEENABLE				= 194,
	D3DRS_DEPTHBIAS						= 195,
	D3DRS_WRAP8							= 198,
	D3DRS_WRAP9							= 199,
	D3DRS_WRAP10						= 200,
	D3DRS_WRAP11						= 201,
	D3DRS_WRAP12						= 202,
	D3DRS_WRAP13						= 203,
	D3DRS_WRAP14						= 204,
	D3DRS_WRAP15						= 205,
	D3DRS_SEPARATEALPHABLENDENABLE		= 206,
	D3DRS_SRCBLENDALPHA					= 207,
	D3DRS_DESTBLENDALPHA				= 208,
	D3DRS_BLENDOPALPHA					= 209,
	D3DRS_FORCE_DWORD					= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DSTATEBLOCKTYPE
{
	D3DSBT_ALL			= 1,
	D3DSBT_PIXELSTATE	= 2,
	D3DSBT_VERTEXSTATE	= 3,
	D3DSBT_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DTEXTURESTAGESTATETYPE
{
	D3DTSS_COLOROP					=  1,
	D3DTSS_COLORARG1				=  2,
	D3DTSS_COLORARG2				=  3,
	D3DTSS_ALPHAOP					=  4,
	D3DTSS_ALPHAARG1				=  5,
	D3DTSS_ALPHAARG2				=  6,
	D3DTSS_BUMPENVMAT00				=  7,
	D3DTSS_BUMPENVMAT01				=  8,
	D3DTSS_BUMPENVMAT10				=  9,
	D3DTSS_BUMPENVMAT11				= 10,
	D3DTSS_TEXCOORDINDEX			= 11,
	D3DTSS_BUMPENVLSCALE			= 22,
	D3DTSS_BUMPENVLOFFSET			= 23,
	D3DTSS_TEXTURETRANSFORMFLAGS	= 24,
	D3DTSS_COLORARG0				= 26,
	D3DTSS_ALPHAARG0				= 27,
	D3DTSS_RESULTARG				= 28,
	D3DTSS_CONSTANT					= 32,
	D3DTSS_FORCE_DWORD				= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DSAMPLERSTATETYPE
{
	D3DSAMP_ADDRESSU		= 1,
	D3DSAMP_ADDRESSV		= 2,
	D3DSAMP_ADDRESSW		= 3,
	D3DSAMP_BORDERCOLOR		= 4,
	D3DSAMP_MAGFILTER		= 5,
	D3DSAMP_MINFILTER		= 6,
	D3DSAMP_MIPFILTER		= 7,
	D3DSAMP_MIPMAPLODBIAS	= 8,
	D3DSAMP_MAXMIPLEVEL		= 9,
	D3DSAMP_MAXANISOTROPY	= 10,
	D3DSAMP_SRGBTEXTURE		= 11,
	D3DSAMP_ELEMENTINDEX	= 12,
	D3DSAMP_DMAPOFFSET		= 13,
	D3DSAMP_FORCE_DWORD		= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DPRIMITIVETYPE
{
	D3DPT_POINTLIST		= 1,
	D3DPT_LINELIST		= 2,
	D3DPT_LINESTRIP		= 3,
	D3DPT_TRIANGLELIST	= 4,
	D3DPT_TRIANGLESTRIP	= 5,
	D3DPT_TRIANGLEFAN	= 6,
	D3DPT_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DBACKBUFFER_TYPE
{
	D3DBACKBUFFER_TYPE_MONO			= 0,
	D3DBACKBUFFER_TYPE_LEFT			= 1,
	D3DBACKBUFFER_TYPE_RIGHT		= 2,
	D3DBACKBUFFER_TYPE_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DQUERYTYPE
{
	D3DQUERYTYPE_VCACHE				= 4,
	D3DQUERYTYPE_RESOURCEMANAGER	= 5,
	D3DQUERYTYPE_VERTEXSTATS		= 6,
	D3DQUERYTYPE_EVENT				= 8,
	D3DQUERYTYPE_OCCLUSION			= 9,
	D3DQUERYTYPE_TIMESTAMP			= 10,
	D3DQUERYTYPE_TIMESTAMPDISJOINT	= 11,
	D3DQUERYTYPE_TIMESTAMPFREQ		= 12,
	D3DQUERYTYPE_PIPELINETIMINGS	= 13,
	D3DQUERYTYPE_INTERFACETIMINGS	= 14,
	D3DQUERYTYPE_VERTEXTIMINGS		= 15,
	D3DQUERYTYPE_PIXELTIMINGS		= 16,
	D3DQUERYTYPE_BANDWIDTHTIMINGS	= 17,
	D3DQUERYTYPE_CACHEUTILIZATION	= 18,
	D3DQUERYTYPE_MEMORYPRESSURE		= 19
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DBLEND
{
	D3DBLEND_ZERO				= 1,
	D3DBLEND_ONE				= 2,
	D3DBLEND_SRCCOLOR			= 3,
	D3DBLEND_INVSRCCOLOR		= 4,
	D3DBLEND_SRCALPHA			= 5,
	D3DBLEND_INVSRCALPHA		= 6,
	D3DBLEND_DESTALPHA			= 7,
	D3DBLEND_INVDESTALPHA		= 8,
	D3DBLEND_DESTCOLOR			= 9,
	D3DBLEND_INVDESTCOLOR		= 10,
	D3DBLEND_SRCALPHASAT		= 11,
	D3DBLEND_BOTHSRCALPHA		= 12,
	D3DBLEND_BOTHINVSRCALPHA	= 13,
	D3DBLEND_BLENDFACTOR		= 14,
	D3DBLEND_INVBLENDFACTOR		= 15,
	D3DBLEND_SRCCOLOR2			= 16,
	D3DBLEND_INVSRCCOLOR2		= 17,
	D3DBLEND_FORCE_DWORD		= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DRESOURCETYPE
{
	D3DRTYPE_SURFACE		=  1,
	D3DRTYPE_VOLUME			=  2,
	D3DRTYPE_TEXTURE		=  3,
	D3DRTYPE_VOLUMETEXTURE	=  4,
	D3DRTYPE_CUBETEXTURE	=  5,
	D3DRTYPE_VERTEXBUFFER	=  6,
	D3DRTYPE_INDEXBUFFER	=  7,
	D3DRTYPE_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DSWAPEFFECT
{
	D3DSWAPEFFECT_DISCARD		= 1,
	D3DSWAPEFFECT_FLIP			= 2,
	D3DSWAPEFFECT_COPY			= 3,
	D3DSWAPEFFECT_OVERLAY		= 4,
	D3DSWAPEFFECT_FLIPEX		= 5,
	D3DSWAPEFFECT_FORCE_DWORD	= 0x7fffffff
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
struct D3DPRESENT_PARAMETERS
{
	UINT				BackBufferWidth;
	UINT				BackBufferHeight;
	D3DFORMAT			BackBufferFormat;
	UINT				BackBufferCount;
	D3DMULTISAMPLE_TYPE	MultiSampleType;
	DWORD				MultiSampleQuality;
	D3DSWAPEFFECT		SwapEffect;
	HWND				hDeviceWindow;
	BOOL				Windowed;
	BOOL				EnableAutoDepthStencil;
	D3DFORMAT			AutoDepthStencilFormat;
	DWORD				Flags;
	UINT				FullScreen_RefreshRateInHz;
	UINT				PresentationInterval;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
struct D3DLOCKED_RECT
{
	INT	  Pitch;
	void *pBits;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
struct D3DVIEWPORT9
{
	DWORD	X;
	DWORD	Y;
	DWORD	Width;
	DWORD	Height;
	float	MinZ;
	float	MaxZ;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
struct D3DSURFACE_DESC
{
	D3DFORMAT			Format;
	D3DRESOURCETYPE		Type;
	DWORD				Usage;
	D3DPOOL				Pool;
	D3DMULTISAMPLE_TYPE	MultiSampleType;
	DWORD				MultiSampleQuality;
	UINT				Width;
	UINT				Height;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
typedef struct IDirect3DSurface9 *LPDIRECT3DSURFACE9;

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
enum D3DDECLMETHOD
{
	D3DDECLMETHOD_DEFAULT = 0,
	D3DDECLMETHOD_PARTIALU,
	D3DDECLMETHOD_PARTIALV,
	D3DDECLMETHOD_CROSSUV,
	D3DDECLMETHOD_UV,
	D3DDECLMETHOD_LOOKUP,
	D3DDECLMETHOD_LOOKUPPRESAMPLED
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9types.h"
struct D3DVERTEXELEMENT9
{
	WORD	Stream;
	WORD	Offset;
	BYTE	Type;
	BYTE	Method;
	BYTE	Usage;
	BYTE	UsageIndex;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9xshader.h"
typedef __interface ID3DXInclude *LPD3DXINCLUDE;
#ifndef D3DXFX_LARGEADDRESS_HANDLE
	typedef LPCSTR D3DXHANDLE;
#else
	typedef UINT_PTR D3DXHANDLE;
#endif
typedef D3DXHANDLE *LPD3DXHANDLE;

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9caps.h"
struct D3DVSHADERCAPS2_0
{
	DWORD Caps;
	INT DynamicFlowControlDepth;
	INT NumTemps;
	INT StaticFlowControlDepth;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9caps.h"
struct D3DPSHADERCAPS2_0
{
	DWORD Caps;
	INT DynamicFlowControlDepth;
	INT NumTemps;
	INT StaticFlowControlDepth;
	INT NumInstructionSlots;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9caps.h"
struct D3DCAPS9
{
	D3DDEVTYPE			DeviceType;
	UINT				AdapterOrdinal;
	DWORD				Caps;
	DWORD				Caps2;
	DWORD				Caps3;
	DWORD				PresentationIntervals;
	DWORD				CursorCaps;
	DWORD				DevCaps;
	DWORD				PrimitiveMiscCaps;
	DWORD				RasterCaps;
	DWORD				ZCmpCaps;
	DWORD				SrcBlendCaps;
	DWORD				DestBlendCaps;
	DWORD				AlphaCmpCaps;
	DWORD				ShadeCaps;
	DWORD				TextureCaps;
	DWORD				TextureFilterCaps;
	DWORD				CubeTextureFilterCaps;
	DWORD				VolumeTextureFilterCaps;
	DWORD				TextureAddressCaps;
	DWORD				VolumeTextureAddressCaps;
	DWORD				LineCaps;
	DWORD				MaxTextureWidth, MaxTextureHeight;
	DWORD				MaxVolumeExtent;
	DWORD				MaxTextureRepeat;
	DWORD				MaxTextureAspectRatio;
	DWORD				MaxAnisotropy;
	float				MaxVertexW;
	float				GuardBandLeft;
	float				GuardBandTop;
	float				GuardBandRight;
	float				GuardBandBottom;
	float				ExtentsAdjust;
	DWORD				StencilCaps;
	DWORD				FVFCaps;
	DWORD				TextureOpCaps;
	DWORD				MaxTextureBlendStages;
	DWORD				MaxSimultaneousTextures;
	DWORD				VertexProcessingCaps;
	DWORD				MaxActiveLights;
	DWORD				MaxUserClipPlanes;
	DWORD				MaxVertexBlendMatrices;
	DWORD				MaxVertexBlendMatrixIndex;
	float				MaxPointSize;
	DWORD				MaxPrimitiveCount;
	DWORD				MaxVertexIndex;
	DWORD				MaxStreams;
	DWORD				MaxStreamStride;
	DWORD				VertexShaderVersion;
	DWORD				MaxVertexShaderConst;
	DWORD				PixelShaderVersion;
	float				PixelShader1xMaxValue;
	DWORD				DevCaps2;
	float				MaxNpatchTessellationLevel;
	DWORD				Reserved5;
	UINT				MasterAdapterOrdinal;
	UINT				AdapterOrdinalInGroup;
	UINT				NumberOfAdaptersInGroup;
	DWORD				DeclTypes;
	DWORD				NumSimultaneousRTs;
	DWORD				StretchRectFilterCaps;
	D3DVSHADERCAPS2_0	VS20Caps;
	D3DPSHADERCAPS2_0	PS20Caps;
	DWORD				VertexTextureFilterCaps;
	DWORD				MaxVShaderInstructionsExecuted;
	DWORD				MaxPShaderInstructionsExecuted;
	DWORD				MaxVertexShader30InstructionSlots;
	DWORD				MaxPixelShader30InstructionSlots;
};


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3D9, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(RegisterSoftwareDevice)(THIS_ void* pInitializeFunction) PURE;
	STDMETHOD_(UINT, GetAdapterCount)(THIS) PURE;
	STDMETHOD(GetAdapterIdentifier)(THIS_ UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier) PURE;
	STDMETHOD_(UINT, GetAdapterModeCount)(THIS_ UINT Adapter,D3DFORMAT Format) PURE;
	STDMETHOD(EnumAdapterModes)(THIS_ UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode) PURE;
	STDMETHOD(GetAdapterDisplayMode)(THIS_ UINT Adapter,D3DDISPLAYMODE* pMode) PURE;
	STDMETHOD(CheckDeviceType)(THIS_ UINT Adapter,D3DDEVTYPE DevType,D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed) PURE;
	STDMETHOD(CheckDeviceFormat)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat) PURE;
	STDMETHOD(CheckDeviceMultiSampleType)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels) PURE;
	STDMETHOD(CheckDepthStencilMatch)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat) PURE;
	STDMETHOD(CheckDeviceFormatConversion)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat) PURE;
	STDMETHOD(GetDeviceCaps)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps) PURE;
	STDMETHOD_(HMONITOR, GetAdapterMonitor)(THIS_ UINT Adapter) PURE;
	STDMETHOD(CreateDevice)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DDevice9, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(TestCooperativeLevel)(THIS) PURE;
	STDMETHOD_(UINT, GetAvailableTextureMem)(THIS) PURE;
	STDMETHOD(EvictManagedResources)(THIS) PURE;
	STDMETHOD(GetDirect3D)(THIS_ IDirect3D9** ppD3D9) PURE;
	STDMETHOD(GetDeviceCaps)(THIS_ D3DCAPS9* pCaps) PURE;
	STDMETHOD(GetDisplayMode)(THIS_ UINT iSwapChain,D3DDISPLAYMODE* pMode) PURE;
	STDMETHOD(GetCreationParameters)(THIS_ D3DDEVICE_CREATION_PARAMETERS *pParameters) PURE;
	STDMETHOD(SetCursorProperties)(THIS_ UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap) PURE;
	STDMETHOD_(void, SetCursorPosition)(THIS_ int X,int Y,DWORD Flags) PURE;
	STDMETHOD_(BOOL, ShowCursor)(THIS_ BOOL bShow) PURE;
	STDMETHOD(CreateAdditionalSwapChain)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain) PURE;
	STDMETHOD(GetSwapChain)(THIS_ UINT iSwapChain,IDirect3DSwapChain9** pSwapChain) PURE;
	STDMETHOD_(UINT, GetNumberOfSwapChains)(THIS) PURE;
	STDMETHOD(Reset)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters) PURE;
	STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) PURE;
	STDMETHOD(GetBackBuffer)(THIS_ UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer) PURE;
	STDMETHOD(GetRasterStatus)(THIS_ UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus) PURE;
	STDMETHOD(SetDialogBoxMode)(THIS_ BOOL bEnableDialogs) PURE;
	STDMETHOD_(void, SetGammaRamp)(THIS_ UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp) PURE;
	STDMETHOD_(void, GetGammaRamp)(THIS_ UINT iSwapChain,D3DGAMMARAMP* pRamp) PURE;
	STDMETHOD(CreateTexture)(THIS_ UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle) PURE;
	STDMETHOD(CreateVolumeTexture)(THIS_ UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle) PURE;
	STDMETHOD(CreateCubeTexture)(THIS_ UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle) PURE;
	STDMETHOD(CreateVertexBuffer)(THIS_ UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle) PURE;
	STDMETHOD(CreateIndexBuffer)(THIS_ UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle) PURE;
	STDMETHOD(CreateRenderTarget)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) PURE;
	STDMETHOD(CreateDepthStencilSurface)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) PURE;
	STDMETHOD(UpdateSurface)(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint) PURE;
	STDMETHOD(UpdateTexture)(THIS_ IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture) PURE;
	STDMETHOD(GetRenderTargetData)(THIS_ IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface) PURE;
	STDMETHOD(GetFrontBufferData)(THIS_ UINT iSwapChain,IDirect3DSurface9* pDestSurface) PURE;
	STDMETHOD(StretchRect)(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter) PURE;
	STDMETHOD(ColorFill)(THIS_ IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color) PURE;
	STDMETHOD(CreateOffscreenPlainSurface)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) PURE;
	STDMETHOD(SetRenderTarget)(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget) PURE;
	STDMETHOD(GetRenderTarget)(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget) PURE;
	STDMETHOD(SetDepthStencilSurface)(THIS_ IDirect3DSurface9* pNewZStencil) PURE;
	STDMETHOD(GetDepthStencilSurface)(THIS_ IDirect3DSurface9** ppZStencilSurface) PURE;
	STDMETHOD(BeginScene)(THIS) PURE;
	STDMETHOD(EndScene)(THIS) PURE;
	STDMETHOD(Clear)(THIS_ DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil) PURE;
	STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix) PURE;
	STDMETHOD(GetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix) PURE;
	STDMETHOD(MultiplyTransform)(THIS_ D3DTRANSFORMSTATETYPE,CONST D3DMATRIX*) PURE;
	STDMETHOD(SetViewport)(THIS_ CONST D3DVIEWPORT9* pViewport) PURE;
	STDMETHOD(GetViewport)(THIS_ D3DVIEWPORT9* pViewport) PURE;
	STDMETHOD(SetMaterial)(THIS_ CONST D3DMATERIAL9* pMaterial) PURE;
	STDMETHOD(GetMaterial)(THIS_ D3DMATERIAL9* pMaterial) PURE;
	STDMETHOD(SetLight)(THIS_ DWORD Index,CONST D3DLIGHT9*) PURE;
	STDMETHOD(GetLight)(THIS_ DWORD Index,D3DLIGHT9*) PURE;
	STDMETHOD(LightEnable)(THIS_ DWORD Index,BOOL Enable) PURE;
	STDMETHOD(GetLightEnable)(THIS_ DWORD Index,BOOL* pEnable) PURE;
	STDMETHOD(SetClipPlane)(THIS_ DWORD Index,CONST float* pPlane) PURE;
	STDMETHOD(GetClipPlane)(THIS_ DWORD Index,float* pPlane) PURE;
	STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD Value) PURE;
	STDMETHOD(GetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD* pValue) PURE;
	STDMETHOD(CreateStateBlock)(THIS_ D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB) PURE;
	STDMETHOD(BeginStateBlock)(THIS) PURE;
	STDMETHOD(EndStateBlock)(THIS_ IDirect3DStateBlock9** ppSB) PURE;
	STDMETHOD(SetClipStatus)(THIS_ CONST D3DCLIPSTATUS9* pClipStatus) PURE;
	STDMETHOD(GetClipStatus)(THIS_ D3DCLIPSTATUS9* pClipStatus) PURE;
	STDMETHOD(GetTexture)(THIS_ DWORD Stage,IDirect3DBaseTexture9** ppTexture) PURE;
	STDMETHOD(SetTexture)(THIS_ DWORD Stage,IDirect3DBaseTexture9* pTexture) PURE;
	STDMETHOD(GetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue) PURE;
	STDMETHOD(SetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value) PURE;
	STDMETHOD(GetSamplerState)(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue) PURE;
	STDMETHOD(SetSamplerState)(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value) PURE;
	STDMETHOD(ValidateDevice)(THIS_ DWORD* pNumPasses) PURE;
	STDMETHOD(SetPaletteEntries)(THIS_ UINT PaletteNumber,CONST PALETTEENTRY* pEntries) PURE;
	STDMETHOD(GetPaletteEntries)(THIS_ UINT PaletteNumber,PALETTEENTRY* pEntries) PURE;
	STDMETHOD(SetCurrentTexturePalette)(THIS_ UINT PaletteNumber) PURE;
	STDMETHOD(GetCurrentTexturePalette)(THIS_ UINT *PaletteNumber) PURE;
	STDMETHOD(SetScissorRect)(THIS_ CONST RECT* pRect) PURE;
	STDMETHOD(GetScissorRect)(THIS_ RECT* pRect) PURE;
	STDMETHOD(SetSoftwareVertexProcessing)(THIS_ BOOL bSoftware) PURE;
	STDMETHOD_(BOOL, GetSoftwareVertexProcessing)(THIS) PURE;
	STDMETHOD(SetNPatchMode)(THIS_ float nSegments) PURE;
	STDMETHOD_(float, GetNPatchMode)(THIS) PURE;
	STDMETHOD(DrawPrimitive)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount) PURE;
	STDMETHOD(DrawIndexedPrimitive)(THIS_ D3DPRIMITIVETYPE,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount) PURE;
	STDMETHOD(DrawPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride) PURE;
	STDMETHOD(DrawIndexedPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride) PURE;
	STDMETHOD(ProcessVertices)(THIS_ UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags) PURE;
	STDMETHOD(CreateVertexDeclaration)(THIS_ CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl) PURE;
	STDMETHOD(SetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9* pDecl) PURE;
	STDMETHOD(GetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9** ppDecl) PURE;
	STDMETHOD(SetFVF)(THIS_ DWORD FVF) PURE;
	STDMETHOD(GetFVF)(THIS_ DWORD* pFVF) PURE;
	STDMETHOD(CreateVertexShader)(THIS_ CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader) PURE;
	STDMETHOD(SetVertexShader)(THIS_ IDirect3DVertexShader9* pShader) PURE;
	STDMETHOD(GetVertexShader)(THIS_ IDirect3DVertexShader9** ppShader) PURE;
	STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) PURE;
	STDMETHOD(GetVertexShaderConstantF)(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount) PURE;
	STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) PURE;
	STDMETHOD(GetVertexShaderConstantI)(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount) PURE;
	STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) PURE;
	STDMETHOD(GetVertexShaderConstantB)(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) PURE;
	STDMETHOD(SetStreamSource)(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride) PURE;
	STDMETHOD(GetStreamSource)(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride) PURE;
	STDMETHOD(SetStreamSourceFreq)(THIS_ UINT StreamNumber,UINT Setting) PURE;
	STDMETHOD(GetStreamSourceFreq)(THIS_ UINT StreamNumber,UINT* pSetting) PURE;
	STDMETHOD(SetIndices)(THIS_ IDirect3DIndexBuffer9* pIndexData) PURE;
	STDMETHOD(GetIndices)(THIS_ IDirect3DIndexBuffer9** ppIndexData) PURE;
	STDMETHOD(CreatePixelShader)(THIS_ CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader) PURE;
	STDMETHOD(SetPixelShader)(THIS_ IDirect3DPixelShader9* pShader) PURE;
	STDMETHOD(GetPixelShader)(THIS_ IDirect3DPixelShader9** ppShader) PURE;
	STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) PURE;
	STDMETHOD(GetPixelShaderConstantF)(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount) PURE;
	STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) PURE;
	STDMETHOD(GetPixelShaderConstantI)(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount) PURE;
	STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) PURE;
	STDMETHOD(GetPixelShaderConstantB)(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) PURE;
	STDMETHOD(DrawRectPatch)(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo) PURE;
	STDMETHOD(DrawTriPatch)(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo) PURE;
	STDMETHOD(DeletePatch)(THIS_ UINT Handle) PURE;
	STDMETHOD(CreateQuery)(THIS_ D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery) PURE;
};
typedef struct IDirect3DDevice9 *LPDIRECT3DDEVICE9;

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DVertexDeclaration9, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD(GetDeclaration)(THIS_ D3DVERTEXELEMENT9* pElement,UINT* pNumElements) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DVertexShader9, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD(GetFunction)(THIS_ void*,UINT* pSizeOfData) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DPixelShader9, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD(GetFunction)(THIS_ void*,UINT* pSizeOfData) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DResource9, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD(SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) PURE;
	STDMETHOD(GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) PURE;
	STDMETHOD(FreePrivateData)(THIS_ REFGUID refguid) PURE;
	STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
	STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
	STDMETHOD_(void, PreLoad)(THIS) PURE;
	STDMETHOD_(D3DRESOURCETYPE, GetType)(THIS) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DIndexBuffer9, IDirect3DResource9)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD(SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) PURE;
	STDMETHOD(GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) PURE;
	STDMETHOD(FreePrivateData)(THIS_ REFGUID refguid) PURE;
	STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
	STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
	STDMETHOD_(void, PreLoad)(THIS) PURE;
	STDMETHOD_(D3DRESOURCETYPE, GetType)(THIS) PURE;
	STDMETHOD(Lock)(THIS_ UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags) PURE;
	STDMETHOD(Unlock)(THIS) PURE;
	STDMETHOD(GetDesc)(THIS_ D3DINDEXBUFFER_DESC *pDesc) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DVertexBuffer9, IDirect3DResource9)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD(SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) PURE;
	STDMETHOD(GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) PURE;
	STDMETHOD(FreePrivateData)(THIS_ REFGUID refguid) PURE;
	STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
	STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
	STDMETHOD_(void, PreLoad)(THIS) PURE;
	STDMETHOD_(D3DRESOURCETYPE, GetType)(THIS) PURE;
	STDMETHOD(Lock)(THIS_ UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags) PURE;
	STDMETHOD(Unlock)(THIS) PURE;
	STDMETHOD(GetDesc)(THIS_ D3DVERTEXBUFFER_DESC *pDesc) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DSurface9, IDirect3DResource9)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD(SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) PURE;
	STDMETHOD(GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) PURE;
	STDMETHOD(FreePrivateData)(THIS_ REFGUID refguid) PURE;
	STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
	STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
	STDMETHOD_(void, PreLoad)(THIS) PURE;
	STDMETHOD_(D3DRESOURCETYPE, GetType)(THIS) PURE;
	STDMETHOD(GetContainer)(THIS_ REFIID riid,void** ppContainer) PURE;
	STDMETHOD(GetDesc)(THIS_ D3DSURFACE_DESC *pDesc) PURE;
	STDMETHOD(LockRect)(THIS_ D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) PURE;
	STDMETHOD(UnlockRect)(THIS) PURE;
	STDMETHOD(GetDC)(THIS_ HDC *phdc) PURE;
	STDMETHOD(ReleaseDC)(THIS_ HDC hdc) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DBaseTexture9, IDirect3DResource9)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD(SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) PURE;
	STDMETHOD(GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) PURE;
	STDMETHOD(FreePrivateData)(THIS_ REFGUID refguid) PURE;
	STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
	STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
	STDMETHOD_(void, PreLoad)(THIS) PURE;
	STDMETHOD_(D3DRESOURCETYPE, GetType)(THIS) PURE;
	STDMETHOD_(DWORD, SetLOD)(THIS_ DWORD LODNew) PURE;
	STDMETHOD_(DWORD, GetLOD)(THIS) PURE;
	STDMETHOD_(DWORD, GetLevelCount)(THIS) PURE;
	STDMETHOD(SetAutoGenFilterType)(THIS_ D3DTEXTUREFILTERTYPE FilterType) PURE;
	STDMETHOD_(D3DTEXTUREFILTERTYPE, GetAutoGenFilterType)(THIS) PURE;
	STDMETHOD_(void, GenerateMipSubLevels)(THIS) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DTexture9, IDirect3DBaseTexture9)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD(SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) PURE;
	STDMETHOD(GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) PURE;
	STDMETHOD(FreePrivateData)(THIS_ REFGUID refguid) PURE;
	STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
	STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
	STDMETHOD_(void, PreLoad)(THIS) PURE;
	STDMETHOD_(D3DRESOURCETYPE, GetType)(THIS) PURE;
	STDMETHOD_(DWORD, SetLOD)(THIS_ DWORD LODNew) PURE;
	STDMETHOD_(DWORD, GetLOD)(THIS) PURE;
	STDMETHOD_(DWORD, GetLevelCount)(THIS) PURE;
	STDMETHOD(SetAutoGenFilterType)(THIS_ D3DTEXTUREFILTERTYPE FilterType) PURE;
	STDMETHOD_(D3DTEXTUREFILTERTYPE, GetAutoGenFilterType)(THIS) PURE;
	STDMETHOD_(void, GenerateMipSubLevels)(THIS) PURE;
	STDMETHOD(GetLevelDesc)(THIS_ UINT Level,D3DSURFACE_DESC *pDesc) PURE;
	STDMETHOD(GetSurfaceLevel)(THIS_ UINT Level,IDirect3DSurface9** ppSurfaceLevel) PURE;
	STDMETHOD(LockRect)(THIS_ UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) PURE;
	STDMETHOD(UnlockRect)(THIS_ UINT Level) PURE;
	STDMETHOD(AddDirtyRect)(THIS_ CONST RECT* pDirtyRect) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9xcore.h"
DECLARE_INTERFACE_(ID3DXBuffer, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	STDMETHOD_(LPVOID, GetBufferPointer)(THIS) PURE;
	STDMETHOD_(DWORD, GetBufferSize)(THIS) PURE;
};
typedef __interface ID3DXBuffer *LPD3DXBUFFER;

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9xshader.h"
DECLARE_INTERFACE_(ID3DXConstantTable, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	STDMETHOD_(LPVOID, GetBufferPointer)(THIS) PURE;
	STDMETHOD_(DWORD, GetBufferSize)(THIS) PURE;
	STDMETHOD(GetDesc)(THIS_ D3DXCONSTANTTABLE_DESC *pDesc) PURE;
	STDMETHOD(GetConstantDesc)(THIS_ D3DXHANDLE hConstant, D3DXCONSTANT_DESC *pConstantDesc, UINT *pCount) PURE;
	STDMETHOD_(UINT, GetSamplerIndex)(THIS_ D3DXHANDLE hConstant) PURE;
	STDMETHOD_(D3DXHANDLE, GetConstant)(THIS_ D3DXHANDLE hConstant, UINT Index) PURE;
	STDMETHOD_(D3DXHANDLE, GetConstantByName)(THIS_ D3DXHANDLE hConstant, LPCSTR pName) PURE;
	STDMETHOD_(D3DXHANDLE, GetConstantElement)(THIS_ D3DXHANDLE hConstant, UINT Index) PURE;
	STDMETHOD(SetDefaults)(THIS_ LPDIRECT3DDEVICE9 pDevice) PURE;
	STDMETHOD(SetValue)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, LPCVOID pData, UINT Bytes) PURE;
	STDMETHOD(SetBool)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, BOOL b) PURE;
	STDMETHOD(SetBoolArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST BOOL* pb, UINT Count) PURE;
	STDMETHOD(SetInt)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, INT n) PURE;
	STDMETHOD(SetIntArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST INT* pn, UINT Count) PURE;
	STDMETHOD(SetFloat)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, FLOAT f) PURE;
	STDMETHOD(SetFloatArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST FLOAT* pf, UINT Count) PURE;
	STDMETHOD(SetVector)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXVECTOR4* pVector) PURE;
	STDMETHOD(SetVectorArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXVECTOR4* pVector, UINT Count) PURE;
	STDMETHOD(SetMatrix)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX* pMatrix) PURE;
	STDMETHOD(SetMatrixArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
	STDMETHOD(SetMatrixPointerArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
	STDMETHOD(SetMatrixTranspose)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX* pMatrix) PURE;
	STDMETHOD(SetMatrixTransposeArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
	STDMETHOD(SetMatrixTransposePointerArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
};
typedef __interface ID3DXConstantTable *LPD3DXCONSTANTTABLE;

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DQuery9, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD_(D3DQUERYTYPE, GetType)(THIS) PURE;
	STDMETHOD_(DWORD, GetDataSize)(THIS) PURE;
	STDMETHOD(Issue)(THIS_ DWORD dwIssueFlags) PURE;
	STDMETHOD(GetData)(THIS_ void* pData,DWORD dwSize,DWORD dwGetDataFlags) PURE;
};

// "Microsoft Direct3D SDK (June 2010)" -> "d3d9.h"
DECLARE_INTERFACE_(IDirect3DSwapChain9, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags) PURE;
	STDMETHOD(GetFrontBufferData)(THIS_ IDirect3DSurface9* pDestSurface) PURE;
	STDMETHOD(GetBackBuffer)(THIS_ UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer) PURE;
	STDMETHOD(GetRasterStatus)(THIS_ D3DRASTER_STATUS* pRasterStatus) PURE;
	STDMETHOD(GetDisplayMode)(THIS_ D3DDISPLAYMODE* pMode) PURE;
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
	STDMETHOD(GetPresentParameters)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters) PURE;
};


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D9RENDERER_D3D9_H__
