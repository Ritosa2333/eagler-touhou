#include "PlayerGraze.hpp"
namespace th08 {
void graze_player(PlayerGrazeContext& context,GameGlobals& values,GameGauge& gauge,GameRank& rank,const Vec3& bullet,bool laser,PlayerGrazeActions& actions){
    if(!context.bomb){const i32 amount=gauge.human_bonus()?3:gauge.human()?2:1;if(values.graze_stage<99999)values.graze_stage=wrapping_add(values.graze_stage,amount);if(values.graze<999999)values.graze=wrapping_add(values.graze,amount);}
    const auto midpoint=[](float a,float b){return (number(Scalar::add(a,b))/number(2)).to_float();};
    actions.effect(8,{midpoint(context.position.x,bullet.x),midpoint(context.position.y,bullet.y),midpoint(context.position.z,bullet.z)},1,0xffffffff);
    rank.add(6);context.hud_flags=(context.hud_flags&~0xc0u)|0x80;actions.sound(30,bullet.x);values.score+=gauge.youkai()?400:200;
    if(context.youkai)gauge.add(100,context.bomb,false);
    const bool human_solo=context.character>=4&&!(context.character&1),youkai_solo=context.character>=4&&(context.character&1);
    if((!human_solo||context.character==10)&&context.boss_present&&gauge.youkai_bonus()){
        actions.item(10,bullet,1);if(!laser&&context.time_spell){actions.item(10,bullet,1);if(!youkai_solo)actions.item(10,bullet,1);}
    }
    context.replay_flags|=0x2000;
}
}
