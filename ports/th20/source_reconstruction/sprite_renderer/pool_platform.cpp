#include "pool_platform.hpp"
#include "dispatch.hpp"
#include "quad.hpp"
#include "draw.hpp"
#include "projected_draw.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include <cstring>
namespace th20::source::sprite {
namespace pe=program_entry;
void select_layer_viewport(pe::GraphicsStatePrefix& graphics,int index) {
    auto& viewport=graphics.viewports[index];graphics.current_viewport=&viewport;
    platform_window::update_camera(viewport,viewport.adjusted_viewport);
    platform_window::apply_camera(viewport);
#ifdef TH_SDL3
    {const auto& v=viewport.adjusted_viewport;graphics.device->set_viewport(v.X,v.Y,v.Width,v.Height,v.MinZ,v.MaxZ);}
#else
    graphics.device->SetViewport(&viewport.adjusted_viewport);
#endif
    auto& c=*pe::sprite_controller;
    const float x=_mm_cvtss_f32(_mm_cvtsi32_ss(_mm_setzero_ps(),viewport.offset_x));
    const float y=_mm_cvtss_f32(_mm_cvtsi32_ss(_mm_setzero_ps(),viewport.offset_y));
    std::memcpy(&c.fields_c8[2],&x,4);std::memcpy(&c.fields_c8[3],&y,4);
    std::memcpy(&c.fields_c8[0],viewport.points[0],8);
    // Unlike 41dce0, 44f720 leaves graphics+0xb04 untouched.
}
namespace dispatch_environment {
Controller& controller(){return *pe::sprite_controller;}
void select_camera(std::int32_t preset){platform_window::select_viewport(pe::graphics_state,preset);}
void select_layer_camera(std::int32_t preset){select_layer_viewport(pe::graphics_state,preset);}
void select_viewport_camera(std::int32_t preset){select_layer_viewport(pe::graphics_state,preset);}
void disable_fog(){platform_window::disable_fog(pe::graphics_state);}
void disable_depth_write(){platform_window::disable_depth_write(pe::graphics_state);}
#ifndef TH_SDL3
void set_render_state(std::uint32_t state,std::uint32_t value){platform_window::set_render_state(pe::graphics_state,static_cast<D3DRENDERSTATETYPE>(state),value);}
#endif
}
namespace draw_environment {
Controller& controller(){return *pe::sprite_controller;}
Device& device(){return *pe::graphics_state.device;}
const float* viewport_bounds(){return pe::graphics_state.current_viewport->bounds;}
pe::ViewportState& current_camera(){return *pe::graphics_state.current_viewport;}
std::int32_t scaled_dimension(unsigned axis){return axis?pe::window_state.scaled_height:pe::window_state.scaled_width;}
void enable_fog(){auto& g=pe::graphics_state;if(g.render_value!=1){flush_textured_quads(*pe::sprite_controller,*g.device);g.render_value=1;
#ifdef TH_SDL3
    g.device->set_fog(true);
#else
    g.device->SetRenderState(D3DRS_FOGENABLE,1);
#endif
}}
void disable_fog(){platform_window::disable_fog(pe::graphics_state);}
void enable_depth_write(){auto& g=pe::graphics_state;if(g.field_0dbc!=1){flush_textured_quads(*pe::sprite_controller,*g.device);g.field_0dbc=1;
#ifdef TH_SDL3
    g.device->set_depth_mask(true);
#else
    g.device->SetRenderState(D3DRS_ZWRITEENABLE,1);
#endif
}}
void disable_depth_write(){platform_window::disable_depth_write(pe::graphics_state);}
}
}
namespace th20::source::platform_window::unrecovered {
sprite::Animation* create_animation_vm(){return sprite::create_heap_animation();}
void destroy_animation_vm(void* a){sprite::destroy_heap_animation(static_cast<sprite::Animation*>(a));}
void draw_animation(sprite::Animation& a){sprite::draw_animation(*program_entry::sprite_controller,a);}
void draw_animation_layer(sprite::Controller& c,int layer){sprite::draw_animation_layer(c,layer);}
void select_sprite_layer(sprite::Controller& c,int layer,int group){sprite::configure_animation_layer(c,layer,group);}
}
