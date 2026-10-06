#include "PlayerShots.hpp"
#include "AnmVisualState.hpp"
namespace th15 {
bool PlayerShots::hit_rewards(const Vec3& position){
    if(context.power_step==0){error="Hit reward power divisor is zero";return false;}const i32 level=context.power/context.power_step;
    auto spawn=[&](i32 kind,float angle,float speed){if(!item){error="Hit reward item manager unavailable";return false;}return item(kind,position,angle,speed);};
    while(reward_counters[0]>=500){if(level<4){if(!spawn(1,-1.5707964f,2.2f))return false;reward_counters[0]=wrapping_add(reward_counters[0],wrapping_add(wrapping_mul(level,-60),-200));}else{const float angle=float(float(random.signed_unit()*.5235988f)-1.5707964f);if(!spawn(9,angle,2.2f))return false;reward_counters[0]=wrapping_add(reward_counters[0],-50);}}
    while(reward_counters[1]>=100){const float speed=float(float(random.unit()*1.2f)+1),angle=float(float(random.signed_unit()*.5235988f)-1.5707964f);if(!spawn(9,angle,speed))return false;reward_counters[1]=wrapping_add(reward_counters[1],-100);}
    while(reward_counters[2]>=100){if(!spawn(1,-1.5707964f,2.2f))return false;reward_counters[2]=wrapping_add(reward_counters[2],-100);}return true;
}
i32 PlayerShots::ordinary_hit(PlayerShot& shot){
    auto* vm=animations.registry.find(shot.animation);if(!vm){shot.animation=0;error="Hit shot animation unavailable";return -1;}shot.motion.position.z=.1f;vm->pending_interrupt=1;shot.state=2;shot.motion.speed=float(shot.motion.speed*.125f);vm->presentation_motion=true;vm->visual.translation=shot.motion.position;auto* source=damage.find(shot.damage_source);if(!source){error="Hit shot damage source unavailable";return -1;}source->flags&=~1u;shot.damage_source=0;return shot.damage;
}
i32 PlayerShots::contact(PlayerShot& shot,const ShotSpec& spec,const DamageQuery& q){
    const u32 kind=spec.collision_kind;if(kind==0)return ordinary_hit(shot);
    if(kind==2)return laser_hit(shot,q);
    if(kind==3||kind==4){auto* old=damage.find(shot.damage_source);if(!old){error="Exploding shot damage source unavailable";return -1;}const i32 id=damage.circle(shot.motion.position,24,kind==3?1.f:2.f,20,spec.damage);auto* explosion=damage.find(id);if(!explosion){error="Exploding shot allocation failed";return -1;}explosion->interval=4;if(kind==3){explosion->motion.speed=.3f;explosion->motion.angle=-1.5707964f;}if(!animations.interrupt(shot.animation,1)){error=animations.error;return -1;}shot.state=2;shot.motion.speed=kind==3?.3f:2.f;old->flags&=~1u;shot.damage_source=0;(kind==3?old:explosion)->motion=shot.motion;if(sound&&!sound(65,shot.motion.position.x,SoundAction::play)){error="Exploding shot sound unavailable";return -1;}return shot.damage;}
    if(kind==1||kind==5||kind==6){const float variation=float(visual_random.signed_unit()*.34906584f);float angle=normalize_angle(float(shot.motion.angle+variation));if(kind==1)angle=normalize_angle(float(angle+3.1415927f));const u32 handle=animations.create(effect_resource,151,-1,0,shot.motion.position,angle);auto* vm=animations.registry.find(handle);if(!vm){error=animations.error;return -1;}
        if(kind==1){const u32 r=(visual_random.next32()&127)+127,g=(visual_random.next32()&63)+64,b=(visual_random.next32()&63)+64,a=(visual_random.next32()&63)+96;vm->visual.color=b|(g<<8)|(r<<16)|(a<<24);}
        if(kind==6){vm->interpolators.scale.begin(20,0,{1,1},{3,3});const u32 r=(visual_random.next32()&127)+127,g=(visual_random.next32()&127)+64,b=(visual_random.next32()&63)+64;vm->visual.color=(vm->visual.color&0xff000000)|b|(g<<8)|(r<<16);}
        return ordinary_hit(shot);
    }
    error="Unsupported player shot collision callback";return -1;
}
i32 PlayerShots::hit(DamageSource& source,const DamageQuery& query){
    if(source.callback_kind!=1&&source.callback_kind!=2){damage.error=error="Unsupported player damage callback";return -1;}if(source.shot_index<0||source.shot_index>=256){damage.error=error="Hit shot index unavailable";return -1;}auto& shot=shots[u32(source.shot_index)];const auto* spec=specification(shot.specification);if(!spec){damage.error=error="Hit shot specification unavailable";return -1;}
    const Vec3 midpoint={float(float(query.position.x+source.motion.position.x)*.5f),float(float(source.motion.position.y+query.position.y)*.5f),0};if(!hit_rewards(midpoint)){damage.error=error;return -1;}i32 result;if(spec->collision_kind)result=contact(shot,*spec,query);else if(source.callback_kind==1)result=ordinary_hit(shot);else{result=shot.damage;shot.damage=2;}if(!error.empty())damage.error=error;return result;
}
}
