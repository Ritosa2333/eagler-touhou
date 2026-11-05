#pragma once
#include "selection.hpp"
namespace th20::source::title {
class LoadoutEnvironment {
public:
    SelectionEnvironment& selection;
    explicit LoadoutEnvironment(SelectionEnvironment& value):selection(value){}
    virtual ~LoadoutEnvironment()=default;
    virtual int clear_count(int difficulty,int character,int profile)=0;
    virtual void open_stones()=0;
    virtual int stone_display_state()=0; //51bb90
    virtual int stone_visibility()=0; //52c9c0/52c700 read+14
    virtual bool effects_ready()=0;
    virtual void loading_transition()=0; //4a0aa0,49dcf0,51c6b0,4d99d0
    virtual void request_start(int stage)=0;
};
LoadoutEnvironment& loadout_environment();
void update_loadout(TitleInf&,LoadoutEnvironment&); //52b8e0
}
