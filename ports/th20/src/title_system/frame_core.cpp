#include "frame.hpp"
#include "data.hpp"
#include "../runtime_state/state.hpp"
#include "../pause_system/menu_support.hpp"
#include <cstring>
namespace th20::source::title {
namespace {
void launch_demo(FrameEnvironment& e){
    auto& s=e.main.session;
    s.flags=(s.flags&~0x60u)|0x20u;
    strcpy_s(e.replay_filename,256,data::demo_files[static_cast<int>(s.fields_74[3])]);
    auto* metadata=e.read_replay(e.replay_filename);
    s.fields_74[3]=static_cast<unsigned>(recovered::signed_bits(s.fields_74[3]+1)%4);
    //488770 reads playback[index].stage (+e8+index*2c+10), not stages[].
    int stage=0;while(stage<8&&!metadata->playback[stage].stage)++stage;
    e.select_stage(stage);e.next_scene=13;
    auto& user=*metadata->user;set_character(s,static_cast<int>(user.fields_d0[2]));
    auto& p=*s.contexts[0].current_player;
    p.fields_00[3]=user.stones[0];p.fields_00[5]=user.stones[1];p.fields_00[4]=user.stones[2];p.fields_00[6]=user.stones[3];
    s.field_2bc=s.player_table.field_1e0;s.player_table.field_1e0=user.difficulty;
    e.retire(metadata);e.menu_selection=1;s.fields_74[2]=0;
}
void prepare_transition(TitleInf& o,FrameEnvironment& e,int next){
    e.transition_effect();e.main.spawn(o,0);e.main.interrupt(o,0,3,false);set_state(o,next);scheduler::enable(*o.draw_node);
}
void initialize_state(TitleInf& o,FrameEnvironment& e){
    auto& s=e.main.session;auto& c=o.cursor;
    e.slowdown_frames=0;e.replay_selection=-1;
    e.seed(0,e.entropy());e.seed(1,e.entropy());e.enable_stones();
    e.release_mesh(o.mesh);o.mesh=nullptr;
    for(int index:{5,7,2,0})e.hide_file(index);
    e.clear_loading();e.background(o);
    if(e.menu_selection==3){c.count=10;c.select(0);pause::save_cursor(c);set_state(o,15);e.menu_selection=1;scheduler::enable(*o.draw_node);return;}
    if(!(s.flags&0x20)){o.ui_flags|=1;o.music_delay=0;}else{o.ui_flags&=~1u;s.player_table.field_1e0=s.field_2bc;}
    s.flags&=~0x20u;o.ui_flags=e.menu_selection==0?o.ui_flags|2:o.ui_flags&~2u;
    switch(e.menu_selection){
    case 0:set_state(o,1);e.menu_selection=1;break;
    case 1:if(s.player_table.field_1e0==4)c.select(1);set_state(o,1);break;
    case 2:set_mode(s,0);c.count=10;c.select(4);pause::save_cursor(c);set_state(o,12);e.menu_selection=1;scheduler::enable(*o.draw_node);break;
    case 6:set_mode(s,0);c.count=10;c.select(5);pause::save_cursor(c);c.count=10;c.select(2);pause::save_cursor(c);set_character(s,e.data_character);s.contexts[0].current_player->fields_00[3]=e.data_profile;e.select_profile(static_cast<int>(s.contexts[0].current_player->fields_00[2]),static_cast<int>(s.contexts[0].current_player->fields_00[3]));prepare_transition(o,e,23);e.menu_selection=1;break;
    case 5:prepare_transition(o,e,6);break;
    case 4:prepare_transition(o,e,5);break;
    default:break; //Original falls through to the main page, preserving state0.
    }
}
}
int update(TitleInf& o,FrameEnvironment& e){
    auto& s=e.main.session;
    if(auto* ending=e.current_ending()){if(ending->ending_flags&8){e.retire(ending);e.play_music(data::s_00575098);}return 1;}
    if(o.state==1){++s.fields_74[2];const auto* b=e.buttons();if(b&&(b->current&0xffff))s.fields_74[2]=0;if(recovered::signed_bits(s.fields_74[2])>1799)launch_demo(e);}
    if(o.ui_flags&1){o.music_delay=recovered::signed_bits(static_cast<unsigned>(o.music_delay)+1);if(e.menu_selection!=1||o.music_delay>29){e.stop_music();e.queued_track_first=0;e.play_music(data::s_00575098);o.ui_flags&=~1u;o.music_delay=0;}}
    const bool just_initialized=o.state==0;if(just_initialized)initialize_state(o,e);
    if(just_initialized&&o.state==0){scheduler::enable(*o.draw_node);e.update_page(o,1);}
    else switch(o.state){
    case 1:scheduler::enable(*o.draw_node);e.update_page(o,1);break;
    case 2:e.request_scene(3);e.stop_music();break;
    case 9:case 13:e.stop_music();break;
    case 3:case 5:case 6:case 7:case 8:case 10:case 11:case 12:case 14:case 15:case 16:case 17:case 18:case 19:case 20:case 23:e.update_page(o,o.state);break;
    case 24:break; //412540 returns0 with no effects; its return is discarded.
    }
    recovered::timer_tick(o.age,state::timer_rate);
    if(o.selection_age.current>0)recovered::timer_add(o.selection_age,-1.f,state::timer_rate);
    if(o.flash_age.current>0)recovered::timer_add(o.flash_age,-1.f,state::timer_rate);
    return 1;
}
int draw(TitleInf& o,FrameEnvironment& e){
    if(e.current_ending())return 1;
    switch(o.state){case 1:case 8:case 11:case 12:case 14:case 15:case 16:case 19:case 20:case 23:e.draw_page(o,o.state);break;}
    return 1;
}
}
