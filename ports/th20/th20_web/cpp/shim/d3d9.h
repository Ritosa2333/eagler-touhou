// Direct3D 9 surface for the Emscripten/SDL3 (TH_SDL3) build.
// Types keep the real DirectX SDK x86 layouts (struct sizes are load-bearing:
// the reconstructed sources static_assert on them). COM interfaces declare
// only the members the reconstructed sources actually call; the wasm renderer
// host (Phase 2/3) provides the implementations. Declaring an interface here
// does not claim an implementation exists.
#pragma once

#include "windows.h"

// --- basic value types ------------------------------------------------------
typedef DWORD D3DCOLOR;

typedef struct _D3DVECTOR { float x, y, z; } D3DVECTOR;
typedef struct _D3DMATRIX {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} D3DMATRIX;
typedef struct _D3DRECT { LONG x1, y1, x2, y2; } D3DRECT;
typedef struct _D3DVIEWPORT9 {
    DWORD X, Y, Width, Height;
    float MinZ, MaxZ;
} D3DVIEWPORT9;
typedef struct _D3DLOCKED_RECT { INT Pitch; void* pBits; } D3DLOCKED_RECT;
typedef struct _D3DRASTER_STATUS { BOOL InVBlank; UINT ScanLine; } D3DRASTER_STATUS;
typedef struct tagPALETTEENTRY { BYTE peRed, peGreen, peBlue, peFlags; } PALETTEENTRY;

// --- enums / constants (real SDK values) ------------------------------------
typedef enum _D3DFORMAT {
    D3DFMT_UNKNOWN = 0,
    D3DFMT_R8G8B8 = 20,
    D3DFMT_A8R8G8B8 = 21,
    D3DFMT_X8R8G8B8 = 22,
    D3DFMT_R5G6B5 = 23,
    D3DFMT_X1R5G5B5 = 24,
    D3DFMT_A1R5G5B5 = 25,
    D3DFMT_A4R4G4B4 = 26,
    D3DFMT_R3G3B2 = 27,
    D3DFMT_A8 = 28,
    D3DFMT_A8R3G3B2 = 29,
    D3DFMT_X4R4G4B4 = 30,
    D3DFMT_A2B10G10R10 = 31,
    D3DFMT_A8B8G8R8 = 32,
    D3DFMT_X8B8G8R8 = 33,
    D3DFMT_G16R16 = 34,
    D3DFMT_A2R10G10B10 = 35,
    D3DFMT_A16B16G16R16 = 36,
    D3DFMT_D16_LOCKABLE = 70,
    D3DFMT_D32 = 71,
    D3DFMT_D15S1 = 73,
    D3DFMT_D24S8 = 75,
    D3DFMT_D24X8 = 77,
    D3DFMT_D24X4S4 = 79,
    D3DFMT_D16 = 80,
    D3DFMT_VERTEXDATA = 100,
    D3DFMT_INDEX16 = 101,
    D3DFMT_INDEX32 = 102,
    D3DFMT_DXT1 = 0x31545844, // 'DXT1'
    D3DFMT_DXT3 = 0x33545844, // 'DXT3'
    D3DFMT_DXT5 = 0x35545844, // 'DXT5'
} D3DFORMAT;

typedef enum _D3DPRIMITIVETYPE {
    D3DPT_POINTLIST = 1,
    D3DPT_LINELIST = 2,
    D3DPT_LINESTRIP = 3,
    D3DPT_TRIANGLELIST = 4,
    D3DPT_TRIANGLESTRIP = 5,
    D3DPT_TRIANGLEFAN = 6,
} D3DPRIMITIVETYPE;

typedef enum _D3DRESOURCETYPE {
    D3DRTYPE_SURFACE = 1,
    D3DRTYPE_VOLUME = 2,
    D3DRTYPE_TEXTURE = 3,
    D3DRTYPE_VOLUMETEXTURE = 4,
    D3DRTYPE_CUBETEXTURE = 5,
    D3DRTYPE_VERTEXBUFFER = 6,
    D3DRTYPE_INDEXBUFFER = 7,
} D3DRESOURCETYPE;

