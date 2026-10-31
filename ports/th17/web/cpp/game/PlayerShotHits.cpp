#include "PlayerShots.hpp"
#include "AnmVisualState.hpp"
namespace th17 {
bool PlayerShots::hit_rewards(const Vec3& position){
    if(!(context.roaring_flags&2))return true;
    while(reward_counter>=100){
        const float speed=float(float(random.unit()*1.2f)+1.f),angle=float(float(random.signed_unit()*.5235988f)-1.5707964f);
        if(!item||!item(9,position,angle,speed)){error="Roaring hit item manager unavailable";return false;}
        reward_counter=wrapping_add(reward_counter,-100);
    }return true;
}
i32 PlayerShots::ordinary_hit(PlayerShot& shot){
    auto* vm=animations.registry.find(shot.animation);if(!vm){shot.animation=0;error="Hit shot animation unavailable";return -1;}
    shot.motion.position.z=.1f;vm->pending_interrupt=1;shot.state=2;shot.motion.speed=float(shot.motion.speed*.125f);vm->visual.translation=shot.motion.position;
    auto* source=damage.find(shot.damage_source);if(!source){error="Hit shot damage source unavailable";return -1;}source->flags&=~1u;shot.damage_source=0;return shot.damage;
}
i32 PlayerShots::contact(PlayerShot& shot,const ShotSpec& spec,const DamageQuery& q){
    const u32 kind=spec.collision_kind;
    auto reward=[&](i32 amount){if(context.roaring_flags&2)reward_counter=wrapping_add(reward_counter,amount);};
    if(kind==0){reward(50);return ordinary_hit(shot);}
    if(kind==2||kind==5)return laser_hit(shot,q,kind==2);
    if(kind==4){reward(70);return -1;}
    if(kind==3||kind==6){
        auto* old=damage.find(shot.damage_source);if(!old){error="Exploding shot damage source unavailable";return -1;}
        const i32 id=damage.circle(shot.motion.position,24,kind==3?1.f:5.f,kind==3?10:16,spec.damage);auto* explosion=damage.find(id);if(!explosion){error="Exploding shot allocation failed";return -1;}
        explosion->interval=kind==3?4:3;explosion->motion.speed=.3f;explosion->motion.angle=-1.5707964f;
        if(!animations.interrupt(shot.animation,1)){error=animations.error;return -1;}shot.state=2;old->flags&=~1u;shot.damage_source=0;shot.motion.speed=.3f;old->motion=shot.motion;
        if(sound&&!sound(65,shot.motion.position.x,SoundAction::play)){error="Exploding shot sound unavailable";return -1;}reward(200);return shot.damage;
    }
    if(kind==1){
        const float variation=float(visual_random.signed_unit()*.34906584f),angle=normalize_angle(float(normalize_angle(float(shot.motion.angle+variation))+3.1415927f));
        const u32 handle=animations.create(effect_resource,149,-1,0,shot.motion.position,angle);auto* vm=animations.registry.find(handle);if(!vm){error=animations.error;return -1;}
        const u32 r=(visual_random.next32()&127)+127,g=(visual_random.next32()&63)+64,b=(visual_random.next32()&63)+64,a=(visual_random.next32()&63)+96;vm->visual.color=b|(g<<8)|(r<<16)|(a<<24);
        reward(50);return ordinary_hit(shot);
    }
    error="Unsupported player shot collision callback";return -1;
}
i32 PlayerShots::hit(DamageSource& source,const DamageQuery& query){
    if(source.callback_kind!=1&&source.callback_kind!=2){damage.error=error="Unsupported player damage callback";return -1;}
    if(source.shot_index<0||source.shot_index>=256){damage.error=error="Hit shot index unavailable";return -1;}
    auto& shot=shots[u32(source.shot_index)];const auto* spec=specification(shot.specification);if(!spec){damage.error=error="Hit shot specification unavailable";return -1;}
    Vec3 position=query.position;if(source.callback_kind==2)position={float(float(query.position.x+source.motion.position.x)*.5f),float(float(source.motion.position.y+query.position.y)*.5f),0};
    if(!hit_rewards(position)){damage.error=error;return -1;}i32 result;
    if(source.callback_kind==2){result=shot.damage;shot.damage=1;}
    else result=contact(shot,*spec,query);
    if(!error.empty())damage.error=error;return result;
}
}
