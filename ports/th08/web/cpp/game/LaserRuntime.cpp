#include "LaserRuntime.hpp"
#include "GameMath.hpp"
namespace th08 {
LaserState* LaserRuntime::create(const BulletEmission& e){
    auto* laser=state.lasers;invalid=false;
    if(state.cancel_frames&&!(e.flags&4))return laser;
    for(;laser!=state.lasers+256;++laser)if(!laser->in_use){
        auto* file=state.animation;const i32 script=wrapping_add(e.sprite,10);
        if(!file||u32(script)>=file->scriptCount||u32(e.color)>=16){invalid=true;return nullptr;}
        static constexpr i32 palette[]{0,1,1,1,1,2,2,2,2,3,3,3,4,4,4,0};
        animation.timing=timing;animation.invalid=false;auto& main=laser->animation[0];main.anmFile=file;main.scriptIndex=i16(script);animation.start(*file,main,file->scripts[script]);
        if(animation.invalid||file->SetSprite(&main,wrapping_add(main.activeSpriteIndex,e.color))!=0){invalid=true;return nullptr;}
        auto& origin=laser->animation[1];origin.Initialize();origin.anmFile=file;
        if(file->SetSprite(&origin,146+palette[e.color])!=0){invalid=true;return nullptr;}origin.blendMode=1;
        laser->position=e.position;laser->color=e.color;laser->in_use=1;laser->angle=e.angle;
        if(e.pattern==0)laser->angle=(bullet_aim_extended(e.position,player)+number(laser->angle)).to_float();
        laser->flags=u16(e.flags);laser->timer.set(0);laser->start_offset=e.laser.start_offset;laser->end_offset=e.laser.end_offset;
        laser->length=e.laser.length;laser->width=e.laser.width;laser->speed=e.speed;
        laser->start=e.laser.start;laser->duration=e.laser.duration;laser->stop=e.laser.stop;laser->hitbox_start=e.laser.hitbox_start;laser->hitbox_stop=e.laser.hitbox_stop;
        laser->unknown599=0;laser->state=laser->start==0?1:0;return laser;
    }
    return laser;
}
bool LaserRuntime::step(LaserState& l){
    auto& vm=l.animation[0];if(!vm.loadedSprite){invalid=true;return false;}
    l.end_offset=(number(timing.rate)*number(l.speed)+number(l.end_offset)).to_float();
    if(number(l.length)<number(l.end_offset)-number(l.start_offset))l.start_offset=Scalar::sub(l.end_offset,l.length);
    if(l.start_offset<0)l.start_offset=0;
    Vec2 size;size.y=Scalar::div(l.width,2);
    size.x=(l.start_offset<=0?number(l.end_offset)-number(l.start_offset):(number(l.end_offset)-number(l.start_offset))*number(.7f)).to_float();
    const Vec2 center{((number(l.end_offset)-number(l.start_offset))/number(2)+number(l.start_offset)+number(l.position.x)).to_float(),l.position.y};
    vm.scale.x=Scalar::div(l.width,vm.loadedSprite->widthPx);
    const float length=Scalar::sub(l.end_offset,l.start_offset);vm.scale.y=Scalar::div(length,vm.loadedSprite->heightPx);
    vm.rotation.z=add_angle(Scalar::add(1.5707963705062866f,l.angle),0);vm.updateRotation=true;
    const auto hit=[&](bool graze){if(actions)actions->collision(center,size,l.position,l.angle,graze);};
    const auto alpha=[&](){i32 a=(l.timer.value()*number(255)/Extended::from_int(l.start)).truncate_int();if(a>255)a=255;vm.color1.d3dColor=signed_bits(u32(a)<<24);};
    if(l.state==0){
        if(l.flags&1)alpha();
        else{
            const i32 tail=l.start>30?30:l.start;
            const float width=wrapping_sub(l.start,tail)<l.timer.current?(l.timer.value()*number(l.width)/Extended::from_int(l.start)).to_float():1.2f;
            l.width2=width;vm.scale.x=Scalar::div(width,16);size.x=Scalar::div(width,2);
        }
        if(l.timer.current>=l.hitbox_start)hit(false);
        if(l.timer.current>=l.start){l.timer.set(0);++l.state;l.width2=l.width;}
    }
    if(l.state==1){
        hit(l.timer.current%20==0);
        if(l.timer.current>=l.duration){l.timer.set(0);++l.state;if(l.stop==0){l.in_use=0;return true;}}
    }
    if(l.state==2){
        if(l.flags&1)alpha();
        else if(l.stop>0){
            const auto width=number(l.width)-l.timer.value()*number(l.width)/Extended::from_int(l.stop);const float stored=width.to_float();
            vm.scale.x=(width/number(16)).to_float();size.x=Scalar::div(stored,2);
        }
        if(l.timer.current<l.hitbox_stop)hit(false);
        if(l.timer.current>=l.stop){l.in_use=0;return true;}
    }
    if(l.start_offset>=640)l.in_use=0;
    l.timer.tick(timing);animation.execute(vm);if(animation.invalid){invalid=true;return false;}return true;
}
bool LaserRuntime::update(){
    invalid=false;animation.invalid=false;animation.timing=timing;
    for(auto& laser:state.lasers)if(laser.in_use&&!step(laser))return false;return true;
}
}