typedef enum _D3DRENDERSTATETYPE {
    D3DRS_ZENABLE = 7,
    D3DRS_FILLMODE = 8,
    D3DRS_SHADEMODE = 9,
    D3DRS_ZWRITEENABLE = 14,
    D3DRS_ALPHATESTENABLE = 15,
    D3DRS_LASTPIXEL = 16,
    D3DRS_SRCBLEND = 19,
    D3DRS_DESTBLEND = 20,
    D3DRS_CULLMODE = 22,
    D3DRS_ZFUNC = 23,
    D3DRS_ALPHAREF = 24,
    D3DRS_ALPHAFUNC = 25,
    D3DRS_DITHERENABLE = 26,
    D3DRS_ALPHABLENDENABLE = 27,
    D3DRS_FOGENABLE = 28,
    D3DRS_SPECULARENABLE = 29,
    D3DRS_FOGCOLOR = 34,
    D3DRS_FOGTABLEMODE = 35,
    D3DRS_FOGSTART = 36,
    D3DRS_FOGEND = 37,
    D3DRS_FOGDENSITY = 38,
    D3DRS_RANGEFOGENABLE = 48,
    D3DRS_STENCILENABLE = 52,
    D3DRS_STENCILFAIL = 53,
    D3DRS_STENCILZFAIL = 54,
    D3DRS_STENCILPASS = 55,
    D3DRS_STENCILFUNC = 56,
    D3DRS_STENCILREF = 57,
    D3DRS_STENCILMASK = 58,
    D3DRS_STENCILWRITEMASK = 59,
    D3DRS_TEXTUREFACTOR = 60,
    D3DRS_WRAP0 = 128,
    D3DRS_WRAP7 = 135,
    D3DRS_CLIPPING = 136,
    D3DRS_LIGHTING = 137,
    D3DRS_AMBIENT = 139,
    D3DRS_FOGVERTEXMODE = 140,
    D3DRS_COLORVERTEX = 141,
    D3DRS_LOCALVIEWER = 142,
    D3DRS_NORMALIZENORMALS = 143,
    D3DRS_DIFFUSEMATERIALSOURCE = 145,
    D3DRS_SPECULARMATERIALSOURCE = 146,
    D3DRS_AMBIENTMATERIALSOURCE = 147,
    D3DRS_EMISSIVEMATERIALSOURCE = 148,
    D3DRS_VERTEXBLEND = 151,
    D3DRS_CLIPPLANEENABLE = 152,
    D3DRS_POINTSIZE = 154,
    D3DRS_POINTSIZE_MIN = 155,
    D3DRS_POINTSPRITEENABLE = 156,
    D3DRS_POINTSCALEENABLE = 157,
    D3DRS_MULTISAMPLEANTIALIAS = 161,
    D3DRS_MULTISAMPLEMASK = 162,
    D3DRS_PATCHEDGESTYLE = 163,
    D3DRS_BLENDOP = 171,
    D3DRS_SCISSORTESTENABLE = 174,
    D3DRS_SLOPESCALEDEPTHBIAS = 175,
    D3DRS_ANTIALIASEDLINEENABLE = 180,
    D3DRS_TWOSIDEDSTENCILMODE = 185,
    D3DRS_CCW_STENCILFAIL = 186,
    D3DRS_CCW_STENCILZFAIL = 187,
    D3DRS_CCW_STENCILPASS = 188,
    D3DRS_CCW_STENCILFUNC = 189,
    D3DRS_COLORWRITEENABLE = 191,
    D3DRS_BLENDFACTOR = 193,
    D3DRS_SRGBWRITEENABLE = 194,
    D3DRS_DEPTHBIAS = 195,
    D3DRS_SEPARATEALPHABLENDENABLE = 206,
    D3DRS_SRCBLENDALPHA = 207,
    D3DRS_DESTBLENDALPHA = 208,
    D3DRS_BLENDOPALPHA = 209,
} D3DRENDERSTATETYPE;

