#include "LaserState.hpp"
#include "BulletAppearance.hpp"
#include "AnmRenderer.hpp"
#include "Movement.hpp"
#include <cmath>
namespace th11 {
namespace {
i32 sprite_callback(AnmVm& vm,i32 frame){
    auto& l=*reinterpret_cast<LaserInfinite*>(reinterpret_cast<u8*>(&vm)-offsetof(LaserInfinite,body));
    const auto& a=bullet_appearances[l.base.sprite];if(a.sprites[0]<0)return frame;
    const i32 index=l.base.color*3+frame;return index>=0&&index<48?a.sprites[index]:-1;
}
}
bool laser_infinite_initialize(LaserInfinite& l,const LaserInfiniteParameters& parameters,AnmResource& resource,AnmEnvironment& animations,LaserWorld& w,u16 file_id){
    auto& b=l.base;l.parameters=parameters;b.state=3;b.type=1;b.sprite=parameters.sprite;b.color=parameters.color;
    if(u32(b.sprite)>=bullet_appearances.size()||b.color<0||b.color>=16)return false;
    auto& body=l.body;if(!body.bind_script(resource,bullet_appearances[b.sprite].script,file_id,&animations.rate))return false;
    body.sprite_callback=sprite_callback;body.reserved_430=u32(uintptr_t(&l));
    if(body.update(animations)<0)return false;body.pending_interrupt=2;if(body.update(animations)<0)return false;
    if(parameters.flags&2)body.flags=(body.flags&0xffffff9f)|0x10;
    body.flags=(body.flags&0xfc63ffff)|0x600000;
    auto& origin=l.origin;if(!origin.bind_script(resource,b.color+47,file_id,&animations.rate)||origin.update(animations)<0)return false;
    origin.pending_interrupt=2;if(origin.update(animations)<0)return false;
    origin.flags=(origin.flags&0xffffff9f)|0x10;origin.flags=(origin.flags&0xfc7fffff)|0x400000;
    if(parameters.start_sound>=0&&!w.sound(parameters.start_sound,0,true))return false;
    b.length=parameters.initial_length;b.width=2;b.position=parameters.position;b.speed=parameters.speed;b.angle=parameters.angle;l.reserved_644=0;
    if(parameters.flags&4)b.id=parameters.id;return true;
}
i32 laser_infinite_update(LaserInfinite& l,AnmEnvironment& animations,LaserWorld& w){
    auto& b=l.base;auto& p=l.parameters;
    if(b.length<p.max_length){b.length=float(double(b.speed)*w.rate+b.length);if(b.length>p.max_length)b.length=p.max_length;}
    b.angle=normalize_angle(float(double(b.angle)+float(double(p.angular_velocity)*w.rate)));
    if(p.flags&1){Vec3 boss;if(w.boss_position(boss))b.position=boss;}
    b.position.x=float(double(b.position.x)+float(double(p.velocity.x)*w.rate));
    b.position.y=float(double(b.position.y)+float(double(p.velocity.y)*w.rate));
    b.position.z=float(double(b.position.z)+float(double(p.velocity.z)*w.rate));
    switch(b.state){
    case 3:if(b.lifetime.current>=p.warning_frames){b.lifetime.set(0,&w.rate);b.state=4;}break;
    case 4:
        if(b.lifetime.current<p.expand_frames){b.width=float(double(p.width)*b.lifetime.fractional/p.expand_frames);break;}
        b.lifetime.set(0,&w.rate);b.width=p.width;b.state=2;[[fallthrough]];
    case 2:if(b.lifetime.current<p.active_frames)break;b.lifetime.set(0,&w.rate);b.state=5;[[fallthrough]];
    case 5:if(b.lifetime.current>=p.shrink_frames)return 1;b.width=float(double(p.width)-double(b.lifetime.fractional)*p.width/p.shrink_frames);break;
    default:break;
    }
    if((b.state==4||b.state==2)&&b.length>16){
        const auto v=polar(b.angle,float(double(b.length)/10));const Vec3 start{float(double(b.position.x)+v.x),float(double(b.position.y)+v.y),b.position.z};
        const float width=b.width>=32?float(double(b.width)-(double(b.width)+16)/3):float(double(b.width)*.5);
        const i32 hit=w.collision(start,b.angle,width,float(double(b.length)*4/5));if(hit<0)return -2;
        if(hit==1){if(!w.cut_infinite(l,w.player,{32,32,0},false,true))return -2;}
        else if(hit==2&&b.lifetime.current%3==0){if(!w.graze_effect(w.player)||!w.graze_reward()||!w.sound(28,b.position.x,true)||!w.graze_register())return -2;}
    }
    if(!l.body.sprite)return -2;
    l.body.scale.x=float(double(b.width)/l.body.sprite->width);l.body.scale.y=float(double(b.length)/l.body.sprite->height);l.body.flags|=8;
    if(l.body.update(animations)<0)return -2;if(b.offset==0&&l.origin.update(animations)<0)return -2;return 0;
}
bool laser_infinite_draw(LaserInfinite& l,AnmRenderer& renderer){
    auto& b=l.base;l.body.script_position={float((double(b.position.x)+32)+192),float(double(b.position.y)+16),b.position.z};
    l.body.rotation.z=normalize_angle(float(double(b.angle)+1.57079637050628662109375));l.body.flags|=4;
    if(renderer.draw(l.body)==-2)return false;
    if(b.offset==0){l.origin.script_position={float((double(b.position.x)+32)+192),float(double(b.position.y)+16),b.position.z};if(renderer.draw(l.origin)==-2)return false;}return true;
}
bool laser_infinite_warning(LaserInfinite& l,float angular_delta,LaserWorld& w){
    auto& b=l.base;auto& p=l.parameters;if(b.type!=1||b.state!=3)return true;
    const auto v=polar(b.angle,float(double(b.length)/10));
    const Vec3 start{float(double(b.position.x)+v.x),float(double(b.position.y)+v.y),float(double(b.position.z)+0)};
    p.angle=normalize_angle(float(double(p.angle)-angular_delta));
    const i32 hit=w.warning_collision(start,b.angle,5,b.length);if(hit<0)return false;
    if(hit==1||b.lifetime.current>=wrapping_sub(p.warning_frames,60)){l.body.flags|=0x8000;l.body.secondary_color=0xffff4040;}
    else{
        l.body.flags&=~0x8000u;l.reserved_644=0;
        double difference=double(p.angle)-b.angle;
        if(difference>3.1415927410125732421875)difference=double(p.angle)-(double(b.angle)+6.283185482025146484375);
        else if(double(b.angle)-p.angle>3.1415927410125732421875)difference=double(p.angle)-(double(b.angle)-6.283185482025146484375);
        difference=float(difference);
        if(std::abs(difference)>=0.04908738657832146)difference*=0.10000000149011612;
        b.angle=normalize_angle(float(difference+b.angle));
    }return true;
}
}
