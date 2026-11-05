#pragma once
#include "../../cpp/game/Ending.hpp"
#include <map>
namespace ending_test {
using namespace th09;
struct Fixture:ResourceReader,ResourceTextures,EndingServices {
    std::map<std::string,std::vector<u8>> files;Rng random;AnmExecutor executor{random};GameResources resources{*this,*this,executor};Ending ending{resources,*this};
    std::vector<i32> events;std::vector<u8> text;u32 handles=0;
    bool read(const char* n,std::vector<u8>& out)override{auto it=files.find(n);if(it==files.end())return false;out=it->second;return true;}
    TextureAllocation create(const AnmTextureSource& s,const u8*,u32)override{return {++handles,s.width,s.height};}void destroy(u32)override{}
    void string(const char* s){text.insert(text.end(),s,s+std::strlen(s));text.push_back(0);}
    bool ending_picture(const char* n)override{events.insert(events.end(),{0,0});string(n);return true;}
    void ending_music(i32 n)override{events.insert(events.end(),{1,n});}
    void ending_music_fade(i32 n)override{events.insert(events.end(),{2,n});}
    void ending_text(AnmVm& vm,const char* s,u32 color)override{events.insert(events.end(),{3,i32(&vm-ending.animations.data()),4,i32(color)});string(s);}
    void ending_background(i32 x,i32 y)override{events.insert(events.end(),{5,x,6,y});}
    void ending_sprite(AnmVm& vm)override{events.insert(events.end(),{7,i32(&vm-ending.animations.data())});}
    void ending_cover(u32 color)override{events.insert(events.end(),{8,i32(color)});}
    void clear(){events.clear();text.clear();}
};
#define ENDING_FIELD(n,f) {n,offsetof(EndingState,f),sizeof(EndingState::f)}
constexpr u32 fields[][3]={ENDING_FIELD(8,x),ENDING_FIELD(12,y),ENDING_FIELD(16,scroll),ENDING_FIELD(0x2a64,elapsed),ENDING_FIELD(0x2a70,line_wait),ENDING_FIELD(0x2a7c,page_wait),ENDING_FIELD(0x2a88,page_lock),ENDING_FIELD(0x2a8c,line_lock),ENDING_FIELD(0x2a90,line_delay),ENDING_FIELD(0x2a94,fast_delay),ENDING_FIELD(0x2a9c,line),ENDING_FIELD(0x2aa0,text_color),ENDING_FIELD(0x2aa4,cover_color),ENDING_FIELD(0x2aa8,fade_frame),ENDING_FIELD(0x2aac,fade_duration),ENDING_FIELD(0x2ab0,fade_mode)};
#undef ENDING_FIELD
}
