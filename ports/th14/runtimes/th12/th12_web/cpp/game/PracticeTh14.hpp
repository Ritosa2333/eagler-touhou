#pragma once
#include "Types.hpp"
#include <string>
namespace th11 {
#if PORT_TITLE == 14
class EclProgram;
struct GameEconomy;
enum PracticeToolTh14 : u32 {
    PrInvincible=1, PrLives=2, PrBombs=4, PrPower=8, PrTime=16, PrAutoBomb=32,
    PrEnemy=64, PrAutoShoot=128, PrNoShoot=256, PrNoBomb=512, PrForceFocus=1024,
    PrNoFocus=2048, PrLivesNoContinue=4096, PrMarisaFix=8192, PrBossDown=16384, PrAllClear=32768,
    PrBossRange=0x01ff0000,
};
inline constexpr u32 practice_mask_th14=0x01ffffff;
struct PracticeTh14 {
    i32 stage=0,section=0,phase=0,flags=0,life=8,life_fragment=0,bomb=8,bomb_fragment=0,
        power=400,value=10000,graze=0,score=0,cycle=0,angle=0;
    bool valid(i32 difficulty,i32 spell=-1)const noexcept;
    explicit operator bool()const noexcept{return stage!=0;}
    void apply(GameEconomy&)const noexcept;
    i32 bgm()const noexcept;
};
bool patch_practice_th14(EclProgram&,const PracticeTh14&,i32 spell,std::string&);
struct PracticeEventTh14 {u32 frame,tools;};
#endif
}