typedef enum _D3DTEXTURESTAGESTATETYPE {
    D3DTSS_COLOROP = 1,
    D3DTSS_COLORARG1 = 2,
    D3DTSS_COLORARG2 = 3,
    D3DTSS_ALPHAOP = 4,
    D3DTSS_ALPHAARG1 = 5,
    D3DTSS_ALPHAARG2 = 6,
    D3DTSS_BUMPENVMAT00 = 7,
    D3DTSS_TEXTURETRANSFORMFLAGS = 24,
    D3DTSS_COLORARG0 = 26,
    D3DTSS_ALPHAARG0 = 27,
    D3DTSS_RESULTARG = 28,
} D3DTEXTURESTAGESTATETYPE;

typedef enum _D3DSAMPLERSTATETYPE {
    D3DSAMP_ADDRESSU = 1,
    D3DSAMP_ADDRESSV = 2,
    D3DSAMP_ADDRESSW = 3,
    D3DSAMP_BORDERCOLOR = 4,
    D3DSAMP_MAGFILTER = 5,
    D3DSAMP_MINFILTER = 6,
    D3DSAMP_MIPFILTER = 7,
    D3DSAMP_MIPMAPLODBIAS = 8,
    D3DSAMP_MAXMIPLEVEL = 9,
    D3DSAMP_MAXANISOTROPY = 10,
    D3DSAMP_SRGBTEXTURE = 11,
} D3DSAMPLERSTATETYPE;

typedef enum _D3DTRANSFORMSTATETYPE {
    D3DTS_VIEW = 2,
    D3DTS_PROJECTION = 3,
    D3DTS_TEXTURE0 = 16,
    D3DTS_TEXTURE7 = 23,
    D3DTS_WORLD = 256,
    D3DTS_WORLD1 = 257,
} D3DTRANSFORMSTATETYPE;

typedef enum _D3DPOOL {
    D3DPOOL_DEFAULT = 0,
    D3DPOOL_MANAGED = 1,
    D3DPOOL_SYSTEMMEM = 2,
    D3DPOOL_SCRATCH = 3,
} D3DPOOL;

typedef enum _D3DDEVTYPE { D3DDEVTYPE_HAL = 1, D3DDEVTYPE_REF = 2, D3DDEVTYPE_SW = 3 } D3DDEVTYPE;
typedef enum _D3DBACKBUFFER_TYPE { D3DBACKBUFFER_TYPE_MONO = 0 } D3DBACKBUFFER_TYPE;
typedef enum _D3DSWAPEFFECT {
    D3DSWAPEFFECT_DISCARD = 1,
    D3DSWAPEFFECT_FLIP = 2,
    D3DSWAPEFFECT_COPY = 3,
} D3DSWAPEFFECT;
typedef enum _D3DMULTISAMPLE_TYPE { D3DMULTISAMPLE_NONE = 0 } D3DMULTISAMPLE_TYPE;
typedef enum _D3DCMPFUNC {
    D3DCMP_NEVER = 1,
    D3DCMP_LESS = 2,
    D3DCMP_EQUAL = 3,
    D3DCMP_LESSEQUAL = 4,
    D3DCMP_GREATER = 5,
    D3DCMP_NOTEQUAL = 6,
    D3DCMP_GREATEREQUAL = 7,
    D3DCMP_ALWAYS = 8,
} D3DCMPFUNC;
typedef enum _D3DBLEND {
    D3DBLEND_ZERO = 1,
    D3DBLEND_ONE = 2,
    D3DBLEND_SRCCOLOR = 3,
    D3DBLEND_INVSRCCOLOR = 4,
    D3DBLEND_SRCALPHA = 5,
    D3DBLEND_INVSRCALPHA = 6,
    D3DBLEND_DESTALPHA = 7,
    D3DBLEND_INVDESTALPHA = 8,
    D3DBLEND_DESTCOLOR = 9,
    D3DBLEND_INVDESTCOLOR = 10,
    D3DBLEND_SRCALPHASAT = 11,
    D3DBLEND_BLENDFACTOR = 14,
} D3DBLEND;
typedef enum _D3DBLENDOP {
    D3DBLENDOP_ADD = 1,
    D3DBLENDOP_SUBTRACT = 2,
    D3DBLENDOP_REVSUBTRACT = 3,
    D3DBLENDOP_MIN = 4,
    D3DBLENDOP_MAX = 5,
} D3DBLENDOP;

