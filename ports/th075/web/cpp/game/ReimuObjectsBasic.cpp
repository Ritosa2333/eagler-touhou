#include "ReimuObjects.hpp"
#include "ObjectTools.hpp"
#include <algorithm>
namespace th075 {
using namespace object;
namespace {
void counters(ReimuObject& p){p.counter=p.timer=p.angle=p.cycle=0;}
void active(ReimuObject& p,float size,i16 hits){p.begin_pose();scale(p,size);p.hits=hits;p.response=0;}
void petal(ReimuObject& p){if(start(p)){active(p,1,1);}if(p.animation.cursor.index==0||p.animation.cursor.index==1){p.pose.scale[0]=float(double(p.pose.scale[0])-.1f);p.pose.scale[1]=float(double(p.pose.scale[1])-.1f);if(p.animation.cursor.index==1)alpha(p,u8(alpha(p)-50));}if(end(p))p.destroy();}
void expanding(ReimuObject& p,float size,float step,u8 fading){if(start(p)){p.begin_pose();scale(p,size);p.response=0;}p.pose.scale[0]=float(double(p.pose.scale[0])+step);p.pose.scale[1]=float(double(p.pose.scale[1])+step);if(p.animation.cursor.index>0)fade(p,fading);if(end(p))p.destroy();}
void orb(ReimuObject& p,FighterActor& owner,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events,i16 trail,bool strong){
    if(p.response==5){p.destroy();return;}if(p.response==6){p.response=0;p.hits=1;}
    if(start(p)){p.counter=1;p.timer=strong?2000:1500;for(i32 i=0;i<8;i++)if(p.parameters[0]==float(i))p.angle=i16(i*45);p.begin_pose();p.hits=1;p.response=0;scale(p,.1f);p.pose.rotation[2]=0;}
    if(!strong&&at(p,1)){p.hits=1;p.response=0;p.angle=aim(p,opponent);}
    if(p.animation.cursor.index==1){const float centerY=float(double(p.motion.y)+15);const float y=float(double(centerY)-random.range(0,30));const float centerX=float(double(p.motion.x)+15);const float x=float(double(centerX)-random.range(0,30));spawn(owner,events,trail,x,y,p.motion.facing);}
    if(p.response!=0&&p.animation.cursor.index==1&&(!strong||p.hits==0)){events.shake(strong?5:2);p.frame(2);p.pose.rotation.fill(0);scale(p,strong?3.5f:4);if(strong)p.motion.velocityX=p.motion.velocityY=0;return;}
    if(p.animation.cursor.index==0){p.timer=add(p.timer,strong?-80:-60);if(p.timer<0)p.timer=0;p.pose.scale[0]=float(double(p.pose.scale[0])+(strong?.2f:.1f));p.pose.scale[1]=float(double(p.pose.scale[1])+(strong?.2f:.1f));if(p.pose.scale[0]>(strong?1.5f:1)){scale(p,strong?1.5f:1);}}
    if(p.animation.cursor.index==1)p.timer=add(p.timer,100);
    if(p.animation.cursor.index==2){p.timer=add(p.timer,-80);if(p.timer<0)p.timer=0;p.pose.scale[0]=float(double(p.pose.scale[0])-(strong?.3f:.2f));p.pose.scale[1]=float(double(p.pose.scale[1])-(strong?.3f:.2f));alpha(p,u8(alpha(p)-10));if(p.pose.scale[0]<0)scale(p,0);}
    if(!strong||p.animation.cursor.index<2)p.polar(float(p.angle),float(double(p.timer)*.01f));move(p);p.pose.rotation[2]=float(double(p.pose.rotation[2])+10);
    if(outside(p,-200,1480,-320,1280)){p.destroy();return;}if(end(p))p.destroy();
    if(strong&&at(p,1)){p.hits=3;p.response=0;p.angle=aim(p,opponent);}if(strong&&at(p,2)){p.frame(1);p.response=0;}
}
void pillar(ReimuObject& p,FighterActor& owner,FighterActionEvents& events,i16 action,float size,i16 child,i16 loop){
    if(owner.animation.cursor.action==action){p.motion.x=owner.motion.x;p.motion.y=owner.motion.y;}
    if(p.animation.cursor.index<6&&owner.animation.cursor.action!=action){p.destroy();return;}
    if(start(p)){p.begin_pose();scale(p,size);counters(p);}if(p.animation.cursor.index==6)fade(p,7);if(end(p))p.destroy();if(at(p,6))spawn(owner,events,p,child);
}
void pillar_body(ReimuObject& p,FighterActor& owner,float size,i16 action,i16 limit,i16 loop){
    if(start(p)&&p.angle==0){active(p,size,99);p.counter=p.timer=0;p.angle=1;}
    if(owner.animation.cursor.action==action){p.motion.x=owner.motion.x;p.motion.y=owner.motion.y;}
    if((owner.animation.cursor.action==action&&owner.animation.cursor.index<12)||p.animation.cursor.index>limit){
        if(p.response!=0&&((p.counter=add(p.counter,1))>2)){p.counter=0;p.response=0;}
        if(p.animation.cursor.index>limit){if(alpha(p)<5){alpha(p,0);p.destroy();return;}alpha(p,u8(alpha(p)-5));}
        if(end(p))p.frame(loop);else if(at(p,loop))p.frame(0);
    }else p.frame(add(p.animation.cursor.index,loop));
}
}
bool advance_reimu_object_basic(ReimuObject& p,FighterActor& owner,FighterActor& opponent,ReimuState& state,ReplayStreams& random,FighterActionEvents& events){
    const auto index=[&](){return p.animation.cursor.index;};
    switch(p.animation.cursor.action){
    case 800:case 834:{
        const bool arrow=p.animation.cursor.action==834;if(outside(p)||p.response==5){p.destroy();return true;}if(cancel_on_hurt(p,owner,events,arrow?1:3,4))return true;
        if(start(p)){active(p,1,1);p.counter=10;p.timer=p.cycle=0;p.angle=i16(u16(i32(p.parameters[0])));if(arrow)p.polar(float(p.angle),float(double(p.counter)*.1f+10));}
        if(index()>(arrow?0:1)){p.timer=aim(p,opponent,true,false);if(p.timer>i32(p.angle)+10)p.angle=add(p.angle,arrow?1:2);if(p.timer<i32(p.angle)-10)p.angle=add(p.angle,arrow?-1:-2);p.angle=std::clamp<i16>(p.angle,-90,90);}
        p.polar(float(p.angle),float(double(p.counter)*.1f+(arrow?10:6)));if(arrow)p.pose.rotation[2]=degrees(p.motion.velocityY,p.motion.velocityX);p.cycle=add(p.cycle,30);p.counter=add(p.counter,2);
        if(!arrow&&index()>0&&p.timer<60){if(double(opponent.motion.y)+100<p.motion.y)p.parameters[0]=float(double(p.parameters[0])+2);if(p.motion.y<double(opponent.motion.y)+100)p.parameters[0]=float(double(p.parameters[0])-2);p.timer=add(p.timer,1);}
        move(p);if(!arrow)p.pose.rotation[2]=float(double(p.pose.rotation[2])+8);if(p.response!=0&&p.response!=6)p.frame(arrow?2:3);if(index()>(arrow?1:2))fade(p,25);if(end(p))p.destroy();return true;
    }
    case 801:case 802:case 831:{
        const i16 action=p.animation.cursor.action;if(p.response==5){p.destroy();return true;}if(action==831&&cancel_on_hurt(p,owner,events,3,6))return true;if(p.motion.x>1380||p.motion.x< -100){p.destroy();return true;}
        if(start(p)){for(i32 i=0;i<4;i++)if(p.parameters[0]==float(i)){p.begin_pose();p.motion.velocityX=action==802?(i%2==0?25:22):(i%2==0?11:8);p.motion.velocityY=i%2==0?(action==831?3:5):6;if(i>=2)p.motion.velocityY=-p.motion.velocityY;p.pose.rotation[2]=-degrees(p.motion.velocityY,p.motion.velocityX);}}
        move(p,1);if(end(p)){if(action==831)spawn(owner,events,p,832);p.destroy();}if(action!=831&&at(p,1)){p.motion.velocityX=p.motion.velocityY=0;if(p.parameters[0]==0)spawn(owner,events,805,float(double(p.motion.x)-i32(p.motion.facing)*30),p.parameters[1],p.motion.facing);}return true;
    }
    case 804:
        if(p.response==5){p.destroy();return true;}if(p.response==6){p.response=0;p.hits=1;}if(start(p)){p.counter=0;active(p,1.5f,1);}if(p.response==4&&p.counter<6){p.hits=1;p.counter=add(p.counter,1);p.response=0;}if(p.hits==0&&index()==1){p.frame(2);return true;}if(index()>1&&alpha(p)>10)alpha(p,u8(alpha(p)-10));if(end(p))p.destroy();return true;
    case 805:
        if(start(p)){p.begin_pose();scale(p,1);}p.pose.scale[0]=float(double(p.pose.scale[0])+.2f);p.pose.scale[1]=float(double(p.pose.scale[1])+.2f);if(alpha(p)>20&&index()>0)alpha(p,u8(alpha(p)-20));if(end(p))p.destroy();if(at(p,1)){spawn(owner,events,p,804);p.begin_pose();scale(p,8);p.pose.rotation[1]=60;}return true;
    case 807:case 809:case 810:case 828:case 829:case 830:{const auto action=p.animation.cursor.action;orb(p,owner,opponent,random,events,action==807||action==828?821:action==809||action==829?822:823,action>=828);return true;}
    case 808:
        p.motion.facing=owner.motion.facing;if(owner.animation.cursor.action==305&&owner.animation.cursor.index<4&&index()==0){p.motion.x=float(double(owner.motion.x)+i32(p.motion.facing)*145);p.motion.y=float(double(owner.motion.y)+66);}else if(index()!=1)p.frame(1);
        if(start(p)&&p.counter==0){p.begin_pose();scale(p,1);p.pose.rotation[1]=float(i32(p.motion.facing)*70);p.pose.rotation[0]=15;p.counter=1;}p.pose.rotation[2]=float(double(p.pose.rotation[2])+5);if(index()>0)fade(p,30);if(end(p))p.destroy();if(at(p,1))p.frame(0);return true;
    case 811:case 827:{
        const bool rebound=p.animation.cursor.action==827;if(p.response==5){p.destroy();return true;}if(cancel_on_hurt(p,owner,events,3,2,rebound))return true;
        if(start(p)){active(p,1,1);p.polar(p.parameters[0],p.parameters[1]);p.counter=0;p.pose.rotation[2]=degrees(p.motion.velocityY,p.motion.velocityX);}p.counter=add(p.counter,1);
        if(!rebound||index()<3)p.polar(p.parameters[0],!rebound?float(double(p.counter)*.1f+p.parameters[1]):p.parameters[1]);
        if(rebound&&(p.response==1||p.response==2||p.response==4)&&index()==1){if(p.response==1)p.frame(2);else{p.frame(3);p.motion.velocityX=-8;p.motion.velocityY=-10;}return true;}
        if(!rebound&&p.response!=0&&index()==1){p.frame(2);return true;}move(p);if(outside(p,-100,1380,-120,640)){p.destroy();return true;}
        if((!rebound&&index()>1)||(rebound&&index()==2)){if(rebound&&alpha(p)<25){alpha(p,0);p.destroy();return true;}fade(p,25);}if(rebound&&index()==3){p.pose.rotation[2]=float(double(p.pose.rotation[2])+30);p.motion.velocityY=float(double(p.motion.velocityY)+.5);}if(end(p))p.destroy();return true;
    }
    case 812:
        if(p.response==5){p.destroy();return true;}if(cancel_on_hurt(p,owner,events,3,8))return true;if(outside(p)){p.destroy();return true;}
        if(start(p)){active(p,1,4);counters(p);p.angle=i16(u16(i32(p.parameters[0])));}p.polar(p.parameters[0],float(double(p.timer)*.1f+10));p.cycle=add(p.cycle,30);p.timer=add(p.timer,1);
        if(index()>0&&p.angle<90){if(double(opponent.motion.y)+100<p.motion.y)p.parameters[0]=float(double(p.parameters[0])+1);if(p.motion.y<double(opponent.motion.y)+100)p.parameters[0]=float(double(p.parameters[0])-1);p.angle=add(p.angle,1);}move(p,1);p.pose.rotation[2]=float(double(p.pose.rotation[2])+12);
        if((p.response==1||p.response==2||p.response==4)&&p.hits==0)p.frame(3);if(index()>2)fade(p,25);if(end(p))p.destroy();if(at(p,3)&&p.counter<12&&p.hits>0){p.frame(2);p.counter=add(p.counter,1);p.response=0;}return true;
    case 813:
        if(owner.animation.cursor.action==506||owner.animation.cursor.action==507){p.motion.x=owner.motion.x;p.motion.y=owner.motion.y;}if(owner.animation.cursor.action!=506||owner.combat.freeze<1)if(end(p))p.destroy();return true;
    case 814:
        if(owner.animation.cursor.action==704){p.motion.x=float(double(owner.motion.x)+i32(p.motion.facing)*228);p.motion.y=float(double(owner.motion.y)+93);}if(index()<57&&(owner.animation.cursor.action!=704||owner.animation.cursor.index>12)){scale(p,3);spawn(owner,events,p,815);p.frame(57);return true;}
        if(start(p)){active(p,2,99);counters(p);}p.pose.rotation[2]=float(double(p.pose.rotation[2])-2);if(p.response!=0&&((p.counter=add(p.counter,1))>5)){p.counter=0;p.response=0;}if(index()>56){p.pose.scale[0]=float(double(p.pose.scale[0])*.8f);p.pose.scale[1]=float(double(p.pose.scale[1])*.8f);}if(index()==58)fade(p,15);if(end(p))p.destroy();if(at(p,57)&&owner.animation.cursor.action==704&&owner.animation.cursor.index<13)p.frame(27);return true;
    case 815:expanding(p,1,.3f,15);return true;
    case 816:pillar(p,owner,events,702,4,817,10);return true;
    case 817:pillar_body(p,owner,4,702,9,10);return true;
    case 818:
        state.commands.activeSeals=1;if(owner.animation.cursor.action==705&&index()<19){p.motion.x=float(double(owner.motion.x)+i32(p.motion.facing)*228);p.motion.y=float(double(owner.motion.y)+93);}if(start(p)){active(p,6,999);counters(p);p.variant=0;p.motion.velocityX=p.motion.velocityY=0;}
        if(index()>19){p.pose.rotation[2]=float(double(p.pose.rotation[2])-4);p.motion.velocityX=float(double(p.motion.velocityX)+.2f);if(owner.combat.spirit<6){p.variant=1;consume_reimu_spirit(owner,add(owner.combat.spirit,-1),events);}else consume_reimu_spirit(owner,4,events);if(p.variant==1){p.hits=0;if(alpha(p)<11){p.destroy();state.commands.activeSeals=0;return true;}alpha(p,u8(alpha(p)-10));}}
        if(p.response!=0){p.timer=10;p.counter=add(p.counter,1);if((p.response==6||p.response==4)&&p.counter<2)p.counter=2;if(p.counter>2){p.counter=0;p.response=0;}}if(p.timer>0){p.timer=add(p.timer,-1);p.motion.velocityX=2;}move(p,1);if(p.motion.x< -320||p.motion.x>1600){p.destroy();state.commands.activeSeals=0;return true;}if(end(p))p.frame(40);if(at(p,19)){p.motion.velocityX=2;p.motion.velocityY=0;}return true;
    case 819:pillar(p,owner,events,703,6,820,22);return true;
    case 820:pillar_body(p,owner,6,703,21,22);return true;
    case 821:case 822:case 823:petal(p);return true;
    case 824:case 825:case 826:{const i32 type=p.animation.cursor.action-824;if(p.response==5){p.destroy();return true;}if(cancel_on_hurt(p,owner,events,3,8,true))return true;
        if(start(p)){active(p,2,3);p.pose.rotation[0]=75;p.motion.velocityX=float(20*(type+1));p.counter=0;}if(p.response==4)p.response=0;if(p.hits==0&&index()==2){p.frame(3);return true;}p.pose.rotation[2]=float(double(p.pose.rotation[2])+10);if(p.pose.rotation[2]>360)p.pose.rotation[0]=float(double(p.pose.rotation[0])-360);if(index()<3){p.motion.velocityX=float(double(p.motion.velocityX)-(type+1));if(p.motion.velocityX<.5f)p.motion.velocityX=.5f;}
        if(index()==3){alpha(p,u8(alpha(p)-10));if(alpha(p)==0){p.destroy();return true;}}p.motion.x=float(double(p.motion.x)+i32(p.motion.facing)*double(p.motion.velocityX));if(end(p))p.destroy();if(at(p,3)&&p.counter<6){p.counter=add(p.counter,1);p.frame(2);p.response=0;}return true;}
    case 832:
        if(p.response==5){p.destroy();return true;}if(cancel_on_hurt(p,owner,events,3,6))return true;if(start(p)){active(p,1,2);p.counter=p.timer=p.angle=0;p.motion.velocityX=1;p.motion.velocityY=0;}
        if(double(opponent.motion.y)+100<p.motion.y)p.motion.velocityY=float(double(p.motion.velocityY)-.02f);if(p.motion.y<=double(opponent.motion.y)+100)p.motion.velocityY=float(double(p.motion.velocityY)+.02f);p.motion.velocityY=std::clamp(p.motion.velocityY,-2.f,2.f);p.motion.velocityX=float(double(p.motion.velocityX)+.05f);
        if(p.counter<1){p.response=0;p.counter=10;}p.counter=add(p.counter,-1);move(p,1);if(p.animation.cursor.elapsed==0){const float centerY=float(double(p.motion.y)-10);const float y=float(double(centerY)+random.range(0,20));const float centerX=float(double(p.motion.x)-10);const float x=float(double(centerX)+random.range(0,20));spawn(owner,events,833,x,y,p.motion.facing);p.timer=i16(random.range(0,20));p.pose.scale[0]=p.pose.scale[1]=float(double(p.timer)*.01f+.9f);}if(end(p)){if(p.angle<60&&p.hits>0){p.frame(1);p.angle=add(p.angle,1);}else p.destroy();}return true;
    case 833:
        p.motion.facing=owner.motion.facing;if(start(p)&&p.counter==0){p.begin_pose();scale(p,.8f);p.counter=1;p.motion.velocityY=4;p.motion.velocityX=0;}p.motion.velocityY=float(double(p.motion.velocityY)-.1f);move(p,1);fade(p,13);p.pose.scale[0]=float(double(p.pose.scale[0])*.94f);p.pose.scale[1]=float(double(p.pose.scale[1])*.94f);if(end(p))p.destroy();return true;
    default:return false;
    }
}
}
