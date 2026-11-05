#pragma once
#include "title.hpp"
#include "../progress_state/manager.hpp"
#include <array>
#include <span>
#include <stdexcept>
namespace th20::source::title {
extern int last_stage; //5b0a50
extern std::array<std::uint8_t,256> shortcut_keyboard; //5c6148
int read_shortcut_keyboard(); //51f310
int read_shortcut_keyboard(std::span<std::uint8_t,256>);
struct StageEnvironment {
    game_session::Session& session;std::uint32_t& menu_selection;int& last_stage;
    StageEnvironment(game_session::Session& s,std::uint32_t& m,int& last):session(s),menu_selection(m),last_stage(last){}
    virtual ~StageEnvironment()=default;
    virtual bool pressed(unsigned)=0;
    virtual bool repeated(unsigned)=0;
    virtual const progress::Profile& profile()=0;
    virtual int keyboard_number()=0;
    virtual void spawn_heading(TitleInf&)=0;
    virtual void retire_heading(TitleInf&)=0;
    virtual void sound(int)=0;
    virtual void fade(float)=0;
    virtual bool effects_ready()=0;
    virtual void loading()=0;
    virtual void request_start(int)=0;
};
StageEnvironment& stage_environment();
//52ca30 after current-profile selection
inline bool stage_available(const progress::Profile& p,int difficulty,int index){if(difficulty<0||difficulty>=5||index<0||index>=9)throw std::out_of_range("Practice stage record index");const auto offset=0x76f8+difficulty*0x90+index*16;return p.bytes[offset+8]!=0||p.bytes[offset+9]!=0;}
int update_stage(TitleInf&,StageEnvironment&); //529430
void draw_stage(TitleInf&,text::Renderer&,const progress::Profile&,int difficulty); //529b40
}
