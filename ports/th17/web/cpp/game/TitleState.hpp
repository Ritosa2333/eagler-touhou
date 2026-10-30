#pragma once
#include "MenuCursor.hpp"
#include "Timer.hpp"
#include <array>
namespace th17 {
enum class TitleScreen:i32 {Initialize=0,Main=1,Quit=2,Options=3,Controller=4,Difficulty=5,Character=6,Animal=7,Stage=8,StartGame=9,PlayerData=10,PlayerDataDetails=11,Replay=12,ReplaySave=13,MusicRoom=14,Ranking=15,RankingName=16,Manual=17,SpellPractice=18,SpellGroup=19,SpellRank=20,SpellExtraRank=21,Trophies=23};
struct GameSelection {
    u32 mode_flags=0;i32 spell_id=-1,difficulty=1,character=0,animal=0,stage=1,practice_checkpoint=0;
    void mode(i32 value)noexcept{if((mode_flags&0x30)!=0x20)spell_id=-1;mode_flags=(mode_flags&~0x30u)|(u32(value)<<4&0x30);}
};
struct TitleSelectionSettings {i32 configured_difficulty=1,last_spell_character=0,preferred_animal=0,practice_stage=0;};
struct TitleState {
    MenuCursor menu,spell_character;TitleScreen screen=TitleScreen::Main,previous_screen=TitleScreen::Initialize;
    i32 substate=0;Timer age{};u32 flags=2;i32 return_reason=0;
    std::array<u32,335> handles{};u32 portrait=0;
    TitleState(){age.set(0);}
    void change_screen(TitleScreen value)noexcept{previous_screen=screen;screen=value;substate=0;age.set(0);}
    void change_substate(i32 value)noexcept{substate=value;age.set(0);}
};
}
