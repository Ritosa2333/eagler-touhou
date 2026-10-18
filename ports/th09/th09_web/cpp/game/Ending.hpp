#pragma once
#include "GameResources.hpp"
#include "GameInput.hpp"
namespace th09 {
struct EndingServices {
    virtual ~EndingServices()=default;
    virtual bool ending_picture(const char*)=0;
    virtual void ending_music(i32)=0;
    virtual void ending_music_fade(i32 seconds)=0;
    virtual void ending_text(AnmVm&,const char*,u32)=0;
    virtual void ending_background(i32 x,i32 y)=0;
    virtual void ending_sprite(AnmVm&)=0;
    virtual void ending_cover(u32)=0;
};
struct EndingState {
    float x=0,y=0,scroll=0;
    Timer elapsed,line_wait,page_wait;
    i32 page_lock=0,line_lock=0,line_delay=8,fast_delay=0,line=0;
    u32 text_color=0,cover_color=0;
    i32 fade_frame=0,fade_duration=0,fade_mode=0;
};
class Ending {
    GameResources& resources;EndingServices& output;
    std::vector<u8> script;
    bool load(const char*);bool number(i32&);bool skip_line();void fade();bool step(const InputFrame&);
public:
    EndingState state;std::array<AnmVm,16> animations;
    u32 cursor=0;bool skip_enabled=true,finished=false,ready=false;std::string filename,error;
    FrameTiming timing;
    Ending(GameResources& r,EndingServices& s):resources(r),output(s){std::memset(animations.data(),0,sizeof(animations));}
    bool initialize(i32 character);
    bool update(const InputFrame&);
    void draw();
};
}
