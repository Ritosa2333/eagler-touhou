#include "ReimuAttacks.hpp"
#include "FighterActionTools.hpp"
namespace th075 {
using namespace action;
bool advance_reimu_melee(FighterActor& a,FighterActor& opponent,ReimuRecoveryState& s,FighterActionEvents& events){
    const i16 id=a.animation.cursor.action;
    const bool ordinary=id>=300&&id<=310;
    const bool supported=ordinary||(id>=390&&id<=393)||(id>=594&&id<=596);
    if(!supported)return false;if(a.combat.freeze!=0||opponent.combat.opponentStop!=0)return true;
    if(id==300||id==301||id==302||id==303||id==304||id==308||id==310||id==390){
        if(id==310&&start(a))a.carryMomentum=0;
        attack_start(a,id==303?6:1);carry_brake(a);
        if(id==302||id==310||id==390){if(a.animation.cursor.index>(id==302?2:3)&&a.motion.velocityX>0){a.motion.velocityX-=id==310?1.5f:1.0f;if(a.motion.velocityX<0)a.motion.velocityX=0;}}
        if(id==304){if(a.motion.y>0)a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0&&a.animation.cursor.index>5&&a.animation.cursor.index<12){frame(a,12);a.motion.y=0;clear_flight(a);return true;}}
        if(end(a)){a.select_action(id==303?2:0);if(id==301)s.loops=0;}
        if(id==300||id==301){if(at(a,id==300?3:2))events.system_sound(id==300?27:28);}
        else if(id==302){if(at(a,3)){a.motion.velocityX=12;if(u32(a.controls.current.buttons[0])>=10){a.select_action(310);a.motion.velocityX=0;return true;}}if(at(a,5)){events.shake(2);emit(a,events,FighterEffectSpace::Owned,805,x(a,170),y(a,-6),0);}}
        else if(id==303){if(a.animation.cursor.elapsed==0&&a.animation.cursor.index<12)a.response=0;for(i16 f:{3,5,7,9})if(at(a,f))events.system_sound(27);}
        else if(id==304){if(at(a,5)){a.motion.velocityX=5;a.motion.velocityY=12.5f;a.motion.accelerationY=1;a.carryMomentum=0;events.system_sound(28);}if(at(a,12))frame(a,11);}
        else if(id==308){if(at(a,4))events.system_sound(28);}
        else if(id==390||id==310){if(at(a,4)){a.motion.velocityX=15;events.system_sound(29);}}return true;
    }
    if(id==305||id==306){
        if(air_land(a))return true;if(start(a)){a.remainingHits=id==305?3:1;a.response=0;if(id==305)s.loops=0;}
        if(end(a))a.select_action(9);
        if(id==306){if(at(a,2))events.system_sound(28);return true;}
        if(at(a,1))events.system_sound(27);if(at(a,2))emit(a,events,FighterEffectSpace::Owned,808,x(a,145),y(a,66));if(at(a,4)&&s.loops<=1){s.loops=add(s.loops,1);frame(a,2);a.response=0;}return true;
    }
    if(id==307||id==391){
        if(id==391&&start(a))s.loops=0;
        if(id==391&&a.animation.cursor.index<5&&a.motion.y==0){a.motion.velocityX-=.75f;if(a.motion.velocityX<0){a.motion.velocityX=0;a.carryMomentum=0;}}
        if(start(a)){clear_flight(a);a.remainingHits=1;a.response=0;if(id==391)a.motion.velocityX=9;}
        if(a.motion.y>0){a.motion.velocityY-=a.motion.accelerationY;if(id==307)a.motion.velocityX-=.2f;if(a.motion.velocityX<0)a.motion.velocityX=0;if(double(a.motion.velocityY)+a.motion.y<=0){clear_flight(a);a.motion.y=0;frame(a,11);return true;}}
        if(end(a))a.select_action(0);if(at(a,5)){a.motion.velocityX=10;a.motion.velocityY=id==307?8:12.5f;a.motion.accelerationY=id==307?.8f:.85f;events.system_sound(28);}return true;
    }
    if(id==309){
        if(start(a)){a.motion.clear();s.loops=0;}if(a.animation.cursor.index>4&&a.motion.y==0){a.motion.velocityX-=.75f;if(a.motion.velocityX<0){a.motion.velocityX=0;a.carryMomentum=0;}}
        if(a.motion.y>0)a.motion.velocityY-=a.motion.accelerationY;
        if(double(a.motion.y)+a.motion.velocityY<=0&&a.motion.velocityY<0&&a.animation.cursor.index==2){frame(a,3);a.motion.y=a.motion.velocityY=a.motion.accelerationY=0;events.system_sound(29);return true;}
        if(end(a))a.select_action(2);if(at(a,2)){a.remainingHits=1;a.response=0;a.motion.velocityX=12;a.motion.velocityY=6;a.motion.accelerationY=.8f;}return true;
    }
    if(id==392){
        if(start(a)){if(a.carryMomentum==0)a.motion.clear();a.response=0;a.motion.velocityX=8;}
        if(a.animation.cursor.index>0){a.motion.velocityX-=.6f;if(a.motion.velocityX<0){a.motion.velocityX=0;a.carryMomentum=0;}}
        if(start(a)){a.remainingHits=8;a.response=0;}if(end(a))a.select_action(2);
        if(a.animation.cursor.elapsed==0&&a.animation.cursor.index<12)a.response=0;for(i16 f:{3,5,7,9})if(at(a,f))events.system_sound(27);return true;
    }
    if(id==393){
        if(start(a)){attack_start(a,1);a.motion.velocityX=12;a.carryMomentum=0;s.loops=0;}
        if(a.animation.cursor.index>4&&a.motion.y==0){a.motion.velocityX-=.75f;if(a.motion.velocityX<0){a.motion.velocityX=0;a.carryMomentum=0;}}
        if(end(a))a.select_action(2);if(at(a,2)){a.remainingHits=1;a.response=0;a.motion.velocityX=12;}if(at(a,3))events.system_sound(29);if(at(a,5)&&s.loops<=0){s.loops=add(s.loops,1);frame(a,3);}return true;
    }
    if(start(a)){a.selectedSpell=u8(a.selectedSpell-1);a.motion.clear();a.remainingHits=1;a.response=0;a.carryMomentum=0;s.loops=0;}
    if(id!=596&&a.combat.spiritBreak==0&&a.response==1)a.combat.spirit=1000;
    if(end(a))a.select_action(0);if(at(a,5)){emit(a,events,FighterEffectSpace::Shared,55,a.motion.x,y(a,100));events.system_sound(33);opponent.knockdownTime=0;}return true;
}
bool advance_reimu_shooting(FighterActor& a,const FighterActor& opponent,ReimuRecoveryState&,FighterActionEvents& events){
    const i16 id=a.animation.cursor.action;
    if(id<600||id>611)return false;if(a.combat.freeze!=0||opponent.combat.opponentStop!=0)return true;
    const bool air=id==602||id==603||id==606||id==607||id==610||id==611;
    if(start(a)){if(air){a.motion.velocityX=float(double(a.motion.velocityX)*.25);a.motion.velocityY=float(double(a.motion.velocityY)*.25);}else a.motion.clear();a.response=1;}
    const i16 trigger=air?3:(id==600||id==601?8:6);
    // Air fans and straight throws trigger after animation advance, ground
    // amulets before it. This timing affects both collisions and replay RNG.
    const bool post=air||id==600||id==601;
    if(post&&end(a))a.select_action(air?9:0);
    if(at(a,trigger)){
        if(id<=603){a.launchParameters[1]=y(a,air?120:96);for(i32 n=0;n<4;n++){a.launchParameters[0]=float(n);emit(a,events,FighterEffectSpace::Owned,(id==600||id==602)?801:802,x(a,air?12:114),y(a,air?120:96));}consume_reimu_spirit(a,250,events);events.sound(4);}
        else if(id<=607){a.launchParameters[1]=10;const bool wide=id==605||id==607;const float wideAngle=air?60:70;if(wide){for(float angle:{20.0f,40.0f,wideAngle,-20.0f,-40.0f,-wideAngle}){a.launchParameters[0]=angle;emit(a,events,FighterEffectSpace::Owned,800,x(a,air?12:30),y(a,air?120:130));}}else for(float angle:air?std::array<float,2>{-30,30}:std::array<float,2>{30,-30}){a.launchParameters[0]=angle;emit(a,events,FighterEffectSpace::Owned,800,x(a,air?12:30),y(a,air?120:130));}consume_reimu_spirit(a,wide?250:100,events);events.sound(5);}
        else{a.launchParameters[0]=id==609?-45:id==611?45:0;a.launchParameters[1]=8;emit(a,events,FighterEffectSpace::Owned,812,x(a,id==609?37:air?12:30),y(a,id==609?117:air?120:130));if(id!=608)events.system_sound(41);consume_reimu_spirit(a,200,events);}
    }
    if(!post&&end(a))a.select_action(0);return true;
}
}
