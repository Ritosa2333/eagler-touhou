#pragma once
#include "Timer.hpp"
namespace th17 {
struct StageDrawState {
    u32 flags=1,tint_color=0x00ffffff;Timer transition{0,0,0,0,0};
    void depart()noexcept{flags|=2;transition.set(30);}
    void arrive()noexcept{flags|=4;transition.set(60);}
};
}
