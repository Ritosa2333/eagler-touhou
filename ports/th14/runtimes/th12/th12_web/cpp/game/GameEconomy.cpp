#include "GameEconomy.hpp"
#include "PortTitle.hpp"
#include "ScoreScalarTh14.hpp"
#include "GrazeWordsTh14.hpp"
#include "PowerPointWordsTh14.hpp"
#include <algorithm>
#include <cmath>
namespace th11 {
void GameEconomy::add_score(i32 amount) noexcept {
#if PORT_TITLE==14
    th14_add_score_words(amount,[this](){return th14_hud_load(score_units);},
        [this](i32 value){th14_hud_store(score_units,value);});
#else
    const i32 sum=wrapping_add(score_units,amount/10);
    score_units=kPortTitle==14?signed_bits(std::min(u32(sum),999999999u)):std::min(sum,999999999);
#endif
}
void GameEconomy::add_point_value(i32 amount) noexcept {
#if PORT_TITLE==14
    th14_add_point_words(amount,[&](PointAddAccessTh14 access){
        return access==PointAddAccessTh14::Point?th14_hud_load(point_value):th14_hud_load(max_point_value);
    },[&](PointAddAccessTh14,i32 value){th14_hud_store(point_value,value);});
#else
    point_value=std::min(wrapping_add(point_value,amount),max_point_value);
#endif
}
void GameEconomy::add_rank(i32 amount) noexcept {rank=std::max(-1024,std::min(1024,wrapping_add(rank,amount)));}
#if PORT_TITLE == 14
Vec3 th14_graze(GameEconomy& e,Vec3 player,Vec3 contact)noexcept{
    th14_graze_words([&](GrazeAccessTh14 access){
        switch(access){
        case GrazeAccessTh14::GrazeRead:return th14_hud_load(e.graze);
        case GrazeAccessTh14::PointRead:return th14_hud_load(e.point_value);
        default:return th14_hud_load(e.max_point_value);
        }
    },[&](GrazeAccessTh14 access,i32 value){
        if(access==GrazeAccessTh14::GrazeWrite)th14_hud_store(e.graze,value);
        else th14_hud_store(e.point_value,value);
    });
    return {(player.x+contact.x)*.5f,(player.y+contact.y)*.5f,0};
}
#endif
#if PORT_TITLE == 16
bool player_th16_graze(GameEconomy& e,Vec3 player,Vec3 contact,Th16GrazeServices& world){
    // Complete444cf0. Both signed counters wrap before their upper clamp.
    e.graze=std::min(wrapping_add(e.graze,1),99999999);
    e.th16_graze_chain=std::min(wrapping_add(e.th16_graze_chain,1),99999999);
    const float x=contact.x+player.x,y=player.y+contact.y;
    const Vec3 midpoint{x*.5f,y*.5f,0};
    if(!world.effect(midpoint,24)||!world.popup(midpoint,e.th16_graze_chain,0xffc0c0ff)||!world.sound(42,contact.x))return false;
    const float dx=contact.x-player.x,dy=contact.y-player.y;
    const float angle=float(std::atan2(double(dy),double(dx)));
    return world.item(16,contact,angle,1.9f,0);
}
#endif
i32 GameEconomy::point_item_score() const noexcept {
    const i32 percent=communication>=10000?100:signed_bits(u32(communication)*100)/10000;
    const i32 graze_factor=std::min(graze/100,899),value=point_value/100-(point_value/100)%10;
    const i32 product=signed_bits(u32(wrapping_add(percent,graze_factor))*u32(value));
    const i32 result=signed_bits(u32(product/1000)*10);
    return communication<10000&&result<=0?10:result;
}
}
