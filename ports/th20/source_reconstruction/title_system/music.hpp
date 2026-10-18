#pragma once
#include "title.hpp"
#include "../progress_state/manager.hpp"
#include <string_view>
namespace th20::source::title {
struct MusicEnvironment {
    virtual ~MusicEnvironment()=default;
    virtual bool pressed(unsigned)=0;
    virtual bool repeated(unsigned)=0;
    virtual bool unlocked(int)=0;
    virtual void spawn_background(TitleInf&)=0;
    virtual void spawn_heading(TitleInf&)=0;
    virtual void retire_heading(TitleInf&)=0;
    virtual void begin_read(TitleInf&)=0;
    virtual void interrupt(unsigned,int)=0;
    virtual void sound(int)=0;
    virtual void play(const char*)=0;
    virtual void stop()=0;
};
void parse_music_comments(TitleInf&,std::string_view); //51f890 parse,32 slots
void read_music_comments(TitleInf&); //51f890 resource wrapper
int update_music(TitleInf&,MusicEnvironment&); //5205d0
void draw_music(TitleInf&,text::Renderer&,const progress::Metadata&); //520c80
MusicEnvironment& music_environment();
}