#define D3DTOP_DISABLE 1
#define D3DTOP_SELECTARG1 2
#define D3DTOP_SELECTARG2 3
#define D3DTOP_MODULATE 4
#define D3DTA_DIFFUSE 0
#define D3DTA_CURRENT 1
#define D3DTA_TEXTURE 2

#define D3DFVF_XYZ 0x002
#define D3DFVF_XYZRHW 0x004
#define D3DFVF_NORMAL 0x010
#define D3DFVF_DIFFUSE 0x040
#define D3DFVF_SPECULAR 0x080
#define D3DFVF_TEX1 0x100
#define D3DFVF_TEX2 0x200

#define D3DCREATE_FPU_PRESERVE 0x00000002L
#define D3DCREATE_MULTITHREADED 0x00000004L
#define D3DCREATE_SOFTWARE_VERTEXPROCESSING 0x00000020L
#define D3DCREATE_HARDWARE_VERTEXPROCESSING 0x00000040L
#define D3DCREATE_MIXED_VERTEXPROCESSING 0x00000080L

#define D3DUSAGE_RENDERTARGET 0x00000001L
#define D3DUSAGE_DEPTHSTENCIL 0x00000002L
#define D3DUSAGE_WRITEONLY 0x00000008L
#define D3DUSAGE_DYNAMIC 0x00000200L
#define D3DUSAGE_AUTOGENMIPMAP 0x00000400L

#define D3DLOCK_READONLY 0x00000010L
#define D3DLOCK_DISCARD 0x00002000L
#define D3DLOCK_NOOVERWRITE 0x00001000L
#define D3DLOCK_NOSYSLOCK 0x00000800L
#define D3DLOCK_NO_DIRTY_UPDATE 0x00008000L

#define D3DCLEAR_TARGET 0x00000001L
#define D3DCLEAR_ZBUFFER 0x00000002L
#define D3DCLEAR_STENCIL 0x00000004L

#define D3DPRESENT_INTERVAL_DEFAULT 0x00000000L
#define D3DPRESENT_INTERVAL_ONE 0x00000001L
#define D3DPRESENT_INTERVAL_TWO 0x00000002L
#define D3DPRESENT_INTERVAL_IMMEDIATE 0x80000000L
#define D3DPRESENTFLAG_LOCKABLE_BACKBUFFER 0x00000001L

#define D3D_OK S_OK
#define D3DERR_DEVICELOST ((HRESULT)0x88760868L)
#define D3DERR_DEVICENOTRESET ((HRESULT)0x88760869L)
#define D3DERR_DRIVERINTERNALERROR ((HRESULT)0x88760827L)
#define D3DERR_INVALIDCALL ((HRESULT)0x8876086CL)
#define D3DERR_OUTOFVIDEOMEMORY ((HRESULT)0x8876017CL)
#define D3DERR_NOTAVAILABLE ((HRESULT)0x8876086AL)
#define D3D_SDK_VERSION 32

#define D3DADAPTER_DEFAULT 0

typedef struct _D3DDISPLAYMODE {
    UINT Width, Height, RefreshRate;
    D3DFORMAT Format;
} D3DDISPLAYMODE;

