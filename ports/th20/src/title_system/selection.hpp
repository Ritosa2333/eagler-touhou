#pragma once
#include "title.hpp"
namespace th20::source::title {
class SelectionEnvironment {
public:
    MainEnvironment& main;
    std::uint32_t& menu_selection;
    int& last_profile;
    SelectionEnvironment(MainEnvironment& m,std::uint32_t& selection,int& profile):main(m),menu_selection(selection),last_profile(profile){}
    virtual ~SelectionEnvironment()=default;
    virtual void spawn_text_overlay(TitleInf&)=0;
    virtual void signal(TitleInf&,int index,int event,bool execute)=0;
    virtual void child_visible(TitleInf&,int index,int child,bool)=0;
    virtual void child_signal(TitleInf&,int index,int child,int event,bool execute)=0;
    virtual bool all_cleared(int difficulty,int character)=0; //character-1 selects52c640
    virtual bool character_extra_unlocked(int)=0;
    virtual int selected_profile(int slot,int character)=0;
    virtual void commit_progress()=0;
    virtual void set_weapon(int slot,int character,int profile)=0;
};
SelectionEnvironment& selection_environment();
int update_difficulty(TitleInf&,SelectionEnvironment&); //521bc0
void update_character(TitleInf&,SelectionEnvironment&); //521060
}
