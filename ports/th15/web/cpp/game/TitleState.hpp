#pragma once
#include "MenuCursor.hpp"
#include "Timer.hpp"
namespace th15 {
enum class TitleScreen:i32 {Initialize=0,Main=1,Quit=2,StartGame=2,Options=3,Controller=4,Mode=5,Difficulty=6,Character=7,Stage=9,PlayerData=11,Replay=12,MusicRoom=14,Records=15,ReplaySave=16,Manual=17,ContinuePrompt=21};
struct TitleSelectionSettings {i32 preferred_mode=0,configured_difficulty=1,saved_difficulty=0,preferred_character=0,preferred_stage=0;};
struct TitleState {
    MenuCursor menu;TitleScreen screen=TitleScreen::Main,previous_screen=TitleScreen::Initialize;
    i32 substate=0;Timer age{0,0,0,0,0};u32 flags=2;
    std::array<u32,248> handles{};u32 portrait=0;i32 return_reason=1;u32 start_effect=0;i32 practice_chapter=0;
    TitleState(){age.set(0);}
    void change_screen(TitleScreen screen)noexcept{previous_screen=this->screen;this->screen=screen;substate=0;age.set(0);}
    void change_substate(i32 value)noexcept{substate=value;age.set(0);}
};
}
