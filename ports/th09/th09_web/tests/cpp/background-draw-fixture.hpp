#pragma once
#include "background-fixture.hpp"
#include "../../cpp/game/StageRenderer.hpp"
namespace background_draw_test {
using namespace th09;
inline u32 bits(float n){u32 value;std::memcpy(&value,&n,4);return value;}
struct Fixture:background_test::Fixture,BackgroundDrawServices {
    BackgroundDrawState state;bool custom=false;std::vector<std::array<u32,6>> calls;std::vector<AnmVm> animations;
    void select_field(i32 side)override{calls.push_back({1,u32(side)});}
    void flush()override{calls.push_back({2});}
    void clear(bool color,bool depth,u32 value)override{calls.push_back({3,u32(color)|(u32(depth)<<1),value});}
    void rectangle(float l,float t,float r,float b,u32 value)override{calls.push_back({5,bits(l),bits(t),bits(r),bits(b),value});}
    void draw_animation(AnmVm& vm)override{calls.push_back({6,u32(index(vm))});animations.push_back(vm);}
    void models(Background&,i32 layer)override{calls.push_back({7,u32(layer)});}
    void depth_compare(Compare c)override{calls.push_back({4,23,u32(c)+1});}
    void fog(bool value)override{calls.push_back({0,u32(value)});}
    void fog_color(u32 value)override{calls.push_back({4,34,value});}
    void fog_range(float a,float b)override{calls.push_back({4,36,bits(a)});calls.push_back({4,37,bits(b)});}
    void texture_mode(bool value)override{calls.push_back({8,u32(value)});}
    void screen_camera()override{calls.push_back({9});}
    void custom_boss(Background&)override{if(custom)calls.push_back({10});}
    void draw(u32 phase){calls.clear();animations.clear();if(phase)BackgroundDraw::overlay(bg,state,*this);else BackgroundDraw::base(bg,state,*this);}
};
}