// --- structures with load-bearing SDK layouts -------------------------------
typedef struct _D3DPRESENT_PARAMETERS {
    UINT BackBufferWidth, BackBufferHeight;
    D3DFORMAT BackBufferFormat;
    UINT BackBufferCount;
    D3DMULTISAMPLE_TYPE MultiSampleType;
    DWORD MultiSampleQuality;
    D3DSWAPEFFECT SwapEffect;
    HWND hDeviceWindow;
    BOOL Windowed;
    BOOL EnableAutoDepthStencil;
    D3DFORMAT AutoDepthStencilFormat;
    DWORD Flags;
    UINT FullScreen_RefreshRateInHz;
    UINT PresentationInterval;
} D3DPRESENT_PARAMETERS;
static_assert(sizeof(D3DPRESENT_PARAMETERS) == 0x38, "SDK x86 layout");

typedef struct _D3DSURFACE_DESC {
    D3DFORMAT Format;
    D3DRESOURCETYPE Type;
    DWORD Usage;
    D3DPOOL Pool;
    D3DMULTISAMPLE_TYPE MultiSampleType;
    DWORD MultiSampleQuality;
    UINT Width, Height;
} D3DSURFACE_DESC;

typedef struct _D3DVSHADERCAPS2_0 {
    DWORD Caps;
    INT DynamicFlowControlDepth;
    INT NumTemps;
    INT StaticFlowControlDepth;
} D3DVSHADERCAPS2_0;
typedef struct _D3DPSHADERCAPS2_0 {
    DWORD Caps;
    INT DynamicFlowControlDepth;
    INT NumTemps;
    INT StaticFlowControlDepth;
    INT NumInstructionSlots;
} D3DPSHADERCAPS2_0;
typedef struct _D3DCAPS9 {
    D3DDEVTYPE DeviceType;
    UINT AdapterOrdinal;
    DWORD Caps, Caps2, Caps3;
    DWORD PresentationIntervals;
    DWORD CursorCaps;
    DWORD DevCaps;
    DWORD PrimitiveMiscCaps;
    DWORD RasterCaps;
    DWORD ZCmpCaps;
    DWORD SrcBlendCaps;
    DWORD DestBlendCaps;
    DWORD AlphaCmpCaps;
    DWORD ShadeCaps;
    DWORD TextureCaps;
    DWORD TextureFilterCaps;
    DWORD CubeTextureFilterCaps;
    DWORD VolumeTextureFilterCaps;
    DWORD TextureAddressCaps;
    DWORD VolumeTextureAddressCaps;
    DWORD LineCaps;
    DWORD MaxTextureWidth, MaxTextureHeight;
    DWORD MaxVolumeExtent;
    DWORD MaxTextureRepeat;
    DWORD MaxTextureAspectRatio;
    DWORD MaxAnisotropy;
    float MaxVertexW;
    float GuardBandLeft, GuardBandTop, GuardBandRight, GuardBandBottom;
    float ExtentsAdjust;
    DWORD StencilCaps;
    DWORD FVFCaps;
    DWORD TextureOpCaps;
    DWORD MaxTextureBlendStages;
    DWORD MaxSimultaneousTextures;
    DWORD VertexProcessingCaps;
    DWORD MaxActiveLights;
    DWORD MaxUserClipPlanes;
    DWORD MaxVertexBlendMatrices;
    DWORD MaxVertexBlendMatrixIndex;
    float MaxPointSize;
    DWORD MaxPrimitiveCount;
    DWORD MaxVertexIndex;
    DWORD MaxStreams;
    DWORD MaxStreamStride;
    DWORD VertexShaderVersion;
    DWORD MaxVertexShaderConst;
    DWORD PixelShaderVersion;
    float PixelShader1xMaxValue;
    DWORD DevCaps2;
    float MaxNpatchTessellationLevel;
    DWORD Reserved5;
    UINT MasterAdapterOrdinal;
    UINT AdapterOrdinalInGroup;
    UINT NumberOfAdaptersInGroup;
    DWORD DeclTypes;
    DWORD NumSimultaneousRTs;
    DWORD StretchRectFilterCaps;
    D3DVSHADERCAPS2_0 VS20Caps;
    D3DPSHADERCAPS2_0 PS20Caps;
    DWORD VertexTextureFilterCaps;
    DWORD MaxVShaderInstructionsExecuted;
    DWORD MaxPShaderInstructionsExecuted;
    DWORD MaxVertexShader30InstructionSlots;
    DWORD MaxPixelShader30InstructionSlots;
} D3DCAPS9;
static_assert(sizeof(D3DCAPS9) == 0x130, "SDK x86 layout");

