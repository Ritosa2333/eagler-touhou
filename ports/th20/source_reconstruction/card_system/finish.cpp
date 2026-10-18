#include "card.hpp"
#include "records.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../stage_background/background.hpp"
#include "../stage_completion/replay_access.hpp"
#include "../stage_completion/dependencies.hpp"
#include "../hud_system/hud.hpp"
#include "../damage_regions/damage.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/anm_vm.hpp"
namespace th20::source::card {
void finish(CardInf& o){
    if(!(o.flags&1u))return;
    background::primary->state_flags|=1u;auto& sprites=*program_entry::sprite_controller;
    for(auto handle:o.info_handles)sprite::interrupt_animation_children(sprites,handle,1);
    o.flags&=~1u;sprite::request_animation_deletion(sprites,o.background_handle);o.flags&=~0x20u;
    for(auto* a:hud::controller->number_animations){a->base.field_438=3;sprite::execute_animation(*a);}
    sprite::request_animation_deletion(sprites,o.effect_handle);
    if(!(o.flags&2u))hud::notify(*hud::controller,1,0);
    else {
        damage::add_score(game_session::player(0),static_cast<std::uint32_t>(o.bonus));hud::notify(*hud::controller,0,o.bonus);
        if(!replay::is_playback()){
            const unsigned mode=game_session::mode()==2?1:0;increment_record(record(*progress::current_profile(),o.spell_index),0xc0,mode);
            auto& fallback=progress::fallback_profile();increment_record(record(fallback,o.spell_index),0xc0,mode);
            unsigned captures=0;for(int i=0;i<113;++i){const auto* entry=record(fallback,i);if(record_count(entry,0xc0,0)!=0||record_count(entry,0xc0,1)!=0)++captures;}
            if(captures>112)stage_completion::unrecovered::announce_achievement(40);
        }
        program_entry::thread_registry.request_effect(46,0);
    }
    if(o.flags&0x80u)program_entry::thread_registry.request_effect(69,0);
}
}
