#include "PlayerOptions.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th08 {
namespace {
float value(u32 bits){float result;std::memcpy(&result,&bits,4);return result;}
void add(Vec3& a,const Vec3& b){a.x=Scalar::add(a.x,b.x);a.y=Scalar::add(a.y,b.y);a.z=Scalar::add(a.z,b.z);}
Vec3 subtract(const Vec3& a,const Vec3& b){return {Scalar::sub(a.x,b.x),Scalar::sub(a.y,b.y),Scalar::sub(a.z,b.z)};}
void scale(Vec3& a,float x){a.x=Scalar::mul(a.x,x);a.y=Scalar::mul(a.y,x);a.z=Scalar::mul(a.z,x);}
void follow(Vec3& center,const Vec3& target,float rate){auto delta=subtract(target,center);scale(delta,rate);add(delta,center);center=delta;}
void orbit(PlayerOption& option,float radius){option.position.x=(cosine(option.angle)*number(radius)).to_float();option.position.y=(sine(option.angle)*number(radius)).to_float();}
Vec3 remilia_center(Vec3 position,i32 index){static const Vec2 offsets[]{{-30,-16},{-10,-32},{10,-32},{30,-16}};if(index>=0&&index<4){position.x=Scalar::add(position.x,offsets[index].x);position.y=Scalar::add(position.y,offsets[index].y);}return position;}
}
void PlayerOptions::initialize(PlayerOption* slots,u8 character){
    static const PlayerOptionKind table[12][4]{
        {PlayerOptionKind::Yukari},{PlayerOptionKind::Alice},
        {PlayerOptionKind::RemiliaTeam,PlayerOptionKind::RemiliaTeam,PlayerOptionKind::RemiliaTeam,PlayerOptionKind::RemiliaTeam},
        {PlayerOptionKind::Yuyuko,PlayerOptionKind::Yuyuko},{},{PlayerOptionKind::Yukari},{},{PlayerOptionKind::Alice},{},
        {PlayerOptionKind::RemiliaSolo,PlayerOptionKind::RemiliaSolo,PlayerOptionKind::RemiliaSolo,PlayerOptionKind::RemiliaSolo},
        {PlayerOptionKind::None,PlayerOptionKind::None,PlayerOptionKind::YoumuSolo},{PlayerOptionKind::Yuyuko,PlayerOptionKind::Yuyuko}
    };
    for(u32 i=0;i<4;++i){auto& option=slots[i];std::memset(&option,0,sizeof(option));option.update=character<12?table[character][i]:PlayerOptionKind::None;option.draw=option.update!=PlayerOptionKind::None;if(option.draw){option.state=1;option.timer.set(0);option.index=i;}}
}
void PlayerOptions::approach_yukari(PlayerOption& o,bool enemy){
    Vec3 target=enemy?context.enemy:context.player;target.y=Scalar::add(target.y,enemy?32:-96);if(target.y<32)target.y=32;
    auto desired=subtract(target,o.position);scale(desired,Scalar::div(1,16));auto acceleration=subtract(desired,o.velocity);scale(acceleration,.2f);add(o.velocity,acceleration);add(o.position,o.velocity);
    if(std::fabs(o.velocity.x)<.05f)o.velocity.x=0;
    if(!enemy){if(context.shooting_timer.current>=0&&context.enemy_present&&o.timer.current>=10){o.animation.SetInterrupt(3);o.pose=3;}else context.enemy_present=0;}
}
void PlayerOptions::turn_youmu(PlayerOption& o){
    static const float directions[]{0,1.5707963705062866f,-1.5707963705062866f,0,3.1415927410125732f,.7853981852531433f,2.356194496154785f,-.7853981852531433f,-2.356194496154785f};
    if(context.direction<=0||context.direction>8)return;float target=directions[context.direction];
    float difference=std::fabs(Scalar::sub(o.shot_angle,target));
    if(difference>3.1415927410125732f){target=Scalar::add(target,o.shot_angle<=target?-6.2831854820251465f:6.2831854820251465f);difference=std::fabs(Scalar::sub(o.shot_angle,target));}
    if(difference<=1.5707963705062866f)o.shot_angle=add_angle(((number(target)-number(o.shot_angle))*number(.07f)).to_float(),o.shot_angle);else o.shot_angle=target;
}
void PlayerOptions::update(PlayerOption& o,PlayerOptionKind kind){
    const auto animation=[&](i32 script){if(actions)actions->animation(o.animation,script);};
    const auto effect=[&](u32 color){if(actions)actions->effect(47,o.position,color);};
    const auto player_offset=[&](float y){o.position=context.player;o.position.y=Scalar::sub(o.position.y,y);};
    if(o.state==3){
        if(kind==PlayerOptionKind::Alice)player_offset(32);
        if(o.timer.current==0)o.animation.SetInterrupt(5);
        if(o.timer.current>16){o.state=0;o.update=PlayerOptionKind::None;o.draw=0;}return;
    }
    if(o.state!=1&&o.state!=2)return;
    if(kind==PlayerOptionKind::Yukari){
        if(o.state==1){animation(18);player_offset(96);if(o.position.y<32)o.position.y=32;o.state=2;context.enemy_present=0;return;}
        if(o.pose>=0&&o.pose<=2){const i32 pose=o.pose;approach_yukari(o,false);
            if(o.velocity.x==0&&pose!=0){o.animation.SetInterrupt(1);o.pose=0;if(o.animation.scale.x<0)o.animation.scale.x=-o.animation.scale.x;}
            else if(o.velocity.x<0&&pose!=1){o.animation.SetInterrupt(2);o.pose=1;if(o.animation.scale.x<0)o.animation.scale.x=-o.animation.scale.x;}
            else if(o.velocity.x>0&&pose!=2){o.animation.SetInterrupt(2);o.pose=2;if(o.animation.scale.x>0)o.animation.scale.x=-o.animation.scale.x;}
        }else if(o.pose==3){if(context.enemy_present)approach_yukari(o,true);if((context.shooting_timer.current<0&&!(context.buttons&1))||!context.enemy_present){context.enemy_present=0;o.animation.SetInterrupt(1);o.pose=0;}}
        return;
    }
    if(kind==PlayerOptionKind::Alice){if(o.state==1){animation(29);o.state=2;}if(!context.bomb)player_offset(32);return;}
    if(kind==PlayerOptionKind::RemiliaTeam||kind==PlayerOptionKind::RemiliaSolo){
        if(o.state==1){animation(24);o.state=2;o.center=remilia_center(context.player,o.index);if(o.index>=0&&o.index<4)o.angle=o.index&1?3.1415927410125732f:0;}
        if(o.timer.current>12&&o.index>=0&&o.index<4){static const u32 deltas[]{0x3cd67750,0xbd0efa35,0x3d0efa35,0xbcd67750};o.angle=add_angle(o.angle,value(deltas[o.index]));}
        if(kind==PlayerOptionKind::RemiliaSolo)o.animation.color1.d3dColor=i32(0xffff8080);orbit(o,8);
        if(kind==PlayerOptionKind::RemiliaSolo&&!context.focused){o.animation.color1.d3dColor=i32(0xff80ffff);follow(o.center,remilia_center(context.player,o.index),.2f);}
        add(o.position,o.center);if(kind==PlayerOptionKind::RemiliaSolo){o.position.z=0;effect(0x80602050);}return;
    }
    if(kind==PlayerOptionKind::Yuyuko){
        Vec3 target=context.player;if(o.state==1){animation(21);o.state=2;o.center=target;if(o.index==0)o.angle=0;else if(o.index==1)o.angle=-3.1415927410125732f;}
        if(o.index==0){target.x=Scalar::sub(target.x,32);o.angle=add_angle(o.angle,value(0x3d567750));}
        else if(o.index==1){target.x=Scalar::add(target.x,32);o.angle=add_angle(o.angle,value(0xbd567750));}
        orbit(o,6);follow(o.center,target,.09f);add(o.position,o.center);o.position.z=0;effect(0x80602050);return;
    }
    if(kind==PlayerOptionKind::YoumuTeam||kind==PlayerOptionKind::YoumuSolo){
        if(o.state==1){animation(21);o.state=2;o.center=context.history_last;o.angle=0;o.shot_angle=-1.5707963705062866f;}
        o.angle=add_angle(o.angle,value(0x3d567750));orbit(o,8);follow(o.center,context.history_last,.05f);add(o.position,o.center);o.position.z=0;
        if(kind==PlayerOptionKind::YoumuTeam){effect(0x80405080);turn_youmu(o);}
        else{ o.animation.color1.d3dColor=i32(0xffff8080);if(!context.focused){o.animation.color1.d3dColor=-1;if(!context.direction)return;turn_youmu(o);effect(0x80405080);}else effect(0xfff05080);}
    }
}
void PlayerOptions::draw(PlayerOption& o,const Vec2& offset){
    o.animation.pos={Scalar::add(offset.x,o.position.x),Scalar::add(offset.y,o.position.y),.49f};if(actions)actions->draw(o.animation);
}
}