// --- COM interfaces (called members only) -----------------------------------
struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DResource9;
struct IDirect3DBaseTexture9;
struct IDirect3DTexture9;
struct IDirect3DSurface9;
struct IDirect3DVertexBuffer9;
struct IDirect3DVertexShader9;
typedef IDirect3D9* LPDIRECT3D9;
typedef IDirect3DDevice9* LPDIRECT3DDEVICE9;
typedef IDirect3DTexture9* LPDIRECT3DTEXTURE9;
typedef IDirect3DSurface9* LPDIRECT3DSURFACE9;
typedef IDirect3DVertexBuffer9* LPDIRECT3DVERTEXBUFFER9;

struct IDirect3D9 : IUnknown {
    virtual HRESULT WINAPI RegisterSoftwareDevice(void*) = 0;
    virtual UINT WINAPI GetAdapterCount() = 0;
    virtual HRESULT WINAPI GetAdapterIdentifier(UINT, DWORD, void*) = 0;
    virtual UINT WINAPI GetAdapterModeCount(UINT, D3DFORMAT) = 0;
    virtual HRESULT WINAPI EnumAdapterModes(UINT, D3DFORMAT, UINT, D3DDISPLAYMODE*) = 0;
    virtual HRESULT WINAPI GetAdapterDisplayMode(UINT, D3DDISPLAYMODE*) = 0;
    virtual HRESULT WINAPI CheckDeviceType(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, BOOL) = 0;
    virtual HRESULT WINAPI CheckDeviceFormat(UINT, D3DDEVTYPE, D3DFORMAT, DWORD, D3DRESOURCETYPE, D3DFORMAT) = 0;
    virtual HRESULT WINAPI CheckDeviceMultiSampleType(UINT, D3DDEVTYPE, D3DFORMAT, BOOL, D3DMULTISAMPLE_TYPE, DWORD*) = 0;
    virtual HRESULT WINAPI CheckDepthStencilMatch(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, D3DFORMAT) = 0;
    virtual HRESULT WINAPI CheckDeviceFormatConversion(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT) = 0;
    virtual HRESULT WINAPI GetDeviceCaps(UINT, D3DDEVTYPE, D3DCAPS9*) = 0;
    virtual void* WINAPI GetAdapterMonitor(UINT) = 0;
    virtual HRESULT WINAPI CreateDevice(UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice9**) = 0;
};

