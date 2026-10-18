#pragma once
#include "selection.hpp"
#include "../replay_system/replay.hpp"
namespace th20::source::title {
extern int last_replay; //5c612c
struct ReplayMenuEnvironment {
    SelectionEnvironment& selection;int& last_replay;
    ReplayMenuEnvironment(SelectionEnvironment& s,int& last):selection(s),last_replay(last){}
    virtual ~ReplayMenuEnvironment()=default;
    virtual void begin_read(TitleInf&)=0;
    virtual bool effects_ready()=0;
    virtual void loading_transition()=0;
    virtual void fade(float)=0;
    virtual void request_start(int,const char*)=0;
    virtual void retire(runtime::CallbackOwner*)=0;
};
ReplayMenuEnvironment& replay_menu_environment();
int update_replay_menu(TitleInf&,ReplayMenuEnvironment&); //523440
void read_replay_list(TitleInf&); //51f490,51fd60
extern char replay_filename_fragment[5]; //5c6100
const char* replay_filename_suffix(const replay::ReplayInf&); //50a220
int draw_replay_menu(TitleInf&); //5240d0
}
