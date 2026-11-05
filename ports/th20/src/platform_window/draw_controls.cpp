#include "graphics_callbacks.hpp"
#include "../sprite_renderer/quad.hpp"
namespace th20::source::platform_window {
namespace {
void flush_draws(){auto& g=program_entry::graphics_state;sprite::flush_textured_quads(*program_entry::sprite_controller,*g.device);}
}
#ifndef TH_SDL3
HRESULT set_render_state(GraphicsStatePrefix& g,D3DRENDERSTATETYPE state,DWORD value){ //4d9db0
    flush_draws();return g.device->SetRenderState(state,value);
}
#else
void set_depth_compare(GraphicsStatePrefix& g,touhou::graphics::Compare value){ //4d9db0(ZFUNC)
    flush_draws();g.device->set_depth_compare(value);
}
void set_fog_color(GraphicsStatePrefix& g,std::uint32_t value){ //4d9db0(FOGCOLOR)
    flush_draws();g.device->set_fog_color(value);
}
void set_fog_range(GraphicsStatePrefix& g,float start,float end){ //4d9db0(FOGSTART/FOGEND)
    flush_draws();g.device->set_fog_range(start,end);
}
#endif
HRESULT disable_depth_write(GraphicsStatePrefix& g){ //4ddf80
    if(!g.field_0dbc)return 0;
    flush_draws();g.field_0dbc=0;
#ifdef TH_SDL3
    g.device->set_depth_mask(false);return 0;
#else
    return g.device->SetRenderState(D3DRS_ZWRITEENABLE,0);
#endif
}
HRESULT disable_fog(GraphicsStatePrefix& g){ //4dda60
    if(!g.render_value)return 0;
    // This is the same4455c0 source previously reached through reset_sprite_queue.
    flush_draws();g.render_value=0;
#ifdef TH_SDL3
    g.device->set_fog(false);return 0;
#else
    return g.device->SetRenderState(D3DRS_FOGENABLE,FALSE);
#endif
}
}
