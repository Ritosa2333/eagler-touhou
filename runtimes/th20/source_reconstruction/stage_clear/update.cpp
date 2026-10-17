#include "stage_clear.hpp"
#include "data_constants.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../gameplay/gameplay.hpp"
#include "../input/input.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../damage_regions/damage.hpp"
#include "../hud_system/dialogue.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::stage_clear {
int update(StageClearInf& o){
    switch(o.state){
    case 1:
        gameplay::controller->game_flags|=0x40u;gameplay::controller->field_10c=0; //511590
        o.panel_handle=sprite::spawn_named_animation(*program_entry::sprite_controller,*o.file,"front",111);
        o.bonus=calculate_bonus(*game_session::context(0).current_player,gameplay::player_state::stage(game_session::session.player_table));
        damage::add_score(game_session::player(0),o.bonus); //488550 zero-extends the wrapped value
        o.state=2;o.field_1c=0;gameplay::slowdown_frames=0;
        [[fallthrough]];
    case 2:o.state=4;[[fallthrough]];
    case 4:
        if(o.age.current>=120){const auto* buttons=input::button_slot(0);if((buttons&&(buttons->pressed&0x80001u))||o.age.current>=300){
            if(gameplay::controller)gameplay::controller->game_flags&=~0x04000000u;
            sprite::request_animation_deletion(*program_entry::sprite_controller,o.panel_handle);o.state=6;recovered::timer_set(o.age,0);hud::fade_stage_track(data::f_0056c8d0);
        }}
        break;
    case 6:
        if(o.age.current==10){if(gameplay::controller)hud::unrecovered::complete_stage_004bc570();runtime::retire_callback_owner(&o);return 1;}
        break;
    default:break;
    }
    recovered::timer_tick(o.age,state::timer_rate);return 1;
}
}
