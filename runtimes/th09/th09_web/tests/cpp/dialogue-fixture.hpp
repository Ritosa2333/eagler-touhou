#pragma once
#include "../../cpp/game/Dialogue.hpp"
namespace dialogue_test {
using namespace th09;
struct Fixture:DialogueServices {
    Dialogue dialogue;MessageResource resources[2];Rng random;
    std::vector<std::array<i32,8>> events;std::vector<u8> text_bytes;std::vector<AttackColorVertex> panel;
    i32 backgrounds[2]{};u8 game_over=0,match_complete=0;i32 transition_code=0;
    Fixture():dialogue(*this){dialogue.set_resources(resources[0],&resources[1]);}
    i32 index(const AnmVm& vm){return i32(&vm-dialogue.animations.data());}
    void start_animation(AnmVm& vm,DialogueResource res,i32 script)override{events.push_back({0,index(vm),i32(res),script});std::memset(&vm,0,sizeof(vm));vm.scriptIndex=i16(script);vm.flags=1;vm.color1.d3dColor=-1;}
    void set_sprite(AnmVm& vm,DialogueResource res,i32 sprite)override{events.push_back({1,index(vm),i32(res),sprite});vm.activeSpriteIndex=i16(sprite);}
    void advance_animation(AnmVm& vm)override{events.push_back({2,index(vm)});vm.intVar0=wrapping_add(vm.intVar0,1);}
    void draw_animation(AnmVm& vm,bool right)override{events.push_back({3,index(vm),i32(right)});}
    void text(AnmVm& vm,u32 color,u32 shadow,const std::string& text)override{events.push_back({4,index(vm),i32(color),i32(shadow),i32(text.size())});text_bytes.insert(text_bytes.end(),text.begin(),text.end());text_bytes.push_back(0);}
    void music(i32 track)override{events.push_back({5,track});}
    void fade_music()override{events.push_back({6});}
    void show_results()override{events.push_back({7});}
    void transition(DialogueTransition t)override{if(t==DialogueTransition::game_over)game_over=1;else if(t==DialogueTransition::match_complete)match_complete=1;else transition_code=3;}
    void white_transition()override{events.push_back({9});}
    void enter_player(i32 side)override{events.push_back({10,side});}
    void show_huds()override{events.push_back({11,0});events.push_back({11,1});}
    void background_setting(i32 s,i32 value)override{backgrounds[s]=value;}
    void draw_panel(const AttackColorVertex* vertices,u32 count)override{events.push_back({12,i32(count)});panel.assign(vertices,vertices+count);}
    void clear(){events.clear();text_bytes.clear();panel.clear();}
};
struct Field {u32 original,offset,size;};
#define DIALOGUE_FIELD(original,field) {original,offsetof(Dialogue,field),sizeof(Dialogue::field)}
inline const Field fields[]={DIALOGUE_FIELD(8,id),DIALOGUE_FIELD(0x10,inverted),DIALOGUE_FIELD(0x14,animations),DIALOGUE_FIELD(0x1d20,colors),DIALOGUE_FIELD(0x1d30,shadows),DIALOGUE_FIELD(0x1d40,time),DIALOGUE_FIELD(0x1d4c,wait_frames),DIALOGUE_FIELD(0x1d50,minimum_wait),DIALOGUE_FIELD(0x1d54,font_size),DIALOGUE_FIELD(0x1d58,box_time),DIALOGUE_FIELD(0x1d64,speaker),DIALOGUE_FIELD(0x1d65,new_page),DIALOGUE_FIELD(0x1d66,line_number),DIALOGUE_FIELD(0x1d67,previous_speaker),DIALOGUE_FIELD(0x1d68,counter),DIALOGUE_FIELD(0x1d6c,skippable),DIALOGUE_FIELD(0x1d6d,box_visible)};
#undef DIALOGUE_FIELD
}