struct IDirect3DDevice9 : IUnknown {
    virtual HRESULT WINAPI TestCooperativeLevel() = 0;
    virtual UINT WINAPI GetAvailableTextureMem() = 0;
    virtual HRESULT WINAPI EvictManagedResources() = 0;
    virtual HRESULT WINAPI GetDirect3D(IDirect3D9**) = 0;
    virtual HRESULT WINAPI GetDeviceCaps(D3DCAPS9*) = 0;
    virtual HRESULT WINAPI GetDisplayMode(UINT, D3DDISPLAYMODE*) = 0;
    virtual HRESULT WINAPI GetCreationParameters(void*) = 0;
    virtual HRESULT WINAPI Reset(D3DPRESENT_PARAMETERS*) = 0;
    virtual HRESULT WINAPI Present(const RECT*, const RECT*, HWND, const void*) = 0;
    virtual HRESULT WINAPI GetBackBuffer(UINT, UINT, D3DBACKBUFFER_TYPE, IDirect3DSurface9**) = 0;
    virtual HRESULT WINAPI GetRasterStatus(UINT, D3DRASTER_STATUS*) = 0;
    virtual HRESULT WINAPI CreateTexture(UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9**, HANDLE*) = 0;
    virtual HRESULT WINAPI CreateVertexBuffer(UINT, DWORD, DWORD, D3DPOOL, IDirect3DVertexBuffer9**, HANDLE*) = 0;
    virtual HRESULT WINAPI CreateRenderTarget(UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, DWORD, BOOL, IDirect3DSurface9**, HANDLE*) = 0;
    virtual HRESULT WINAPI CreateOffscreenPlainSurface(UINT, UINT, D3DFORMAT, D3DPOOL, IDirect3DSurface9**, HANDLE*) = 0;
    virtual HRESULT WINAPI UpdateSurface(IDirect3DSurface9*, const RECT*, IDirect3DSurface9*, const POINT*) = 0;
    virtual HRESULT WINAPI StretchRect(IDirect3DSurface9*, const RECT*, IDirect3DSurface9*, const RECT*, DWORD) = 0;
    virtual HRESULT WINAPI ColorFill(IDirect3DSurface9*, const RECT*, D3DCOLOR) = 0;
    virtual HRESULT WINAPI SetRenderTarget(DWORD, IDirect3DSurface9*) = 0;
    virtual HRESULT WINAPI GetRenderTarget(DWORD, IDirect3DSurface9**) = 0;
    virtual HRESULT WINAPI Clear(DWORD, const D3DRECT*, DWORD, D3DCOLOR, float, DWORD) = 0;
    virtual HRESULT WINAPI SetTransform(D3DTRANSFORMSTATETYPE, const D3DMATRIX*) = 0;
    virtual HRESULT WINAPI GetTransform(D3DTRANSFORMSTATETYPE, D3DMATRIX*) = 0;
    virtual HRESULT WINAPI MultiplyTransform(D3DTRANSFORMSTATETYPE, const D3DMATRIX*) = 0;
    virtual HRESULT WINAPI SetViewport(const D3DVIEWPORT9*) = 0;
    virtual HRESULT WINAPI GetViewport(D3DVIEWPORT9*) = 0;
    virtual HRESULT WINAPI SetMaterial(const void*) = 0;
    virtual HRESULT WINAPI SetLight(DWORD, const void*) = 0;
    virtual HRESULT WINAPI LightEnable(DWORD, BOOL) = 0;
    virtual HRESULT WINAPI SetTexture(DWORD, IDirect3DBaseTexture9*) = 0;
    virtual HRESULT WINAPI GetTexture(DWORD, IDirect3DBaseTexture9**) = 0;
    virtual HRESULT WINAPI SetTextureStageState(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD) = 0;
    virtual HRESULT WINAPI GetTextureStageState(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD*) = 0;
    virtual HRESULT WINAPI SetSamplerState(DWORD, D3DSAMPLERSTATETYPE, DWORD) = 0;
    virtual HRESULT WINAPI GetSamplerState(DWORD, D3DSAMPLERSTATETYPE, DWORD*) = 0;
    virtual HRESULT WINAPI SetScissorRect(const RECT*) = 0;
    virtual HRESULT WINAPI SetRenderState(D3DRENDERSTATETYPE, DWORD) = 0;
    virtual HRESULT WINAPI GetRenderState(D3DRENDERSTATETYPE, DWORD*) = 0;
    virtual HRESULT WINAPI BeginScene() = 0;
    virtual HRESULT WINAPI EndScene() = 0;
    virtual HRESULT WINAPI SetFVF(DWORD) = 0;
    virtual HRESULT WINAPI GetFVF(DWORD*) = 0;
    virtual HRESULT WINAPI SetVertexShader(IDirect3DVertexShader9*) = 0;
    virtual HRESULT WINAPI SetStreamSource(UINT, IDirect3DVertexBuffer9*, UINT, UINT) = 0;
    virtual HRESULT WINAPI GetStreamSource(UINT, IDirect3DVertexBuffer9**, UINT*, UINT*) = 0;
    virtual HRESULT WINAPI DrawPrimitive(D3DPRIMITIVETYPE, UINT, UINT) = 0;
    virtual HRESULT WINAPI DrawPrimitiveUP(D3DPRIMITIVETYPE, UINT, const void*, UINT) = 0;
};

