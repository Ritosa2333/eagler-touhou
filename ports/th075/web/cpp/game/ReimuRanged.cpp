#include "ReimuRanged.hpp"
#include "FighterActionTools.hpp"
namespace th075 {
using namespace action;
namespace {
void fan(FighterActor& a,FighterActionEvents& events,i16 id,float initial,float step,i32 count,i32 dx,float dy){for(i32 i=0;i<count;i++){a.launchParameters[0]=initial+float(i)*step;a.launchParameters[1]=10;emit(a,events,FighterEffectSpace::Owned,id,x(a,dx),y(a,dy));}}
void shot(FighterActor& a,ReplayStreams& rng,FighterActionEvents& events,i16 id,float center,i32 spread,float speed,i32 dx,float dy,i16 cost){a.launchParameters[1]=speed;a.launchParameters[0]=float(i32(center)-rng.range(0,spread));emit(a,events,FighterEffectSpace::Owned,id,x(a,dx),y(a,dy));events.sound(0);consume_reimu_spirit(a,cost,events);}
}
bool advance_reimu_ranged(FighterActor& a,const FighterActor& opponent,ReimuRecoveryState& s,ReplayStreams& rng,FighterActionEvents& events){
    const i16 id=a.animation.cursor.action;
    if(id==403){if(a.combat.freeze==0&&opponent.combat.opponentStop==0&&end(a))a.select_action(0);return true;}
    const bool fanShot=id==400||id==401||id==404||id==405;
    const bool burst=id==406||id==407||id==410||id==411||id==412||id==416;
    const bool crouch=id==402||id==408||id==414;
    if(!fanShot&&!burst&&!crouch&&id!=413&&id!=417)return false;
    if(a.combat.freeze!=0||opponent.combat.opponentStop!=0)return true;
    if(fanShot){
        const bool air=id==404||id==405;if(start(a)){a.response=1;s.loops=0;if(!air)a.motion.clear();}
        if(air)a.motion.velocityY-=a.motion.accelerationY;
        if(end(a))a.select_action(air?9:0);
        const i16 trigger=air?4:8,repeat=air?6:10;
        const i32 dx=id==400?114:id==401?90:id==404?83:67;const float dy=id==400?96:id==401?158:id==404?117:75;
        const float first=id==400||id==404?-20:id==401?60:-30,step=id==400||id==404?10:-10;
        if(at(a,trigger)){if(air){a.motion.velocityX=-3;a.motion.velocityY=8;a.motion.accelerationY=.55f;}fan(a,events,811,first,step,5,dx,dy);events.sound(0);consume_reimu_spirit(a,100,events);}
        if(at(a,repeat)&&s.loops<=1&&a.controls.current.buttons[2]!=0){fan(a,events,811,first,step,5,dx,dy);events.sound(0);consume_reimu_spirit(a,25,events);s.loops=add(s.loops,1);frame(a,trigger);}return true;
    }
    if(crouch){
        if(start(a)){a.response=1;a.motion.clear();s.loops=0;}if(end(a))a.select_action(2);
        if(at(a,3)){a.launchParameters[0]=40;a.launchParameters[1]=10;emit(a,events,FighterEffectSpace::Owned,id==402?824:id==408?826:825,x(a,0),y(a,0),0);events.sound(4);consume_reimu_spirit(a,250,events);}
        if(at(a,6)&&s.loops<=1){s.loops=add(s.loops,1);frame(a,4);}return true;
    }
    if(burst){
        const bool air=id==410||id==411||id==416;
        if(start(a)){if(air){a.motion.velocityX=float(double(a.motion.velocityX)*.1f);a.motion.velocityY=float(double(a.motion.velocityY)*.1f);}else a.motion.clear();a.response=1;s.loops=0;}
        if(air){if(a.animation.cursor.index>5)a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0){a.select_action(10);a.motion.y=a.motion.accelerationY=a.motion.velocityY=0;return true;}}
        if(end(a))a.select_action(air?9:0);
        const i16 trigger=air?4:5,repeat=air?6:7;const bool heavy=id==412||id==416;
        const float center=id==407?22:id==411?-28:2;const i32 dx=id==407?90:id==411?67:air?83:114;const float dy=id==407?158:id==411?75:air?117:96;
        if(at(a,trigger))shot(a,rng,events,heavy?834:827,heavy?5:center,heavy?10:4,id==412?10:25,dx,dy,heavy?30:20);
        if(at(a,trigger+1))shot(a,rng,events,heavy?834:827,heavy&&id==412?5:center,heavy&&id==412?10:4,id==412?10:25,dx,dy,heavy&&id==416?30:20);
        if(at(a,repeat)&&s.loops<=(heavy?2:3)&&a.controls.current.buttons[heavy?1:0]!=0){shot(a,rng,events,heavy?834:827,heavy&&id==412?5:center,heavy&&id==412?10:4,id==412?10:25,dx,dy,id==412?30:20);s.loops=add(s.loops,1);frame(a,trigger);}return true;
    }
    if(id==413){if(start(a)){a.response=1;a.motion.clear();}if(at(a,6)){a.launchParameters[0]=-30;a.launchParameters[1]=8;emit(a,events,FighterEffectSpace::Owned,812,x(a,37),y(a,117));events.sound(1);consume_reimu_spirit(a,200,events);}if(end(a))a.select_action(0);return true;}
    if(double(a.motion.y)+a.motion.velocityY<=0){a.select_action(10);a.motion.y=a.motion.accelerationY=a.motion.velocityY=0;return true;}
    if(start(a)){a.motion.velocityX=float(double(a.motion.velocityX)*.1f);a.motion.velocityY=float(double(a.motion.velocityY)*.1f);a.response=1;}
    if(end(a))a.select_action(9);if(at(a,3)){a.launchParameters[0]=30;a.launchParameters[1]=8;emit(a,events,FighterEffectSpace::Owned,812,x(a,12),y(a,120));events.sound(1);consume_reimu_spirit(a,200,events);}return true;
}
bool advance_reimu_special_melee(FighterActor& a,const FighterActor& opponent,ReimuRecoveryState& s,FighterActionEvents& events){
    const i16 id=a.animation.cursor.action;if(id==505){if(a.combat.freeze==0&&opponent.combat.opponentStop==0&&end(a))a.select_action(0);return true;}
    if(id<500||id>507)return false;if(a.combat.freeze!=0||opponent.combat.opponentStop!=0)return true;
    if(id<=504){
        const bool shift=id==503||id==504;const bool forward=id==501||id==504;const i16 shrinkFrame=shift?2:1,growFrame=shift?4:3;
        if(start(a)){a.motion.clear();a.remainingHits=1;a.response=0;a.carryMomentum=0;if(forward)s.loops=0;}
        if(at(a,shrinkFrame)){a.begin_pose(0,100,0);a.pose.scale[0]=a.pose.scale[1]=1;a.pose.color=0xffffffff;events.sound(2);}
        if(a.animation.cursor.index==shrinkFrame){a.pose.scale[0]-=.1f;a.pose.scale[1]+=.2f;a.pose.color=(a.pose.color&0x00ffffff)|(u32(u8((a.pose.color>>24)-25))<<24);}
        if(a.animation.cursor.index==growFrame){if(a.pose.scale[0]>=1){a.pose.scale[0]=a.pose.scale[1]=1;a.pose.color|=0xff000000;}else{a.pose.scale[0]+=.1f;a.pose.scale[1]-=.2f;a.pose.color=(a.pose.color&0x00ffffff)|(u32(u8((a.pose.color>>24)+25))<<24);}}
        const i16 groundFrame=forward?(shift?7:6):shift?10:9;
        if(forward){if(a.motion.y>0&&a.animation.cursor.index>growFrame-1){a.motion.velocityY-=1;if(double(a.motion.y)+a.motion.velocityY<=0&&a.animation.cursor.index<groundFrame){a.motion.y=a.motion.velocityY=0;frame(a,groundFrame);}}if(a.animation.cursor.index>groundFrame+1){a.motion.velocityX-=.8f;if(a.motion.velocityX<0)a.motion.velocityX=0;}}
        else{if(a.motion.y>0&&a.animation.cursor.index<groundFrame-2&&(!shift||a.animation.cursor.index>3))a.motion.velocityY-=.7f;if((a.animation.cursor.index==groundFrame-2||a.animation.cursor.index==groundFrame-1)&&double(a.motion.y)+a.motion.velocityY<0){a.motion.y=a.motion.velocityX=a.motion.velocityY=0;frame(a,groundFrame);return true;}}
        if(end(a))a.select_action(0);
        if(at(a,growFrame)){a.motion.x=forward?x(a,100):float(double(opponent.motion.x)-i32(a.motion.facing)*180);a.motion.y=forward?50:150;a.motion.velocityX=forward?12:6;a.motion.velocityY=forward?6:8;events.sound(2);}
        if(!forward&&at(a,groundFrame-3)){a.motion.velocityX=5;a.motion.velocityY=-16;}
        if(at(a,groundFrame)){if(!forward)frame(a,groundFrame-2);}
        if(forward&&at(a,groundFrame+2)&&s.loops<=0){frame(a,groundFrame);s.loops=add(s.loops,1);}return true;
    }
    if(start(a)){clear_flight(a);a.remainingHits=1;a.response=0;}
    if(a.motion.y>0){a.motion.velocityY-=a.motion.accelerationY;a.motion.velocityX-=.3f;if(a.motion.velocityX<0)a.motion.velocityX=0;if(double(a.motion.velocityY)+a.motion.y<=0){clear_flight(a);a.motion.y=0;frame(a,10);return true;}}
    if(end(a))a.select_action(0);if(at(a,4)){emit(a,events,FighterEffectSpace::Owned,813,a.motion.x,a.motion.y);events.sound(3);}if(at(a,5)){a.motion.velocityX=id==506?7.5f:9.5f;a.motion.velocityY=id==506?8.5f:12.5f;a.motion.accelerationY=.9f;emit(a,events,FighterEffectSpace::Shared,127,a.motion.x,a.motion.y);}if(at(a,10))frame(a,8);return true;
}
}
