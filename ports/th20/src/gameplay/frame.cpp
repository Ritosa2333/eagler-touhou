#include "frame.hpp"
#include "loading_dependencies.hpp"
#include "../program_entry/program_entry.hpp"
#include "../stage_background/background.hpp"
#include "../screen_effect/effect.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../input/input.hpp"
#include "../hud_system/scoring.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::gameplay {
void request_scene(int scene){auto& g=program_entry::graphics_state;g.field_0b0c=(g.event_flags&0x200u)?2:scene;}
namespace {
void increment_counter(unsigned offset,int amount){auto& table=game_session::session.player_table;const auto current=player_state::read<unsigned>(table,offset);player_state::set_table_counter(table,offset,recovered::signed_bits(current+static_cast<unsigned>(amount)),999999999);}
}
namespace unrecovered {
int update_game(GameController& game){
    ++game.load_stage;increment_counter(0x1f0,1);
    if(program_entry::graphics_state.event_flags&0x60u)return 0;
    if(game.game_flags&0x80u){
        if(game.game_flags&0x10u)return 3;
        ++game.field_10c;
        if(game.restart_mode!=0&&game.field_10c==120)finish_replay_004e5f30();
        if(game.restart_mode==0&&game.field_10c==180)screen::create_effect(5,60,0,0,0,103);
        if(game.restart_mode==0&&recovered::signed_bits(game.field_10c)>239)request_scene(player_state::difficulty(game_session::session.player_table)==4?16:15);
    }
    if(game.frame_timer.current==0){if(start_game_frame(game)!=0)return 1;}
    else if(game.frame_timer.current==30)complete_background_transition(game);
    if(background::secondary&&(background::secondary->state_flags&8u))runtime::retire_callback_owner(background::secondary);
    if(game.frame_timer.current==5)program_entry::window_state.input_latch=2;
    if(game.game_flags&4u){game.game_flags|=0x100u;return 1;}
    if(game_session::flags()&0x20u){
        const auto* buttons=input::button_slot(0);if((buttons&&(buttons->current&0x8010fu))||(game.game_flags&0x70u))request_scene(4);
        if(game.frame_timer.current==3540)screen::create_effect(5,60,0,0,0,109);else if(game.frame_timer.current==3600)request_scene(4);
    }
    hud::update_score(*hud::controller);
    if(game.game_flags&0x30u)return 3;
    for(unsigned i=0;i<4;++i)sprite::execute_animation(*program_entry::graphics_state.surface_sprites[i]);
    increment_counter(0x1ec,0);increment_counter(0x1f0,0);
    if(slowdown_frames)--slowdown_frames;
    recovered::timer_tick(game.frame_timer,state::timer_rate);increment_counter(0x1ec,1);return 1;
}
int draw_game(GameController&){auto& sprites=*program_entry::sprite_controller;sprites.field_bc=0;sprites.field_c0=0;sprites.field_b8=0;sprites.draw_calls=0;return 1;} //475170 +4ba870
}
}
