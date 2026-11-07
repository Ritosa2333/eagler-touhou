#pragma once
#include "../../cpp/game/GameFrame.hpp"
struct GameFrameFixture:GameFrameHost {
    GameProgressionFixture& state;GameFrame frame;std::vector<u32> events;i32 initialize_result=0,loading=0;
    explicit GameFrameFixture(GameProgressionFixture& p):state(p),frame(p.run,p.hud.hud,p.hud.score,p.hud.world,*this){}
    bool event(u32 id,u32 a=0,u32 b=0,u32 c=0,u32 d=0){events.insert(events.end(),{id,a,b,c,d});return true;}
    i32 initialize_stage()override{event(1);return initialize_result;}bool start_stage()override{return event(2);}
    i32 loading_state()const noexcept override{return loading;}bool retire_loading()override{loading=0;return event(3);}
    bool return_to_title()override{return event(4);}bool transition(const SceneTransition& s)override{return event(5,u32(s.mode),u32(s.frames),u32(s.layer),s.color);}
    bool pause_stage_music()override{return event(6);}bool interrupt_stage_visual()override{return event(7);}
    bool reload_stage_music(i32 id)override{return event(8,u32(id));}bool flush_audio()override{return event(9);}
    bool resume_environment(double time)override{std::array<u32,2> bits;std::memcpy(bits.data(),&time,8);return event(10,bits[0],bits[1]);}
    bool advance_scene_animations()override{for(u32 i=0;i<4;i++)event(11,i);return true;}
};
API(game_frame_create) GameFrameFixture* game_frame_create(GameProgressionFixture* p){return new GameFrameFixture(*p);}
API(game_frame_delete) void game_frame_delete(GameFrameFixture* p){delete p;}
API(game_frame_update) i32 game_frame_update(GameFrameFixture* p,float rate,u32 pressed){p->events.clear();return p->frame.update({rate,pressed});}
API(game_frame_error) const char* game_frame_error(GameFrameFixture* p){return p->frame.error.c_str();}
API(game_frame_field) void* game_frame_field(GameFrameFixture* p,u32 kind){switch(kind){case 0:return &p->frame.age;case 1:return p->frame.counters.data();case 2:return &p->frame.shutdown_frames;case 3:return &p->frame.global_phase;case 4:return &p->frame.reset_age;case 5:return &p->frame.reset_duration;case 6:return &p->initialize_result;case 7:return &p->loading;case 8:return &p->state.hud.world.current_chapter;case 9:return &p->state.hud.hud.notice_age;default:return nullptr;}}
API(game_frame_events) const u32* game_frame_events(GameFrameFixture* p){return p->events.data();}
API(game_frame_event_size) u32 game_frame_event_size(GameFrameFixture* p){return p->events.size();}
