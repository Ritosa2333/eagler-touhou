#pragma once
#include "Types.hpp"
#include <array>
#include <vector>
#include <algorithm>
namespace th19 {
// SHT v6 header thresholds (0x2c..0x38) are tenths of the native
// PlayerStat gauge units. Native 0x4e8a20 clamps each to 0..2500.
struct ChargeGauge {
    std::array<i32,4> thresholds{500,1000,1700,2500};
    i32 energy=500, charge=0, ex_level=0, boss_level=0, last_release=0;
    bool held=false;
    int cooldown=0,charge_speed_percent=100;
    float move_speed=4.5f, slow_speed=2.f;
    float diagonal_speed=3.1819805f, slow_diagonal_speed=1.4142136f;
    bool load(const std::vector<u8>& bytes){
        if(bytes.size()<0x3c)return false;
        auto integer=[&](size_t at){i32 n;std::memcpy(&n,bytes.data()+at,4);return n;};
        if((integer(0)&65535)!=6)return false;
        for(int i=0;i<4;++i){int n=integer(0x2c+i*4);if(n<0||n>250)return false;}
        for(int i=0;i<4;++i)thresholds[i]=integer(0x2c+i*4)*10;
        std::memcpy(&move_speed,bytes.data()+0x10,4);std::memcpy(&slow_speed,bytes.data()+0x14,4);
        std::memcpy(&diagonal_speed,bytes.data()+0x18,4);std::memcpy(&slow_diagonal_speed,bytes.data()+0x1c,4);return true;
    }
    int level() const {int n=0;for(int v:thresholds)if(charge>=v)++n;return n;}
    void tick(bool down,bool attack_busy=false){
        energy=std::clamp(energy,0,2500);
        // 0x4e79e0 gates on the attack controller's +8 virtual method.
        // Release sets the native timer to -10, then advances it that tick.
        if(cooldown>0||attack_busy){if(cooldown>0)--cooldown;charge=0;held=false;return;}
        if(down)charge=std::min(energy,charge+10*std::clamp(charge_speed_percent,0,200)/100); // 4e79e0, PlayerStat+88.
        else {if(held){last_release=level();cooldown=9;}charge=0;}
        held=down;
    }
    // 0x4e811d / 0x40b3e0: higher releases retain the C1 threshold.
    void spend_attack(int tier){
        if(tier!=3&&tier!=4)return;
        energy=std::clamp(energy+thresholds[0]-thresholds[tier-1],0,2500);
        int& rank=tier==3?ex_level:boss_level;rank=std::clamp(rank+1,0,7);
    }
    static float marker(i32 value){return -224.f+448.f*std::clamp(value,0,2500)/2500.f;}
};
}
