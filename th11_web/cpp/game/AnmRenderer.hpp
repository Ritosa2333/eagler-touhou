#pragma once
#include "AnmGeometry.hpp"
#include "AnmProjection.hpp"
#include "ZunGraphics.hpp"
#include "SceneCamera.hpp"
#include <vector>
namespace th11 {
#ifdef TH11_MULTIPLAYER
class AnmManager;
#endif
class AnmRenderer {
public:
    explicit AnmRenderer(ZunGraphics& graphics):graphics(graphics){vertices.reserve(6144);}
    Vec2 offset{};
    GraphicsViewport viewport;
    AnmCamera camera;
    // The original interpolates fog channels independently of the packed
    // camera endpoint. Keep those values and its reference origin explicit.
    Vec3 fog_origin{},fog_channels{160,160,160};float fog_start=1000;
    bool tint_enabled=false;u32 tint=0x80808080;
#ifdef TH11_MULTIPLAYER
    const AnmManager* multiplayer_animations=nullptr;
    const AnmVm* multiplayer_hud_background=nullptr;
    std::array<u8,4> multiplayer_opacity{{255,255,255,255}};
    u32 multiplayer_color(const AnmVm&,u32)const noexcept;
    u32 multiplayer_tint(u32,unsigned)const noexcept;
    void multiplayer_geometry(const AnmVm&,touhou::graphics::Topology,u32,const AnmVertex*);
#endif
    int draw(AnmVm& vm);
    int draw_ascii_sprite(AnmVm& vm);
    int draw_ripple(AnmVm& vm);
    int draw_layer(AnmVm* first);
    void solid_rectangle(float left,float top,float right,float bottom,u32 color);
    void flush();
    void invalidate();
    touhou::graphics::PipelineState& pipeline(){flush();return graphics.pipeline();}
    bool select_target(const AnmResource* file,u32 index){invalidate();return graphics.select_target(file,index);}
    bool clear_target(u32 color,const GraphicsViewport* rect=nullptr){flush();return graphics.clear_target(color,rect);}
    void set_viewport(const GraphicsViewport& value){flush();viewport=value;graphics.set_viewport(value);}
    void set_camera(SceneCamera&,bool screen);
private:
    ZunGraphics& graphics;
    std::vector<AnmVertex> vertices;
#ifdef TH11_MULTIPLAYER
    std::vector<AnmVertex> multiplayer_geometry_vertices;
#endif
    u32 texture_handle=~0u,blend_mode=~0u,filter=~0u;
    const AnmSprite* uv_sprite=nullptr;Matrix4 cached_uv{};
    void material(const AnmVm& vm);
    int submit_quad(AnmVm& vm,Vec3 (&positions)[4],bool pixel,const u32* colors=nullptr);
    float fog_amount(Vec3 position,Vec3 origin)const noexcept;
    u32 fog_color(u32 color,float amount,bool fade_alpha)const noexcept;
    u32 color(const AnmVm& vm)const noexcept;
};
}