struct IDirect3DResource9 : IUnknown {
    virtual HRESULT WINAPI GetDevice(IDirect3DDevice9**) = 0;
    virtual HRESULT WINAPI SetPrivateData(REFGUID, const void*, DWORD, DWORD) = 0;
    virtual HRESULT WINAPI GetPrivateData(REFGUID, void*, DWORD*) = 0;
    virtual HRESULT WINAPI FreePrivateData(REFGUID) = 0;
    virtual DWORD WINAPI SetPriority(DWORD) = 0;
    virtual DWORD WINAPI GetPriority() = 0;
    virtual void WINAPI PreLoad() = 0;
    virtual D3DRESOURCETYPE WINAPI GetType() = 0;
};

struct IDirect3DBaseTexture9 : IDirect3DResource9 {
    virtual DWORD WINAPI SetLOD(DWORD) = 0;
    virtual DWORD WINAPI GetLOD() = 0;
    virtual DWORD WINAPI GetLevelCount() = 0;
    virtual HRESULT WINAPI SetAutoGenFilterType(DWORD) = 0;
    virtual DWORD WINAPI GetAutoGenFilterType() = 0;
    virtual void WINAPI GenerateMipSubLevels() = 0;
};

struct IDirect3DTexture9 : IDirect3DBaseTexture9 {
    virtual HRESULT WINAPI GetLevelDesc(UINT, D3DSURFACE_DESC*) = 0;
    virtual HRESULT WINAPI GetSurfaceLevel(UINT, IDirect3DSurface9**) = 0;
    virtual HRESULT WINAPI LockRect(UINT, D3DLOCKED_RECT*, const RECT*, DWORD) = 0;
    virtual HRESULT WINAPI UnlockRect(UINT) = 0;
    virtual HRESULT WINAPI AddDirtyRect(const RECT*) = 0;
};

struct IDirect3DSurface9 : IUnknown {
    virtual HRESULT WINAPI GetContainer(REFIID, void**) = 0;
    virtual HRESULT WINAPI GetDesc(D3DSURFACE_DESC*) = 0;
    virtual HRESULT WINAPI LockRect(D3DLOCKED_RECT*, const RECT*, DWORD) = 0;
    virtual HRESULT WINAPI UnlockRect() = 0;
    virtual HRESULT WINAPI GetDC(HDC*) = 0;
    virtual HRESULT WINAPI ReleaseDC(HDC) = 0;
};

struct IDirect3DVertexBuffer9 : IDirect3DResource9 {
    virtual HRESULT WINAPI Lock(UINT, UINT, void**, DWORD) = 0;
    virtual HRESULT WINAPI Unlock() = 0;
    virtual HRESULT WINAPI GetDesc(void*) = 0;
};

struct IDirect3DVertexShader9 : IUnknown {
    virtual HRESULT WINAPI GetDevice(IDirect3DDevice9**) = 0;
    virtual HRESULT WINAPI GetFunction(void*, UINT*) = 0;
};

extern "C" IDirect3D9* WINAPI Direct3DCreate9(UINT sdk_version);
