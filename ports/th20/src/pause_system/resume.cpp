#include "menu_support.hpp"
#include "../progress_state/records.hpp"
#include "../stage_completion/progress.hpp"
#include "../progress_state/manager.hpp"
#include "../gameplay/player_state.hpp"
#include "../program_entry/program_entry.hpp"
#include "../player_entity/power.hpp"
#include "../item_system/rewards.hpp"
#include "../audio_runtime/music_stream.hpp"
#include "../hud_system/hud.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include <algorithm>
namespace th20::source::pause {
namespace pe=program_entry;namespace ps=gameplay::player_state;namespace pr=progress;
namespace {
int clamped(game_session::Player& p,unsigned offset,int minimum,int maximum){const int value=std::clamp(ps::read<int>(p,offset),minimum,maximum);ps::write(p,offset,value);return value;}
void resume_effects(){for(auto& effect:pe::thread_registry.effects)if(effect.buffer&&effect.was_playing)effect.buffer->Play(0,0,effect.definition->play_flags);} //4e2080/6450
class BombObserver final:public ps::BombObserver {public:void update(int count,int fragments,int maximum)override{hud::set_bombs(*hud::controller,count,fragments,maximum);}};
void continue_game(PauseInf& o,Services& e){
    BombObserver observer;
    auto& player=*e.session().contexts[0].current_player;
    ps::set(player,0x4bde90,2);ps::set(player,0x4bdf30,0);ps::set_bombs(player,3,hud::controller?&observer:nullptr);ps::set(player,0x4bda80,0);ps::set(player,0x4be0a0,0);
    item::add_power(player,recovered::signed_bits(static_cast<unsigned>(ps::starting_power(player))*4u),item::reward_environment());
    player_entity::refresh_power(*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]),-1);
    const int max_lives=clamped(player,0xbc,0,7),life_fragments=clamped(player,0xc0,0,10),lives=clamped(player,0xb8,-1,7);hud::set_lives(*hud::controller,lives,life_fragments,max_lives);
    const int max_bombs=clamped(player,0xd8,0,7),bomb_fragments=clamped(player,0xd0,0,10),bombs=clamped(player,0xcc,0,10);hud::set_bombs(*hud::controller,bombs,bomb_fragments,max_bombs);
    game_session::increment_continue_count();pr::write(&e.session(),0x78,recovered::signed_bits(static_cast<unsigned>(credits(e.session()))-1u));pr::write(&e.session().player_table.players[0],0,std::uint64_t(0));
    e.game().game_flags&=~0x10u;resume_effects();pe::thread_registry.enqueue(2,-1,o.saved_music_name);while(e.poll_audio()!=0){}pe::thread_registry.stream->seek_seconds(o.saved_music_position);
    e.set_clock_scale(o.saved_clock_scale);if(e.dialogue_present())e.show_dialogue(true);e.show_hud_message(true);e.input_latch()=o.saved_input;
}
}
void finish_choice(PauseInf& o,Services& e){
    if(o.state==1)restore_after_pause(o,e);else if(o.state==2||o.state==3)restore_after_result(o,e);
    const int choice=o.cursor.current;
    if(choice==0){
        if(o.state==1){resume_effects();pe::thread_registry.enqueue(7,0,"UnPause");}
        else if(o.state==2){if(o.completed==0){if(ps::stage(e.session().player_table)==7)e.select_scene(14,false);else continue_game(o,e);}else e.select_scene(10,false);}
    }else if(choice==1||choice==5){
        sprite::spawn_named_animation(*pe::sprite_controller,*pe::graphics_state.surface_animation,"text",3);
        e.delete_animation(o.background_handle);e.delete_animation(o.panel_handle);
        if(choice==1){e.select_scene(4,true);const int stage=ps::stage(e.session().player_table);
            if(stage>3&&stage<7&&o.state==2&&e.session().mode==0&&e.game().restart()==0){const auto index=ps::read<std::uint32_t>(*e.session().contexts[0].current_player,0xc);if(static_cast<int>(pr::manager->stone_count(index))<9){pr::notify_unlock(*pr::manager,index+1);pr::grant_stone(*pr::manager,index);}}
        }else if(e.game().restart()==0){e.select_scene(e.session().mode!=0||ps::difficulty(e.session().player_table)==4?10:24,false);}else e.select_scene(11,false);
    }
    set_state(o,0);
}
}
