#pragma once
#include "AnmCoordinates.hpp"
#include "ZunGraphics.hpp"
#include "AnmTransforms.hpp"
#include "AnmCamera.hpp"
namespace th13 {
// 调试探针：draw()/draw_glyph() 调用计量（按精灵聚合；单线程 wasm，无锁）。
struct AnmDrawProbe {
    u32 calls = 0, quads = 0, shapes_drawn = 0, strips = 0, glyphs = 0, other = 0;
    static constexpr u32 kSlots = 96;
    i32 sprite_ids[kSlots]{};
    u32 sprite_counts[kSlots]{};
    u32 sprite_sizes[kSlots][2]{};
    bool enabled = false;
    void reset() noexcept { *this = AnmDrawProbe{}; }
};
extern AnmDrawProbe anm_draw_probe;
// [蓝图 §8.3/§8.9] 绘制侧层覆盖（诊断捕获/无头导出用）：不改 VM 层（脚本
// ins_304 真值保持），仅让 DrawCapture 的 sprite 钩子按覆盖值记录 op 层。
// 原版 supervisor 的 +1/+2 偏移发生在绘制侧（oracle VM dump 层恒为脚本值，
// op 层 +1/+2）。每帧 draw 前 clear 后由标题场景重设。
void set_draw_layer_override(const AnmVm& vm, i32 layer);
void clear_draw_layer_overrides();
class AnmRenderer {
    // 0x467960 初始化 UP 批池：基址 [0xb781f4]，指针字段 0xef81f4 之前容量
    // 0x380000 字节；0x467a80 每 quad 追加 0xa8 字节（6 顶点 × 28B）→
    // 21845 quads = 131070 顶点。0x4679a0 只在材质键/管线态变化时冲批
    // （层不是触发，f61 实证），池满无检查。端口以此为安全上限：旧值 6144
    // 会在原生池未满时提前 flush。r23i 实测该上限在 gate-my laser 窗从未
    // 触发（web-draws 与 r23h 逐字节相同）；op183 的 10+2 拆分来自 pass
    // 入口同值相机 flush，见 ScreenViews::camera_if_changed。
    static constexpr u32 kSpriteBatchVertices = 131070;
    ZunGraphics& graphics;std::vector<AnmGeometryVertex> vertices;u32 bound_texture=~0u;
    struct ColorVertex {Vec3 position;float reciprocal_w;u32 color;};std::vector<ColorVertex> shapes;
    bool material(AnmVm&,bool textured=true);int quad(AnmVm&,bool pixel);int pack_quad(AnmVm&,Vec3 (&)[4],bool pixel,const u32* colors=nullptr);int projected_quad(AnmVm&,bool fog);int world_quad(AnmVm&);int mesh(AnmVm&);int shape(AnmVm&);int trail(AnmVm&);int circle(AnmVm&);
    u8 draw_opacity_ = 255;
    int draw_vm(AnmVm&);
    u32 apply_draw_opacity(u32) const noexcept;
    template<class Vertex>
    void submit_colored(AnmVm& vm, touhou::graphics::Topology kind, u32 count,
                        const Vertex* data);
    u32 tint_color(u32)const noexcept;int overlay(AnmVm&);
public:
    std::array<u8, 4> player_opacity{255, 255, 255, 255};
    u32 presentation_instance=0;
    void submit(AnmVm&,touhou::graphics::Topology,u32,const void*,u32);
    Vec2 offset{};GraphicsViewport viewport;AnmCamera camera;bool tint_enabled=false;u32 tint=0x80808080;
    // 0x467350 的四角裁剪用 0x4dcc18+0xd8 的 2D 视口块（战场为 0x43be9b 的
    // {116,2,408,476}），与 D3D viewport（0x43b900 设置，可为全屏）不同。
    // 战场层 0..20 的绘制必须用该块裁剪，否则顶部/边缘的 quad 会多画。
    GraphicsViewport clip;bool clip_set=false;
    AnmCamera scenery_camera;GraphicsViewport scenery_viewport;bool scenery_camera_ready=false;
    std::string error;explicit AnmRenderer(ZunGraphics& graphics):graphics(graphics){vertices.reserve(kSpriteBatchVertices);shapes.reserve(192);}
    void flush();void invalidate();void set_viewport(const GraphicsViewport&);void set_camera(const AnmCamera&);
    int draw(AnmVm&);
    int draw_with_opacity(AnmVm&, u8 opacity);
    int draw_glyph(AnmVm&);bool draw_layer(const std::vector<AnmVm*>&);
    int draw_screen_strip(AnmVm&,const AnmGeometryVertex*,u32 count);
    const std::vector<AnmGeometryVertex>& pending()const noexcept{return vertices;}
};
}
