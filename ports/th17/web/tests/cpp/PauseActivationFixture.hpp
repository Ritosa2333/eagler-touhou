#include "../../cpp/game/PauseActivation.hpp"
struct PauseActivationFixture final:PauseActivationServices {
    PauseState state;GameRun run;PlayerLifeSession player;i32 skip=7;std::vector<i32> events;PauseActivation activation{state,run,player,skip,*this};std::array<u8,0x3f8> projection{};std::array<u32,4> values{};
    bool event(std::initializer_list<i32> value){events.insert(events.end(),value);return true;}
    bool record_elapsed_play_time()override{return event({1});}
    bool menu_visual(u32& h,i32 script,i32 interrupt)override{h=0x1111;return event({2,script,interrupt});}
    bool reset_audio_slots()override{return event({3});}bool sound(i32 id)override{return event({4,id});}
    bool suspend_music()override{return event({5});}bool finish_audio_requests()override{return event({6});}
    bool capture_background(u32& h,bool playfield)override{h=0x2222;return event({7,i32(playfield)});}
    bool freeze_dialogue()override{return event({8});}bool freeze_chapter_result()override{return event({9});}
    bool preserve_current_music(std::string& n,double& p)override{n="native-current.wav";p=123.125;return true;}
    bool start_game_over_music()override{return event({11,12});}
    bool replay_destination(bool selection)override{run.screen=selection?2:4;return event({13,run.screen});}
    void configure(u32 mode,bool replay,float rate,i32 screen,u32 flags){state=PauseState{};state.screen=PauseScreen(screen);state.flags=flags;state.saved_rate=.25f;state.saved_frame_skip=3;run=GameRun{};run.rate=rate;run.scene_flags=0x4080;run.scoring.mode_flags=mode;run.practice_record=replay;skip=7;events.clear();activation.error.clear();}
    const u8* bytes(){projection.fill(0);auto put=[&](u32 at,const auto& value){std::memcpy(projection.data()+at,&value,sizeof value);};put(0xc,state.age);put(0x20,state.selection_age);put(0x108,state.menu.disabled_count);put(0x1e4,state.menu_animation);put(0x1e8,state.snapshot_animation);put(0x1ec,state.screen);put(0x1f0,state.previous);put(0x1f4,state.phase);put(0x1fc,state.result_mode);put(0x208,state.saved_frame_skip);put(0x2e0,state.saved_rate);put(0x2e8,state.saved_music_position);std::memcpy(projection.data()+0x2f0,state.saved_music.c_str(),state.saved_music.size()+1);put(0x3f0,state.flags);put(0x3f4,state.front_bank);return projection.data();}
};
API(pause_activation_create) PauseActivationFixture* pause_activation_create(){return new PauseActivationFixture;}
API(pause_activation_delete) void pause_activation_delete(PauseActivationFixture* p){delete p;}
API(pause_activation_configure) void pause_activation_configure(PauseActivationFixture* p,u32 mode,bool replay,float rate,i32 screen,u32 flags){p->configure(mode,replay,rate,screen,flags);}
API(pause_activation_open) bool pause_activation_open(PauseActivationFixture* p,i32 entrance,bool selection){return p->activation.open(PauseEntrance(entrance),101,selection);}
API(pause_activation_state) const u8* pause_activation_state(PauseActivationFixture* p){return p->bytes();}
API(pause_activation_events) const i32* pause_activation_events(PauseActivationFixture* p){return p->events.data();}
API(pause_activation_event_count) u32 pause_activation_event_count(PauseActivationFixture* p){return p->events.size();}
API(pause_activation_values) const u32* pause_activation_values(PauseActivationFixture* p){p->values={p->run.scene_flags,float_to_bits(p->run.rate),u32(p->skip),u32(p->run.screen)};return p->values.data();}
API(pause_activation_error) const char* pause_activation_error(PauseActivationFixture* p){return p->activation.error.c_str();}
