#include "ReimuRecovery.hpp"
namespace th075 {
namespace {
i16 add_word(i16 value,i32 amount){return i16(u16(u32(i32(value))+u32(amount)));}
bool start(const FighterActor& a){return a.animation.cursor.index==0&&a.animation.cursor.elapsed==0;}
bool at(const FighterActor& a,i16 frame){return a.animation.cursor.index==frame&&a.animation.cursor.elapsed==0;}
void frame(FighterActor& a,i16 value){a.animation.set(a.animation.cursor.action,value,0);}
bool end(FighterActor& a){return a.animation.advance()==a.animation.cursor.frameCount;}
void slowdown(FighterActor& a){if(a.motion.velocityX<0){a.motion.velocityX+=0.6f;if(a.motion.velocityX>0)a.motion.velocityX=0;}}
void charge(FighterActor& a){a.combat.spellCharge+=1;if(a.combat.spellCharge>1000)a.combat.spellCharge=1000;}
void landing(FighterActor& a,i16 action){a.select_action(action);a.motion.y=0;a.motion.clear();}
void trail(FighterActor& a){a.combat.noticeTimer=3;a.trails.interval=4;a.trails.colorMask=0xff;}
void emit(FighterActionEvents& events,const FighterActor& a,FighterEffectSpace space,i16 id,float x,float y,i8 facing){events.effect(space,id,x,y,facing,1,a.launchParameters);}
void impact_smoke(FighterActor& a,ReimuRecoveryState& s,FighterActionEvents& events){if(s.smokeEmitted==0){s.smokeEmitted=1;emit(events,a,FighterEffectSpace::Owned,951,a.motion.x,a.motion.y,a.motion.facing);}}
void save_impact(FighterActor& a,ReimuRecoveryState& s){s.loops=i16(u16(i32(double(a.motion.velocityX)*100.0)));s.impactVelocity=i16(u16(i32(double(a.motion.velocityY)*100.0)));}
void dust(FighterActor& a,ReimuRecoveryState& s,FighterActionEvents& events){for(u32 i=0;i<10;i++)emit(events,a,FighterEffectSpace::Owned,996,a.motion.x,a.motion.y,1);if(a.knockdownTime>25){a.knockdownTime=0;emit(events,a,FighterEffectSpace::Owned,995,a.motion.x,a.motion.y,1);}}
void inertia(FighterActor& a){
    if(a.carryMomentum==0){a.motion.clear();return;}
    if(a.motion.velocityX>0){a.motion.velocityX=float(double(a.motion.velocityX)-.5);if(a.motion.velocityX<0){a.motion.clear();a.carryMomentum=0;}}
    if(a.motion.velocityX<0){a.motion.velocityX=float(double(a.motion.velocityX)+.5);if(a.motion.velocityX>0){a.motion.clear();a.carryMomentum=0;}}
}
struct HurtSlide {i16 action;float speed;i16 finish;i16 loopFrame;i16 limit;bool resetLoop;bool gainCharge;};
constexpr HurtSlide slides[]={
    {50,-8,0,0,0,false,false},{51,-9,0,0,0,false,false},{52,-12,0,0,0,false,false},{53,-15,0,7,0,true,false},
    {56,-8,0,0,0,false,false},{57,-9,0,0,0,false,false},{58,-12,0,0,0,false,false},{59,-15,0,7,0,true,false},
    {62,-8,2,0,0,false,false},{63,-9,2,0,0,false,false},{64,-12,2,0,0,false,false},{65,-15,0,7,0,true,false},
    {140,-11,0,3,4,true,false},{143,-12,0,3,6,true,false},{144,-12,0,3,6,true,false},
    {150,-9,0,0,0,true,true},{151,-10,0,0,0,true,true},{152,-13,0,0,0,true,true},
    {154,-9,2,0,0,true,true},{155,-10,2,0,0,true,true},{156,-13,2,0,0,true,true},
    {159,-9,0,0,0,true,true},{160,-10,0,0,0,true,true},{161,-13,0,0,0,true,true},
    {163,-9,2,0,0,false,true},{164,-10,2,0,0,false,true},{165,-13,2,0,0,false,true}
};
}
void consume_reimu_spirit(FighterActor& a,i16 amount,FighterActionEvents& events){
    if(a.combat.spiritBreak<1){a.combat.spirit=add_word(a.combat.spirit,-i32(amount));if(a.combat.spirit<1&&a.combat.spiritBreak<1){events.system_sound(43);a.combat.spirit=0;a.combat.spiritBreak=1000;}}
    else a.combat.spiritBreak=1000;
}
bool advance_reimu_recovery(FighterActor& a,const FighterActor& opponent,ReimuRecoveryState& s,GameMode mode,FighterActionEvents& events){
    const i16 action=a.animation.cursor.action;
    const HurtSlide* slide=nullptr;for(const auto& value:slides)if(value.action==action){slide=&value;break;}
    const bool supported=slide||(action>=0&&action<=10)||(action>=70&&action<=74)||action==76||action==88||action==89||(action>=93&&action<=95)||action==97||action==98||action==99||action==145||action==158||action==167||action==168||(action>=180&&action<=181)||(action>=200&&action<=210);
    if(!supported)return false;if(a.combat.freeze!=0||opponent.combat.opponentStop!=0)return true;
    if(slide){
        if(slide->gainCharge)charge(a);if(start(a)){a.motion.velocityX=slide->speed;a.motion.velocityY=0;a.response=0;if(slide->resetLoop)s.loops=0;}
        slowdown(a);if(a.motion.horizontal_limit(opponent.motion)!=0&&a.allowPush)a.motion.opponentPush=float(double(a.motion.velocityX)*.75);
        if(end(a)){a.select_action(slide->finish);if(slide->loopFrame==7)s.loops=0;}
        if(slide->loopFrame&&a.animation.cursor.index==slide->loopFrame&&(slide->loopFrame==7||a.animation.cursor.elapsed==0)&&s.loops<=slide->limit){s.loops=add_word(s.loops,1);frame(a,1);}return true;
    }
    if(action<=5){
        if(action<4)inertia(a);else a.motion.velocityX=action==4?4.5f:-4.5f;
        if(action==1){if(end(a))a.select_action(2);}else if(action==3){if(end(a))a.select_action(0);}else a.animation.advance();return true;
    }
    if(action>=6&&action<=8){
        const u8 command=action==6?3:action==7?5:4;
        if(start(a)&&(a.command==command||i8(a.carryMomentum)>0)){a.select_action(action+198);return true;}
        if(a.animation.cursor.index==0&&a.carryMomentum==0)a.motion.clear();
        if(a.motion.y>0){a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0){landing(a,10);return true;}}
        auto& cursor=a.animation.cursor;cursor.elapsed=add_word(cursor.elapsed,1);
        if(cursor.elapsed>=cursor.duration){cursor.elapsed=0;cursor.index=add_word(cursor.index,1);if(cursor.index==2){a.motion.velocityX=action==6?0:action==7?4:-4;a.motion.velocityY=12.5f;a.motion.accelerationY=.55f;a.carryMomentum=0;}if(cursor.index>=cursor.frameCount)cursor.index=add_word(cursor.index,-2);a.animation.set(cursor.action,cursor.index,0);}return true;
    }
    if(action==9||action==208){
        if(action==208&&start(a)){a.motion.facing=i8(-a.motion.facing);a.motion.velocityX=-a.motion.velocityX;}
        if(a.motion.accelerationY==0)a.motion.accelerationY=.6f;a.motion.velocityY-=a.motion.accelerationY;
        if(double(a.motion.y)+a.motion.velocityY<=0)landing(a,10);else if(end(a))frame(a,action==9?0:4);return true;
    }
    if(action==10){if(start(a))events.system_sound(30);a.motion.clear();if(end(a))a.select_action(a.controls.current.vertical==0?0:2);return true;}
    if(action==70||action==71||action==72||action==73||action==74||action==145){
        if(start(a)){
            if(action==145){a.motion.velocityX=-10;a.motion.velocityY=14;a.motion.accelerationY=.5f;}
            else{a.motion.velocityX=-a.impulseX;a.motion.velocityY=action==70?float(double(a.motion.velocityY)+a.impulseY):a.impulseY;a.motion.accelerationY=action==72?1.3f:.8f;if(action==70){if(a.motion.velocityY<2)a.motion.velocityY=2;if(a.motion.velocityY>12)a.motion.velocityY=12;}}
            a.response=0;
        }
        if(action!=72&&action!=73&&at(a,1))impact_smoke(a,s,events);
        const i16 transitionFrame=action==70?3:1;
        if(action!=73&&a.animation.cursor.index==transitionFrame&&a.motion.velocityY<2){frame(a,action==70?4:2);return true;}
        if(action==74&&a.motion.horizontal_limit(opponent.motion)!=0&&a.motion.velocityX<=-15){impact_smoke(a,s,events);a.select_action(76);a.motion.velocityX=float(-double(a.motion.velocityX)*.25);a.motion.velocityY=15;a.motion.accelerationY=.5f;events.shake(2);events.system_sound(22);s.loops=0;return true;}
        if(action!=73)a.motion.velocityY-=a.motion.accelerationY;
        if(double(a.motion.velocityY)+a.motion.y<=0){save_impact(a,s);a.motion.clear();a.motion.y=0;a.select_action(97);events.shake(2);if(action!=145)events.system_sound(22);return true;}
        if(end(a))frame(a,action==73?add_word(a.animation.cursor.index,-1):5);
        if(action==73&&at(a,1)){impact_smoke(a,s,events);if(a.motion.velocityY!=0)frame(a,a.motion.velocityX==0?3:2);}return true;
    }
    if(action==76){
        if(s.loops==0){s.loops=add_word(s.loops,1);a.begin_pose(0,95,0);}a.pose.rotation[2]+=30;a.motion.velocityY-=a.motion.accelerationY;
        if(double(a.motion.velocityY)+a.motion.y<=0){events.system_sound(22);save_impact(a,s);a.motion.clear();a.motion.y=0;a.select_action(97);a.pose.rotation[2]=0;events.shake(2);a.pose.active=false;}else if(end(a))frame(a,0);return true;
    }
    if(action==88){
        if(start(a)){a.motion.velocityX=-a.impulseX;a.motion.velocityY=a.impulseY;a.motion.accelerationY=.8f;a.response=0;}
        if(double(a.motion.velocityY)+a.motion.y<=0){save_impact(a,s);a.motion.clear();a.motion.y=0;a.select_action(89);events.shake(3);events.system_sound(22);dust(a,s,events);return true;}
        if(end(a))frame(a,add_word(a.animation.cursor.index,-1));if(at(a,1)){impact_smoke(a,s,events);if(a.motion.velocityY!=0)frame(a,a.motion.velocityX==0?3:2);}return true;
    }
    if(action==89){
        if(a.animation.cursor.index==3){a.pose.rotation[2]+=40;a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.velocityY)+a.motion.y<=0){save_impact(a,s);a.motion.clear();a.motion.y=0;a.select_action(97);a.pose.rotation[2]=0;events.shake(2);a.pose.active=false;events.system_sound(22);return true;}}
        if(end(a))frame(a,3);else if(at(a,3)){a.motion.velocityX=float(double(s.loops)*.0025f);a.motion.velocityY=float(double(-i32(s.impactVelocity))*.01f);a.motion.accelerationY=.7f;a.begin_pose(-7,75,0);}return true;
    }
    if(action==93||action==94||action==95){
        if(action!=95&&a.animation.cursor.index<11&&a.motion.y>0){a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0){a.motion.y=0;a.motion.clear();frame(a,11);return true;}}
        if(end(a)){a.select_action(0);if(a.controls.current.vertical>0)a.select_action(1);}
        if(a.animation.cursor.index==a.animation.cursor.frameCount-1){if(opponent.motion.x<a.motion.x)a.motion.facing=-1;if(a.motion.x<opponent.motion.x)a.motion.facing=1;}
        if(action!=95&&at(a,3)){a.motion.velocityY=4.5f;a.motion.velocityX=action==93?12.5f:-12.5f;a.motion.accelerationY=.35f;}if(action!=95&&at(a,11))frame(a,7);return true;
    }
    if(action==97||action==98){
        bool pressed=false;for(auto button:a.controls.current.buttons)pressed|=button!=0;const i32 direction=i32(a.motion.facing)*a.controls.current.horizontal;
        if(pressed&&direction>0)s.loops=1;if(pressed&&direction<0)s.loops=2;
        if(action==97){
            if(start(a))dust(a,s,events);
            if(a.animation.cursor.index==3||a.animation.cursor.index==4){a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.velocityY)+a.motion.y<0){a.motion.y=0;a.motion.clear();frame(a,5);return true;}}
            if(end(a)){a.select_action(a.transitionReady?98:99);return true;}
            if(at(a,3)){a.motion.velocityX=float(double(s.loops)*.005f);a.motion.velocityY=float(double(-i32(s.impactVelocity))*.005f);a.motion.accelerationY=2;}if(at(a,4))frame(a,3);
        }else if(end(a))a.select_action(s.loops==1?93:s.loops==2?94:95);return true;
    }
    if(action==99){
        const i16 index=a.animation.cursor.index;const i8 phase=i8(a.combat.spellPhase);
        if(mode==GameMode::Story&&a.combat.storyOpponent){
            if(index>1&&phase>0&&opponent.animation.cursor.action<150&&a.phaseVariants[phase]==5&&a.phaseKinds[phase]==2){a.select_action(771);return true;}
            if(((index>1&&a.phaseKinds[phase]==2)||(index>0&&a.phaseKinds[phase]==1))&&phase>0&&opponent.animation.cursor.action<150){a.select_action(796);return true;}
            if(index>1&&phase>0&&opponent.animation.cursor.action<150&&a.phaseKinds[phase]==0){a.select_action(95);a.transitionReady=true;return true;}
        }else if(index==2&&phase>0&&opponent.animation.cursor.action<150){a.select_action(95);a.transitionReady=true;return true;}
        if(end(a))frame(a,2);return true;
    }
    if(action==158){
        charge(a);if(start(a)){a.motion.velocityX=-6;a.motion.velocityY=8;a.motion.accelerationY=.75f;}
        if(a.motion.y>=0){a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0){landing(a,10);return true;}}
        if(end(a))a.select_action(9);return true;
    }
    if(action==167||action==168){
        charge(a);if(start(a)){consume_reimu_spirit(a,250,events);events.system_sound(44);s.loops=180;emit(events,a,FighterEffectSpace::Shared,56,float(i32(a.motion.facing)*30+double(a.motion.x)),float(double(a.motion.y)+100),a.motion.facing);}
        slowdown(a);a.motion.opponentPush=float(double(-i32(s.loops))*.1f);if(s.loops>0)s.loops=add_word(s.loops,-10);if(end(a))a.select_action(action==167?0:2);return true;
    }
    if(action==180||action==181){
        if(start(a)){a.motion.clear();a.motion.velocityX=action==180?8:-8;a.motion.velocityY=12.5f;a.motion.accelerationY=.75f;a.pose.rotation[2]=0;a.pose.active=false;}
        if(a.motion.y>0){a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0){landing(a,10);return true;}}
        if(end(a))frame(a,action==180?7:5);return true;
    }
    if(action==200){
        a.carryMomentum=1;trail(a);if(start(a)){a.motion.velocityX=0;s.loops=0;}
        if(a.motion.velocityX>8){a.motion.velocityX-=.5f;if(a.motion.velocityX<9)a.motion.velocityX=9;}
        const i32 direction=i32(a.motion.facing)*a.controls.current.horizontal;
        if(a.controls.current.vertical<0){a.select_action(direction>0?205:204);return true;}
        if(s.loops>0&&direction<1){a.select_action(207);return true;}
        if(end(a)){if(direction>0&&s.loops<3){frame(a,5);s.loops=add_word(s.loops,1);}else a.select_action(207);return true;}
        if(at(a,3)){a.motion.velocityX=11;emit(events,a,FighterEffectSpace::Shared,125,float(i32(a.motion.facing)*80+double(a.motion.x)),float(double(a.motion.y)+80),a.motion.facing);emit(events,a,FighterEffectSpace::Shared,126,a.motion.x,float(double(a.motion.y)+80),a.motion.facing);events.system_sound(31);}return true;
    }
    if(action==201){
        trail(a);if(start(a))a.motion.velocityX=0;
        if(at(a,2)){a.motion.velocityX=-10;a.motion.velocityY=5;a.motion.accelerationY=.6f;emit(events,a,FighterEffectSpace::Shared,125,a.motion.x,float(double(a.motion.y)+80),i8(-a.motion.facing));emit(events,a,FighterEffectSpace::Shared,126,a.motion.x,float(double(a.motion.y)+80),i8(-a.motion.facing));events.system_sound(31);}
        if(a.motion.y>0&&a.animation.cursor.index>1&&a.animation.cursor.index<5){a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0){a.motion.velocityX=a.motion.velocityY=a.motion.accelerationY=a.motion.y=0;frame(a,5);return true;}}
        if(end(a))a.select_action(0);if(at(a,5))frame(a,add_word(a.animation.cursor.index,-2));return true;
    }
    if(action==202||action==203){
        trail(a);if(start(a)){a.motion.velocityX=a.motion.velocityY=a.motion.accelerationY=0;if(action==203)s.loops=0;}
        if(action==202&&at(a,3)){a.motion.velocityX=9;a.motion.velocityY=5;a.motion.accelerationY=.45f;emit(events,a,FighterEffectSpace::Shared,125,float(i32(a.motion.facing)*80+double(a.motion.x)),float(double(a.motion.y)+110),a.motion.facing);emit(events,a,FighterEffectSpace::Shared,126,a.motion.x,float(double(a.motion.y)+110),a.motion.facing);events.system_sound(31);}
        if(a.motion.y>0&&a.animation.cursor.index>(action==202?1:0)){a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0){a.motion.velocityX=a.motion.velocityY=a.motion.accelerationY=a.motion.y=0;a.animation.cursor.elapsed=0;a.select_action(10);return true;}}
        if(end(a))frame(a,action==202?9:8);
        if(action==203&&at(a,1)){a.motion.velocityX=-9;a.motion.velocityY=5;a.motion.accelerationY=.45f;emit(events,a,FighterEffectSpace::Shared,125,a.motion.x,float(double(a.motion.y)+120),i8(-a.motion.facing));emit(events,a,FighterEffectSpace::Shared,126,a.motion.x,float(double(a.motion.y)+120),i8(-a.motion.facing));events.system_sound(31);}return true;
    }
    if(action>=204&&action<=206){
        if(a.motion.y==0||a.motion.velocityY>0){trail(a);a.combat.recoveryTimers[2]=2;}
        if(a.animation.cursor.index==0&&a.carryMomentum==0)a.motion.clear();
        if(a.motion.y>0){a.motion.velocityY-=a.motion.accelerationY;if(double(a.motion.y)+a.motion.velocityY<=0){landing(a,10);return true;}}
        if(action!=206&&a.animation.cursor.index==4&&a.motion.velocityY<4)frame(a,5);
        auto& cursor=a.animation.cursor;cursor.elapsed=add_word(cursor.elapsed,1);if(cursor.elapsed>=cursor.duration){cursor.elapsed=0;cursor.index=add_word(cursor.index,1);if(cursor.index==2){a.motion.velocityX=action==204?0:action==205?9:-9;a.motion.velocityY=action==204?20:15;a.motion.accelerationY=.65f;a.carryMomentum=0;emit(events,a,FighterEffectSpace::Shared,60,a.motion.x,a.motion.y,1);}if(cursor.index>=cursor.frameCount)cursor.index=add_word(cursor.index,-2);a.animation.set(cursor.action,cursor.index,0);}return true;
    }
    if(action==207){a.carryMomentum=1;a.motion.velocityX-=.5f;if(a.motion.velocityX<0){a.motion.velocityX=0;a.carryMomentum=0;}if(end(a)){a.select_action(0);a.carryMomentum=0;}return true;}
    if(action==209||action==210){
        if(a.combat.spirit<2){a.select_action(action==209?0:9);return true;}a.combat.spirit=add_word(a.combat.spirit,-3);
        if(start(a)){if(action==209)a.motion.clear();else{a.motion.velocityX=float(double(a.motion.velocityX)*.25);a.motion.velocityY=float(double(a.motion.velocityY)*.25);}events.system_sound(44);emit(events,a,FighterEffectSpace::Owned,997,a.motion.x,float(double(a.motion.y)+100),a.motion.facing);}
        if(action==210)a.motion.velocityY=float(double(a.motion.velocityY)-double(a.motion.accelerationY)*.25);
        if(end(a)){if(a.controls.current.buttons[3]!=0)frame(a,2);else a.select_action(action==209?0:9);}return true;
    }
    return false;
}
}
