#include "dispatch.hpp"
#include "../core_scheduler/scheduler.hpp"
#include "../runtime_core/runtime_core.hpp"
#ifdef TH_SDL3
#include "../platform_window/graphics_callbacks.hpp"
#include "../startup_scene/startup.hpp"
#include "sprite.hpp"
#include "../../../web/cpp/sdl/HitboxDisplay.hpp"
#include "quad.hpp"
extern "C" void sdl_startup_branding_draw(unsigned alpha);
#endif
namespace th20::source::sprite {
std::uint32_t select_layer_animations(AnimationList* group,std::int32_t layer,bool secondary) noexcept {
    initialize_animation_list(group[2]);std::uint32_t selected=0;
    for(scheduler::Iterator it(reinterpret_cast<scheduler::Link*>(group[secondary?1:0].sentinel.next));it.current;it.advance()){
        auto& a=*reinterpret_cast<Animation*>(it.current->value);if(a.fields_550[8])continue;
        auto effective=static_cast<std::int32_t>(a.base.fields_10_28[1]);
        if(secondary){if(effective>=29 && effective<=36)effective+=17;else effective=effective==26?45:47;}
        else {if(effective>=46 && effective<=53)effective-=17;else if(effective==45)effective=26;}
        if(effective==layer){initialize_animation_link(a.links[1],&a);scheduler::append(reinterpret_cast<scheduler::List&>(group[2]),reinterpret_cast<scheduler::Link&>(a.links[1]));++selected;}
    }return selected;
}
void configure_animation_layer(Controller& c,std::int32_t layer,std::int32_t group) {
    namespace e=dispatch_environment;const auto g=static_cast<std::uint32_t>(group);
    auto reset_draw_state=[] {auto& global=dispatch_environment::controller();global.fields_c8[0]=global.fields_c8[1]=0;};
    if(layer==41){e::select_layer_camera(3);e::disable_fog();c.field_6c0=g+1;}
    else if(layer==43||layer==44){e::select_camera(3);e::disable_fog();c.field_6c0=13;}
    else if(layer<3||(layer>38&&layer<43))c.field_6c0=0;
    else if(layer<20){const auto target=g+(layer<6?1u:(layer<12?3u:5u));if(c.field_6c0!=target){e::select_layer_camera(0);e::disable_fog();c.field_6c0=target;}}
    else if(layer<24){if(c.field_6c0!=g+7){e::select_viewport_camera(1);e::disable_depth_write();
#ifdef TH_SDL3
        platform_window::set_depth_compare(program_entry::graphics_state,touhou::graphics::Compare::Always);
#else
        e::set_render_state(23,8);
#endif
    }}
    else if(layer<26){if(c.field_6c0!=9){e::select_camera(4);e::disable_depth_write();
#ifdef TH_SDL3
        platform_window::set_depth_compare(program_entry::graphics_state,touhou::graphics::Compare::Always);
#else
        e::set_render_state(23,8);
#endif
        reset_draw_state();c.field_6c0=9;}}
    else if(layer<32||(layer>33&&layer<37)||(layer>44&&layer<49)||(layer>50&&layer<54)){
        if(c.field_6c0!=10){e::select_camera(2);e::disable_depth_write();
#ifdef TH_SDL3
        platform_window::set_depth_compare(program_entry::graphics_state,touhou::graphics::Compare::Always);
#else
        e::set_render_state(23,8);
#endif
        reset_draw_state();c.field_6c0=10;}
    }else if(((layer>31&&layer<34)||(layer>48&&layer<51))&&c.field_6c0!=g+11){e::select_viewport_camera(5);e::disable_depth_write();
#ifdef TH_SDL3
        platform_window::set_depth_compare(program_entry::graphics_state,touhou::graphics::Compare::Always);
#else
        e::set_render_state(23,8);
#endif
        reset_draw_state();c.field_6c0=g+11;}
}
void draw_selected_animations(Controller& c,AnimationList* group) {
    for(scheduler::Iterator it(reinterpret_cast<scheduler::Link*>(group[2].sentinel.next));it.current;it.advance()){
        auto& animation=*reinterpret_cast<Animation*>(it.current->value);
        draw_animation(c,animation);++c.fields_c8[4];
#ifdef TH_SDL3
        if(startup::loading_scene&&animation.handle==startup::loading_scene->animation_handle&&(animation.base.flags[1]&1)){
            flush_textured_quads(c,draw_environment::device());sdl_startup_branding_draw(animation.base.field_490);
        }
#endif
    }
}
std::int32_t draw_animation_layer(Controller& c,std::int32_t layer) {
    std::lock_guard lock(runtime::shared_locks().slot(9));
    for(bool secondary:{false,true})if(select_layer_animations(c.lists,layer,secondary)){configure_animation_layer(c,layer,0);draw_selected_animations(dispatch_environment::controller(),c.lists);}
#ifdef TH_SDL3
    web::draw_hitbox_layer(c,layer);
#endif
    return 1;
}
}
