#include "PlayerShots.hpp"
#include "AnmVisualState.hpp"
namespace th17 {
namespace {u32 laser_index(i32 option){return option>=16?u32(option)>>4:u32(option)&15;}}
const ShotSpec* PlayerShots::specification(u32 id)const{const u32 group=id>>8,index=id&255;return group<resource.groups.size()&&index<resource.groups[group].size()?&resource.groups[group][index]:nullptr;}
Vec3 PlayerShots::option_origin(i32 option)const{
    if(option==0)return context.player.position;
    const i32 index=(option>=101?option-96:option)-1;
    if(index<0||index>=12)return {};
    const auto& p=context.player.options[u32(index)].position;return {float(p.x)*.0078125f,float(p.y)*.0078125f,0};
}
bool PlayerShots::slash(PlayerShot& shot,bool reverse){
    auto* initial=damage.find(shot.damage_source);if(!initial){error="Slash damage source unavailable";return false;}initial->flags&=~1u;
    Vec3 position=shot.motion.position;position.x=float(position.x+(reverse?90.f:-90.f));
    constexpr float angle[4]={-1.8325958f,-1.5707964f,-1.5707964f,-1.3089969f};
    constexpr float y[4]={-16,0,16,0};constexpr i32 reduction[4]={-40,0,0,-40};
    for(i32 i=0;i<4;i++){
        const i32 id=damage.rectangle(position,angle[reverse?3-i:i],{50,60},13+i,wrapping_add(shot.damage,reduction[i]));
        auto* source=damage.find(id);if(!source){error="Slash damage allocation failed";return false;}
        source->callback_kind=1;source->shot_index=shot.index;source->damage_limit=10000000;source->interval=1;source->cooldown=i;source->overlap_group=1;
        position.x=float(position.x+(reverse?-60.f:60.f));position.y=float(position.y+y[i]);
    }return true;
}
bool PlayerShots::initialize(PlayerShot& shot,u32 id,const Vec3& position){
    const auto* s=specification(id);if(!s){error="Player shot specification unavailable";return false;}
    const u32 option=u8(s->option)&15,lock=laser_index(s->option);
    if(s->option<0||option>12||lock>=context.laser_power.size()||s->spawn_kind>5){error="Invalid player shot callback or option";return false;}
    shot.state=1;shot.specification=id;shot.age.set(0);shot.damage=s->damage;shot.hitbox=s->hitbox;shot.flags=(shot.flags&~2u)|(context.player.focus?2u:0u);shot.motion={};
    shot.motion.position=option?option_origin(i32(option)):position;
    const float specified=s->angle;shot.motion.speed=s->speed;
    if(specified>=1000&&option){shot.motion.angle=normalize_angle(float(float(float(random.signed_unit()*3.1415927f)/12.f)+context.option_angles[option-1]));shot.motion.speed=float(float(random.signed_unit()*2.f)+s->speed);}
    else shot.motion.angle=normalize_angle(specified>=995&&option?context.option_angles[option-1]:specified);
    if(s->type==2){if(context.power_step==0){error="Player power divisor is zero";return false;}context.laser_power[lock]=context.power/context.power_step;}
    shot.motion.integrate(rate);shot.motion.position.x=float(float(s->offset.x-shot.motion.velocity.x)+shot.motion.position.x);shot.motion.position.y=float(float(s->offset.y-shot.motion.velocity.y)+shot.motion.position.y);
    shot.animation=animations.create(player_resource,i32(s->animation)+5,-1,0);auto* vm=animations.registry.find(shot.animation);if(!vm){error=animations.error;return false;}if(vm->visual.render_flags&0x80){vm->visual.flags|=4;vm->variables.rotation.z=specified;}
    shot.damage_source=damage.rectangle(shot.motion.position,shot.motion.angle,shot.hitbox,9999999,shot.damage);auto* source=damage.find(shot.damage_source);if(!source){error="Player shot damage source unavailable";return false;}source->callback_kind=1;source->damage_limit=10000000;source->shot_index=shot.index;shot.flags|=1;
    switch(s->spawn_kind){
    case 0:break;case 1:shot.target=0;break;
    case 2:case 4:shot.hitbox.x=0;source->interval=1;source->size.x=0;shot.flags&=~1u;if(sound&&!sound(20,context.player.position.x,SoundAction::play)){error="Player laser audio request failed";return false;}break;
    case 3:case 5:if(!slash(shot,s->spawn_kind==5))return false;break;
    }
    if(s->sound>=0&&sound&&!sound(s->sound,shot.motion.position.x,SoundAction::play)){error="Player shot audio request failed";return false;}
    vm->visual.translation=shot.motion.position;return true;
}
i32 PlayerShots::spawn(u32 id,const Vec3& position){
    const auto* s=specification(id);if(!s){error="Player shot specification unavailable";return -1;}const u32 lock=laser_index(s->option);
    if(lock>=context.laser_power.size()){error="Player laser slot unavailable";return -1;}if(s->type==2&&context.laser_power[lock]!=0)return 0;
    for(auto& shot:shots)if(shot.state==0)return initialize(shot,id,position)?0:-1;return 0;
}
bool PlayerShots::fire(i32 shot_frame,i32 sound_frame){
    if(context.power_step==0){error="Player power divisor is zero";return false;}const i32 level=context.power/context.power_step;i32 group=level;if(context.player.focus)group+=1+resource.header.max_power_level;
    if((context.roaring_flags&2)&&!(context.roaring_flags&4)){if(context.roaring_kind==1)group=level+10;else if(context.roaring_kind==3)group=level+15;}
    if(group<0||u32(group)>=resource.groups.size()){error="Player power group unavailable";return false;}
    const auto& specs=resource.groups[u32(group)];for(u32 i=0;i<specs.size();i++){const auto& s=specs[i];const i32 interval=s.sound_interval?s.sound_interval:s.interval,frame=s.sound_interval?sound_frame:shot_frame,delay=s.sound_interval?s.sound_delay:s.delay;if(interval==0){error="Player shot interval is zero";return false;}if(frame%interval==delay){spawn((u32(group)<<8)|i,context.player.position);if(!error.empty())return false;}}return true;
}
}
