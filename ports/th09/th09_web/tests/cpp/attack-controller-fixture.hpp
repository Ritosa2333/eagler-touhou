#pragma once
#include "../../cpp/game/AttackController.hpp"
#include <vector>
namespace attack_controller_test {
using namespace th09;
struct Fixture:AttackControllerServices {
    AttackController controller;EclVm bosses[2];u32 has_boss[2]{};i32 background[2][2]{};i32 sequence=0;bool announce=true;
    std::vector<std::array<i32,8>> events;
    explicit Fixture(i32 side):controller(side,*this){}
    i32 index(const AnmVm& vm){return i32(&vm-controller.animations.data());}
    EclVm* boss(i32 s)override{events.push_back({0,s});return has_boss[s]?&bosses[s]:nullptr;}
    void spawn_attack_enemy(i32 side,i32 script,i32 life,i32 score)override{events.push_back({1,side,script,life,score});if(script==2)has_boss[side]=1;if(announce)controller.notify_pattern(u32(script+1));}
    void start_animation(AnmVm& vm,i32 script)override{events.push_back({2,index(vm),script});std::memset(&vm,0,sizeof(vm));vm.scriptIndex=i16(script);vm.flags=3;vm.color1.d3dColor=i32(0x80706050);vm.pos2={float(script*3-17),float(script*2+3),.25f};}
    void set_sprite(AnmVm& vm,bool ascii,i32 sprite)override{events.push_back({3,index(vm),i32(ascii),sprite});vm.activeSpriteIndex=i16(sprite);}
    bool advance_animation(AnmVm& vm)override{events.push_back({4,index(vm)});return sequence%43==42;}
    void draw_animation(AnmVm& vm)override{events.push_back({5,index(vm)});}
    void draw_text(AnmVm& vm,const char*,u32 color,u32 shadow)override{events.push_back({6,index(vm),i32(color),i32(shadow)});}
    void play_sound(i32 id,i32 pan)override{events.push_back({7,id,pan});}
    void background_transition(i32 s,i32 state,i32 frames)override{background[s][0]=state;background[s][1]=frames;}
    void reset_background(i32 s)override{events.push_back({9,s});}
    void boss_background(i32 s)override{events.push_back({10,s});}
    void portrait(i32 s,u32 layer,i32 script)override{events.push_back({11,s,i32(layer),script});}
    void begin_draw(i32 s)override{events.push_back({12,s});}
};
struct Field{u32 original,offset,size;};
inline const Field fields[]={{0x18,offsetof(AttackController,name),128},{0x98,offsetof(AttackController,time),12},{0xa4,offsetof(AttackController,notices),8},{0xac,offsetof(AttackController,animations),7*sizeof(AnmVm)},{0x1348,offsetof(AttackController,parameters),24}};
}
