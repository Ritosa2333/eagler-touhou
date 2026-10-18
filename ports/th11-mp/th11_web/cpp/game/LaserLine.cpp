#include "LaserState.hpp"
#include "BulletAppearance.hpp"
#include "AnmRenderer.hpp"
#include "Movement.hpp"
namespace th11 {
namespace {
i32 sprite_callback(AnmVm& vm,i32 frame){
    auto& l=*reinterpret_cast<LaserLine*>(reinterpret_cast<u8*>(&vm)-offsetof(LaserLine,body));
    const auto& a=bullet_appearances[l.base.sprite];if(a.sprites[0]<0)return frame;
    const i32 index=l.base.color*3+frame;return index>=0&&index<48?a.sprites[index]:-1;
}
bool outside(Vec3 p,float width){return !(double(p.x)+width>-192&&double(p.x)-width<192&&double(p.y)+width>0&&double(p.y)-width<448);}
}
bool laser_line_initialize(LaserLine& l,const LaserLineParameters& parameters,AnmResource& resource,AnmEnvironment& animations,LaserWorld& world,u16 file_id){
    auto& b=l.base;l.parameters=parameters;b.state=2;b.type=0;b.sprite=parameters.sprite;b.color=parameters.color;
    if(u32(b.sprite)>=bullet_appearances.size()||b.color<0||b.color>=16)return false;
    auto& body=l.body;
    if(!body.bind_script(resource,bullet_appearances[b.sprite].script,file_id,&animations.rate))return false;
    body.sprite_callback=sprite_callback;body.reserved_430=u32(uintptr_t(&l));
    if(body.update(animations)<0)return false;body.pending_interrupt=2;if(body.update(animations)<0)return false;
    if(parameters.flags&1)body.flags=(body.flags&0xffffff9f)|0x10;
    body.flags=(body.flags&0xfc63ffff)|0x600000;
    auto& origin=l.origin;
    if(!origin.bind_script(resource,b.color+47,file_id,&animations.rate)||origin.update(animations)<0)return false;
    origin.pending_interrupt=2;if(origin.update(animations)<0)return false;
    origin.flags=(origin.flags&0xffffff9f)|0x10;origin.flags=(origin.flags&0xfc7fffff)|0x400000;
    b.offscreen_timer.set(30,&world.rate);
    if(parameters.start_sound>=0&&!world.sound(parameters.start_sound,0,true))return false;
    b.graze_timer.set(0,&world.rate);b.length=parameters.initial_length;b.position=parameters.position;b.width=parameters.width;b.speed=parameters.speed;b.angle=parameters.angle;b.offset=parameters.initial_length>0?.01f:0;
    const auto v=polar(b.angle,b.speed);b.velocity.x=v.x;b.velocity.y=v.y;return true;
}
bool laser_line_appearance(LaserLine& l,i32 script,AnmResource& resource,AnmEnvironment& animations,u16 file_id){
    return l.body.bind_script(resource,script,file_id,&animations.rate)&&l.body.update(animations)>=0;
}
i32 laser_line_update(LaserLine& l,AnmEnvironment& animations,LaserWorld& w){
    auto& b=l.base;auto& p=l.parameters;
    if(laser_line_transforms(l,w)<0)return -2;
    if(b.active_transforms){
        constexpr u32 order[]={1,4,8,0x10,0x40,0x20,0x100,0x20000,0x40000,0x800000,0x1000};
        for(const auto op:order)if((b.active_transforms&op)&&laser_line_motion(l,op,w)<0)return -2;
        if(b.protection)b.protection=wrapping_sub(b.protection,1);
    }
    if(p.growth_limit<=b.length){
        b.offset=float(double(b.speed)*w.rate+b.offset);
        b.position.x=float(double(b.position.x)+float(double(b.velocity.x)*w.rate));
        b.position.y=float(double(b.position.y)+float(double(b.velocity.y)*w.rate));
        b.position.z=float(double(b.position.z)+float(double(b.velocity.z)*w.rate));
        if(p.end_distance>0&&p.end_distance<double(b.length)+b.offset){b.length=float(double(p.end_distance)-b.offset);p.growth_limit=b.length;if(b.length<=0)return 1;}
    }else {b.length=float(double(b.speed)*w.rate+b.length);if(p.growth_limit<b.length)b.length=p.growth_limit;}
    if(b.offscreen_timer.current<1){const auto v=polar(b.angle,b.length);const Vec3 end{float(double(b.position.x)+v.x),float(double(b.position.y)+v.y),b.position.z};if(outside(b.position,b.width)&&outside(end,b.width))return 1;}
    else b.offscreen_timer.advance(-1);
    if(b.length>16&&b.width>3){
        const auto v=polar(b.angle,float(double(b.length)/10));const Vec3 start{float(double(b.position.x)+v.x),float(double(b.position.y)+v.y),b.position.z};
        const float width=b.width>=32?float(double(b.width)-(double(b.width)+16)*.5):float(double(b.width)*.5);
        const i32 hit=w.collision(start,b.angle,width,float(double(b.length)*4/5));if(hit<0)return -2;
        if(hit==1){if(!w.cut(l,w.player,{32,32,0},false,true))return -2;}
        else if(hit==2){if(b.graze_timer.current%3==0){if(!w.graze_effect(w.player)||!w.sound(28,b.position.x,true)||!w.graze_reward()||!w.graze_register())return -2;}b.graze_timer.tick();}
    }
    if(!l.body.sprite)return -2;
    l.body.scale.x=float(double(b.width)/l.body.sprite->width);l.body.scale.y=float(double(b.length)/l.body.sprite->height);l.body.flags|=8;
    if(l.body.update(animations)<0)return -2;if(b.offset==0&&l.origin.update(animations)<0)return -2;return 0;
}
bool laser_line_draw(LaserLine& l,AnmRenderer& renderer){
    auto& b=l.base;l.body.script_position={float((double(b.position.x)+32)+192),float(double(b.position.y)+16),b.position.z};
    l.body.rotation.z=normalize_angle(float(double(b.angle)+1.57079637050628662109375));l.body.flags|=4;
    if(renderer.draw(l.body)==-2)return false;
    if(b.offset==0){l.origin.script_position={float((double(b.position.x)+32)+192),float(double(b.position.y)+16),b.position.z};if(renderer.draw(l.origin)==-2)return false;}return true;
}
}
