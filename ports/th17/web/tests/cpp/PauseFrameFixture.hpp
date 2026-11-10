#include "../../cpp/game/PauseFrame.hpp"
struct PauseFrameFixture final:PauseActivationServices {
 PauseMenuFixture menu;Timer session_age;FrameScheduler scheduler;i32 skip=7;
 PauseActivation activation{menu.pause,menu.progress,menu.player,skip,*this};PauseFrame frame{menu.pause,menu.progress,menu.player,session_age,activation,menu.controller,scheduler,101};std::vector<i32> events;
 std::array<u32,4> projection{};
 explicit PauseFrameFixture(AnmManager& a):menu(a){session_age.set(30);}
 bool event(std::initializer_list<i32> e){events.insert(events.end(),e);return true;}
 bool record_elapsed_play_time()override{return event({1});}
 bool menu_visual(u32& handle,i32 script,i32 label)override{handle=0x1111;return event({2,script,label});}
 bool reset_audio_slots()override{return event({3});}bool sound(i32 id)override{return event({4,id});}
 bool suspend_music()override{return event({5});}bool finish_audio_requests()override{return event({6});}
 bool capture_background(u32& handle,bool field)override{handle=0x2222;return event({7,i32(field)});}
 bool freeze_dialogue()override{return event({8});}bool freeze_chapter_result()override{return event({9});}
 bool preserve_current_music(std::string&,double&)override{return false;}bool start_game_over_music()override{return false;}
 bool replay_destination(bool selection)override{menu.progress.screen=selection?2:4;return true;}
 void configure(i32 age,i32 demo,u32 flags,u32 mode,bool replay,float rate){
  menu.pause=PauseState{};menu.pause.flags=7;menu.progress=GameRun{};menu.progress.scene_flags=flags;menu.progress.scoring.mode_flags=mode;menu.progress.demo_slot=demo;menu.progress.practice_record=replay;menu.progress.rate=rate;session_age.set(age);skip=7;events.clear();
 }
};
API(pause_frame_create) PauseFrameFixture* pause_frame_create(AnmManagerFixture* a){return new PauseFrameFixture(a->manager);}
API(pause_frame_delete) void pause_frame_delete(PauseFrameFixture* p){delete p;}
API(pause_frame_configure) void pause_frame_configure(PauseFrameFixture* p,i32 age,i32 demo,u32 flags,u32 mode,bool replay,float rate){p->configure(age,demo,flags,mode,replay,rate);}
API(pause_frame_run) bool pause_frame_run(PauseFrameFixture* p,u32 pressed,u32 repeated,bool focus,bool enabled,bool selection){p->menu.progress.application_flags=selection?0x2000:0;p->events.clear();return p->frame.update({pressed,repeated,focus,enabled});}
API(pause_frame_state) const u8* pause_frame_state(PauseFrameFixture* p){return p->menu.bytes();}
API(pause_frame_events) const i32* pause_frame_events(PauseFrameFixture* p){return p->events.data();}
API(pause_frame_event_count) u32 pause_frame_event_count(PauseFrameFixture* p){return p->events.size();}
API(pause_frame_values) const u32* pause_frame_values(PauseFrameFixture* p){p->projection={p->menu.progress.scene_flags,float_to_bits(p->menu.progress.rate),u32(p->skip),u32(p->menu.progress.screen)};return p->projection.data();}
API(pause_frame_demo) i32 pause_frame_demo(PauseFrameFixture* p){return p->menu.progress.demo_slot;}
API(pause_frame_error) const char* pause_frame_error(PauseFrameFixture* p){return p->frame.error.c_str();}
