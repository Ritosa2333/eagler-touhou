#pragma once
#include "PlayerAnmHost.hpp"
namespace th17 {
class PlayerOptions {
    const ShtResource& shots;PlayerMotion& motion;PlayerAnmHost& visuals;AnmManager& animations;i32 resource,character;
    PlayerFixedPosition offset(i32 index,bool focused)const noexcept;
    bool follow_history(PlayerOption&);
public:
    i32 power_level=0;std::string error;
    PlayerOptions(const ShtResource& s,PlayerMotion& m,PlayerAnmHost& v,AnmManager& a,i32 id,i32 ch):shots(s),motion(m),visuals(v),animations(a),resource(id),character(ch){}
    ~PlayerOptions(){for(u32 i=0;i<4;i++)motion.options[i].movement_callback={};}
    bool configure(i32 power,i32 power_step,i32 max_power);
};
}
