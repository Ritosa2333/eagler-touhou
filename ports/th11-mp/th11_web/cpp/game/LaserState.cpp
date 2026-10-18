#include "LaserState.hpp"
#include "BulletAppearance.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
void LaserState::initialize(const float* rate)noexcept{std::memset(this,0,sizeof *this);lifetime.set(0,rate);}
i32 LaserState::probe(Vec2 p,float radius)const noexcept{
    const float dx=float(double(p.x)-position.x),dy=float(double(p.y)-position.y);
    const float sine=float(std::sin(-double(angle))),cosine=float(std::cos(-double(angle)));
    const float x=float(double(dx)*cosine-double(dy)*sine),y=float(double(dy)*cosine+double(dx)*sine);
    return float(double(x)-radius)<=length&&float(double(y)-radius)<=double(width)*.5&&float(double(x)+radius)>=0&&float(double(y)+radius)>=-double(width)*.5?2:0;
}
i32 laser_line_transforms(LaserLine& l,LaserWorld& w)noexcept{
    auto& b=l.base;auto& p=l.parameters;
    const auto centered=[&](){return b.transform_index==0||p.transform_sound<0||w.sound(p.transform_sound,0,false);};
    for(u32 budget=0;b.transform_index<18;++budget){
        if(budget>=4096||b.transform_index<0)return -2;
        auto& t=p.transforms[b.transform_index];const u32 op=t.opcode;
        if(!op||(!t.concurrent&&b.active_transforms))return 0;
        switch(op){
        case 1:b.active_transforms|=1;b.motion[0].timer.set(0,&w.rate);b.motion[0].vector.z=0;break;
        case 4:{auto& s=b.motion[1];b.active_transforms|=4;s.a=t.a;s.b=t.b<=-990?b.angle:t.b>=990?bullet_aim(b.position,w.player):t.b;s.timer.set(0,&w.rate);s.duration=t.c;const auto v=polar(s.b,s.a);s.vector.x=v.x;s.vector.y=v.y;if(!centered())return -2;break;}
        case 8:{auto& s=b.motion[2];b.active_transforms|=8;s.a=t.a;s.b=t.b;s.timer.set(0,&w.rate);s.duration=t.c;if(!centered())return -2;break;}
        case 0x10:case 0x20:case 0x40:{auto& s=b.motion[3];b.active_transforms|=op;s.b=t.a;s.a=t.b<=-999?b.speed:t.b;s.timer.set(0,&w.rate);s.duration=t.c;s.limit=t.d;s.count=0;break;}
        case 0x100:if(t.c>0){auto& s=b.motion[4];b.active_transforms|=op;s.a=t.a>=0?t.a:b.speed;--t.c;s.limit=t.c;s.duration=0;s.count=t.d;}break;
        case 0x200:b.protection=t.c;break;
        case 0x800:if(u32(t.c)>=bullet_appearances.size()||!w.change_appearance(l,wrapping_add(bullet_appearances[t.c].script,t.d)))return -2;break;
        case 0x1000:b.active_transforms|=op;b.motion[5].timer.set(t.c,&w.rate);break;
        case 0x2000:b.state=3;break;
        case 0x4000:if(!w.sound(t.c,b.position.x,true))return -2;break;
        case 0x20000:case 0x40000:b.active_transforms|=op;b.motion[6].timer.set(t.c,&w.rate);break;
        case 0x200000:b.id=t.c;break;
        case 0x400000:b.transform_index=t.c;continue;
        case 0x800000:{auto& s=b.motion[8];b.active_transforms|=op;s.a=t.a;s.b=t.b;s.timer.set(0,&w.rate);s.duration=t.c;break;}
        default:break;
        }
        ++b.transform_index;
    }return 0;
}
i32 laser_line_motion(LaserLine& l,u32 kind,LaserWorld& w)noexcept{
    auto& b=l.base;auto& p=l.parameters;
    const auto velocity=[&](float speed){const auto v=polar(b.angle,speed);b.velocity.x=v.x;b.velocity.y=v.y;};
    if(kind==4){auto& s=b.motion[1];if(s.timer.current<s.duration){
        b.speed=float(double(s.a)*w.rate+b.speed);
        b.velocity.x=float(double(b.velocity.x)+float(double(s.vector.x)*w.rate));
        b.velocity.y=float(double(b.velocity.y)+float(double(s.vector.y)*w.rate));
        b.velocity.z=float(double(b.velocity.z)+float(double(s.vector.z)*w.rate));
        if(std::abs(double(b.velocity.x))>.0001||std::abs(double(b.velocity.y))>.0001)b.angle=float(std::atan2(double(b.velocity.y),double(b.velocity.x)));
    }else b.active_transforms&=~4u;s.timer.tick();}
    else if(kind==0x10){auto& s=b.motion[3];float speed;
        if(s.timer.current<s.duration)speed=float(double(b.speed)-double(s.timer.fractional)*b.speed/s.duration);
        else {if(p.transform_sound>=0&&!w.sound(p.transform_sound,0,false))return -2;s.count=wrapping_add(s.count,1);if(s.count>=s.limit)b.active_transforms&=~0x10u;b.angle=float(double(s.b)+b.angle);speed=b.speed=s.a;s.timer.set(0,&w.rate);}
        velocity(speed);s.timer.tick();
    }else if(kind==0x100){auto& s=b.motion[4];const auto v=polar(b.angle,b.length);const Vec3 tip{float(double(b.position.x)+v.x),float(double(b.position.y)+v.y),0};
        if(tip.x>-192&&tip.x<192&&tip.y>0&&tip.y<448)return 0;
        bool reflected=false;const auto spawn=[&](Vec3 pos,float angle){p.position=pos;p.angle=angle;p.speed=s.a;reflected=true;return w.spawn_line(p);};
        if((s.count&1)&&tip.y<0&&!spawn({tip.x,-tip.y,0},-b.angle))return -2;
        if((s.count&2)&&tip.y>448&&!spawn({tip.x,float(896-double(tip.y)),0},-b.angle))return -2;
        if((s.count&4)&&tip.x<-192&&!spawn({float(-double(tip.x)-384),tip.y,0},normalize_angle(float(-double(b.angle)-3.1415927410125732421875))))return -2;
        if((s.count&8)&&tip.x>192&&!spawn({float(384-double(tip.x)),tip.y,0},normalize_angle(float(-double(b.angle)-3.1415927410125732421875))))return -2;
        if(reflected){b.active_transforms&=~0x100u;if(p.transform_sound>=0&&!w.sound(p.transform_sound,0,false))return -2;}
    }else if(kind==0x1000){auto& t=b.motion[5].timer;if(t.current<1)b.active_transforms^=kind;else t.advance(-1);}
    else if(kind!=1&&kind!=8&&kind!=0x20&&kind!=0x40&&kind!=0x20000&&kind!=0x40000&&kind!=0x800000)return -2;
    // These virtual slots are RET in the original line-laser vtable.
    return 0;
}
i32 laser_line_cancel(LaserLine& l,bool reward,bool skip_protected,LaserWorld& w)noexcept{
    auto& b=l.base;if(skip_protected&&b.protection)return 0;
    const auto v=polar(b.angle,8);Vec3 pos{float(double(v.x)+b.position.x),float(double(v.y)+b.position.y),float(double(b.position.z)+0)};
    const Vec2 step{float(double(v.x)+v.x),float(double(v.y)+v.y)};i32 count=0;float distance=8;
    if(b.length>16)do{
        if(++count>1000000)return -2;
        if(!w.cancel_effect(pos,l.parameters.color*2+2))return -2;
        if(reward&&double(pos.x)+32>-192&&double(pos.x)-32<192&&double(pos.y)+32>0&&double(pos.y)-32<448&&!w.cancel_reward(pos))return -2;
        pos.x=float(double(pos.x)+step.x);pos.y=float(double(pos.y)+step.y);distance=float(double(distance)+16);
    }while(double(distance)+8<b.length);
    b.state=1;return count;
}
}
