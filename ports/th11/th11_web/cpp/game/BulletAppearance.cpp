#include "BulletAppearance.hpp"
namespace th11 {
const std::array<i32,8> bullet_cancel_scripts={2,6,10,14,18,22,26,32};
namespace {
constexpr std::array<BulletAppearance,29> appearances(){
    constexpr i32 scripts[]={0,34,35,36,37,38,39,40,41,42,43,44,63,64,65,66,67,72,95,68,69,70,71,45,33,96,46,191,193};
    constexpr float hitboxes[]={4,6,6,4,4,4,4,4,4,0,4,6,10,8,8,8,8,14,6,10,8,8,8,6,4,6,4,10,14};
    constexpr i32 groups[]={5,3,3,4,4,4,3,4,4,4,4,3,1,2,2,2,2,0,2,2,2,1,2,3,5,2,4,3,0};
    constexpr i32 effects[]={0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,3,1,1,1,1,0,0,4,0,1,2};
    constexpr i32 small[]={211,212,212,212,212,213,213,213,213,214,214,214,215,215,215,211};
    constexpr i32 normal[]={211,212,212,213,213,214,214,215,215,216,216,216,217,217,217,218};
    std::array<BulletAppearance,29> result{};
    for(i32 type=0;type<29;++type){auto& a=result[type];a.script=scripts[type];a.hitbox=hitboxes[type];a.group=groups[type];a.effect_mode=effects[type];
        if(type==18||type==25){for(auto& v:a.sprites)v=-1;}
        else if(type==17||type==28){for(i32 c=0;c<4;++c)a.sprites[c*3]=(type==17?296:385)+c;}
        else if(type==23){constexpr i32 s[]={208,217,228,209,218,224,210,212,225};for(i32 n=0;n<9;++n)a.sprites[n]=s[n];}
        else if(type>=12&&type<=16||type>=19&&type<=22||type==27){
            const i32 base=type==27?377:256+8*(type>=19?type-19:type-12);
            for(i32 c=0;c<8;++c){a.sprites[c*3]=base+c;a.sprites[c*3+1]=211+c;}
        }else {const i32 base=type==0?0:type==24?16:type==26?64:32+16*(type-1);
            for(i32 c=0;c<16;++c){a.sprites[c*3]=base+c;a.sprites[c*3+1]=(type==0||type==24?small:normal)[c];a.sprites[c*3+2]=224+c;}
        }
    }return result;
}
}
const std::array<BulletAppearance,29> bullet_appearances=appearances();
}
