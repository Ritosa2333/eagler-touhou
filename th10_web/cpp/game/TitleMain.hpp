#pragma once
#include "TitleMenu.hpp"
#include "GameEconomy.hpp"
namespace th10 {
struct TitleMainEnvironment : TitleAnimationEnvironment {
    GameEconomy* game;
    const u8* extra_unlocked;
    const u16* pressed;
    const u16* repeated;
    virtual void sound(i32 sound)=0;
    virtual void interrupt_immediately(u32 animation,i32 label)=0;
};
}
