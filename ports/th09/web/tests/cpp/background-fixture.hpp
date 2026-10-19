#pragma once
#include "../../cpp/game/Background.hpp"
namespace background_test {
using namespace th09;
struct Fixture:BackgroundServices {
    Background bg;std::vector<std::array<i32,4>> events;
    Fixture():bg(*this){}
    i32 index(AnmVm& a){const auto p=reinterpret_cast<uintptr_t>(&a),first=reinterpret_cast<uintptr_t>(bg.primitives.data());if(p>=first&&p<first+bg.primitives.size()*sizeof(AnmVm))return i32(&a-bg.primitives.data());const auto overlay=reinterpret_cast<uintptr_t>(bg.overlays.data());if(p>=overlay&&p<overlay+sizeof(bg.overlays))return 1000+i32(&a-bg.overlays.data());return 2000+i32(&a-bg.boss_animations.data());}
    void start_animation(AnmVm& a,bool boss,i32 script)override{events.push_back({0,index(a),i32(boss),script});std::memset(&a,0,sizeof(a));a.scriptIndex=i16(script);a.activeSpriteIndex=i16(script+1);a.currentInstruction=reinterpret_cast<AnmRawInstr*>(1);a.color1.d3dColor=-1;}
    void advance_animation(AnmVm& a)override{events.push_back({1,index(a)});a.intVar0=wrapping_add(a.intVar0,1);if(a.intVar0%97==0)a.currentInstruction=nullptr;}
};
struct Field{u32 original,offset,size;};
#define BG_FIELD(original,field) {original,offsetof(Background,field),sizeof(Background::field)}
inline const Field fields[]={BG_FIELD(0x834,goals),BG_FIELD(0x924,starts),BG_FIELD(0xa14,start_tangents),BG_FIELD(0xb04,end_tangents),BG_FIELD(0x87c,fov_goal),BG_FIELD(0x96c,fov_start),BG_FIELD(0xbf4,durations),BG_FIELD(0xc08,interpolation_times),BG_FIELD(0xc44,interpolation_modes),BG_FIELD(0xc58,script_time),BG_FIELD(0xc64,instruction),BG_FIELD(0xc68,frame),BG_FIELD(0xc70,position),BG_FIELD(0xc7c,clear_color),BG_FIELD(0xc94,fog),BG_FIELD(0xca0,fog_start),BG_FIELD(0xcac,fog_goal),BG_FIELD(0xcb8,fog_duration),BG_FIELD(0xcbc,fog_time),BG_FIELD(0x18,requested_label),BG_FIELD(0x1c,transition_state),BG_FIELD(0x20,transition_frames),BG_FIELD(0x2c,overlays),BG_FIELD(0xce0,boss_animations),BG_FIELD(0xccc,boss_state),BG_FIELD(0xcd0,boss_frames),BG_FIELD(0xcd8,boss_count),BG_FIELD(0xcdc,boss_parameter),BG_FIELD(0x642c,tint),BG_FIELD(0x6408,next_position),BG_FIELD(0x6414,next_position_time),BG_FIELD(0x6418,previous_position),BG_FIELD(0x6424,previous_position_time),BG_FIELD(0x6428,jumped),BG_FIELD(0x6438,sway),BG_FIELD(0x6434,distance_squared)};
#undef BG_FIELD
}
