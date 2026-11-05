#include "PlayerShots.hpp"
#include "AnmVisualState.hpp"
namespace th15 {
const ShotSpec* PlayerShots::specification(u32 id)const{const u32 group=id>>8,index=id&255;return group<resource.groups.size()&&index<resource.groups[group].size()?&resource.groups[group][index]:nullptr;}
bool PlayerShots::initialize(PlayerShot& shot,u32 id,const Vec3& position){
    const auto* s=specification(id);if(!s){error="Player shot specification unavailable";return false;}if(s->option<0||s->option>8){error="Invalid player shot option";return false;}if(s->spawn_kind>5){error="Unsupported player shot spawn callback";return false;}
    shot.state=1;shot.specification=id;shot.age.set(0);shot.damage=s->damage;shot.hitbox=s->hitbox;shot.flags=(shot.flags&~2u)|(context.player.focus?2u:0u);shot.motion={};
    if(s->option==0)shot.motion.position=position;else{const auto& p=context.player.options[u32(s->option-1)].position;shot.motion.position={float(p.x)*.0078125f,float(p.y)*.0078125f,0};}
    const float specified=s->angle;shot.motion.speed=s->speed;
    if(specified>=1000&&s->option!=0){shot.motion.angle=normalize_angle(float(float(float(random.signed_unit()*3.1415927f)/12.f)+context.option_angles[u32(s->option-1)]));shot.motion.speed=float(float(random.signed_unit()*2.f)+s->speed);}
    else shot.motion.angle=normalize_angle(specified>=995&&s->option!=0?context.option_angles[u32(s->option-1)]:specified);
    if(s->type==2){if(context.power_step==0){error="Player power divisor is zero";return false;}context.laser_power[u32(s->option)]=context.power/context.power_step;}
    shot.motion.integrate(rate);shot.motion.position.x=float(float(s->offset.x-shot.motion.velocity.x)+shot.motion.position.x);shot.motion.position.y=float(float(s->offset.y-shot.motion.velocity.y)+shot.motion.position.y);
    shot.animation=animations.create(player_resource,i32(s->animation)+5,-1,0);auto* vm=animations.registry.find(shot.animation);if(!vm){error=animations.error;return false;}if(vm->visual.render_flags&0x80){vm->visual.flags|=4;vm->variables.rotation.z=specified;}
    shot.damage_source=damage.rectangle(shot.motion.position,shot.motion.angle,shot.hitbox,9999999,shot.damage);auto* source=damage.find(shot.damage_source);if(!source){error="Player shot damage source unavailable";return false;}source->callback_kind=1;source->damage_limit=10000000;source->shot_index=shot.index;shot.flags|=1;
    switch(s->spawn_kind){
    case 0:break;case 1:shot.target=0;break;
    case 2:shot.hitbox.x=0;source->interval=1;source->size.x=0;shot.flags&=~1u;if(sound&&!sound(20,context.player.position.x,SoundAction::play)){error="Player laser audio request failed";return false;}break;
    case 3:shot.flags&=~0x3cu;shot.target=0;break;
    case 4:source->callback_kind=2;break;
    case 5:shot.motion.angle=normalize_angle(float(shot.motion.angle+float(float(random.signed_unit()*.017453292f)*15.f)));break;
    }
    if(s->sound>=0&&sound&&!sound(s->sound,shot.motion.position.x,SoundAction::play)){error="Player shot audio request failed";return false;}
    vm->presentation_motion=true;vm->visual.translation=shot.motion.position;return true;
}
i32 PlayerShots::spawn(u32 id,const Vec3& position){
    const auto* spec=specification(id);if(!spec){error="Player shot specification unavailable";return -1;}if(spec->option<0||spec->option>8){error="Invalid player shot option";return -1;}if(spec->type==2&&context.laser_power[u32(spec->option)]!=0)return 0;
    for(auto& shot:shots)if(shot.state==0)return initialize(shot,id,position)?0:-1;return 0;
}
bool PlayerShots::fire(i32 shot_frame,i32 sound_frame){
    if(context.power_step==0){error="Player power divisor is zero";return false;}i32 group=context.power/context.power_step;if(context.player.focus)group+=1+resource.header.max_power_level;if(group<0||u32(group)>=resource.groups.size()){error="Player power group unavailable";return false;}
    const auto& shots=resource.groups[u32(group)];for(u32 i=0;i<shots.size();i++){const auto& s=shots[i];const i32 interval=s.sound_interval?s.sound_interval:s.interval,frame=s.sound_interval?sound_frame:shot_frame,delay=s.sound_interval?s.sound_delay:s.delay;if(interval==0){error="Player shot interval is zero";return false;}if(frame%interval==delay){spawn((u32(group)<<8)|i,context.player.position);if(!error.empty())return false;}}return true;
}
}
