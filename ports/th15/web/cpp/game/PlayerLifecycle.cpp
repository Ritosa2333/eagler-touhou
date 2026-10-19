#include "PlayerLife.hpp"
#include <cmath>
namespace th15 {
bool PlayerLife::advance_state(u32 pressed,float& rate,bool hud_available,PlayerLifecycleHost& host){
    const auto bomb=[&](){if(!(pressed&2)&&!(state==4&&cheat(PracticeAutoBomb)))return 0;if(session.bombs<=0||!host.allow_bomb())return 0;return host.begin_bomb()?1:-1;};
    switch(state){
    case 0:{const i32 y=wrapping_sub(0xf000,wrapping_mul(age.current,0x2800)/60);motion.position_fixed.y=y;motion.position.y=float(float(y)*.0078125f);motion.snap_options();
        if(age.current<30){const float radius=float(float(float(float(age.current)*512.f)/30.f)+64.f);if(!host.cancel_lasers_near(last_death_position,radius,0,true)||!host.cancel_lasers_near(last_death_position,float(radius*.25f),0,false))return false;}
        else if(!host.cancel_bullets_near(motion.position,640.f,0)||!host.cancel_lasers(0,true))return false;
        if(age.current<60)return true;state=1;age.set(0);[[fallthrough]];}
    case 1:if(bomb()<0)return false;return host.move();
    case 4:if(age.current<8){const i32 result=bomb();if(result<0)return false;if(result)cancel_death();return true;}if(!commit_death())return false;[[fallthrough]];
    case 2:
        if(!(session.mode_flags&0x300)&&age.current==3){if(!cheat(PracticePower)){session.power=wrapping_sub(session.power,session.power_step/2);if(session.power<session.power_step)session.power=session.power_step;}
            const float dy=float(float(motion.position.y-224.f)-motion.position.y),dx=float(-motion.position.x);const float angle=dy==0&&dx==0?0.f:float(std::atan2(double(dy),double(dx)));
            for(i32 i=0;i<7;i++){const float direction=float(float(float(float(float(i)*3.1415927f)/28.f)+angle)-.3926991f);if(!host.item(1,motion.position,direction,3.f))return false;}if(!host.options_changed())return false;}
        if(age.current<=29)return true;
        if(session.mode_flags&0x300)return host.game_over();
        if(session.extra_lives<0&&age.current==30){if(session.replay_state!=1&&!host.game_over())return false;age.tick(&rate);return true;}
        state=0;rate=1;if(!host.respawn_damage(motion.position,32.f,16.f,30,150))return false;session.bombs=3;if(hud_available&&!host.bomb_hud(3,session.bomb_pieces))return false;
        last_death_position=motion.position;motion.position_fixed={0,61440};motion.position.x=0;motion.position.y=480;motion.snap_options();invulnerability.set(280);age.set(0);return true;
    case 3:if(age.current==15)return host.cancel_lasers(1,false);return true;
    default:return true;
    }
}
}
