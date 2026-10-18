#pragma once
#include "Timer.hpp"
#include <functional>
#include <string>
namespace th15 {
class PlayerShooting {
    void wrap(Timer&,i32,float)noexcept;
public:
    Timer shot,continuous;
    i32 auxiliary=0;bool auxiliary_active=false;std::string error;
    std::function<bool(i32,i32)> fire;
    PlayerShooting(){shot.set(-1);continuous.set(-1);}
    bool update(i32 player_state,bool held,float rate);
};
}
