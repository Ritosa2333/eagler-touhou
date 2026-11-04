#pragma once
#include "Types.hpp"
#include <algorithm>
#include <cstdio>
#include <string>
namespace th19 {
// Original 0x50db70 / 0x50d980. Text uses 640x480 native coordinates;
// aura ANM uses the same coordinates, portrait ANM supplies its own scale.
struct AttackWarning {
    int sender=0,tier=0,remaining=-1,extra_level=0,boss_level=0;
    int freeze_frames=0; // 0x50db70 writes 40; 0x516450 skips simulation while --timer > 0.
    static constexpr int sound=77;
    bool start(int side,int charge,int ex,int boss){
        if(side<0||side>1||(charge!=3&&charge!=4))return false;
        sender=side;tier=charge;remaining=60;freeze_frames=40;
        extra_level=std::clamp(ex,0,7);boss_level=std::clamp(boss,0,7);return true;
    }
    bool advance_freeze(){if(freeze_frames<=0)return false;return --freeze_frames>0;}
    bool visible()const{return remaining>=0;}
    void after_draw(){if(visible())--remaining;}
    int aura_script()const{return 17+sender+(tier==4?2:0);}
    float x()const{return 148.f+(sender==0?328.f:16.f);}
    static constexpr float title_y=256,detail_y=288;
    u32 shadow_color()const{
        const bool alternate=(remaining%4)<2;
        return tier==3?(alternate?0xff404000u:0xff808040u):(alternate?0xffd00000u:0xff804040u);
    }
    std::string detail(int difficulty)const{
        const char* type=tier==3?"Extra":"Boss";char out[64];
        if(sender==0||std::clamp(difficulty,0,3)==2)
            std::snprintf(out,sizeof out,"%s Attack Level %d",type,(tier==3?extra_level:boss_level)+1);
        else std::snprintf(out,sizeof out,"%s Attack !!",type);
        return out;
    }
};
}
