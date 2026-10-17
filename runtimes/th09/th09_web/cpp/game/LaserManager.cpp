#include "LaserManager.hpp"
#include "Arithmetic.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th09 {
Laser* LaserManager::create(const BulletEmission& e,const Vec3& player,i32 cancel_frames){
    if(cancel_frames&&!(e.flags&4))return &pool[0];
    u32 index=0;while(index<capacity&&pool[index].active)++index;if(index==capacity)return &pool[capacity];
    const i32 script=e.sprite+10;if(script<0||u32(script)>=file.scriptCount||e.color<0||e.color>=16)return nullptr;
    static constexpr i32 glow_colors[]={0,1,1,1,1,2,2,2,2,3,3,3,4,4,4,0};
    auto& l=pool[index];l.body.scriptIndex=i16(script);executor.start(file,l.body,file.scripts[script]);
    if(file.SetSprite(&l.body,l.body.activeSpriteIndex+e.color))return nullptr;
    l.glow.Initialize();l.glow.anmFile=&file;if(file.SetSprite(&l.glow,glow_colors[e.color]+146))return nullptr;l.glow.blendMode=1;
    l.position=e.position;l.color=e.color;l.active=1;l.direction=e.angle;
    if(e.pattern==0){const float dx=player.x-e.position.x,dy=player.y-e.position.y;
        const double aim=(dx==0&&dy==0)?double(1.5707963705062866f):std::atan2(double(dy),double(dx));l.direction=float(aim+double(l.direction));}
    l.flags=u16(e.flags);l.time.reset();l.start_offset=e.laser.start_offset;l.end_offset=e.laser.end_offset;l.length=e.laser.length;l.width=e.laser.width;l.speed=e.speed;
    l.appear=e.laser.start;l.duration=e.laser.duration;l.disappear=e.laser.stop;l.hit_start=e.laser.hitbox_start;l.hit_end=e.laser.hitbox_stop;
    l.reserved=0;l.phase=l.appear==0;return &l;
}
bool LaserManager::update(const FrameTiming& timing,u32 field_flags,u32 game_flags,LaserCollisionActions& actions){
    if((game_flags&0x1800)||(field_flags&1))return true;
    executor.timing=timing;
    for(u32 index=0;index<capacity;++index){
        auto& l=pool[index];if(!l.active)continue;
        l.end_offset=timing.rate*l.speed+l.end_offset;
        if(l.length<l.end_offset-l.start_offset)l.start_offset=l.end_offset-l.length;
        if(l.start_offset<0)l.start_offset=0;
        Vec2 size{l.end_offset-l.start_offset,l.width*.5f},center{(size.x*.5f+l.start_offset)+l.position.x,l.position.y};
        if(!l.body.loadedSprite)return false;
        l.body.scale.x=l.width/l.body.loadedSprite->widthPx;l.body.scale.y=size.x/l.body.loadedSprite->heightPx;
        l.body.rotation.z=float(add_angle(l.direction+1.5707963705062866f,0));l.body.updateRotation=true;
        const auto collide=[&]{actions.add_laser_collision(center,size,l.position,l.direction);};
        const auto alpha=[&]{i32 amount=truncate((l.time.time*255.f)/float(l.appear));if(amount>255)amount=255;l.body.color1.d3dColor=signed_bits(u32(amount)<<24);};
        if(l.phase==0){
            if(l.flags&1)alpha();
            else {const i32 ramp=l.appear<31?l.appear:30;l.active_width=l.appear-ramp<l.time.current?(l.time.time*l.width)/float(l.appear):1.2f;
                l.body.scale.x=.0625f*l.active_width;size.x=l.active_width*.5f;}
            if(l.time.current>=l.hit_start)collide();
            if(l.time.current>=l.appear){l.time.reset();++l.phase;l.active_width=l.width;}
        }
        if(l.phase==1){
            collide();if(l.time.current>=l.duration){l.time.reset();++l.phase;if(l.disappear==0){l.active=0;continue;}}
        }
        if(l.phase==2){
            if(l.flags&1)alpha();
            else if(l.disappear>0){const float width=l.width-(l.time.time*l.width)/float(l.disappear);l.body.scale.x=.0625f*width;size.x=width*.5f;}
            if(l.time.current<l.hit_end)collide();if(l.time.current>=l.disappear){l.active=0;continue;}
        }
        if(l.start_offset>=640.f)l.active=0;l.time.tick(timing);executor.execute(l.body);
    }
    return !executor.invalid;
}
}
