// th16 渲染设备接口——游戏逻辑侧对"显卡"的全部要求。
//
// 原版通过 IDirect3DDevice9（DAT_004c10d8）直接下发渲染状态号与顶点字节；移植版把这些调用翻译成共享
// 语义管线状态（portable/sdl/GraphicsState.hpp 的 PipelineState）加上少数资源操作，由两种实现承接：
//   sdl::GraphicsDevice（web/cpp/sdl）     真机 GLES3 后端，'@R' 渲染目标用帧缓冲纹理承接
//   harness RecordingDevice（tests/harness）把每次绘制时生效的状态与顶点记成 JSON，与
//                                          tools/oracle/game-oracle.py --draw 录下的原版设备调用流对拍
//
// 原版设备调用 → 本接口的对应关系：
//   SetRenderTarget（0x43d140 / 0x43d400 …）  select_target      纹理身份 = ANM 槽 + entry（'@R' 为 text.anm 2 / 3）
//   Clear                                      clear
//   SetViewport                                set_viewport
//   SetTransform（0x43c710 / 0x467410）         set_matrix
//   SetTexture                                 bind_texture
//   SetRenderState / SetTextureStageState /
//   SetSamplerState                            pipeline() 字段（StateCommands）
//   DrawPrimitive(UP)                          draw               顶点布局与原版 FVF 一一对应（见 VertexLayout）
#pragma once
#include "Types.hpp"
#include "../../../portable/sdl/GraphicsState.hpp"

namespace th16 {

// 纹理 / 渲染目标身份：ANM 槽号 + 纹理 entry 序号。slot < 0 表示"无纹理"或"后台缓冲"。
struct TextureRef {
    i32 slot = -1;
    i32 entry = -1;

    static constexpr TextureRef none() { return {}; }
    static constexpr TextureRef backbuffer() { return {}; }
    bool valid() const { return slot >= 0; }
    bool operator==(const TextureRef& o) const { return slot == o.slot && entry == o.entry; }
    bool operator!=(const TextureRef& o) const { return !(*this == o); }
};

// D3D RECT 语义：左上含、右下不含
struct ScreenRect {
    i32 left = 0, top = 0, right = 0, bottom = 0;
};

// D3DVIEWPORT9：深度范围恒为 0..1（原版从不改）
struct Viewport {
    i32 x = 0, y = 0, width = 640, height = 480;
    bool operator==(const Viewport& o) const { return x == o.x && y == o.y && width == o.width && height == o.height; }
};

// Clear 标志（D3DCLEAR_TARGET / D3DCLEAR_ZBUFFER）
enum ClearFlags : u32 {
    kClearColor = 1u,
    kClearDepth = 2u,
};

class RenderDevice : public touhou::graphics::StateCommands {
public:
    using MatrixKind = touhou::graphics::MatrixKind;
    using Topology = touhou::graphics::Topology;
    using VertexLayout = touhou::graphics::VertexLayout;

    // 切换渲染目标（随之把视口复位为目标全幅——D3D SetRenderTarget 的隐含行为）
    virtual void select_target(TextureRef target) = 0;
    // 清当前目标；rect 为空时清整个当前视口
    virtual void clear(u32 flags, u32 argb, const ScreenRect* rect) = 0;
    virtual void set_viewport(const Viewport& viewport) = 0;
    // 行主序 D3DX 矩阵（v' = v · M）
    virtual void set_matrix(MatrixKind kind, const float matrix[16]) = 0;
    virtual void bind_texture(TextureRef texture) = 0;
    // primitive_count 与 D3D 口径一致（三角形列表 = 三角形数，条带 = 顶点数 - 2）
    virtual void draw(Topology topology, VertexLayout layout, u32 primitive_count, const void* vertices, u32 stride) = 0;
};

// 0x45ba80：设备创建后下发的一组渲染状态（原版此后只按需改动其中几项）。两种设备实现在建立 / 重置时都以它为起点
inline void apply_original_device_defaults(touhou::graphics::PipelineState& p) {
    using namespace touhou::graphics;
    p.depthTest = true;                      // ZENABLE
    p.depthWrite = true;
    p.depthCompare = Compare::Always;        // ZFUNC = ALWAYS（关卡背景临时改 LESSEQUAL）
    p.cull = Cull::None;
    p.blend = true;                          // ALPHABLENDENABLE
    p.separateAlphaBlend = false;
    p.sourceBlend = BlendFactor::SourceAlpha;
    p.destinationBlend = BlendFactor::InverseSourceAlpha;
    p.blendEquation = BlendEquation::Add;
    p.alphaTest = true;                      // ALPHATESTENABLE，REF 1，GREATEREQUAL
    p.alphaReference = 1;
    p.alphaCompare = Compare::GreaterEqual;
    p.fog = true;                            // FOGENABLE（主循环每帧开头再关掉）
    p.fogMode = FogMode::Linear;             // FOGVERTEXMODE = LINEAR，FOGTABLEMODE = NONE
    p.fogDensity = 1.0f;
    p.fogColor = 0xffa0a0a0u;
    p.fogNear = 1000.0f;
    p.fogFar = 5000.0f;
    p.color = {ColorOperation::Multiply, {ArgumentSource::Texture}, {ArgumentSource::Factor}};   // COLOROP MODULATE, TEXTURE × TFACTOR
    p.alpha = {ColorOperation::Multiply, {ArgumentSource::Texture}, {ArgumentSource::Factor}};
    p.textureTransform = true;               // TEXTURETRANSFORMFLAGS = COUNT2
    p.minFilter = p.magFilter = Filter::Linear;
    p.addressU = p.addressV = Address::Repeat;
    p.textureFactor = 0xffffffffu;
}

// 原版四种 FVF 顶点（字节布局与 D3D 一致，直接作为 draw() 的顶点数据）
struct ScreenColorUvVertex {      // D3DFVF_XYZRHW | DIFFUSE | TEX1（0x144，0x1c 字节）
    float x, y, z, rhw;
    u32 color;
    float u, v;
};
struct ScreenColorVertex {        // D3DFVF_XYZRHW | DIFFUSE（0x44，0x14 字节）
    float x, y, z, rhw;
    u32 color;
};
struct WorldUvVertex {            // D3DFVF_XYZ | TEX1（0x102，0x14 字节）
    float x, y, z;
    float u, v;
};
struct WorldColorUvVertex {       // D3DFVF_XYZ | DIFFUSE | TEX1（0x142，0x18 字节）
    float x, y, z;
    u32 color;
    float u, v;
};
static_assert(sizeof(ScreenColorUvVertex) == 0x1c && sizeof(ScreenColorVertex) == 0x14 &&
              sizeof(WorldUvVertex) == 0x14 && sizeof(WorldColorUvVertex) == 0x18, "FVF 顶点字节布局");

}  // namespace th16
