#include "PlayerLife.hpp"
#include <algorithm>
#include <cmath>
namespace th17 {
bool PlayerLife::advance_state(u32 pressed,float& rate,bool hud_available,PlayerLifecycleHost& host){
    switch(state){
    case 0:{
        const i32 y=wrapping_sub(0xf000,wrapping_mul(age.current,0x2800)/60);motion.position_fixed.y=y;motion.position.y=float(float(y)*.0078125f);snap();
        for(auto& p:motion.history)p=motion.position_fixed;
        if(age.current<30){const float radius=float(float(float(float(age.current)*512.f)/30.f)+64.f);if(!host.cancel_lasers_near(last_death_position,radius,0,true)||!host.cancel_lasers_near(last_death_position,float(radius*.25f),0,false))return false;}
        else{if((spell.flags&1)&&spell.id==100&&!host.cancel_bullets_near(motion.position,128.f,0,true))return false;if(!host.cancel_bullets_near(motion.position,640.f,0,false)||!host.cancel_lasers_near(motion.position,640.f,0,false))return false;}
        if(age.current<60)return true;state=1;age.set(0);[[fallthrough]];
    }
    case 1:
        if(session.bomb_available&&session.bombs>0&&host.allow_bomb()&&(pressed&2)&&!host.begin_bomb())return false;
        return host.move();
    case 4:
        if(age.current<8){
            if((session.roaring_flags&2)&&!(session.roaring_flags&4)){if(!host.end_roaring(true))return false;cancel_death();}
            if(session.bomb_available&&(pressed&2)&&session.bombs>0&&host.allow_bomb()){if(!host.begin_bomb())return false;cancel_death();}return true;
        }
        if(!commit_death())return false;[[fallthrough]];
    case 2:
        if(age.current==3){
            session.power=wrapping_sub(session.power,session.power_step/2);if(session.power<session.power_step)session.power=session.power_step;
            const float dy=float(float(motion.position.y-224.f)-motion.position.y),dx=float(-motion.position.x),angle=dy==0&&dx==0?1.5707964f:float(std::atan2(double(dy),double(dx)));
            for(i32 i=0;i<7;i++){const float direction=float(float(float(float(float(i)*3.1415927f)/28.f)+angle)-.3926991f);if(!host.item(1,motion.position,direction,3.f))return false;}
            Vec3 p=motion.position;p.x=float(p.x-16);p.y=float(p.y-96);if(!host.item(session.animal+16,p,-1.5707964f,2.2f))return false;p.x=float(p.x+32);if(!host.item(session.animal+16,p,-1.5707964f,2.2f)||!host.options_changed())return false;
        }
        if(age.current<30)return true;
        if(session.extra_lives<0&&age.current==30){if(session.replay_state!=1&&!host.game_over())return false;age.tick(&rate);return true;}
        state=0;rate=1;if(!host.respawn_damage(motion.position,32.f,16.f,30,150))return false;session.bombs=std::clamp(session.initial_bombs,0,8);if(hud_available&&(!bomb_hud||!bomb_hud(session.bombs,session.bomb_pieces)))return false;
        last_death_position=motion.position;motion.position_fixed={0,61440};motion.position.x=0;motion.position.y=480;snap();invulnerability.set(280);age.set(0);return true;
    case 3:if(age.current==15)return host.cancel_lasers(1,false);return true;
    default:return true;
    }
}
}
