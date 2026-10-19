#include "ReimuSpells.hpp"
#include "FighterActionTools.hpp"
namespace th075 {
using namespace action;
namespace {
void spell_start(FighterActor& a,ReimuRecoveryState& s,bool both=true){if(start(a)){a.response=1;s.loops=0;if(both)s.impactVelocity=0;a.motion.clear();}}
void aura(FighterActor& a,FighterActionEvents& events){events.effect(FighterEffectSpace::Owned,950,x(a,0),y(a,200),a.motion.facing,1,a.launchParameters,1);}
void declare(FighterActor& a,GameMode mode,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){a.combat.fade=240;a.fadeKind=0;a.begin_spell(mode,replayMode,core);events.system_sound(23);}
void cycle_orbs(FighterActor& a,ReimuRecoveryState& s,FighterActionEvents& events,i16 first,i16 second,i16 third,i16 sound){events.sound(sound);a.launchParameters[0]=float(s.loops);if(s.impactVelocity==0)emit(a,events,FighterEffectSpace::Owned,first,x(a,18),y(a,120));if(s.impactVelocity==1)emit(a,events,FighterEffectSpace::Owned,second,x(a,18),y(a,120));if(s.impactVelocity==2)emit(a,events,FighterEffectSpace::Owned,third,x(a,18),y(a,120));s.impactVelocity=add(s.impactVelocity,1);if(s.impactVelocity>2)s.impactVelocity=0;}
void rings(FighterActor& a,i8 difficulty,FighterActionEvents& events){if(difficulty<0||difficulty>3)return;const i32 count=difficulty<2?6:8;for(i32 i=0;i<count;i++){a.launchParameters[0]=float(i*(difficulty<2?60:45));events.effect(FighterEffectSpace::Owned,919,a.motion.x,y(a,100),1,1,a.launchParameters);}}
}
bool advance_reimu_spells(FighterActor& a,FighterActor& opponent,ReimuRecoveryState& s,GameMode mode,i8 difficulty,u8 replayMode,ReplayStreams& random,FighterCoreEvents& core,FighterActionEvents& events){
    const i16 id=a.animation.cursor.action;
    const bool spell=id>=700&&id<=707,story=id>=720&&id<=723;
    if(!spell&&!story&&id!=731&&id!=735&&id!=736&&id!=741&&id!=751&&id!=761&&id!=771&&id!=772&&id!=795&&id!=796)return false;
    if(a.combat.freeze!=0||opponent.combat.opponentStop!=0)return true;
    if(spell){
        spell_start(a,s);const bool enhanced=id==701||id==703||id==705||id==707;
        if(id==700||id==701||id==706||id==707){
            if(at(a,10)&&s.loops<8)cycle_orbs(a,s,events,enhanced?828:807,enhanced?829:809,enhanced?830:810,id==701?10:6);
            if(id>=706&&a.animation.cursor.index>11&&a.motion.y>0){a.motion.velocityY-=.5f;if(double(a.motion.y)+a.motion.velocityY<=0){frame(a,15);a.motion.y=0;a.motion.clear();return true;}}
            if(end(a))a.select_action(0);
            if(at(a,2)){a.motion.set_hit_freeze(40);events.system_sound(23);emit(a,events,FighterEffectSpace::Shared,enhanced?116:115,x(a,-2),y(a,120));a.combat.spellExtent-=100;consume_reimu_spirit(a,500,events);}
            if(at(a,4))aura(a,events);
            if(at(a,12)&&s.loops<=12){s.loops=add(s.loops,1);frame(a,10);}if(id>=706&&at(a,15))frame(a,13);return true;
        }
        if(id==702||id==703){
            if(at(a,6)){a.motion.set_hit_freeze(40);events.system_sound(23);emit(a,events,FighterEffectSpace::Shared,enhanced?116:115,x(a,7),y(a,226));a.combat.spellExtent-=100;}
            if(a.animation.cursor.index==10||a.animation.cursor.index==11)consume_reimu_spirit(a,5,events);
            if(end(a))a.select_action(0);if(at(a,8)){emit(a,events,FighterEffectSpace::Owned,enhanced?819:816,x(a,0),a.motion.y);events.sound(enhanced?11:7);}
            if(at(a,12)&&s.loops<10){s.loops=add(s.loops,1);frame(a,10);}if(at(a,14)&&s.impactVelocity<=4){s.impactVelocity=add(s.impactVelocity,1);frame(a,12);}return true;
        }
        if(start(a)){a.motion.set_hit_freeze(40);events.system_sound(23);emit(a,events,FighterEffectSpace::Shared,enhanced?116:115,x(a,24),y(a,144));a.combat.spellExtent-=100;if(id==704)consume_reimu_spirit(a,500,events);}
        if(end(a))a.select_action(0);if(at(a,7)){const float px=float(double(a.motion.x)+100+i32(a.motion.facing)*128);emit(a,events,FighterEffectSpace::Owned,enhanced?818:814,px,y(a,93));events.sound(enhanced?9:8);}
        if(at(a,12)&&s.loops<=(enhanced?5:8)){s.loops=add(s.loops,1);frame(a,10);}return true;
    }
    if(story){
        const bool air=id>=722;if(start(a)){a.response=1;s.loops=0;if(!air)a.motion.clear();}
        if(air&&a.animation.cursor.index>=0){if(air_land(a))return true;}
        if(end(a)){if(air)frame(a,9);else a.select_action(0);}
        const i16 trigger=air?4:8,again=air?6:10;
        const auto fire=[&](){a.launchParameters[1]=0;a.launchParameters[0]=float(id==720?s.loops*5:id==721?s.loops*5+30:id==722?s.loops*-6:s.loops*-9-30);emit(a,events,FighterEffectSpace::Owned,913,x(a,id==720?114:id==721?90:id==722?83:67),y(a,id==720?96:id==721?158:id==722?117:75));events.sound(0);s.loops=add(s.loops,1);};
        if(at(a,trigger))fire();if(at(a,trigger+1))fire();if(at(a,again)&&s.loops<difficulty*3+1){fire();frame(a,trigger);}return true;
    }
    if(id==731){
        if(start(a)){a.response=1;s.loops=s.impactVelocity=0;if(a.motion.y==0)a.motion.clear();else{a.motion.velocityX=float(double(a.motion.velocityX)*.2f);a.motion.velocityY=float(double(a.motion.velocityY)*.2f);}}
        if(a.motion.y>0){if(a.animation.cursor.index<12){a.motion.velocityY-=.01f;if(double(a.motion.y)+a.motion.velocityY<=0)a.motion.velocityX=0;}else{a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0&&a.animation.cursor.index<16){a.motion.y=0;a.motion.clear();frame(a,16);return true;}}}
        if(at(a,10)&&s.loops<difficulty*3+4){events.sound(6);a.launchParameters[0]=float(random.range(0,359));if(s.impactVelocity>=0&&s.impactVelocity<=2){emit(a,events,FighterEffectSpace::Owned,900+s.impactVelocity,x(a,18),y(a,120));a.launchParameters[0]=float(random.range(0,359));}s.impactVelocity=add(s.impactVelocity,1);if(s.impactVelocity>2)s.impactVelocity=0;}
        if(end(a)){a.select_action(0);a.commandTimers[2]=i16(300-difficulty*20);}if(at(a,4))aura(a,events);
        if(at(a,12)&&s.loops<30){s.loops=add(s.loops,1);frame(a,10);}if(at(a,15)&&a.motion.y==0){a.select_action(10);return true;}if(at(a,16))frame(a,14);return true;
    }
    if(id==735||id==736){spell_start(a,s);if(a.motion.y>0){a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0){a.select_action(10);a.motion.y=0;a.motion.clear();return true;}}
        if(end(a))frame(a,id==735?10:8);if(at(a,2)){a.motion.velocityX=id==735?float(random.range(0,3)+6):float(-6-random.range(0,3));a.motion.velocityY=float(random.range(0,id==735?4:3)+10);a.motion.accelerationY=.3f;}return true;
    }
    if(id==741){spell_start(a,s);if(end(a)){a.select_action(0);a.commandTimers[2]=i16(240-difficulty*30);}if(at(a,8)){emit(a,events,FighterEffectSpace::Owned,910,x(a,0),a.motion.y);events.sound(7);a.commandTimers[2]=180;}if(at(a,12)&&s.loops<1){s.loops=add(s.loops,1);frame(a,10);}if(at(a,14)&&s.impactVelocity<=0){s.impactVelocity=add(s.impactVelocity,1);frame(a,12);}return true;}
    if(id==751){
        if(start(a)){a.motion.clear();a.remainingHits=1;a.response=0;a.carryMomentum=0;s.loops=0;}
        if(at(a,1)){a.begin_pose(0,100,0);a.pose.scale[0]=a.pose.scale[1]=1;a.pose.color=0xffffffff;events.sound(2);}
        if(a.animation.cursor.index==1){a.pose.scale[0]-=.1f;a.pose.scale[1]+=.2f;a.pose.color=(a.pose.color&0x00ffffff)|(u32(u8((a.pose.color>>24)-25))<<24);}
        if(a.animation.cursor.index==3){if(a.pose.scale[0]>=1){a.pose.scale[0]=a.pose.scale[1]=1;a.pose.color|=0xff000000;}else{a.pose.scale[0]+=.1f;a.pose.scale[1]-=.2f;a.pose.color=(a.pose.color&0x00ffffff)|(u32(u8((a.pose.color>>24)+25))<<24);}}
        if(a.motion.y>0&&a.animation.cursor.index<14){a.motion.velocityY-=.2f;if(double(a.motion.y)+a.motion.velocityY<0){a.motion.y=a.motion.velocityX=a.motion.velocityY=0;frame(a,14);return true;}}
        if(s.loops<difficulty*2+3&&a.animation.cursor.elapsed==0&&(a.animation.cursor.index==7||a.animation.cursor.index==8)){s.loops=add(s.loops,1);events.sound(12);a.launchParameters[0]=float(s.loops);emit(a,events,FighterEffectSpace::Owned,916,x(a,80),y(a,120));a.commandTimers[2]=120;}
        if(end(a)){a.select_action(0);a.commandTimers[2]=i16(random.range(0,60)+120-difficulty*40);}
        if(at(a,3)){const float teleportCenter=float(double(opponent.motion.x)+1000);a.motion.x=float(double(teleportCenter)-random.range(0,2000));a.motion.y=200;a.motion.velocityX=float(3-random.range(0,6));a.motion.velocityY=6;events.sound(2);a.motion.facing=a.motion.x<=opponent.motion.x?1:-1;}
        if(at(a,9)&&s.loops<6)frame(a,7);if(at(a,14))frame(a,12);return true;
    }
    if(id==761){spell_start(a,s);if(end(a)){a.select_action(0);a.commandTimers[2]=i16(240-difficulty*20);}if(at(a,7)){emit(a,events,FighterEffectSpace::Owned,difficulty<2?917:918,x(a,228),y(a,93));events.sound(8);}return true;}
    if(id==771){
        spell_start(a,s,false);a.motion.opponentPush=opponent.motion.opponentPush=0;if(a.animation.cursor.index<15&&a.motion.y>0)a.motion.velocityY-=a.motion.accelerationY;if(a.animation.cursor.index>14)s.boundaryAngle=add(s.boundaryAngle,5);
        if(end(a)){if(s.loops<8){s.loops=add(s.loops,1);frame(a,15);a.transitionReady=true;return true;}a.select_action(772);return true;}
        if(at(a,3)){a.motion.velocityX=float((640.0-double(a.motion.x))/30.0*i32(a.motion.facing));a.motion.velocityY=12;a.motion.accelerationY=.5f;a.motion.opponentPush=opponent.motion.opponentPush=0;}
        if(at(a,13))a.motion.clear();if(at(a,15)){declare(a,mode,replayMode,core,events);a.commandTimers[2]=0;a.launchParameters[1]=0;events.effect(FighterEffectSpace::Owned,932,a.motion.x,y(a,100),1,1,a.launchParameters);rings(a,difficulty,events);}return true;
    }
    if(id==772){
        spell_start(a,s,false);if(opponent.combat.health>0&&opponent.animation.cursor.index==2&&i8(opponent.combat.spellPhase)>0&&opponent.animation.cursor.action==99){opponent.select_action(95);opponent.transitionReady=true;return true;}
        s.loops=add(s.loops,1);if(s.loops<=(300-(10000-a.combat.health*2)/100)-difficulty*30){}else{for(i32 i=0;i<30;i++){a.launchParameters[0]=float(i*12);a.launchParameters[1]=60;events.effect(FighterEffectSpace::Owned,929,a.motion.x,y(a,100),1,1,a.launchParameters);}if(difficulty>1)for(i32 i=0;i<30;i++){a.launchParameters[0]=float(i*12);a.launchParameters[1]=100;events.effect(FighterEffectSpace::Owned,930,a.motion.x,y(a,100),1,1,a.launchParameters);}s.loops=0;}
        a.motion.opponentPush=opponent.motion.opponentPush=0;if(end(a))frame(a,1);return true;
    }
    spell_start(a,s,false);
    if(id==795){if(end(a))a.select_action(0);if(at(a,5)){declare(a,mode,replayMode,core,events);emit(a,events,FighterEffectSpace::Owned,997,a.motion.x,y(a,100));}if(at(a,9)&&s.loops<=7){s.loops=add(s.loops,1);frame(a,7);}return true;}
    if(end(a)){a.select_action(0);if(a.phaseVariants[a.combat.spellPhase]==4){s.boundaryAngle=0;a.commandTimers[2]=0;a.launchParameters[1]=0;events.effect(FighterEffectSpace::Owned,931,a.motion.x,y(a,100),1,1,a.launchParameters);rings(a,difficulty,events);}}
    if(at(a,20))declare(a,mode,replayMode,core,events);if(at(a,24)&&s.loops<=7){s.loops=add(s.loops,1);frame(a,22);a.transitionReady=true;}return true;
}
}
