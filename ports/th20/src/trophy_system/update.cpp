#include "trophy.hpp"
#include "data_strings.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../runtime_state/state.hpp"
#include "../text_renderer/centered.hpp"
#include "../audio_runtime/audio.hpp"
namespace th20::source::trophy {
int update(TrophyInf& o){
    auto& sprites=*program_entry::sprite_controller;
    switch(o.state){
    case 0:
        if(o.pending.count){
            const auto index=o.pending.pop_front();change_state(o,1);
            sprite::spawn_named_animation(sprites,*o.animation_file,o.handles[0],"trophy",0,nullptr,0,-1,4);
            sprite::spawn_named_animation(sprites,*o.animation_file,o.handles[1],"trophy",7,nullptr,0,-1,4);
            sprite::spawn_named_animation(sprites,*o.animation_file,o.handles[2],"trophy",8,nullptr,0,-1,4);
            // The two original std::function completions ultimately call40e5e0,
            // a verified empty function. Text uploads still use the real queue.
            text::write_midpoint_animation_text(sprites,*sprite::resolve_animation_handle(sprites,o.handles[1]),0x80c0c0,0,2,0,nullptr,{},data::s_005757cc,decode_string(messages[index].title));
            text::write_midpoint_animation_text(sprites,*sprite::resolve_animation_handle(sprites,o.handles[2]),0x80c0c0,0,2,0,nullptr,{},data::announcement);
            program_entry::thread_registry.request_effect(79,0);
        }
        [[fallthrough]];
    case 1:if(o.age.current>=240)change_state(o,o.pending.count?0:2);break;
    case 2:
        for(auto& handle:o.handles)sprite::request_animation_deletion(sprites,handle);
        runtime::retire_callback_owner(&o);return 1;
    }
    recovered::timer_tick(o.age,state::timer_rate);return 1;
}
}
