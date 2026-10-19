#include "ShotControl.hpp"
#include <algorithm>
namespace th09 {
void ShotControl::protect(i32 level,const Vec3& p){
    const i32 duration=level==0?48:level==1?64:128;const float speed=level==2?3.5f:4.f;
    areas.circle(p,0,speed,0,duration,0,AttackAreaKind::cancel);areas.circle(p,0,speed,2,duration,0,AttackAreaKind::special_damage);
    state.player_state=3;state.protection.reset(duration);actions.effect(13+level,p);
}
void ShotControl::release(const ShotResource& resource,const Vec3& p,const FrameTiming& timing){
    if(state.charge>=100){state.flags|=4;state.charged_fire.reset();state.shock.reset(8);
        i32 level=state.charge>=300?1:state.charge>=200?0:-1;
        if(state.charge>=400&&actions.opponent_boss_available())level=2;
        if(level>=0){actions.attack(level,resource.spell_names[level]);state.available=std::max(state.available-float((level+1)*100),1.f);state.shock.reset(8);protect(level,p);}
    }
    actions.end_charge();state.charging_time.reset(-1);state.charge=0;if(state.cooldown.current>0)state.cooldown.decrement(1,timing);
}
void ShotControl::update(const GameInput& input,bool automatic_focus,i32 side,const ShotResource& resource,const Vec3& position,const FrameTiming& timing){
    const bool focus=(input.held&4)!=0,held=(input.held&1)!=0,pressed=(input.pressed&1)!=0;
    const bool tap=(pressed&&!(automatic_focus&&focus))||(automatic_focus&&held&&!focus);
    if(tap){
        if(state.cooldown.current==0){
            if(state.normal_fire.current<0)state.normal_fire.reset();
            else if(state.normal_fire.current>=10)state.normal_fire.decrement(10,timing);
            else if(state.normal_fire.current>=5)state.normal_fire.decrement(5,timing);
            state.cooldown.reset(3);state.charging_time.reset(1);
        }else if(state.cooldown.current>0)state.cooldown.decrement(1,timing);
        if(automatic_focus)release(resource,position,timing);
    }else if((automatic_focus&&focus)||(!automatic_focus&&held)){
        state.charging_time.tick(timing);
        if(state.charging_time.current>=10&&!(state.flags&4)){
            if(state.charge==0){actions.play_sound(43,side?500:-500);actions.begin_charge();}
            const i32 previous=i32(state.charge);state.charge=timing.rate*resource.charge_speed+state.charge;
            bool automatic_release=false;
            if(state.charge<state.available)state.full_charge.reset();
            else {state.charge=state.available;state.full_charge.tick(timing);automatic_release=state.full_charge.current>=300;}
            if(automatic_release)release(resource,position,timing);
            else if(previous/100<i32(state.charge)/100)actions.play_sound(50,side?500:-500);
        }
    }else release(resource,position,timing);
    if(state.flags&4){
        if(state.charged_fire.current!=state.charged_fire.previous)actions.fire(1,state.charged_fire.current);
        state.charged_fire.tick(timing);if(state.charged_fire.value()>=resource.charge_duration){state.charged_fire.reset();state.flags&=~4u;}
    }
    if(state.normal_fire.current>=0){
        if(state.normal_fire.current!=state.normal_fire.previous)actions.fire(0,state.normal_fire.current);
        state.normal_fire.tick(timing);if(state.normal_fire.current>=15)state.normal_fire.reset(-1);
    }
}
bool ShotControl::bomb(const GameInput& input,bool scene_locked,const ShotResource& resource,const Vec3& p){
    if(scene_locked||state.player_state==4||!(input.held&2)||state.available<200)return false;
    return automatic_defence(resource,p);
}
bool ShotControl::automatic_defence(const ShotResource& resource,const Vec3& p){
    if(state.available<200)return false;
    i32 level=state.available>=300?1:0;float remaining=0;
    if(state.available>=400){if(actions.opponent_boss_available())level=2;else remaining=100;}
    actions.attack(level,resource.spell_names[level]);state.charge=state.available=remaining;protect(level,p);return true;
}
}
