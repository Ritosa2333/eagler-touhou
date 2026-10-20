#include "ReimuObjects.hpp"
#include "ObjectTools.hpp"
#include <algorithm>
namespace th075 {
using namespace object;
namespace {
void counters(ReimuObject& p){p.counter=p.timer=p.angle=p.cycle=0;}
void active(ReimuObject& p,float size,i16 hits){p.begin_pose();scale(p,size);p.hits=hits;p.response=0;}
void spell_orb(ReimuObject& p,const FighterActor& owner,const FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events,bool aimed){
    if(p.response==6){p.response=0;p.hits=1;}if(owner.combat.spellTimer<1){p.destroy();return;}
    if(start(p)){p.counter=1;p.timer=add(i16(random.range(0,aimed?500:400)),aimed?1500:300);p.angle=i16(u16(i32(p.parameters[0])));active(p,.1f,1);p.pose.rotation[2]=0;}
    if(at(p,1)){p.hits=1;p.response=0;if(aimed)p.angle=aim(p,opponent);}
    if(p.response!=0&&p.animation.cursor.index==1){events.shake(2);p.frame(2);p.pose.rotation.fill(0);scale(p,4);return;}
    if((aimed&&p.animation.cursor.index==0)||(!aimed&&p.animation.cursor.index<2)){
        if(aimed){p.timer=add(p.timer,-60);if(p.timer<0)p.timer=0;}p.pose.scale[0]=float(double(p.pose.scale[0])+.1f);p.pose.scale[1]=float(double(p.pose.scale[1])+.1f);if(p.pose.scale[0]>1)scale(p,1);
    }
    if(p.animation.cursor.index==1)p.timer=add(p.timer,50);
    if(p.animation.cursor.index==2){p.timer=add(p.timer,-80);if(p.timer<0)p.timer=0;p.pose.scale[0]=float(double(p.pose.scale[0])-.2f);p.pose.scale[1]=float(double(p.pose.scale[1])-.2f);alpha(p,u8(alpha(p)-10));if(p.pose.scale[0]<0)scale(p,0);}
    p.polar(float(p.angle),float(double(p.timer)*.01f));move(p);p.pose.rotation[2]=float(double(p.pose.rotation[2])+10);
    if(outside(p,-200,1480,-320,1280)||owner.combat.spellTimer==0){p.destroy();return;}if(end(p))p.destroy();
}
void slowing_arrow(ReimuObject& p,const FighterActor& owner,i16 initial,bool parameterSpeed,i16 decelerate,i16 accelerate,float initialScale=.1f){
    if(outside(p)||owner.combat.spellTimer<1){p.destroy();return;}if(start(p)){active(p,1,1);p.counter=parameterSpeed?i16(u16(i32(p.parameters[1]))):initial;p.timer=p.cycle=0;p.angle=i16(u16(i32(p.parameters[0])));p.polar(float(p.angle),float(double(p.counter)*initialScale));}
    if(p.animation.cursor.index==1){if(p.counter>1)p.counter=add(p.counter,-decelerate);if(p.counter<1)p.counter=1;}if(p.animation.cursor.index==2)p.counter=add(p.counter,accelerate);
    p.polar(float(p.angle),float(double(p.counter)*.1f));p.pose.rotation[2]=degrees(p.motion.velocityY,p.motion.velocityX);move(p);if(p.animation.cursor.index<3&&p.hits==0)p.frame(3);if(p.animation.cursor.index>2)fade(p,25);if(end(p))p.destroy();
}
void floating_seal(ReimuObject& p,const FighterActor& owner,bool large){
    if(owner.combat.spellTimer<1){p.destroy();return;}if(start(p)){active(p,large?6:2,99);counters(p);p.motion.velocityX=p.motion.velocityY=0;}
    if(p.animation.cursor.index>19){p.pose.rotation[2]=float(double(p.pose.rotation[2])-4);p.motion.velocityX=float(double(p.motion.velocityX)+(large?.02f:.05f));}
    const auto reset=[&](){if(p.response!=0){p.counter=add(p.counter,1);if(p.response==6||p.response==4)p.counter=large?3:6;if(p.counter>(large?2:5)){p.counter=0;p.response=0;}}};
    if(large)reset();move(p,1);if(!large)reset();if(p.motion.x< -320||p.motion.x>1600){p.destroy();return;}if(end(p))p.frame(large?40:27);
}
}
bool advance_reimu_object_story(ReimuObject& p,FighterActor& owner,FighterActor& opponent,ReimuState& state,ReplayStreams& random,FighterActionEvents& events){
    const auto index=[&](){return p.animation.cursor.index;};const i16 ownerAction=owner.animation.cursor.action;
    switch(p.animation.cursor.action){
    case 900:case 901:case 902:case 903:case 904:case 905:spell_orb(p,owner,opponent,random,events,p.animation.cursor.action<903);return true;
    case 910:
        if(start(p)){p.begin_pose();scale(p,4);counters(p);}if(owner.combat.spellTimer<1){p.destroy();return true;}if(index()==6)fade(p,7);if(end(p))p.destroy();if(at(p,6))spawn(owner,events,p,911);return true;
    case 911:
        if(start(p)){active(p,1,0);p.counter=p.timer=0;}if(owner.combat.spellTimer<1||alpha(p)<15){p.destroy();return true;}alpha(p,u8(alpha(p)-15));if(at(p,1)){spawn(owner,events,912,p.motion.x,p.motion.y,1);spawn(owner,events,912,p.motion.x,p.motion.y,-1);}if(end(p))p.destroy();return true;
    case 912:
        if(start(p)){active(p,1,99);p.counter=p.timer=0;p.motion.velocityX=1.5f;}if(owner.combat.spellTimer<1){p.destroy();return true;}p.motion.x=float(double(p.motion.x)+i32(p.motion.facing)*double(p.motion.velocityX));if(p.motion.x>1480||p.motion.x< -200||owner.combat.spellTimer==0){p.destroy();return true;}if(end(p))p.frame(0);return true;
    case 913:slowing_arrow(p,owner,80,false,4,3);return true;
    case 916:
        if(outside(p)||owner.combat.spellTimer<1){p.destroy();return true;}if(start(p)){active(p,0,1);p.motion.velocityX=float(double(p.parameters[0])+0);p.motion.velocityY=3;}if(index()==1){p.pose.scale[0]=float(double(p.pose.scale[0])+.1f);if(p.pose.scale[0]>1)p.pose.scale[0]=1;p.pose.scale[1]=p.pose.scale[0];}p.pose.rotation[2]=float(double(p.pose.rotation[2])+10);move(p,1);p.motion.velocityY=float(double(p.motion.velocityY)-.5);if(double(p.motion.y)+p.motion.velocityY<0){p.motion.y=0;p.motion.velocityY=-p.motion.velocityY;p.response=0;p.hits=1;}if(p.response==6){p.hits=1;p.response=0;}if(end(p))p.destroy();return true;
    case 917:case 918:floating_seal(p,owner,p.animation.cursor.action==918);return true;
    case 919:{
        if(owner.combat.spellTimer<1){p.destroy();return true;}p.motion.facing=1;if(start(p)){p.begin_pose();scale(p,1);counters(p);p.variant=0;}p.variant=add(p.variant,-1);p.cycle=add(p.cycle,-1);p.pose.rotation[2]=float(double(p.pose.rotation[2])+6);p.polar(float(double(state.orbitAngle)+p.parameters[0]),float(state.recovery.boundaryAngle));p.motion.x=float(double(owner.motion.x)+p.motion.velocityX);p.motion.y=float((double(owner.motion.y)+p.motion.velocityY)+100);
        if(state.volleyMode==0&&ownerAction==771)p.variant=0;
        if(state.volleyMode==1&&p.variant<1){events.sound(0);owner.launchParameters[0]=0;owner.launchParameters[1]=60;spawn(owner,events,927,p.motion.x,p.motion.y,owner.motion.facing);p.variant=i16(5-state.difficulty);}
        if(state.volleyMode==2&&p.variant<1){events.sound(0);owner.launchParameters[0]=float(-(double(state.orbitAngle)+p.parameters[0])*i32(p.motion.facing));owner.launchParameters[1]=60;spawn(owner,events,927,p.motion.x,p.motion.y,1);p.variant=i16(5-state.difficulty);}
        if(state.volleyMode==3&&p.cycle<1){events.sound(13);owner.launchParameters[0]=float(-((double(state.orbitAngle)+p.parameters[0])+0)*i32(p.motion.facing));owner.launchParameters[1]=100;spawn(owner,events,928,p.motion.x,p.motion.y,1);p.cycle=10;}
        if(state.volleyMode==4&&p.cycle<1){events.sound(13);owner.launchParameters[0]=float(-((double(state.orbitAngle)+p.parameters[0])+180)*i32(p.motion.facing));owner.launchParameters[1]=40;spawn(owner,events,929,p.motion.x,p.motion.y,1);p.cycle=10;}
        if(ownerAction==772&&p.variant<1){events.sound(0);owner.launchParameters[0]=float(-(double(state.orbitAngle)+p.parameters[0])*i32(p.motion.facing));owner.launchParameters[1]=250;spawn(owner,events,940,p.motion.x,p.motion.y,1);p.variant=3;}if(end(p))p.frame(1);return true;
    }
    case 927:case 928:case 929:case 930:case 940:{const i16 kind=p.animation.cursor.action;slowing_arrow(p,owner,0,true,2,kind==928?-2:kind==930?-3:3,kind==928?.05f:.1f);return true;}
    case 931:case 932:{
        const bool steady=p.animation.cursor.action==932;
        if(owner.combat.spellTimer<1){p.destroy();state.orbitAngle=state.recovery.boundaryAngle=0;return true;}
        if(start(p)){p.begin_pose();scale(p,1);p.motion.velocityX=p.motion.velocityY=0;counters(p);p.timer=1;p.variant=0;owner.commandTimers[2]=60;}
        if(steady){p.parameters[1]=float(double(p.parameters[1])+.1f);if(p.parameters[1]>=5)p.parameters[1]=5;}else{
            if(p.timer==1){p.parameters[1]=float(double(p.parameters[1])+.1f);if(p.parameters[1]>=10)p.timer=-1;}
            if(p.timer==-1){p.parameters[1]=float(double(p.parameters[1])-.1f);if(p.parameters[1]<=-10)p.timer=1;}
        }
        if(owner.motion.hitFreeze==0)state.orbitAngle=i16(u16(i32(double(state.orbitAngle)+(steady?1.f:p.parameters[1]))));if(state.orbitAngle>180)state.orbitAngle=add(state.orbitAngle,-360);if(state.orbitAngle< -180)state.orbitAngle=add(state.orbitAngle,360);
        if(steady&&ownerAction==772){p.variant=add(p.variant,1);if(p.variant>7){state.recovery.boundaryAngle=add(state.recovery.boundaryAngle,-1);p.variant=0;}if(state.recovery.boundaryAngle<120)state.recovery.boundaryAngle=add(state.recovery.boundaryAngle,2);}
        if(!steady){p.motion.facing=1;owner.commandTimers[2]=add(owner.commandTimers[2],-1);p.angle=add(p.angle,-1);}
        if(owner.commandTimers[2]<1){
            if(!steady){state.volleyMode=random.range(0,100)<50?1:2;owner.commandTimers[2]=i16(300-state.difficulty*20);p.angle=i16(20+state.difficulty*5);}else{owner.commandTimers[2]=i16(300-state.difficulty*30);state.volleyMode=3;p.angle=1;}return true;
        }
        if(p.angle<1)state.volleyMode=0;if(steady){p.motion.facing=1;owner.commandTimers[2]=add(owner.commandTimers[2],-1);p.angle=add(p.angle,-1);}if(state.recovery.boundaryAngle<120)state.recovery.boundaryAngle=add(state.recovery.boundaryAngle,2);if(end(p))p.frame(1);return true;
    }
    case 933:
        if(outside(p)||owner.combat.spellTimer<1){p.destroy();return true;}if(start(p)){active(p,1,1);p.timer=p.cycle=0;p.angle=i16(u16(i32(p.parameters[0])));p.variant=0;const float y=float((double(opponent.motion.y)-p.motion.y)+100),x=float((double(opponent.motion.x)-p.motion.x)*i32(p.motion.facing));p.polar(degrees(y,x),2);}move(p);p.pose.rotation[2]=float(double(p.pose.rotation[2])+4);if(p.response==6)p.hits=1;if(end(p)){p.frame(1);p.response=0;if(p.variant>20)p.destroy();}return true;
    case 934:
        if(owner.combat.spellTimer<1){p.destroy();return true;}if(start(p)){scale(p,1);p.pose.color=0xffffffff;}if(index()==1){p.pose.scale[0]=float(double(p.pose.scale[0])+.2f);p.pose.scale[1]=float(double(p.pose.scale[1])+.2f);if(alpha(p)<21){p.destroy();return true;}alpha(p,u8(alpha(p)-20));}if(end(p))p.destroy();return true;
    case 950:
        if(ownerAction!=700&&ownerAction!=701&&ownerAction!=706&&ownerAction!=707){p.destroy();return true;}if(start(p)){active(p,1,1);p.motion.velocityY=15;}p.pose.rotation[2]=float(double(p.pose.rotation[2])-35);p.motion.y=float(double(p.motion.y)+p.motion.velocityY);if(p.motion.y>1000){p.destroy();return true;}if(end(p))p.destroy();return true;
    case 951:{
        if(index()<1)p.motion.y=owner.motion.y;p.motion.x=owner.motion.x;state.recovery.smokeEmitted=1;
        if(owner.animation.cursor.index==0&&((ownerAction>69&&ownerAction<89&&ownerAction!=76)||ownerAction==180||ownerAction==181||ownerAction==145)){p.destroy();state.recovery.smokeEmitted=0;return true;}
        if(start(p)){p.begin_pose(-45,45,0);scale(p,1);}
        if(index()==0&&(ownerAction==96||ownerAction==97)&&owner.animation.cursor.index==5){p.frame(1);p.pose.rotation[2]=0;return true;}
        if(index()==5&&(ownerAction==95||ownerAction==94||ownerAction==93||ownerAction==796||ownerAction==771)&&owner.animation.cursor.index==3){p.frame(6);return true;}
        if(index()==7&&(ownerAction==95||ownerAction==94||ownerAction==93||ownerAction==796)&&owner.animation.cursor.index==11){p.destroy();state.recovery.smokeEmitted=0;return true;}
        if(at(p,7)&&ownerAction!=771&&ownerAction!=772){p.motion.velocityY=15;p.motion.accelerationY=-1;}
        if(index()==7&&ownerAction!=771&&ownerAction!=772){p.motion.velocityY=float(double(p.motion.velocityY)+p.motion.accelerationY);p.motion.y=float(double(p.motion.y)+p.motion.velocityY);}
        if(index()==7&&(ownerAction==771||ownerAction==772)){p.motion.x=float(double(owner.motion.x)+i32(p.motion.facing)*120);p.motion.y=float(double(owner.motion.y)+100);}
        if(index()==0||index()==7)p.pose.rotation[2]=float(double(p.pose.rotation[2])-35);if(end(p))p.destroy();return true;
    }
    case 990:case 991:{const bool large=p.animation.cursor.action==991;
        if(state.frame.farRangeFrames==0&&index()<2){p.frame(2);return true;}p.motion.x=owner.motion.x;p.motion.y=float(double(owner.motion.y)+100);p.contactDelay=1;if(start(p)){p.begin_pose();scale(p,large?0:2);alpha(p,0);}if(large)scale(p,float(double(state.frame.farRangeFrames)/45));else p.pose.rotation[2]=float(double(p.pose.rotation[2])+1);
        if(index()==0){const u8 cap=large?255:180;if(alpha(p)<(large?230:180))alpha(p,u8(alpha(p)+25));else alpha(p,cap);if(state.frame.farRangeFrames>119){p.frame(1);alpha(p,cap);return true;}}
        if(index()==2){fade(p,large?10:30);if(alpha(p)==0){p.destroy();return true;}}if(end(p))p.destroy();return true;
    }
    case 994:case 997:{const bool small=p.animation.cursor.action==997;p.motion.x=owner.motion.x;p.motion.y=small?0:1;p.contactDelay=1;
        if(start(p)){p.begin_pose();scale(p,3);p.pose.projection[2]=1250;p.pose.rotation={-80,0,0};p.counter=owner.combat.spellTimer;}if(owner.combat.spellTimer<1){p.destroy();return true;}
        // Each ratio is stored as float by the original before the final scale.
        const float ratio=float(double(owner.combat.spellTimer)/p.counter);scale(p,float((double(ratio)+ratio)+(small?0:1)));p.pose.rotation[2]=float(double(p.pose.rotation[2])+(small?1:-1));if(end(p))p.frame(small?10:1);return true;
    }
    case 995:case 996:{const bool small=p.animation.cursor.action==996;p.motion.facing=1;if(start(p)){p.begin_pose();scale(p,1);p.motion.velocityX=0;p.motion.velocityY=-1;p.counter=add(i16(random.range(0,360)),-180);p.timer=15;}
        if(index()==0){p.timer=add(p.timer,-1);if(p.timer<3)p.timer=3;p.polar(float(p.counter),float(double(p.timer)*.8f));}
        if(index()==1){p.counter=aim(p,opponent,false);p.polar(float(p.counter),20);const double dx=double(p.motion.x)-opponent.motion.x,dy=double(p.motion.y)-opponent.motion.y;
            if(dx<20&&dx> -20&&dy<120&&dy>80){
                if(!small){p.destroy();if(i8(opponent.selectedSpell)<2)opponent.selectedSpell=u8(opponent.selectedSpell+1);else opponent.score=increment_wrapped(opponent.score,20000);events.system_sound(46);return true;}
                owner.knockdownTime=add(owner.knockdownTime,1);events.system_sound(46);p.destroy();if(opponent.combat.spiritBreak>0){if(opponent.combat.spiritBreak>5)opponent.combat.spiritBreak=add(opponent.combat.spiritBreak,-5);else{opponent.combat.spiritBreak=0;opponent.combat.spirit=1000;}return true;}
                if(opponent.combat.spirit<990){opponent.combat.spirit=add(opponent.combat.spirit,10);return true;}opponent.combat.spirit=1000;if(opponent.combat.spellEnded!=0)return true;if(opponent.combat.spellExtent<998.9f)opponent.combat.spellExtent=float(double(opponent.combat.spellExtent)+.5);else opponent.combat.spellExtent=900;return true;
            }
        }
        move(p);if(end(p))p.frame(1);return true;
    }
    case 998:
        p.motion.facing=owner.motion.facing;if(index()==0&&ownerAction>=150&&ownerAction<=158){p.motion.x=float(double(owner.motion.x)+i32(p.motion.facing)*57);p.motion.y=float(double(owner.motion.y)+(ownerAction<=153?94:ownerAction<=157?60:100));}
        if(index()==0&&(ownerAction<150||ownerAction>158)){p.frame(1);state.frame.blockEffectSent=0;}if(start(p)&&p.counter==0){p.begin_pose();scale(p,.75f);p.pose.rotation[1]=50;p.counter=1;}p.pose.rotation[2]=float(double(p.pose.rotation[2])+2);if(index()>0){if(alpha(p)<31)alpha(p,0);else alpha(p,u8(alpha(p)-30));}if(end(p))p.destroy();if(at(p,1))p.frame(0);return true;
    default:return false;
    }
}
}
