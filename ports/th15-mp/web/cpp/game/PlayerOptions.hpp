#pragma once
#include "PlayerAnmHost.hpp"
namespace th15 {
class PlayerOptions {
    const ShtResource& shots;PlayerMotion& motion;PlayerAnmHost& visuals;AnmManager& animations;i32 resource,character;
    PlayerFixedPosition offset(i32 index,bool focus)const noexcept;
public:
    i32 power_level=0,auxiliary=0;std::string error;
    PlayerOptions(const ShtResource& s,PlayerMotion& m,PlayerAnmHost& v,AnmManager& a,i32 id,i32 ch):shots(s),motion(m),visuals(v),animations(a),resource(id),character(ch){}
    bool configure(i32 power,i32 power_step,i32 max_power);
};
}
