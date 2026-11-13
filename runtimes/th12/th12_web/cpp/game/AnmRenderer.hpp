#pragma once
#include "AnmGeometry.hpp"
#include "AnmProjection.hpp"
#include "ZunGraphics.hpp"
#include "SceneCamera.hpp"
#include <vector>
namespace th11 {
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
    int draw(AnmVm& vm);
#if PORT_TITLE == 14
    u32 render_width=640,render_height=480;
    int draw_transition_type0(AnmVm& vm);
#endif
    int draw_ascii_sprite(AnmVm& vm);
    int draw_ripple(AnmVm& vm);
    int draw_layer(AnmVm* first);
    void solid_rectangle(float left,float top,float right,float bottom,u32 color);
    void flush();
    void invalidate();
    touhou::graphics::PipelineState& pipeline(){flush();return graphics.pipeline();}
    bool select_target(const AnmResource* file,u32 index){invalidate();return graphics.select_target(file,index);}
    bool clear_target(u32 color,const GraphicsViewport* rect=nullptr,u32 flags=3){flush();return graphics.clear_target(color,rect,flags);}
    void set_viewport(const GraphicsViewport& value){flush();viewport=value;graphics.set_viewport(value);}
    void set_camera(SceneCamera&,bool screen);
#if PORT_TITLE == 14
    void upload_camera(const SceneCamera&);
#endif
private:
    ZunGraphics& graphics;
    std::vector<AnmVertex> vertices;
    u32 texture_handle=~0u,blend_mode=~0u,filter=~0u;
    const AnmSprite* uv_sprite=nullptr;Matrix4 cached_uv{};
    void material(const AnmVm& vm);
    int submit_quad(AnmVm& vm,Vec3 (&positions)[4],bool pixel,const u32* colors=nullptr);
    float fog_amount(Vec3 position,Vec3 origin)const noexcept;
    u32 fog_color(u32 color,float amount,bool fade_alpha)const noexcept;
    u32 color(const AnmVm& vm)const noexcept;
    u32 tinted_color(u32 value)const noexcept;
#if PORT_TITLE == 13
    int draw_ribbon(AnmVm& vm);
#endif
#if PORT_TITLE == 14
public:
    // Stage traversal keeps the Supervisor mask separate from device writes
    // made directly by the Ribbon callback. The scope restores its caller.
    bool* use_stage_depth_cache(bool* value){auto* previous=stage_depth_cache;stage_depth_cache=value;return previous;}
    void set_stage_depth_write(bool);
    int draw_th14_screen_strip(AnmVm&,const AnmVertex*,u32 count);
private:
    bool* stage_depth_cache=nullptr;
    friend struct AnmQuadProbeTh14;
    Matrix4 th14_world{};
    // Native shared screen quad. Other producers/lifetime remain candidate-only.
    AnmVertex th14_quad[4]={{{},1,0,{0,0}},{{},1,0,{1,0}},{{},1,0,{0,1}},{{},1,0,{1,1}}};
    int draw_world_strip(AnmVm& vm,const AnmWorldVertex* data,u32 count,int depth_override=-1);
#endif
};
}
