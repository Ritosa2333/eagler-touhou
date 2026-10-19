#include "pause.hpp"
#include <cstring>
namespace th20::source::pause {
void set_state(PauseInf& o,int state) {
    o.previous_state=o.state;o.state=state;o.substate=0;
    recovered::timer_set(o.age,0);recovered::timer_set(o.secondary_age,0);
    o.cursor.excluded.clear();
}
void set_substate(PauseInf& o,int substate){o.substate=substate;recovered::timer_set(o.age,0);}
namespace {
void save_clock(PauseInf& o,Services& e){o.saved_clock_scale=e.clock_scale();e.set_clock_scale(1.f);}
void save_input(PauseInf& o,Services& e,std::uint32_t replacement){o.saved_input=e.input_latch();e.input_latch()=replacement;}
void hide_dialogue(Services& e){if(e.dialogue_present())e.show_dialogue(false);e.show_hud_message(false);}
void pause_audio(Services& e){e.stop_effects();e.effect(14);if(e.session().mode!=2)e.pause_music();while(e.poll_audio()!=0){}}
}
void open_pause(PauseInf& o,Services& e) {
    e.update_playtime();set_state(o,1);e.game().game_flags|=0x10;
    o.file=e.hud_file();e.delete_animation(o.panel_handle);
    o.panel_handle=e.spawn_panel(*o.file,e.game().restart()==0?0x90:0x91);
    e.interrupt_animation(o.panel_handle,3);pause_audio(e);e.capture_background(o);
    save_clock(o,e);save_input(o,e,0);hide_dialogue(e);e.hide_hud_numbers();o.menu_flags&=~4u;
}
void finish_game(PauseInf& o,Services& e) {
    e.update_playtime();if(e.game().restart()==1){e.select_scene(4,true);return;}
    set_state(o,2);set_substate(o,2);e.game().game_flags|=0x10;
    pause_audio(e);e.capture_background(o);o.file=e.hud_file();
    if(e.session().mode!=2){strcpy_s(o.saved_music_name,e.music_name());o.saved_music_position=e.music_position();e.play_game_over_music();}
    o.completed=0;save_clock(o,e);save_input(o,e,1);o.menu_flags&=~4u;
}
void finish_practice(PauseInf& o,Services& e) {
    e.update_playtime();if(e.game().restart()==1){e.select_scene(4,true);return;}
    e.game().game_flags|=0x10;set_state(o,3);set_substate(o,3);
    if(e.session().mode==2)set_substate(o,5);
    if(e.session().mode==0)e.capture_practice_background(o);
    o.file=e.hud_file();o.completed=1;save_clock(o,e);save_input(o,e,1);hide_dialogue(e);o.menu_flags|=4;
}
void finish_replay(PauseInf& o,Services& e) {
    if(e.replay_finished())return;
    set_state(o,1);set_substate(o,1);e.game().game_flags|=0x10;e.capture_background(o);o.file=e.hud_file();
    e.delete_animation(o.panel_handle);o.panel_handle=e.spawn_panel(*o.file,0x92);e.interrupt_animation(o.panel_handle,3);
    e.stop_effects();e.pause_music();save_clock(o,e);save_input(o,e,0);hide_dialogue(e);e.hide_hud_numbers();o.menu_flags&=~4u;e.mark_replay_finished();
}
void restore_after_pause(PauseInf& o,Services& e) {
    e.reset_playtime_origin();e.game().game_flags&=~0x10u;e.set_clock_scale(o.saved_clock_scale);
    if(e.dialogue_present())e.show_dialogue(true);e.show_hud_message(true);e.input_latch()=o.saved_input;
}
void restore_after_result(PauseInf& o,Services& e) {
    e.reset_playtime_origin();e.interrupt_animation(o.background_handle,1);e.interrupt_animation(o.panel_handle,1);e.set_clock_scale(o.saved_clock_scale);
}
int update(PauseInf& o,Services& e) {
    switch(o.state){
    case 0:
        if((e.session().flags&0x20)==0&&(e.game().game_flags&0x10000)==0&&e.replay_selection()<0){
            if((e.pressed(0x100)||(e.graphics_flags()&8)!=0)&&e.game().update_node&&(e.game().update_node->flags&2u)!=0&&e.game().frame_timer.current>29)open_pause(o,e);
        }
        if(e.replay_selection()>=0&&e.pressed(0x100)){e.select_scene(4,true);e.replay_selection()=-1;return 1;}
        break;
    case 1:case 2:case 3:update_menu(o,e);break;
    }
    const float rate=e.clock_scale();recovered::timer_tick(o.age,&rate);recovered::timer_tick(o.secondary_age,&rate);return 1;
}
}
